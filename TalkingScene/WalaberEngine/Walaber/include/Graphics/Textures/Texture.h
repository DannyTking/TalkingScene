#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <CommonGL.h>

#include <macros.h>
#include <Vector2.h>
#include <Rect.h>
#include <Color.h>
#include <vector>
#include <FileManager.h>
#include <Callback.h>
#include <Utilities.h>

namespace Walaber 
{
	// ---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]
	// ---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]
	const unsigned int NO_TEXTURE = -1;
    
    // ---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]
	// ---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]
	struct TextureData 
	{
		TextureData() : width(0), height(0), maxS(1.0f), maxT(1.0f), minS(0.0f), minT(0.0f), offset(0,0), flags(0) {}
		
		GLuint width;
		GLuint height;
		GLfloat maxS;
		GLfloat maxT;
		GLfloat minS;
		GLfloat minT;
        Vector2 offset;
        int flags;
	};
	
    
	// ---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]
	// ---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]
	enum TextureInMemoryColorspace
	{
		TextureInMemoryColorspace_32_R8_G8_B8_A8,
		TextureInMemoryColorspace_16_R5_G6_B5,
		TextureInMemoryColorspace_16_R5_G5_B5_A1,
		TextureInMemoryColorspace_16_R4_G4_B4_A4
	};
	
	// ---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]
	TextureInMemoryColorspace strToTextureInMemoryColorspace( const std::string& s );
	std::string textureInMemoryColorspaceToStr( TextureInMemoryColorspace cs );
	unsigned int textureInMemoryColorspaceBytesPerPixel( TextureInMemoryColorspace cs );
	
	// ---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]
	// ---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]
	enum ProcessImageAlpha
	{
		ProcessImageAlpha_PremultiplyAlpha,
		ProcessImageAlpha_AlphaUntouched
	};
	
	enum ProcessImageOptions
	{
		ProcessImageOptions_None,
		ProcessImageOptions_ConvertGreyscale,
		ProcessImageOptions_ApplyColorFiler
	};
	
	enum ProcessImageBufferOptions
	{
		ProcessImageBufferOptions_AllowInPlace,
		ProcessImageBufferOptions_ForceNewBuffer
	};
    
    struct TextureBuffer
    {
        TextureBuffer() :
        imgBuffer ( 0 ),
        colorSpace ( TextureInMemoryColorspace_32_R8_G8_B8_A8 ),
        alphaState ( ProcessImageAlpha_AlphaUntouched ),
        width ( 0 ),
        height ( 0 )
        {}
        
        unsigned char* imgBuffer;
        TextureInMemoryColorspace colorSpace;
        ProcessImageAlpha alphaState;
        int width;
        int height;
    };
	
	// given a 32-bit RGBA color buffer, expand to a "power to two" in size, and convert to the
	// desired colorspace, along with potentially premultiplying the alpha channel, and also scaling
    // the alpha values as well.  in the 32-bit case, optinally apply filtering "in-place" in the
    // original buffer.  in this case, return pointer == fullColorSrcImage.
	unsigned char* processImageBufferAndMakePOT( TextureBuffer& textureBuffer,
												TextureInMemoryColorspace convertToColorSpace,
												ProcessImageBufferOptions bufferOption = ProcessImageBufferOptions_AllowInPlace,
												ProcessImageOptions processOption =
                                                    ProcessImageOptions_None,
												ProcessImageAlpha processAlpha =
                                                    ProcessImageAlpha_PremultiplyAlpha,
												float alphaClip = 0.0f,
                                                Color clipColor = Color::White,
                                                float clipTolerance = 0.0f,
												float colorFilterR = 1.0f,
												float colorFilterG = 1.0f,
												float colorFilterB = 1.0f,
												float colorFilterA = 1.0f,
												float colorFilterBrightness = 1.0f);
	
	
	
    // ---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]
	// ---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]
	enum TextureSettingsShortcut
    {
        TextureSettingsShortcut_PremultiplyAlpha = 1,
        TextureSettingsShortcut_FilteringSmooth = 2,
        TextureSettingsShortcut_WrappingClamp = 4,
		TextureSettingsShortcut_ConvertToGreyscale = 8,
        TextureSettingsShortcut_ApplyColorFilter = 16,
        
        TextureSettingsShortcut_Default =
            TextureSettingsShortcut_FilteringSmooth |
            TextureSettingsShortcut_WrappingClamp,
        TextureSettingsShortcut_DefaultRepeat = TextureSettingsShortcut_FilteringSmooth,
        TextureSettingsShortcut_Blocky = TextureSettingsShortcut_WrappingClamp,
        TextureSettingsShortcut_BlockyRepeat = 0
    };
    
	
	
    // ---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]
	// ---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]
	enum TextureType
    {
        TT_Standard,
        TT_Subtexture,
		TT_RenderTexture,
		TT_Programmatic
    };
    
	
	
    // ---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]
	// ---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]
	struct TextureSettings
    {
        TextureSettings( unsigned int tss = TextureSettingsShortcut_Default )
        {
            premultiplyAlpha = (tss & TextureSettingsShortcut_PremultiplyAlpha);
			convertToGreyscale = (tss & TextureSettingsShortcut_ConvertToGreyscale);
            applyColorFilter = (tss & TextureSettingsShortcut_ApplyColorFilter);
            
            if (tss & TextureSettingsShortcut_FilteringSmooth)
                minFilter = magFilter = GL_LINEAR;
            else
                minFilter = magFilter = GL_NEAREST;
            
            if (tss & TextureSettingsShortcut_WrappingClamp)
                wrapU = wrapV = GL_CLAMP_TO_EDGE;
            else
                wrapU = wrapV = GL_REPEAT;
			
			
			colorspace = TextureInMemoryColorspace_32_R8_G8_B8_A8;
			stayInMemory = false;
			alphaClip = 0.0f;
            clipColor = Color::Black;
            clipTolerance = 0.0f;
			colorFilterR = colorFilterG = colorFilterB = colorFilterA = 1.0f;
			brightness = 1.0f;
			
			_dirtyReload = _dirtyReapply = false;
        }
		
		void setStayInMemory( bool stay ) { stayInMemory = stay; }
		bool getStayInMemory() const { return stayInMemory; }
		
		void setPremultiplyAlpha( bool premult )
        {
            if (premult != premultiplyAlpha)
            {
                premultiplyAlpha = premult;
                _dirtyReload = true;
            }
        }
        
		bool getPremultiplyAlpha() const
        {
            return premultiplyAlpha;
        }
		
		void setConvertToGreyscale( bool convert )
        {
            if (convert != convertToGreyscale)
            {
                convertToGreyscale = convert;
                _dirtyReload = true;
            }
        }
        
		bool getConvertToGreyscale() const
        {
            return convertToGreyscale;
        }
		
		void setAlphaClip( float s )
        {
            alphaClip = s;
            _dirtyReload = true;
        }
        
        void setClipColor( const Color color )
        {
            clipColor = color;
            _dirtyReload = true;
        }
        
        void setClipTolerance( const float pClipTolerance )
        {
            clipTolerance = pClipTolerance;
            _dirtyReload = true;
        }
        
        float getClipTolerance() const
        {
            return clipTolerance;
        }
        
        
		float getAlphaClip() const
        {
            return alphaClip;
        }
		
        Color getClipColor() const
        {
            return clipColor;
        }
        
		void setColorspace( TextureInMemoryColorspace space )
        {
            if (colorspace != space)
            {
                colorspace = space;
                _dirtyReload = true;
            }
        }
        
		TextureInMemoryColorspace getColorspace() const
        {
            return colorspace;
        }
		
		void setFilters( GLuint min, GLuint mag )
        {
            minFilter = min;
            magFilter = mag;
            _dirtyReapply = true;
        }
        
		void setMinFilter( GLuint min )
        {
            minFilter = min;
            _dirtyReapply = true;
        }
        
		void setMagFilter( GLuint mag )
        {
            magFilter = mag;
            _dirtyReapply = true;
        }
        
		GLuint getMinFilter() const
        {
            return minFilter;
        }
        
		GLuint getMagFilter() const
        {
            return magFilter;
        }
        
		void setWrap( GLuint u, GLuint v )
        {
            wrapU = u;
            wrapV = v;
            _dirtyReapply = true;
        }
        
		void setWrapU( GLuint u )
        {
            wrapU = u;
            _dirtyReapply = true;
        }
        
		void setWrapV( GLuint v )
        {
            wrapV = v;
            _dirtyReapply = true;
        }
		
		GLuint getWrapU() const
        {
            return wrapU;
        }
        
		GLuint getWrapV() const
        {
            return wrapV;
        }
		
		
		bool getShouldReload() const
        {
            return _dirtyReload;
        }
        
		bool getShouldReapply() const
        {
            return _dirtyReapply;
        }
		
		void setShouldReload( bool reload )
        {
            _dirtyReload = reload;
        }
        
		void setShouldReapply( bool reapply )
        {
            _dirtyReapply = reapply;
        }
		
		void reloaded()
        {
            _dirtyReload = false;
        }
        
		void reapplied()
        {
            _dirtyReapply = false;
        }
        
		void setUncompressedExt(const std::string& ext)
        {
            uncompressedExt = ext;
        }
        
		std::string getUncompressedExt()
        {
            return uncompressedExt;
        }
		
        void debugLog()
        {
            Logger::printf("Walaber", Logger::SV_DEBUG, "pA[%d] grey[%d] alphaClip[%f] outline[%d %d %d] greyTolerance[%d] minF[%d] magF[%d]"
                           " wrapU[%d] wrapV[%d] colorspace[%d] stayInMem[%d]",
				   premultiplyAlpha, convertToGreyscale, alphaClip, clipColor.R, clipColor.G, clipColor.B, clipTolerance, minFilter,
                           magFilter, wrapU, wrapV, colorspace, stayInMemory);
        }
        
        //get/set functions for customized grey scale option
        void setApplyColorFilter( bool apply )
        {
            if (apply != applyColorFilter)
            {
                applyColorFilter = apply;
                _dirtyReload = true;
            }
        }
        
        bool getApplyColorFilter() const
        {
            return applyColorFilter;
        }
        
        void setColorFilterR( float filterR )
        {
            colorFilterR = filterR;
        }
        
        float getColorFilterR() const
        {
            return colorFilterR;
        }
        
        void setColorFilterG( float filterG )
        {
            colorFilterG = filterG;
        }
        
        float getColorFilterG() const
        {
            return colorFilterG;
        }
        
        void setColorFilterB( float filterB )
        {
            colorFilterB = filterB;
        }
        
        float getColorFilterB() const
        {
            return colorFilterB;
        }
        
        void setColorFilterA( float filterA )
        {
            colorFilterA = filterA;
        }
        
        float getColorFilterA() const
        {
            return colorFilterA;
        }
        
        void setBrightness( float bright )
        {
            brightness = bright;
        }
        
        float getBrightness() const
        {
            return brightness;
        }
        
        void setColorFilter( float filterR, float filterG, float filterB, float filterA, float bright)
        {
            colorFilterR = filterR;
            colorFilterG = filterG;
            colorFilterB = filterB;
            colorFilterA = filterA;
            brightness = bright;
        }
        
        
		
	private:
		bool						_dirtyReload;	// something has changed and texture should be reloaded
		bool						_dirtyReapply;	// something has changed and openGL needs to be notified of new settings.
		bool						stayInMemory;
        bool						premultiplyAlpha;
		bool						convertToGreyscale;
		float						alphaClip;
        Color                       clipColor;
        float                       clipTolerance;
		TextureInMemoryColorspace	colorspace;
        GLuint						minFilter;
        GLuint						magFilter;
        GLuint						wrapU;
        GLuint						wrapV;
		std::string					uncompressedExt;
        
        //customize grey scale option
        bool                        applyColorFilter;
        float                       colorFilterR;
        float                       colorFilterG;
        float                       colorFilterB;
        float                       colorFilterA;
        float                       brightness;
    };
    
    
	// ---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]
	// ---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]
	class Texture : Utilities::noncopyable
    {
	public:
        
        enum TextureFlags
        {
            TF_SubtextureHasBeenRotatedNinetyDegrees = 1,
            TF_SubtextureHasOffset = 2,
        };
	
        // Types 
		typedef std::vector<Walaber::Color>         buffer_type;
		typedef buffer_type::value_type             buffer_value;
		typedef SharedPtr<buffer_type>              buffer_type_ptr;
		
        struct TextureLoadedCallbackParameters
        {
            std::string path;
            ResultCode result;
            
            TextureLoadedCallbackParameters(const std::string& ppath, const ResultCode presult):
            path(ppath),
            result(presult)
            {}
        };
		
        struct TextureDataCallbackParameters
        {
            Texture* texture;
            buffer_type_ptr buffer;
            
            TextureDataCallbackParameters(Texture* tex, buffer_type_ptr buf):
            texture(tex),
            buffer(buf)
            {}
        };
		
	public:
		Texture(const std::string& fileName, TextureType type = TT_Standard) 
		: name_(NO_TEXTURE), 
        settings_(TextureSettingsShortcut_Default),
		fileName_(fileName), 
		width_(0), 
		height_(0),
		maxS_(0), 
		maxT_(0),
		minS_(0), 
		minT_(0),
		size_(Vector2::Zero),
        offset_(Vector2::Zero),
        flags_(0),
        type_(type)
		{ 
		}
        
        Texture(const std::string& fileName, TextureSettings ts, TextureType type = TT_Standard) 
		: name_(NO_TEXTURE), 
        settings_(ts),
		fileName_(fileName), 
		width_(0), 
		height_(0),
		maxS_(0), 
		maxT_(0),
		minS_(0), 
		minT_(0),
		size_(Vector2::Zero),
        offset_(Vector2::Zero),
        flags_(0),
        type_(type)
		{ 
		}
		
		virtual ~Texture() {
		}
        
        void setSettings( TextureSettings ts )
        {
            settings_ = ts;
            _applySettings();
        }
        
		const TextureSettings& getSettings() const
        {
            return settings_;
        }
		
		std::string getFileName() const
        {
            return fileName_;
        }
        
		GLuint getName() const
        {
            return name_;
        }
        
		GLuint getWidth() const
        {
            return width_;
        }
        
		GLuint getHeight() const
        {
            return height_;
        }
        
		Vector2 getTextureSize() const
        {
            return Vector2(width_, height_);
        }
        
		Vector2 getMaxUV() const
        {
            return Vector2(maxS_, maxT_);
        }
        
		Vector2 getMinUV() const
        {
            return Vector2(minS_, minT_);
        }
        
		Rect getTextureRect() const
        {
            return Rect(minS_,minT_, maxS_-minS_, maxT_-minT_);
        }
        
		Vector2 getContentSize() const
        {
            return size_;
        }
        
        Vector2 getContentOffset() const
        {
            return offset_;
        }
        
        int getTextureFlags()  const
        {
            return flags_;
        }
        
        TextureType getTextureType() const
        {
            return type_;
        }        
		
		void getData(CallbackPtr callback)
        {
            _getData( callback );
        }
		
		virtual void setData(buffer_type_ptr data)
        { }
        
		virtual void setData(buffer_type_ptr source, int s_x, int s_y, int d_x, int d_y, int width, int height, int sourceWidth)
        { }
        
		virtual void writeToFile(std::string filePath)
        { }
		
		buffer_type_ptr create_empty_buffer(void) 
        {
			Vector2 size = getTextureSize();
            unsigned long bufferSize = static_cast<unsigned long>( size.X * size.Y );
			buffer_type_ptr temp = buffer_type_ptr(new buffer_type(bufferSize));
			return temp;
		}

		virtual void reload( CallbackPtr ptr, FileManager::PlatformPolicy policy = FileManager::PP_AllowAbstraction )
        { }

		virtual void invalidate()
        {
            name_ = NO_TEXTURE;
        }

		bool isValid() const
        {
            return name_ != NO_TEXTURE;
        }
		
		unsigned int getMemoryInBytes()
		{
			return width_ * height_ * textureInMemoryColorspaceBytesPerPixel(settings_.getColorspace());
		}
        
    protected:
		std::string					fileName_;
        TextureSettings             settings_;
		GLuint						name_;
		GLuint						width_;
		GLuint						height_;
		GLfloat						maxS_;
		GLfloat						maxT_;
		GLfloat						minS_;
		GLfloat						minT_;
		Vector2						size_;
        Vector2                     offset_;
        int                         flags_;
		buffer_type_ptr				imageBuffer_;
        TextureType                 type_;
		
		virtual void _getData( CallbackPtr callback ) 
        {
            if(callback)
            {
                TextureDataCallbackParameters params( this, imageBuffer_ ); 
                callback->invoke( &params );
            }
        }
        virtual void _applySettings() { }
		
	};
	
	
	MAKE_SHARED_PTR(Texture);
    
    
}
#endif
