#ifndef _PVRT_LOADER_H_
#define _PVRT_LOADER_H_

#include <GraphicsGL.h>

namespace Walaber 
{
	// ---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]
	// given a PVR texture FILE loaded in memory, returns a pointer (into the same raw memory) of the beginning
	// of the compressed image data, and also fills in outWidth, outHeight with the size of the image, and fills
	// outGLFormat w/ the OpenGL format enum for this data, and compLength with the # of compressed bytes.
	// ---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]
	unsigned char* loadPVR( unsigned char* fileData, unsigned int fileLength, int& outWidth, int& outHeight, GLenum& outGLFormat, unsigned int& compLength );   

}
#endif
