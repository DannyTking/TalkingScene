/*
 *  VertexArraysObject.h
 *  WalaberEngine
 *
 *  Created by Filip Stoklas on 10/20/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef VERTEXARRAYSOBJECT_H
#define VERTEXARRAYSOBJECT_H

#include <GraphicsGL.h>

namespace Walaber 
{
    // Note that in order to closely mimic GL VAO, the class has value semantics. So be careful
    // about multiple copies of the same object as they point to the same resource and don't
    // see each other.
    class VertexArraysObject
    {
     public:

        VertexArraysObject();

        void create(GLuint VBO); // glGenVertexArrays()
        void destroy(); // glDeleteVertexArrays()

        void bind(); // glBindVertexArray()
        void unbind(); // glBindVertexArray(0)

		bool valid(); // does this object have a created VAO?
		
        void setVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
        void setColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
        void setTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);

     private:

#ifdef TARGET_NDK

        bool isBound;
        GLuint mVBO;
        GLuint mPrevBoundVBO;

        struct VertexAttribute
        {
            bool active; // if false, everything below is invalid
            GLint size;
            GLenum type;
            GLsizei stride;
            const GLvoid *pointer;

            struct BindState // this part gets set later in bind()
            {
                bool bound; // if false, everything below is invalid
                bool prevClientStateEnabled;
                GLint size;
                GLenum type;
                GLsizei stride;
                const GLvoid *pointer;

                BindState() : bound(false) {}
                BindState(bool prevClientStateEnabled, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) :
                bound(true), prevClientStateEnabled(prevClientStateEnabled), size(size), type(type),
                stride(stride), pointer(pointer) {}

            } bindState;

            VertexAttribute() : active(false) {}
            VertexAttribute(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) :
            active(true), size(size), type(type), stride(stride), pointer(pointer) {}

        };

        VertexAttribute mVertexAttrib;
        VertexAttribute mColorAttrib[WALABER_MAX_TEXTURE_UNITS];
        VertexAttribute mTexCoordAttrib[WALABER_MAX_TEXTURE_UNITS];

#else // IOS:

        GLuint mVAO;

#endif		

    };
} // namespace Walber

#ifdef TARGET_NDK

#include "VertexArraysObject-ndk.inl.h"

#elif defined(TARGET_IPHONE)

#include "VertexArraysObject-ios.inl.h"

#else

#include "VertexArraysObject-ogl.inl.h"

#endif

#endif // VERTEXARRAYSOBJECT_H
