//
//  CurveManager.cpp
//  WalaberEngine
//
//  Created by Jason Pecho on 5/5/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include <algorithm>

#include "CurveManager.h"
#include "PlatformManager.h"
#include "FileManager.h"

#include <CurveLoading.h>
#include <MemberCallback.h>

namespace Walaber
{
    const std::string CurveManager::ANIMATION_LIST_POINTER_KEY = "anilist_pointer";

    std::set< std::string >      CurveManager::tempAnimationsToLoad;
    
    // -------------==============-------------==============-------------==============-------------===
    CurveManager* CurveManager::_instance = 0;
    const CurvePtr CurveManager::EmptyCurve( new LinearCurve() );
    const CurvePtr CurveManager::OneCurve( new LinearCurve( 1.0f ) );

    
    // -------------==============-------------==============-------------==============-------------===
    CurveManager::CurveManager():
    mCurveMap(),
	mFileManager(0)
    {
        //mPlatformManager = PlatformManager::getInstancePtr();
        mFileManager = FileManager::getInstancePtr();
    }
    
    // -------------==============-------------==============-------------==============-------------===
    CurveManager::~CurveManager()
    {      
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void CurveManager::getCurve(CurvePtr* curve, const std::string& filename, CallbackPtr callback)
    {
        //std::string realFilename = mPlatformManager->getPlatformSpecificFilename(filename);
        CurvePtrMap::iterator it = mCurveMap.find(filename);
        
        if(it == mCurveMap.end())
        {
            // setup caching for callback
            PtrCurvePtrCallback ptrcb;
            ptrcb.mPtrToCurvePtr = curve;
            ptrcb.mCallback = callback;
            
            bool isFirst = false;
            if(mCurvePtrToUpdate.find(filename) == mCurvePtrToUpdate.end())
            {
                isFirst = true;
            }
            
            mCurvePtrToUpdate.insert(std::make_pair(filename, ptrcb));
            (*curve) = EmptyCurve;
            
            // load file
            if(isFirst)
            {
                PropertyList plist;    
				
				MemberCallbackPtr<CurveManager>::type memberCallback( new MemberCallback<CurveManager>(this, &CurveManager::_initCurveFileLoaded) );
				CallbackPtr callback_ = static_pointer_cast<Callback>(memberCallback);
                mFileManager->readFile(filename, callback_, plist, FileManager::PP_NoAbstraction);
            }
        }
        else
        {
            (*curve) = it->second.mCurve;
            
            if(callback)
            {
                CurveLoadedCallbackParameters params(filename, (*curve), ResultCode_OK);
                callback->invoke(&params);
            }
        }
    }
    
    // -------------==============-------------==============-------------==============-------------===

    void CurveManager::loadCurveCollection(const std::string& filename, CallbackPtr callback)
    {
        loadCurveCollection( filename, callback, tempAnimationsToLoad );
    }
    
    // -------------==============-------------==============-------------==============-------------===

    void CurveManager::loadCurveCollection( const std::string& filename,
                                            CallbackPtr callback,
                                            const std::set< std::string >&  animationsToLoad )
    {
        if(std::find(mLoadedCurveCollections.begin(),mLoadedCurveCollections.end(),filename) == mLoadedCurveCollections.end())
        {
            bool isFirst = true;
            CurveCollectionPathToCallbackMap::iterator it = mCollectionPathToCallbackMap.find(filename);
            
            if(it != mCollectionPathToCallbackMap.end())
            {
                isFirst = false;
            }
            
            mCollectionPathToCallbackMap.insert(std::make_pair(filename, callback));
            
            // load file
            if(isFirst)
            {
                PropertyList plist;
                
                tempAnimationsToLoad = animationsToLoad;
                
				MemberCallbackPtr<CurveManager>::type memberCallback( new MemberCallback<CurveManager>(this, &CurveManager::_initCurveCollectionLoaded) );
				CallbackPtr callback_ = static_pointer_cast<Callback>(memberCallback);
                mFileManager->readFile(filename, callback_, plist, FileManager::PP_NoAbstraction);
            }
        }
        else
        {
            if(callback)
            {
                CurveCollectionLoadedCallbackParameters params(filename, true);
                callback->invoke(&params);
            }
        }
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void CurveManager::optimize()
    {
        CurvePtrMap::iterator it = mCurveMap.begin();
        CurvePtrMap::iterator itEnd = mCurveMap.end();
        
		
		while(it != itEnd) 
        {
			if (it->second.mCurve.unique()) 
			{
				CurvePtrMap::iterator toErase = it;
				++it;
				mCurveMap.erase(toErase);
			}
			else
			{
                ++it;
			}
		}
    }
	
	// -------------==============-------------==============-------------==============-------------===
	// <Summary>
	// removes all of the curves from the map that belong to this collection
	// </Summary>
	void CurveManager::unloadCurveCollection(const std::string& filename)
	{
		int collectionIndex = -1;
		unsigned int i = 0;
		std::vector<CurveCollection>::iterator collectionIter;
		for(collectionIter = mCurveCollections.begin(); collectionIter != mCurveCollections.end(); ++collectionIter, ++i)
		{
			if(filename == (*collectionIter).mCollectionName)
			{				
				// we've found the collection we're trying to unload, check to make sure it's loaded
				if((*collectionIter).mIsLoaded)
				{
					collectionIndex = i;
					break;
				}
			}
		}
		
		if(collectionIndex >= 0)
		{
			// the collection exists and has been loaded			
			// let's unload it by iterating the collection map
			CurvePtrMap::iterator it = mCurveMap.begin();
			CurvePtrMap::iterator itEnd = mCurveMap.end();
			
			while(it != itEnd)
			{
				if(collectionIndex == it->second.mCurveCollectionIndex)
				{
					// this curve belongs to the group that we are exercising, evict it
					CurvePtrMap::iterator toErase = it;
					++it;
					mCurveMap.erase(toErase);
				}
				else
				{
					++it;
				}
			}
			
			mCurveCollections.erase(collectionIter);
			
		}
		
	}
	
	// -------------==============-------------==============-------------==============-------------===
	void CurveManager::printMappedCurves()
	{
		CurvePtrMap::iterator it = mCurveMap.begin();
		CurvePtrMap::iterator itEnd = mCurveMap.end();
		
		while( it != itEnd )
		{
			Logger::printf("Walaber", Logger::SV_INFO, "Curve Name: [%s]\n", it->first.c_str() );
			
			++it;
		}
	}						   
    
    // -------------==============-------------==============-------------==============-------------===
    void CurveManager::_initCurveFileLoaded(void* params) //const std::string& path, char* buffer, size_t length, const std::string& actualPath, const std::string& handlerName, const PropertyList& userData )
    {
		FileManager::ReadFileCallbackParameters* rcp = (FileManager::ReadFileCallbackParameters*)params;
		
        CurvePtr curvePtr;
        
        if( rcp->result == ResultCode_OK )
        {
            
            CurvePtrMap::iterator it = mCurveMap.find(rcp->path);
            if(it != mCurveMap.end())
            {
                curvePtr = it->second.mCurve;
            }
            else
            {          
                xmlDocPtr curDoc;
                xmlNodePtr root;
                
                root = XML::loadDocMemory(rcp->buffer, rcp->length, curDoc);
                
                Curve* curve = Curve::loadFromXml(root);
                if(curve)
                {                    
                    curvePtr = CurvePtr(curve);                   
                }
                 
                XML::cleanupDoc(curDoc);                    
            }
        
            // delete the load buffer
            delete [] rcp->buffer;            
        }
        
        std::pair<PtrToCurvePtrMap::iterator, PtrToCurvePtrMap::iterator> ret;
        ret = mCurvePtrToUpdate.equal_range(rcp->path);
        bool setCurve = false;
        int i = 0;
        for(PtrToCurvePtrMap::iterator it_ = ret.first; it_ != ret.second; ++it_)
        {
            PtrCurvePtrCallback ptrcb = it_->second;
            
            // get the CurvePtr* from the struct
            CurvePtr* ptrCurvePtr = ptrcb.mPtrToCurvePtr;
            (*ptrCurvePtr) = curvePtr;
            
            if(!setCurve)
            {
                mCurveMap[rcp->path] = CurveMapEntry(curvePtr);
                setCurve = true;
            }
            
            // if there's a callback for loading, call it
            if(ptrcb.mCallback)
            {
                CurveLoadedCallbackParameters curveParameters(rcp->path, curvePtr, rcp->result);
                ptrcb.mCallback->invoke(&curveParameters);
            }
            
            ++i;
        }
        
        if(i)
        {
            // pull that ptr out of the cache since its been loaded
            mCurvePtrToUpdate.erase(ret.first, ret.second);
        }
    }
    
    // -------------==============-------------==============-------------==============-------------===
    // <Summary>
    // curve collection loaded callback
    // </Summary>
    void CurveManager::_initCurveCollectionLoaded(void* params) //const std::string& path, char* buffer, size_t length, const std::string& actualPath, const std::string& handlerName, const PropertyList& userData )
    {
		FileManager::ReadFileCallbackParameters* rcp = (FileManager::ReadFileCallbackParameters*)params;
		
        bool loadSuccess = false;
        
        if(rcp->buffer)
        {            
			int* magic = (int*)rcp->buffer;
			if( (*magic) == CURVE_COLLECTION_FILE_HEADER_MAGIC )
			{
                CurveCollectionFileHeader* header = (CurveCollectionFileHeader*)rcp->buffer;
                if( header->mVersion == 3 )
                {
                    loadSuccess = _loadV3Collection(rcp->path, rcp->buffer, rcp->length, tempAnimationsToLoad);
                }
                else if ( header->mVersion == 2 )
                {
                    loadSuccess = _loadV2Collection(rcp->path, rcp->buffer, rcp->length, tempAnimationsToLoad);
                }
                else
                {
                    loadSuccess = false;
                }
			}
			else if( (*magic) == CURVE_COLLECTION_FILE_HEADER_MAGIC_V1 )
			{
				loadSuccess = _loadV1Collection(rcp->path, rcp->buffer, rcp->length, tempAnimationsToLoad);
			}
            
            tempAnimationsToLoad.clear();
            delete [] rcp->buffer;
        }
        
        // notify any callbacks that this file load has finished
        std::pair<CurveCollectionPathToCallbackMap::iterator, CurveCollectionPathToCallbackMap::iterator> ret;
        ret = mCollectionPathToCallbackMap.equal_range(rcp->path);
        for(CurveCollectionPathToCallbackMap::iterator it = ret.first; it != ret.second; ++it)
        {
            if(it->second)
            {
                CurveCollectionLoadedCallbackParameters curveCollectionParameters(rcp->path, loadSuccess);
                it->second->invoke(&curveCollectionParameters);
            }
        }
        
        if(ret.first != ret.second)
        {
            mCollectionPathToCallbackMap.erase(ret.first, ret.second);            
        }
    }
    
	// -------------==============-------------==============-------------==============-------------===
	bool CurveManager::_loadV1Collection(const std::string& collectionName, char* bytes,
                                         unsigned int numBytes,
                                          const std::set< std::string >& animationsToLoad )
	{
		// load the curves
		CurveCollectionFileHeader_V1* fileHeader = (CurveCollectionFileHeader_V1*)bytes;
		
		// this is a curve collection file!!
		unsigned int offset = sizeof(CurveCollectionFileHeader_V1);
		
		mCurveCollections.push_back(CurveCollection());
		unsigned int curveCollectionIndex = mCurveCollections.size()-1;
		CurveCollection& collectionMapping = mCurveCollections[curveCollectionIndex];
		collectionMapping.mCollectionName = collectionName;
		
		for(unsigned int i=0; i < fileHeader->mNumCurves; ++i)
		{
			unsigned int stringSize;
			memcpy(&stringSize, &bytes[offset], sizeof(unsigned int));
			offset += sizeof(unsigned int);
			
			std::string curveName(&bytes[offset]);
			offset += stringSize;
            
            bool loadCurve = _shouldLoadCurve( animationsToLoad, curveName );
            
			CurveType type;
			memcpy(&type, &bytes[offset], sizeof(CurveType));
			offset += sizeof(CurveType);
			
            unsigned int bytesRead = 0;
			Curve* curve = createCurveForTypeFromBuffer( type, &bytes[offset], bytesRead );
			offset += bytesRead;
			
			if( loadCurve && curve)
            {
                CurvePtr curvePtr(curve);
                mCurveMap[curveName] = CurveMapEntry(curvePtr, curveCollectionIndex);
            }
            else
            {
                delete curve;
            }
		}
		
		assert (offset == numBytes); 
		
		
		collectionMapping.mIsLoaded = true;
		
		return true;
	}
	
	// -------------==============-------------==============-------------==============-------------===
	bool CurveManager::_loadV2Collection(const std::string& collectionName, char* bytes,
                                         unsigned int numBytes,
                                         const std::set< std::string >& animationsToLoad)
	{
		// load the curves
		CurveCollectionFileHeader_V2* fileHeader = (CurveCollectionFileHeader_V2*)bytes;
		
		// this is a curve collection file!!
		unsigned int offset = sizeof(CurveCollectionFileHeader_V2);
		
		mCurveCollections.push_back(CurveCollection());
		unsigned int curveCollectionIndex = mCurveCollections.size()-1;
		CurveCollection& collectionMapping = mCurveCollections[curveCollectionIndex];
		collectionMapping.mCollectionName = collectionName;
		
		// pull data
		if( fileHeader->mFlags & CCF_Skeletal_Animation_Collection )
		{
			// get the numbers of compression records
			unsigned int* numRecord = (unsigned int*)&bytes[offset];
			offset += sizeof(unsigned int);
			
			// get the compression records
			std::map< unsigned char, CompressionRecord > typeToRecordMap;
			CompressionRecord cr;
			
			for(unsigned int i = 0; i < (*numRecord); ++i)
			{
				unsigned char* recordType = (unsigned char*)&bytes[offset];
				offset += sizeof(unsigned char);
				
				memcpy(&cr, &bytes[offset], sizeof(CompressionRecord));
				offset += sizeof(CompressionRecord);
				
				if( !typeToRecordMap.insert( std::make_pair( (*recordType), cr) ).second )
				{
					// insert returned false, this key already exists!!
					Logger::printf("Walaber", Logger::SV_ERROR, "ERROR: Two compression records have the same id: [%d]\n", (*recordType));
					return false;
				}
			}
			
			for(unsigned int i=0; i < fileHeader->mNumCurves; ++i)
			{				
				unsigned int stringSize;
				memcpy(&stringSize, &bytes[offset], sizeof(unsigned int));
				offset += sizeof(unsigned int);
				
				std::string curveName(&bytes[offset]);
				offset += stringSize;
                
                bool loadCurve = _shouldLoadCurve( animationsToLoad, curveName );
				
				CurveType type = (CurveType)bytes[offset];
				offset += sizeof(unsigned char);
				
				AnimationCurveType aniType = (AnimationCurveType)bytes[offset];
				offset += sizeof(unsigned char);
				
				AnimationCurveCompressionTracks track;
				switch( aniType )
				{
					case AnimationCurveType_PosX:
						track = AnimationCurveCompressionTrack_POS_X;
						break;
						
					case AnimationCurveType_PosY:
						track = AnimationCurveCompressionTrack_POS_Y;
						break;
						
					case AnimationCurveType_AngleZ:
						track = AnimationCurveCompressionTrack_ANGLE_Z;
						break;
						
					default:
						assert(0);
						Logger::printf("Walaber", Logger::SV_WARN, "Curve type [%d] for curve [%s] cannot be used in a Skeletal Animation Collection!!\n", type, curveName.c_str());
						return false;
				}
				
				Curve* curve = 0;
				unsigned int bytesRead = 0;
				switch(type)
				{
					case CT_Hermite:
					{
						curve = new HermiteCurve( &bytes[offset], bytesRead, fileHeader->mFlags, typeToRecordMap[AnimationCurveCompressionTrack_TIME], typeToRecordMap[track] );
					}
						break;
						
					case CT_Bezier:
					{
						curve = new BezierCurve(&bytes[offset], bytesRead, fileHeader->mFlags, typeToRecordMap[AnimationCurveCompressionTrack_TIME], typeToRecordMap[track] );
					}
						break;
						
					case CT_Linear:
					{
						curve = new LinearCurve(&bytes[offset], bytesRead, fileHeader->mFlags, typeToRecordMap[AnimationCurveCompressionTrack_TIME], typeToRecordMap[track] );
					}
						break;
						
					case CT_TweenedLinear:
					{
						curve = new TweenedLinearCurve(&bytes[offset], bytesRead, fileHeader->mFlags, typeToRecordMap[AnimationCurveCompressionTrack_TIME], typeToRecordMap[track] );
					}
						break;
						
					default:
						// type came back FUBAR
						assert(0);
						return false;
				}
				offset += bytesRead;
				
				if( loadCurve && curve)
				{                    
					CurvePtr curvePtr(curve);                   
					mCurveMap[curveName] = CurveMapEntry(curvePtr, curveCollectionIndex);
				}
                else
                {
                    delete curve;
                }
			}
		}
		else if ( fileHeader->mFlags & CCF_1_Byte_Compression || fileHeader->mFlags & CCF_2_Byte_Compression )
		{				
			// get the compression records
			CompressionRecord timeCR;
			CompressionRecord valueCR;
			
			memcpy(&valueCR, &bytes[offset], sizeof(CompressionRecord));
			offset += sizeof(CompressionRecord);
			
			memcpy(&timeCR, &bytes[offset], sizeof(CompressionRecord));
			offset += sizeof(CompressionRecord);				
			
			
			for(unsigned int i=0; i < fileHeader->mNumCurves; ++i)
			{				
				unsigned int stringSize;
				memcpy(&stringSize, &bytes[offset], sizeof(unsigned int));
				offset += sizeof(unsigned int);
				
				std::string curveName(&bytes[offset]);
				offset += stringSize;
                
                bool loadCurve = _shouldLoadCurve( animationsToLoad, curveName );
				
				CurveType type = (CurveType)bytes[offset];
				offset += sizeof(unsigned char);
				
				// we don't need this value here so we don't read it, we just skip it
				//AnimationCurveType aniType = (AnimationCurveType)fileBytes[offset];
				offset += sizeof(unsigned char);
				
				Curve* curve = 0;
				unsigned int bytesRead = 0;
				switch(type)
				{
					case CT_Hermite:
					{
						curve = new HermiteCurve( &bytes[offset], bytesRead, fileHeader->mFlags, timeCR, valueCR );
					}
						break;
						
					case CT_Bezier:
					{
						curve = new BezierCurve(&bytes[offset], bytesRead, fileHeader->mFlags, timeCR, valueCR );
					}
						break;
						
					case CT_Linear:
					{
						curve = new LinearCurve(&bytes[offset], bytesRead, fileHeader->mFlags, timeCR, valueCR );
					}
						break;
						
					case CT_TweenedLinear:
					{
						curve = new TweenedLinearCurve(&bytes[offset], bytesRead, fileHeader->mFlags, timeCR, valueCR );
					}
						break;
						
					default:
						// type came back FUBAR
						assert(0);
						return false;
				}
				offset += bytesRead;
				
				if( loadCurve && curve)
				{
					CurvePtr curvePtr(curve);
					mCurveMap[curveName] = CurveMapEntry(curvePtr, curveCollectionIndex);
				}
                else
                {
                    delete curve;
                }
			}			
		}
		else
		{		
			for(unsigned int i=0; i < fileHeader->mNumCurves; ++i)
			{
				unsigned int stringSize;
				memcpy(&stringSize, &bytes[offset], sizeof(unsigned int));
				offset += sizeof(unsigned int);
				
				std::string curveName(&bytes[offset]);
				offset += stringSize;
                
                bool loadCurve = _shouldLoadCurve( animationsToLoad, curveName );
				
				CurveType type = (CurveType)bytes[offset];
				offset += sizeof(unsigned char);
				
				// we don't need this value here so we don't read it, we just skip it
				//AnimationCurveType aniType = (AnimationCurveType)fileBytes[offset];
				offset += sizeof(unsigned char);
				
				Curve* curve = 0;
				unsigned int bytesRead = 0;
				switch(type)
				{
					case CT_Hermite:
					{
						curve = new HermiteCurve(&bytes[offset], bytesRead);
					}
						break;
						
					case CT_Bezier:
					{
						curve = new BezierCurve(&bytes[offset], bytesRead);
					}
						break;
						
					case CT_Linear:
					{
						curve = new LinearCurve(&bytes[offset], bytesRead);
					}
						break;
						
					case CT_TweenedLinear:
					{
						curve = new TweenedLinearCurve(&bytes[offset], bytesRead);
					}
						break;
						
					default:
						// type came back FUBAR
						assert(0);
						return false;
				}
				offset += bytesRead;
				
				if( loadCurve && curve)
				{
					CurvePtr curvePtr(curve);
					mCurveMap[curveName] = CurveMapEntry(curvePtr, curveCollectionIndex);
				}
                else
                {
                    delete curve;
                }
			}
		}
		
		assert (offset == numBytes); 		
		
		collectionMapping.mIsLoaded = true;
		return true;
	}
	
    // -------------==============-------------==============-------------==============-------------===
	bool CurveManager::_loadV3Collection(const std::string& collectionName, char* bytes,
                                         unsigned int numBytes,
                                         const std::set< std::string >& animationsToLoad)
	{
		// load the curves
		CurveCollectionFileHeader* fileHeader = (CurveCollectionFileHeader*)bytes;
		
		// this is a curve collection file!!
		unsigned int offset = sizeof(CurveCollectionFileHeader);
		
		mCurveCollections.push_back(CurveCollection());
		unsigned int curveCollectionIndex = mCurveCollections.size()-1;
		CurveCollection& collectionMapping = mCurveCollections[curveCollectionIndex];
		collectionMapping.mCollectionName = collectionName;
		
		// pull data
		if( fileHeader->mFlags & CCF_Skeletal_Animation_Collection )
		{
			// get the numbers of compression records
			unsigned int* numRecord = (unsigned int*)&bytes[offset];
			offset += sizeof(unsigned int);
			
			// get the compression records
			std::map< unsigned char, CompressionRecord > typeToRecordMap;
			CompressionRecord cr;
			
			for(unsigned int i = 0; i < (*numRecord); ++i)
			{
				unsigned char* recordType = (unsigned char*)&bytes[offset];
				offset += sizeof(unsigned char);
				
				memcpy(&cr, &bytes[offset], sizeof(CompressionRecord));
				offset += sizeof(CompressionRecord);
				
				if( !typeToRecordMap.insert( std::make_pair( (*recordType), cr) ).second )
				{
					// insert returned false, this key already exists!!
					Logger::printf("Walaber", Logger::SV_ERROR, "ERROR: Two compression records have the same id: [%d]\n", (*recordType));
					return false;
				}
			}
            
            // TODO: make this algorith smarter for loading groups in groups based on
            //       animationsToLoad
            std::vector< std::string > parentGroupName;
            std::vector< unsigned int > groupsBeforePop;
            int numGroupsLeft = 1;
            do
            {
                unsigned int groupOffset = offset;
                unsigned int bytesRead;
                std::string groupName = readStringFromBuffer(&bytes[offset], bytesRead);
                offset += bytesRead;
                
                unsigned int groupSize;
                memcpy( &groupSize, &bytes[offset], sizeof(unsigned int ));
                offset += sizeof( unsigned int );
                
                unsigned int numCurves;
                memcpy( &numCurves, &bytes[offset], sizeof(unsigned int ));
                offset += sizeof( unsigned int );
                
                unsigned int numGroups;
                memcpy( &numGroups, &bytes[offset], sizeof(unsigned int ));
                offset += sizeof( unsigned int );
                
                if( !numCurves || _shouldLoadGroup( animationsToLoad, groupName ) )
                {
                    if( !groupsBeforePop.empty() )
                    {
                        if( groupsBeforePop.back() == 0 )
                        {
                            parentGroupName.pop_back();
                            groupsBeforePop.pop_back();
                        }
                        
                        --groupsBeforePop.back();
                    }
                    
                    parentGroupName.push_back( groupName );
                    
                    std::string finalName;
                    for( unsigned int i = 0; i < parentGroupName.size(); ++i )
                    {
                        finalName += parentGroupName[i];
                        finalName += "/";
                    }
                    
                    for( unsigned int i = 0; i < numCurves; ++i )
                    {
                        bytesRead = 0;
                        std::string curveName = readStringFromBuffer(&bytes[offset], bytesRead);
                        offset += bytesRead;
                        
                        CurveType type = (CurveType)bytes[offset];
                        offset += sizeof(unsigned char);
                        
                        AnimationCurveType aniType = (AnimationCurveType)bytes[offset];
                        offset += sizeof(unsigned char);
                        
                        AnimationCurveCompressionTracks track;
                        switch( aniType )
                        {
                            case AnimationCurveType_PosX:
                                track = AnimationCurveCompressionTrack_POS_X;
                                break;
                                
                            case AnimationCurveType_PosY:
                                track = AnimationCurveCompressionTrack_POS_Y;
                                break;
                                
                            case AnimationCurveType_AngleZ:
                                track = AnimationCurveCompressionTrack_ANGLE_Z;
                                break;
                                
                            default:
                                assert(0);
                                printf("Curve type [%d] for curve [%s] cannot be used in a Skeletal Animation Collection!!\n", type, curveName.c_str());
                        }
                        
                        std::string name = finalName + curveName;
                        bytesRead = 0;
                        Curve* curve = createCurveForTypeFromBuffer( type, fileHeader->mFlags,
                                                                     &bytes[offset],
                                                                     typeToRecordMap[AnimationCurveCompressionTrack_TIME],
                                                                     typeToRecordMap[track],
                                                                     bytesRead);
                        offset += bytesRead;
                        CurvePtr curvePtr(curve);
                        mCurveMap[name] = CurveMapEntry(curvePtr, curveCollectionIndex);
                    }
                    
                    groupsBeforePop.push_back( numGroups );
                    numGroupsLeft += numGroups;
                }
                else
                {
                    // skip this group
                    offset = groupOffset + groupSize;
                }
                
                --numGroupsLeft;
                
            } while (numGroupsLeft > 0 );
            
		}
		else if ( fileHeader->mFlags & CCF_1_Byte_Compression || fileHeader->mFlags & CCF_2_Byte_Compression )
		{
			// get the compression records
			CompressionRecord timeCR;
			CompressionRecord valueCR;
			
			memcpy(&valueCR, &bytes[offset], sizeof(CompressionRecord));
			offset += sizeof(CompressionRecord);
			
			memcpy(&timeCR, &bytes[offset], sizeof(CompressionRecord));
			offset += sizeof(CompressionRecord);
			
			
			std::vector< std::string > parentGroupName;
            std::vector< unsigned int > groupsBeforePop;
            int numGroupsLeft = 1;
            do
            {
                unsigned int groupOffset = offset;
                unsigned int bytesRead;
                std::string groupName = readStringFromBuffer(&bytes[offset], bytesRead);
                offset += bytesRead;
                
                unsigned int groupSize;
                memcpy( &groupSize, &bytes[offset], sizeof(unsigned int ));
                offset += sizeof( unsigned int );
                
                unsigned int numCurves;
                memcpy( &numCurves, &bytes[offset], sizeof(unsigned int ));
                offset += sizeof( unsigned int );
                
                unsigned int numGroups;
                memcpy( &numGroups, &bytes[offset], sizeof(unsigned int ));
                offset += sizeof( unsigned int );
                
                if( !numCurves || _shouldLoadGroup( animationsToLoad, groupName ) )
                {
                    if( !groupsBeforePop.empty() )
                    {
                        if( groupsBeforePop.back() == 0 )
                        {
                            parentGroupName.pop_back();
                            groupsBeforePop.pop_back();
                        }
                        
                        --groupsBeforePop.back();                        
                    }
                    
                    parentGroupName.push_back( groupName );
                    
                    std::string finalName;
                    for( unsigned int i = 0; i < parentGroupName.size(); ++i )
                    {
                        finalName += parentGroupName[i];
                        finalName += "/";
                    }
                    
                    for( unsigned int i = 0; i < numCurves; ++i )
                    {
                        bytesRead = 0;
                        std::string curveName = readStringFromBuffer(&bytes[offset], bytesRead);
                        offset += bytesRead;
                        
                        CurveType type = (CurveType)bytes[offset];
                        offset += sizeof(unsigned char);
                        
                        // Skip it, we dont need it
                        //AnimationCurveType aniType = (AnimationCurveType)bytes[offset];
                        offset += sizeof(unsigned char);
                        
                        std::string name = finalName + curveName;
                        bytesRead = 0;
                        Curve* curve = createCurveForTypeFromBuffer( type, fileHeader->mFlags,
                                                                    &bytes[offset],
                                                                    timeCR,
                                                                    valueCR,
                                                                    bytesRead);
                        offset += bytesRead;
                        CurvePtr curvePtr(curve);
                        mCurveMap[name] = CurveMapEntry(curvePtr, curveCollectionIndex);
                    }
                    
                    groupsBeforePop.push_back( numGroups );
                    numGroupsLeft += numGroups;
                }
                else
                {
                    // skip this group
                    offset = groupOffset + groupSize;
                }
                
                --numGroupsLeft;
                
            } while (numGroupsLeft > 0 );
		}
		else
		{
			std::vector< std::string > parentGroupName;
            std::vector< unsigned int > groupsBeforePop;
            int numGroupsLeft = 1;
            do
            {
                unsigned int groupOffset = offset;
                unsigned int bytesRead;
                std::string groupName = readStringFromBuffer(&bytes[offset], bytesRead);
                offset += bytesRead;
                
                unsigned int groupSize;
                memcpy( &groupSize, &bytes[offset], sizeof(unsigned int ));
                offset += sizeof( unsigned int );
                
                unsigned int numCurves;
                memcpy( &numCurves, &bytes[offset], sizeof(unsigned int ));
                offset += sizeof( unsigned int );
                
                unsigned int numGroups;
                memcpy( &numGroups, &bytes[offset], sizeof(unsigned int ));
                offset += sizeof( unsigned int );
                
                if( !numCurves || _shouldLoadGroup( animationsToLoad, groupName ) )
                {
                    if( !groupsBeforePop.empty() )
                    {
                        if( groupsBeforePop.back() == 0 )
                        {
                            parentGroupName.pop_back();
                            groupsBeforePop.pop_back();
                        }
                        
                        --groupsBeforePop.back();
                    }
                    
                    parentGroupName.push_back( groupName );
                    
                    std::string finalName;
                    for( unsigned int i = 0; i < parentGroupName.size(); ++i )
                    {
                        finalName += parentGroupName[i];
                        finalName += "/";
                    }
                    
                    for( unsigned int i = 0; i < numCurves; ++i )
                    {
                        bytesRead = 0;
                        std::string curveName = readStringFromBuffer(&bytes[offset], bytesRead);
                        offset += bytesRead;
                        
                        CurveType type = (CurveType)bytes[offset];
                        offset += sizeof(unsigned char);
                        
                        // SKip it, we odnt need it
                        //AnimationCurveType aniType = (AnimationCurveType)bytes[offset];
                        offset += sizeof(unsigned char);
                        
                        std::string name = finalName + curveName;
                        bytesRead = 0;
                        Curve* curve = createCurveForTypeFromBuffer( type,
                                                                     &bytes[offset],
                                                                     bytesRead);
                        offset += bytesRead;
                        CurvePtr curvePtr(curve);
                        mCurveMap[name] = CurveMapEntry(curvePtr, curveCollectionIndex);
                    }
                    
                    groupsBeforePop.push_back( numGroups );
                    numGroupsLeft += numGroups;
                }
                else
                {
                    // skip this group
                    offset = groupOffset + groupSize;
                }
                
                --numGroupsLeft;
                
            } while (numGroupsLeft > 0 );
		}
		
		assert (offset == numBytes);
		
		collectionMapping.mIsLoaded = true;
		return true;
	}

    
    // -------------==============-------------==============-------------==============-------------===

    bool CurveManager::_shouldLoadCurve( const std::set< std::string >& animationsToLoad,
                          const std::string& curveName )
    {
        bool ret = true;
        if ( !animationsToLoad.empty() )
        {
            // currently the only way we know which animation a curve belongs to is by the
            // name of the directory it is in
            std::string animationName = StringHelper::dirname( curveName );
            animationName = animationName.substr( animationName.rfind("/")+1 );
            
            if( animationsToLoad.find( animationName ) == animationsToLoad.end() )
            {
                // animation name doesn't exist in loading set
                ret = false;
            }
        }
        return ret;
    }
    
    // -------------==============-------------==============-------------==============-------------===
    
    bool CurveManager::_shouldLoadGroup( const std::set< std::string >& groupsToLoad,
                                         const std::string& groupName )
    {
        bool ret = true;
        if ( !groupsToLoad.empty() )
        {
            if( groupsToLoad.find( groupName ) == groupsToLoad.end() )
            {
                // animation name doesn't exist in loading set
                ret = false;
            }
        }
        return ret;
    }
}
