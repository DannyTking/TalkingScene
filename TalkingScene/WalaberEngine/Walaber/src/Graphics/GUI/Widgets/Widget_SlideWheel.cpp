#include "Widget_SlideWheel.h"
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
	Widget_SlideWheel::Widget_SlideWheel( int name, Vector2 pos, Vector2 size ) :
	Widget( name, WT_SLIDEWHEEL, pos, size, WO_PortraitUp )
	{
		init();
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	Widget_SlideWheel::Widget_SlideWheel( int name, Vector2 pos, Vector2 size, TexturePtr texName) :
	Widget( name, WT_SLIDEWHEEL, pos, size, WO_PortraitUp ),
	mTexture(texName)
	{
		init();
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	Widget_SlideWheel::~Widget_SlideWheel()
	{
		mHilightColor = Color( 0.5f, 0.45f, 0.15f, 0.5f );
		mUVWarp = true;
		mOmega = 0.0f;
		mOmegaDamping = 0.95f;
		mOffset = 0.0f;
		mLastDeltaOffset = 0.0f;
		mUVSpeed = 1.0f;
		mUVScale = Vector2::One;
		mDrawPadding = Vector2::Zero;
		mDrawOffset = Vector2::Zero;
		mFinger = NULL;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_SlideWheel::init()
	{
		
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_SlideWheel::setPadding( float l, float r, float t, float b )
	{
		float lr = l + r;
		float tb = t + b;
		
		mDrawPadding = Vector2(lr, tb);
		mDrawOffset = Vector2( l - r, t - b ) * 0.5f;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	bool Widget_SlideWheel::update( float elapsedSec, WidgetActionRet& ret )
	{
#ifdef _DEBUG
		//printf("Widget_SlideWheel::update -- mFinger(%p)\n", mFinger);
#endif
		if (mFinger == NULL)
		{
			mOmega *= mOmegaDamping;
			mOffset += (mOmega * elapsedSec);
		}
		else
		{
			float goalOmega = mLastDeltaOffset / elapsedSec;
			mOmega += (goalOmega - mOmega) * 0.8f;
		}
		
		if (mFinger != NULL)
		{
			ret.valFloat1 = mOffset;
			ret.valFloat2 = mOmega;
			return true;
		}
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_SlideWheel::draw(SpriteBatch* sb)//, const Vector2& offset )
	{
		float angle = _getAngle();
		
		//Vector2 size = getSize() - mDrawPadding;
		
		if (mUVWarp)
		{
			// we draw this as several quads to get the effect of a round object...
			
			float tP = 0.0f;
			float tV = 0.0f;
			
			float sizeP = getSize().Y - mDrawOffset.Y;
			float sizeV = -1.0f * mUVScale.Y;
			
			int steps = 16;
			
			for (int i = 1; i <= steps; i++)
			{
				float percent = (float)i / (float)steps;
				float bP = slerp( 0.0f, sizeP, percent );
				float bV = lerp( 0.0f, sizeV, percent );
				
				Rect textureRect( Vector2(0.0f, tV - (mOffset * mUVSpeed)), Vector2(1.0f, tV-bV) );
				
				Vector2 p = (getWorldPosition2D() + mDrawOffset) + VectorTools::rotateVector((Vector2(0.0f, getSize().Y * 0.5f)) - (Vector2(0.0f, lerp(tP, bP, 0.5f))), angle);
				
				sb->drawQuad(mLayer, mTexture, p, angle, Vector2(getSize().X - mDrawPadding.X, bP-tP), textureRect);
				
				tP = bP;
				tV = bV;
			}
			
		}
		else
		{
			Rect textureRect( Vector2(0.0f, (mOffset * mUVSpeed)), Vector2(1.0f, 1.0f) * mUVScale );
			
			sb->drawQuad(mLayer, mTexture, getWorldPosition2D() + mDrawOffset, angle, getSize()-mDrawPadding, textureRect);
		}
		
		// debug outline
		Widget::draw(sb);//, offset);
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_SlideWheel::acceptNewFingerDown( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		printf("Widget_SlideWheel::acceptNewFingerDown(%d)\n", fingerID);
#endif
		if (mFinger == NULL)
		{
			mFinger = info;
			mLastDeltaOffset = 0.0f;
			return true;
		}
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_SlideWheel::acceptNewFingerEntered( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		printf("Widget_SlideWheel::acceptNewFingerEntered(%d)\n", fingerID);
#endif
		if (mFinger == NULL)
		{
			mFinger = info;
			return true;
		}
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_SlideWheel::releaseFingerStayed( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		printf("Widget_SlideWheel::releaseFingerStayed(%d)\n", fingerID);
#endif
		if (info == mFinger)
		{
			_updateFinger();
		}
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_SlideWheel::releaseFingerMoved( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		printf("Widget_SlideWheel::releaseFingerMoved(%d)\n", fingerID);
#endif
		if (info == mFinger)
		{
			_updateFinger();
		}
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_SlideWheel::releaseFingerUp( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		printf("Widget_SlideWheel::releaseFingerUp(%d)\n", fingerID);
#endif
		if (info == mFinger)
		{
			mFinger = NULL;
		}
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_SlideWheel::releaseFingerLeft( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		printf("Widget_SlideWheel::releaseFingerLeft(%d)\n", fingerID);
#endif
		if (info == mFinger)
		{
			_updateFinger();
			
			mFinger = NULL;
		}
		
		return true;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_SlideWheel::notifyFingerLost( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		printf("Widget_SlideWheel::notifyFingerLost(%d)\n", fingerID);
#endif
		if (info == mFinger)
			mFinger = NULL;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_SlideWheel::_updateFinger()
	{
		float angle = _getAngle();
		
		Vector2 fingerMove = mFinger->curPos - mFinger->lastPos;
		Vector2 movementDir = VectorTools::rotateVector(Vector2(0,1), angle);
		
		float dot = movementDir.dotProduct(fingerMove);
		
		mLastDeltaOffset = dot;
		
		mOffset += mLastDeltaOffset;
	}
}


