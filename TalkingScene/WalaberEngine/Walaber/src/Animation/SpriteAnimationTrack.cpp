//
//  SpriteAnimationTrack.cpp
//  WalaberEngine
//
//  Created by Jason Pecho on 7/3/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "SpriteAnimationTrack.h"

namespace Walaber
{
    // -------------==============-------------==============-------------==============-------------===
    SpriteAnimationTrack::SpriteAnimationTrack():
    mSprite(0),
    mSpriteAnimation(),
    mAnimation(0),
    mAnimationEvents(),
	mPreviousPlaybackMode(PM_Once),
    mLastEvent(-1),
    mNumEvents(0)
    {        
    }
    
    // -------------==============-------------==============-------------==============-------------===
    SpriteAnimationTrack::SpriteAnimationTrack(Sprite* sprite):
    mSprite(sprite),
    mSpriteAnimation(),
    mAnimation(0),
    mAnimationEvents(),
	mPreviousPlaybackMode(PM_Once),
    mLastEvent(-1),
    mNumEvents(0)
    {        
    }
    
    
    // -------------==============-------------==============-------------==============-------------===
    SpriteAnimationTrack::SpriteAnimationTrack(const SpriteAnimationTrack& spriteAnimationTrack):
    mSprite(spriteAnimationTrack.mSprite),
    mSpriteAnimation(spriteAnimationTrack.mSpriteAnimation),
    mAnimation(spriteAnimationTrack.mAnimation),
    mAnimationEvents(spriteAnimationTrack.mAnimationEvents),
	mPreviousPlaybackMode(spriteAnimationTrack.mPreviousPlaybackMode),
    mLastEvent(spriteAnimationTrack.mLastEvent),
    mNumEvents(spriteAnimationTrack.mNumEvents)
    {
        
    }
    
    // -------------==============-------------==============-------------==============-------------===
    SpriteAnimationTrack& SpriteAnimationTrack::operator=(const SpriteAnimationTrack& spriteAnimationTrack)
    {
        mSprite = spriteAnimationTrack.mSprite;
        mSpriteAnimation = spriteAnimationTrack.mSpriteAnimation;
        mAnimation = spriteAnimationTrack.mAnimation;
        mAnimationEvents = spriteAnimationTrack.mAnimationEvents; 
		mPreviousPlaybackMode = spriteAnimationTrack.mPreviousPlaybackMode;
        mLastEvent = spriteAnimationTrack.mLastEvent; 
        mNumEvents = spriteAnimationTrack.mNumEvents;
        
        return *this;
    }
    
    // -------------==============-------------==============-------------==============-------------===
    SpriteAnimationTrack::~SpriteAnimationTrack()
    {
        
    }
}