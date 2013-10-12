#include "Widget_Canvas.h"
#include "VectorTools.h"

#include "Texture.h"

#include "BitmapFont.h"
#include "GraphicsGL.h"
#include "SpriteBatch.h"

namespace Walaber
{
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	// widget for a simple canvas, that forwards basic information about 1 or 2 fingers interacting with it.
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	Widget_Canvas::Widget_Canvas( int name, Vector2 pos, Vector2 size ) :
	Widget( name, WT_CANVAS, pos, size, WO_PortraitUp ),
	mTapDuration(0.25f),
	mTapThreshold(10)
	{
		init();
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	Widget_Canvas::~Widget_Canvas()
	{
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_Canvas::init()
	{
		mMainFinger = NULL;
		mSecondaryFinger = NULL;
		mTapTimer = 0.0f;
		mTapPos = Vector2::Zero;
		mFingerIDMain = 0;
		mFingerIDSecondary = 0;
		mRetVal = CRV_None;
		mAcceptsEntered = true;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_Canvas::setProperties( const PropertyList& plist )
	{
		Widget::setProperties(plist);
		
		const Property* p = plist.getValueForKey("TapDuration");
		if (p)
			mTapDuration = p->asFloat();
		
		p = plist.getValueForKey("TapThreshold");
		if (p)
			mTapThreshold = p->asFloat();
		
		p = plist.getValueForKey("AcceptsEntered");
		if (p)
			mAcceptsEntered = (p->asInt() == 1);
	}
		
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_Canvas::update( float elapsedSec, WidgetActionRet& ret )
	{
		
		if (mRetVal == CRV_Tap)
		{
			ret.valInt1 = mRetVal;
			ret.valInt2 = mFingerIDMain;
			ret.valFloat1 = mTapPos.X;
			ret.valFloat2 = mTapPos.Y;
			mRetVal = CRV_None;
			return true;
		}
		else if (mRetVal == CRV_Drag)
		{
			ret.valInt1 = mRetVal;
			ret.valInt2 = mFingerIDMain;
			Vector2 delta = mMainFinger->curPos - mMainFinger->lastPos;
			ret.valFloat1 = delta.X;
			ret.valFloat2 = delta.Y;
			mRetVal = CRV_None;
			return true;
		}
		else if (mRetVal == CRV_Touch)
		{
			ret.valInt1 = mRetVal;
			ret.valInt2 = mFingerIDMain;
			ret.valFloat1 = mTapPos.X;
			ret.valFloat2 = mTapPos.Y;
			mRetVal = CRV_None;
			return true;
		}
		else if (mRetVal == CRV_Up)
		{
			ret.valInt1 = mRetVal;
			ret.valInt2 = mFingerIDMain;
			ret.valFloat1 = mTapPos.X;
			ret.valFloat2 = mTapPos.Y;
			mRetVal = CRV_None;
			return true;
		}
		else if ((mMainFinger != NULL) && (mSecondaryFinger != NULL))
		{
			ret.valInt1 = CRV_TwoFingers;
			ret.valInt2 = mFingerIDMain;
			
			float delta = (mMainFinger->curPos - mSecondaryFinger->curPos).length() - (mMainFinger->lastPos - mSecondaryFinger->lastPos).length();
			ret.valFloat1 = delta;
			
			return true;
		}
		
		
		mRetVal = CRV_None;
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_Canvas::acceptNewFingerDown( int fingerID, FingerInfo* info )
	{
		if(_acceptFinger(fingerID, info ))
		{
			mTapTimer = 0.0f;
			mTapPos = info->curPos;
			mRetVal = CRV_Touch;
			return true;
		}
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_Canvas::acceptNewFingerEntered( int fingerID, FingerInfo* info )
	{
		if (!mAcceptsEntered)
			return false;
		
		if (_acceptFinger(fingerID, info ))
		{
			mTapTimer = -1.0f;
			return true;
		}
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_Canvas::releaseFingerStayed( int fingerID, FingerInfo* info )
	{
		if ((mMainFinger != NULL) && (mSecondaryFinger == NULL))
		{
			if (info == mMainFinger)
			{
				mRetVal = CRV_Drag;
			}
		}
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_Canvas::releaseFingerMoved( int fingerID, FingerInfo* info )
	{
		if ((mMainFinger != NULL) && (mSecondaryFinger == NULL))
		{
			if (info == mMainFinger)
			{
				mRetVal = CRV_Drag;
			}
		}
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_Canvas::releaseFingerUp( int fingerID, FingerInfo* info )
	{
		_releaseFinger( fingerID, info );
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_Canvas::releaseFingerLeft( int fingerID, FingerInfo* info )
	{
		return _releaseFinger( fingerID, info );
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_Canvas::notifyFingerLost( int fingerID, FingerInfo* info )
	{
		_releaseFinger( fingerID, info );
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_Canvas::_acceptFinger( int fingerID, FingerInfo* info )
	{
		if (mMainFinger == NULL)
		{
			mMainFinger = info;
			mFingerIDMain = fingerID;
			return true;
		}
		else if (mSecondaryFinger == NULL)
		{
			mSecondaryFinger = info;
			mFingerIDSecondary = fingerID;
			return true;
		}
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_Canvas::_releaseFinger( int fingerID, FingerInfo* info )
	{
		if (info == mMainFinger)
		{
			// is this a tap?
			if ((mTapTimer < mTapDuration) && ((mMainFinger->curPos - mTapPos).length() < mTapThreshold))
			{
				mRetVal = CRV_Tap;
				mTapPos = mMainFinger->curPos;
			}
			else 
			{
				if (mSecondaryFinger == NULL)
				{
					mRetVal = CRV_Up;
					mTapPos = mMainFinger->curPos;
				}
				else
				{
					mRetVal = CRV_None;
				}
			}
			
			
			mMainFinger = NULL;
			
			
			if (mSecondaryFinger != NULL)
			{
				mMainFinger = mSecondaryFinger;
				mFingerIDMain = mFingerIDSecondary;
				
				mSecondaryFinger = NULL;
				mFingerIDSecondary = 0;
			}
			
			return true;
		}
		else if (info == mSecondaryFinger)
		{
			mSecondaryFinger = NULL;
			
			return true;
		}
		
		return false;
	}
}


