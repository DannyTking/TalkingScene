#ifndef _PROGRAMMATIC_TEXTURE_H_
#define _PROGRAMMATIC_TEXTURE_H_

#include "Texture.h"
#include <assert.h>


namespace Walaber
{
	// ----------------------------------------------------------------------------------------------------------------
	class ProgrammaticTexture2D : public Texture
    {
	public:
		
		// ----------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------
		ProgrammaticTexture2D(const std::string& name, unsigned int width, unsigned int height, TextureInMemoryColorspace fmt = TextureInMemoryColorspace_32_R8_G8_B8_A8)
		: Texture(name, TT_Programmatic)
        {
			initWithSize(width, height, fmt);
		}
		
		// ----------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------
		ProgrammaticTexture2D(const std::string& name, unsigned int width, unsigned int height, TextureInMemoryColorspace fmt, buffer_type_ptr data)
		: Texture(name, TT_Programmatic)
        {
			initWithSize(width, height, fmt);
			setData( data );
		}
        
		
        // ----------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------
		~ProgrammaticTexture2D()
        {
			Logger::printf("Walaber", Logger::SV_DEBUG, "ProgrammaticTexture2D::~ProgrammaticTexture2D()\n" );
			
			if (name_ != NO_TEXTURE)
			{
				glDeleteTextures(1, &name_);
			}
		}
		
		
		// ----------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------
        void setData(buffer_type_ptr data);
		
		// ----------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------
        void setData(buffer_type_ptr source, int s_x, int s_y, int d_x, int d_y, int width, int height, int sourceWidth);
		
		
		// ----------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------
        void invalidate()
		{
			name_ = NO_TEXTURE;
		}
		
		// ----------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------
        void reload( CallbackPtr ptr, FileManager::PlatformPolicy policy = FileManager::PP_AllowAbstraction );

		
	protected:
        
        // ----------------------------------------------------------------------------------------------------------------
		// ----------------------------------------------------------------------------------------------------------------
        void initWithSize( unsigned int w, unsigned int h, TextureInMemoryColorspace fmt );
		
		// ----------------------------------------------------------------------------------------------------------------
		// ----------------------------------------------------------------------------------------------------------------
        void _applySettings();
        
	};
	
	MAKE_SHARED_PTR(ProgrammaticTexture2D);
	
    
    // ----------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------
	ProgrammaticTexture2DPtr createProgrammaticTexture2D(const std::string& name, unsigned int width, unsigned int height, TextureInMemoryColorspace fmt = TextureInMemoryColorspace_32_R8_G8_B8_A8);
	
	ProgrammaticTexture2DPtr createProgrammaticTexture2D(const std::string& name, unsigned int width, unsigned int height, TextureInMemoryColorspace fmt, Texture::buffer_type_ptr data);
	
    ProgrammaticTexture2DPtr createProgrammaticSolidColorTexture2D(const std::string& name, unsigned int width, unsigned int height, Color color, TextureInMemoryColorspace fmt = TextureInMemoryColorspace_32_R8_G8_B8_A8);
}



#endif // _PROGRAMMATIC_TEXTURE_H_
