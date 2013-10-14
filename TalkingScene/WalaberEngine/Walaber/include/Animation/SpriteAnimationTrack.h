//
//  SpriteAnimationTrack.h
//  WalaberEngine
//
//  Created by Jason Pecho on 7/3/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef _SPRITE_ANIMATION_TRACK_H_
#define _SPRITE_ANIMATION_TRACK_H_

#include <string>
#include <vector>

#include <Sprite.h>
#include <Animation.h>

namespace Walaber
{    
    class SpriteAnimationTrack
    {
    public:// typedef, enums, and structs
        
        struct SpriteAnimationEvent
        {
            float       mTime;
            float       mPlayOffset;
            std::string mAnimationName;
            bool        mLoops;
            bool        mIsSingleFrame;
            
            SpriteAnimationEvent():
            mTime(0.0f),
            mPlayOffset(0.0f),
            mAnimationName(),
            mLoops(),
            mIsSingleFrame()
            {}
        };
        
    public: // interface
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Default Ctor
        // </Summary>
        SpriteAnimationTrack();
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // init with node track is animating
        // </Summary>
        explicit SpriteAnimationTrack(Sprite* sprite);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // copy ctor
        // </Summary>
        SpriteAnimationTrack(const SpriteAnimationTrack& spriteAnimationTrack);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // op=
        // </Summary>
        SpriteAnimationTrack& operator=(const SpriteAnimationTrack& spriteAnimationTrack);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Dtor
        // </Summary>
        ~SpriteAnimationTrack();
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // copy last event marker, used for the same animation trackes
        // </Summary>
        inline void copyLastEvent(const SpriteAnimationTrack& spriteAnimationTrack)
        {
            mLastEvent = spriteAnimationTrack.mLastEvent;
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set the sprite this track controls
        // </Summary>
        inline void setSprite(Sprite* sprite)
        {
            mSprite = sprite;
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set the animation thise track is linked to
        // </Summary>
        inline void setAnimation(Animation* animation)
        {
            mAnimation = animation;
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // rewinds the sprite track to the beginning
        // </Summary>
        inline void rewind()
        {
			mLastEvent = -1;
        }
        
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // sets up sprite track to start tracking from time
        // </Summary>
        inline void trackTo(const float time)
        {
			mLastEvent = -1;
			for(unsigned int i = 0; i < mNumEvents; ++i)
			{
				if(time > mAnimationEvents[i].mTime)
				{
					mLastEvent = i;
				}
				else 
				{
					break;
				}
			}
        }
		
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set curves for local pos
        // </Summary>
        void addSpriteAnimationEvent(const float time, const std::string& name, const bool loops, const float mPlayOffset=0.0f, const bool isSingleFrame = false)
        {
            if(mNumEvents)
            {            
                // start from the end and go backwards because more often than not the events will be added in order
                std::vector<SpriteAnimationEvent>::reverse_iterator rIt = mAnimationEvents.rbegin();
                std::vector<SpriteAnimationEvent>::reverse_iterator rItEnd = mAnimationEvents.rbegin();
                for(; rIt != rItEnd; ++rIt)
                {
                    if(time < (*rIt).mTime)
                    {
                        // new time is less than the last time so we insert here
                        break;
                    }
                }
                
                std::vector<SpriteAnimationEvent>::iterator it = rIt.base();
                
                it = mAnimationEvents.insert(it, SpriteAnimationEvent());
                (*it).mTime = time;
                (*it).mPlayOffset = mPlayOffset;
                (*it).mAnimationName = name; 
                (*it).mLoops = loops;
                (*it).mIsSingleFrame = isSingleFrame;
            }
            else
            {
                mAnimationEvents.push_back(SpriteAnimationEvent());
                SpriteAnimationEvent& event = mAnimationEvents[0];
                event.mTime = time;
                event.mPlayOffset = mPlayOffset;
                event.mAnimationName = name; 
                event.mLoops = loops;
                event.mIsSingleFrame = isSingleFrame;
            }
            
            ++mNumEvents;
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // check to see if this time triggers a new animation
        // </Summary>
        void apply(const float time)
        {
			if(mLastEvent != (mNumEvents-1))
			{
				float t = time-mAnimationEvents[mLastEvent+1].mTime;
				while(Walaber::absf(t) < EPSILON_IS_EQUAL || time > mAnimationEvents[mLastEvent+1].mTime)
				{
					// set the aniamtion to loop if it needs to
					if(mSpriteAnimation.use_count())
					{
						mSpriteAnimation->stop();
						
						if( mAnimationEvents[mLastEvent].mLoops )
						{
							mSpriteAnimation->setPlaybackMode( mPreviousPlaybackMode );
						}
					}
					
					++mLastEvent; 
					
					SpriteAnimationEvent& aniEvent = mAnimationEvents[mLastEvent];				
					
					if(aniEvent.mIsSingleFrame)
					{
						mSpriteAnimation = mSprite->playAnimation(aniEvent.mAnimationName, aniEvent.mPlayOffset);
						if(mSpriteAnimation.use_count())
						{							
							mSpriteAnimation->pause();
						}
						else
						{
							printf("MISSING SYMBOL ANIMATION: %s in Sprite '%s' in Animation '%s'\n", aniEvent.mAnimationName.c_str(), mSprite->getName(), mAnimation->getName().c_str());
						}
					}
					else
					{						
						float aniStart = aniEvent.mTime;
						mSpriteAnimation = mSprite->playAnimation(aniEvent.mAnimationName, (time-aniStart) + aniEvent.mPlayOffset);
						
						if( mSpriteAnimation.use_count() && aniEvent.mLoops )
						{
							mPreviousPlaybackMode = mSpriteAnimation->getPlaybackMode();
							mSpriteAnimation->setPlaybackMode( PM_Loops );
						}
					}
					
					//assert( mSpriteAnimation.use_count() );
					
					if(mLastEvent != (mNumEvents-1))
					{
						t = time-mAnimationEvents[mLastEvent+1].mTime;
					}
					else
					{
						break;
					}
				}
			}
        }
		
		// -------------==============-------------==============-------------==============-------------===
        float getLastEventTime()
		{
			if (mAnimationEvents.empty())
				return 0.0f;
			
			return mAnimationEvents[mAnimationEvents.size()-1].mTime;
		}
        
    private:
        
        Sprite*                             mSprite;
        SpriteAnimationPtr                  mSpriteAnimation;
        Animation*                          mAnimation;
        std::vector<SpriteAnimationEvent>   mAnimationEvents;
		AnimationPlaybackMode               mPreviousPlaybackMode;
        
        int                                 mLastEvent;
        unsigned int                        mNumEvents;
    };
}

#endif
