 /*
 *  Texture2D.mm
 *  TextureManager
 *
 *  Created by bdoig on 11/3/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "Texture2D.h"

#include <GraphicsGL.h>
#include <FileManager.h>
#include <FileHelper.h>
#include <WalaberMath.h>
#include <StringHelper.h>
#include <TextureManager.h>

#include <MemberCallback.h>

#include "./SOIL/SOIL.h"
#include "./webp/webp/decode.h"

// iPhone supports PVR texture format.
#ifdef TARGET_IPHONE
#include "PVRT_Loader.h"
#endif

#include "WALT_Loader.h"

//#include <ProfileTimer.h>

namespace Walaber 
{
    const std::string Texture2D::ERROR_TEXTURE_FILE_NAME("__ERR__");
    const std::string Texture2D::TEXTURE_SCALE_KEY("TextureScale");
    const float       Texture2D::HD_TO_SD_TEXTURE_SCALE( .5f );
    
	// ----------------------------------------------------------------------------------------------------------------
	// ----------------------------------------------------------------------------------------------------------------
	void Texture2D::loadFromFile( CallbackPtr callback, FileManager::PlatformPolicy policy )
	{
		mLoadedCallback = callback;
		mPlatformPolicy = policy;
		
		mTextureFormat = TF_Standard;
		
		initFromFileName();
	}
	
    // ----------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------
    void Texture2D::_getData(CallbackPtr cb)
    {
		if (mTextureFormat == TF_Compressed)
		{
			Logger::printf("Walaber", Logger::SV_ERROR, "cannot get data on compressed texture '%s'!\n", getFileName().c_str());
			return;
		}
		
        if (!imageBuffer_.use_count())
        {   
			printf("Texture2D::_getData() - reading data from disk\n");
			
			mGetDataCallback = cb;
			
			
            PropertyList plist;
			plist.setValueForKey( "init_tex", Property(0) );
			plist.setValueForKey( "keep_buffer", Property(1) );
			
			
			MemberCallbackPtr<Texture2D>::type memberCallback( new MemberCallback< Texture2D >( this, &Texture2D::_fileLoaded ) );
			CallbackPtr callback = static_pointer_cast<Callback>(memberCallback);
			
			FileManager::getInstancePtr()->readFile( fileName_, callback, plist, mPlatformPolicy );
        }
		else
		{
			printf("Texture2D::_getData() - returning EXISTING data\n");
			
            if(cb)
            {
                Texture::TextureDataCallbackParameters params(this, imageBuffer_ );
                cb->invoke( &params );
            }
		}
    }
    
    // ----------------------------------------------------------------------------------------------------------------
    void Texture2D::setData(buffer_type_ptr data)
    {
        if (!imageBuffer_.use_count())
        {
            //int w, h, pot_w, pot_h;
            //imageBuffer_ = loadImageData(w, h, pot_w, pot_h);
        }
		
		if (mTextureFormat == TF_Compressed)
		{
			Logger::printf("Walaber", Logger::SV_ERROR, "Cannot set data on compressed texture '%s'!\n", getFileName().c_str());
			return;
		}
        
        assert( data->size() == imageBuffer_->size() );
        
        memcpy( &(imageBuffer_->at(0)), &(data->at(0)), data->size() * sizeof(Texture::buffer_value) );
        
        // update openGL texture!
        if (name_ != NO_TEXTURE)
        {
            GraphicsGL::bindTexture( name_ );
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, &(imageBuffer_->at(0)) );
            
        }
    }
    
    // ----------------------------------------------------------------------------------------------------------------
    void Texture2D::setData(buffer_type_ptr source, int s_x, int s_y, int d_x, int d_y, int width, int height, int sourceWidth)
    {
        if (!imageBuffer_.use_count())
        {
            //int w, h, pot_w, pot_h;
            //imageBuffer_ = loadImageData(w,h, pot_w, pot_h);
        }
		
		if (mTextureFormat == TF_Compressed)
		{
			Logger::printf("Walaber", Logger::SV_ERROR, "Cannot set data on compressed texture '%s'!\n", getFileName().c_str());
			return;
		}
        
        // memcpy based on rows...
        assert( (d_x + width) < width_ );
        
        for (int r = 0; r < height; r++)
        {
            int sourceOffset = ((s_y + r) * sourceWidth) + (s_x);
            int destOffset = ((d_y + r) * width_) + (d_x);
            
            memcpy( &(imageBuffer_->at(destOffset)), &(source->at(sourceOffset)), width * sizeof(Texture::buffer_value) );
        }
        
        
        // update openGL texture!
        if (name_ != NO_TEXTURE)
        {
            GraphicsGL::bindTexture( name_ );
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, &(imageBuffer_->at(0)) );
            
        }
        
    }

    // ----------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------
    void Texture2D::_applySettings()
    {
#ifdef _DEBUG
        printf("_applySettings...\n");
#endif
        
        if (name_ != NO_TEXTURE)
        {
            GraphicsGL::bindTexture(name_);
            
            
            // set texture parameters here
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, settings_.getMinFilter());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, settings_.getMagFilter());
            
            //GraphicsGL::setMipMapFilters(settings_.minFilter, settings_.magFilter, true);
            
            
            //GraphicsGL::setTextureWrap(settings_.wrapU, settings_.wrapV, true);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, settings_.getWrapU());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, settings_.getWrapV());
            
            
#ifdef _DEBUG
            settings_.debugLog();
            printf("\n");
#endif
        }
    }

	
    // ----------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------
	void Texture2D::initFromFileName() 
    {
		PropertyList plist;
		plist.setValueForKey( "init_tex", Property(1) );
		plist.setValueForKey( "keep_buffer", Property(0) );
		
		MemberCallbackPtr<Texture2D>::type memberCallback( new MemberCallback< Texture2D >( this, &Texture2D::_fileLoaded ) );
		CallbackPtr callback = static_pointer_cast<Callback>(memberCallback);
		
		FileManager::getInstancePtr()->readFile( fileName_, callback, plist, mPlatformPolicy );
	}
	
	// ----------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------
	void Texture2D::_fileLoaded( void* params ) //const std::string& path, char* buffer, size_t length, const std::string& actualPath, const std::string& handlerName, const PropertyList& userData )
	{
		FileManager::ReadFileCallbackParameters* rcp = (FileManager::ReadFileCallbackParameters*)params;
		
		Logger::printf("Walaber", Logger::SV_DEBUG, "texture file loaded (%s)\n", fileName_.c_str());
		
		if (rcp->result != ResultCode_OK)
		{
            PlatformManager* platformManager = PlatformManager::getInstancePtr();
            
            if ( platformManager->getDisplayType() == PlatformManager::PDT_StandardDefinition &&
                 platformManager->getFormFactor() == PlatformManager::PFF_Handheld &&
                 !rcp->userData.keyExists( TEXTURE_SCALE_KEY ) )
            {
                PropertyList plist = rcp->userData;
                plist.setValueForKey(TEXTURE_SCALE_KEY, HD_TO_SD_TEXTURE_SCALE);
                std::string hdFileName = PlatformManager::getInstancePtr()->appendSpecificPlatformTagToFilename(rcp->path, PlatformManager::PSN_HD);
                
                MemberCallbackPtr<Texture2D>::type memberCallback( new MemberCallback< Texture2D >( this, &Texture2D::_fileLoaded ) );
                CallbackPtr callback = static_pointer_cast<Callback>(memberCallback);
                
                FileManager::getInstancePtr()->readFile( hdFileName, callback, plist, FileManager::PP_NoAbstraction );
            }
            
			_failedToLoad( rcp->path );
			
			return;
		}
		
        // We may need a webp config so get a pointer ready
        WebPDecoderConfig* config = 0;
        
		std::string extension = StringHelper::getExtension(rcp->path);
		
        if (extension == ".waltex")
		{
            mTextureFormat = TF_Walaber;
        }
#ifdef TARGET_IPHONE
		else if (extension == ".pvr")
        {
			mTextureFormat = TF_Compressed;
        }
#endif
		else if ( extension == ".webp" )
        {
            mTextureFormat = TF_WebP;
		}
		
        if( rcp->userData.keyExists( TEXTURE_SCALE_KEY ) && mTextureFormat != TF_WebP )
        {
            // WEBP is the only format that supports scaling a texture on load
            assert( 0 );
            _failedToLoad( rcp->path );
        }
		
		bool init_tex = (rcp->userData["init_tex"].asInt() == 1);
		bool keep_buffer = (rcp->userData["keep_buffer"].asInt() == 1);
		bool expanded_to_POT = false;
		
		if (((settings_.getColorspace() != TextureInMemoryColorspace_32_R8_G8_B8_A8) || (mTextureFormat == TF_Compressed)) && (keep_buffer))
		{
			printf("ERROR!  cannot keep buffer on image with colorspace that is not R8G8B8A8!\n");
			keep_buffer = false;
		}
		
		
		int pot_w, pot_h, native_format;
		TextureBuffer textureBuffer;
		GLenum compressed_format = 0;
		unsigned int compressed_length = 0;
        
		if (mTextureFormat == TF_Standard)
		{
			textureBuffer.imgBuffer = SOIL_load_image_from_memory( (unsigned char*)rcp->buffer, (int)rcp->length, &textureBuffer.width, &textureBuffer.height, &native_format, SOIL_LOAD_RGBA);
            textureBuffer.colorSpace = TextureInMemoryColorspace_16_R4_G4_B4_A4;
            
			// remove raw file data from memory now.
			if (rcp->buffer)
				delete[] rcp->buffer;
		}
		else if (mTextureFormat == TF_Compressed)
		{
#ifdef TARGET_IPHONE
			// load PVR Texture!
			textureBuffer.imgBuffer = loadPVR((unsigned char*)rcp->buffer, rcp->length, textureBuffer.width, textureBuffer.height, compressed_format, compressed_length);
#endif
		}
		else if (mTextureFormat == TF_Walaber)
		{
			TextureInMemoryColorspace colSpace = TextureInMemoryColorspace_32_R8_G8_B8_A8;
			
			textureBuffer.imgBuffer = loadWALT((unsigned char*)rcp->buffer, rcp->length, textureBuffer.width, textureBuffer.height, colSpace);
			
			settings_.setColorspace(colSpace);
		}
        else if (mTextureFormat == TF_WebP )
        {
            // TODO: Make webp options works
            
            //TextureInMemoryColorspace colSpace = settings_.getColorspace();
            //bool premultiplyAlpha = settings_.getPremultiplyAlpha();
            
            // TODO: Use RAII such as a scoped ptr
            config = new WebPDecoderConfig;
            bool bRet = WebPInitDecoderConfig( config );
            assert (bRet);

            /*            
            if( colSpace != TextureInMemoryColorspace_32_R8_G8_B8_A8 )
            {
                int i = 0;
                i += 1;
            }
            if( colSpace == TextureInMemoryColorspace_16_R4_G4_B4_A4 )
            {
                if( premultiplyAlpha )
                {
                    config.output.colorspace = MODE_rgbA_4444;
                    textureBuffer.alphaState = ProcessImageAlpha_PremultiplyAlpha;
                    textureBuffer.colorSpace = TextureInMemoryColorspace_16_R4_G4_B4_A4;
                }
                else
                {
                    config.output.colorspace = MODE_RGBA_4444;
                    textureBuffer.colorSpace = TextureInMemoryColorspace_16_R4_G4_B4_A4;
                }
            }
            else if ( colSpace == TextureInMemoryColorspace_16_R5_G6_B5 )
            {
                config.output.colorspace = MODE_RGB_565;
                textureBuffer.colorSpace = TextureInMemoryColorspace_16_R5_G6_B5;
            }
            else if ( colSpace == TextureInMemoryColorspace_32_R8_G8_B8_A8 )
            {
                if( premultiplyAlpha )
                {
                    config.output.colorspace = MODE_rgbA;
                    textureBuffer.alphaState = ProcessImageAlpha_PremultiplyAlpha;
                }
                else
                {*/
                    config->output.colorspace = MODE_RGBA;
                //}
                
                textureBuffer.colorSpace = TextureInMemoryColorspace_16_R4_G4_B4_A4;
            //}
            
            if ( rcp->userData.keyExists( TEXTURE_SCALE_KEY ) )
            {
                int iRet = WebPGetInfo( const_cast<const u_int8_t*>
                                            ( reinterpret_cast<u_int8_t*>( rcp->buffer ) ),
                                        rcp->length,
                                        &textureBuffer.width, &textureBuffer.height );
                assert ( iRet != 0 );
                
                float scale = rcp->userData.getValueForKey( TEXTURE_SCALE_KEY )->asFloat();
                config->options.use_scaling = 1;
                config->options.scaled_height = (int)(scale * textureBuffer.width);
                config->options.scaled_width = (int)(scale * textureBuffer.height);
                
                textureBuffer.width = config->options.scaled_width;
                textureBuffer.height = config->options.scaled_height;
            }
            
            
            
            VP8StatusCode vpRet=  WebPDecode( const_cast<const u_int8_t*>
                                ( reinterpret_cast<u_int8_t*>( rcp->buffer ) ),
                               rcp->length,
                               config );
            assert ( vpRet == VP8_STATUS_OK );
            
            textureBuffer.imgBuffer = config->output.u.RGBA.rgba;
            
            if( !textureBuffer.width && !textureBuffer.height )
            {
                textureBuffer.width = config->input.width;
                textureBuffer.height = config->input.height;
            }
                        
            //textureBuffer.imgBuffer = WebPDecodeRGBA( const_cast<const u_int8_t*>
            //                                        ( reinterpret_cast<u_int8_t*>( rcp->buffer ) ),
            //                                         rcp->length,
            //                                         &textureBuffer.width,
            //                                         &textureBuffer.height);
            
            // remove raw file data from memory now.
            if (rcp->buffer)
                delete[] rcp->buffer;            
        }

		if (textureBuffer.imgBuffer == NULL)
        {
			if (mTextureFormat == TF_Standard)
            {
				Logger::printf("Walaber", Logger::SV_ERROR, "error loading image!  reason: %s\n  --> filename:%s\n", SOIL_last_result(), rcp->path.c_str());
            }
			else if (mTextureFormat == TF_Compressed)
            {
				Logger::printf("Walaber", Logger::SV_ERROR, "error loading compressed image! --> filename:%s\n", rcp->path.c_str());
            }
            else if ( mTextureFormat == TF_WebP )
            {
                Logger::printf("Walaber", Logger::SV_ERROR, "error loading webp image! --> filename:%s\n", rcp->path.c_str());
            }
			else
            {
				Logger::printf("Walaber", Logger::SV_ERROR, "error loading walaber raw texture --> filename:%s\n", rcp->path.c_str());
            }
        }
        else
		{
			pot_w = nextPOT(textureBuffer.width);
			pot_h = nextPOT(textureBuffer.height);
        
#ifdef _DEBUG
			printf("loaded image, size[%d x %d]  pot[%d x %d]  file:%s\n", textureBuffer.width, textureBuffer.height, pot_w, pot_h, rcp->path.c_str());
#endif
			
			// various image processing, only OK on standard textures...
			if ( mTextureFormat == TF_Standard )
			{
				// processing + colorspace conversion handled here!
				unsigned char* originalData = textureBuffer.imgBuffer;
				
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
				
				if (textureBuffer.imgBuffer != originalData)
				{
					// clear up the SOIL memory
					expanded_to_POT = true;                   
                    SOIL_free_image_data(originalData);
				}
			}
            else if( mTextureFormat == TF_WebP )
			{
				// processing + colorspace conversion handled here!
				unsigned char* originalData = textureBuffer.imgBuffer;
				
				ProcessImageAlpha pia = ProcessImageAlpha_AlphaUntouched;
				ProcessImageOptions pio = ProcessImageOptions_None;
				ProcessImageBufferOptions pibo = ProcessImageBufferOptions_AllowInPlace;
                
                // only manually process the file if webp wasn't able to do the processing for us
				if ( settings_.getPremultiplyAlpha() )
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
				
				if (textureBuffer.imgBuffer != originalData)
				{
					// clear up the SOIL memory
					expanded_to_POT = true;
                    
                    // webp lib uses c and malloc, so we need to free this memory
                    free( originalData );
				}
			}
			
			
			
			// we have data now, as a POT texture in the proper colorspace... do someting with it!
			if (init_tex)
			{
				assert( name_ == NO_TEXTURE );
				
				glGenTextures(1, &name_);
				
#ifdef _DEBUG
				printf("generated texture: %d\n", (int)name_);
#endif
				
                GraphicsGL::invalidateTextures();
				_applySettings();
				
				if ( ( mTextureFormat == TF_Standard) || (mTextureFormat == TF_Walaber || mTextureFormat == TF_WebP ) )
				{
					
					switch (settings_.getColorspace())
					{
							
						default:
						case TextureInMemoryColorspace_32_R8_G8_B8_A8:
							glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pot_w, pot_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, &textureBuffer.imgBuffer[0] );
							break;
							
						case TextureInMemoryColorspace_16_R5_G6_B5:
							glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pot_w, pot_h, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, &textureBuffer.imgBuffer[0] );
							break;
							
						case TextureInMemoryColorspace_16_R5_G5_B5_A1:
							glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pot_w, pot_h, 0, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, &textureBuffer.imgBuffer[0] );
							break;
							
						case TextureInMemoryColorspace_16_R4_G4_B4_A4:
							glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pot_w, pot_h, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, &textureBuffer.imgBuffer[0] );
							break;
					}
				}
				else if (mTextureFormat == TF_Compressed)
				{
					glCompressedTexImage2D(GL_TEXTURE_2D, 0, compressed_format, pot_w, pot_h, 0, compressed_length, &textureBuffer.imgBuffer[0]);
					
					GLenum err = glGetError();
					if (err != GL_NO_ERROR)
					{
						Logger::printf("Walaber", Logger::SV_ERROR, "Error creating gl texture from compressed data for file '%s' --> 0x%04X\n", getFileName().c_str(), err);
					}
				}
				
				
				size_ = Vector2(textureBuffer.width,textureBuffer.height);
				width_ = pot_w;
				height_ = pot_h;
				maxS_ = (float)textureBuffer.width / (float)pot_w;
				maxT_ = (float)textureBuffer.height / (float)pot_h;
				
				if (mLoadedCallback)
                {
                    Texture::TextureLoadedCallbackParameters textureParameters( fileName_, ResultCode_OK );
                    mLoadedCallback->invoke( &textureParameters );
                }
				
			}
			
			
			if (keep_buffer && (mTextureFormat == TF_Standard))
			{
				imageBuffer_ = bufferFromImageData(textureBuffer.imgBuffer, pot_w, pot_h);
				
				if (mGetDataCallback)
                {
                    Texture::TextureDataCallbackParameters textureParameters( this, imageBuffer_ );
                    mGetDataCallback->invoke( &textureParameters );
                }
			}

			
			// clean up the memory for the image now!
			if (mTextureFormat == TF_Standard)
			{
				if (!expanded_to_POT)
				{
					// our buffer was the SOIL buffer all-along, free it now.
					SOIL_free_image_data( textureBuffer.imgBuffer );
				}
				else
				{
					// we had to pad to a POT, so the data is our own buffer, free it now.
					delete[] textureBuffer.imgBuffer;
				}
			}
            else if (mTextureFormat == TF_WebP)
			{
				if (!expanded_to_POT)
				{
					// our buffer was the SOIL buffer all-along, free it now.
					free( textureBuffer.imgBuffer );
				}
				else
				{
					// we had to pad to a POT, so the data is our own buffer, free it now.
					delete[] textureBuffer.imgBuffer;
				}
                
                if( config )
                {
                    delete config;
                }
			}
			else if ((mTextureFormat == TF_Compressed) || 
					 (mTextureFormat == TF_Walaber))
			{
				// compressed data was original file straight from the disk.
				if (rcp->buffer)
					delete[] rcp->buffer;
			}
		}    
	}
	
	// ----------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------
	void Texture2D::reload(CallbackPtr ptr, FileManager::PlatformPolicy policy)
	{
		printf("Texture2D::reload( %s )\n", fileName_.c_str());
		
		if (name_ != NO_TEXTURE)
		{
			glDeleteTextures(1, &name_);
		}
		
		name_ = NO_TEXTURE;
		
		loadFromFile(ptr, policy);
	}

	// ----------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------
	Texture::buffer_type_ptr Texture2D::bufferFromImageData( unsigned char* data, const int w, const int h )
	{
		Texture::buffer_type_ptr retBuff = Texture::buffer_type_ptr(new buffer_type(w*h));
        
		memcpy( &(retBuff->at(0)), &data[0], w * h * 4 );
		
		return retBuff;
	}
    
    // Texture failed to load so re
    void Texture2D::_failedToLoad( const std::string& path )
    {
        // file missing, replace with "no texture" contents!
        Logger::printf("Walaber", Logger::SV_DEBUG, "FAILED TO FIND TEXTURE: %s\n", path.c_str());
        
        ProgrammaticTexture2DPtr errTex = TextureManager::getManager().getErrorTexture();
        
        width_ = errTex->getWidth();
        height_ = errTex->getHeight();
        
        minS_ = minT_ = 0.0f;
        maxS_ = maxT_ = 1.0f;
        
        fileName_ = ERROR_TEXTURE_FILE_NAME;
        
        name_ = errTex->getName();
        
        
        // call loaded callback now...
        if (mLoadedCallback)
        {
            TextureLoadedCallbackParameters loadedParams( path, ResultCode_ErrorFileNotFound );
            
            mLoadedCallback->invoke((void*)&loadedParams);
        }
    }

	
    
    // ----------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------
	Texture2DPtr createTexture2D(const std::string& fileName) {
		Texture2DPtr newTexture(new Texture2D(fileName));
		return newTexture;
	}
    
    // ----------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------
	Texture2DPtr createTexture2D(const std::string& fileName, TextureSettings ts) {
		Texture2DPtr newTexture(new Texture2D(fileName, ts));
		return newTexture;
	}
}


