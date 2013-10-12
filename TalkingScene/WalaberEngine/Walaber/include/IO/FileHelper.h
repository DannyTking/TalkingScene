#ifndef _FILE_HELPER_H_
#define _FILE_HELPER_H_

#include <string>

namespace Walaber
{

namespace FileHelper
{

	bool fileExists( std::string& path );

	unsigned int fileSize( std::string& path );
	
	bool deleteFile(const std::string& path );
    
    bool copyFile(const std::string& sourcePath, const std::string& destinationPath);

}

}


#endif	// _FILE_HELPER_H_