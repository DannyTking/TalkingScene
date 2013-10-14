#ifndef __WALBER_COMMON_GL_HEADER_FILE__
#define __WALBER_COMMON_GL_HEADER_FILE__


#ifdef OPENGLES
// OpenGLES

#ifdef TARGET_NDK
#include <GLES/gl.h>
#include <GLES/glext.h>

#else // iOS

// use OpenGLES 2.0 for all platforms
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>


#endif // def TARGET_NDK


// defines to make functions common between OpenGL and OpenGLES
#define GL_GEN_FRAMEBUFFERS				glGenFramebuffersOES
#define GL_DELETE_FRAMEBUFFERS			glDeleteFramebuffersOES
#define GL_BIND_FRAMEBUFFER				glBindFramebufferOES
#define GL_FRAMEBUFFER_TEXTURE2D		glFramebufferTexture2DOES
#define GL_CHECK_FRAMEBUFFER_STATUS		glCheckFramebufferStatusOES

#define GL_MAP_BUFFER					glMapBufferOES
#define GL_UNMAP_BUFFER					glUnmapBufferOES

#undef GL_FRAMEBUFFER_BINDING
#undef GL_RENDERBUFFER_BINDING
#undef GL_FRAMEBUFFER
#undef GL_FRAMEBUFFER_COMPLETE
#undef GL_COLOR_ATTACHMENT0

#define GL_FRAMEBUFFER_BINDING			GL_FRAMEBUFFER_BINDING_OES
#define GL_RENDERBUFFER_BINDING			GL_RENDERBUFFER_BINDING_OES
#define GL_FRAMEBUFFER					GL_FRAMEBUFFER_OES
#define GL_FRAMEBUFFER_COMPLETE			GL_FRAMEBUFFER_COMPLETE_OES
#define GL_COLOR_ATTACHMENT0			GL_COLOR_ATTACHMENT0_OES

#define GL_WRITE_ONLY					GL_WRITE_ONLY_OES

#else
// OpenGL

#include <OpenGL/gl.h>


// defines to make functions common between OpenGL and OpenGLES
#define GL_GEN_FRAMEBUFFERS				glGenFramebuffers
#define GL_DELETE_FRAMEBUFFERS			glDeleteFramebuffers
#define GL_BIND_FRAMEBUFFER				glBindFramebuffer
#define GL_FRAMEBUFFER_TEXTURE2D		glFramebufferTexture2D
#define GL_CHECK_FRAMEBUFFER_STATUS		glCheckFramebufferStatus

#define GL_MAP_BUFFER					glMapBuffer
#define GL_UNMAP_BUFFER					glUnmapBuffer


#endif



#endif //__WALBER_COMMON_GL_HEADER_FILE__
