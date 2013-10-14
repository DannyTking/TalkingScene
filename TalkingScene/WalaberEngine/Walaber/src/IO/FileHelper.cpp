
#include "FileHelper.h"

#include <sys/stat.h>

#ifdef TARGET_NDK
#include <cstdio>
#endif // #ifdef TARGET_NDK

namespace Walaber
{
	
	
namespace FileHelper
{
		
    /*! Platform independent check to see if file exists
     @param path the path and filename to check if it is existing
     @return true if we are able ot get the file attributes which indicates that the file exists 
                and we have permission to access the folder which contains the file. If you
                need to do that level of checking, lookup the return values of stat which will 
                give you more details on why stat failed.
     */
	bool fileExists( std::string& path )
	{ 
		//Logger::printf("Walaber", Logger::SV_DEBUG, "looking for file : %s ... ", path.c_str());

		struct stat stFileInfo; 
		bool blnReturn; 
		int intStat; 
		
		// Attempt to get the file attributes 
		intStat = stat(path.c_str(),&stFileInfo); 
		
		if(intStat == 0) 
		{ 
			// We were able to get the file attributes 
			// so the file obviously exists.
			
			//Logger::printf("Walaber", Logger::SV_DEBUG, "exists\n");
			
			blnReturn = true; 
		} 
		else 
		{ 
			// We were not able to get the file attributes. 
			// This may mean that we don't have permission to 
			// access the folder which contains this file. If you 
			// need to do that level of checking, lookup the 
			// return values of stat which will give you 
			// more details on why stat failed. 

			//Logger::printf("Walaber", Logger::SV_DEBUG, "not found!\n");

			blnReturn = false; 
		} 
		
		return(blnReturn); 
	}
	
	unsigned int fileSize( std::string& path )
	{
		struct stat stFileInfo; 
		int intStat; 
		
		// Attempt to get the file attributes 
		intStat = stat(path.c_str(),&stFileInfo); 
		
		if (intStat == 0)
		{
			// file exists, return size...
			return stFileInfo.st_size;
		}
		else
		{
			// no file, return 0
			return 0;
		}
	}
	
	bool deleteFile(const std::string& path )
	{
		int ret = remove(path.c_str());
		return (ret == 0);
	}
    
    /*! Copy the file at sourcePath to the destinationPath
        @param sourcePath copy this file, including the name of the file to be copied
        @param destinationPath to this file, including the name of the file to be copied to
        @return true if the copy has taken place
     */
    bool copyFile(const std::string& sourcePath,
                  const std::string& destinationPath)
    {
        bool copied = false;
        FILE * sourceFile = std::fopen( sourcePath.c_str(), "rb" );
        FILE * destFile = std::fopen( destinationPath.c_str(), "w" );
		if (destFile)
		{
            char buffer[1024];
            size_t bytesRead;
            
            while ((bytesRead = fread(buffer, sizeof(char), sizeof(buffer), sourceFile)) > 0)
            {
                if (fwrite(buffer, sizeof(char), bytesRead, destFile) != bytesRead)
                {
                    /*Walaber::Logger::printf("Init", Walaber::Logger::SV_FATAL,
                                            "copyFile() - couldn't write to file %s",
                                            destinationPath.c_str());
                    */
                    copied = false;
                    break;
                }
            }
            
			if (fclose(destFile) == EOF)
			{
				/*Walaber::Logger::printf("Init", Walaber::Logger::SV_FATAL,
                                        "copyFile() - fclose of open file failed");
                 */
                copied = false;
			}
		}
		else
		{
			/*Walaber::Logger::printf("Init", Walaber::Logger::SV_FATAL,
                                    "copyFile() - couldn't open %s for writing ",
                                    destinationPath.c_str());
             */
            copied = false;
		}
        if (fclose(sourceFile) == EOF)
        {
            /*Walaber::Logger::printf("Init", Walaber::Logger::SV_FATAL,
                                    "copyFile() - fclose of open file failed");
             */
            copied = false;
        }
        return copied;
    }
	
}
	
}
