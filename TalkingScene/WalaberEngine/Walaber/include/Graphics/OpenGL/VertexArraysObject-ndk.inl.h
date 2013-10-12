/*
 *  VertexArraysObject-ndk.inl
 *  WalaberEngine
 *
 *  Created by Filip Stoklas on 10/20/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */  
namespace Walaber {

inline VertexArraysObject::VertexArraysObject():
isBound(false),
mVBO(0),
mPrevBoundVBO(0)
{
}

inline void VertexArraysObject::create(GLuint VBO)
{
    assert(VBO != 0);

    isBound = false;
    mVBO = VBO;
    mPrevBoundVBO = 0;

    mVertexAttrib = VertexAttribute();
        
    for (unsigned int i = 0; i < WALABER_MAX_TEXTURE_UNITS; i++)
    {
		mColorAttrib[i] = VertexAttribute();
        mTexCoordAttrib[i] = VertexAttribute();
    }
}

inline void VertexArraysObject::destroy()
{
    mVBO = 0;
}

namespace
{
    GLboolean getGLBool(GLenum pname) { GLboolean v = false; glGetBooleanv(pname, &v); return v; }
    GLint getGLInt(GLenum pname) { GLint v = 0; glGetIntegerv(pname, &v); return v; }
    GLvoid * getGLPtr(GLenum pname) { GLvoid *v = 0; glGetPointerv(pname, &v); return v; }
}

inline void VertexArraysObject::bind()
{
    assert(!isBound);
    assert(mVBO != 0);

    mPrevBoundVBO = getGLInt(GL_ARRAY_BUFFER_BINDING);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    assert(getGLInt(GL_ARRAY_BUFFER_BINDING) == mVBO);
    
    mVertexAttrib.bindState = VertexAttribute::BindState(
     GraphicsGL::isVertexClientStateEnabled(),
     getGLInt(GL_VERTEX_ARRAY_SIZE),
     getGLInt(GL_VERTEX_ARRAY_TYPE),
     getGLInt(GL_VERTEX_ARRAY_STRIDE),
     getGLPtr(GL_VERTEX_ARRAY_POINTER)
    );

    if(mVertexAttrib.active)
    {
        GraphicsGL::enableVertexClientState();
        glVertexPointer(mVertexAttrib.size, mVertexAttrib.type, mVertexAttrib.stride, mVertexAttrib.pointer);
    }
    else
        GraphicsGL::disableVertexClientState();
        
    for (unsigned int i = 0; i < WALABER_MAX_TEXTURE_UNITS; i++)
    {
        GraphicsGL::setActiveTexture( i );
		
		// COLORS
		mColorAttrib[i].bindState = VertexAttribute::BindState(
															   GraphicsGL::isColorClientStateEnabled(),
															   getGLInt(GL_COLOR_ARRAY_SIZE),
															   getGLInt(GL_COLOR_ARRAY_TYPE),
															   getGLInt(GL_COLOR_ARRAY_STRIDE),
															   getGLPtr(GL_COLOR_ARRAY_POINTER)
															   );
		
		if(mColorAttrib[i].active)
		{
			GraphicsGL::enableColorClientState();
			glColorPointer(mColorAttrib[i].size, mColorAttrib[i].type, mColorAttrib[i].stride, mColorAttrib[i].pointer);
		}
		else
		{
			GraphicsGL::disableColorClientState();
		}
		
		// TEXTURE COORDINATES
        mTexCoordAttrib[i].bindState = VertexAttribute::BindState(
																  GraphicsGL::isTextureClientStateEnabled(),
																  getGLInt(GL_TEXTURE_COORD_ARRAY_SIZE),
																  getGLInt(GL_TEXTURE_COORD_ARRAY_TYPE),
																  getGLInt(GL_TEXTURE_COORD_ARRAY_STRIDE),
																  getGLPtr(GL_TEXTURE_COORD_ARRAY_POINTER)
																  );
    	
        if(mTexCoordAttrib[i].active)
        {
            GraphicsGL::enableTextureClientState();
            glTexCoordPointer(mTexCoordAttrib[i].size, mTexCoordAttrib[i].type, mTexCoordAttrib[i].stride, mTexCoordAttrib[i].pointer);
        }
        else
		{
            GraphicsGL::disableTextureClientState();
		}
    }

    // back to texture zero.
    GraphicsGL::setActiveTexture(0);

    isBound = true;
}

inline void VertexArraysObject::unbind()
{
    assert(isBound);
    
    if(mVertexAttrib.bindState.bound)
    {
        if(mVertexAttrib.bindState.prevClientStateEnabled)
            GraphicsGL::enableVertexClientState();
        else
            GraphicsGL::disableVertexClientState();
            
        glVertexPointer(
						mVertexAttrib.bindState.size,
						mVertexAttrib.bindState.type,
						mVertexAttrib.bindState.stride,
						mVertexAttrib.bindState.pointer
						);
        
        // paranoia!
        assert(getGLBool(GL_VERTEX_ARRAY) == mVertexAttrib.bindState.prevClientStateEnabled);
        assert(getGLInt(GL_VERTEX_ARRAY_SIZE) == mVertexAttrib.bindState.size);
        assert(getGLInt(GL_VERTEX_ARRAY_TYPE) == mVertexAttrib.bindState.type);
        assert(getGLInt(GL_VERTEX_ARRAY_STRIDE) == mVertexAttrib.bindState.stride);
        assert(getGLPtr(GL_VERTEX_ARRAY_POINTER) == mVertexAttrib.bindState.pointer);

        mVertexAttrib.bindState = VertexAttribute::BindState(); // reset
    }

    for (unsigned int i = 0; i < WALABER_MAX_TEXTURE_UNITS; i++)
    {
        GraphicsGL::setActiveTexture( i );

		if(mColorAttrib[i].bindState.bound)
		{
			if(mColorAttrib[i].bindState.prevClientStateEnabled)
			{
				GraphicsGL::enableColorClientState();
				
				glColorPointer(
							   mColorAttrib[i].bindState.size,
							   mColorAttrib[i].bindState.type,
							   mColorAttrib[i].bindState.stride,
							   mColorAttrib[i].bindState.pointer
							   );
				
				// paranoia!
				assert(getGLBool(GL_COLOR_ARRAY) == mColorAttrib[i].bindState.prevClientStateEnabled);
				assert(getGLInt(GL_COLOR_ARRAY_SIZE) == mColorAttrib[i].bindState.size);
				assert(getGLInt(GL_COLOR_ARRAY_TYPE) == mColorAttrib[i].bindState.type);
				assert(getGLInt(GL_COLOR_ARRAY_STRIDE) == mColorAttrib[i].bindState.stride);
				assert(getGLPtr(GL_COLOR_ARRAY_POINTER) == mColorAttrib[i].bindState.pointer);
			}
			else
			{
				GraphicsGL::disableColorClientState();
            }
			
			mColorAttrib[i].bindState = VertexAttribute::BindState(); // reset
		}

		
        if(mTexCoordAttrib[i].bindState.bound)
        {
            if(mTexCoordAttrib[i].bindState.prevClientStateEnabled)
			{
                GraphicsGL::enableTextureClientState();
				
				glTexCoordPointer(
								  mTexCoordAttrib[i].bindState.size,
								  mTexCoordAttrib[i].bindState.type,
								  mTexCoordAttrib[i].bindState.stride,
								  mTexCoordAttrib[i].bindState.pointer
								  );
				
				// paranoya!
				assert(getGLBool(GL_TEXTURE_COORD_ARRAY) == mTexCoordAttrib[i].bindState.prevClientStateEnabled);
				assert(getGLInt(GL_TEXTURE_COORD_ARRAY_SIZE) == mTexCoordAttrib[i].bindState.size);
				assert(getGLInt(GL_TEXTURE_COORD_ARRAY_TYPE) == mTexCoordAttrib[i].bindState.type);
				assert(getGLInt(GL_TEXTURE_COORD_ARRAY_STRIDE) == mTexCoordAttrib[i].bindState.stride);
				assert(getGLPtr(GL_TEXTURE_COORD_ARRAY_POINTER) == mTexCoordAttrib[i].bindState.pointer);
			}
            else
			{
                GraphicsGL::disableTextureClientState();
			}
                
            mTexCoordAttrib[i].bindState = VertexAttribute::BindState(); // reset

        }
    }

    GraphicsGL::setActiveTexture(0);

    glBindBuffer(GL_ARRAY_BUFFER, mPrevBoundVBO);
    assert(getGLInt(GL_ARRAY_BUFFER_BINDING) == mPrevBoundVBO);

    isBound = false;
}
	
inline bool VertexArraysObject::valid()
{
	return (mVBO != 0);
}

inline void VertexArraysObject::setVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
    assert(isBound);
    mVertexAttrib = VertexAttribute(size, type, stride, pointer);
}

inline void VertexArraysObject::setColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
    assert(isBound);
    mColorAttrib[ GraphicsGL::getActiveTexture() ] = VertexAttribute(size, type, stride, pointer);
}

inline void VertexArraysObject::setTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
    assert(isBound);
    mTexCoordAttrib[ GraphicsGL::getActiveTexture() ] = VertexAttribute(size, type, stride, pointer);
}
	
} // namespace Walber
