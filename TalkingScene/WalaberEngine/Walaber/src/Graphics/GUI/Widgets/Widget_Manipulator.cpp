#include "Widget_Manipulator.h"
#include "VectorTools.h"

#include "Texture.h"

#include "BitmapFont.h"
#include "GraphicsGL.h"
#include "SpriteBatch.h"

namespace Walaber
{
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	// widget for a simple manipulator - a button that you can drag around and use it's position to manipulate things.
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	Widget_Manipulator::Widget_Manipulator( int name, Vector2 pos, Vector2 size, TexturePtr tex, bool oneShot ) : 
	Widget( name, WT_MANIPULATOR, pos, size, WO_PortraitUp ),
	mOneShot(oneShot)
	{
		init();
		
		setTexture(TextureName_Main, tex);
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	Widget_Manipulator::~Widget_Manipulator()
	{
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	void Widget_Manipulator::init()
	{
		mAngle = 0.0f;
		mMoved = false;
		mFingerPos = Vector2::Zero;
		mFingerOffset = Vector2::Zero;
		mHaveFinger = false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_Manipulator::setProperties( const PropertyList& plist )
	{
		Widget::setProperties(plist);
		
		const Property* p = plist.getValueForKey("OneShot");
		if (p)
			mOneShot = p->asBool();
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	bool Widget_Manipulator::update( float elapsedSec, WidgetActionRet& ret )
	{
		if (mMoved)
		{
			mMoved = false;
			ret.valFloat1 = mFingerPos.X;
			ret.valFloat2 = mFingerPos.Y;
			return true;
		}
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	void Widget_Manipulator::draw( SpriteBatch* sb )//, const Vector2& offset )
	{
		float angle = _getAngle();
		
		if (_hasTexture(TextureName_Main))
		{
			unsigned char alpha = (unsigned char)(mTransparency * 255.0f);
			
			sb->drawQuad( mLayer, _getTexture(TextureName_Main),
						 getWorldPosition2D(),
						 angle + radToDeg(mAngle), 
						 Vector2(getSize().X, getSize().Y), 
						 _getTexture(TextureName_Main)->getTextureRect(),
						 (mHaveFinger ? Color(0, 0, 0, alpha) : Color(alpha, alpha, alpha, alpha)) );
		}
		
		Widget::draw(sb);//, offset);
		
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	bool Widget_Manipulator::acceptNewFingerDown( int fingerID, FingerInfo* info )
	{
		mMoved = true;
		mFingerOffset = info->curPos - getWorldPosition2D();
		mFingerPos = getWorldPosition2D();
		if (!mOneShot)
			mHaveFinger = true;
		else
			mHaveFinger = false;
		
		return true;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	bool Widget_Manipulator::acceptNewFingerEntered( int fingerID, FingerInfo* info )
	{
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	bool Widget_Manipulator::releaseFingerStayed( int fingerID, FingerInfo* info )
	{
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	bool Widget_Manipulator::releaseFingerMoved( int fingerID, FingerInfo* info )
	{
		if (!mOneShot)
		{
			mMoved = true;
			mFingerPos = info->curPos - mFingerOffset;
			return false;
		}
		
		return true;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	void Widget_Manipulator::releaseFingerUp( int fingerID, FingerInfo* info )
	{
		mHaveFinger = false;
		mFingerOffset = Vector2::Zero;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	bool Widget_Manipulator::releaseFingerLeft( int fingerID, FingerInfo* info )
	{
		if (!mOneShot)
		{
			mMoved = true;
			mFingerPos = info->curPos - mFingerOffset;
			return false;
		}
		
		return true;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	void Widget_Manipulator::notifyFingerLost( int fingerID, FingerInfo* info )
	{
		mHaveFinger = false;
		mFingerOffset = Vector2::Zero;
	}
}


