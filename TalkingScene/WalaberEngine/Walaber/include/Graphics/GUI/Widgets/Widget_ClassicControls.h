#ifndef _WALABER_WIDGET_CLASSIC_CONTROLS_H_
#define _WALABER_WIDGET_CLASSIC_CONTROLS_H_

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
	// widget for classic JellyCar controls (half screen for driving in each direction, pinching for zooming)
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	class Widget_ClassicControls : public Widget
	{
	public:
		Widget_ClassicControls( int name, Vector2 pos, Vector2 size );
		~Widget_ClassicControls();
		
		void init();
		
		bool update( float elapsedSec, WidgetActionRet& ret );
		
		bool acceptNewFingerDown( int fingerID, FingerInfo* info );
		bool acceptNewFingerEntered( int fingerID, FingerInfo* info );
		bool releaseFingerStayed( int fingerID, FingerInfo* info );
		bool releaseFingerMoved( int fingerID, FingerInfo* info );
		void releaseFingerUp( int fingerID, FingerInfo* info );
		bool releaseFingerLeft( int fingerID, FingerInfo* info );
		void notifyFingerLost( int fingerID, FingerInfo* info );
		
		bool hasFinger() { return (mMainFinger != NULL); }
		Vector2 getMainFingerPos();
		
		void reset() { mMainFinger = mSecondaryFinger = NULL; }
		
	private:
		bool _acceptFinger( int fingerID, FingerInfo* info );
		void _releaseFinger( int fingerID, FingerInfo* info );
		
		FingerInfo*			mMainFinger;
		FingerInfo*			mSecondaryFinger;
	};
}

#endif	// _WALABER_WIDGET_CLASSIC_CONTROLS_H_