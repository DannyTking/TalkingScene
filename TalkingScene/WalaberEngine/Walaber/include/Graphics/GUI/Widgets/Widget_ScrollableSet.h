#ifndef _WALABER_WIDGET_SCROLLABLE_SET_H_
#define _WALABER_WIDGET_SCROLLABLE_SET_H_

#include "GUI.h"
#include "QuadHelper.h"
#include "CircleHelper.h"
#include "Rect.h"
#include "Color.h"
#include "Camera.h"

#include "Widget_Group.h"

#include <Texture.h>
#include <string>
#include <vector>
#include <list>

namespace Walaber
{
	class SpriteBatch; 
	class BitmapFont; 
	
	class Widget_Slider;
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	// CHAD-O-TYPE: widget for a scrollable set of widgets
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	class Widget_ScrollableSet : public Widget
	{
	public:
		Widget_ScrollableSet( int name, Vector2 pos, Vector2 size );
		~Widget_ScrollableSet();
		
		void init();
		
		bool update( float elapsedSec, WidgetActionRet& ret );
		void draw( SpriteBatch* sb );//, const Vector2& offset );
		
		void addGroup( Widget_Group* wp );
        
		void setIndex( int newIndex );
        int getCurrentIndex();
		int getGroupCount() { return mSize; }
		
		void dollyTo( int toIndex, float duration );
		void dollyFromTo( int fromIndex, int toIndex, float duration );
		
		void setDistanceBetween( float newDistance );
		
		void linkSlider( Widget_Slider* slider );
		void sliderMoved( Widget_Slider* slider );
        
        void setCameraMode (bool useCamera);
        bool getCameraMode ();
        
        void setCamera (Camera* mCamera);
		
		bool acceptNewFingerDown( int fingerID, FingerInfo* info );
		bool acceptNewFingerEntered( int fingerID, FingerInfo* info );
		bool releaseFingerStayed( int fingerID, FingerInfo* info );
		bool releaseFingerMoved( int fingerID, FingerInfo* info );
		void releaseFingerUp( int fingerID, FingerInfo* info );
		bool releaseFingerLeft( int fingerID, FingerInfo* info );
		void notifyFingerLost( int fingerID, FingerInfo* info );
		
	private:
		typedef std::vector<Widget_Group*>	GroupList;
		typedef std::vector<int>			PosList;
		
		void _updateFinger(bool firstFrame = false);
		
		void _updateSlider();
		
		FingerInfo*			mFinger;
		
		int					mFirstFinger; 
		int					mIndex;
        int                 mStartIndex;
		
		int					mSize;
		
		float				mOffset;
		float				mLastOffsetDelta;
		float				mDistanceBetween;
		
		float				mLastTouchElapsed;
		
		float				mVel;
		float				mVelDamping;
        
        int                 mAnimateToIndex;
        float               mAnimateT;
		float				mAnimateDuration;
		
		GroupList			mWidgets;
		PosList				mOrigXs;
		
		Widget_Slider*		mSlider;
        
        Camera*             mCamera;
        bool                mCameraMode;
	};
}

#endif	// _WALABER_WIDGET_SCROLLABLE_SET_H_
