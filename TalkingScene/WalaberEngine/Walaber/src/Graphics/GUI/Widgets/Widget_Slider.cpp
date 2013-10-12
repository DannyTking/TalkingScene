#include "Widget_Slider.h"
#include "VectorTools.h"

#include "Texture.h"

#include "BitmapFont.h"
#include "GraphicsGL.h"
#include "SpriteBatch.h"

#include "Widget_ScrollableSet.h"

namespace Walaber
{
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	// widget for a slider
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	Widget_Slider::Widget_Slider( int name, Vector2 pos, Vector2 size ) : 
	Widget( name, WT_SLIDER, pos, size, WO_PortraitUp ),
	mMin(0.0f),
	mMax(1.0f),
	mStep(0.001f),
	mSet(NULL),
	mHilightDrawStep(-1)
	{
		init();
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	Widget_Slider::Widget_Slider( int name, Vector2 pos, Vector2 size, TexturePtr bg, TexturePtr handle, Vector2 handleSize, float min, float max, float step) : 
	Widget( name, WT_SLIDER, pos, size, WO_PortraitUp ),
	mTextureBG(bg),
	mTextureHandle(handle),
	mHandleSize(handleSize),
	mMin( min ),
	mMax( max ),
	mStep( step ),
	mSet(NULL),
	mHilightDrawStep(-1)
	{
		init();
		mSliderArea = size - Vector2(handleSize.X, 0.0f);
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	Widget_Slider::~Widget_Slider()
	{
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	void Widget_Slider::init()
	{
		mFinger = NULL;
		mFingerOffset = Vector2::Zero;
		mVal = 0.0f;
		mMaxMove = 1.0f;
		mValChanged = false;
		mHandleAngle = 0.0f;
		mDrawSteps = false;
		mStepSize = Vector2::One;
		mLineSize = Vector2::One;
		
		mDrawStepCount = ceilf((mMax - mMin) / mStep);
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	void Widget_Slider::setHandleSize( Vector2 handleSize )
	{
		mHandleSize = handleSize;
		mSliderArea = getSize() - Vector2(handleSize.X, 0.0f);
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	void Widget_Slider::setStepSize( Vector2 stepSize )
	{
		mStepSize = stepSize;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	bool Widget_Slider::update( float elapsedSec, WidgetActionRet& ret )
	{
		if (mHilightDrawStep >= 0)
			mHilightElapsed += elapsedSec;
		
		if (mValChanged)
		{
			mValChanged = false;
			
			ret.valFloat1 = getValue();
			
			if (mSet)
				mSet->sliderMoved(this);
			
			return true;
		}
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	void Widget_Slider::draw(SpriteBatch* sb)//, const Vector2& offset )
	{
		float angle = _getAngle();
		
		Color drawColor = (mEnabled) ? Color::White : Color(96,96,96,96);
		
		sb->drawQuad(mLayer, mTextureBG, getWorldPosition2D() + mDrawOffset, angle, Vector2(getSize().X, getSize().Y), mTextureBG->getTextureRect(), drawColor  );
		
		
		// Draw all of the steps
		if (mDrawSteps)
		{
			int steps = mDrawStepCount;
			
			Vector2 firstPos = getWorldPosition2D();
			Vector2 lastPos = getWorldPosition2D();
			
			
			// Draw a line connecting the first and last dot
			Vector2 tempSize = Vector2( lastPos.X - firstPos.X, mStepSize.Y / 3.0f );
			sb->drawQuad(mLayer, mLineTexture, (firstPos + lastPos) / 2.0f, angle, tempSize, mLineTexture->getTextureRect(), drawColor);
			
			
			for (int i = 0; i <= steps; i++)
			{
				// Difference between each dot
				//float tempX = (i) * getSize().X / (steps - 1.0f);
				
				// Offset to the middle of the slider
				//tempX -= getSize().X / 2.0f;
				
				// Offset the width of the dot
				//tempX -= mStepSize.X / 2.0f;
				
				Color dotColor = drawColor;
				float drawScale = 1.0f;
				
				if (i == mHilightDrawStep)
				{
					float l = absf( sinf( mHilightElapsed * 3.0f ) );
								   
					dotColor = drawColor.lerp( mHilightDrawColor, l );
					drawScale = 1.0f + ( l * 0.3f);					  
				}
				
				float valUnit = (i / (steps * 1.0f));
				
				Vector2 tempPos = VectorTools::rotateVector( Vector2( (valUnit - 0.5f) * mSliderArea.X, 0.0f ), angle );
				sb->drawQuad(mLayer, mStepTexture, getWorldPosition2D() + tempPos, angle, Vector2(mStepSize.X, mStepSize.Y) * drawScale, mStepTexture->getTextureRect(), dotColor);
				
				if (i == 0)
				{
					firstPos = getWorldPosition2D() + tempPos;
				}
				else if (i == steps)
				{
					lastPos = getWorldPosition2D() + tempPos;
				}
			}
			
		}
		
		
		float valUnit = (getValue() - mMin) / (mMax - mMin);
		
		Vector2 handleOffset = VectorTools::rotateVector( Vector2( (valUnit - 0.5f) * mSliderArea.X, 0.0f ), angle );

		sb->drawQuad(mLayer, mTextureHandle, getWorldPosition2D() + mDrawOffset + handleOffset, angle + mHandleAngle, Vector2(mHandleSize.X, mHandleSize.Y), mTextureHandle->getTextureRect(), drawColor );
		
		// debug outline
		Widget::draw(sb);//, offset);
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	void Widget_Slider::linkScrollSet(Widget_ScrollableSet* set)
	{
		mSet = set;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	float Widget_Slider::getValue()
	{
		float scaledVal = mMin + ((mMax - mMin) * mVal);
		int steps = (int)(scaledVal / mStep);
		
		float final =  (float)(steps * mStep);
		return final;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	void Widget_Slider::setValue(float val)
	{
		int steps = (int)(val / mStep);
		float clipped = (float)(steps * mStep);
		
		mVal = (clipped - mMin) / (mMax - mMin);
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	bool Widget_Slider::acceptNewFingerDown( int fingerID, FingerInfo* info )
	{
		if (!mEnabled)
			return false;
		
		if (mFinger == NULL)
		{
			mFinger = info;
			
			float angle = _getAngle();	
			float valUnit = (getValue() - mMin) / (mMax - mMin);
			Vector2 handleOffset = VectorTools::rotateVector( Vector2( (valUnit - 0.5f) * mSliderArea.X, 0.0f ), angle );
			
			mFingerOffset = info->curPos - (getWorldPosition2D() + handleOffset);
			
			if ((absf(mFingerOffset.X) > mHandleSize.X) || (absf(mFingerOffset.Y) > mHandleSize.Y))
			{
				mFingerOffset = Vector2::Zero;
			}
			
			_setValue();
			return true;
		}
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	bool Widget_Slider::acceptNewFingerEntered( int fingerID, FingerInfo* info )
	{
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	bool Widget_Slider::releaseFingerStayed( int fingerID, FingerInfo* info )
	{
		if (mFinger != NULL)
			_setValue();
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	bool Widget_Slider::releaseFingerMoved( int fingerID, FingerInfo* info )
	{
		if (mFinger != NULL)
			_setValue();
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	void Widget_Slider::releaseFingerUp( int fingerID, FingerInfo* info )
	{
		if (mFinger != NULL)
		{
			mFinger = NULL;
		}
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	bool Widget_Slider::releaseFingerLeft( int fingerID, FingerInfo* info )
	{
		if (mFinger!= NULL)
		{
			_setValue();
			return false;
		}
		
		return true;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	void Widget_Slider::notifyFingerLost( int fingerID, FingerInfo* info )
	{
		if (mFinger != NULL)
		{
			mFinger = NULL;
		}
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	void Widget_Slider::_setValue()
	{
		if (mFinger != NULL)
		{
			float prevVal = mVal;
			
			float angle = 90.0f;
			if (mOrientation == WO_LandscapeDown) { angle = 270.0f; }
			else if (mOrientation == WO_PortraitUp) { angle = 0.0f; }
			else if (mOrientation == WO_PortraitDown) { angle = 180.0f; }
			
			Vector2 localPos = VectorTools::rotateVector( (
                                ( mFinger->curPos - mFingerOffset) - getWorldPosition2D() ),
                                  degToRad( -angle ) );
			
			mVal = (localPos.X / (mSliderArea.X * 0.5f));
			
#ifdef _DEBUG
			printf("localPos[%4.1f,%4.1f]  mVal[%4.2f]\n",localPos.X, localPos.Y, mVal);
#endif
			
			if (mVal < -1.0f)
				mVal = -1.0f;
			else if (mVal > 1.0f)
				mVal = 1.0f;
			
			mVal = ((mVal * 0.5f) + 0.5f);
			
			if (absf(mVal - prevVal) > mMaxMove)
			{
				mVal = prevVal + (mMaxMove * signf(mVal-prevVal));
			}
			
			mValChanged = true;
		}
	}
}


