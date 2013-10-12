#ifndef _WALABER_WIDGET_SPINNER_H_
#define _WALABER_WIDGET_SPINNER_H_

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
	// widget for a "spinner" with momentum.
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	class Widget_Spinner : public Widget
	{
	public:
		Widget_Spinner( int name, Vector2 pos, Vector2 size );
		Widget_Spinner( int name, Vector2 pos, Vector2 size, TexturePtr texName);
		~Widget_Spinner();
		
		void init();
		
		void setTexture( TexturePtr texName );
		
		float getOmega() { return mOmega; }
		void setOmega(float val) { mOmega = val; }
		
        void setOmegaDamping( float d ) { mOmegaDamping = d; }
        
		bool update( float elapsedSec, WidgetActionRet& ret );
		void draw( SpriteBatch* sb );//, const Vector2& offset );
		
		void reset() { mFinger = NULL; }
		
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
		Rect				mTextureRect;
		
		float				mOmega;
		float				mOmegaDamping;
		
		float				mAngle;
		float				mLastDeltaAngle;
		
		FingerInfo*			mFinger;
	};
}

#endif	// _WALABER_WIDGET_LABEL_H_