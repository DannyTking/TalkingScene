#include "RenderTexture.h"

#include <GraphicsGL.h>
#include <FileManager.h>
#include <FileHelper.h>
#include <WalaberMath.h>
#include <StringHelper.h>

#include <MemberCallback.h>

#include "./SOIL/SOIL.h"

namespace Walaber 
{
    
	// ----------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------
	void RenderTexture2D::initWithSize( unsigned int w, unsigned int h, TextureInMemoryColorspace fmt )
	{
		// FRAMEBUFFER 1 - this is the main buffer for rendering the fluids.
		width_ = Walaber::nextPOT(w);
		height_ = Walaber::nextPOT(h);
		
		size_ = Vector2(w, h);
		offset_ = Vector2::Zero;
		
		minS_ = 0.0f;
		minT_ = 0.0f;
		
		maxS_ = (float)w / (float)width_;
		maxT_ = (float)h / (float)height_;
				
		GLint prevBuffer = GraphicsGL::getBoundFramebuffer();
				
		GL_GEN_FRAMEBUFFERS(1, &frameBuffer_);
        
        Walaber::GraphicsGL::bindFramebuffer( frameBuffer_);
		
		glGenTextures(1, &name_);
		
		GraphicsGL::bindTexture( name_ );		
		
		Logger::printf("Walaber", Logger::SV_DEBUG, "RenderTexture2D::initWithSize() - tex: %d framebuffer: %d", name_, frameBuffer_); 
		
		// settings...
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		bool ok = false;
		
		while (!ok)
		{
			
			switch (fmt)
			{
				case TextureInMemoryColorspace_16_R4_G4_B4_A4:
				default:
					// 16-bit render buffer... (r4 g4 b4 a4)
                    mFormat = GL_RGBA;
                    mType = GL_UNSIGNED_SHORT_4_4_4_4;
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,  width_, height_, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, NULL);	
					break;
					
				case TextureInMemoryColorspace_16_R5_G5_B5_A1:
					// 16-bit render buffer... (r5, g5, b5, a1)
                    mFormat = GL_RGBA;
                    mType = GL_UNSIGNED_SHORT_5_5_5_1;
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,  width_, height_, 0, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, NULL);
					break;
					
				case TextureInMemoryColorspace_16_R5_G6_B5:
					// 16-bit render buffer... (r5, g6, b5)
                    mFormat = GL_RGB;
                    mType = GL_UNSIGNED_SHORT_5_6_5;
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);
					break;
				case TextureInMemoryColorspace_32_R8_G8_B8_A8:
					// 32-bit render buffer...
                    mFormat = GL_RGBA;
                    mType = GL_UNSIGNED_BYTE;
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
					break;
					
			}
			
			
			GL_FRAMEBUFFER_TEXTURE2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, name_, 0);
			
			
			GLenum status = GL_CHECK_FRAMEBUFFER_STATUS(GL_FRAMEBUFFER);
			
			if(status != GL_FRAMEBUFFER_COMPLETE) 
			{
				Logger::printf("Walaber", Logger::SV_ERROR, "failed to make complete framebuffer object %x", status);
				
				// if we were NOT a 32-bit format, try again with that format, because some devices don't support 16-bit framebuffers.
				if (fmt != TextureInMemoryColorspace_32_R8_G8_B8_A8)
				{
					fmt = TextureInMemoryColorspace_32_R8_G8_B8_A8;
				}
				else
				{
					break;
				}
			}
			else 
			{
				ok = true;
				break;
			}
		}
		
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		// go back to old frame buffer...
		Walaber::GraphicsGL::bindFramebuffer( prevBuffer );
	}
	
    // ----------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------

	/**
	 * Similar to {@link initWithSize}
	 * - In fact it's nearly identical except it also initializes the texture with the supplied pixel data.
	 *   If pixelData is NULL the result should be the same
	 *
	 */
	void RenderTexture2D::initWithPixelData( unsigned int w, unsigned int h, TextureInMemoryColorspace fmt, unsigned char *pixelData )
    {
        width_ = Walaber::nextPOT(w);
        height_ = Walaber::nextPOT(h);

        size_ = Vector2(w, h);
        offset_ = Vector2::Zero;

        minS_ = 0.0f;
        minT_ = 0.0f;

        maxS_ = (float)w / (float)width_;
        maxT_ = (float)h / (float)height_;

        GLint prevBuffer = GraphicsGL::getBoundFramebuffer();

        GL_GEN_FRAMEBUFFERS(1, &frameBuffer_);

        Walaber::GraphicsGL::bindFramebuffer( frameBuffer_);

        glGenTextures(1, &name_);

        GraphicsGL::bindTexture( name_ );

        Logger::printf("Walaber", Logger::SV_DEBUG, "RenderTexture2D::initWithPixelData() - tex: %d framebuffer: %d", name_, frameBuffer_);

        // settings...
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        bool ok = false;
        while (!ok)
        {
            switch (fmt)
            {
                case TextureInMemoryColorspace_16_R4_G4_B4_A4:
                default:
                    // 16-bit render buffer... (r4 g4 b4 a4)
                    mFormat = GL_RGBA;
                    mType = GL_UNSIGNED_SHORT_4_4_4_4;
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,  width_, height_, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, pixelData);
                    break;

                case TextureInMemoryColorspace_16_R5_G5_B5_A1:
                    // 16-bit render buffer... (r5, g5, b5, a1)
                    mFormat = GL_RGBA;
                    mType = GL_UNSIGNED_SHORT_5_5_5_1;
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,  width_, height_, 0, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, pixelData);
                    break;

                case TextureInMemoryColorspace_16_R5_G6_B5:
                    // 16-bit render buffer... (r5, g6, b5)
                    mFormat = GL_RGB;
                    mType = GL_UNSIGNED_SHORT_5_6_5;
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, pixelData);
                    break;

                case TextureInMemoryColorspace_32_R8_G8_B8_A8:
                    // 32-bit render buffer...
                    mFormat = GL_RGBA;
                    mType = GL_UNSIGNED_BYTE;
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
                    break;
            }


            GL_FRAMEBUFFER_TEXTURE2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, name_, 0);


            GLenum status = GL_CHECK_FRAMEBUFFER_STATUS(GL_FRAMEBUFFER);
            if(status != GL_FRAMEBUFFER_COMPLETE)
            {
                Logger::printf("Walaber", Logger::SV_ERROR, "failed to make complete framebuffer object %x", status);

                // if we were NOT a 32-bit format, try again with that format, because some devices don't support 16-bit framebuffers.
                if (fmt != TextureInMemoryColorspace_32_R8_G8_B8_A8)
                {
                    fmt = TextureInMemoryColorspace_32_R8_G8_B8_A8;
                }
                else
                {
                    break;
                }
            }
            else
            {
                ok = true;
                break;
            }
        }

        // Ha! ... we just set the color data! (hopefully)
        // So don't clear buffer ... unless pixelData is NULL
        if ( pixelData == NULL ) {
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);
        }

        // go back to old frame buffer...
        Walaber::GraphicsGL::bindFramebuffer( prevBuffer );
    }

	// ----------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------
	void RenderTexture2D::bind()
	{
		// store previously bound buffer
		prevBuffer_ = GraphicsGL::getBoundFramebuffer();
				
		// store viewport settings...
		GraphicsGL::getViewport(prevViewportX_, prevViewportY_, prevViewportW_, prevViewportH_);
		
		// store matrices...
		glGetFloatv(GL_MODELVIEW_MATRIX, prevModelView_);
		glGetFloatv(GL_PROJECTION_MATRIX, prevProjection_);

		Logger::printf("Walaber", Logger::SV_VERBOSE, "RenderTexture2D::bind() - prev buffer [%d] viewport[%d %d %d %d]\n",
			prevBuffer_, prevViewportX_, prevViewportY_, prevViewportW_, prevViewportH_);

		Logger::printf("Walaber", Logger::SV_VERBOSE, "RenderTexture2D::bind() - this buffer [%d]\n",
			frameBuffer_);
		
		
		// now bind US
		Walaber::GraphicsGL::bindFramebuffer( frameBuffer_ );
		
		Logger::printf("Walaber", Logger::SV_VERBOSE, "RenderTexture2D::bind() - check for error\n");
		GraphicsGL::checkForError();
	}
	
	// ----------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------
	void RenderTexture2D::unbind()
	{
		// bind back to prev. buffer
		Logger::printf("Walaber", Logger::SV_VERBOSE, "RenderTexture2D::unbind() - prev buffer [%d]\n",
					   prevBuffer_);
		Walaber::GraphicsGL::bindFramebuffer( prevBuffer_ );
		
		// restore viewport.
		GraphicsGL::setViewport(prevViewportX_, prevViewportY_, prevViewportW_, prevViewportH_);

        glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glLoadMatrixf(prevProjection_);
        
		// restore matrices...
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glLoadMatrixf(prevModelView_);
	}
	
	// ----------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------
	void RenderTexture2D::setViewport()
	{
		GraphicsGL::setViewport(0, 0, (int)size_.X, (int)size_.Y);		
	}
	
	// ----------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------
    void RenderTexture2D::_applySettings()
    {
		Walaber::Logger::printf("WMW", Logger::SV_DEBUG, "_applySettings...\n");
        
        if (name_ != NO_TEXTURE)
        {
            GraphicsGL::bindTexture(name_);
            
            // set texture parameters here
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, settings_.getMinFilter());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, settings_.getMagFilter());
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, settings_.getWrapU());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, settings_.getWrapV());
            
            
            settings_.debugLog();
            Walaber::Logger::printf("WMW", Logger::SV_DEBUG, "\n");
        }
    }
	
	
    
    // ----------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------
	RenderTexture2DPtr createRenderTexture2D(unsigned int width, unsigned int height, TextureInMemoryColorspace fmt)
	{
		RenderTexture2DPtr newTexture(new RenderTexture2D(width, height, fmt));
		return newTexture;
	}
	
	// ----------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------
	RenderTexture2DPtr createRenderTexture2D(unsigned int width, unsigned int height, TextureSettings settings, TextureInMemoryColorspace fmt)
	{
		RenderTexture2DPtr newTexture(new RenderTexture2D(width, height, settings, fmt));
		return newTexture;
	}

    // ----------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------
	/**
	 * Overload of createRenderTexture2D which accepts pixel data to use to initialize the render texture's contents
	 *
	 * NOTE: Format of the pixelData needs to match the width/height/format arguments
	 */
    RenderTexture2DPtr createRenderTexture2D(unsigned int rttWidth, unsigned int rttHeight, TextureInMemoryColorspace fmt, unsigned char *pixelData)
    {
        RenderTexture2DPtr newTexture(new RenderTexture2D(rttWidth, rttHeight, pixelData, fmt ));
        return newTexture;
    }

    // ----------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------


    /**
     * Allocates a new buffer and reads this texture's framebuffer contents into --CPU-- memory.
     *
     * Pixel format may differ from requested and could be 16 or 32-bit.
     *
     * @NOTE: It's up the the caller to delete the allocated buffer when done with it.
     */
    unsigned char *RenderTexture2D::createScreenGrab(unsigned int *outWidth,
            unsigned int *outHeight, GLenum *outGlPixelFormat)
    {
        // populate caller args so it knows what we are handing back to them
        *outWidth = width_;
        *outHeight = height_;
        *outGlPixelFormat = mType;

        const int bpp = ((mType == GL_UNSIGNED_BYTE) ? 4 : 2); // bytes per pixel
        const int bufSize = width_ * height_ * bpp;

        // allocate the buffer for the user
        // - this will reside in CPU memory not GPU (ie, not a GL texture)
        unsigned char *cpuBuffer = new unsigned char[bufSize];

        if ( cpuBuffer == NULL ) {
            return NULL;
        }

        // We don't know what the caller wants to do with the results or, more significantly, when it's calling us.
        // But we should make sure we read a clean image before reading and returning result.
        glFinish();

        // Bind ourself as the active framebuffer and read in the data
        this->bind();
        glReadPixels(0, 0, width_, height_, mFormat, mType, cpuBuffer);
        this->unbind();

        return cpuBuffer;
    }

}


