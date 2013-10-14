//
//  NumericAnimationTrack.cpp
//  WalaberEngine
//
//  Created by Jason Pecho on 5/4/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "NumericAnimationTrack.h"

#include <CurveManager.h>
#include <Animation.h>
#include <MemberCallback.h>

namespace Walaber
{
    // -------------==============-------------==============-------------==============-------------===
    NumericAnimationTrack::NumericAnimationTrack() : 
	mAnimation(0)
    {}
    
    // -------------==============-------------==============-------------==============-------------===
    NumericAnimationTrack::NumericAnimationTrack(CurvePtr curve, float* numericValue):
    mCurve(curve),
	mAnimation(0)
    {
        setNumericValue(numericValue);
    }
    
    // -------------==============-------------==============-------------==============-------------===
    NumericAnimationTrack::NumericAnimationTrack(CurvePtr curve, int* numericValue):
    mCurve(curve),
	mAnimation(0)
    {
        setNumericValue(numericValue);
    }
    
    // -------------==============-------------==============-------------==============-------------===
    NumericAnimationTrack::NumericAnimationTrack(const NumericAnimationTrack& numericAnimationTrack):
    mCurve(numericAnimationTrack.mCurve),
	mAnimation(0)
    {
        if(numericAnimationTrack.mNumericValue.mNumericValueType == NVT_Float)
        {
            setNumericValue(numericAnimationTrack.mNumericValue.mFloatPtr);
        }
        else if(numericAnimationTrack.mNumericValue.mNumericValueType == NVT_Int)
        {
            setNumericValue(numericAnimationTrack.mNumericValue.mIntPtr);
        }
    }
    
    // -------------==============-------------==============-------------==============-------------===
    NumericAnimationTrack& NumericAnimationTrack::operator=(const NumericAnimationTrack& numericAnimationTrack)
    {
        mCurve = numericAnimationTrack.mCurve;
        
        if(numericAnimationTrack.mNumericValue.mNumericValueType == NVT_Float)
        {
            setNumericValue(numericAnimationTrack.mNumericValue.mFloatPtr);
        }
        else if(numericAnimationTrack.mNumericValue.mNumericValueType == NVT_Int)
        {
            setNumericValue(numericAnimationTrack.mNumericValue.mIntPtr);
        }
        return *this;
    }
    
    // -------------==============-------------==============-------------==============-------------===
    NumericAnimationTrack::~NumericAnimationTrack()
    {        
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void NumericAnimationTrack::setAnimation(Animation* animation)
    {
        mAnimation = animation;
    }
    
	// -------------==============-------------==============-------------==============-------------===
	// <Summary>
	// laod curve through the curve manager..
	// </Summary>
	void NumericAnimationTrack::loadCurve(const std::string& curve_name)
	{
        MemberCallbackPtr<NumericAnimationTrack>::type memcallback(new MemberCallback<NumericAnimationTrack>(this, &NumericAnimationTrack::_curveFinishedLoading));
        CallbackPtr callback = static_pointer_cast<Callback>(memcallback);
        
		CurveManager::getManager().getCurve(&mCurve, curve_name, callback);	
	}
	
    // -------------==============-------------==============-------------==============-------------===
    void NumericAnimationTrack::apply(const AnimationBlendMode blendMode, const float time, const float weight, const float scale, bool isScreenSpace)
    {
        float trackValue = mCurve->evaluate(time) * (weight * scale);
        int mult = (1 ^ -isScreenSpace) + isScreenSpace;
        
        switch(blendMode)
        {
            case ABM_Default:
            {
                switch(mNumericValue.mNumericValueType)
                {
                    case NVT_Float:
                        
                        (*mNumericValue.mFloatPtr) = (float)mult*trackValue;
                        
                        break;
                        
                    case NVT_Int:
                        
                        (*mNumericValue.mIntPtr) = mult*(int)trackValue;
                        
                        break;
                        
                    default:
                        
                        // numeric value has no type!    
                        assert(0);
                        break;
                }
            }
                break;
                
            case ABM_Additive:
            {
                switch(mNumericValue.mNumericValueType)
                {
                    case NVT_Float:
                        
                        (*mNumericValue.mFloatPtr) += (float)mult*trackValue;
                        
                        break;
                        
                    case NVT_Int:
                        
                        (*mNumericValue.mIntPtr) += mult*(int)trackValue;
                        
                        break;
                        
                    default:
                        
                        // numeric value has no type!    
                        assert(0);
                        break;
                }
            }
                break;
            
            default:
                
                // illegal blend mode
                assert(0);
                break;               
        }
    }
	
	// -------------==============-------------==============-------------==============-------------===
    void NumericAnimationTrack::_curveFinishedLoading(void* data)
    {
        //CurveManager::CurveLoadedCallbackParameters* params = (CurveManager::CurveLoadedCallbackParameters*)data;
        
        if(mAnimation)
        {
            mAnimation->notifyTrackLength(getTrackLength());
        }
    }
}