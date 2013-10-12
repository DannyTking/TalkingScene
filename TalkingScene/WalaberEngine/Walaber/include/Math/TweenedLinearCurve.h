//
//  TweenedLinearCurve.h
//  WalaberEngine
//
//  Created by Jason Pecho on 5/13/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef _TWEENED_LINEAR_CURVE_H_
#define _TWEENED_LINEAR_CURVE_H_

#include <LinearCurve.h>

#include <DynamicArray.h>

#include <Tweens.h>

#ifdef TARGET_NDK
#include <algorithm>
#endif // #ifdef TARGET_NDK

namespace Walaber
{
    class TweenedLinearCurve : public LinearCurve
    {
    public: // typedefs, structs, and enums
        
        struct KeyToKeyDuration
        {
            float valueChange;
            float duration;
        };
        
    public: // interface
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // default ctor
        // </Summary>
        TweenedLinearCurve():
        LinearCurve()
        {}
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Load a curve from a buffer that was loaded from a curve collection
        // </Summary>
        TweenedLinearCurve(char* buf, unsigned int& bytesRead):
        LinearCurve(buf, bytesRead),
		mKeyDurations(buf, bytesRead)
        {
            unsigned int size = mKeys.size();
	
            mTweeningFunctions.reserve(size);
            for(unsigned int i=0; i < size; ++i)
            {
                Tweens::Type tweenType = (Tweens::Type)buf[bytesRead];
                mTweeningFunctions.push_back(Tweens::getEasingFunctionForTweenType(tweenType));
                bytesRead += sizeof(char);
            }
        }
		
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // DO NOT USE THIS CTOR!! THIS IS SOLELY TO SUPPORT A LEGACY FORMAT!!!
        // </Summary>
        TweenedLinearCurve(char* buf, unsigned int& bytesRead, bool legacy):
        LinearCurve(buf, bytesRead)
        {
            unsigned int size = mKeys.size();
			
			mKeyDurations.reserve(size);
			for(unsigned int i=0; i < size; ++i)
			{
				mKeyDurations.push_back(KeyToKeyDuration());
				memcpy(&mKeyDurations[mKeyDurations.size()-1], &buf[bytesRead], sizeof(KeyToKeyDuration));                
				bytesRead += sizeof(KeyToKeyDuration);
			}
			
            mTweeningFunctions.reserve(size);
            for(unsigned int i=0; i < size; ++i)
            {
                Tweens::Type tweenType;
                memcpy(&tweenType, &buf[bytesRead], sizeof(Tweens::Type));
                mTweeningFunctions.push_back(Tweens::getEasingFunctionForTweenType(tweenType));
                bytesRead += sizeof(Tweens::Type);
            }
        }
        
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Load a curve from a buffer that was loaded from a curve collection with flags
        // </Summary>
		TweenedLinearCurve(char* buffer, unsigned int& bytesRead, unsigned int flags, const CompressionRecord& timeCR, const CompressionRecord& valueCR, float keyDifferenceToIgnore = 0.0f ):
		LinearCurve()
        {
            std::vector< int > ignoredKeyIndices;
            _loadCompressedKeys( buffer,
                                 bytesRead,
                                 flags,
                                 timeCR,
                                 valueCR,
                                 keyDifferenceToIgnore,
                                 ignoredKeyIndices );
            
			unsigned int size = mKeys.size();
            
            int currentTweenIndexFromOriginal=0;
            
			for(unsigned int i=0; i < size; ++i)
			{
                if(std::find(ignoredKeyIndices.begin(),ignoredKeyIndices.end(),currentTweenIndexFromOriginal)!=ignoredKeyIndices.end())
                {
                    --i;
                }
                else
                {
                    Tweens::Type tweenType = (Tweens::Type)buffer[bytesRead];
                    mTweeningFunctions.push_back(Tweens::getEasingFunctionForTweenType(tweenType));
                }
                
                bytesRead += sizeof(unsigned char);
                currentTweenIndexFromOriginal++;
			}
            
            int remaining=mKeys.size()+ignoredKeyIndices.size()-currentTweenIndexFromOriginal;
            for(unsigned int i=0; i<remaining; ++i)
            {
                bytesRead+=sizeof(unsigned char);
            }
			
			_computeDurations();
		}
		
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // dtor
        // </Summary>
        ~TweenedLinearCurve()
        {}
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // implementation of base class load from XML node
        // </Summary>
        void loadFromXmlNode(xmlNodePtr root, float keyDifferenceToIgnore = 0.0f);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // function gives the size this curve takes up in our own binary format
        // </Summary>
        virtual size_t getWriteSize(unsigned int flags)
        {
            size_t size = Curve::getWriteSize(flags);
			
			if(!flags)
			{
				size += mKeyDurations.getWriteSize();
			}			
			
			// There is way less than 255 tweens, so lets cram it into a char
			size += mTweeningFunctions.size();
			
            return size;
        }
		
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Writes the data of a curve object into the buffer - returns the number of bytes written
        // </Summary>
        virtual unsigned int writeToBuffer(char* buf)
        {
            unsigned int bytesWritten = Curve::writeToBuffer(buf);
            
            // write key data
            bytesWritten += mKeyDurations.writeToBuffer(&buf[bytesWritten]);
            
            unsigned int size = mTweeningFunctions.size();
            for(unsigned int i = 0; i < size; ++i)
            {
                Tweens::Type tweenType = Tweens::getTweenTypeForEasingFunction(mTweeningFunctions[i]);
				unsigned char charTween = (unsigned char) tweenType;
                memcpy(&buf[bytesWritten], &charTween, sizeof(charTween));
                bytesWritten += sizeof(charTween);
            }           
            
            return bytesWritten;
        }
		
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Writes the data of a curve object into the buffer based on the flags passed in - returns the number of bytes written
        // </Summary>
        virtual unsigned int writeToBuffer(char* buf, const unsigned int flags, const CompressionRecord& timeCR, const CompressionRecord& valueCR)
        {
			unsigned int bytesWritten = 0;
			if( !flags )
			{
				bytesWritten = writeToBuffer(buf);
			}
			else
			{
				bytesWritten = Curve::writeToBuffer(buf, flags, timeCR, valueCR);
				
				// write the tween types out as chars
				unsigned int size = mTweeningFunctions.size();
				for(unsigned int i = 0; i < size; ++i)
				{
					unsigned char tween = (unsigned char)Tweens::getTweenTypeForEasingFunction(mTweeningFunctions[i]);
					memcpy(&buf[bytesWritten], &tween, sizeof(tween));
					bytesWritten += sizeof(tween);
				}
			}
			
			return bytesWritten;
		}
        
    private: // interface
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // actually evaluate the hermite curve at the position
        // </Summary>
        float _evaluateCurve(const float position, const float offset) const; 
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // computes the durations and changes between keys
        // </Summary>
        void _computeDurations(); 
        
        
    private: // members
        
        // Key to Key Durations are 1 to 1 with Keys so mKeyDurations[0] is the duration from
        // mKey[0] to mKey[1]
        DynamicArray< KeyToKeyDuration >       mKeyDurations;
        
        // Tweening functions are 1 to 1 with Keys, so mTweeningFunctions[0] is the tweening function
        // for mKey[0] to mKey[1]
        DynamicArray< EasingFunction >         mTweeningFunctions;
    };
}

#endif
