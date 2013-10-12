#include "Camera.h"
#include "WalaberMath.h"
#include "ScreenCoord.h"

namespace Walaber
{
	
	// --------========--------========--------========--------========--------========--------========--------========
    Camera::Camera() :
    mEmpty( true ),
    mAnimationSpeed( 1.0f )
    {
    }
    
    // --------========--------========--------========--------========--------========--------========--------========
    Camera::Camera( Vector2 newPos, Vector2 newSize ) :
    mEmpty( true ),
    mAnimationSpeed( 1.0f )
    {
        mPos = newPos;
        mSize = newSize;
    }
    
    // --------========--------========--------========--------========--------========--------========--------========
    Camera::~Camera()
    {
    }
    
    // --------========--------========--------========--------========--------========--------========--------========
    bool Camera::setPosition( Vector2 newPos ) 
    { 
        if (mEmpty)
        {
            mPos = newPos; 
            return true;
        }
        return false;
    }
    
    // --------========--------========--------========--------========--------========--------========--------========
    bool Camera::setSize( Vector2 newSize ) 
    { 
        if (mEmpty)
        {
            mSize = newSize; 
            return true;
        }
        return false;
    }
    
    // --------========--------========--------========--------========--------========--------========--------========
    bool Camera::setWidth( float camWidth, float aspect ) 
    { 
        if (mEmpty)
        {
			if (aspect < 0.0f) { aspect = ScreenCoord::getScreenAspectRatio(); }
			
            mSize.X = camWidth; 
            mSize.Y = camWidth * (1.0f / aspect);
            return true;
        }
        return false;
    }
    
    // --------========--------========--------========--------========--------========--------========--------========
    bool Camera::setHeight( float camHeight, float aspect ) 
    { 
        if (mEmpty)
        {
			if (aspect < 0.0f) { aspect = ScreenCoord::getScreenAspectRatio(); }
			
            mSize.X = camHeight * aspect;
            mSize.Y = camHeight;
            return true;
        }
        return false;
    }
    
    // --------========--------========--------========--------========--------========--------========--------========
    bool Camera::changePosition( Vector2 newPos ) 
    { 
        if (mEmpty)
        {
            mPos += newPos;  
            return true;
        } 
        return false;
    }
    
    // --------========--------========--------========--------========--------========--------========--------========
    void Camera::animate( CameraMovement type, Vector2 from, Vector2 to, float duration )
    {
        mTypes.push_back( type );
        mFroms.push_back( from );
        mTos.push_back( to );
        mDurations.push_back( duration );
        mTs.push_back( 0.0f );
        
        mEmpty = false;
    }
    
    // --------========--------========--------========--------========--------========--------========--------========
    void Camera::animate( CameraMovement type, Vector2 from, Vector2 to, float duration, float tInit )
    {
        mTypes.push_back( type );
        mFroms.push_back( from );
        mTos.push_back( to );
        mDurations.push_back( duration );
        mTs.push_back( tInit );
        
        mEmpty = false;
    }
    
    // --------========--------========--------========--------========--------========--------========--------========
    void Camera::shake( float amplitude, float angularFreq, float duration )
    {
		mPreShakePos = mPos;
        animate( CM_Shake, Vector2(amplitude, angularFreq), Vector2(0, 0), duration );
    }
    
    // --------========--------========--------========--------========--------========--------========--------========
    void Camera::halfShake( float amplitude, float angularFreq, float duration )
    {
		mPreShakePos = mPos;
        animate( CM_Shake, Vector2(amplitude, angularFreq), Vector2(0, 0), duration * 2, 0.5f );
    }
    
    // --------========--------========--------========--------========--------========--------========--------========
    void Camera::dollyToPosition( Vector2 finalPosition, float duration )
    {
        animate( CM_Dolly, mPos, finalPosition, duration );
    }

    // --------========--------========--------========--------========--------========--------========--------========
    void Camera::linearDollyToPosition( Vector2 finalPosition, float duration )
    {
        animate( CM_LDolly, mPos, finalPosition, duration );
    }
    
    // --------========--------========--------========--------========--------========--------========--------========
    void Camera::zoomToFactor( float factor, float duration )
    {
        animate( CM_Zoom, mSize, mSize / factor, duration );
    }
    
    // --------========--------========--------========--------========--------========--------========--------========
    void Camera::zoomToWidth( float finalWidth, float duration ) 
    {
        Vector2 newSize = Vector2( finalWidth, finalWidth * (1.0f / ScreenCoord::getScreenAspectRatio()) );
        animate( CM_Zoom, mSize, newSize, duration );
    }
   
    // --------========--------========--------========--------========--------========--------========--------========
    void Camera::zoomToHeight( float finalHeight, float duration )
    {
        Vector2 newSize = Vector2( finalHeight * ScreenCoord::getScreenAspectRatio(), finalHeight );
        animate( CM_Zoom, mSize, newSize, duration );
    }
    
    // --------========--------========--------========--------========--------========--------========--------========
    void Camera::endAnimations(bool skip) 
    {
        // Speed up the animation
        if (!skip)
        {
            mAnimationSpeed = 4.0f;
        }
        // Otherwise skip to the end
        else 
        {
            // Apply all of the changes in the animation list to the camera now
            for (int i = 0; i < mTypes.size(); i++) 
            {
                // Animate appropiately
                if ( (int) mTypes[i] == CM_Zoom )
                {
                    mSize = mTos[i];
                }
                else if ( (int) mTypes[i] == CM_Dolly )
                {
                    mPos = mTos[i];
                }
				else if ( (int) mTypes[i] == CM_Shake )
                {
					mPos = mPreShakePos;
				}
            }
            // Then clear the list
            clearAnimations();
        }
    }
	
	// --------========--------========--------========--------========--------========--------========--------========
    bool Camera::isAnimating() 
    {
		if (mEmpty)
		{
			return false;
		}
	
		if (mTypes.size() == numAnimationType(CM_Shake))
		{
			return false;
		}
		
		return !mEmpty; 
	}
	
	// --------========--------========--------========--------========--------========--------========--------========
    int Camera::numAnimationType( CameraMovement type ) 
    {
		int ret = 0;
		for (int i = 0; i < mTypes.size(); i++)
		{
			if (mTypes[i] == type)
			{
				ret++;
			}
		}
		return ret;
	}
    
    // --------========--------========--------========--------========--------========--------========--------========
    Vector2 Camera::screenToWorld(const Vector2& screenPos)
    {
        Vector2 toPos = screenPos / ScreenCoord::getScreenResolution();
		toPos.Y = -toPos.Y;
		Vector2 camTopLeft = mPos + Vector2(mSize.X * -0.5f, mSize.Y * 0.5f);
		return camTopLeft + (toPos * mSize);
    }
    
    // --------========--------========--------========--------========--------========--------========--------========
    Vector2 Camera::worldToScreen(const Vector2& worldPos)
    {
        Vector2 camTopLeft = mPos + Vector2(mSize.X * -0.5f, mSize.Y * 0.5f);
		Vector2 toPos = worldPos - camTopLeft;
		toPos.Y = -toPos.Y;
		toPos /= mSize;
		return toPos * ScreenCoord::getScreenResolution();
    }
    
    // --------========--------========--------========--------========--------========--------========--------========
    Vector2 Camera::screenSizeToWorldSize(const Vector2& screenSize)
    {
        Vector2 toSize = screenSize / ScreenCoord::getScreenResolution();
         
        return toSize * mSize;
    }
    
    // --------========--------========--------========--------========--------========--------========--------========
    Vector2 Camera::worldSizeToScreenSize(const Vector2& worldSize)
    {
        Vector2 toSize = worldSize/mSize;
        return toSize *  ScreenCoord::getScreenResolution();
    }
    
    // --------========--------========--------========--------========--------========--------========--------========
    void Camera::clearAnimations() 
    {
        mTypes.clear();
        mFroms.clear();
        mTos.clear();
        mDurations.clear();
        mTs.clear();
        
        mEmpty = true;
		
		mAnimationSpeed = 1.0f;
    }
    
    // --------========--------========--------========--------========--------========--------========--------========
    void Camera::update(float elapsedSec)
    {
        // If we have elements to animate, animate!
        if (!mEmpty)
        {
            // Dreams are offsets, duh!
            Vector2 dreamPos = Vector2(0, 0);
            Vector2 dreamSize = Vector2(0, 0);
            
            // Animate all things in the list
            for (int i = 0; i < mTypes.size(); i++) 
            {
                // Advance
                mTs[i] += elapsedSec * mAnimationSpeed;
                
                // Animate appropiately
                if ( (int) mTypes[i] == CM_Zoom )
                {
                    mSize = mFroms[i].slerp( mTos[i], mTs[i] / mDurations[i] );
                }
                else if ( (int) mTypes[i] == CM_Dolly )
                {
                    mPos = mFroms[i].slerp( mTos[i], mTs[i] / mDurations[i] );
                }
                else if( (int) mTypes[i] == CM_LDolly )
                {
                    mPos = mFroms[i].lerp( mTos[i], mTs[i] / mDurations[i] );                    
                }
                else if ( (int) mTypes[i] == CM_Shake )
                {
                    // Amp
                    float amp = mFroms[i].X;
                    // Freq
                    float freq = (int) mFroms[i].Y;
                    // t
                    float t = mTs[i] / mDurations[i];
                    
                    float offset = 0.5f;
                    
                    // Magic equation: amp * ( sin(freq * PI * t - 1.5) + cos((freq + 1) * PI * t - 1.5) )
                    dreamPos.Y = amp * ( sin(freq * PI * (t * 2 + offset)) + cos((freq + 1.0f) * PI * (t * 2 + offset)) );
                    
                    t = (mTs[i] - elapsedSec) / mDurations[i];
                    dreamPos.Y -= amp * ( sin(freq * PI * (t * 2 + offset)) + cos((freq + 1.0f) * PI * (t * 2 + offset)) );
                }
                
                // Finally, see if the animation is done (animated the entire duration)
                if (mTs[i] > mDurations[i])
                {
                    // Clear this entry
                    mTypes.erase( mTypes.begin()+i );
                    mFroms.erase( mFroms.begin()+i );
                    mTos.erase( mTos.begin()+i );
                    mDurations.erase( mDurations.begin()+i );
                    mTs.erase( mTs.begin()+i );
                }
            }
            
            // Check if animations are done...animating
            if (mTypes.size() == 0)
            {
                mAnimationSpeed = 1.0f;
                mEmpty = true;
            }
            
            // Finally, apply the changes
            mPos += dreamPos;
            mSize += dreamSize;
        }
    }

    // --------========--------========--------========--------========--------========--------========--------========
    void Camera::updateViewport()
    {
        // Resize the viewport appropiately
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        
        Vector2 camHalfSize = mSize * 0.5f;
#if OPENGLES
        glOrthof( mPos.X - camHalfSize.X, mPos.X + camHalfSize.X, mPos.Y - camHalfSize.Y, mPos.Y + camHalfSize.Y, 0.05f, 5.0f );
#else
        glOrtho( mPos.X - camHalfSize.X, mPos.X + camHalfSize.X, mPos.Y - camHalfSize.Y, mPos.Y + camHalfSize.Y, 0.05f, 5.0f );
#endif
    }
    
    // --------========--------========--------========--------========--------========--------========--------========
    void Camera::updateViewport(bool shouldInvert)
    {
        if (shouldInvert)
        {
            // Resize the viewport appropiately
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            
            Vector2 camHalfSize = mSize * 0.5f;
#if OPENGLES
            glOrthof( mPos.X - camHalfSize.X, mPos.X + camHalfSize.X, mPos.Y + camHalfSize.Y, mPos.Y - camHalfSize.Y, 0.05f, 10.0f );
#else
            glOrtho( mPos.X - camHalfSize.X, mPos.X + camHalfSize.X, mPos.Y + camHalfSize.Y, mPos.Y 
                    - camHalfSize.Y, 0.05f, 10.0f );
#endif

        }
        else
        {
            // Resize the viewport appropiately
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            
            Vector2 camHalfSize = mSize * 0.5f;
#if OPENGLES
            glOrthof( mPos.X - camHalfSize.X, mPos.X + camHalfSize.X, mPos.Y - camHalfSize.Y, mPos.Y + camHalfSize.Y, 0.05f, 5.0f );
#else
            glOrtho( mPos.X - camHalfSize.X, mPos.X + camHalfSize.X, mPos.Y - camHalfSize.Y, mPos.Y + camHalfSize.Y, 0.05f, 5.0f );
#endif

        }
    }
    
    // --------========--------========--------========--------========--------========--------========--------========
    void Camera::restoreViewport()
    {
        // draw UI
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
#if OPENGLES    
        glOrthof( 0, ScreenCoord::getScreenResolution().X, ScreenCoord::getScreenResolution().Y, 0, -1.0f, 1.0f );
#else
        glOrtho( 0, ScreenCoord::getScreenResolution().X, ScreenCoord::getScreenResolution().Y, 0, -1.0f, 1.0f );
#endif
    }
}
