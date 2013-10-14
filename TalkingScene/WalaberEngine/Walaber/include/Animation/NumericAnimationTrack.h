//
//  NumericAnimationTrack.h
//  WalaberEngine
//
//  Created by Jason Pecho on 5/4/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef _NUMERIC_ANIMATION_TRACK_H_
#define _NUMERIC_ANIMATION_TRACK_H_

#include <AnimationSharedDefinitions.h>

#include <Curve.h>

namespace Walaber
{
    class Animation;
    
    class NumericAnimationTrack 
    {
    public:
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Default Ctor
        // </Summary>
        NumericAnimationTrack();
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set curve and float value Ctor
        // </Summary>
        NumericAnimationTrack(CurvePtr curve, float* numericValue);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set curve and float value Ctor
        // </Summary>
        NumericAnimationTrack(CurvePtr curve, int* numericValue);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Copy Ctor
        // </Summary>
        NumericAnimationTrack(const NumericAnimationTrack& numericAnimationTrack);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // operator=
        // </Summary>
        NumericAnimationTrack& operator=(const NumericAnimationTrack& numericAnimationTrack);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Dtor
        // </Summary>
        ~NumericAnimationTrack();
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set curves for local pos
        // </Summary>
        void setAnimation(Animation* animation);
       
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Track length virtual overload
        // </Summary>
        float getTrackLength()const
        {
#if _DEBUG
            if(mCurve.use_count())
            {
#endif
                return mCurve->getLastTime();
#if _DEBUG
            }
            else
            {
                // get time on a track with no curve!!!
                assert(0);
                return 0.0f;
            }
#endif
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Set the curve for the track
        // </Summary>
        void setCurve(CurvePtr curve)
        {
            mCurve = curve;
        }
		
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // laod curve through the curve manager..
        // </Summary>
        void loadCurve(const std::string& curve_name);
        
    
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Set numeric value to a float
        // </Summary>
        void setNumericValue(float* numericValue)
        {
            mNumericValue.mNumericValueType = NVT_Float;
            mNumericValue.mFloatPtr = numericValue;
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Set numeric value to an int
        // </Summary>
        void setNumericValue(int* numericValue)
        {
            mNumericValue.mNumericValueType = NVT_Int;
            mNumericValue.mIntPtr = numericValue;
        }        
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // apply the track to its numeric value
        // </Summary>
        void apply(const AnimationBlendMode blendMode, const float time, const float weight = 1.0f, const float scale = 1.0f, bool isScreenSpace = false);
        
    public: //callbacks
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // called when a curve is finished loading
        // </Summary>
        void _curveFinishedLoading(void* data);
        
        
    private: // members        
        
        CurvePtr                mCurve;
        PtrToNumericValue       mNumericValue;
        
        Animation*              mAnimation;        
    };
}

#endif