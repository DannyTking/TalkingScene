
#ifndef _ANIMATION_CUE_ANIMATION_TRACK_H_
#define _ANIMATION_CUE_ANIMATION_TRACK_H_

#include <string>
#include <vector>

#include "AnimationSharedDefinitions.h"
#include "AnimationManager.h"

namespace Walaber
{
    class Animation;
	
    class AnimationCueAnimationTrack
    {
    public:// typedef, enums, and structs
        
        struct CueAnimationEvent
        {
            float						mTime;
            std::string					mAnimationName;
            AnimationPlaybackMode		mMode;
			int							mLoopCount;
        };
        
    public:
		// interface
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Default Ctor
        // </Summary>
        AnimationCueAnimationTrack();
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // init with node track is animating
        // </Summary>
        explicit AnimationCueAnimationTrack(AnimationManager* mgr);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // copy ctor
        // </Summary>
        AnimationCueAnimationTrack(const AnimationCueAnimationTrack& animationCueAnimationTrack);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // op=
        // </Summary>
        AnimationCueAnimationTrack& operator=(const AnimationCueAnimationTrack& animationCueAnimationTrack);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Dtor
        // </Summary>
        ~AnimationCueAnimationTrack();
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set the animation manager this track talks to
        // </Summary>
        inline void setAnimationManager(AnimationManager* mgr)
        {
            mMgr = mgr;
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
        void addAnimationEvent(const float time, const std::string& name, AnimationPlaybackMode mode, int loops = -1)
        {
            if(mNumEvents)
            {            
                // start from the end and go backwards because more often than not the events will be added in order
                std::vector<CueAnimationEvent>::reverse_iterator rIt = mAnimationEvents.rbegin();
                std::vector<CueAnimationEvent>::reverse_iterator rItEnd = mAnimationEvents.rbegin();
                for(; rIt != rItEnd; ++rIt)
                {
                    if(time < (*rIt).mTime)
                    {
                        // new time is less than the last time so we insert here
                        break;
                    }
                }
                
                std::vector<CueAnimationEvent>::iterator it = rIt.base();
                
                it = mAnimationEvents.insert(it, CueAnimationEvent());
                (*it).mTime = time;
                (*it).mAnimationName = name; 
				(*it).mMode = mode;
                (*it).mLoopCount = loops;
            }
            else
            {
                mAnimationEvents.push_back(CueAnimationEvent());
                CueAnimationEvent& event = mAnimationEvents[0];
                event.mTime = time;
                event.mAnimationName = name; 
				event.mMode = mode;
                event.mLoopCount = loops;
            }
            
            ++mNumEvents;
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // check to see if this time triggers a new animation
        // </Summary>
        void apply(const float time)
        {
            if(mLastEvent != (mNumEvents-1) && time > mAnimationEvents[mLastEvent+1].mTime)
            {
                ++mLastEvent; 
                
                CueAnimationEvent& aniEvent = mAnimationEvents[mLastEvent];
                
                mMgr->playAnimation( aniEvent.mAnimationName, aniEvent.mMode, aniEvent.mLoopCount );
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
        
        AnimationManager*					mMgr;
        Animation*                          mAnimation;
        std::vector<CueAnimationEvent>		mAnimationEvents;
        
        int                                 mLastEvent;
        unsigned int                        mNumEvents;
    };
}

#endif		// _ANIMATION_CUE_ANIMATION_TRACK_H_