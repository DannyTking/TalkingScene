#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <Vector2.h>
#include <SpriteBatch.h>

#include <vector>

namespace Walaber
{	
	// --------========--------========--------========--------========--------========--------========--------========
    /**
     * A class represents camera.
     */
    class Camera
    {
	public:
        
		// --------========--------========--------========--------========--------========--------========--------========
        /**
         *  Enum represents camera movement.
         */
        enum CameraMovement
        {
            CM_Dolly,
            CM_LDolly,
            CM_Zoom,
            CM_Shake,
            CM_SIZE
        };
        
		// --------========--------========--------========--------========--------========--------========--------========
        /**
         * Constructor inits animation speed.
         */
        Camera();
        
        /**
         * Constructor sets initial position and size of the camera.
         */
        Camera( Vector2 newPos, Vector2 newSize );
        
		// --------========--------========--------========--------========--------========--------========--------========
        /**
         * Default deconstructor.
         */
        ~Camera();
        
        // --------========--------========--------========--------========--------========--------========--------========
        Vector2 getPosition() { return mPos; };
        Vector2 getSize() { return mSize; };
        
        // --------========--------========--------========--------========--------========--------========--------========
        bool setPosition( Vector2 newPos );
        bool setSize( Vector2 newSize );
        
        /**
         * Change current position by newPos.
         */
        bool changePosition( Vector2 newPos );
        
        bool setWidth( float camWidth, float aspect = -1.0f );
        bool setHeight( float camHeight, float aspect = -1.0f );
		
        /**
         * Return aspect ratio of camera.
         */
		float getAspectRatio() { return mSize.X / mSize.Y; }
        
        // --------========--------========--------========--------========--------========--------========--------========
        void update(float elapsedSec);
        void updateViewport();
        void restoreViewport();
        void updateViewport(bool shouldInvert);
        
        // --------========--------========--------========--------========--------========--------========--------========
        void animate( CameraMovement type, Vector2 from, Vector2 to, float duration );
        void animate( CameraMovement type, Vector2 from, Vector2 to, float duration, float tInit );
        
        /**
         * Shake the camera. Camera will only shake vertically for now.
         * @param amplitude amplitude of the shake movement.
         * @param angularFreq frequency of the shake movement.
         * @param duration Duration within which the shaking movement should be finished.
         */
        void shake( float amplitude, float angularFreq, float duration );
        void halfShake( float amplitude, float angularFreq, float duration );
        
        /**
         * Move the camera to a positino in fixed time. Speed is not linearly calculated.
         * @param finalPosition Position where the camera should arrive finally.
         * @param duration Duration within which the movement should be completed.
         * @see linearDollyToPosition( Vector2, float);
         */
        void dollyToPosition( Vector2 finalPosition, float duration );
        
        /**
         * Move the camera to a positino in fixed time. Speed is linearly calculated.
         * @param finalPosition Position where the camera should arrive finally.
         * @param duration Duration within which the movement should be completed.
         * @see dollyToPosition( Vector2, float);
         */
        void linearDollyToPosition( Vector2 finalPosition, float duration );     
        
        /**
         * Zoom the camera. How much it zooms depends on factor. The final view port size will be the result of original one devided by factor.
         * @param factor Indicate how much it should zoom.
         * @param duration Duration within which the zooming movement should be completed.
         * @see zoomToWidth( float , float )
         * @see zoomToHeight( float , float )
         */
        void zoomToFactor( float factor, float duration );
        
        /**
         * Zoom the camera. How much it zooms depends on finalWidth. Keeping the aspect ratio fixed, the final view port size will have finalWidth in width and adjusted height.
         * @param finalWidth Indicate how much the width of view port should be.
         * @param duration Duration within which the zooming movement should be completed.
         * @see zoomToFactor( float , float )
         * @see zoomToHeight( float , float )
         */
        void zoomToWidth( float finalWidth, float duration );
        
        /**
         * Zoom the camera. How much it zooms depends on finalHeight. Keeping the aspect ratio fixed, the final view port size will have finalHeight as height and adjusted width.
         * @param finalWidth Indicate how much the height of view port should be.
         * @param duration Duration within which the zooming movement should be completed.
         * @see zoomToFactor( float , float )
         * @see zoomToWidth( float , float )
         */
        void zoomToHeight( float finalHeight, float duration );
        
        /**
         * Stop all the animation applied to camera.
         */
        void clearAnimations();
        
        /**
         * Finish all the animation applied to camera.
         * @param skip If skip is true, then speed up the animation speed so it will finish soon. If skip is false, then just directly jump to the last state of all the animation and stop animation.
         */
        void endAnimations(bool skip);
        
        /**
         * Return bool value to indicate whether there is animation applied to this camera.
         * @return Bool value.
         */
        bool isAnimating();
		
		// --------========--------========--------========--------========--------========--------========--------========
        /** 
         * Return how many kinds of animation of the same type are applied to this camera now.
         * @param type type of camera movement.
         * @return the number of same animation applied to this camera.
         */
        int numAnimationType( CameraMovement type );
        
        // positions
        /**
         * Convert the position in screen coordinate to world coordinate.
         * @param screenPos position in screen coordinate
         * @return position represented in world coordinate.
         * @see worldToScreen(const Vector2&)
         **/
        Vector2 screenToWorld(const Vector2& screenPos);
        
        /**
         * Convert the position in world coordinate to screen coordinate.
         * @param worldPos position in world coordinate
         * @return position represented in world coordinate.
         * @see screenToWorld(const Vector2&)
         **/
        Vector2 worldToScreen(const Vector2& worldPos);
        
        // sizes
        /**
         * Convert the size in screen coordinate to world coordinate.
         * @param screenSize size in screen coordinate
         * @return size represented in world coordinate.
         * @see worldSizeToScreenSize(const Vector2&)
         **/
        Vector2 screenSizeToWorldSize(const Vector2& screenSize);
        
        /**
         * Convert the size in world coordinate to screen coordinate.
         * @param worldSize size in world coordinate
         * @return size represented in screen coordinate.
         * @see screenSizeToWorldSize(const Vector2&)
         **/
        Vector2 worldSizeToScreenSize(const Vector2& worldSize);

    private:
        
        // Camera dimensions
        Vector2                         mPos;
        Vector2                         mSize;
        
        // Movement/Animation lists
        std::vector< int >              mTypes;
        std::vector< Vector2 >          mFroms;
        std::vector< Vector2 >          mTos;
        std::vector< float >            mDurations;
        std::vector< float >            mTs;
        
        // Other Movement/Animation variables
        bool                            mEmpty;
        float                           mAnimationSpeed;
		
		Vector2							mPreShakePos;
    };
}

#endif	// _CAMERA_H_
