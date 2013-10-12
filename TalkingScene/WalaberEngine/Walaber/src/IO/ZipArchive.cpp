#include "ZipArchive.h"
#include "Logger.h"

#include <fstream>

namespace Walaber
{

	// ----====----====----====----====----====----====----====----====----====
	// ----====----====----====----====----====----====----====----====----====
	ZipArchiveReader::ZipArchiveReader( const std::string& zipFile )
	{
		// try to open up the file
		mUnzFile = wUnzOpen( zipFile.c_str() );

		// pre-cache all file locations so we don't need to look them up later.
		if (mUnzFile != NULL)
		{
			if (wUnzGoToFirstFile( mUnzFile ) == UNZ_OK)
			{
				do 
				{
					char filename[ 256 ];
					FileLocation loc;
					
					wUnzGetCurrentFileInfo64(mUnzFile,
										  &loc.file_info,
										  &loc.file_info_internal,
										  filename,
										  256,
										  NULL,
										  0,
										  NULL,
										  0,
										  &loc.num_file,
										  &loc.pos_in_central_dir);
					

					//Logger::printf("Walaber", Logger::SV_INFO, "ZipArchive found file: {%s}\n", filename);
					

					// insert into map!
					mFileLocations[ std::string(filename) ] = loc;
					
				} while (wUnzGoToNextFile(mUnzFile) == UNZ_OK);
			}
		}

		
	}
	
	// ----====----====----====----====----====----====----====----====----====
	// ----====----====----====----====----====----====----====----====----====
	ZipArchiveReader::~ZipArchiveReader()
	{
		if (mUnzFile != NULL)
		{
			// close the zip file.
			wUnzCloseCurrentFile( mUnzFile );
			wUnzClose( mUnzFile );
		}
	}
	
	// ----====----====----====----====----====----====----====----====----====
	// ----====----====----====----====----====----====----====----====----====
	std::vector< std::string > ZipArchiveReader::getFilenames()
	{
		std::vector< std::string > ret;
		
		for (FileLocationMap::iterator it = mFileLocations.begin(); it != mFileLocations.end(); ++it)
		{
			ret.push_back( it->first );
		}
		
		return ret;
	}
	
	// ----====----====----====----====----====----====----====----====----====
	// ----====----====----====----====----====----====----====----====----====
	bool ZipArchiveReader::openFile( const std::string& filename )
	{
		if (mUnzFile == NULL)
			return false;

		FileLocationMap::iterator it = mFileLocations.find( filename );
		if (it != mFileLocations.end())
		{
		
			wUnzLocateFileImmediate( mUnzFile, it->second.file_info, it->second.file_info_internal, it->second.num_file, it->second.pos_in_central_dir );
			wUnzOpenCurrentFile( mUnzFile );
			mCurrentFileInfo = it->second.file_info;
			//Logger::printf("Walaber", Logger::SV_INFO, "openFile(%s) found file in archive, size is %ld uncompressed.\n", filename.c_str(), it->second.file_info.uncompressed_size);
		}
		else
		{
			Logger::printf("Walaber", Logger::SV_ERROR, "openFile(%s) ERROR! File not found in archive!\n", filename.c_str());
		}
        return true;
	}
	
	// ----====----====----====----====----====----====----====----====----====
	// ----====----====----====----====----====----====----====----====----====
	unsigned long ZipArchiveReader::getCurrentFileSize()
	{
		return mCurrentFileInfo.uncompressed_size;
	}	
	
	// ----====----====----====----====----====----====----====----====----====
	// ----====----====----====----====----====----====----====----====----====
	void ZipArchiveReader::readCurrentFile( void* buffer )
	{
		int read = wUnzReadCurrentFile(mUnzFile, buffer, mCurrentFileInfo.uncompressed_size);
		Logger::printf("Walaber", Logger::SV_INFO, "ZipArchiveReader::readCurrentFile() read %d bytes", read);
	}

    /*! Extracts a file to the target destination
     @param sourceFilname the name of the file in the zip to be extracted
     @param destinationFilename the name of the destination (path and filename) to be extracted to
     @return if the file was extracted
     */
    bool ZipArchiveReader::extractFile(const std::string& sourceFilename, const std::string& destinationFilename)
    {
        bool extracted(true);
		if (mUnzFile == NULL)
			return false;
                
		if (openFile(sourceFilename))
        {
            // read the database from app bundle
            int size = getCurrentFileSize();
            
            char *buffer = new char[size];
            readCurrentFile( buffer );
            Logger::printf("Walaber", Logger::SV_INFO, "ZipArchiveReader::readCurrentFile() read %d bytes", read);
            
            if (FILE *destFile = std::fopen( destinationFilename.c_str(), "w" ))
            {
                fwrite((void*)buffer, 1, size, destFile);
                
                if (fclose(destFile) == EOF)
                {
                    Walaber::Logger::printf("Init",
                                            Walaber::Logger::SV_INFO,
                                            "ZipArchivEreader::extractFile() - \
                                            fclose of open file failed");
                    extracted = false;
                }
            }
            else
            {
                Walaber::Logger::printf("Init",
                                        Walaber::Logger::SV_INFO,
                                        "ZipArchivEreader::extractFile() - couldn't \
                                        open %s for writing ", destinationFilename.c_str());
                extracted = false;
            }
            
            delete [] buffer;
        }
        else
        {
            extracted = false;
        }
        return extracted;
    }	
	
	
	// ----====----====----====----====----====----====----====----====----====
	// ----====----====----====----====----====----====----====----====----====
	// ----====----====----====----====----====----====----====----====----====
	// ----====----====----====----====----====----====----====----====----====
	ZipArchiveCreator::ZipArchiveCreator( const std::string& destZipFile )
	{
		// try to create the new file
		mZipFile = wZipOpen( destZipFile.c_str(), APPEND_STATUS_CREATE );
	}
	
	
	// ----====----====----====----====----====----====----====----====----====
	// ----====----====----====----====----====----====----====----====----====
	ZipArchiveCreator::~ZipArchiveCreator()
	{
		if (mZipFile != NULL)
		{
			// close the zip file.
			wZipClose( mZipFile, "created by ZipArchiveCreator" );
		}
	}
	
	// ----====----====----====----====----====----====----====----====----====
	// ----====----====----====----====----====----====----====----====----====
	bool ZipArchiveCreator::addFileToZip( const std::string& filePath, const std::string& nameOfFileInZip )
	{
		if (mZipFile == NULL)
			return false;
		
		std::fstream fileStream( filePath.c_str(), std::ios_base::in | std::ios_base::binary );
		if (fileStream.is_open())
		{
			// the source file is open, now add this as a new file into the ZIP archive.
			zip_fileinfo zip_info;
			
			int ret = wZipOpenNewFileInZip(mZipFile, 
										  nameOfFileInZip.c_str(), 
										  &zip_info, 
										  NULL, 
										  0, 
										  NULL, 
										  0, 
										  NULL, 
										  Z_DEFLATED, 
										  Z_DEFAULT_COMPRESSION);
			
			printf("openNewFileInZip... ret[%d]\n", ret);
			if (ret != ZIP_OK)
			{
				return false;
			}
			
			
			// write this entire file into the ZIP...
			unsigned int length = 0;
			fileStream.seekg(0, std::ios::end);
			length = fileStream.tellg();
			fileStream.seekg(0, std::ios::beg);
			
			char* buffer = new char[length];
			
			fileStream.read( buffer, length );
			fileStream.close();
			
			ret = wZipWriteInFileInZip(mZipFile, buffer, length);
			printf("WriteInFileInZip... ret[%d]\n", ret);
			if (ret != ZIP_OK)
			{
				delete[] buffer;
				return false;
			}
			
			ret = wZipCloseFileInZip(mZipFile);
			printf("CloseFileInZip... ret[%d]\n", ret);
			
			delete[] buffer;
			
			if (ret != ZIP_OK)
			{
				return false;
			}
			
			return true;
			
		}
		
		return false;
		
	}
	

}

