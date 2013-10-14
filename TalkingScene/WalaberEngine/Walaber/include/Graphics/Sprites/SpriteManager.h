//
//  SpriteManager.h
//  WalaberEngine
//
//  Created by Pecho, Jason on 5/8/13.
//
//

#ifndef _SPRITE_MANAGER_H_
#define _SPRITE_MANAGER_H_

#include <map>
#include <vector>
#include <string>

#include <macros.h>
#include <ResultCodes.h>
#include <Callback.h>

namespace Walaber
{
    class BasicBinaryReader;
    class Sprite;
    MAKE_SHARED_PTR( Sprite );
    
    /*! 
        SpriteManager can load in sprite collections and be used to keep sprites laoded in memory 
        across screen boundaries
    */
    class SpriteManager
    {
    public: // enums, structs, and typedefs

        /*
            Parameters to pass back a callbacfk that would like to be notified whether or not
            a SpriteCollection loaded
        */
        struct SpriteCollectionLoadedCallbackParameters
        {
            std::string     name;
            ResultCode      result;
            
            SpriteCollectionLoadedCallbackParameters():
            name(),
            result( ResultCode_Error )
            {}
            
            SpriteCollectionLoadedCallbackParameters( const std::string& pname,
                                                      ResultCode presult ):
            name( pname ),
            result( presult )
            {}
        };
        
    private: // enums, structs, and typedefs
        /* 
            holds the name cnae whether or not a collection is loaded
        */
        struct SpriteCollection
        {
            std::string     mName;
            bool            mIsLoaded;
            
            SpriteCollection():
            mName(),
            mIsLoaded(false)
            {}
            
            bool operator == (const std::string& collection) const
            {
                return (this->mName == collection);
            }
        };
        
        /*
            entry in the map for a sprite, so we can keep track of what sprites belong to what 
            collections
        */
        struct SpriteMapEntry
        {
            SpritePtr   mSprite;
            int         mSpriteCollectionIndex;
            bool        mIsLoaded;
            
            SpriteMapEntry();
            SpriteMapEntry( SpritePtr sprite );
            SpriteMapEntry( SpritePtr sprite, const int index );
        };
        
        ///< A set of strings, used for keeping track of loaded collections
        typedef std::vector< SpriteCollection >                 SpriteCollectionArray;
        
        ///< The name of a sprite will be it's path for now
        typedef std::map< std::string, SpriteMapEntry >         NameToSpriteMap;
        
        ///< The name of the sprite collection or sprite to be loaded to a callback ptr to
        //   notify once loaded
        typedef std::multimap< std::string, CallbackPtr >       NameToCallbackMap;
        
    public: // interface
        
        /*
            Singleton - get the pointer to the instance
        */
        static SpriteManager* getInstancePtr()
        {
            static SpriteManager instance;
            
            return &instance;
        }
        
        /*
            Destructor
        */
        ~SpriteManager();
        
        /*
            Load the sprite collection into memory
            @param filename relative path to the binary sprite file
            @param callback pointer to the function to call when the collection finishes loading
        */
        void loadSpriteCollection( const std::string& filename,
                                   CallbackPtr callback = CallbackPtr() );
        
        /*
         Load the sprite collection into memory
         @param filename relative path to the binary sprite file
         @param callback pointer to the function to call when the collection finishes loading
         */
        void unloadSpriteCollection( const std::string& filename );
        
        /* 
            Get sprite by filename, will attempt to load sprites that aren't loaded
            @param filename the relative filepath for the spirite file, or in the case of a sprite
                   in a collection, the name of that sprite in the collection
            @param callback function to call once sprite has finished loading
        */            
        SpritePtr getSprite( const std::string& filename, CallbackPtr callback = CallbackPtr() );
        
        /*
            Clears any sprite with only a reference count of 1 (it's not being used anywhere)
        */
        void optimize();
        
    private: // interface

        /*!
            Default ctor, private because of singleton
        */
        SpriteManager()
        {}
        
        /*
            Callback for loading in the sprite binary
        */
        void _collectionLoaded( void* data );
        
        /*
            Callback for when a sprite loads
        */
        void _spriteLoaded( void* data );
        
        /* 
            Load a v1 sprite collection
            @param reader a binary read that has already been initialized with a buffer
        */
        void _loadV1Collection( BasicBinaryReader& reader );
        
    private: // members
        
        ///< map of a sprite name ( probably the relative path to the sprite ) to a SpritePtr
        NameToSpriteMap                                     mSpriteMap;
        
        ///< mapping the name of the sprite collection to any numbe rof callbacks
        NameToCallbackMap                                   mCallbackMap;
        
        ///< set of names of loaded collections
        SpriteCollectionArray                               mLoadedCollections;

    };
}

#endif /* defined(_SPRITE_MANAGER_H_) */