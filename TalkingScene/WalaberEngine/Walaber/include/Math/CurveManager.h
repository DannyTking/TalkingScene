//
//  CurveManager.h
//  WalaberEngine
//
//  Created by Jason Pecho on 5/5/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef _CURVE_MANAGER_H_
#define _CURVE_MANAGER_H_

#include <string>
#include <map>

#include <Curve.h>
#include <Callback.h>

namespace Walaber
{
    class PlatformManager;
    class FileManager;
    // -------------==============-------------==============-------------==============-------------===
    // <Summary>
    // Singleton class that loads curves and manages them to keep duplicates down
    // </Summary>
    class CurveManager
    {
    public: // typdefs, enums, andn structs

        struct CurveLoadedCallbackParameters
        {
            std::string name;
            CurvePtr curve;
            ResultCode result;
            
            CurveLoadedCallbackParameters():
            name(),
            curve(),
            result( ResultCode_Error )
            {}
            
            CurveLoadedCallbackParameters(const std::string& pname, CurvePtr pcurve,
                                          ResultCode presult):
            name(pname),
            curve(pcurve),
            result(presult)
            {}
        };
        
        struct CurveCollectionLoadedCallbackParameters
        {
            std::string name;
            bool didLoad;
            
            CurveCollectionLoadedCallbackParameters():
            name(),
            didLoad(false)
            {}
            
            CurveCollectionLoadedCallbackParameters(const std::string& pname, bool pdidLoad):
            name(pname),
            didLoad(pdidLoad)
            {}
        };
        
        struct PtrCurvePtrCallback
        {
            CurvePtr*                   mPtrToCurvePtr;
            CallbackPtr                 mCallback;
            
            PtrCurvePtrCallback():
            mPtrToCurvePtr(NULL),
            mCallback(NULL)
            {}
        };
        
        typedef std::multimap< std::string, PtrCurvePtrCallback >       PtrToCurvePtrMap;
        typedef std::multimap< std::string, CallbackPtr >               CurveCollectionPathToCallbackMap;
    public:        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // singleton get function
        // </Summary>
        static CurveManager& getManager()
        {
            if(!_instance)
            {
                _instance = new CurveManager();
            }
            
            return (*_instance);
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // takes in a ptr to a curvve ptr so we can change it once we've loaded the file
        // </Summary>
        void getCurve(CurvePtr* destPtr, const std::string& filename, CallbackPtr callback = CallbackPtr());
        
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // takes in a path to a curve collection binary file and maps its contents
        // </Summary>
        void loadCurveCollection(const std::string& filename, CallbackPtr callback = CallbackPtr());
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // takes in a path to a curve collection binary file and maps its contents, also only load
        // animations that are in the animations to load set, unless its empty, then load all
        // </Summary>
        void loadCurveCollection( const std::string& filename,
                                  CallbackPtr callback,
                                  const std::set< std::string >&  animationsToLoad );
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // optimises the map by removing unused curves
        // </Summary>
        void optimize();
		
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // removes all of the curves from the map that belong to this collection
        // </Summary>
		void unloadCurveCollection(const std::string& filename);

		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // prints the name of all curves in the map
        // </Summary>
		void printMappedCurves();
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // returns the empty curve object
        // </Summary>
		static CurvePtr getEmptyCurve()
        {
            return EmptyCurve;
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // returns the empty curve object
        // </Summary>
		static CurvePtr getOneCurve()
        {
            return OneCurve;
        }
		
		// -------------==============-------------==============-------------==============-------------===
        // callbacks
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // curve loaded callback
        // </Summary>
        void _initCurveFileLoaded(void* params); //const std::string& path, char* buffer, size_t length, const std::string& actualPath, const std::string& handlerName, const PropertyList& userData );
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // curve collection loaded callback
        // </Summary>
        void _initCurveCollectionLoaded(void* params); //const std::string& path, char* buffer, size_t length, const std::string& actualPath, const std::string& handlerName, const PropertyList& userData );
		
    private: // interface
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // private Ctor for singleton
        // </Summary>
        CurveManager();
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // private dtor for singleton
        // </Summary>
        ~CurveManager();
		
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // load a v1 curve collection
        // </Summary>
        bool _loadV1Collection(const std::string& collectionName, char* bytes,
                               unsigned int numBytes, const std::set< std::string >& animationsToLoad);
		
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // load a v2 curve collection
        // </Summary>
        bool _loadV2Collection(const std::string& collectionName, char* bytes,
                               unsigned int numBytes, const std::set< std::string >& animationsToLoad);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // load a v2 curve collection
        // </Summary>
        bool _loadV3Collection(const std::string& collectionName, char* bytes,
                               unsigned int numBytes, const std::set< std::string >& animationsToLoad);
        
        /*!
         Check if the curve is aprt of an animation to be loaded
         @param animationsToLoad set with names of animations to load, if set is empty, all curves
                should be loaded
         @param name of the curve to potentially load
        */
		bool _shouldLoadCurve( const std::set< std::string >& animationsToLoad,
                              const std::string& curveName );
		
        /*!
         Check if the group name is a part of the animations to be loaded
         @param animationsToLoad set with names of animations to load, if set is empty, all curves
         should be loaded
         @param name of the curve to potentially load
         */
		bool _shouldLoadGroup( const std::set< std::string >& groupsToLoad,
                               const std::string& groupName );
        
		struct CurveCollection
		{
			std::string				mCollectionName;
			bool					mIsLoaded;
			
			CurveCollection():
			mCollectionName(),
			mIsLoaded(false)
			{}
		};
		
		struct CurveMapEntry
		{
			CurvePtr		mCurve;
			int				mCurveCollectionIndex; // -1 if solo
			
			CurveMapEntry():
			mCurve(0),
			mCurveCollectionIndex(-1)
			{}
			
			CurveMapEntry(CurvePtr curvePtr):
			mCurve(curvePtr),
			mCurveCollectionIndex(-1)
			{}
			
			CurveMapEntry(CurvePtr curvePtr, int curveCollectionIndex):
			mCurve(curvePtr),
			mCurveCollectionIndex(curveCollectionIndex)
			{}
		};
        
		typedef std::map< std::string, CurveMapEntry >                           CurvePtrMap;
        
    private: // members
        static CurveManager* _instance;
        
        CurvePtrMap                         mCurveMap;   
		std::vector<CurveCollection>		mCurveCollections;		
		FileManager*                        mFileManager; // pointer to singleton
		
        
        PtrToCurvePtrMap                    mCurvePtrToUpdate;
        CurveCollectionPathToCallbackMap    mCollectionPathToCallbackMap;
        std::vector<std::string>            mLoadedCurveCollections;
        
        static const CurvePtr               EmptyCurve;
        static const CurvePtr               OneCurve;
        static const std::string ANIMATION_LIST_POINTER_KEY;
        static std::set< std::string >      tempAnimationsToLoad; ///< static structure instead of passing
                                                               //   newed stl containers through callbacks

    };
}
#endif 