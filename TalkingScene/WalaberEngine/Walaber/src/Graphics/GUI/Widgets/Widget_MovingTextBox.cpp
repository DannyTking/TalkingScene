#include "Widget_MovingTextBox.h"
#include "VectorTools.h"

#include "Texture.h"

#include "BitmapFont.h"
#include "GraphicsGL.h"
#include "SpriteBatch.h"

namespace Walaber
{
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	Widget_MovingTextBox::MovementDirection Widget_MovingTextBox::stringToMovementDirection( const std::string& s )
	{
		std::string l = StringHelper::toLower(s);
		
		if (l == "left")
			return MD_Left;
		else if (l == "right")
			return MD_Right;
		else if (l == "up")
			return MD_Up;
		else if (l == "down")
			return MD_Down;
		
		return MD_Default;
	}
	
	
	std::string Widget_MovingTextBox::movementDirectionToString( Widget_MovingTextBox::MovementDirection m )
	{
		switch (m)
		{
			case MD_Left:	// also MD_Default
			default:
				return "Left";
				break;
				
			case MD_Right:
				return "Right";
				break;
				
			case MD_Up:
				return "Up";
				break;
				
			case MD_Down:
				return "Down";
				break;
		}
		
		return "Left";
	}
	
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	const float Widget_MovingTextBox::DEFAULT_SPEED = 30.0f;
	
	Widget_MovingTextBox::Widget_MovingTextBox( int name, const Vector2& pos, const Vector2& size ) :
	Widget(name, WT_MOVING_TEXT_BOX, pos, size, WO_PortraitUp)
	{
		init();
	}
	
	Widget_MovingTextBox::Widget_MovingTextBox( int name, const Vector2& pos, const Vector2& size, const TexturePtr bgTexture, const std::string& fontID, const MovementDirection moveDir, const float leftPad, const float rightPad, const float topPad, const float bottomPad, const float speed):
	Widget(name, WT_MOVING_TEXT_BOX, pos, size, WO_PortraitUp),
	mPaddingLeft(leftPad),
	mPaddingRight(rightPad),
	mPaddingTop(topPad),
	mPaddingBottom(bottomPad),
	mMovementDirection(moveDir),
	mSpeed(speed)
	{	
		setFont(fontID);
		init();
		setTexture(TextureName_BG, bgTexture);
		_updateBorders();
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	Widget_MovingTextBox::~Widget_MovingTextBox()
	{
		mBGColorBlend = Color::White;
		mBorders = Rect();
		mTextCenterOffsetFromPos = Vector2::Zero;
		mCurrentString = mTextQueue.begin();
		mTextDisplayColor = Color::Black;
		mTextScale = 1.0f;
		mFingerID = -1;
		mFireTouched = false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	void Widget_MovingTextBox::init()
	{
		
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_MovingTextBox::setProperties( const PropertyList& plist )
	{
		Widget::setProperties(plist);
		
		const Property* p = plist.getValueForKey("TextDisplayColor");
		if (p)
			mTextDisplayColor = p->asColor();
		
		p = plist.getValueForKey("BGColorBlend");
		if (p)
			mBGColorBlend = p->asColor();
		
		p = plist.getValueForKey("TextScale");
		if (p)
			mTextScale = p->asFloat();
		
		p = plist.getValueForKey("Text");
		if (p)
		{
			clearText();
			addString(p->asString());
		}
		
		p = plist.getValueForKey("MovementDirection");
		if (p)
			mMovementDirection = stringToMovementDirection( p->asString() );
		
		p = plist.getValueForKey("Speed");
		if (p)
			mSpeed = p->asFloat();
		
		p = plist.getValueForKey("Padding");
		if (p)
		{
			std::vector< std::string > pieces = StringHelper::split(p->asString(), ' ');
			if (pieces.size() == 4)
			{
				setPadding( StringHelper::strToFloat(pieces[0]), StringHelper::strToFloat(pieces[1]), StringHelper::strToFloat(pieces[2]), StringHelper::strToFloat(pieces[3]) );
			}
		}
	}
	
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	void Widget_MovingTextBox::setPadding( const float leftPad, const float rightPad, const float topPad, const float bottomPad )
	{
		mPaddingLeft = leftPad;
		mPaddingRight = rightPad;
		mPaddingTop = topPad;
		mPaddingBottom = bottomPad;
		
		_updateBorders();
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	void Widget_MovingTextBox::addString(const std::string& str)
	{
		bool isFirst = false;
		if(mTextQueue.empty())
		{
			isFirst=true;
		}
		
		mTextQueue.push_back(TextLineInfo(str, mFont->measureString(str, mTextScale)));
		
		if(isFirst)
		{
			mCurrentString = mTextQueue.begin();
			_setTextStartingPos();
		}
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	void Widget_MovingTextBox::advanceToNextString()
	{
		++mCurrentString;
		if(mCurrentString == mTextQueue.end())
		{
			mCurrentString = mTextQueue.begin();
		}
		
		if(!mTextQueue.empty())
		{
			_setTextStartingPos();
		}
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	bool Widget_MovingTextBox::update( float elapsedSec, WidgetActionRet& ret )
	{
		if(!mTextQueue.empty())
		{
			switch (mMovementDirection) 
			{
				case MD_Left:
					
					mTextCenterOffsetFromPos.X -= mSpeed*elapsedSec;
					if(((getWorldPosition().X+mTextCenterOffsetFromPos.X) + (*mCurrentString).mSize.X*.5f) < mBorders.upper_left.X)
					{
						advanceToNextString();
					}
					
					break;
					
				case MD_Right:
					
					mTextCenterOffsetFromPos.X += mSpeed*elapsedSec;
					if(((getWorldPosition().X+mTextCenterOffsetFromPos.X) - (*mCurrentString).mSize.X*.5f) > mBorders.getBottomRight().X)
					{
						advanceToNextString();
					}
					
					break;
					
				case MD_Up:
					
					mTextCenterOffsetFromPos.Y -= mSpeed*elapsedSec;
					if(((getWorldPosition().Y+mTextCenterOffsetFromPos.Y) + (*mCurrentString).mSize.Y*.5f) < mBorders.upper_left.Y)
					{
						advanceToNextString();
					}
					
					
					break;
					
				case MD_Down:
					
					mTextCenterOffsetFromPos.Y += mSpeed*elapsedSec;
					if(((getWorldPosition().Y+mTextCenterOffsetFromPos.Y) - (*mCurrentString).mSize.Y*.5f) > mBorders.getBottomRight().Y)
					{
						advanceToNextString();
					}
					
					break;
				default:
					break;
			}		
		}		
		
		if (mFireTouched)
		{
			mFireTouched = false;
			ret.valBool = true;
			return true;
		}
		
		return false;
	}
	

	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	// assume sprite batch has been started
	void Widget_MovingTextBox::draw(SpriteBatch* sb)//, const Vector2& offset )
	{
		float angle = _getAngle();
		int layer = mLayer;
		
		if (_hasTexture(TextureName_BG))
		{			
			Vector2 drawPos = getWorldPosition2D() + mDrawOffset;
			sb->drawQuad(layer, _getTexture(TextureName_BG),
								drawPos+mDrawOffset, 
								angle, 
								getSize(), 
								(mFingerID != -1) ? Color(128,128,128) : mBGColorBlend);	
			
			++layer;
		}
		
		if(mFont && !mTextQueue.empty())
		{
			Color drawColor = (mEnabled) ? mTextDisplayColor : Color(0,0,0,96);
			sb->drawStringClipped(layer, mFont, (*mCurrentString).mText, getWorldPosition2D() + mTextCenterOffsetFromPos, mBorders, angle, mTextScale, drawColor, SpriteBatch::JUSTIFICATION_CENTER);
		}
		
		// debug outline
		Widget::draw(sb);//, offset);
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	void Widget_MovingTextBox::_setTextStartingPos()
	{
		switch (mMovementDirection) 
		{
			case MD_Left:
				
				mTextCenterOffsetFromPos = (getWorldPosition2D() + Vector2((getSize().X+(*mCurrentString).mSize.X)*.5f, -(*mCurrentString).mSize.Y*.5f)) - getWorldPosition2D();
				
				break;
				
			case MD_Right:
				
				mTextCenterOffsetFromPos = (getWorldPosition2D() - Vector2((getSize().X+(*mCurrentString).mSize.X)*.5f, (*mCurrentString).mSize.Y*.5f)) - getWorldPosition2D();
				
				break;
				
			case MD_Up:
				
				mTextCenterOffsetFromPos = (getWorldPosition2D() + Vector2(-(*mCurrentString).mSize.X*.5f, (getSize().Y+(*mCurrentString).mSize.Y)*.5f)) - getWorldPosition2D();
				
				break;
				
			case MD_Down:
				
				mTextCenterOffsetFromPos = (getWorldPosition2D() - Vector2((*mCurrentString).mSize.X*.5f, (getSize().Y+(*mCurrentString).mSize.Y)*.5f)) - getWorldPosition2D();
				
				break;
			default:
				break;
		}
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_MovingTextBox::_updateBorders()
	{
		mBorders.upper_left = getWorldPosition2D() - (getSize()*.5f);
		mBorders.size = getSize();
		mBorders.size -= Vector2(mPaddingLeft+mPaddingRight, mPaddingTop+mPaddingBottom);
		mBorders.upper_left += Vector2(mPaddingLeft, mPaddingTop);
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	bool Widget_MovingTextBox::acceptNewFingerDown( int fingerID, FingerInfo* info )
	{
		if (mFingerID == -1)
		{
			mFingerID = fingerID;
			return true;
		}
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	void Widget_MovingTextBox::releaseFingerUp( int fingerID, FingerInfo* info )
	{
		if (mFingerID == fingerID)
		{
			mFireTouched = true;
			mFingerID = -1;
		}
	}
	
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	bool Widget_MovingTextBox::releaseFingerLeft( int fingerID, FingerInfo* info )
	{
		if (mFingerID == fingerID)
		{
			mFingerID = -1;
			return true;
		}
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	void Widget_MovingTextBox::notifyFingerLost( int fingerID, FingerInfo* info )
	{
		if(mFingerID == fingerID)
		{
			mFingerID = -1;
		}
	}
}


