#ifndef _TEXTURE2D_H_
#define _TEXTURE2D_H_

#include "PropertyList.h"
#include "Texture.h"
#include <assert.h>

#include <vector>


#include "ProgrammaticTexture.h"


namespace Walaber 
{	
	// ----------------------------------------------------------------------------------------------------------------
	class Texture2D : public Texture 
    {
	public:
		
        // ----------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------
		Texture2D(const std::string& fileName)
		: Texture(fileName),
		mGetDataCallback(),
		mLoadedCallback(),
		mPlatformPolicy(FileManager::PP_AllowAbstraction),
		mTextureFormat(TF_Standard)
        {
		}
        
        // ----------------------------------------------------------------------------------------------------------------
		Texture2D(const std::string& fileName, TextureSettings ts)
		: Texture(fileName, ts),
		mGetDataCallback(),
		mLoadedCallback(),
		mPlatformPolicy(FileManager::PP_AllowAbstraction),
		mTextureFormat(TF_Standard)
        {
		}
		
        // ----------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------
		~Texture2D() 
        {
			if ((name_ != NO_TEXTURE) && (fileName_ != ERROR_TEXTURE_FILE_NAME))
			{
				glDeleteTextures(1, &name_);
				imageBuffer_.reset();
			}
		}
		
        // ----------------------------------------------------------------------------------------------------------------
		void loadFromFile( CallbackPtr callback, FileManager::PlatformPolicy policy = FileManager::PP_AllowAbstraction );
		
        // ----------------------------------------------------------------------------------------------------------------
		void setData(buffer_type_ptr data);
		
        // ----------------------------------------------------------------------------------------------------------------
		void setData(buffer_type_ptr source, int s_x, int s_y, int d_x, int d_y, int width, int height, int sourceWidth);
		
		// ----------------------------------------------------------------------------------------------------------------
		virtual void reload(CallbackPtr ptr, FileManager::PlatformPolicy policy);

		// ----------------------------------------------------------------------------------------------------------------
		void reapplySettings() { _applySettings(); }
        
	protected:
		
		enum TextureFormat
		{
			TF_Standard,		// standard uncompressed image (PNG, JPG, etc)
			TF_Compressed,		// compressed format (PVRT, etc)
			TF_Walaber,			// "walaber" raw format
            TF_WebP,            // Google "WebP" format
		};
        
        // ----------------------------------------------------------------------------------------------------------------
		void _applySettings();
        
		
		// ----------------------------------------------------------------------------------------------------------------
		void _getData(CallbackPtr cb );
        
        // ----------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------
		void initFromFileName();
		
		// ----------------------------------------------------------------------------------------------------------------
		void _fileLoaded( void* params );// const std::string& path, char* buffer, size_t length, const std::string& actualPath, const std::string& handlerName, const PropertyList& userData );
        
        // ----------------------------------------------------------------------------------------------------------------
		Texture::buffer_type_ptr bufferFromImageData( unsigned char* data, const int w, const int h );

        // calls callback with failure and sets programatic no texture
        void _failedToLoad( const std::string& path );

		
		CallbackPtr                             mGetDataCallback;
		CallbackPtr                             mLoadedCallback;
		
		FileManager::PlatformPolicy				mPlatformPolicy;
		
		TextureFormat							mTextureFormat;
        
        
        static const std::string                ERROR_TEXTURE_FILE_NAME;
        static const std::string                TEXTURE_SCALE_KEY;
        static const float                      HD_TO_SD_TEXTURE_SCALE;

	};
	
	MAKE_SHARED_PTR(Texture2D);
	
    
    // ----------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------
	Texture2DPtr createTexture2D(const std::string& fileName);

    // ----------------------------------------------------------------------------------------------------------------
    Texture2DPtr createTexture2D(const std::string& fileName, TextureSettings ts);
}


#endif	// _TEXTURE2D_H_


