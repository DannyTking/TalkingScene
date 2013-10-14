/*
 *  BinaryReader.h
 *  Serialization
 *
 *  Created by jpecho on 05/09/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _BINARY_READER_H_
#define _BINARY_READER_H_

#include <cstring>
#include <cstdlib>
#include <string>
#include <cassert>

namespace Walaber
{
	
    class BasicBinaryReader
    {
    public:
        enum SeekStart
        {
            SeekStart_Begin,
            SeekStart_Current,
            SeekStart_End
        };
        
    public:
               
        BasicBinaryReader( char* buffer, unsigned long long bufferSize):
        mBuffer( buffer ),
        mBufferSize( bufferSize ),
        mCurrentOffset(0)
        {
            
        }
        
        ~BasicBinaryReader()
        {
        }
        
        template <typename T>
        inline void read( T& value )
        {
            assert( mCurrentOffset < mBufferSize );
            memcpy( &value, &mBuffer[mCurrentOffset], sizeof(T) );
            mCurrentOffset += sizeof(T);
            assert( mCurrentOffset <= mBufferSize );
        }
        
        inline void read( std::string& string )
        {
            assert( mCurrentOffset < mBufferSize );
            
            unsigned int stringSize;
            memcpy(&stringSize, &mBuffer[mCurrentOffset], sizeof(unsigned int));
            mCurrentOffset += sizeof(unsigned int);
            
            char * readString = &mBuffer[mCurrentOffset];
            mCurrentOffset += stringSize;
        
            assert( mCurrentOffset <= mBufferSize );
        
            if( *readString != '\0' )
            {
                // copy the string
                string = std::string( readString );
            }
        }
        
        inline int readInt32()
        {
            assert( mCurrentOffset < mBufferSize );
            
            int readVar;
            read( readVar );
            
            assert( mCurrentOffset <= mBufferSize );
            
            return readVar;
        }
        
        inline int readUInt32()
        {
            assert( mCurrentOffset < mBufferSize );
            
            unsigned int readVar;
            read( readVar );
            
            assert( mCurrentOffset <= mBufferSize );
            
            return readVar;
        }
        
        inline short readInt16()
        {
            assert( mCurrentOffset < mBufferSize );
            
            short readVar;
            read( readVar );
            
            assert( mCurrentOffset <= mBufferSize );
            
            return readVar;
        }
        
        inline unsigned short readUInt16()
        {
            assert( mCurrentOffset < mBufferSize );
            
            unsigned short readVar;
            read( readVar );
            
            assert( mCurrentOffset <= mBufferSize );
            
            return readVar;
        }
        
        inline char readInt8()
        {
            assert( mCurrentOffset < mBufferSize );
            
            char readVar;
            read( readVar );
            
            assert( mCurrentOffset <= mBufferSize );
            
            return readVar;
        }
        
        inline unsigned char readUInt8()
        {
            assert( mCurrentOffset < mBufferSize );
            
            unsigned char readVar;
            read( readVar );
            
            assert( mCurrentOffset <= mBufferSize );
            
            return readVar;
        }
        
        inline float readFloat()
        {
            assert( mCurrentOffset < mBufferSize );
            
            float readVar;
            read( readVar );
            
            assert( mCurrentOffset <= mBufferSize );
            
            return readVar;
        }
        
        inline std::string readString()
        {
            assert( mCurrentOffset < mBufferSize );
            
            std::string readVar;
            read( readVar );
            
            assert( mCurrentOffset <= mBufferSize );
            
            return readVar; 
        }
        
        unsigned long long getCurrentOffset()const
        {
            return mCurrentOffset;
        }
        
        unsigned long long getBufferSize()const
        {
            return mBufferSize;
        }
        
        const char* const getBuffer()const
        {
            return mBuffer;
        }
        
        inline void seek( long long offset, SeekStart seekStart )
        {
            switch ( seekStart )
            {
                case SeekStart_Begin:
                    assert( offset >= 0 && offset < mBufferSize );
                    mCurrentOffset = offset;
                    break;
                    
                case SeekStart_Current:
                    assert( offset + mCurrentOffset >= 0 && offset + mCurrentOffset <= mBufferSize );
                    mCurrentOffset += offset;
                    break;
                    
                case SeekStart_End:
                    assert( offset <= 0 && ( mBufferSize+offset >= 0 ) );
                    mCurrentOffset += offset;
                    break;
            }
        }
        
        bool eob()
        {
            return mCurrentOffset == mBufferSize;
        }
        
    private:
        
        BasicBinaryReader():
        mBuffer( 0 ),
        mBufferSize( 0 ),
        mCurrentOffset( 0 )
        {
            
        }
        
        
        char*                       mBuffer;
        unsigned long long          mBufferSize;
        unsigned long long          mCurrentOffset;
    };

}


#endif	// _BINARY_READER_H_
