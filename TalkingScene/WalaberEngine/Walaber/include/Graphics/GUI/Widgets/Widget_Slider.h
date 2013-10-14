#ifndef _WALABER_WIDGET_SLIDER_H_
#define _WALABER_WIDGET_SLIDER_H_

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
	
	class Widget_ScrollableSet;
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	// widget for a slider
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	class Widget_Slider : public Widget
	{
	public:
		Widget_Slider( int name, Vector2 pos, Vector2 size );
		Widget_Slider( int name, Vector2 pos, Vector2 size, TexturePtr bg, TexturePtr handle, Vector2 handleSize, float min, float max, float step);
		~Widget_Slider();
		
		void init();
		
		void setBGTexture( TexturePtr bg ) { mTextureBG = bg; }
		void setHandleTexture( TexturePtr handle ) { mTextureHandle = handle; }
		
		void setStepTexture( TexturePtr step ) { mStepTexture = step; }
		void setLineTexture( TexturePtr line ) { mLineTexture = line; }
		
		void setMinSlide( float minSlide ) { mMin = minSlide; }
		void setMaxSlide( float maxSlide ) { mMax = maxSlide; }
		void setStepSlide( float stepSlide ) { mStep = stepSlide; }
		
		void setHandleSize( Vector2 handleSize );
		void setStepSize( Vector2 handleSize );
		
		bool update( float elapsedSec, WidgetActionRet& ret );
		void draw( SpriteBatch* sb );//, const Vector2& offset );
		
		void reset() { mFinger = NULL; }
		
		void linkScrollSet( Widget_ScrollableSet* set );
		
		bool acceptNewFingerDown( int fingerID, FingerInfo* info );
		bool acceptNewFingerEntered( int fingerID, FingerInfo* info );
		bool releaseFingerStayed( int fingerID, FingerInfo* info );
		bool releaseFingerMoved( int fingerID, FingerInfo* info );
		void releaseFingerUp( int fingerID, FingerInfo* info );
		bool releaseFingerLeft( int fingerID, FingerInfo* info );
		void notifyFingerLost( int fingerID, FingerInfo* info );
		
		void setValue(float val);
		float getValue();
        
        float getMin() { return mMin; }
        float getMax() { return mMax; }
		
		void setMaxMove( float mm ) { mMaxMove = mm; }
		
		void setHandleAngle( float a ) { mHandleAngle = a; }
		
		bool hasFinger() { return (mFinger != NULL); }
		
		void setDrawSteps( bool visible ) { mDrawSteps = visible; }
		void setManualDrawStepCount( int c ) { mDrawStepCount = c; }
		
		void setDrawStepHilightID( int i, Color c = Color::Red ) 
		{ 
			mHilightDrawStep = i; 
			mHilightElapsed = 0.0f;
			mHilightDrawColor = c;
		}
		
	private:
		void _setValue();
		
		FingerInfo*			mFinger;
		
		TexturePtr			mTextureBG;
		TexturePtr			mTextureHandle;
		
		TexturePtr			mStepTexture;
		TexturePtr			mLineTexture;
		
		Vector2				mHandleSize;		
		Vector2				mSliderArea;
		
		Vector2				mStepSize;
		Vector2				mLineSize;
		
		Vector2				mFingerOffset;
		
		float				mMin;
		float				mMax;
		float				mStep;
		float				mVal;
		float				mMaxMove;		// max percentage of range to move in 1 update.		
		bool				mValChanged;
		
		float				mHandleAngle;
		
		bool				mDrawSteps;
		int					mDrawStepCount;
		int					mHilightDrawStep;
		Color				mHilightDrawColor;
		
		float				mHilightElapsed;
		
		Widget_ScrollableSet*		mSet;
	};
}

#endif	// _WALABER_WIDGET_SLIDER_H_