
#ifndef _SKELETONACTOR_CUE_ANIMATION_TRACK_H_
#define _SKELETONACTOR_CUE_ANIMATION_TRACK_H_

#include <string>
#include <vector>

#include "AnimationSharedDefinitions.h"
#include "SkeletonActor.h"

namespace Walaber
{
    class Animation;
	
    class SkeletonActorCueAnimationTrack
    {
    public:// typedef, enums, and structs
        enum EventType
		{
			PlayAnimation,
			Pause
		};
		
        struct CueAnimationEvent
        {
			CueAnimationEvent( float t, std::string name, AnimationPlaybackMode mode, int loops ) : 
			mType(PlayAnimation), mTime(t), mAnimationName(name), mMode(mode), mLoopCount(loops), 
			mSoundState(SkeletonActor::ASS_On), mOffset(0.0f),
			mPauseImmediately(false), mCrossFade(true)
			{ }
			
			CueAnimationEvent() : 
			mType(Pause), mTime(0), mAnimationName(), mMode(PM_Once), mLoopCount(0),
			mSoundState(SkeletonActor::ASS_On), mOffset(0.0f),
			mPauseImmediately(true), mCrossFade(true)
			{ }
			
			EventType							mType;
            float								mTime;
            std::string							mAnimationName;
            AnimationPlaybackMode				mMode;
			int									mLoopCount;
			SkeletonActor::AnimationSoundState	mSoundState;
			float								mOffset;
			bool								mPauseImmediately;
			bool								mCrossFade;
        };
        
    public:
		// interface
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Default Ctor
        // </Summary>
        SkeletonActorCueAnimationTrack();
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // init with node track is animating
        // </Summary>
        explicit SkeletonActorCueAnimationTrack(SkeletonActor* actor);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // copy ctor
        // </Summary>
        SkeletonActorCueAnimationTrack(const SkeletonActorCueAnimationTrack& skeletonActorCueAnimationTrack);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // op=
        // </Summary>
        SkeletonActorCueAnimationTrack& operator=(const SkeletonActorCueAnimationTrack& skeletonActorCueAnimationTrack);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Dtor
        // </Summary>
        ~SkeletonActorCueAnimationTrack();
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set the animation manager this track talks to
        // </Summary>
        inline void setSkeletonActor(SkeletonActor* actor)
        {
            mActor = actor;
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
        void addAnimationEvent(const float time, const std::string& name, 
							   AnimationPlaybackMode mode, int loops = -1, bool pauseImmediately = false, bool crossFade = true,
							   SkeletonActor::AnimationSoundState soundState = SkeletonActor::ASS_On, float offset = 0.0f)
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
                
                it = mAnimationEvents.insert(it, CueAnimationEvent(time, name, mode, loops));
				(*it).mSoundState = soundState;
				(*it).mOffset = offset;
				(*it).mPauseImmediately = pauseImmediately;
				(*it).mCrossFade = crossFade;
            }
            else
            {
                mAnimationEvents.push_back(CueAnimationEvent(time, name, mode, loops));
                CueAnimationEvent& event = mAnimationEvents[0];
				event.mSoundState = soundState;
				event.mOffset = offset;
				event.mPauseImmediately = pauseImmediately;
				event.mCrossFade = crossFade;
            }
            
            ++mNumEvents;
        }
		
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set curves for local pos
        // </Summary>
        void addAnimationPauseEvent(float time)
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
            }
            else
            {
                mAnimationEvents.push_back(CueAnimationEvent());
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
                
				if (aniEvent.mType == PlayAnimation)
				{
					mActor->playAnimation( aniEvent.mAnimationName,
										  aniEvent.mSoundState,
										  aniEvent.mOffset, 
										  aniEvent.mMode, 
										  aniEvent.mLoopCount,
										  aniEvent.mCrossFade);
					
					if (aniEvent.mPauseImmediately)
						mActor->getSkeleton()->getAnimationManager()->pauseCurrentAnimation();
				}
				else if (aniEvent.mType == Pause)
				{
					mActor->getSkeleton()->getAnimationManager()->pauseCurrentAnimation();
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
        
        SkeletonActor*						mActor;
        std::vector<CueAnimationEvent>		mAnimationEvents;
        
        int                                 mLastEvent;
        unsigned int                        mNumEvents;
    };
}

#endif		// _ANIMATION_CUE_ANIMATION_TRACK_H_