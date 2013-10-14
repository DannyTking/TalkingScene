
#include "FileHelper.h"

#include <sys/stat.h>


namespace Walaber
{
	
	
namespace FileHelper
{
		
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
	
}
	
}
