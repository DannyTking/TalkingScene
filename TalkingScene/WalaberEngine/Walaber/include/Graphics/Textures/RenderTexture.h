#ifndef _RENDER_TEXTURE2D_H_
#define _RENDER_TEXTURE2D_H_

#include "Texture.h"
#include <assert.h>

namespace Walaber
{
    // ----------------------------------------------------------------------------------------------------------------
    class RenderTexture2D: public Texture
    {
    public:
        // ----------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------
        RenderTexture2D(unsigned int width, unsigned int height, TextureInMemoryColorspace fmt =
                TextureInMemoryColorspace_16_R4_G4_B4_A4)
                : Texture("Render_To_Texture2D", TT_RenderTexture), frameBuffer_(0), prevBuffer_(0)
        {
            settings_.setColorspace(fmt);

            initWithSize(width, height, fmt);

        }

        // ----------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------
        RenderTexture2D(unsigned int width, unsigned int height, TextureSettings settings, TextureInMemoryColorspace fmt =
                TextureInMemoryColorspace_16_R4_G4_B4_A4)
                : Texture("Render_To_Texture2D", settings, TT_RenderTexture), frameBuffer_(0), prevBuffer_(0)
        {
            settings_.setColorspace(fmt);

            initWithSize(width, height, fmt);
            _applySettings();
        }

        // ----------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------
        /**
         * Overload of createRenderTexture2D which accepts pixel data to use to initialize the render texture's contents
         *
         * NOTE: Format of the pixelData needs to match the width/height/format arguments
         */
        RenderTexture2D(unsigned int width, unsigned int height, unsigned char *pixelData,
                TextureInMemoryColorspace fmt = TextureInMemoryColorspace_16_R4_G4_B4_A4)
                : Texture("Render_To_Texture2D", TT_RenderTexture), frameBuffer_(0), prevBuffer_(0)
        {
            settings_.setColorspace(fmt);

            initWithPixelData(width, height, fmt, pixelData);

        }

        // ----------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------

        // ----------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------
        ~RenderTexture2D()
        {
            Logger::printf("Walaber", Logger::SV_DEBUG,
                    "RenderTexture2D::~RenderTexture2D() - frame buffer [%d] texture [%d] \n", frameBuffer_, name_);

            if (frameBuffer_ != 0)
            {
                GL_DELETE_FRAMEBUFFERS(1, &frameBuffer_);
            }

            if (name_ != NO_TEXTURE)
            {
                glDeleteTextures(1, &name_);
            }

        }

        // ----------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------
        void bind();

        // ----------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------
        void unbind();

        // ----------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------
        void setViewport();

        // ----------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------
        void invalidate()
        {
            frameBuffer_ = 0;
            prevBuffer_ = 0;
            name_ = NO_TEXTURE;
        }

        unsigned char *createScreenGrab(unsigned int *outWidth, unsigned int *outHeight, GLenum *outColorFormat);

    protected:

        // ----------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------
        void initWithSize(unsigned int w, unsigned int h, TextureInMemoryColorspace fmt);

        // ----------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------

        void initWithPixelData(unsigned int w, unsigned int h, TextureInMemoryColorspace fmt, unsigned char *pixelData);

        // ----------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------
        void _applySettings();

        // ----------------------------------------------------------------------------------------------------------------
        // ----------------------------------------------------------------------------------------------------------------
        GLuint frameBuffer_;
        GLint prevBuffer_;

        // cached viewport settings (to restore when we unbind)
        int prevViewportX_;
        int prevViewportY_;
        int prevViewportW_;
        int prevViewportH_;

        // cached matrices...
        float prevModelView_[16];
        float prevProjection_[16];

        GLenum mFormat;
        GLenum mType;

    };

    MAKE_SHARED_PTR(RenderTexture2D);

    // ----------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------
    RenderTexture2DPtr createRenderTexture2D(unsigned int width, unsigned int height, TextureInMemoryColorspace fmt =
            TextureInMemoryColorspace_16_R4_G4_B4_A4);

    RenderTexture2DPtr createRenderTexture2D(unsigned int width, unsigned int height, TextureSettings settings,
            TextureInMemoryColorspace fmt = TextureInMemoryColorspace_16_R4_G4_B4_A4);

    RenderTexture2DPtr createRenderTexture2D(unsigned int rttWidth, unsigned int rttHeight, TextureInMemoryColorspace fmt, unsigned char *pixelData);


}

#endif	// _RENDER_TEXTURE2D_H_
