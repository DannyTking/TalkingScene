#include "WALT_Loader.h"

#include <GraphicsGL.h>
#include <WalaberMath.h>


namespace Walaber 
{
    
	// ----------------------------------------------------------------------------------------------------------------
	// ----------------------------------------------------------------------------------------------------------------
	unsigned char* loadWALT( unsigned char* fileData, unsigned int fileLength, int& outWidth, int& outHeight, Walaber::TextureInMemoryColorspace& outFmt )   
	{
		// check header data!
		if ((fileData[0] != 'W') ||
			(fileData[1] != 'A') ||
			(fileData[2] != 'L') ||
			(fileData[3] != 'T'))
		{
			// header error...
			return NULL;
		}
		
		int version = (int)fileData[4];
		
		if (version == 1)
		{
			outFmt = (Walaber::TextureInMemoryColorspace)fileData[5];
			unsigned short w = 0;
			unsigned short h = 0;
			
			w = *((unsigned short*)&fileData[6]);
			h = *((unsigned short*)&fileData[8]);
			
			outWidth = (int)w;
			outHeight = (int)h;
			
			return &fileData[16];
		}
		else
		{
			return NULL;
		}
		
		return NULL;
	}
	
	
	// ----------------------------------------------------------------------------------------------------------------
	// ----------------------------------------------------------------------------------------------------------------
	bool saveWALT( unsigned char* imgData, int width, int height, Walaber::TextureInMemoryColorspace fmt, const std::string& filename )
	{
		FILE* outFile = fopen( filename.c_str(), "wb" );
		
		if (!outFile)
		{
			printf("ERROR creating output file : %s\n", filename.c_str());
			return false;
		}
		
		
		// write the header:
		// "WALT" magic string
		// version number (1 byte)
		// image format (1 byte)
		// image width (2-byte unsigned short)
		// image width (2-byte unsigned short)
        // padding (6-bytes)
		const char header[] = { 'W', 'A', 'L', 'T', 1, (char)fmt };
		fwrite(header, 1, sizeof(header)/sizeof(char), outFile);
		
		unsigned short s_w = (unsigned short)width;
		unsigned short s_h = (unsigned short)height;
		
		fwrite(&s_w, sizeof(unsigned short), 1, outFile);
		fwrite(&s_h, sizeof(unsigned short), 1, outFile);
		
		int pW = Walaber::nextPOT(width);
		int pH = Walaber::nextPOT(height);
        
        const char padding[] = { 'P', 'A', 'D', 'I', 'N', 'G' };
        fwrite(padding, 1, sizeof(padding)/sizeof(char), outFile);
		
		// now write the image.
		fwrite(imgData, (fmt == TextureInMemoryColorspace_32_R8_G8_B8_A8) ? (4) : (2), pW * pH, outFile );
		
		fclose(outFile);
		
		return true;
	}
	
}


