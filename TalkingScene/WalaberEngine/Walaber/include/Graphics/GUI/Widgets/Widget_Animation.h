//
//  WaterMeter.h
//  MickeyGame
//
//  Created by Christopher Gulliver on 02/08/13.
//  Copyright 2013 Disney Mobile. All rights reserved.
//

#ifndef _WALABER_GUI_WIDGET_ANIMATION_H_
#define _WALABER_GUI_WIDGET_ANIMATION_H_

// System Includes

// Walaber Includes
#include "GUI.h"
#include "SkeletonActor.h"

namespace Walaber
{
    /*!
    @see Action_PlayAnimationOnWidgetAndWaitForCompletion
    */
    class Widget_Animation : public Widget
    {
    public:
        /*!
        constructor
        */
        Widget_Animation( int name, Vector2 pos, Vector2 size );

        /*!
        Default destructor
        */
        ~Widget_Animation();

		virtual bool update( float elapsedSec, WidgetActionRet& ret );
		void draw( SpriteBatch* sb );        
        
        /*
         In order for a skeleton actor to live longer than an animation widget, 
         it needs to be a pointer and be taken care of by other parts of the code, so we
         can set the skeleton actor here!
         @param skeletonActor the actor to use
        */
        void setSkeletonActor( SkeletonActor* skeletonActor )
        {
            if( mOwnsSkeletonActor && mActor )
            {
                delete mActor;
            }
            mActor = skeletonActor;
        }
        
        void loadSkeletonActor( const std::string & skeletonXMLFile,
                                const std::string & animationXMLFile,
                                const std::string & sceneName,
                                const std::string & groupsToLoad,
                                const std::string & scenesToLoad );
        
        /*!
         This function will initialize the skeleton actor and start playing the right animation
        */
        void initActor();

        void setIdleAnimationOrGroup( const std::string & idleAnimationOrGroup );
        void setTappedAnimationOrGroup( const std::string & tappedAnimationOrGroup );
        std::string getTappedAnimationGroup();
        void playTapAnimation();
        void setLocalScale2D( const Vector2& scale );
        void onFinishedLoading( void * data );
        
        SkeletonActor * getSkeletonActor()
        {
            return mActor;
        }

        void setSkeletonScale(Vector2 scale)
        {
            float scaler = ScreenCoord::getScreenResolution().length() /
                           ScreenCoord::getDesignResolution().length();

            Skeleton* skeleton = mActor->getSkeleton();
            skeleton->setLocalScale2D(scale * scaler);
        }

        void clearAnimationQueue();

        void clearFinishedCallback()
        {
            mCurrentAnimationFinishedCallback.reset();
        }

        void enqueueAnimation(
            const std::string & animation,
            AnimationPlaybackMode playMode,
            int loopCount,
            CallbackPtr finishedCallback );
        
        bool playAnimation(
            const std::string & animation,
            AnimationPlaybackMode playMode,
            int loopCount,
            CallbackPtr finishedCallback );
        
		bool acceptNewFingerDown( int fingerID, FingerInfo* info );
        
		void pauseSounds()
        {
            assert(mActor);
            mActor->pauseSounds();
        }

        void stopSounds() 
		{ 
			assert( mActor);
			mActor->stopAllSounds(); 
		}
        
        void resumeSounds()
        {
            assert(mActor);
            mActor->resumeSounds();
        }
        
		void disallowSounds(bool disallow) 
		{ 
			assert( mActor );
			mActor->setDisallowSounds(disallow); 
		}

        void clearSceneProps()
        {
            assert(mActor);
            mActor->clearSceneProps();
        }

    private:
        Widget_Animation();

        bool _playAnimation( const std::string & animation,
                             AnimationPlaybackMode playMode,
                            int loopCount,
                             CallbackPtr finishedCallback,
                             bool clearQueue );
        
        void _animationEventCallback(void* data);

        SkeletonActor*  mActor;                 ///< Seleton Actor that will be performing the animation
        std::string     mIdleAnimationGroup;    ///< The animation group to play from when idle
        std::string     mTappedAnimationGroup;  ///< The animation group to play from when tapped
        std::string     mScene;                 ///< The name of the scene that this animation is using
        bool            mOwnsSkeletonActor;     ///< If this is true, delete the skeleton actor on destruct
                                    
        struct QueuedAnimationInformation
        {
            std::string mAnimation;             ///< The name of the animation (or group)
            CallbackPtr mFinishedCallback;      ///< A callback for when this animation finishes
            AnimationPlaybackMode mPlayMode;
            int         mLoopCount;             ///< -1 = infinite, >0 is that number of times

            QueuedAnimationInformation(
                const std::string & animationName,
                AnimationPlaybackMode playMode,
                int loopCount,
                CallbackPtr finishedCallback ) :
                    mAnimation( animationName ),
                    mFinishedCallback( finishedCallback ),
                    mPlayMode( playMode ),
                    mLoopCount( loopCount )
            {
            }

            private:
                QueuedAnimationInformation();
        };

        typedef std::list< QueuedAnimationInformation * > AnimationQueueType;
        AnimationQueueType  mAnimationQueue;

        CallbackPtr         mCurrentAnimationFinishedCallback;

        //std::vector< Widget_label * > mHitAreas;
    };
    MAKE_SHARED_PTR( Widget_Animation );

}	// end namespace Walaber

#endif	// _WALABER_GUI_WIDGET_ANIMATION_H_
