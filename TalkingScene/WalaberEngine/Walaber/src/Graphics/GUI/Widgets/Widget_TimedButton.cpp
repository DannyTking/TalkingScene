#include "Widget_TimedButton.h"
#include "VectorTools.h"

#include "Texture.h"

#include "BitmapFont.h"
#include "GraphicsGL.h"
#include "SpriteBatch.h"

namespace Walaber
{
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	Widget_TimedButton::Widget_TimedButton( int name, Vector2 pos, Vector2 size ) :
	Widget( name, WT_TIMED_BUTTON, pos, size, WO_PortraitUp )
	{
		init();
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	Widget_TimedButton::Widget_TimedButton( int name, Vector2 pos, Vector2 size, TexturePtr activeTex, TexturePtr inactiveTex, float maxTime )
	: Widget( name, WT_TIMED_BUTTON, pos, size, WO_PortraitUp ), 
	mActiveTexture(activeTex),
	mInactiveTexture(inactiveTex),
	mMaxTime(maxTime)
	{
		init();
		
		if(activeTex.use_count())
		{
			mTextureRect = activeTex->getTextureRect();
		}
		
		if(inactiveTex.use_count())
		{
			mInactiveTextureRect = inactiveTex->getTextureRect();
		}
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	Widget_TimedButton::~Widget_TimedButton()
	{
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	void Widget_TimedButton::init()
	{
		mEnabled = false;
		mActivated = false;
		mTimeLeft = 0.0f;
		mTimerColor = Color(0.0f, 0.0f, 0.0f, 0.6f);
		mOutlineColor = Color( 0.5f, 0.45f, 0.15f, 0.5f );
		mGoChange = false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	void Widget_TimedButton::setActiveTexture( TexturePtr activeTex )
	{
		mActiveTexture = activeTex;
		
		if(activeTex.use_count())
		{
			mTextureRect = activeTex->getTextureRect();
		} 
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	void Widget_TimedButton::setInactiveTexture( TexturePtr inactiveTex )
	{
		mInactiveTexture = inactiveTex;
		
		if(inactiveTex.use_count())
		{
			mInactiveTextureRect = inactiveTex->getTextureRect();
		}
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	void Widget_TimedButton::setTimeRemaining( float time )
	{
		mTimeLeft = time;
		if (mTimeLeft > mMaxTime) { mTimeLeft = mMaxTime; }
		
		if (mTimeLeft > 0.0f)
		{
			mEnabled = true;
		}
		else
		{
			mTimeLeft = 0.0f;
			mEnabled = false;
			mActivated = false;
		}
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	bool Widget_TimedButton::update( float elapsedSec, WidgetActionRet& ret )
	{
		if (mActivated || mGoChange)
		{
			if (mGoChange)
				ret.valBool = true;
			else
				ret.valBool = false;
			
			mGoChange = false;
			
			mTimeLeft -= elapsedSec;
			if (mTimeLeft <= 0.0f)
			{
				mTimeLeft = 0.0f;
				mActivated = false;
				mEnabled = false;
			}
			
			mFlashCount++;
			if (mFlashCount > 15) { mFlashCount = 0; }
			
			ret.valInt1 = mActivated;
			ret.valFloat1 = mTimeLeft;
			ret.valFloat2 = (mTimeLeft / mMaxTime);	// percent time left.
			return true;
		}
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	void Widget_TimedButton::draw(SpriteBatch* sb)//, const Vector2& offset )
	{
		TexturePtr drawTex = (mEnabled ? mActiveTexture : mInactiveTexture);
		
		float angle = _getAngle();
		
		if (drawTex.use_count())
		{
			if (mEnabled)
			{
				CircleHelper::drawFilledCircleSegment( getWorldPosition2D() + mDrawOffset, angle, (getSize().X * 0.5f) + 4.0f, (mTimeLeft/mMaxTime) * 360.0f, false, mOutlineColor );
			}
			sb->drawQuad(mLayer, drawTex, getWorldPosition2D() + mDrawOffset, angle, Vector2(getSize().X, getSize().Y), (mEnabled ? mTextureRect : mInactiveTextureRect) );
			
			// hack since there's no Cirlce drawing options in the sprite batch yet
			sb->flush();
			sb->start(SpriteBatch::BM_LAYERS);
		}
		
		if (mEnabled)
		{
			if (mActivated)
			{
				if (mFlashCount < 3)
				{
					CircleHelper::drawFilledCircleSegment( getWorldPosition2D() + mDrawOffset, angle, getSize().X * 0.5f, 360.0f, true, Color(0.4f, 0.4f, 0.0f, 0.4f) );
				}
			}
			
			CircleHelper::drawFilledCircleSegment( getWorldPosition2D() + mDrawOffset, angle + 180.0f, getSize().X * 0.5f, (1.0f - (mTimeLeft/mMaxTime)) * 360.0f, true, mTimerColor );
		}
		
		// debug outline
		Widget::draw(sb);//, offset);
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	bool Widget_TimedButton::acceptNewFingerDown( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		//printf("Widget_TimedButton::acceptNewFingerDown(%d)\n", fingerID);
#endif
		if (!mEnabled)
			return false;
		
		//printf("TIMED BUTTON ACCEPTS FINGER\n");
		if (!mActivated)
		{
			if (mTimeLeft > 0.0f)
			{
				mActivated = true;
				mGoChange = true;
			}
		}
		else
		{
			mActivated = false;
			mGoChange = true;
		}
		
		return true;
	}
}


