#include "Texture.h"

#include "StringHelper.h"

namespace Walaber
{
    
	// ---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]
	// ---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]
	unsigned char* processImageBufferAndMakePOT( TextureBuffer& textureBuffer,
												TextureInMemoryColorspace convertToColorSpace,
												ProcessImageBufferOptions bufferOption,
												ProcessImageOptions processOption,
												ProcessImageAlpha processAlpha,
												float alphaClip,
                                                Color clipColor,
                                                float clipTolerance,
												float colorFilterR,
												float colorFilterG,
												float colorFilterB,
												float colorFilterA,
												float colorFilterBrightness)
	{
		unsigned char* ret = NULL;
		
        int imgWidth = textureBuffer.width;
        int imgHeight = textureBuffer.height;
		int srcWidth = imgWidth;
		int srcHeight = imgHeight;
		
		// make width and height a POT.
		imgWidth = Walaber::nextPOT(imgWidth);
		imgHeight = Walaber::nextPOT(imgHeight);
        
        int clipColorLenSqr = clipColor.R * clipColor.R + clipColor.G * clipColor.G + clipColor.B * clipColor.B;
		int clipColorDiffLengSqr = clipTolerance * clipTolerance * 3;
        
		if (convertToColorSpace == TextureInMemoryColorspace_32_R8_G8_B8_A8)
		{
			// 32-bit color case... are we allowed to use this image in-place?  (only if we don't expane to POT, and caller says it's OK)
			if ((srcWidth == imgWidth) && (srcHeight == imgHeight) && (bufferOption == ProcessImageBufferOptions_AllowInPlace))
			{
				ret = textureBuffer.imgBuffer;
                textureBuffer.width = imgWidth;
                textureBuffer.height = imgHeight;
			}
			
			
			// do we need to allocate a new buffer?  if so, do it now...
			if (ret == NULL)
			{
				ret = new unsigned char[ imgWidth * imgHeight * 4 ];
				memset(ret, 0, imgWidth * imgHeight * 4);
			}
			
            // we don't need to do anything if the alpha doesn't need to be changed, or if the
            // alpha has a;ready been changed
			if ( ( processAlpha == ProcessImageAlpha_AlphaUntouched ||
                     processAlpha == textureBuffer.alphaState ) &&
                  processOption == ProcessImageOptions_None )
			{
				// no processing necessary...
				if (ret != textureBuffer.imgBuffer)
				{
					for (int y = 0; y < srcHeight; y++)
					{
						memcpy(&ret[ ((y * imgWidth) * 4) ], &textureBuffer.imgBuffer[ ((y * srcWidth) * 4) ], srcWidth * 4);
					}
				}
			}
			else
			{
				// have to loop over each pixel to premultiply / apply filtering...
				for (int y = 0; y < srcHeight; y++)
				{
					for (int x = 0; x < srcWidth; x++)
					{
						// get the original pixel data...
						int i = ((y * srcWidth) + x) * 4;
						unsigned char r = (textureBuffer.imgBuffer[i + 0]);
						unsigned char g = (textureBuffer.imgBuffer[i + 1]);
						unsigned char b = (textureBuffer.imgBuffer[i + 2]);
						unsigned char a = (textureBuffer.imgBuffer[i + 3]);
						
						
						// greyscale!
						if (processOption == ProcessImageOptions_ConvertGreyscale)
						{
							unsigned char grey = (unsigned char)((r * 0.3f * colorFilterBrightness) + (g * 0.59f * colorFilterBrightness) + (b * 0.11f * colorFilterBrightness));
							grey = clampi(grey, 0, 255);
                            
							r = g = b = grey;
						}
						// color filter
						else if (processOption == ProcessImageOptions_ApplyColorFiler)
						{
							r = clampi((unsigned char)(r * colorFilterR * colorFilterBrightness), 0, 255);
							g = clampi((unsigned char)(g * colorFilterG * colorFilterBrightness), 0, 255);
                            b = clampi((unsigned char)(b * colorFilterB * colorFilterBrightness), 0, 255);
							a = clampi((unsigned char)(a * colorFilterA * colorFilterBrightness), 0, 255);
						}						
						
						// premultiply alpha!
						if (a < 255)
                        {
                            float f_a = ((float)a / 255.0f);
                            int colorLengthSqr = r*r + g*g + b*b;
                            
                            if( f_a < alphaClip &&
                               ( abs(colorLengthSqr - clipColorLenSqr) <= clipColorDiffLengSqr ) )
                            {
                                r *= 0;
                                g *= 0;
                                b *= 0;
                                a = 0;
                            }
                            else
                            {
                                r *= f_a;
                                g *= f_a;
                                b *= f_a;
                            }
                        }
                        
						ret[ (((y * imgWidth) + x) * 4) + 0 ] = r;
						ret[ (((y * imgWidth) + x) * 4) + 1 ] = g;
						ret[ (((y * imgWidth) + x) * 4) + 2 ] = b;
						ret[ (((y * imgWidth) + x) * 4) + 3 ] = a;
					}
				}
			}
			
		}
		else
		{
			// 16-bit cases...
            
            if ( textureBuffer.colorSpace != TextureInMemoryColorspace_32_R8_G8_B8_A8 &&
                 srcWidth == imgWidth &&
                 srcHeight == imgHeight &&                 
                 bufferOption == ProcessImageBufferOptions_AllowInPlace  )
            {
                ret = textureBuffer.imgBuffer;
			}
            else
            {
                ret = new unsigned char[ imgWidth * imgHeight * 2 ];
                memset(ret, 0, imgWidth * imgHeight * 2);
            }
            
            unsigned short* ret_as_short = (unsigned short*)ret;
            
            // no processing necessary...
            if (ret != textureBuffer.imgBuffer && textureBuffer.colorSpace != TextureInMemoryColorspace_32_R8_G8_B8_A8)
            {
                for (int y = 0; y < srcHeight; y++)
                {
                    memcpy(&ret[ ((y * imgWidth) * 2) ], &textureBuffer.imgBuffer[ ((y * srcWidth) * 2) ], srcWidth * 2);
                }
                
            }
			
			for (int y = 0; y < srcHeight; y++)
			{
				for (int x = 0; x < srcWidth; x++)
				{
                    unsigned char r = 0;
                    unsigned char g = 0;
                    unsigned char b = 0;
                    unsigned char a = 0;
                    
					// get the original pixel data...
                    if ( textureBuffer.colorSpace == TextureInMemoryColorspace_32_R8_G8_B8_A8 )
                    {
                        int i = ((y * srcWidth) + x) * 4;
                        r = (textureBuffer.imgBuffer[i + 0]);
                        g = (textureBuffer.imgBuffer[i + 1]);
                        b = (textureBuffer.imgBuffer[i + 2]);
                        a = (textureBuffer.imgBuffer[i + 3]);
					}
                    else
                    {
                        int i = ((y * srcWidth) + x);
                        unsigned short color = ((unsigned short*)textureBuffer.imgBuffer)[i];
                        
                        if ( textureBuffer.colorSpace == TextureInMemoryColorspace_16_R5_G6_B5 )
                        {
                            r = (unsigned char)color >> 11;
                            g = (unsigned char)(( color << 5 ) >> 10 );
                            b = (unsigned char)(( color << 11 ) >> 11 );
                            a = 1;
                        }
                        else if ( textureBuffer.colorSpace == TextureInMemoryColorspace_16_R5_G5_B5_A1 )
                        {
                            r = (unsigned char)color >> 11;
                            g = (unsigned char)(( color << 5 ) >> 11 );
                            b = (unsigned char)(( color << 10 ) >> 11 );
                            a = color & 1;
                        }
                        else
                        {
                            r = (unsigned char)color >> 12;
                            g = (unsigned char)(( color << 4 ) >> 12 );
                            b = (unsigned char)(( color << 8 ) >> 12 );
                            a = (unsigned char)(( color << 12 ) >> 12 );
                        }
                    }
					
					// greyscale!
					if (processOption == ProcessImageOptions_ConvertGreyscale)
					{
						unsigned char grey = (unsigned char)((r * 0.3f * colorFilterBrightness) + (g * 0.59f * colorFilterBrightness) + (b * 0.11f * colorFilterBrightness));
						grey = clampi(grey, 0, 255);
						
						r = g = b = grey;
					}
					// color filter
					else if (processOption == ProcessImageOptions_ApplyColorFiler)
					{
						r = clampi((unsigned char)(r * colorFilterR * colorFilterBrightness), 0, 255);
						g = clampi((unsigned char)(g * colorFilterG * colorFilterBrightness), 0, 255);
						b = clampi((unsigned char)(b * colorFilterB * colorFilterBrightness), 0, 255);
						a = clampi((unsigned char)(a * colorFilterA * colorFilterBrightness), 0, 255);
					}
					
					
					if (processAlpha == ProcessImageAlpha_PremultiplyAlpha)
					{
                        if (a < 255)
                        {
                            float f_a = ((float)a / 255.0f);
                            int colorLengthSqr = r*r + g*g + b*b;                            
                            
                            if( f_a < alphaClip &&
                               ( abs(colorLengthSqr - clipColorLenSqr) <= clipColorDiffLengSqr ) )
                            {
                                r *= 0;
                                g *= 0;
                                b *= 0;
                                a = 0;
                            }
                            else
                            {
                                r *= f_a;
                                g *= f_a;
                                b *= f_a;
                            }
                        }
					}
					
					if (convertToColorSpace == TextureInMemoryColorspace_16_R5_G6_B5)
					{
						ret_as_short[ ((y * imgWidth) + x) ] = (((r>>3) << 11) |
																((g>>2) << 5) |
																((b>>3)));
					}
					else if (convertToColorSpace == TextureInMemoryColorspace_16_R5_G5_B5_A1)
					{
						ret_as_short[ ((y * imgWidth) + x) ] = (((r>>3) << 11) |
																((g>>3) << 6) |
																((b>>3) << 1) |
																((a > 128) ? 1 : 0));
					}
					else if (convertToColorSpace == TextureInMemoryColorspace_16_R4_G4_B4_A4)
					{
						ret_as_short[ ((y * imgWidth) + x) ] = (((r>>4) << 12) |
																((g>>4) << 8) |
																((b>>4) << 4) |
																((a>>4) << 0));
					}
				}
			}
		}
		
		
		return ret;
	}
	
	
	
	// --------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==
	// --------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==
	TextureInMemoryColorspace strToTextureInMemoryColorspace( const std::string& s )
	{
		Walaber::TextureInMemoryColorspace ret = Walaber::TextureInMemoryColorspace_32_R8_G8_B8_A8;
		
		std::string l = StringHelper::toLower(s);
		
		if (l == "rgb565")
		{
			ret = Walaber::TextureInMemoryColorspace_16_R5_G6_B5;
		}
		else if (l == "rgba5551")
		{
			ret = Walaber::TextureInMemoryColorspace_16_R5_G5_B5_A1;
		}
		else if (l == "rgba4444")
		{
			ret = Walaber::TextureInMemoryColorspace_16_R4_G4_B4_A4;
		}
		
		return ret;
	}
	
	
	// --------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==
	// --------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==
	std::string textureInMemoryColorspaceToStr( TextureInMemoryColorspace cs )
	{
		std::string ret = "rgba8888";
		
		if (cs == Walaber::TextureInMemoryColorspace_16_R5_G6_B5)
			ret = "rgb565";
		else if (cs == Walaber::TextureInMemoryColorspace_16_R5_G5_B5_A1)
			ret = "rgba5551";
		else if (cs == Walaber::TextureInMemoryColorspace_16_R4_G4_B4_A4)
			ret = "rgba4444";
		
		return ret;
	}
	
	// --------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==
	// --------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==
	unsigned int textureInMemoryColorspaceBytesPerPixel( TextureInMemoryColorspace cs )
	{
		switch (cs)
		{
			case TextureInMemoryColorspace_16_R4_G4_B4_A4:
			case TextureInMemoryColorspace_16_R5_G5_B5_A1:
			case TextureInMemoryColorspace_16_R5_G6_B5:
				return 2;
				break;
				
			case TextureInMemoryColorspace_32_R8_G8_B8_A8:
			default:
				return 4;
				break;
		}
		
		return 0;
	}
	
}


