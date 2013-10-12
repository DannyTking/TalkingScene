#include "Widget_ScrollableSet.h"
#include "VectorTools.h"

#include "Texture.h"

#include "BitmapFont.h"
#include "GraphicsGL.h"
#include "SpriteBatch.h"

#include "Widget_Slider.h"

namespace Walaber
{
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	// CHAD-O-TYPE: widget for selecting from a scrollable section of widgets
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	Widget_ScrollableSet::Widget_ScrollableSet( int name, Vector2 pos, Vector2 size ) :
	Widget( name, WT_SCROLLABLE_SET, pos, size, WO_PortraitUp )
	{
		// Initialize variables
		init();
        
		mDistanceBetween = 7 * this->getSize().X / 10;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	Widget_ScrollableSet::~Widget_ScrollableSet()
	{
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_ScrollableSet::init()
	{
		mOffset = 0.0f;
		mFirstFinger = -1;
		mIndex = 0;
		mStartIndex = 0;
		mVel = 0.0f;
		mVelDamping = 0.9f;
		mLastTouchElapsed = 0.0f;
		mFinger = NULL;
		mSize = 0;
		mAnimateT = -1;
		mAnimateToIndex = -1;
		mAnimateDuration = -1;
		mSlider = NULL;
        mCamera = NULL;
        mCameraMode = false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_ScrollableSet::update( float elapsedSec, WidgetActionRet& ret )
	{
		if (mCameraMode && mCamera != NULL)
        {
            // move camera and not the widgets
            mCamera->setPosition( Vector2(mCamera->getPosition().X + mOffset, mCamera->getPosition().Y ));
            
            Logger::printf("Walaber", Logger::SV_DEBUG, "Widget_ScrollableSet::(mOffSet:%f)\n", mOffset);
            Logger::printf("Walaber", Logger::SV_DEBUG, "Widget_ScrollableSet::(mIndex:%d)\n", mIndex);
            
        }
        else
        {
            // Position all of the widgets according to the offset
            for (int i = 0; i < mWidgets.size(); i++) 
            {
                mWidgets[i]->setLocalPosition2D(Vector2(mOrigXs[i] + mOffset, mWidgets[i]->getWorldPosition().Y));
            }
        }
		
		// Maybe we're animating!
		if (mAnimateToIndex != -1)
		{
			if (mCameraMode)
            {
                mOffset = slerp( mIndex * mDistanceBetween, mAnimateToIndex * mDistanceBetween, clampf(mAnimateT, 0.0f, 1.0f));
            }
            else 
            {
                mOffset = slerp( -1 * mIndex * mDistanceBetween, -1 * mAnimateToIndex * mDistanceBetween, clampf(mAnimateT, 0.0f, 1.0f));
            }
            
			mAnimateT += elapsedSec / mAnimateDuration;
			
			if (mAnimateT >= mAnimateDuration * 1.25f)
			{
				mIndex = mAnimateToIndex;
				mAnimateToIndex = -1;
				mAnimateT = -1;
			}
			
			// Update the slider
			if (mSlider)
			{
				// No finger?  Do as we please
				if (!mSlider->hasFinger())
				{
					float newVal = clampf( fabs( mOffset / mDistanceBetween ) / (mSize - 1.0f), 0.0f, 1.0f );
					
					if (mAnimateT >= mAnimateDuration)
					{
						newVal = clampf( mAnimateToIndex / (mSize - 1.0f), 0.0f, 1.0f );
					}
					
					mSlider->setValue( newVal );
				}
			}
			
			return false;
		}
		
		// If there's no finger onscreen, snap to the current index
		bool moved = (mFinger != NULL);
		
		if (mFinger	== NULL) 
		{
			// Update velocity
			mVel *= mVelDamping;
			mOffset += (mVel * elapsedSec);
			
			// On the first update, snap to a new index if there's enough velocity
			if (mLastTouchElapsed == 0)
			{
				if (mIndex == mStartIndex)
				{
					if (mVel < 0)
					{
						//if (mIndex)
                        mIndex++;
						moved = true;
					}
					else if (mVel > 0) 
					{
						mIndex--;
						moved = true;
					}
				}
			}
			
			mLastTouchElapsed += elapsedSec;
			
			// If you're past limits, do a super snap (fast)
			
			if (mOffset > 0)
			{
				// move toward zero!
				float move = (0 - mOffset);
				if (abs(move) > 3.0f) { move *= 0.5f; }
				mOffset += move;
				
				mIndex = 0;
				moved = true;
			}
			else if (mOffset < -1 * (mSize - 1.0f) * mDistanceBetween)
			{
				// move toward max!
				float move = ((-1 * (mSize - 1.0f) * mDistanceBetween) - mOffset);
				if (abs(move) > 3.0f) { move *= 0.5f; }
				mOffset += move;
				
				mIndex = mSize - 1;
				moved = true;
			}
			
			// Otherwise, try and snap to the current index
			else if (mLastTouchElapsed > -0.25f)
			{
				if (mOffset > -1 * mIndex * mDistanceBetween)
				{
					// move toward zero!
					float move = ((-1 * mIndex * mDistanceBetween) - mOffset);
					if (abs(move) > 1.0f) { move *= 0.25f; }
					
					mOffset += move;
					moved = true;
				}
				else if (mOffset < -1 * mIndex * mDistanceBetween)
				{
					// move toward max!
					float move = ((-1 * mIndex * mDistanceBetween) - mOffset);
					if (abs(move) > 1.0f) { move *= 0.25f; }
					
					mOffset += move;
					moved = true;
				}	
			}
		}
		else
		{
			mVel = mLastOffsetDelta / elapsedSec;
		}
		
		// Update the slider
		if (moved)
			_updateSlider();
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_ScrollableSet::draw(SpriteBatch* sb)//, const Vector2& offset )
	{
		// debug outline
		Widget::draw(sb);//, offset);
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_ScrollableSet::addGroup( Widget_Group* wp )
	{
		if (mCameraMode && mCamera != NULL)
        {
//            wp->setLocalPosition2D(Vector2(this->getWorldPosition().X + mSize * mDistanceBetween, this->getWorldPosition().Y + wp->getWorldPosition().Y));
        }
        else
        {
            // Set widget's initial position
            wp->setLocalPosition2D(Vector2(this->getWorldPosition().X + mSize * mDistanceBetween, this->getWorldPosition().Y + wp->getWorldPosition().Y));
        }
		
		// add this widget to the list
		mWidgets.push_back( wp );
		
		mOrigXs.push_back( wp->getWorldPosition().X );
		
		mSize++;
	}
    
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_ScrollableSet::setIndex( int newIndex )
	{
		mIndex = clampi(newIndex, 0, mSize - 1);
		
		mOffset = -1 * mIndex * mDistanceBetween;
		
		_updateSlider();
	}
    
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	int Widget_ScrollableSet::getCurrentIndex()
	{
		return mIndex;
	}
    
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
    void Widget_ScrollableSet::setCameraMode(bool useCamera)
    {
        mCameraMode = useCamera;
    }
    
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
    bool Widget_ScrollableSet::getCameraMode()
    {
        return mCameraMode;
    }
    
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
    void Widget_ScrollableSet::setCamera(Camera* camera)
    {
        mCamera = camera;
    }
    
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_ScrollableSet::dollyTo( int toIndex, float duration )
	{
		mAnimateDuration = duration;
		mAnimateToIndex = clampi(toIndex, 0, mSize - 1);
		mAnimateT = mAnimateDuration * -0.25f;
	}
    
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_ScrollableSet::dollyFromTo( int fromIndex, int toIndex, float duration )
	{
		setIndex(fromIndex);
		
		mAnimateDuration = duration;
		mAnimateToIndex = clampi(toIndex, 0, mSize - 1);
		mAnimateT = mAnimateDuration * -0.25f;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_ScrollableSet::setDistanceBetween( float newDistance )
	{
		mDistanceBetween = newDistance;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_ScrollableSet::linkSlider( Widget_Slider* slider )
	{
		mSlider = slider;
		
		float val = 1.0f / (mSize - 1.0f);
		mSlider->setStepSlide( val * 0.01f );
		mSlider->setManualDrawStepCount( mSize - 1 );
		
		slider->linkScrollSet(this);
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_ScrollableSet::sliderMoved( Widget_Slider* slider )
	{
		if (slider != mSlider)
			return;
		
		float max = -1 * (mSize - 1.0f) * mDistanceBetween;
		float t = mSlider->getValue();
		
		mOffset = lerp( 0.0f, max, t );
		mIndex = (int)(lerp( 0.0f, (float)(mSize - 1), t ) + 0.5f);
		mVel = 0.0f;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_ScrollableSet::acceptNewFingerDown( int fingerID, FingerInfo* info )
	{
		if (!mEnabled)
		{
			return false;
		}
		
#ifdef _DEBUG
		printf("Widget_ScrollableSet::acceptNewFingerDown(%d)\n", fingerID);
#endif
		
		// If this is the first finger down, record the index
		if ((mFirstFinger == -1) && (mAnimateToIndex == -1))
		{
			mFirstFinger = fingerID;
			mStartIndex = mIndex;
			
			_updateFinger(true);
		}
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_ScrollableSet::acceptNewFingerEntered( int fingerID, FingerInfo* info )
	{
		if (!mEnabled)
		{
			return false;
		}
		
#ifdef _DEBUG
		printf("Widget_ScrollableSet::acceptNewFingerEntered(%d)\n", fingerID);
#endif
		
		// If the finger moving is the one recorded, start doing some crazy stuff
		if (mFinger == NULL) 
		{
			mFirstFinger = fingerID;
			mStartIndex = mIndex;
			mFinger = info;
			
			_updateFinger(true);
			
			return true;
		}
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_ScrollableSet::releaseFingerStayed( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		printf("Widget_ScrollableSet::releaseFingerStayed(%d)\n", fingerID);
#endif
		
		mFinger = info;
		_updateFinger();
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_ScrollableSet::releaseFingerMoved( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		printf("Widget_ScrollableSet::releaseFingerMoved(%d)\n", fingerID);
#endif
		
		mFinger = info;
		_updateFinger();
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_ScrollableSet::releaseFingerUp( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		printf("Widget_ScrollableSet::releaseFingerUp(%d)\n", fingerID);
#endif
		
		// Release recorded finger
		mFirstFinger = -1;
		mLastTouchElapsed = 0.0f;
		mFinger = NULL;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_ScrollableSet::releaseFingerLeft( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		printf("Widget_ScrollableSet::releaseFingerLeft(%d)\n", fingerID);
#endif
		
		// Release recorded finger
		mFirstFinger = -1;
		mLastTouchElapsed = 0.0f;
		mFinger = NULL;
		
		return true;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_ScrollableSet::notifyFingerLost( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		printf("Widget_ScrollableSet::notifyFingerLost(%d)\n", fingerID);
#endif
		
		// Release recorded finger
		mFirstFinger = -1;
		mLastTouchElapsed = 0.0f;
		mFinger = NULL;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_ScrollableSet::_updateFinger(bool firstFrame)
	{	
		if (mAnimateT != -1) 
		{
			return;
		}
		
		if (firstFrame)
		{
			mVel = 0.0f;
			mLastOffsetDelta = 0.0f;
		}
		else 
		{
			mLastOffsetDelta = mFinger->curPos.X - mFinger->lastPos.X;
		}
		
		// scale this smaller as we drift away from the edges...
		float newPos = mOffset + mLastOffsetDelta;
		
		if (newPos > 0.0f)
		{
			newPos = mOffset + (mLastOffsetDelta * 0.5f);
		}
		else if (newPos < -1 * (mSize - 1.0f) * mDistanceBetween)
		{
			newPos = mOffset + (mLastOffsetDelta * 0.5f);
		}
		
		mOffset = newPos;
		
		
		// Determine which element is the index
		mIndex = (abs(mOffset) + (mDistanceBetween / 2)) / mDistanceBetween;
		
		if (mIndex >= mSize) 
		{
			mIndex = mSize - 1;
		}
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_ScrollableSet::_updateSlider()
	{
		if (mSlider)
		{
			// No finger?  Do as we please
			if (!mSlider->hasFinger())
			{
				float max = -1 * (mSize - 1.0f) * mDistanceBetween;
				float newVal = clampf( mOffset / max, 0.0f, 1.0f );
				
				// Edge case
				if (mOffset > 0)
				{
					newVal = 0.0f;
				}
				
				mSlider->setValue( newVal );
			}
		}
	}
}


