// System Includes

// Walaber Includes
#include "Widget_Animation.h"
#include "MemberCallback.h"

namespace Walaber
{

///@todo play from an animation group when tapped
///@todo play from an animation group when held

    // ---------------------------------------------------------------------------------------------
    Widget_Animation::Widget_Animation( int name, Vector2 pos, Vector2 size ) :
        Widget( name, WT_ANIMATION, pos, size, WO_PortraitUp ),
        mActor(0),
        mOwnsSkeletonActor( false )
    {
    }

    // ---------------------------------------------------------------------------------------------
    Widget_Animation::~Widget_Animation()
    {
        mActor->unregisterEventCallback( this );
        if( mOwnsSkeletonActor )
        {
            delete mActor;
        }
    }

    // ---------------------------------------------------------------------------------------------
    bool Widget_Animation::update( float elapsedSec, WidgetActionRet& ret )
    {
        Widget::update( elapsedSec, ret );

        if ( mActor->isLoaded() )
        {
            mActor->update( elapsedSec );
        }
        
        return false;
    }
    
    // ---------------------------------------------------------------------------------------------
    void Widget_Animation::draw( SpriteBatch* sb )
    {
        if ( mActor->isLoaded() )
        {
            mActor->setPosition( getLocalPosition2D() );
            mActor->draw( sb, ( SkeletonActor::DrawFlags_Skeleton |
                               SkeletonActor::DrawFlags_Props |
                               SkeletonActor::DrawFlags_Emitters ) );
        }
        
        // Draw the widget
        Widget::draw( sb );
    }

    #pragma mark Widget Initialization 

    // ---------------------------------------------------------------------------------------------
    void Widget_Animation::loadSkeletonActor(
        const std::string & skeletonXMLFile,
        const std::string & animationXMLFile,
        const std::string & sceneName,
        const std::string & groupsToLoad,
        const std::string & scenesToLoad )
    {
        if( mOwnsSkeletonActor && mActor )
        {
            delete mActor;
            mActor = 0;
        }
        
        if( !mActor )
        {
            mActor = new SkeletonActor;
            mOwnsSkeletonActor = true;
        }
        
        mScene = sceneName;
        mActor->loadSkeleton( "MrBones", skeletonXMLFile );
        mActor->loadAnimationDescription(
            animationXMLFile,
            groupsToLoad,
            scenesToLoad,
            CreateMemberCallbackPtr( *this, &Widget_Animation::onFinishedLoading ) );
    }
    
    // ---------------------------------------------------------------------------------------------
    void Widget_Animation::initActor()
    {
        Vector2 skeletonPosition = getLocalPosition2D();
        
        float scaler = ScreenCoord::getScreenResolution().length() /
        ScreenCoord::getDesignResolution().length();
        Vector2 skeletonScale = Vector2( 1.0f, -1.0f ) * scaler;
        
        mActor->init( skeletonPosition, mScene, skeletonScale, true );
        
        // Register an event callback so that we can handle events from the skeleton's action manager
        mActor->registerEventCallback( this,
                                      CreateMemberCallbackPtr( *this, &Widget_Animation::_animationEventCallback ) );
        
        // Play the idle animation if there no other animation was playing
        if ( !mIdleAnimationGroup.empty() )
        {
            playAnimation( mIdleAnimationGroup, PM_Once, 0, CallbackPtr() );
        }
    }

    // ---------------------------------------------------------------------------------------------
    void Widget_Animation::setIdleAnimationOrGroup( const std::string & idleAnimationOrGroupName )
    {
        mIdleAnimationGroup = idleAnimationOrGroupName;
    }

    // ---------------------------------------------------------------------------------------------
    void Widget_Animation::setTappedAnimationOrGroup( const std::string & tappedAnimationOrGroupName )
    {
        mTappedAnimationGroup = tappedAnimationOrGroupName;
    }

    // ---------------------------------------------------------------------------------------------
    std::string Widget_Animation::getTappedAnimationGroup()
    {
        return mTappedAnimationGroup;
    }

    // ---------------------------------------------------------------------------------------------
	void Widget_Animation::setLocalScale2D( const Vector2& scale )
	{
		Node::setLocalScale2D( scale );
		_updateAABB();
	}

    // ---------------------------------------------------------------------------------------------
    void Widget_Animation::onFinishedLoading( void * data )
    {
        initActor();
    }

    // ---------------------------------------------------------------------------------------------
    void Widget_Animation::clearAnimationQueue()
    {
        // Delete all of the queued animations
        AnimationQueueType::iterator iter = mAnimationQueue.begin();
        AnimationQueueType::iterator iterEnd = mAnimationQueue.end();
        
        for ( ; iter != iterEnd; ++iter )
        {
            QueuedAnimationInformation * data = *iter;
            if ( data != NULL )
            {
                delete data;
            }
            *iter = NULL;
        }

        // Clear the animation queue
        mAnimationQueue.clear();
    }

    #pragma mark Widget Interaction

    // ---------------------------------------------------------------------------------------------
    void Widget_Animation::enqueueAnimation(
        const std::string & animation,
        AnimationPlaybackMode playMode,
        int loopCount,
        CallbackPtr finishedCallback )
    {
        QueuedAnimationInformation * animationToBeQueued =
            new QueuedAnimationInformation( animation, playMode, loopCount, finishedCallback );
        mAnimationQueue.push_back( animationToBeQueued );
    }

    // ---------------------------------------------------------------------------------------------    
    void Widget_Animation::playTapAnimation()
    {
		if (!mTappedAnimationGroup.empty())
		{
            // Only animate if idle, where idle is either a group that contains the current animation
            // or idle is the name of the animation itself
            if ( mActor->isCurrentAnimationMemberOfGroup( mIdleAnimationGroup ) ||
                 mActor->getCurrentAnimationName() == mIdleAnimationGroup )
            {
                _playAnimation(mTappedAnimationGroup, PM_Once, 0, CallbackPtr(), true);

                // For now, go back to idle after
                enqueueAnimation( mIdleAnimationGroup, PM_Once, 0, CallbackPtr() );
            }
        }
    }
    
    // ---------------------------------------------------------------------------------------------
    bool Widget_Animation::playAnimation(
        const std::string & animation,
        AnimationPlaybackMode playMode,
        int loopCount,
        CallbackPtr finishedCallback )
    {
        return _playAnimation( animation, playMode, loopCount, finishedCallback, true );
    }

    // ---------------------------------------------------------------------------------------------
    bool Widget_Animation::acceptNewFingerDown( int fingerID, FingerInfo* info )
    {
		if (!mEnabled)
		{
            return false;
		}
		
        playTapAnimation();
        return true;
    }

    // ---------------------------------------------------------------------------------------------
    bool Widget_Animation::_playAnimation(
        const std::string & animation,
        AnimationPlaybackMode playMode,
        int loopCount,
        CallbackPtr finishedCallback,
        bool clearQueue )
    {
        if ( clearQueue )
        {
            clearAnimationQueue();
        }

        mCurrentAnimationFinishedCallback = finishedCallback;

        // Try to play group first then animation
        if ( mActor->groupExists( animation ) && mActor->animationExists( animation ) )
        {
            // Group and animation name collision. Try renaming the group.
            assert(0);
            return false;
        }
        else if ( mActor->groupExists( animation ) )
        {
            // Play the animation as a group first
            mActor->playAnimationFromGroup( animation, playMode, -1, false );
        }
        else if ( mActor->animationExists( animation ) )
        {
            // Play the animation
            mActor->playAnimation( animation, 0, playMode, -1, false );
        }
        else
        {
            // The referenced animation was not found
            return false;
        }
        return true;
    }

    // ---------------------------------------------------------------------------------------------
    void Widget_Animation::_animationEventCallback(void* data)
    {        
        SkeletonActor::SkeletonActorAnimationEventCallbackParameters* params =
            (SkeletonActor::SkeletonActorAnimationEventCallbackParameters*)data;

        if ( params->eventType == AET_Looped || params->eventType == AET_Finished )
        {
            if ( mCurrentAnimationFinishedCallback )
            {
                mCurrentAnimationFinishedCallback->invoke(
                    reinterpret_cast<void*>( const_cast< std::string *>( &( params->animationName ) ) ) );
            }        
        
            if ( !mAnimationQueue.empty() )
            {
                QueuedAnimationInformation * animationInformation =
                    * mAnimationQueue.begin();

                if ( animationInformation != NULL )
                {
                    _playAnimation(
                        animationInformation->mAnimation,
                        animationInformation->mPlayMode,
                        animationInformation->mLoopCount,
                        animationInformation->mFinishedCallback,
                        false );

                    // Remove this animation from the queue
                    mAnimationQueue.pop_front();
                }
            }
        }
    }

}	// end namespace Walaber
