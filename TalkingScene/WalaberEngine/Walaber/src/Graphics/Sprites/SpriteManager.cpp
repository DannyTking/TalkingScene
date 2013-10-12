//
//  SpriteManager.cpp
//  WalaberEngine
//
//  Created by Pecho, Jason on 5/8/13.
//
//

#include "SpriteManager.h"

#include <Sprite.h>
#include <BinaryReader.h>
#include <FileManager.h>
#include <SpriteCollectionBinary.h>
#include <MemberCallback.h>

#ifdef TARGET_NDK
#include <algorithm>
#endif

namespace Walaber
{
    SpriteManager::SpriteMapEntry::SpriteMapEntry():
    mSprite(),
    mSpriteCollectionIndex( -1 ),
    mIsLoaded( false )
    {}
    
    SpriteManager::SpriteMapEntry::SpriteMapEntry( SpritePtr sprite ):
    mSprite( sprite ),
    mSpriteCollectionIndex( -1 ),
    mIsLoaded( false )
    {}
    
    SpriteManager::SpriteMapEntry::SpriteMapEntry( SpritePtr sprite, const int index ):
    mSprite( sprite ),
    mSpriteCollectionIndex( index ),
    mIsLoaded( false )
    {}
    
    SpriteManager::~SpriteManager()
    {
        
    }
    
    //==============================================================================================
    
    void SpriteManager::loadSpriteCollection( const std::string& filename, CallbackPtr callback )
    {
        if(std::find( mLoadedCollections.begin(), mLoadedCollections.end(),filename) == mLoadedCollections.end())
        {
            bool isFirst = true;
            NameToCallbackMap::iterator it = mCallbackMap.find(filename);
            
            if(it != mCallbackMap.end())
            {
                isFirst = false;
            }
            
            if( callback )
            {
                mCallbackMap.insert(std::make_pair(filename, callback));
            }
            
            // load file
            if(isFirst)
            {
                PropertyList plist;
				
                CallbackPtr callbackPtr =
                    CreateMemberCallbackPtr( *this, &SpriteManager::_collectionLoaded );
                
                FileManager::getInstancePtr()->readFile(filename, callbackPtr, plist, FileManager::PP_NoAbstraction);
            }
        }
        else
        {
            if(callback)
            {
                SpriteCollectionLoadedCallbackParameters params(filename, ResultCode_OK);
                callback->invoke(&params);
            }
        }
    }
    
    //==============================================================================================
    
    void SpriteManager::unloadSpriteCollection( const std::string& filename )
    {
        int collectionIndex = -1;
		unsigned int i = 0;
		SpriteCollectionArray::iterator collectionIter;
		for(collectionIter = mLoadedCollections.begin(); collectionIter != mLoadedCollections.end(); ++collectionIter, ++i)
		{
			if(filename == (*collectionIter).mName)
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
			NameToSpriteMap::iterator it = mSpriteMap.begin();
			NameToSpriteMap::iterator itEnd = mSpriteMap.end();
			
			while(it != itEnd)
			{
				if(collectionIndex == it->second.mSpriteCollectionIndex)
				{
					// this curve belongs to the group that we are exercising, evict it
					NameToSpriteMap::iterator toErase = it;
					++it;
					mSpriteMap.erase(toErase);
				}
				else
				{
					++it;
				}
			}
			
			mLoadedCollections.erase(collectionIter);
			
		}
    }
    
    //==============================================================================================
    
    SpritePtr SpriteManager::getSprite( const std::string& filename, CallbackPtr callback )
    {
        SpritePtr sprite;
        
        NameToSpriteMap::iterator it = mSpriteMap.find( filename );
        if( it == mSpriteMap.end() )
        {
            Sprite* newSprite = new Sprite( "Sprite", Vector2::Zero, 0.0f, Vector2::One );
            
            sprite = SpritePtr( newSprite );

            mSpriteMap[ filename ] = SpriteMapEntry( sprite );

            CallbackPtr callbackPtr =
            CreateMemberCallbackPtr(*this, &SpriteManager::_spriteLoaded );
            
            newSprite->loadFromXML( filename, callbackPtr );
        }
        else
        {
            SpriteMapEntry& entry = it->second;
            if( !entry.mIsLoaded )
            {
                if( callback )
                {
                    mCallbackMap.insert(std::make_pair(filename, callback));
                }
            }
            else
            {
                sprite = entry.mSprite;
                
                // TODO: Don't let this pass back a raw Sprite pointer
                if( callback )
                {
                    Sprite::LoadedCallbackParameters params( entry.mSprite.get(), filename );
                    callback->invoke( &params );
                }
            }
        }
        
        return sprite;
    }
    
    //==============================================================================================

    void SpriteManager::optimize()
    {
        NameToSpriteMap::iterator it = mSpriteMap.begin();
        NameToSpriteMap::iterator itEnd = mSpriteMap.end();
        
		
		while(it != itEnd)
        {
			if (it->second.mSprite.unique())
			{
				NameToSpriteMap::iterator toErase = it;
				++it;
				mSpriteMap.erase(toErase);
			}
			else
			{
                ++it;
			}
		}
    }

    void SpriteManager::_collectionLoaded( void* data )
    {
        FileManager::ReadFileCallbackParameters* params = (FileManager::ReadFileCallbackParameters*)data;
		
        ResultCode result = params->result;
        
        if(result == ResultCode_OK)
        {
			int* magic = (int*)params->buffer;
			if( (*magic) == SpriteCollectionBinary::MAGIC )
			{
                SpriteCollectionBinary::SpriteCollectionFileHeader* header =
                    (SpriteCollectionBinary::SpriteCollectionFileHeader*)params->buffer;
                
                BasicBinaryReader reader( params->buffer, params->length );
                reader.seek( sizeof( SpriteCollectionBinary::SpriteCollectionFileHeader ),
                            BasicBinaryReader::SeekStart_Begin );
                if( header->mVersion == 1)
                {
                    mLoadedCollections.push_back( SpriteCollection() );
                    SpriteCollection& collectionMapping = mLoadedCollections.back();
                    collectionMapping.mName = params->path;
                    collectionMapping.mIsLoaded = false;
                    
                    _loadV1Collection( reader );
                    
                    collectionMapping.mIsLoaded = true;
                    
                    assert( reader.eob() );
                }
                else
                {
                    result = ResultCode_Invalid;
                }
			}
            
            delete [] params->buffer;
        }
        
        // notify any callbacks that this file load has finished
        std::pair<NameToCallbackMap::iterator, NameToCallbackMap::iterator> ret;
        ret = mCallbackMap.equal_range(params->path);
        for(NameToCallbackMap::iterator it = ret.first; it != ret.second; ++it)
        {
            if(it->second)
            {
                SpriteCollectionLoadedCallbackParameters callbackParams(params->path, result);
                it->second->invoke(&callbackParams);
            }
        }
        
        if(ret.first != ret.second)
        {
            mCallbackMap.erase(ret.first, ret.second);
        }
    }
    
    //==============================================================================================

    void SpriteManager::_spriteLoaded( void* data )
    {
        Sprite::LoadedCallbackParameters* params = (Sprite::LoadedCallbackParameters*)data;
        
        // notify any callbacks that this file load has finished
        std::pair<NameToCallbackMap::iterator, NameToCallbackMap::iterator> ret;
        ret = mCallbackMap.equal_range(params->path);
        for(NameToCallbackMap::iterator it = ret.first; it != ret.second; ++it)
        {
            if(it->second)
            {
                it->second->invoke( params );
            }
        }
        
        if(ret.first != ret.second)
        {
            mCallbackMap.erase(ret.first, ret.second);
        }
        
        if( params->result != ResultCode_OK )
        {
            mSpriteMap.erase( params->path );
        }
        else
        {
            mSpriteMap[ params->path ].mIsLoaded = true;
        }
    }
    
    //==============================================================================================

    void SpriteManager::_loadV1Collection( BasicBinaryReader& reader )
    {
        unsigned int collectionIndex = mLoadedCollections.size()-1;
        
        std::vector< std::string > parentGroupName;
        std::vector< unsigned int > groupsBeforePop;
        int numGroupsLeft = 1;
        do
        {
            //unsigned int groupOffset;
            //groupOffset = reader.getCurrentOffset();
            std::string groupName;
            reader.read( groupName );
            
            unsigned int groupSize;
            groupSize = reader.readUInt32();
            
            unsigned int numSprites = reader.readUInt32();
            
            unsigned int numGroups = reader.readUInt32();
            
            //if( !numSprites )//|| _shouldLoadGroup( animationsToLoad, groupName ) )
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
                
                for( unsigned int i = 0; i < numSprites; ++i )
                {
                    std::string spriteName;
                    reader.read( spriteName );
                    spriteName = finalName + spriteName;
                    
                    Sprite* sprite = new Sprite( "", Vector2::Zero, 0.0f, Vector2::One );
                    sprite->setPath( spriteName );
                    
                    unsigned int numAnimations = reader.readUInt32();
                    
                    for( unsigned int i = 0; i < numAnimations; ++i )
                    {
                        SpriteAnimation * anim = new SpriteAnimation();
                        anim->read( reader );
                        
                        sprite->addAnimation( SpriteAnimationPtr( anim ) );
                    }
                    
                    SpriteMapEntry entry( SpritePtr( sprite ), collectionIndex );
                    entry.mIsLoaded = true;
                    mSpriteMap[spriteName] = entry;
                }
                
                groupsBeforePop.push_back( numGroups );
                numGroupsLeft += numGroups;
            }
            //else
            //{
            //    // skip this group
            //    reader.seek( groupOffset + groupSize, BasicBinaryReader::SeekStart_Begin );
            //}
            
            --numGroupsLeft;
            
        } while (numGroupsLeft > 0 );
    }
}
