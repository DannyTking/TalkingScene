/*
 *  GraphicsGL.h
 *  
 *
 *  Created by TFITZRAN on 11/3/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef GRAPHICSGL_H
#define GRAPHICSGL_H

#include <CommonGL.h>

#include <Color.h>
#include <StringHelper.h>
#include <algorithm>
#include <Logger.h>

#define GL_BUFFER_OFFSET(i) ((char *)NULL + (i))


// currently only supporting 2 texture units (max for older-generation hardware)
#define WALABER_MAX_TEXTURE_UNITS		2


namespace Walaber 
{	
	
	///--------------------------------------------------------------------
	// how vertex colors are blended with textures.
	///--------------------------------------------------------------------
	enum VertexColorBlendMode
	{
		VCBM_Multiply,	// default
		VCBM_Additive,	// additive blending between textures
	};

	
	
	class GraphicsGL	
	{
	public:
	
		struct DumpedGLState
		{
			GLboolean blendEnabled;
			GLenum blendSrc;
			GLenum blendDest;
			
			GLint activeTexture;
			
			GLenum textureEnvMode[WALABER_MAX_TEXTURE_UNITS];
			GLboolean texture2DEnabled[WALABER_MAX_TEXTURE_UNITS];
			GLuint boundTexture[WALABER_MAX_TEXTURE_UNITS];

			GLuint boundArrayBuffer;

			GLint viewport[4];
			GLfloat color[4];

			GLboolean depthTestEnabled;
			GLenum depthTestFunc;

			GLboolean lineSmoothEnabled;
			GLfloat lineWidth;

			GLfloat modelViewMatrix[16];
			GLfloat projectionMatrix[16];
			GLfloat textureMatrix[16];

			void dump();
			void print();
			void printDiffs(const DumpedGLState& other);
		};

		static GLenum checkForError();
    
        static void reset() // called after returning from context-lost
        {
            getState() = State(); // force the default OpenGL state
        }
		
		static void setColor(const Color& color)
		{
			glColor4f(color.R, color.G, color.B, color.A);
		}
		
		static void setActiveTexture( int i )
		{
			//assert( (i >= 0) && (i < WALABER_MAX_TEXTURE_UNITS) );
			
			State &state = getState();
			
			if (i != state.activeTexture)
			{
				glActiveTexture(GL_TEXTURE0 + (i));
				glClientActiveTexture(GL_TEXTURE0 + (i));
				state.activeTexture = i;
			}
		}
		
		static int getActiveTexture()
		{
			State &state = getState();
			
			return state.activeTexture;
		}
		
		
		static void setTextureEnabled( bool enabled )
		{
            State &state = getState();
            
			if (enabled != state.texture[state.activeTexture].enabled)
			{
				state.texture[state.activeTexture].enabled = enabled;
				
				if (enabled)
					glEnable(GL_TEXTURE_2D);
				else
					glDisable(GL_TEXTURE_2D);
			}
		}
		
		static void bindTexture(unsigned int textureName = 0)
		{
            State &state = getState();
            
			if (state.texture[state.activeTexture].name != textureName) 
			{
				state.texture[state.activeTexture].name = textureName;
 				glBindTexture(GL_TEXTURE_2D, textureName );
			}
		}
		
		
		static void invalidateTextures()
		{
			State &state = getState();
			
			for (int i = 0; i < WALABER_MAX_TEXTURE_UNITS; i++)
				state.texture[i].name = 0;
			
			state.activeTexture = 0;
		}
		
		
		static void setBlending( bool enabled )
		{
			State &state = getState();
			
			if (enabled != state.blending.enabled)
			{
				state.blending.enabled = enabled;
				
				if (enabled)
					glEnable(GL_BLEND);
				else
					glDisable(GL_BLEND);
			}
		}
		
		static void setAlphaBlending( bool enabled, unsigned int src = GL_ONE, unsigned int dest = GL_ONE_MINUS_SRC_ALPHA )
		{
            State &state = getState();
        
			setBlending(enabled);
			
			if (enabled && ((state.blending.src != src) || (state.blending.dest != dest)))
			{
				state.blending.src = src;
				state.blending.dest = dest;
				glBlendFunc(src, dest);
			}
		}
		
		static void getAlphaBlending( bool& enabled, unsigned int& src, unsigned int& dest )
		{
			State &state = getState();
			
			enabled = state.blending.enabled;
			src = state.blending.src;
			dest = state.blending.dest;
		}
		
		
		static void setTextureEnvironmentMode(VertexColorBlendMode m = VCBM_Multiply )
		{
			State& state = getState();
			
			if (state.texture[state.activeTexture].textureEnvMode != m)
			{
				state.texture[state.activeTexture].textureEnvMode = m;
				
				switch (m)
				{
					case VCBM_Additive:
						glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
						
						glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_ADD);			//Add RGB with RGB
						glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_PRIMARY_COLOR);
						glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_TEXTURE);
						glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
						glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
						
						
						glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE);	// leave A alone.
						glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_PRIMARY_COLOR);
						glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_ALPHA, GL_TEXTURE);
						glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
						glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA);
						break;
						
					default:
					case VCBM_Multiply:
						glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
						break;
				}
			}
		}

        static void enableVertexClientState()
        {
            State &state = getState();
        
            if(!state.vertexClientStateEnabled)
            {
                glEnableClientState(GL_VERTEX_ARRAY);
                state.vertexClientStateEnabled = true;
            }
        }
        
        static void disableVertexClientState()
        {
            State &state = getState();
        
            if(state.vertexClientStateEnabled)
            {
                glDisableClientState(GL_VERTEX_ARRAY);
                state.vertexClientStateEnabled = false;
            }
        }
        
        static bool isVertexClientStateEnabled()
        {
            return getState().vertexClientStateEnabled;
        }
		
        static void enableNormalClientState()
        {
            State &state = getState();
        
            if(!state.normalClientStateEnabled)
            {
                glEnableClientState(GL_NORMAL_ARRAY);
                state.normalClientStateEnabled = true;
            }
        }
        
        static void disableNormalClientState()
        {
            State &state = getState();
        
            if(state.normalClientStateEnabled)
            {
                glDisableClientState(GL_NORMAL_ARRAY);
                state.normalClientStateEnabled = false;
            }
        }
        
        static bool isNormalClientStateEnabled()
        {
            return getState().normalClientStateEnabled;
        }
        
        static void enableTextureClientState()
        {
            State &state = getState();
        
            if(!state.texture[state.activeTexture].clientStateEnabled)
            {
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                state.texture[state.activeTexture].clientStateEnabled = true;
            }
        }
        
        static void disableTextureClientState()
        {
            State &state = getState();
        
            if(state.texture[state.activeTexture].clientStateEnabled)
            {
                glDisableClientState(GL_TEXTURE_COORD_ARRAY);
                state.texture[state.activeTexture].clientStateEnabled = false;
            }
        }
        
        static bool isTextureClientStateEnabled()
        {
			State &state = getState();
			
            return state.texture[state.activeTexture].clientStateEnabled;
        }
                
        static void enableColorClientState()
        {
            State &state = getState();
        
            if(!state.colorClientStateEnabled[state.activeTexture])
            {
                glEnableClientState(GL_COLOR_ARRAY);
                state.colorClientStateEnabled[state.activeTexture] = true;
            }
        }
        
        static void disableColorClientState()
        {
            State &state = getState();
        
            if(state.colorClientStateEnabled[state.activeTexture])
            {
                glDisableClientState(GL_COLOR_ARRAY);
                state.colorClientStateEnabled[state.activeTexture] = false;
            }
        }
        
        static bool isColorClientStateEnabled()
        {
			State& state = getState();
			
            return state.colorClientStateEnabled[state.activeTexture];
        }
		
		static void setMatrixMode(unsigned int matrixMode)
		{
			glMatrixMode(matrixMode);
		}
		
		static void setMatrixModeAndLoadIdentity(unsigned int matrixMode)
		{
			glMatrixMode(matrixMode);
			glLoadIdentity();
		}
		
		static void createOrthoProjection(const float left, const float right, const float bottom, const float top, const float near, const float far)
		{
#if OPENGLES
			glOrthof( left, right, bottom, top, near, far );
#else
			glOrtho( left, right, bottom, top, near, far );
#endif
		}
		
		static void loadIdentityMatrix()
		{
			glLoadIdentity();
		}
		
		static void translate( const float x, const float y, const float z )
		{
			glTranslatef(x, y, z);
		}
		
		static void rotate( const float angle, const float x, const float y, const float z )
		{
			glRotatef(angle, x, y, z);
		}
		
		static void scale( const float x, const float y, const float z )
		{
			glScalef(x, y, z);
		}
        
		static void setViewport( int x, int y, int w, int h )
		{
			State &state = getState();
			
			if ((x != state.viewport.x) || (y != state.viewport.y) || (w != state.viewport.width) || (h != state.viewport.height))
			{
				state.viewport.x = x;
				state.viewport.y = y;
				state.viewport.width = w;
				state.viewport.height = h;
				
				glViewport(x, y, w, h);
				
				Logger::printf("Walaber", Logger::SV_VERBOSE, "viewport %d,%d %d,%d\n", x, y, w, h);
			}
		}
		
		static void getViewport( int& x, int& y, int& w, int& h )
		{
			State &state = getState();
			
			x = state.viewport.x;
			y = state.viewport.y;
			w = state.viewport.width;
			h = state.viewport.height;
		}
		
		static void bindFramebuffer( const int framebuffer )
		{
			State& state = getState();
			if( state.currentFramebuffer != framebuffer )
			{
				state.currentFramebuffer = framebuffer;
				GL_BIND_FRAMEBUFFER( GL_FRAMEBUFFER, state.currentFramebuffer );
			}
		}
		
		static int getBoundFramebuffer( )
		{
			State& state = getState();
			return state.currentFramebuffer;
		}
		
		static void invalidateFramebuffer()
		{
			State& state = getState();
			state.currentFramebuffer = -1;
		}
		
		struct Extensions
		{
			bool hasMappedBuffers;
			bool supportsRTT;		// supports render-to-texture (aka has framebuffer extension)
			
			Extensions(): 
			hasMappedBuffers(false),
			supportsRTT(false)
			{
				const char* extensions = (const char*)glGetString(GL_EXTENSIONS);
				std::vector<std::string> strExt;
				StringHelper::split(std::string(extensions), ' ', strExt);
				
				for(int i = 0; i < strExt.size(); ++i)
				{
					Logger::printf("Walaber", Logger::SV_DEBUG, "%s\n", strExt[i].c_str());
				}
	
				if(std::find(strExt.begin(), strExt.end(), std::string("GL_OES_mapbuffer")) != strExt.end())
				{
					hasMappedBuffers = true;
					Logger::printf("Walaber", Logger::SV_DEBUG, "got mapped buffers.\n" );
				}

				if (std::find(strExt.begin(), strExt.end(), std::string("GL_OES_framebuffer_object")) != strExt.end())
				{
					supportsRTT = true;
					Logger::printf("Walaber", Logger::SV_DEBUG, "got RTT support.\n" );
				}
			}								
		};
		
		static Extensions& getExtensions()
		{
			static Extensions extensions;
			return extensions;
		}
			
	private:
    
        // This encapsulates the current OpenGL state. Note that the default state is forced in the default
        // constructor, which is supposed to be triggered at startup and after returning from context-lost.
        struct State
        {
            bool vertexClientStateEnabled;
            bool normalClientStateEnabled;
            bool colorClientStateEnabled[ WALABER_MAX_TEXTURE_UNITS ];

            struct Blending
            {
                bool enabled;
                unsigned int src;
                unsigned int dest;

                Blending() : enabled(false), src(GL_ZERO), dest(GL_ZERO)
                {
                    glDisable(GL_BLEND);
                    glBlendFunc(src, dest);
                }

            } blending;
			
			
			int activeTexture;
			
            
            struct Texture
            {
                bool enabled;
				bool clientStateEnabled;
                unsigned int name;
				VertexColorBlendMode textureEnvMode;
                
                Texture() : enabled(false), clientStateEnabled(false), name(0), textureEnvMode(VCBM_Multiply)
                {
                    glDisable(GL_TEXTURE_2D);
					glDisableClientState(GL_TEXTURE_COORD_ARRAY);
                    glBindTexture(GL_TEXTURE_2D, 0);
					glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                }
                
            } texture[WALABER_MAX_TEXTURE_UNITS];
			
			
			struct Viewport
			{
				int x;
				int y;
				int width;
				int height;
			} viewport;
			
			int currentFramebuffer;

            State() :
            vertexClientStateEnabled(false),
            normalClientStateEnabled(false),
			activeTexture(0),
			currentFramebuffer(0)
            {
                glDisableClientState(GL_VERTEX_ARRAY);
                glDisableClientState(GL_NORMAL_ARRAY);
                glDisableClientState(GL_COLOR_ARRAY);
				glActiveTexture(GL_TEXTURE0);
				glClientActiveTexture(GL_TEXTURE0);

                glColor4f(1.f, 1.f, 1.f, 1.f);
				
				viewport.x = 0;
				viewport.y = 0;
				viewport.width = 0;
				viewport.height = 0;
				
				for (int i = 0; i < WALABER_MAX_TEXTURE_UNITS; i++)
				{
					colorClientStateEnabled[i] = false;
					texture[i] = Texture();
				}
            }

        };
		
        static State & getState()
        {
            // we access the state through a static function to prevent invoking
            // any GL command at the time of module initialization
            static State state;
            return state;
        }

		
	};
}
#endif
