#include "CameraController.h"

namespace Walaber
{
    // --------========--------========--------========--------========--------========--------========--------========
    CameraController::CameraController() :
	mCamera( NULL ),
	mOrigPos(Vector2::Zero),
	mOrigSize(Vector2::One),
    mEmpty( true ),
	mTimer( 0.0f ),
    mCurrentMovement(0)
    {
    }
        
    // --------========--------========--------========--------========--------========--------========--------========
    CameraController::~CameraController()
    {
    }

    // --------========--------========--------========--------========--------========--------========--------========
    void CameraController::initCamera( Camera* cam )
    {
        mCamera = cam;
        
        mOrigPos = mCamera->getPosition();
        mOrigSize = mCamera->getSize();
    }
    
    // --------========--------========--------========--------========--------========--------========--------========
    void CameraController::update( float elapsedSec )
    {
        // We only care about updating if we have elements left in the queue
        if (!mEmpty)
        {
            // If the camera has no animations, pile on some more
            if (!mCamera->isAnimating())
            {
                if ((mTimer >= mPauses[mCurrentMovement]) && (mPauses[mCurrentMovement] != -1))
                {
                    // Add the animations
                    if(!mIsLinears[mCurrentMovement])
                        mCamera->dollyToPosition( mPosTos[mCurrentMovement], mDurations[mCurrentMovement] );
                    else
                        mCamera->linearDollyToPosition( mPosTos[mCurrentMovement], mDurations[mCurrentMovement] );                        
                    mCamera->zoomToWidth( mSizeTos[mCurrentMovement].X, mDurations[mCurrentMovement] );
                    
                    // Reset the timer
                    mTimer = 0;
                    mPauses[mCurrentMovement] = -1;
                }
                else if ((mTimer >= mPostPauses[mCurrentMovement]) && (mPauses[mCurrentMovement] == -1))
                {
                    // Remove them from our queue (unless we're storing animations for later/reverse use)
                    mPosTos.erase( mPosTos.begin()+mCurrentMovement );
                    mSizeTos.erase( mSizeTos.begin()+mCurrentMovement );
                    mDurations.erase( mDurations.begin()+mCurrentMovement );
                    mPauses.erase( mPauses.begin()+mCurrentMovement );
                    mPostPauses.erase( mPostPauses.begin()+mCurrentMovement );
                    mIsLinears.erase( mIsLinears.begin()+mCurrentMovement );
                    
                    // Reset the timer
                    mTimer = 0.0f;
                    
                    // See if it's empty
                    if (mPosTos.size() == 0)
                    {
                        mEmpty = true;
                    }
                }
                else 
                {
                    mTimer += elapsedSec;
                }
            }
        }
    }
    
    // --------========--------========--------========--------========--------========--------========--------========
    void CameraController::queueMovement( Vector2 toPos, Vector2 toSize, float duration, float pause, float postPause, bool linear )
    {
        // Add this movement to the queue
        mPosTos.push_back( toPos );
        mSizeTos.push_back( toSize );
        mDurations.push_back( duration );
        mPauses.push_back( pause );
        mPostPauses.push_back( postPause );
        mIsLinears.push_back( linear );
        
        mEmpty = false;
    }
    
    // --------========--------========--------========--------========--------========--------========--------========
    void CameraController::setMovement( Vector2 toPos, Vector2 toSize )
    {
        mCamera->setPosition( toPos );
        mCamera->setSize( toSize );
    }
    
    // --------========--------========--------========--------========--------========--------========--------========
    void CameraController::restoreOrig()
    {
        mCamera->clearAnimations();
        clearQueue();
        mCamera->setPosition(mOrigPos);
        mCamera->setSize(mOrigSize);  
    }
    
    // --------========--------========--------========--------========--------========--------========--------========
    void CameraController::clearQueue()
    {
        mPosTos.clear();
        mSizeTos.clear();
        mDurations.clear();
        mPauses.clear();
        mPostPauses.clear();
        mIsLinears.clear();
        
        mCamera->clearAnimations();
        
        mEmpty = true;
    }
    
    // --------========--------========--------========--------========--------========--------========--------========
    void CameraController::endMovement( bool skip )
    {
        mCamera->endAnimations( skip );
    }
    
    // --------========--------========--------========--------========--------========--------========--------========
    bool CameraController::isAnimating()
    {
        if (mCamera->isAnimating() || !mEmpty)
        {
            return true;
        }
        return false;
    }
}
