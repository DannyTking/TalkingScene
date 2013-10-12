/*
 *  BinaryWriter.h
 *  Serialization
 *
 *  Created by jpecho on 05/08/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _BINARY_WRITER_H_
#define _BINARY_WRITER_H_

#include <cstring>
#include <cstdlib>
#include <string>
#include <cassert>
#include <WalaberMath.h>

namespace Walaber
{
	
    class BasicBinaryWriter
    {
    public:
        enum SeekStart
        {
            SeekStart_Begin,
            SeekStart_Current,
            SeekStart_End
        };
        
    public:
        BasicBinaryWriter():
        mBuffer( new char[1024] ),
        mBufferSize(1024),
        mCurrentOffset(0),
        mEndOffset(0)
        {
            
        }
        
        BasicBinaryWriter( unsigned long long bufferSize):
        mBuffer( new char[bufferSize] ),
        mBufferSize( bufferSize ),
        mCurrentOffset(0),
        mEndOffset(0)
        {
            
        }
        
        ~BasicBinaryWriter()
        {
            if( mBuffer )
            {
                delete [] mBuffer;
            }
        }
        
        template <typename T>
        inline void write(const T& value)
        {
            // make sure we haave enough space
            _checkAndResizeBuffer( static_cast<unsigned long long>( sizeof(T) ) );
            
            memcpy( &mBuffer[mCurrentOffset], &value, sizeof(T) );
            mCurrentOffset += sizeof(T);
            
            mEndOffset = maxf( mCurrentOffset, mEndOffset );
        }
        
        inline void write(const std::string& string)
        {
            unsigned int stringSize = string.size()+1; // +1 to include null terminator
            if(stringSize != 0)
            {
                stringSize = (((stringSize+4)-1)/4)*4; // round to the nearest multiple of 4
                
            }
            
            write( stringSize );
            
            // make sure we have enough space
            _checkAndResizeBuffer( static_cast<unsigned long long>(stringSize) );
            
            unsigned int stringLength = string.size();
            memcpy( &mBuffer[mCurrentOffset], string.c_str(), stringLength+1);
            mCurrentOffset += stringLength+1;
            
            unsigned int filler = stringSize - (stringLength + 1);
            memset( &mBuffer[mCurrentOffset], 0, filler);
            mCurrentOffset += filler;
            
            mEndOffset = maxf( mCurrentOffset, mEndOffset );
        }
        
        unsigned long long getCurrentOffset()const
        {
            return mCurrentOffset;
        }
        
        unsigned long long getUsedBufferSize()const
        {
            return mEndOffset;
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
                    assert( offset >= 0  );
                    if( offset > mBufferSize )
                    {
                        _doubleBuffer();
                    }
                    
                    mCurrentOffset = offset;

                    break;
                    
                case SeekStart_Current:
                    assert( offset + mCurrentOffset >= 0 );
                    _checkAndResizeBuffer( offset );
                    mCurrentOffset += offset;
                    break;
                    
                case SeekStart_End:
                    assert( offset <= 0 && ( mEndOffset+offset >= 0 ) );
                    mCurrentOffset = ( mEndOffset + offset );
                    break;
            }
        }
        
    private:
        
        void _checkAndResizeBuffer( unsigned long long nextWriteSize )
        {
            if( mCurrentOffset + nextWriteSize > mBufferSize )
            {
                _doubleBuffer();
            }
        }
        
        void _doubleBuffer()
        {
            mBufferSize *= 2;
            char * temp = new char[mBufferSize];
            memcpy( temp, mBuffer, mCurrentOffset );
            
            // delete old buffer
            delete [] mBuffer;
            
            mBuffer = temp;
        }
        
        char*                       mBuffer;
        unsigned long long          mBufferSize;
        unsigned long long          mCurrentOffset;
        unsigned long long          mEndOffset;
    };

}


#endif	// _BINARY_WRITER_H_
