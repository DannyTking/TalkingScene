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
mVAO(0)
{
}

inline void VertexArraysObject::create(GLuint /*VBO*/)
{
    //assert(mVAO == 0); // assert commented out to match WMW semantics
    glGenVertexArraysOES(1, &mVAO);
}

inline void VertexArraysObject::destroy()
{
    assert(mVAO != 0);
    glDeleteVertexArraysOES(1, &mVAO);
    mVAO = 0;
}

inline void VertexArraysObject::bind()
{
    assert(mVAO != 0);
    glBindVertexArrayOES(mVAO);
}

inline void VertexArraysObject::unbind()
{
    assert(mVAO != 0);
    glBindVertexArrayOES(0);
}
	
inline bool VertexArraysObject::valid()
{
	return (mVAO != 0);
}


inline void VertexArraysObject::setVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
    assert(mVAO != 0);
    glVertexPointer(size, type, stride, pointer);
    glEnableClientState(GL_VERTEX_ARRAY);
}

inline void VertexArraysObject::setColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
    assert(mVAO != 0);
    glColorPointer(size, type, stride, pointer);
    glEnableClientState(GL_COLOR_ARRAY);
}

inline void VertexArraysObject::setTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)
{
    assert(mVAO != 0);
    glTexCoordPointer(size, type, stride, pointer);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

} // namespace Walber