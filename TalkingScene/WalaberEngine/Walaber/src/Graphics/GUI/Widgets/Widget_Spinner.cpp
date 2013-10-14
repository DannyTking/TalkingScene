#include "Widget_Spinner.h"
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
	Widget_Spinner::Widget_Spinner( int name, Vector2 pos, Vector2 size ) :
	Widget( name, WT_SPINNER, pos, size, WO_PortraitUp )
	{
		init();
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	Widget_Spinner::Widget_Spinner( int name, Vector2 pos, Vector2 size, TexturePtr texName) :
	Widget( name, WT_SPINNER, pos, size, WO_PortraitUp ),
	mTexture(texName)
	{
		init();
		
		if(texName.use_count())
		{
			mTextureRect = texName->getTextureRect();
		}
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	Widget_Spinner::~Widget_Spinner()
	{
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_Spinner::init()
	{
		mHilightColor = Color( 0.5f, 0.45f, 0.15f, 0.5f );
		mOmega = 0.0f;
		mOmegaDamping = 0.95f;
		mAngle = 0.0f;
		mLastDeltaAngle = 0.0f;
		mFinger = NULL;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_Spinner::setTexture( TexturePtr texName )
	{
		mTexture = texName;
		if(texName.use_count())
		{
			mTextureRect = texName->getTextureRect();
		}
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	bool Widget_Spinner::update( float elapsedSec, WidgetActionRet& ret )
	{
#ifdef _DEBUG
		//printf("Widget_Spinner::update -- mFinger(%p)\n", mFinger);
#endif
		if (mFinger == NULL)
		{
			mOmega *= mOmegaDamping;
			mAngle += (mOmega * elapsedSec);
		}
		else
		{
			mOmega = mLastDeltaAngle / elapsedSec;
		}
		
		if (mFinger != NULL)
		{
			ret.valFloat1 = mAngle;
			ret.valFloat2 = mOmega;
			return true;
		}
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_Spinner::draw(SpriteBatch* sb)//, const Vector2& offset )
	{
		float angle = _getAngle();
		
		if (mTexture.use_count())
		{
			sb->drawQuad(mLayer, mTexture, getWorldPosition2D() + mDrawOffset, mAngle + angle, Vector2(getSize().X, getSize().Y), mTextureRect );
			
			if (mFinger != NULL)
			{
				CircleHelper::drawFilledCircleSegment( getWorldPosition2D() + mDrawOffset, 0.0f, getSize().X * 0.48f, 360.0f, true, mHilightColor );
			}
		}
		
		// debug outline
		Widget::draw(sb);//, offset);
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_Spinner::acceptNewFingerDown( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		printf("Widget_Spinner::acceptNewFingerDown(%d)\n", fingerID);
#endif
		if (mFinger == NULL)
		{
			if ((info->curPos - getWorldPosition2D()).length() <= getSize().X * 0.5f)
			{
				mFinger = info;
				return true;
			}
		}
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_Spinner::acceptNewFingerEntered( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		printf("Widget_Spinner::acceptNewFingerEntered(%d)\n", fingerID);
#endif
		if (mFinger == NULL)
		{
			if ((info->curPos - getWorldPosition2D()).length() <= getSize().X * 0.5f)
			{
				mFinger = info;
				return true;
			}
		}
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_Spinner::releaseFingerStayed( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		printf("Widget_Spinner::releaseFingerStayed(%d)\n", fingerID);
#endif
		if (info == mFinger)
		{
			_updateFinger();
		}
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_Spinner::releaseFingerMoved( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		printf("Widget_Spinner::releaseFingerMoved(%d)\n", fingerID);
#endif
		if (info == mFinger)
		{
			_updateFinger();
		}
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_Spinner::releaseFingerUp( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		printf("Widget_Spinner::releaseFingerUp(%d)\n", fingerID);
#endif
		if (info == mFinger)
		{
			mFinger = NULL;
		}
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_Spinner::releaseFingerLeft( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		printf("Widget_Spinner::releaseFingerLeft(%d)\n", fingerID);
#endif
		if (info == mFinger)
		{
			_updateFinger();
			
			mFinger = NULL;
		}
		
		return true;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_Spinner::notifyFingerLost( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		printf("Widget_Spinner::notifyFingerLost(%d)\n", fingerID);
#endif
		if (info == mFinger)
			mFinger = NULL;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_Spinner::_updateFinger()
	{
		Vector2 toFingerNow = mFinger->curPos - getWorldPosition2D();
		toFingerNow.normalise();
		
		Vector2 toFingerBefore = mFinger->lastPos - getWorldPosition2D();
		toFingerBefore.normalise();
		
		float dot = clampf(toFingerNow.dotProduct(toFingerBefore), 0.0f, 1.0f);
		float angle = acos(dot) * (VectorTools::isCCW( toFingerNow, toFingerBefore ) ? -1.0f : 1.0f);
		//if (isnan(angle)) { angle = 0.0f; }
		
		mLastDeltaAngle = radToDeg(angle);
		
		mAngle += mLastDeltaAngle;
	}
}


