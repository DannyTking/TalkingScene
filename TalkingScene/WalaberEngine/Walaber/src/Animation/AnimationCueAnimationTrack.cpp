//
//  SpriteAnimationTrack.cpp
//  WalaberEngine
//
//  Created by Jason Pecho on 7/3/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "AnimationCueAnimationTrack.h"

namespace Walaber
{
    // -------------==============-------------==============-------------==============-------------===
    AnimationCueAnimationTrack::AnimationCueAnimationTrack():
    mMgr(0),
    mAnimation(0),
    mAnimationEvents(),
    mLastEvent(-1),
    mNumEvents(0)
    {        
    }
    
    // -------------==============-------------==============-------------==============-------------===
    AnimationCueAnimationTrack::AnimationCueAnimationTrack(AnimationManager* mgr):
    mMgr(mgr),
    mAnimation(0),
    mAnimationEvents(),
    mLastEvent(-1),
    mNumEvents(0)
    {        
    }
    
    
    // -------------==============-------------==============-------------==============-------------===
    AnimationCueAnimationTrack::AnimationCueAnimationTrack(const AnimationCueAnimationTrack& cueAnimationTrack):
    mMgr(cueAnimationTrack.mMgr),
    mAnimation(cueAnimationTrack.mAnimation),
    mAnimationEvents(cueAnimationTrack.mAnimationEvents),
    mLastEvent(cueAnimationTrack.mLastEvent),
    mNumEvents(cueAnimationTrack.mNumEvents)
    {
        
    }
    
    // -------------==============-------------==============-------------==============-------------===
    AnimationCueAnimationTrack& AnimationCueAnimationTrack::operator=(const AnimationCueAnimationTrack& cueAnimationTrack)
    {
        mMgr = cueAnimationTrack.mMgr;
        mAnimation = cueAnimationTrack.mAnimation;
        mAnimationEvents = cueAnimationTrack.mAnimationEvents; 
        mLastEvent = cueAnimationTrack.mLastEvent; 
        mNumEvents = cueAnimationTrack.mNumEvents;
        
        return *this;
    }
    
    // -------------==============-------------==============-------------==============-------------===
    AnimationCueAnimationTrack::~AnimationCueAnimationTrack()
    {
        
    }
}