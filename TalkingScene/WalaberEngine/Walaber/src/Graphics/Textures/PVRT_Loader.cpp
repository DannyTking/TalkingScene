#include "PVRT_Loader.h"

#include <GraphicsGL.h>
#include <WalaberMath.h>
#include <cassert>

#define PVR_TEXTURE_FLAG_TYPE_MASK  0xff


namespace Walaber 
{
    
	// ----------------------------------------------------------------------------------------------------------------
	// ----------------------------------------------------------------------------------------------------------------
	static const char gPVRTexIdentifier[] = "PVR!";
	
	// ----------------------------------------------------------------------------------------------------------------
	// ----------------------------------------------------------------------------------------------------------------
	enum PVRTextureFlagType
	{
		kPVRTextureFlagTypePVRTC_2 = 24,
		kPVRTextureFlagTypePVRTC_4
	};
	
	// ----------------------------------------------------------------------------------------------------------------
	// ----------------------------------------------------------------------------------------------------------------
	struct PVRTexHeader
	{
		unsigned int headerLength;
		unsigned int height;
		unsigned int width;
		unsigned int numMipmaps;
		unsigned int flags;
		unsigned int dataLength;
		unsigned int bpp;
		unsigned int bitmaskRed;
		unsigned int bitmaskGreen;
		unsigned int bitmaskBlue;
		unsigned int bitmaskAlpha;
		unsigned int pvrTag;
		unsigned int numSurfs;
	};

	
	// ----------------------------------------------------------------------------------------------------------------
	// ----------------------------------------------------------------------------------------------------------------
	unsigned char* loadPVR( unsigned char* fileData, unsigned int fileLength, int& outWidth, int& outHeight, GLenum& outGLFormat, unsigned int& compLength )
	{
#ifdef TARGET_IPHONE
		PVRTexHeader *header = NULL;
		unsigned int dataLength = 0, dataOffset = 0, dataSize = 0;
		unsigned int blockSize = 0, widthBlocks = 0, heightBlocks = 0;
		unsigned int width = 0, height = 0, bpp = 4;
		unsigned char *bytes = NULL;
		unsigned int formatFlags;
		bool hasAlpha = true;
		
		header = (PVRTexHeader *)fileData;
		
		// check header data!
		if (gPVRTexIdentifier[0] != ((header->pvrTag >>  0) & 0xff) ||
			gPVRTexIdentifier[1] != ((header->pvrTag >>  8) & 0xff) ||
			gPVRTexIdentifier[2] != ((header->pvrTag >> 16) & 0xff) ||
			gPVRTexIdentifier[3] != ((header->pvrTag >> 24) & 0xff))
		{
			return NULL;
		}
		
		if (header->bitmaskAlpha)
			hasAlpha = true;
		else
			hasAlpha = false;

		
		formatFlags = header->flags & PVR_TEXTURE_FLAG_TYPE_MASK;
		
		if (formatFlags == kPVRTextureFlagTypePVRTC_4 || formatFlags == kPVRTextureFlagTypePVRTC_2)
		{
			if (formatFlags == kPVRTextureFlagTypePVRTC_4)
				outGLFormat = (hasAlpha) ? GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG : GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
			else if (formatFlags == kPVRTextureFlagTypePVRTC_2)
				outGLFormat = (hasAlpha) ? GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG : GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
			
			outWidth = width = header->width;
			outHeight = height = header->height;
			
			compLength = dataLength = header->dataLength;
			
			// get start of compressed data...
			bytes = (fileData) + sizeof(PVRTexHeader);
			
			
			// Calculate the data size for TOP texture level and respect the minimum number of blocks
			if (dataOffset < dataLength)
			{
				if (formatFlags == kPVRTextureFlagTypePVRTC_4)
				{
					blockSize = 4 * 4; // Pixel by pixel block size for 4bpp
					widthBlocks = width / 4;
					heightBlocks = height / 4;
					bpp = 4;
				}
				else
				{
					blockSize = 8 * 4; // Pixel by pixel block size for 2bpp
					widthBlocks = width / 8;
					heightBlocks = height / 4;
					bpp = 2;
				}
				
				// Clamp to minimum number of blocks
				if (widthBlocks < 2)
					widthBlocks = 2;
				if (heightBlocks < 2)
					heightBlocks = 2;
				
			 	compLength = dataSize = widthBlocks * heightBlocks * ((blockSize  * bpp) / 8);
			}
			
			// return data!
			return (unsigned char*)bytes;
		}
		
#else
		// unsupported on othe rplatforms
		assert(0);
#endif
		
		return NULL;
	}

}


