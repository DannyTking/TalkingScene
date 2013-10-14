#ifndef _WALABER_WIDGET_TIMED_BUTTON_H_
#define _WALABER_WIDGET_TIMED_BUTTON_H_

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
	// widget for buttons with radial "timers".
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	class Widget_TimedButton : public Widget
	{
	public:
		Widget_TimedButton( int name, Vector2 pos, Vector2 size );
		Widget_TimedButton( int name, Vector2 pos, Vector2 size, TexturePtr activeTex, TexturePtr inactiveTex, float maxTime );
		~Widget_TimedButton();
		
		void init();
		
		void setTimerColor( Color col ) {mTimerColor = col; }
		void setOutlineColor( Color col ) { mOutlineColor = col; }
		
		void setActiveTexture( TexturePtr activeTex );
		void setInactiveTexture( TexturePtr inactiveTex );
		
		void setMaxTime( float maxTime ) { mMaxTime = maxTime; }
		
		void setTimeRemaining( float time );
		float getTimeRemaining() { return mTimeLeft; }
		float getTimeRemainingPercent() { return mTimeLeft / mMaxTime; }
		
		void setActivated( bool a ) { mActivated = a; }
		bool getActivated() { return mActivated; }
		
		void setEnabled( bool e ) { mEnabled = e;  if (!e) { setActivated(false); } }
		bool getEnagled() { return mEnabled; }
		
		bool update( float elapsedSec, WidgetActionRet& ret );
		void draw( SpriteBatch* sb );//, const Vector2& offset );
		
		void reset() { mGoChange = false; }
		
		bool acceptNewFingerDown( int fingerID, FingerInfo* info );
		
	private:
		TexturePtr			mActiveTexture;
		TexturePtr			mInactiveTexture;
		Rect				mTextureRect;
		Rect				mInactiveTextureRect;
		
		bool				mEnabled;
		bool				mActivated;
		float				mTimeLeft;
		float				mMaxTime;
		
		Color				mTimerColor;
		Color				mOutlineColor;
		int					mFlashCount;
		bool				mGoChange;
	};
}

#endif	// _WALABER_WIDGET_TIMED_BUTTON_H_