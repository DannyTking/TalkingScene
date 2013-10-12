#ifndef _CAMERA_CONTROLLER_H_
#define _CAMERA_CONTROLLER_H_

#include <Vector2.h>
#include <Camera.h>

#include <vector>

namespace Walaber
{	
	// --------========--------========--------========--------========--------========--------========--------========
    class CameraController
    {
	public:
        
        // --------========--------========--------========--------========--------========--------========--------========
        /**
         * Default constructor. Set initial value to members.
         */
        CameraController();
        
        /**
         * Default deconstructor.
         */
        ~CameraController();
        
        // --------========--------========--------========--------========--------========--------========--------========
        /**
         * Init camera infomation.
         * @param cam A camera object this controller will control.
         */
        void initCamera( Camera* cam );
        
        // --------========--------========--------========--------========--------========--------========--------========
        void update( float elapsedSec );
        
        // --------========--------========--------========--------========--------========--------========--------========
        void queueMovement( Vector2 toPos, Vector2 toSize, float duration, float pause, float postPause, bool linear = false );
        void setMovement( Vector2 toPos, Vector2 toSize );
        
        void clearQueue();
        
        // --------========--------========--------========--------========--------========--------========--------========
        void restoreOrig();
        
        Vector2 getOrigPos() { return mOrigPos; };
        Vector2 getOrigSize() { return mOrigSize; };
        
        void setOrigPos( const Vector2& origPos )
        {
            mOrigPos = origPos;
        }
        
        void setOrigSize( const Vector2& origSize )
        {
            mOrigSize = origSize;
        }
        
        // --------========--------========--------========--------========--------========--------========--------========
        void endMovement( bool skip );
        bool isAnimating();
        
    private:
        
        // Camera reference
        Camera*                                     mCamera;
        Vector2                                     mOrigPos;
        Vector2                                     mOrigSize;
        
        // Queues
        std::vector< Vector2 >                      mPosTos;
        std::vector< Vector2 >                      mSizeTos;
        std::vector< float >                        mDurations;
        std::vector< float >                        mPauses;                /**> If -1, call.*/  
        std::vector< float >                        mPostPauses;            /**> If -1, call.*/  
        std::vector< bool >                         mIsLinears;             /**> should we use linear dolly or spherical linear dolly.*/
        
        // State
        bool                                        mEmpty;
        float                                       mTimer;
        unsigned int                                mCurrentMovement;
         
    };
}

#endif	// _CAMERA_CONTROLLER_H_
