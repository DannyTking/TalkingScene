#include "Widget_ClassicControls.h"
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
	Widget_ClassicControls::Widget_ClassicControls( int name, Vector2 pos, Vector2 size) : 
	Widget( name, WT_CLASSIC_CONTROLS, pos, size, WO_PortraitUp )
	{
		init();
		mAllowCustomize = false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	Widget_ClassicControls::~Widget_ClassicControls()
	{
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	void Widget_ClassicControls::init()
	{
		mMainFinger = NULL;
		mSecondaryFinger = NULL;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	bool Widget_ClassicControls::update( float elapsedSec, WidgetActionRet& ret )
	{
#ifdef _DEBUG
		//printf("Widget_ClassicControls::update() mainFinger[%p], secondaryFinger[%p]\n", mMainFinger, mSecondaryFinger);
#endif
		
		if (mMainFinger != NULL)
		{
			Vector2 toCurrent = mMainFinger->curPos - getWorldPosition2D();
			
			if ((mOrientation == WO_LandscapeUp) || (mOrientation == WO_LandscapeDown))
			{
				ret.valFloat1 = ((toCurrent.Y > 0.0f) ? 1.0f : -1.0f);
				if (mOrientation == WO_LandscapeDown) { ret.valFloat1 = -ret.valFloat1; }
			}
			else
			{
				ret.valFloat1 = ((toCurrent.X > 0.0f) ? 1.0f : -1.0f);
				if (mOrientation == WO_PortraitDown) { ret.valFloat1 = -ret.valFloat1; }
			}
			
			if (mSecondaryFinger != NULL)
			{
				float oldD = (mMainFinger->lastPos - mSecondaryFinger->lastPos).length();
				float newD = (mMainFinger->curPos - mSecondaryFinger->curPos).length();
				
				ret.valFloat2 = newD - oldD;
				
#ifdef _DEBUG
				//printf(" oldD[%4.2f] newD[%4.2f] delta[%4.2f]\n", oldD, newD, newD - oldD);
#endif
			}
			else
			{
				ret.valFloat2 = 0.0f;
			}
		}
		else
		{
#ifdef _DEBUG
			//printf("returning zero...\n");
#endif
			ret.valFloat1 = 0.0f;
			ret.valFloat2 = 0.0f;
		}
		
		return true;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	Vector2 Widget_ClassicControls::getMainFingerPos()
	{
		if (mMainFinger != NULL)
			return mMainFinger->curPos;
		else 
			return Vector2::Zero;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	bool Widget_ClassicControls::acceptNewFingerDown( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		//printf("ClassicControls::acceptNewFingerDown(%d)\n", fingerID);
#endif
		return _acceptFinger( fingerID, info );
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	bool Widget_ClassicControls::acceptNewFingerEntered( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		//printf("ClassicControls::acceptNewFingerEntered(%d)\n", fingerID);
#endif
		return _acceptFinger( fingerID, info );
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	bool Widget_ClassicControls::releaseFingerStayed( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		//printf("ClassicControls::releaseFingerStayed(%d)\n", fingerID);
#endif
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	bool Widget_ClassicControls::releaseFingerMoved( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		//printf("ClassicControls::releaseFingerMoved(%d)\n", fingerID);
#endif
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	void Widget_ClassicControls::releaseFingerUp( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		//printf("ClassicControls::releaseFingerUp(%d)\n", fingerID);
#endif
		_releaseFinger( fingerID, info );
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	bool Widget_ClassicControls::releaseFingerLeft( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		//printf("ClassicControls::releaseFingerLeft(%d)\n", fingerID);
#endif
		_releaseFinger( fingerID, info );
		
		return true;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	void Widget_ClassicControls::notifyFingerLost( int fingerID, FingerInfo* info )
	{
#ifdef _DEBUG
		//printf("ClassicControls::notifyFingerLost(%d)\n", fingerID);
#endif
		
		_releaseFinger( fingerID, info );
		
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	bool Widget_ClassicControls::_acceptFinger( int fingerID, FingerInfo* info )
	{
		if (mMainFinger == NULL)
		{
			mMainFinger = info;
			return true;
		}
		else if (mSecondaryFinger == NULL)
		{
			mSecondaryFinger = info;
			return true;
		}
		
		return false;
	}
	
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	void Widget_ClassicControls::_releaseFinger( int fingerID, FingerInfo* info )
	{
		if (info == mMainFinger)
		{
			mMainFinger = NULL;
			if (mSecondaryFinger != NULL)
			{
				mMainFinger = mSecondaryFinger;
				mSecondaryFinger = NULL;
			}
		}
		else if (info == mSecondaryFinger)
		{
			mSecondaryFinger = NULL;
		}
	}
}


