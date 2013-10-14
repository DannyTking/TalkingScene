//
//  SpriteCollectionBinary.h
//  WalaberEngine
//
//  Created by Pecho, Jason on 5/8/13.
//
//

#ifndef _SPRITE_COLLECTION_BINARY_H_
#define _SPRITE_COLLECTION_BINARY_H_

namespace Walaber
{
    namespace SpriteCollectionBinary
    {
        const static int MAGIC          =       0xC081DA00;
        const static int VERSION        =       1;
        
        struct SpriteCollectionFileHeader
        {
            const int               mMagic;
            unsigned short          mVersion;
            unsigned short          mFlags;
            unsigned int            mNumSprites;
            unsigned int            mNumGroups;
            
            SpriteCollectionFileHeader():
            mMagic( MAGIC ),
            mVersion( VERSION ),
            mFlags( 0 ),
            mNumSprites( 0 ),
            mNumGroups( 0 )
            {}
        };
        
    }// namespace SpriteCollection
}// namespace Walaber

#endif // defined( _SPRITE_COLLECTION_BINARY_H_ )
