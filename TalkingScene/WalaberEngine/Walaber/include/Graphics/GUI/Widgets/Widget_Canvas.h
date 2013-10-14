#ifndef _WALABER_WIDGET_CANVAS_H_
#define _WALABER_WIDGET_CANVAS_H_

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
	// widget for a simple canvas, that forwards basic information about 1 or 2 fingers interacting with it.
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	class Widget_Canvas : public Widget
	{
	public:
		enum CanvasReturnVal
		{
			CRV_None,
			CRV_Touch,	// when a new finger touches the canvas, and there are currently no other fingers down.
			CRV_Drag,	// when a single finger moves on the canvas.
			CRV_Tap,	// when a single finger touches and then releases quickly in a single spot.
			CRV_Up,		// when a single finger leaves the screen.
			CRV_TwoFingers	// when there are 2 fingers on the canvas.
		};
		
		Widget_Canvas( int name, Vector2 pos, Vector2 size );
		~Widget_Canvas();
		
		void init();
		
		// valid properties:
		// "TapDuration", "TapThreshold", "AcceptsEntered"
		void setProperties( const PropertyList& plist );
		
		bool update( float elapsedSec, WidgetActionRet& ret );
		
		void reset() { mMainFinger = mSecondaryFinger = NULL; }
        
        void setReactToSlideOn( bool react ) { mAcceptsEntered = react; }
		
		bool acceptNewFingerDown( int fingerID, FingerInfo* info );
		bool acceptNewFingerEntered( int fingerID, FingerInfo* info );
		bool releaseFingerStayed( int fingerID, FingerInfo* info );
		bool releaseFingerMoved( int fingerID, FingerInfo* info );
		void releaseFingerUp( int fingerID, FingerInfo* info );
		bool releaseFingerLeft( int fingerID, FingerInfo* info );
		void notifyFingerLost( int fingerID, FingerInfo* info );
		
		bool hasMainFinger() { return (mMainFinger != NULL); }
		Vector2 getMainFingerPos() { return mMainFinger->curPos; }
		
		bool hasTwoFingers() { return ((mMainFinger != NULL) && (mSecondaryFinger != NULL)); }
		FingerInfo* getFinger( int finger_id ) { if (finger_id == 0) { return mMainFinger; } else if (finger_id == 1) { return mSecondaryFinger; } return NULL; }
		
        int getFingerID( int finger_id ) { if (finger_id == 0) { return mFingerIDMain; } else if (finger_id == 1) { return mFingerIDSecondary; } return 0; }
		
		void setTapThresholds( float duration, float distance ) { mTapDuration = duration; mTapThreshold = distance; }
		float getTapDuration() { return mTapDuration; }
		float getTapDistance() { return mTapThreshold; }
		
	private:
		bool _acceptFinger( int fingerID, FingerInfo* info );
		bool _releaseFinger( int fingerID, FingerInfo* info );
		
		FingerInfo*			mMainFinger;
		FingerInfo*			mSecondaryFinger;
		
		float				mTapTimer;
		Vector2				mTapPos;
        
        int                 mFingerIDMain;
        int                 mFingerIDSecondary;
		
		CanvasReturnVal		mRetVal;
        bool                mAcceptsEntered;
		
		float				mTapDuration;
		float				mTapThreshold;
	};
}

#endif	// _WALABER_WIDGET_CANVAS_H_