#include "SkeletonActorCueAnimationTrack.h"

namespace Walaber
{
    // -------------==============-------------==============-------------==============-------------===
    SkeletonActorCueAnimationTrack::SkeletonActorCueAnimationTrack():
    mActor(0),
    mAnimationEvents(),
    mLastEvent(-1),
    mNumEvents(0)
    {        
    }
    
    // -------------==============-------------==============-------------==============-------------===
    SkeletonActorCueAnimationTrack::SkeletonActorCueAnimationTrack(SkeletonActor* actor):
    mActor(actor),
    mAnimationEvents(),
    mLastEvent(-1),
    mNumEvents(0)
    {        
    }
    
    
    // -------------==============-------------==============-------------==============-------------===
    SkeletonActorCueAnimationTrack::SkeletonActorCueAnimationTrack(const SkeletonActorCueAnimationTrack& skeletonActorAnimationTrack):
    mActor(skeletonActorAnimationTrack.mActor),
    mAnimationEvents(skeletonActorAnimationTrack.mAnimationEvents),
    mLastEvent(skeletonActorAnimationTrack.mLastEvent),
    mNumEvents(skeletonActorAnimationTrack.mNumEvents)
    {
        
    }
    
    // -------------==============-------------==============-------------==============-------------===
    SkeletonActorCueAnimationTrack& SkeletonActorCueAnimationTrack::operator=(const SkeletonActorCueAnimationTrack& rhs)
    {
        mActor = rhs.mActor;
        mAnimationEvents = rhs.mAnimationEvents; 
        mLastEvent = rhs.mLastEvent; 
        mNumEvents = rhs.mNumEvents;
        
        return *this;
    }
    
    // -------------==============-------------==============-------------==============-------------===
    SkeletonActorCueAnimationTrack::~SkeletonActorCueAnimationTrack()
    {
        
    }
}