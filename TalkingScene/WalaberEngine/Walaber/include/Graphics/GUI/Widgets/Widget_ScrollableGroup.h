#ifndef _WALABER_WIDGET_SCROLLABLE_GROUP_H_
#define _WALABER_WIDGET_SCROLLABLE_GROUP_H_

#include "GUI.h"
#include "QuadHelper.h"
#include "CircleHelper.h"
#include "Rect.h"
#include "Color.h"

#include "Widget_Group.h"

#include <Texture.h>
#include <string>
#include <vector>
#include <list>

namespace Walaber
{
	class SpriteBatch; 
	class BitmapFont; 
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	// CHAD-O-TYPE: widget for a scrollable group (which may contain any number of widgets)
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	class Widget_ScrollableGroup : public Widget
	{
	public:
		Widget_ScrollableGroup( int name, Vector2 pos, Vector2 size );
		~Widget_ScrollableGroup();
		
		void init();
		
		void setGroup( Widget_Group* group );
		
		void setDirectionMask( Vector2 mask );
		
		void snapTo( int widgetName );
		void snapTo( Vector2 relativePosition );
		
		void dollyTo( int widgetName, float duration = 0.25f );
		void dollyFromTo( int fromWidget, int toWidget, float duration = 0.25f );
		
		void dollyTo( Vector2 relativePosition, float duration = 0.25f );
		void dollyFromTo( Vector2 fromRelativePosition, Vector2 toRelativePosition, float duration = 0.25f );
		
		bool isAnimating() { if (mAnimateT != -1) { return true; } return false; };
		
		bool update( float elapsedSec, WidgetActionRet& ret );
		void draw( SpriteBatch* sb );//, const Vector2& offset );
		
		void setLimitPixels( int pixels ) { mLimitPixels = pixels; };
		
		void addSnapWidget( int widgetName ) { mSnapWidgets.push_back( widgetName ); }
		
		bool acceptNewFingerDown( int fingerID, FingerInfo* info );
		bool acceptNewFingerEntered( int fingerID, FingerInfo* info );
		bool releaseFingerStayed( int fingerID, FingerInfo* info );
		bool releaseFingerMoved( int fingerID, FingerInfo* info );
		void releaseFingerUp( int fingerID, FingerInfo* info );
		bool releaseFingerLeft( int fingerID, FingerInfo* info );
		void notifyFingerLost( int fingerID, FingerInfo* info );
		
	private:
		
		Vector2 _clampInBounds( const Vector2& pos );
		
		Vector2 _relativePositionToOffset( const Vector2& relativePosition );
		Vector2 _widgetPositionToOffset( int widgetName );
		void _dollyFromTo( const Vector2& fromPos, const Vector2& toPos, float duration = 0.25f );
		
		void _updateFinger(bool firstFrame = false);
		
		FingerInfo*			mFinger;
		
		int					mFirstFinger; 
		
		Widget_Group*		mGroup;
		Vector2				mSize;
		
		Vector2				mUpperLeft;
		Vector2				mLowerRight;
		
		Vector2				mOffset;
		Vector2				mLastOffsetDelta;
		
		Vector2				mOriginalPosition;
		
		float				mLastTouchElapsed;
		
		Vector2				mVel;
		float				mVelDamping;
		
		Vector2				mDirectionMask;
		
		float				mAnimateT;
		float				mAnimateDuration;
		Vector2				mAnimateFrom;
		Vector2				mAnimateTo;
		
		int					mLimitPixels;
		
		std::vector<int>	mSnapWidgets;
		float				mMinSpeed;
		Vector2				mDesiredOffset;
        
        Vector2             mPositionAtDown;
        float               mScrollIntentThreshold;
	};
}

#endif	// _WALABER_WIDGET_SCROLLABLE_GROUP_H_