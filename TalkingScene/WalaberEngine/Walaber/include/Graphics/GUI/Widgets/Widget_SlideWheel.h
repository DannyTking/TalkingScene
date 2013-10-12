#ifndef _WALABER_WIDGET_SLIDE_WHEEL_H_
#define _WALABER_WIDGET_SLIDE_WHEEL_H_

#include "GUI.h"
#include "QuadHelper.h"
#include "CircleHelper.h"
#include "Rect.h"
#include "Color.h"


#include <Texture.h>
#include <string>
#include <vector>
#include <list>

namespace Walaber
{
	class SpriteBatch; 
	class BitmapFont; 
	
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	// widget for a "slide wheel" with momentum.
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	class Widget_SlideWheel : public Widget
	{
	public:
		Widget_SlideWheel( int name, Vector2 pos, Vector2 size );
		Widget_SlideWheel( int name, Vector2 pos, Vector2 size, TexturePtr texName);
		~Widget_SlideWheel();
        
		void init();
		
		void setTexture( TexturePtr texName ) { mTexture = texName; }
		
		float getOmega() { return mOmega; }
		void setOmega(float val) { mOmega = val; }
		
        void setOmegaDamping( float d ) { mOmegaDamping = d; }
        
		void setUVWarping( bool warpOn ) { mUVWarp = warpOn; }
		bool getUVWarping() { return mUVWarp; }
		
		void setPadding( float l, float r, float t, float b );
		
        void setUVScale( Vector2 s ) { mUVScale = s; }
        Vector2 getUVScale() { return mUVScale; }
		
		void setUVSpeedFactor( float s ) { mUVSpeed = s; }
		float getUVSpeedFactor() { return mUVSpeed; }
		
		bool update( float elapsedSec, WidgetActionRet& ret );
		void draw( SpriteBatch* sb );//, const Vector2& offset );
        
		void reset() { mFinger = NULL; }
		
		bool hasFinger() { return (mFinger != NULL); }
		FingerInfo* getFinger() { return mFinger; }
		
		bool acceptNewFingerDown( int fingerID, FingerInfo* info );
		bool acceptNewFingerEntered( int fingerID, FingerInfo* info );
		bool releaseFingerStayed( int fingerID, FingerInfo* info );
		bool releaseFingerMoved( int fingerID, FingerInfo* info );
		void releaseFingerUp( int fingerID, FingerInfo* info );
		bool releaseFingerLeft( int fingerID, FingerInfo* info );
		void notifyFingerLost( int fingerID, FingerInfo* info );
		
	private:
		
		void _updateFinger();
		
		Color				mHilightColor;
		TexturePtr			mTexture;
		
		bool				mUVWarp;
		
		float				mOmega;
		float				mOmegaDamping;
		
		float				mOffset;
		float				mLastDeltaOffset;
		
		float				mUVSpeed;
        
        Vector2				mUVScale;
		
		Vector2				mDrawPadding;
		Vector2				mDrawOffset;
		
		FingerInfo*			mFinger;
	};
}

#endif	// _WALABER_WIDGET_LABEL_H_