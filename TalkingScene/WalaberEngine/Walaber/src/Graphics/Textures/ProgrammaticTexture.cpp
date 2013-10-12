#include "ProgrammaticTexture.h"

#include <GraphicsGL.h>
#include <FileManager.h>
#include <FileHelper.h>
#include <WalaberMath.h>
#include <StringHelper.h>

#include <MemberCallback.h>

namespace Walaber
{
    
	// ----------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------
	void ProgrammaticTexture2D::initWithSize( unsigned int w, unsigned int h, TextureInMemoryColorspace fmt )
	{
		width_ = w;
		height_ = h;
		
		int pot_w = Walaber::nextPOT(w);
		int pot_h = Walaber::nextPOT(h);
		
		maxS_ = (float)w / (float)pot_w;
		maxT_ = (float)h / (float)pot_h;
		
		glGenTextures(1, &name_);
		
		GraphicsGL::bindTexture( name_ );
		
		Logger::printf("Walaber", Logger::SV_DEBUG, "ProgrammaticTexture2D::initWithSize() - tex: %d", name_ );
		
		// settings...
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		switch (fmt)
		{
			case TextureInMemoryColorspace_16_R4_G4_B4_A4:
			default:
				// 16-bit render buffer... (r4 g4 b4 a4)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,  pot_w, pot_h, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, NULL);
				break;
					
			case TextureInMemoryColorspace_16_R5_G5_B5_A1:
				// 16-bit render buffer... (r5, g5, b5, a1)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,  pot_w, pot_h, 0, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, NULL);
				break;
					
			case TextureInMemoryColorspace_16_R5_G6_B5:
				// 16-bit render buffer... (r5, g6, b5)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pot_w, pot_h, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);
					break;
					
			case TextureInMemoryColorspace_32_R8_G8_B8_A8:
				// 32-bit render buffer...
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pot_w, pot_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
				break;
		}

	}
	
	// ----------------------------------------------------------------------------------------------------------------
	// ----------------------------------------------------------------------------------------------------------------
	void ProgrammaticTexture2D::setData(buffer_type_ptr data)
	{
		if (data->size() != (width_ * height_))
		{
			Logger::printf("Walaber", Logger::SV_ERROR, "ProgrammaticTexture::setData ERROR - source data size does not match texture size %dx%d", width_, height_);
			return;
		}
		
		// convert and go!
		// processing + colorspace conversion handled here!
        TextureBuffer textureBuffer;
		textureBuffer.imgBuffer = (unsigned char*)&data->at(0);
		unsigned char* originalData = textureBuffer.imgBuffer ;
		
		ProcessImageAlpha pia = ProcessImageAlpha_AlphaUntouched;
		ProcessImageOptions pio = ProcessImageOptions_None;
		ProcessImageBufferOptions pibo = ProcessImageBufferOptions_AllowInPlace;
		
		if (settings_.getPremultiplyAlpha())
        {
			pia = ProcessImageAlpha_PremultiplyAlpha;
        }
		
		if (settings_.getConvertToGreyscale())
        {
			pio = ProcessImageOptions_ConvertGreyscale;
        }
		else if (settings_.getApplyColorFilter())
        {
			pio = ProcessImageOptions_ApplyColorFiler;
        }
		
		textureBuffer.imgBuffer =
            Walaber::processImageBufferAndMakePOT( textureBuffer,
                                                   settings_.getColorspace(),
                                                   pibo,
                                                   pio,
                                                   pia,														 
                                                   settings_.getAlphaClip(),
                                                   settings_.getClipColor(),
                                                   settings_.getClipTolerance(),
                                                   settings_.getColorFilterR(),
                                                   settings_.getColorFilterG(),
                                                   settings_.getColorFilterB(),
                                                   settings_.getColorFilterA(),
                                                   settings_.getBrightness());
		
		// set the data now...
		int pot_w = Walaber::nextPOT(width_);
		int pot_h = Walaber::nextPOT(height_);
		
		switch (settings_.getColorspace())
		{
			default:
			case TextureInMemoryColorspace_32_R8_G8_B8_A8:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pot_w, pot_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, &textureBuffer.imgBuffer [0] );
				break;
				
			case TextureInMemoryColorspace_16_R5_G6_B5:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pot_w, pot_h, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, &textureBuffer.imgBuffer [0] );
				break;
				
			case TextureInMemoryColorspace_16_R5_G5_B5_A1:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pot_w, pot_h, 0, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, &textureBuffer.imgBuffer [0] );
				break;
				
			case TextureInMemoryColorspace_16_R4_G4_B4_A4:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pot_w, pot_h, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, &textureBuffer.imgBuffer [0] );
				break;
		}
		
		
		if (textureBuffer.imgBuffer != originalData)
		{
			// delete the allocated memory
			delete[] textureBuffer.imgBuffer ;
		}
	}
	
	// ----------------------------------------------------------------------------------------------------------------
	// ----------------------------------------------------------------------------------------------------------------
	void ProgrammaticTexture2D::setData(buffer_type_ptr source, int s_x, int s_y, int d_x, int d_y, int width, int height, int sourceWidth)
	{
		Walaber::Logger::printf("Walaber", Logger::SV_ERROR, "ProgrammaticTexture::setData override for specific region not currently supported!\n");
	}
	
	
	// ----------------------------------------------------------------------------------------------------------------
	// ----------------------------------------------------------------------------------------------------------------
	void ProgrammaticTexture2D::_applySettings()
	{
		if (name_ != NO_TEXTURE)
        {
            GraphicsGL::bindTexture(name_);
            
            // set texture parameters here
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, settings_.getMinFilter());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, settings_.getMagFilter());
            
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, settings_.getWrapU());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, settings_.getWrapV());
            
        }
	}
	
	
	// ----------------------------------------------------------------------------------------------------------------
	// ----------------------------------------------------------------------------------------------------------------
	void ProgrammaticTexture2D::reload( CallbackPtr ptr, FileManager::PlatformPolicy policy )
	{
		if (name_ == NO_TEXTURE)
		{
			// rebuild!
			initWithSize(width_, height_, settings_.getColorspace());
			setData(imageBuffer_);
		}
		
		if (ptr)
		{
			TextureLoadedCallbackParameters params( fileName_, ResultCode_OK );
			ptr->invoke((void*)&params);
		}
	}

	
	
	// ----------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------
	ProgrammaticTexture2DPtr createProgrammaticTexture2D(const std::string& name, unsigned int width, unsigned int height, TextureInMemoryColorspace fmt)
	{
		ProgrammaticTexture2DPtr newTexture( new ProgrammaticTexture2D(name, width, height, fmt) );
		return newTexture;
	}
	
	// ----------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------
	ProgrammaticTexture2DPtr createProgrammaticTexture2D(const std::string& name, unsigned int width, unsigned int height, TextureInMemoryColorspace fmt, Texture::buffer_type_ptr data)
	{
		ProgrammaticTexture2DPtr newTexture( new ProgrammaticTexture2D(name, width, height, fmt, data) );
		return newTexture;
	}
    
    // ----------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------
    ProgrammaticTexture2DPtr createProgrammaticSolidColorTexture2D(const std::string& name, unsigned int width, unsigned int height, Color color, TextureInMemoryColorspace fmt)
    {
        ProgrammaticTexture2DPtr texture = createProgrammaticTexture2D(name, width, height, fmt);
        Texture::buffer_type_ptr data = texture->create_empty_buffer();
        
        // set the data...
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                (*data)[ y * width + x ] = color;
            }
        }
        
        texture->setData(data);
        
        return texture;
    }
	
}


