//
//  AnimationSharedDefinitions.h
//  WalaberEngine
//
//  Created by Jason Pecho on 4/22/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef _ANIMATION_SHARED_DEFINITIONS_H_
#define _ANIMATION_SHARED_DEFINITIONS_H_

namespace Walaber
{
    // -------------==============-------------==============-------------==============-------------===
    enum AnimationPlaybackMode
    {
        PM_Once,
        PM_Loops,
        PM_PingPongs
    };
    
    // -------------==============-------------==============-------------==============-------------===
    enum NumericValueType
    {
        NVT_Float,
        NVT_Int,
    };
    
    // -------------==============-------------==============-------------==============-------------===
    struct PtrToNumericValue
    {
        NumericValueType mNumericValueType;
        
        union
        {
            float*      mFloatPtr;
            int*        mIntPtr;
        };
    };
    
    // -------------------------------------------------------------------------------------------
    enum AnimationBlendMode
    {
        ABM_None, // no blend mode, set node values absolutely
        ABM_Additive, // 
        ABM_Default = ABM_None,
    };

    // -------------------------------------------------------------------------------------------
    enum AnimationEventType
    {
        AET_None        = 0,            // none
        AET_Began       = 1,            // animation started playing
        AET_Paused      = 2,            // animation was paused
        AET_Stopped     = 4,            // animation was stopped
        AET_Looped      = 8,            // animation has looped
        AET_Finished    = 16,           // animation finished playing
        AET_Marker      = 32,           // playback head has passed a custom marker
        AET_DWORD       = 0xFFFFFFFF    // make sure the sizeof AnimationEventType is 4 bytes
    };
    
    class Animation;
    struct AnimationCallbackParameters
    {
        Animation* anim;
        int eventID;
        AnimationEventType eType;
    };
}

#endif