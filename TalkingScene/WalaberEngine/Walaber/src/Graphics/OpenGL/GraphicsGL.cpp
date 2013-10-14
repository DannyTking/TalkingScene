//
//  GraphicsGL.cpp
//  WalaberEngine
//
//  Created by Jason Pecho on 8/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "GraphicsGL.h"

#include "Logger.h"
#include "TextureManager.h"

namespace Walaber
{
    //NDK/
    /*bool GraphicsGL::sVertexClientStateEnabled = false;
    bool GraphicsGL::sNormalClientStateEnabled = false;
    bool GraphicsGL::sTextureClientStateEnabled = false;
    bool GraphicsGL::sColorClientStateEnabled = false;*/
    //GraphicsGL::State GraphicsGL::sState;

	namespace
	{
		void printBool(const char* name, GLboolean val)
		{ Logger::printf("Walaber", Logger::SV_DEBUG, "%s: %s", name, val ? "YES" : "NO"); }

		void printEnum(const char* name, GLenum val)
		{ Logger::printf("Walaber", Logger::SV_DEBUG, "%s: %d", name, val); }

		void printInt(const char* name, GLint val)
		{ Logger::printf("Walaber", Logger::SV_DEBUG, "%s: %d", name, val); }

		void printUInt(const char* name, GLuint val)
		{ Logger::printf("Walaber", Logger::SV_DEBUG, "%s: %u", name, val); }

		void printTexture(const char* name, GLuint texId)
		{
			Logger::printf("Walaber", Logger::SV_DEBUG, "%s: %10u (%s)", name, texId,
					TextureManager::getManager().getTextureFileName(texId).c_str());
		}

		void printFloat(const char* name, GLfloat val)
		{ Logger::printf("Walaber", Logger::SV_DEBUG, "%s: %f", name, val); }

		void print4Ints(const char* name, const GLint* val)
		{ Logger::printf("Walaber", Logger::SV_DEBUG, "%s: %d %d %d %d", name, val[0], val[1], val[2], val[3]); }

		void print4Floats(const char* name, const GLfloat* val)
		{ Logger::printf("Walaber", Logger::SV_DEBUG, "%s: %f %f %f %f", name, val[0], val[1], val[2], val[3]); }

		void printMatrix(const char* name, const GLfloat* val)
		{
			// note matrix is stored in column major order
			for (size_t i = 0; i < 4; i++)
			{
				Logger::printf("Walaber", Logger::SV_DEBUG, "%s: %f %f %f %f", name,
						val[i * 4 + 0], val[i * 4 + 1], val[i * 4 + 2], val[i * 4 + 3]);
			}
		}

		bool cmp4Floats(const GLfloat* a, const GLfloat* b)
		{ return memcmp(a, b, sizeof(GLfloat) * 4) == 0; }

		bool cmp4Ints(const GLint* a, const GLint* b)
		{ return memcmp(a, b, sizeof(GLint) * 4) == 0; }

		bool cmpMatrices(const GLfloat* a, const GLfloat* b)
		{ return memcmp(a, b, sizeof(GLfloat) * 16) == 0; }

	}

	void GraphicsGL::DumpedGLState::dump()
	{
		/*
   			bool blendEnabled;
			GLenum blendSrc;
			GLenum blendDest;
		*/

		glGetBooleanv(GL_BLEND, &blendEnabled);
		glGetIntegerv(GL_BLEND_SRC, (GLint*)&blendSrc);
		glGetIntegerv(GL_BLEND_DST, (GLint*)&blendDest);
		
		
		// Active texture
		glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&activeTexture);
		
		for (int i = 0; i < WALABER_MAX_TEXTURE_UNITS; i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			
			// GL_TEXTURE_ENV_MODE setting
			glGetIntegerv(GL_TEXTURE_ENV_MODE, (GLint*)&textureEnvMode[i]);
			glGetBooleanv(GL_TEXTURE_2D, &texture2DEnabled[i]);
			glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint*)&boundTexture[i]);

		}
		
		// set back to original active texture...
		glActiveTexture(GL_TEXTURE0 + activeTexture);
		
		
		
		glGetIntegerv(GL_ARRAY_BUFFER_BINDING, (GLint*)&boundArrayBuffer);

		/*
			GLuint viewport[4];
		*/
		glGetIntegerv(GL_VIEWPORT, viewport);

		/*
			Color color;
		*/
		glGetFloatv(GL_CURRENT_COLOR, (GLfloat*)&color);

		/*
			bool depthTestEnabled;
			GLenum depthFunc;
		*/
		glGetBooleanv(GL_DEPTH_TEST, &depthTestEnabled);
		glGetIntegerv(GL_DEPTH_FUNC, (GLint*)&depthTestFunc);

		/*
			bool lineSmoothEnabled;
			GLfloat lineWidth;
		 */
		glGetBooleanv(GL_LINE_SMOOTH, &lineSmoothEnabled);
		glGetFloatv(GL_LINE_WIDTH, &lineWidth);


		/*
			GLfloat modelViewMatrix[16];
			GLfloat projectionMatrix[16];
			GLfloat textureMatrix[16];
 	 	 */

		glGetFloatv(GL_MODELVIEW_MATRIX, modelViewMatrix);
		glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);
		glGetFloatv(GL_TEXTURE_MATRIX, textureMatrix);

	}

	void GraphicsGL::DumpedGLState::print()
	{
		/*
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
		*/
		
		printBool("GL_BLEND", blendEnabled);
		printEnum("GL_BLEND_SRC", blendSrc);
		printEnum("GL_BLEND_DST", blendDest);
		printInt("GL_ACTIVE_TEXTURE", activeTexture);

		for (int i = 0; i < WALABER_MAX_TEXTURE_UNITS; i++)
		{
			printEnum("GL_TEXTURE_ENV_MODE", textureEnvMode[i]);
			printBool("GL_TEXTURE_2D", texture2DEnabled[i]);
			printTexture("GL_TEXTURE_BIDING_2D", boundTexture[i]);
		}
		
		printUInt("GL_ARRAY_BUFFER_BINDING", boundArrayBuffer);
		print4Ints("GL_VIEWPORT", viewport);
		print4Floats("GL_CURRNET_COLOR", color);
		printBool("GL_DEPTH_TEST", depthTestEnabled);
		printEnum("GL_DEPTH_FUNC", depthTestFunc);
		printBool("GL_LINE_SMOOTH", lineSmoothEnabled);
		printFloat("GL_LINE_WIDTH", lineWidth);
		printMatrix("GL_MODELVIEW_MATRIX", modelViewMatrix);
		printMatrix("GL_PROJECTION_MATRIX", projectionMatrix);
		printMatrix("GL_TEXTURE_MATRIX", textureMatrix);

	}

	void GraphicsGL::DumpedGLState::printDiffs(const GraphicsGL::DumpedGLState& other)
	{
		if (blendEnabled != other.blendEnabled)
		{
			printBool("A GL_BLEND", blendEnabled);
			printBool("B GL_BLEND", other.blendEnabled);
		}

		if (blendSrc != other.blendSrc)
		{
			printEnum("A GL_BLEND_SRC", blendSrc);
			printEnum("B GL_BLEND_SRC", other.blendSrc);
		}

		if (blendDest != other.blendDest)
		{
			printEnum("A GL_BLEND_DST", blendDest);
			printEnum("B GL_BLEND_DST", other.blendDest);
		}
		
		if (activeTexture != other.activeTexture)
		{
			printInt("A GL_ACTIVE_TEXTURE", activeTexture);
			printInt("B GL_ACTIVE_TEXTURE", other.activeTexture);
		}

		for (int i = 0; i < WALABER_MAX_TEXTURE_UNITS; i++)
		{
			if (texture2DEnabled[i] != other.texture2DEnabled[i])
			{
				printBool("A GL_TEXTURE_2D", texture2DEnabled[i]);
				printBool("B GL_TEXTURE_2D", other.texture2DEnabled[i]);
			}
			
			if (boundTexture[i] != other.boundTexture[i])
			{
				printTexture("A GL_TEXTURE_BIDING_2D", boundTexture[i]);
				printTexture("B GL_TEXTURE_BIDING_2D", other.boundTexture[i]);
			}
			
			if (textureEnvMode[i] != other.textureEnvMode[i])
			{
				printEnum("A GL_TEXTURE_ENV_MODE", textureEnvMode[i]);
				printEnum("B GL_TEXTURE_ENV_MODE", other.textureEnvMode[i]);
			}
		}

		if (boundArrayBuffer != other.boundArrayBuffer)
		{
			printUInt("A GL_ARRAY_BUFFER_BINDING", boundArrayBuffer);
			printUInt("B GL_ARRAY_BUFFER_BINDING", other.boundArrayBuffer);
		}

		if (! cmp4Ints(viewport, other.viewport))
		{
			print4Ints("A GL_VIEWPORT", viewport);
			print4Ints("B GL_VIEWPORT", other.viewport);
		}

		if (! cmp4Floats(color, other.color))
		{
			print4Floats("A GL_CURRNET_COLOR", color);
			print4Floats("B GL_CURRNET_COLOR", other.color);
		}

		if (depthTestEnabled != other.depthTestEnabled)
		{
			printBool("A GL_DEPTH_TEST", depthTestEnabled);
			printBool("B GL_DEPTH_TEST", other.depthTestEnabled);
		}

		if (depthTestFunc != other.depthTestFunc)
		{
			printEnum("A GL_DEPTH_FUNC", depthTestFunc);
			printEnum("B GL_DEPTH_FUNC", other.depthTestFunc);
		}

		if (lineSmoothEnabled != other.lineSmoothEnabled)
		{
			printBool("A GL_LINE_SMOOTH", lineSmoothEnabled);
			printBool("B GL_LINE_SMOOTH", other.lineSmoothEnabled);
		}

		if (lineWidth != other.lineWidth)
		{
			printFloat("A GL_LINE_WIDTH", lineWidth);
			printFloat("B GL_LINE_WIDTH", other.lineWidth);
		}

		if (! cmpMatrices(modelViewMatrix, other.modelViewMatrix))
		{
			printMatrix("A GL_MODELVIEW_MATRIX", modelViewMatrix);
			printMatrix("B GL_MODELVIEW_MATRIX", other.modelViewMatrix);
		}

		if (! cmpMatrices(projectionMatrix, other.projectionMatrix))
		{
			printMatrix("A GL_PROJECTION_MATRIX", projectionMatrix);
			printMatrix("B GL_PROJECTION_MATRIX", other.projectionMatrix);
		}

		if (! cmpMatrices(textureMatrix, other.textureMatrix))
		{
			printMatrix("A GL_TEXTURE_MATRIX", textureMatrix);
			printMatrix("B GL_TEXTURE_MATRIX", other.textureMatrix);
		}
	}


	GLenum GraphicsGL::checkForError()
	{
		GLenum err = glGetError();

		if (err != GL_NO_ERROR)
		{
			const char* errMsg = "Unknown";

			switch (err)
			{
			case GL_INVALID_ENUM:
				errMsg = "Invalid enum";
				break;

			case GL_INVALID_VALUE:
				errMsg ="Invalid value";
				break;

			case GL_INVALID_OPERATION:
				errMsg = "Invalid operation";
				break;

			case GL_STACK_OVERFLOW:
				errMsg = "Stack overflow";
				break;

			case GL_STACK_UNDERFLOW:
				errMsg = "Stack underflow";
				break;

			case GL_OUT_OF_MEMORY:
				errMsg = "Out of memory";
				break;

			}

			Logger::printf("Walaber", Logger::SV_DEBUG, "##########################################################################################");
			Logger::printf("Walaber", Logger::SV_DEBUG, "OpenGL error: %s [%x]", errMsg, err);
			Logger::printf("Walaber", Logger::SV_DEBUG, "##########################################################################################");
		}

		return err;
	}
}
