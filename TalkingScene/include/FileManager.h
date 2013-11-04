#ifndef _FILE_MANAGER_H_
#define _FILE_MANAGER_H_

#include "ResultCodes.h"
#include "Callback.h"
#include "PropertyList.h"
#include "PlatformManager.h"
#include "Logger.h"

#include <fstream>
#include <map>
#include <vector>



namespace Walaber
{

    class ZipArchiveReader;
    
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
	class FileManager
	{
	public:
		static const int DEFAULT_HANDLER_PRIORITY;
		
		// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
		enum PlatformPolicy
		{
			PP_AllowAbstraction,
			PP_NoAbstraction
		};
		
		// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
		struct FileExistsCallbackParameters
		{
			FileExistsCallbackParameters( const PropertyList& plist ) : exists(false), userData(plist) { }
			FileExistsCallbackParameters( const std::string& p, bool e, const std::string& aP, const std::string& hN, const PropertyList& uD ) : 
											path(p), exists(e), actualPath(aP), handlerName(hN), userData(uD) { }
			
			std::string				path;
			bool					exists;
			std::string				actualPath;
			std::string				handlerName;
			const PropertyList&		userData;
		};
		
		// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
		struct ReadFileCallbackParameters
		{
			ReadFileCallbackParameters( const PropertyList& plist ) : result(ResultCode_OK), buffer(NULL), length(0), userData( plist ) { }
			ReadFileCallbackParameters( const std::string& p, char* b, size_t l, const std::string& aP, const std::string& hN, const PropertyList& uD ) :
											result(ResultCode_OK), path(p), buffer(b), length(l), actualPath(aP), handlerName(hN), userData(uD) { }
			
			ResultCode	result;
			std::string				path;
			char*					buffer;
			size_t					length;
			std::string				actualPath;
			std::string				handlerName;
			const PropertyList&		userData;
		};
		
		
		// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
		// base class for all File Handlers
		// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
		class FileHandler
		{
		public:
			virtual ~FileHandler()
			{}

			virtual void fileExists( const std::string& path, CallbackPtr callback, const PropertyList& userData ) = 0;
			virtual void readFile( const std::string& path, CallbackPtr callback, const PropertyList& userData ) = 0;
			
			virtual std::string getName() { return std::string("NONE"); }

		};
		
		
		
		// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
		// standard class that implements standard file i/o.
		// supports having multiple "base paths", to allow passing of relative filenames, and
		// still finding the full path for those files.
		// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
		class FH_StandardFileSystem : public FileHandler
		{
		public:
			void fileExists( const std::string& path, CallbackPtr callback, const PropertyList& userData );
			void readFile( const std::string& path, CallbackPtr callback, const PropertyList& userData );
            
            // returns the next lowest priority number for a base path
            unsigned int getNextLowestPriorityForBasePath()
            {
                unsigned int priority = 0;
                if(!mBasePaths.empty())
                {
                    std::multimap<unsigned int, std::string>::iterator last = --(mBasePaths.end());
                    priority = last->first;
                    ++priority;
                }
                return priority;
            }
            
            // lower numbers get higher priority
            // paths with the same priority have no guarenteed order
            void addBasePath( const std::string& base, const unsigned int priority ) 
            { 
                mBasePaths.insert(std::make_pair(priority, base)); 
            }
			
			std::string getName() { return std::string("FILE_SYSTEM"); }
			
		private:
			
			std::multimap<unsigned int, std::string >		mBasePaths;
		};

		
		// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
		// standard class that implements file i/o from specified zip file.
		// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
		class FH_ZipFileSystem : public FileHandler
		{
		private:
			// prevent copy ops
			FH_ZipFileSystem( const FH_ZipFileSystem& );
			FH_ZipFileSystem& operator=( const FH_ZipFileSystem& );
		public:
			FH_ZipFileSystem( const std::string& zipFile, const std::string& prefix );
            ~FH_ZipFileSystem();

			void fileExists( const std::string& path, CallbackPtr callback, const PropertyList& userData );
			void readFile( const std::string& path, CallbackPtr callback, const PropertyList& userData );

			std::string getName() { return std::string("ZIP"); }

		private:
			void fileExistsSuceeded( const std::string& path, const std::string& testPath, CallbackPtr callback, const PropertyList& userData );
			void readFileSucceeded( const std::string& path, const std::string& testPath, CallbackPtr callback, const PropertyList& userData );
		private:
			typedef std::vector< std::string > FilenameList;

			ZipArchiveReader* mZipReader;
			std::string mPrefix;
			FilenameList mFilenames;
		};
		
		// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
		static FileManager* getInstancePtr();
		
		// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
		// adds path with next lowest priority        
        void addBasePath( const std::string& bp ) 
        {
            mDefaultHandler->addBasePath(bp, mDefaultHandler->getNextLowestPriorityForBasePath()); 
        }
		
        // ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
		// adds path with next lowest priority        
        void addBasePath( const std::string& bp, unsigned int priority ) 
        {
            mDefaultHandler->addBasePath(bp, priority); 
        }
        
		// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
		void addFileHandler( int priorityLowerIsFirst, FileHandler* fh );

		// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
		void removeFileHandler( FileHandler* fh );
		
		// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
		void updatePlatformTags() { mPlatformTags = PlatformManager::getInstancePtr()->getPrioritizedPlatformTags(); }
		
		// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
		void fileExists( const std::string& path, CallbackPtr callback, const PropertyList& userData, PlatformPolicy policy = PP_AllowAbstraction );
				
		
		// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
		void readFile( const std::string& path, CallbackPtr callback, const PropertyList& userData, PlatformPolicy policy = PP_AllowAbstraction );
		
		
		// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
		void _existSuccess( const std::string& path, FileHandler* h );
		void _readSuccess( const std::string& path, FileHandler* h );
		
		// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
		void _existFail( const std::string& path, FileHandler* h, CallbackPtr callback, const PropertyList& userData );
		void _readFail( const std::string& path, FileHandler* h, CallbackPtr callback, const PropertyList& userData );
		
		void _printFileRequests();

		
	protected:
		// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
		struct FileRecord
		{
			std::string			originalPath;
			int					pathTagIndex;
            PlatformPolicy      policy;
		};
		typedef std::map< std::string, FileRecord >		FileRecordMap;
		
		
		// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
		FileManager();
		~FileManager();
		
		
		
		// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
		void _initDefaultHandler();
		void _cleanupDefaultHandler();
		
		// returns TRUE if it was able to move to a new tag.  to move to the first tag, pass -1 into "inIndex".
		bool _appendNextPlatformTag( const std::string& inputPath, std::string& outPath, const int inIndex, int& outIndex );
		
		
		// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
		typedef std::multimap< int, FileHandler* >		FileHandlerMap;

		
		
		// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
		// ---------fm---------fm---------fm---------fm---------fm---------fm---------fm---------fm
		static FileManager*						sInstance;

		FileHandlerMap							mFileHandlers;
		
		FileRecordMap							mPendingExists;
		FileRecordMap							mPendingReads;
		
		std::vector< std::string >				mPlatformTags;
		
		FH_StandardFileSystem*					mDefaultHandler;

		std::map< std::string, int >			mFileRequests;
	};
}


#endif	// _FILE_MANAGER_H_
