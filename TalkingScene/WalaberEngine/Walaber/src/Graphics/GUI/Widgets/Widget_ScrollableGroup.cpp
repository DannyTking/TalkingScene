#include "Widget_ScrollableGroup.h"
#include "VectorTools.h"

#include "Texture.h"

#include "BitmapFont.h"
#include "GraphicsGL.h"
#include "SpriteBatch.h"

namespace Walaber
{
    static const float SCROLL_INTENT_THRESHOLD = 20.0f;
    
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	// CHAD-O-TYPE: widget for selecting from a scrollable section of widgets
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	Widget_ScrollableGroup::Widget_ScrollableGroup( int name, Vector2 pos, Vector2 size ) :
	Widget( name, WT_SCROLLABLE_GROUP, pos, size, WO_PortraitUp )
	{
		init();
		mOriginalPosition = pos;
	}
	
	
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	Widget_ScrollableGroup::~Widget_ScrollableGroup()
	{
		mGroup = NULL;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_ScrollableGroup::init()
	{
		mOffset = Vector2::Zero;
		mFirstFinger = -1;
		mVel = Vector2::Zero;
		mVelDamping = 0.9f;
		mLastTouchElapsed = 0.0f;
		mFinger = NULL;
		mDirectionMask = Vector2( 1, 1 );
		mAnimateT = -1;
		mGroup = NULL;
		mLimitPixels = 200;
		mMinSpeed = 5.0f;
		mDesiredOffset = ScreenCoord( 0.0f, -0.25f ).toScreen();
        mPositionAtDown = Vector2::Zero;
        
        mScrollIntentThreshold = (SCROLL_INTENT_THRESHOLD * ScreenCoord::getScreenResolution().X) / 768.0f;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_ScrollableGroup::setGroup( Widget_Group* group )
	{
		// Initialize variables
		mGroup = group;
		
		mSize = mGroup->getSize();
		
		mUpperLeft = (mSize - this->getSize()) * 0.5f;
		mLowerRight = (mSize - this->getSize()) * -0.5f;
		
		// Check to see if the group is big enough to fill the section
		// In this case, just snap to center
		if (mSize.X < this->getSize().X)
		{
			mUpperLeft.X = 0;
			mLowerRight.X = 0;
		}
		
		if (mSize.Y < this->getSize().Y)
		{
			mUpperLeft.Y = 0;
			mLowerRight.Y = 0;
		}
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_ScrollableGroup::setDirectionMask( Vector2 mask )
	{
		mDirectionMask = mask;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_ScrollableGroup::snapTo( int toWidget )
	{
		// Snap to widget
		mOffset = _widgetPositionToOffset( toWidget );
		mOffset *= mDirectionMask;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_ScrollableGroup::snapTo( Vector2 relativePosition )
	{
		// The position is from (-1, -1) to (1, 1), and describes where to place the group
		mOffset = _relativePositionToOffset( relativePosition );
		mOffset *= mDirectionMask;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_ScrollableGroup::dollyTo( int toWidget, float duration )
	{
		// Find the widget, and then dolly to that position (from here)
		_dollyFromTo( mOffset, _widgetPositionToOffset( toWidget ), duration );
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_ScrollableGroup::dollyFromTo( int fromWidget, int toWidget, float duration )
	{
		// Find the widgets, and then dolly between them 
		_dollyFromTo( _widgetPositionToOffset( fromWidget ), _widgetPositionToOffset( toWidget ), duration );
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_ScrollableGroup::dollyTo( Vector2 relativePosition, float duration )
	{
		// Find the widget, and then dolly to that position (from here)
		_dollyFromTo( mOffset, _relativePositionToOffset( relativePosition ), duration );
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_ScrollableGroup::dollyFromTo( Vector2 fromRelativePosition, Vector2 toRelativePosition, float duration )
	{
		// Find the widgets, and then dolly between them 
		_dollyFromTo( _relativePositionToOffset( fromRelativePosition ), _relativePositionToOffset( toRelativePosition ), duration );
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_ScrollableGroup::update( float elapsedSec, WidgetActionRet& ret )
	{
		if (mGroup != NULL)
		{
			// Position the group according to the offset
			mGroup->setLocalPosition2D( mOriginalPosition + mOffset );
			
			// Maybe we're animating!
			if (mAnimateT != -1)
			{
				mAnimateT += elapsedSec;
				
				mOffset = mAnimateFrom.slerp( mAnimateTo, clampf(mAnimateT / mAnimateDuration, 0.0f, 1.0f)) * mDirectionMask;
				
				if (mAnimateT >= mAnimateDuration)
				{
					mAnimateT = -1;
				}
				
				return false;
			}
			
			// If there's no finger onscreen, snap to the current index
			if (mFinger	== NULL) 
			{
				// Update velocity
				mVel *= mVelDamping;
				mOffset += (mVel * elapsedSec);
				
				mLastTouchElapsed += elapsedSec;
				
				// If you're past limits, do a super snap (fast)
				// Do it per axis
				if (mOffset.X > mUpperLeft.X)
				{
					// move toward zero!
					float move = (mUpperLeft.X - mOffset.X);
					if (abs(move) > 3.0f) 
					{ 
						move *= 0.5f; 
					}
					mOffset.X += move;
				}
				else if (mOffset.X < mLowerRight.X)
				{
					// move toward max!
					float move = (mLowerRight.X - mOffset.X);
					if (abs(move) > 3.0f) 
					{ 
						move *= 0.5f; 
					}
					mOffset.X += move;
				}
				
				if (mOffset.Y > mUpperLeft.Y)
				{
					// move toward zero!
					float move = (mUpperLeft.Y - mOffset.Y);
					if (abs(move) > 3.0f) 
					{ 
						move *= 0.5f; 
					}
					mOffset.Y += move;
				}
				else if (mOffset.Y < mLowerRight.Y)
				{
					// move toward max!
					float move = (mLowerRight.Y - mOffset.Y);
					if (abs(move) > 3.0f) 
					{ 
						move *= 0.5f; 
					}
					mOffset.Y += move;
				}
				
				/*
				// Is our velocity below a threshold?  Try moving to the closest snap widget
				if (!mSnapWidgets.empty())
				{
					float velDis = mVel.fastLength();
					if ((velDis > mMinSpeed / 2.0f) && (velDis < mMinSpeed))
					{
						// Find the closest snap widget, and then animate there
						float shortest = 10000;
						int shortestIndex = -1;
						
						float dis = 0.0f;
						
						for (int snapCount = 0; snapCount < mSnapWidgets.size(); snapCount++)
						{
							dis = (_widgetPositionToOffset(mSnapWidgets[snapCount]) - mOffset + mDesiredOffset).fastLength();
							
							if (dis < shortest)
							{
								shortest = dis;
								shortestIndex = mSnapWidgets[snapCount];
							}
						}
						
						// Snap to it!
						if ((shortestIndex != -1) && (shortest > 0.1f))
						{
							dollyTo(shortestIndex, 0.025f);
						}	
					}
				}
				 */
			}
			else
			{
				mVel = mLastOffsetDelta / elapsedSec;
			}
		}
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_ScrollableGroup::draw(SpriteBatch* sb)//, const Vector2& offset )
	{
		// debug outline
		Widget::draw(sb);//, offset);
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_ScrollableGroup::acceptNewFingerDown( int fingerID, FingerInfo* info )
	{
		if (!mEnabled)
		{
			return false;
		}
		
#ifdef _DEBUG
		//printf("Widget_ScrollableSet::acceptNewFingerDown(%d)\n", fingerID);
#endif
		
		// If this is the first finger down, record the index
		if (mFirstFinger == -1)
		{
			mFirstFinger = fingerID;
			mPositionAtDown = info->curPos;
			_updateFinger(true);
		}
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_ScrollableGroup::acceptNewFingerEntered( int fingerID, FingerInfo* info )
	{
		if (!mEnabled)
		{
			return false;
		}
		
#ifdef _DEBUG
		//printf("Widget_ScrollableSet::acceptNewFingerEntered(%d)\n", fingerID);
#endif
        
        float distanceMoved = (info->curPos - mPositionAtDown).fastLength();
        
        // Don't take this finger unless we know the player intended to scroll.
        if (distanceMoved < mScrollIntentThreshold)
        {
            return false;
        }
		
		// If the finger moving is the one recorded, start doing some crazy stuff
		if (mFinger == NULL) 
		{
			mFirstFinger = fingerID;
			mFinger = info;
			
			_updateFinger(true);
			
			return true;
		}
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_ScrollableGroup::releaseFingerStayed( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		//printf("Widget_ScrollableSet::releaseFingerStayed(%d)\n", fingerID);
#endif
		
		mFinger = info;
		_updateFinger();
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_ScrollableGroup::releaseFingerMoved( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		//printf("Widget_ScrollableSet::releaseFingerMoved(%d)\n", fingerID);
#endif
		
		mFinger = info;
		_updateFinger();
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_ScrollableGroup::releaseFingerUp( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		//printf("Widget_ScrollableSet::releaseFingerUp(%d)\n", fingerID);
#endif
		
		// Release recorded finger
		mFirstFinger = -1;
		mLastTouchElapsed = 0.0f;
		mFinger = NULL;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_ScrollableGroup::releaseFingerLeft( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		//printf("Widget_ScrollableSet::releaseFingerLeft(%d)\n", fingerID);
#endif
		
		// Release recorded finger
		mFirstFinger = -1;
		mLastTouchElapsed = 0.0f;
		mFinger = NULL;
		
		return true;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_ScrollableGroup::notifyFingerLost( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		//printf("Widget_ScrollableSet::notifyFingerLost(%d)\n", fingerID);
#endif
		
		// Release recorded finger
		mFirstFinger = -1;
		mLastTouchElapsed = 0.0f;
		mFinger = NULL;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	Vector2 Widget_ScrollableGroup::_clampInBounds( const Vector2& pos )
	{
		return Vector2( clampf(pos.X, mLowerRight.X, mUpperLeft.X), clampf(pos.Y, mLowerRight.Y, mUpperLeft.Y) );
	}
		
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	Vector2 Widget_ScrollableGroup::_relativePositionToOffset( const Vector2& relativePosition )
	{
		return _clampInBounds(((mSize - this->getSize()) * 0.5f) * (relativePosition * -1.0f));
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	Vector2 Widget_ScrollableGroup::_widgetPositionToOffset( int widgetName )
	{
		Vector2 pos = mGroup->getWidget( widgetName )->getWorldPosition2D() - mGroup->getWorldPosition2D();
		pos *= -1;
		
		return _clampInBounds(pos);
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_ScrollableGroup::_dollyFromTo( const Vector2& fromPos, const Vector2& toPos, float duration )
	{
		// Set the positions (into offsets), and let the update do the rest
		mAnimateFrom = _clampInBounds(fromPos);
		mAnimateTo = _clampInBounds(toPos);
		
		mAnimateT = 0.0f;
		mAnimateDuration = duration;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_ScrollableGroup::_updateFinger(bool firstFrame)
	{
		if (mAnimateT != -1)
		{
			return;
		}
		
		if (firstFrame)
		{
			mVel = Vector2::Zero;
			mLastOffsetDelta = Vector2::Zero;
		}
		else
		{
			mLastOffsetDelta = mFinger->curPos - mFinger->lastPos;
		}
		mLastOffsetDelta *= mDirectionMask;
		
		// scale this smaller as we drift away from the edges...
		Vector2 newPos = mOffset + mLastOffsetDelta;
		
		if (newPos.X > mUpperLeft.X)
		{
			// If it's really out there, stop moving
			if (newPos.X > mUpperLeft.X + mLimitPixels)
			{
				newPos.X = mOffset.X;
			}
			else
			{
				newPos.X = mOffset.X + (mLastOffsetDelta.X * 0.5f);
			}
			
		}
		else if (newPos.X < mLowerRight.X)
		{
			// If it's really out there, stop moving
			if (newPos.X < mUpperLeft.X - mLimitPixels)
			{
				newPos.X = mOffset.X;
			}
			else
			{
				newPos.X = mOffset.X + (mLastOffsetDelta.X * 0.5f);
			}
		}
		
		if (newPos.Y > mUpperLeft.Y)
		{
			// If it's really out there, stop moving
			if (newPos.Y > mUpperLeft.Y + mLimitPixels)
			{
				newPos.Y = mOffset.Y;
			}
			else
			{
				newPos.Y = mOffset.Y + (mLastOffsetDelta.Y * 0.5f);
			}
		}
		else if (newPos.Y < mLowerRight.Y)
		{
			// If it's really out there, stop moving
			if (newPos.Y < mLowerRight.Y - mLimitPixels)
			{
				newPos.Y = mOffset.Y;
			}
			else
			{
				newPos.Y = mOffset.Y + (mLastOffsetDelta.Y * 0.5f);
			}
		}
		
		newPos *= mDirectionMask;
		
		mOffset = newPos;
	}	
}


