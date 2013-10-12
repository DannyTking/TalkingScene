#ifndef _WALT_LOADER_H_
#define _WALT_LOADER_H_

#include <GraphicsGL.h>
#include <Texture.h>


namespace Walaber 
{
	// ---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]
	// load a "Walaber texture" (.waltex) raw image from the file data.
	// ---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]---------[#]
	unsigned char* loadWALT( unsigned char* fileData, unsigned int fileLength, int& outWidth, int& outHeight, Walaber::TextureInMemoryColorspace& outFmt );   

	
	// simple helper function for saving an image buffer to disk in "waltex" format.
	bool saveWALT( unsigned char* imgData, int width, int height, Walaber::TextureInMemoryColorspace fmt, const std::string& filename );
	
}
#endif
