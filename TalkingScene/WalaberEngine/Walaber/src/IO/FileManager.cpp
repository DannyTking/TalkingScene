#include "FileManager.h"

#include "FileHelper.h"
#include "StringHelper.h"

#include "PlatformManager.h"
#include "ZipArchive.h"

#include "Logger.h"
#include <dirent.h>
#include <sys/stat.h>

#include <cassert>
#include <algorithm>

namespace Walaber
{
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	FileManager* FileManager::sInstance = 0;
	const int FileManager::DEFAULT_HANDLER_PRIORITY = 50;
	
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	FileManager::FileManager()
	{
		_initDefaultHandler();
		
		// get platform tags.
		mPlatformTags = PlatformManager::getInstancePtr()->getPrioritizedPlatformTags();
	}
	
	
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	FileManager::~FileManager()
	{
		_cleanupDefaultHandler();
	}
	
	
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	FileManager* FileManager::getInstancePtr()
	{
		if (!sInstance)
			sInstance = new FileManager();
		
		return sInstance;
	}
	
	
	
	
	
	
	
	
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	void FileManager::addFileHandler( int priorityLowerIsFirst, FileHandler* fh )
	{
		mFileHandlers.insert( std::pair<int, FileHandler*>( priorityLowerIsFirst, fh ) );
	}
	
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	void FileManager::removeFileHandler( FileHandler* fh )
	{
		for (FileHandlerMap::iterator it = mFileHandlers.begin(); it != mFileHandlers.end(); it++)
		{
			if (it->second == fh)
			{
				mFileHandlers.erase( it );
				return;
			}
		}
	}
	
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	bool FileManager::fileExistsInFileMappedCache( const std::string& path )
	{
        return mFileMapper.fileExists( path );
	}
	
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	void FileManager::fileExists( const std::string& path, CallbackPtr callback, const PropertyList& userData, PlatformPolicy policy )
	{
		// try the first file handler.
		FileHandlerMap::iterator it = mFileHandlers.begin();
		
		if (it != mFileHandlers.end())
		{
			// store this record...
			bool ignorePlatform = PlatformManager::getInstancePtr()->extensionShouldBeIgnored(path);
			
			FileRecord fr;
			fr.originalPath = path;
			fr.pathTagIndex = (ignorePlatform || (policy == PP_NoAbstraction)) ? mPlatformTags.size()-1 : 0;
			fr.policy = policy;
            
			std::string testPath = path;
			
			if ((policy == PP_AllowAbstraction) && (!ignorePlatform))
				_appendNextPlatformTag( path, testPath, -1, fr.pathTagIndex );
			
			
			mPendingExists[ testPath ] = fr;
			
			// now query if the file exists...
			it->second->fileExists( testPath, callback, userData );
		}
	}
	
		
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	void FileManager::readFile( const std::string& path, CallbackPtr callback, const PropertyList& userData, PlatformPolicy policy )
	{
		// try the first file handler.
		FileHandlerMap::iterator it = mFileHandlers.begin();
		
		if (it != mFileHandlers.end())
		{
			// store this record...
			bool ignorePlatform = PlatformManager::getInstancePtr()->extensionShouldBeIgnored(path);
			
			FileRecord fr;
			fr.originalPath = path;
			fr.pathTagIndex = (ignorePlatform || (policy == PP_NoAbstraction)) ? mPlatformTags.size()-1 : 0;
			if (fr.pathTagIndex < 0) { fr.pathTagIndex = 0; }
			
            fr.policy = policy;
            
			std::string testPath = path;
			
			if ((policy == PP_AllowAbstraction) && (!ignorePlatform))
				_appendNextPlatformTag(path, testPath, -1, fr.pathTagIndex);
			
			mPendingReads[ testPath ] = fr;
			
			// now try to read the file!
			it->second->readFile( testPath, callback, userData );
		}
	}
	
	
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	void FileManager::_existSuccess( const std::string& path, FileHandler* h )
	{
		mPendingExists.erase( path );
	}
	
	
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	void FileManager::_readSuccess( const std::string& path, FileHandler* h )
	{
		mPendingReads.erase( path );

		Logger::printf("Walaber", Logger::SV_DEBUG, "_readSuccess('%s')\n", path.c_str());

		std::map< std::string, int >::iterator it = mFileRequests.find(path);
		if (it != mFileRequests.end())
			it->second++;
		else
			mFileRequests[ path ] = 1;
	}


	void FileManager::_printFileRequests()
	{
		Logger::printf("Walaber", Logger::SV_INFO, "_printFileRequests():%d\n", mFileRequests.size());
	
		for (std::map< std::string, int >::iterator it = mFileRequests.begin(); it != mFileRequests.end(); ++it)
			Logger::printf("Walaber", Logger::SV_INFO, "File:'%s'\t\t\t\t%d\n", it->first.c_str(), it->second);

	}

	
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	void FileManager::_existFail( const std::string& path, FileHandler* h, CallbackPtr callback, const PropertyList& userData )
	{
		// failed, see if we have another platform string to try...
		FileRecordMap::iterator r_it = mPendingExists.find( path );
		
		
		// this must be in the map!
		assert(r_it != mPendingExists.end());
		
		
		FileRecord fr = r_it->second;
		
		// remove old pending record...
		mPendingExists.erase( r_it );
		
		
		std::string newPath;
		if (fr.policy == PP_AllowAbstraction && _appendNextPlatformTag( fr.originalPath, newPath, fr.pathTagIndex, fr.pathTagIndex ))
		{
			// we can try this new one before failing through to another file handler...
			
			// store new record...
			mPendingExists[ newPath ] = fr;
			
			// query file.
			h->fileExists(newPath, callback, userData);

		}
		else
		{
			
			// failed, see if we have another handler after this one, and try it if we do...
			FileHandlerMap::iterator it;
			
			for (it = mFileHandlers.begin(); it != mFileHandlers.end(); it++)
			{
				if (it->second == h)
				{
					it++;
					break;
				}
			}
			
			if (it != mFileHandlers.end())
			{
				Logger::printf("Walaber", Logger::SV_DEBUG, "FileManager::_existFail - trying next handler...\n");

				// trying from scratch again...
				if(fr.policy == PP_AllowAbstraction)
                {
                    _appendNextPlatformTag(fr.originalPath, newPath, -1, fr.pathTagIndex);
				}
                else
                {
                    newPath = fr.originalPath;
                }
                
				mPendingExists[ newPath ] = fr;
				
				it->second->fileExists( newPath, callback, userData );
			}
			else
			{
				// compete failure!
				Logger::printf("Walaber", Logger::SV_DEBUG, "FileManager::_existFail - total failure...\n");

				if (callback)
				{
					FileExistsCallbackParameters params( path, false, path, "NOT_FOUND", userData );
					callback->invoke( (void *)&params );
				}
			}
		}
	}
	
	
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	void FileManager::_readFail( const std::string& path, FileHandler* h, CallbackPtr callback, const PropertyList& userData )
	{
		// failed, see if we have another platform string to try...
		FileRecordMap::iterator r_it = mPendingReads.find( path );
		
		// this must be in the map!
		if(r_it == mPendingReads.end())
		{
			if (callback)
			{
				ReadFileCallbackParameters params(path, NULL, 0, path, "NOT_FOUND", userData);
				params.result = ResultCode_ErrorFileNotFound;
				
				callback->invoke( (void *)&params );
			}
			
			return;
		}
		
		
		FileRecord fr = r_it->second;
		
		mPendingReads.erase( r_it );
		
		
		std::string newPath;
		if (fr.policy == PP_AllowAbstraction && _appendNextPlatformTag( fr.originalPath, newPath, fr.pathTagIndex, fr.pathTagIndex ))
		{
			// try reading this next platform-tag before failing through to next handler...
			mPendingReads[ newPath ] = fr;
			
			h->readFile( newPath, callback, userData );
		}
		else
		{
			
			// failed, see if we have another handler after this one, and try it if we do...
			FileHandlerMap::iterator it;
			
			for (it = mFileHandlers.begin(); it != mFileHandlers.end(); it++)
			{
				if (it->second == h)
				{
					it++;
					break;
				}
			}
			
			if (it != mFileHandlers.end())
			{
				Logger::printf("Walaber", Logger::SV_DEBUG, "FileManager::_readFail - trying next handler\n");
				
				if(fr.policy == PP_AllowAbstraction)
                {
                    _appendNextPlatformTag(fr.originalPath, newPath, -1, fr.pathTagIndex);
                }
                else
                {
                    newPath = fr.originalPath;
                }
				
				mPendingReads[ newPath ] = fr;
				
				it->second->readFile( newPath, callback, userData );
			}
			else
			{
				// compete failure!
				Logger::printf("Walaber", Logger::SV_DEBUG, "FileManager::_readFail - complete fail [%s]\n", path.c_str());

				if (callback)
				{
					ReadFileCallbackParameters params(path, NULL, 0, path, "NOT_FOUND", userData);
					params.result = ResultCode_ErrorFileNotFound;
					
					callback->invoke( (void *)&params );
				}
			}
		}
	}
	
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	void FileManager::_initDefaultHandler()
	{
		mDefaultHandler = new FH_StandardFileSystem();
		addFileHandler(DEFAULT_HANDLER_PRIORITY, mDefaultHandler);
	}
	
	
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	void FileManager::_cleanupDefaultHandler()
	{
		delete mDefaultHandler;
	}
					   
   // ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
   // ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
   bool FileManager::_appendNextPlatformTag( const std::string& inputPath, std::string& outPath, const int inIndex, int& outIndex )
   {
	   outPath = inputPath;
	   
	   outIndex = (inIndex + 1);
	   if (outIndex < 0) { outIndex = 0; }
	   
	   
	   if (outIndex >= mPlatformTags.size())
	   {
		   outIndex = -1;
		   return false;
	   }
	   
	   outPath = StringHelper::removeExtension( inputPath ) + 
					mPlatformTags[ outIndex ] + 
					StringHelper::getExtension( inputPath );
	   
	   return true;
   }
   

	

	
	
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	void FileManager::FH_StandardFileSystem::fileExists( const std::string& path, CallbackPtr callback, const PropertyList& userData )
	{
		// try this as a fully qualified path first...
		std::string testPath = path;
        std::multimap<unsigned int, std::string>::iterator testBase = mBasePaths.begin();
		
		do
		{
			bool exists = FileManager::getInstancePtr()->fileExistsInFileMappedCache( testPath );
		
			if (exists)
			{
				FileManager::getInstancePtr()->_existSuccess( path, this );
				
				FileExistsCallbackParameters params( path, exists, testPath, getName(), userData );
				callback->invoke( (void*)&params );
				
				return;
			}
			else
			{
				if (testBase != mBasePaths.end())
				{
					testPath = testBase->second + path;
					testBase++;
				}
				else
				{
					// no more paths to test... we're done!
					break;
				}
			}
		} while (true);
		
		// fail!
		FileManager::getInstancePtr()->_existFail( path, this, callback, userData );
	}
	
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	void FileManager::FH_StandardFileSystem::readFile( const std::string& path, CallbackPtr callback, const PropertyList& userData )
	{
		// try this as a fully qualified path first...
		std::string testPath = path;
        std::multimap<unsigned int, std::string>::iterator testBase = mBasePaths.begin();
		
		do
		{

            if ( FileManager::getInstancePtr()->fileExistsInFileMappedCache( testPath ) )
            {
                std::fstream str( testPath.c_str(), std::ios::in | std::ios::binary );
                
                if (str.is_open())
                {
                    str.seekg( 0, std::ios::end );
                    size_t length = str.tellg();
                    str.seekg( 0, std::ios::beg );
                    
                    char* buffer = new char[ length ];

                    str.read( buffer, length );
                    
                    str.close();
                    
                    FileManager::getInstancePtr()->_readSuccess(path, this);
                    
                    ReadFileCallbackParameters params(path, buffer, length, testPath, getName(), userData);
                    callback->invoke( (void *)&params );
                    
                    return;
                }
                else
                {
                    // Failed to open a file that should have existed...
                    // This should only happen if files were removed from the base paths after
                    // startup.
                    assert( false );
                }
            }
			else
			{
				if (testBase != mBasePaths.end())
				{
					testPath = StringHelper::addPathComponent(testBase->second, path);
					testBase++;
				}
				else
				{
					// no more paths to test... we're done!
					break;
				}
			}
		} while (true);
		
		// fail!
		FileManager::getInstancePtr()->_readFail( path, this, callback, userData );
	}
	
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	FileManager::FH_ZipFileSystem::FH_ZipFileSystem(const std::string& zipFile, const std::string& prefix):
	mZipReader(0), mPrefix(prefix)
	{
        Logger::printf("Walaber IO", Logger::SV_VERBOSE, "Scanning for APKs in: '%s'", zipFile.c_str());
		mZipReader = new ZipArchiveReader(zipFile);

		mFilenames = mZipReader->getFilenames();

		Logger::printf("Walaber IO", Logger::SV_DEBUG, "List of APK (%d):", mFilenames.size());
        
		for (FilenameList::const_iterator i = mFilenames.begin(); i != mFilenames.end(); ++i)
		{
			Logger::printf("Walaber IO", Logger::SV_DEBUG, "%s", i->c_str());
		}
	}
    
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
    FileManager::FH_ZipFileSystem::~FH_ZipFileSystem()
    {
        delete mZipReader;
        mZipReader = 0;
    }

	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	void FileManager::FH_ZipFileSystem::fileExists( const std::string& path, CallbackPtr callback, const PropertyList& userData )
	{
		// try this as a fully qualified path first...
		std::string testPath = path[0] == '/' ? path.substr(1) : path;
		bool exists = std::count( mFilenames.begin(), mFilenames.end(), testPath );

		if (exists)
		{
			fileExistsSuceeded( path, testPath, callback, userData );
			return;
		}

		// now try path with prefix
		testPath = mPrefix + testPath;
		exists = std::count( mFilenames.begin(), mFilenames.end(), testPath );
		if (exists)
		{
			fileExistsSuceeded( path, testPath, callback, userData );
		}
		else
		{
			FileManager::getInstancePtr()->_existFail( path, this, callback, userData );
		}
	}

	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	void FileManager::FH_ZipFileSystem::fileExistsSuceeded( const std::string& path, const std::string& testPath, CallbackPtr callback, const PropertyList& userData )
	{
		FileManager::getInstancePtr()->_existSuccess( path, this );

		FileExistsCallbackParameters params( path, true, testPath, getName(), userData );
		callback->invoke( (void*)&params );
	}

	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	void FileManager::FH_ZipFileSystem::readFile( const std::string& path, CallbackPtr callback, const PropertyList& userData )
	{
		// try this as a fully qualified path first...
		std::string testPath = path[0] == '/' ? path.substr(1) : path;
		bool exists = std::count( mFilenames.begin(), mFilenames.end(), testPath );

		if (exists)
		{
			readFileSucceeded( path, testPath, callback, userData );
			return;
		}

		// now try path with prefix
		testPath = mPrefix + testPath;
		exists = std::count( mFilenames.begin(), mFilenames.end(), testPath );

		if (exists)
		{
			readFileSucceeded( path, testPath, callback, userData );
		}
		else
		{
			FileManager::getInstancePtr()->_readFail(path, this, callback, userData);
		}
	}

	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	void FileManager::FH_ZipFileSystem::readFileSucceeded( const std::string& path, const std::string& testPath, CallbackPtr callback, const PropertyList& userData )
	{
		mZipReader->openFile(testPath);
		unsigned long length = mZipReader->getCurrentFileSize();

		char* buffer = new char[ length ];
		mZipReader->readCurrentFile(buffer);

		FileManager::getInstancePtr()->_readSuccess(path, this);

		ReadFileCallbackParameters params(path, buffer, length, testPath, getName(), userData);
		callback->invoke( (void *)&params );
	}

	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
    FileManager::FileMapper::FileMapper()
    {
    }

	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
    FileManager::FileMapper::~FileMapper()
    {
    }

	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
    void FileManager::FileMapper::discoverFilesOnDisk( const std::string& basePath )
    {
        _recursivelyDiscoverFiles( basePath );
    }

	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
    void FileManager::FileMapper::_recursivelyDiscoverFiles( const std::string& basePath )
    {
        DIR * currentDirectory = opendir( basePath.c_str() );
        dirent * currentDirectoryEntry;

        // Iterate through all entries in the file directory recursing into directories and caching
        // file names
        while ( currentDirectory != NULL &&
               ( currentDirectoryEntry = readdir( currentDirectory ) ) != NULL )
        {
            const char * fileName = currentDirectoryEntry->d_name;

            // Ignore . and .. which would always be directories
            if ( ( strcmp( fileName, "." ) == 0 ) ||
                 ( strcmp( fileName, ".." ) == 0 ) )
            {
                continue;
            }

            // Get the full file path
            std::string fullPath = basePath + "/" + currentDirectoryEntry->d_name;

            // Find the stats for the file or directory, to find out which it is
            struct stat fileStat;

            int statCode = stat( fullPath.c_str(), &fileStat );
            assert( statCode != -1 );

            // If the file is a directory then recurse into it
            if ( S_ISDIR( fileStat.st_mode ) )
            {
                _recursivelyDiscoverFiles( fullPath );
            }
            // Else this is a file, add it to our listing
            else if ( S_ISREG( fileStat.st_mode ) )
            {
                mFileListingCache.insert( fullPath );
            }
        }

        // The directory no longer needs to be open
        if ( currentDirectory != NULL )
        {
            closedir( currentDirectory );
        }
    }

	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
    bool FileManager::FileMapper::fileExists( const std::string& file ) const
    {
        FileMapType::const_iterator iter = mFileListingCache.find( file );
        return ( iter != mFileListingCache.end() );
    }
}
