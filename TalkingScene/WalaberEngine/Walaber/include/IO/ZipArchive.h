#ifndef _WALABER_ZIP_ARCHIVE_H_
#define _WALABER_ZIP_ARCHIVE_H_


#include <string>
#include <vector>
#include <map>

#include <zip.h>
#include <unzip.h>

namespace Walaber
{

	// ----====----====----====----====----====----====----====----====----====
	/// simple class for READING data from a .zip archive.
	class ZipArchiveReader
	{
	public:
		// ----====----====----====----====----====----====----====----====----====
		ZipArchiveReader( const std::string& zipFile );
		~ZipArchiveReader();
		
		// ----====----====----====----====----====----====----====----====----====
		std::vector< std::string > getFilenames();
		
		// ----====----====----====----====----====----====----====----====----====
		bool openFile( const std::string& filename );
		unsigned long getCurrentFileSize();
		void readCurrentFile( void* buffer );
        bool extractFile(const std::string& sourceFilename, const std::string& destinationFilename);

	private:
		// ----====----====----====----====----====----====----====----====----====
		wUnzFile			mUnzFile;

		// ----====----====----====----====----====----====----====----====----====
		struct FileLocation
		{

			wUnz_file_info64 			file_info;
    		wUnz_file_info64_internal 	file_info_internal;
    		ZPOS64_T 					num_file;
    		ZPOS64_T 					pos_in_central_dir;
		};

		// ----====----====----====----====----====----====----====----====----====
		typedef std::map< std::string, FileLocation > 	FileLocationMap;


		// ----====----====----====----====----====----====----====----====----====
		FileLocationMap			mFileLocations;
		wUnz_file_info64		mCurrentFileInfo;
	};
	
	
	// ----====----====----====----====----====----====----====----====----====
	/// simple class for CREATING a .zip archive.
	class ZipArchiveCreator
	{
	public:
		// ----====----====----====----====----====----====----====----====----====
		ZipArchiveCreator( const std::string& destZipFile );
		~ZipArchiveCreator();
		
		bool addFileToZip( const std::string& filePath, const std::string& nameOfFileInZip );
		
	private:
		// ----====----====----====----====----====----====----====----====----====
		zipFile			mZipFile;
	};
	
}

#endif