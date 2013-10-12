#include "Widget_Label.h"
#include "VectorTools.h"

#include "Texture.h"

#include "BitmapFont.h"
#include "GraphicsGL.h"
#include "SpriteBatch.h"

namespace Walaber
{
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	Widget_Label::VAlign Widget_Label::stringToVAlign( const std::string& s )
	{
		std::string l = StringHelper::toLower(s);
		
		if (s == "top")
			return VA_Top;
		else if (s == "bottom")
			return VA_Bottom;
		
		return VA_Center;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	std::string Widget_Label::VAlignToString( Widget_Label::VAlign v )
	{
		switch (v)
		{
			case VA_Top:
				return "Top";
				break;
				
			case VA_Center:
			default:
				return "Center";
				break;
				
			case VA_Bottom:
				return "Bottom";
				break;
		}
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	Widget_Label::HAlign Widget_Label::stringToHAlign( const std::string& s )
	{
		std::string l = StringHelper::toLower(s);
		
		if (s == "left")
			return HA_Left;
		else if (s == "right")
			return HA_Right;
		
		return HA_Center;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	std::string Widget_Label::HAlignToString( Widget_Label::HAlign h )
	{
		switch (h)
		{
			case HA_Left:
				return "Left";
				break;
				
			case HA_Right:
			default:
				return "Right";
				break;
				
			case HA_Center:
				return "Center";
				break;
		}
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	Widget_Label::Widget_Label( int name, const Vector2& pos, const Vector2& size) :
	Widget(name, WT_LABEL, pos, size, WO_PortraitUp),
	mVAlignment(VA_Center),
	mHAlignment(HA_Center),
	mTextPaddingL(0),
	mTextPaddingR(0),
	mTextPaddingT(0),
	mTextPaddingB(0),
    scaleToFitY( false )
	{
		init();
		mFont = FontManager::getInstancePtr()->getFont("normal");
	}
	
	Widget_Label::Widget_Label( int name, const Vector2& pos, const Vector2& size,const TexturePtr bgTexture):
	Widget(name, WT_LABEL, pos, size, WO_PortraitUp),
	mVAlignment(VA_Center),
	mHAlignment(HA_Center),
	mTextPaddingL(0),
	mTextPaddingR(0),
	mTextPaddingT(0),
	mTextPaddingB(0),
    scaleToFitY( false )
	{
		init();
		mFont = FontManager::getInstancePtr()->getFont("normal");
		
		setTexture(TextureName_BG, bgTexture);
		if (_hasTexture(TextureName_BG))
		{
			mTextureRect = _getTexture(TextureName_BG)->getTextureRect();
		}
	}
	
	Widget_Label::Widget_Label( int name, const Vector2& pos, const Vector2& size, const TexturePtr bgTexture, const std::string& text, const std::string& fontID, const float textPadding, const VAlign valign, const HAlign halign):
	Widget(name, WT_LABEL, pos, size, WO_PortraitUp),
	mVAlignment(valign),
	mHAlignment(halign),
	mTextPaddingL(textPadding*getSize().X),
	mTextPaddingR(textPadding*getSize().X),
	mTextPaddingT(textPadding*getSize().Y),
	mTextPaddingB(textPadding*getSize().Y),
    scaleToFitY( false )
	{
		setFont(fontID);
		
		init();
		
		if ((text.size()) && (mFont != NULL))
		{
			mText = text; 
            mTextSize = mFont->measureString(text);
			
			_setTextScale();			
			_setTextTopLeft();
		}
		
        if( bgTexture )
        {
            setTexture(TextureName_BG, bgTexture);
			mTextureRect = _getTexture(TextureName_BG)->getTextureRect();
		}
	}
    
    Widget_Label::Widget_Label( int name, const Vector2& pos, const Vector2& size, const TexturePtr bgTexture, const std::string& text, const std::string& fontID, const float textPaddingR, const float textPaddingL, const float textPaddingT, const float textPaddingB, const VAlign valign, const HAlign halign):
    Widget(name, WT_LABEL, pos, size, WO_PortraitUp),
    mVAlignment(valign),
    mHAlignment(halign),
    mTextPaddingL(textPaddingL*getSize().X),
    mTextPaddingR(textPaddingR*getSize().X),
    mTextPaddingT(textPaddingT*getSize().Y),
    mTextPaddingB(textPaddingB*getSize().Y),
    scaleToFitY( false )
	{
		setFont(fontID);
		
		init();
		
		if ((text.size()) && (mFont))
		{
			mText = text; 
            mTextSize = mFont->measureString(text);
			
			_setTextScale();			
			_setTextTopLeft();
		}

		if (bgTexture.use_count())
		{
            setTexture(TextureName_BG, bgTexture);
            if (_hasTexture(TextureName_BG))
            {
                mTextureRect = _getTexture(TextureName_BG)->getTextureRect();
            }
        }
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	Widget_Label::~Widget_Label()
	{
		
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	void Widget_Label::init()
	{
		mBGColorBlend = Color::White;
		mTextScale = 1.0f;
		mAuthoredTextScale = 1.0f;
		mTextureTile = Vector2(1, 1);
		mTileAnimation = Vector2::Zero;
		
		mTextDisplayColor = Color::Black;
		
		mOffset = Vector2::Zero;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_Label::setProperties( const PropertyList& plist )
	{
		Widget::setProperties(plist);
		
		float tpL = -1.0f;
		float tpR = -1.0f;
		float tpT = -1.0f;
		float tpB = -1.0f;
		
		const Property* p = plist.getValueForKey("TextPaddingPercent");
		if (p)
			tpL = tpR = tpT = tpB = p->asFloat();
		
		p = plist.getValueForKey("TextPaddingPercentL");
		if (p)
			tpL = p->asFloat();
		
		p = plist.getValueForKey("TextPaddingPercentR");
		if (p)
			tpR = p->asFloat();
		
		p = plist.getValueForKey("TextPaddingPercentT");
		if (p)
			tpT = p->asFloat();
		
		p = plist.getValueForKey("TextPaddingPercentB");
		if (p)
			tpB = p->asFloat();
		
		if ((tpL >= 0.0f) && (tpR >= 0.0f))
			setTextEdgeLRPaddingPercentSize(tpL, tpR);
		
		if ((tpT >= 0.0f) && (tpB >= 0.0f))
			setTextEdgeTBPaddingPercentSize(tpT, tpB);
		
		
		p = plist.getValueForKey("Tiles");
		if (p)
			setTile(p->asVector2());
		
		p = plist.getValueForKey("TileOffset");
		if (p)
			setTileOffset(p->asVector2());
		
		p = plist.getValueForKey("TileAnimation");
		if (p)
			setTileAnimation(p->asVector2());
		
		p = plist.getValueForKey("TextScale");
		if (p)
			setTextScale(p->asFloat());
		
		p = plist.getValueForKey("TextAlignV");
		if (p)
			mVAlignment = stringToVAlign(p->asString());
		
		p = plist.getValueForKey("TextAlignH");
		if (p)
			mHAlignment = stringToHAlign(p->asString());
			
		p = plist.getValueForKey("Text");
		if (p)
			mText = p->asString();		
		
	}
	
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	void Widget_Label::reloadFont()
	{
		Widget::reloadFont();
		mTextSize = getFont()->measureString(mText);
		_setTextScale();
		_setTextTopLeft();			
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	void Widget_Label::setTile( const Vector2& numberTiles )
	{
		mTextureTile = numberTiles;
		
		_applyTile();
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	void Widget_Label::setTile( const Vector2& numberTiles, float aspectRatio )
	{
		mTextureTile = numberTiles;
		mTextureTile.Y = aspectRatio * getSize().Y / (getSize().X / numberTiles.X);
		
		_applyTile();
	}
    
    Vector2 Widget_Label::getTextureTile()
    {
        return mTextureTile;
    }
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	void Widget_Label::setTileOffset( const Vector2& offset )
	{
		mTextureRect.upper_left = offset;
	} 
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	Vector2 Widget_Label::getTileOffsetBySize()
	{
		// Determine the offset of the tile given the size
		
		printf("Rect info - upperleft: %f, %f -- size: %f, %f\n", mTextureRect.upper_left.X, mTextureRect.upper_left.Y, mTextureRect.size.X, mTextureRect.size.Y);
		printf("Tile: %f, %f\n", mTextureTile.X, mTextureTile.Y);
		printf("Texture size: %f, %f\n", _getTexture(TextureName_BG)->getContentSize().X, _getTexture(TextureName_BG)->getContentSize().Y);
		printf("Widget size: %f, %f\n", getSize().X, getSize().Y);
		
		
		return mTextureRect.upper_left + (mTextureRect.size - mTextureRect.upper_left);
	} 
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	void Widget_Label::setTileAnimation( const Vector2& direction )
	{
		mTileAnimation = direction;
	}
	
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	void Widget_Label::setText(const std::string& text)
	{
		mText = text;
		if ((mFont) && (mText != ""))
		{
			mTextSize = mFont->measureString(mText);
			mTextScale = mAuthoredTextScale;
			
			_setTextScale();
			_setTextTopLeft();
		}
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	void Widget_Label::addText(const std::string& text)
	{
		mText += text;
        mTextSize = mFont->measureString(mText);
		mTextScale = mAuthoredTextScale;
		
		_setTextScale();
		_setTextTopLeft();
	}
	
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	void Widget_Label::_textureWasSet(int i, TexturePtr tex)
	{
        if( tex )
        {
            mTextureRect = tex->getTextureRect();
		
            _applyTile();
        }
		mTextureRect = tex->getTextureRect();
		
		_applyTile();
	}
	
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	bool Widget_Label::update( float elapsedSec, WidgetActionRet& ret )
	{
		// Animate the tiling (if necessary)
		if (mTileAnimation != Vector2::Zero)
		{
			mTextureRect.upper_left += mTileAnimation;
		}
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	void Widget_Label::draw( SpriteBatch* sb )//, const Vector2& offset )
	{
		float angle = _getAngle();
		int layer = mLayer;
		if (_hasTexture(TextureName_BG))
		{			
			Vector2 drawPos = getWorldPosition2D() + mDrawOffset;
			sb->drawQuad(layer, _getTexture(TextureName_BG),
						 drawPos, 
						 angle, 
						 Vector2(getSize().X, getSize().Y), 
						 mTextureRect,
						 mBGColorBlend * mTransparency);
			++layer;
		}
		else if(mBGColorBlend != Color::White)
		{
			Vector2 drawPos = getWorldPosition2D() + mDrawOffset;
			sb->drawQuadColored(layer, drawPos+mDrawOffset, 
						 angle, Vector2(getSize().X, getSize().Y),
						 mBGColorBlend * mTransparency);
			++layer;
		}
		
		if ((mFont) && (mText.size()))
		{
			Color drawColor = (mEnabled) ? mTextDisplayColor : Color(0,0,0,96);
			
			SpriteBatch::Justification just;
			
			switch(mHAlignment)
			{
				case HA_Left: 
					just = SpriteBatch::JUSTIFICATION_LEFT;
					break;
				case HA_Right:
					just = SpriteBatch::JUSTIFICATION_RIGHT;
					break;
				default:
					just = SpriteBatch::JUSTIFICATION_CENTER;
					break;
			}
			
			// Pick the smaller scale to multiply the text scale by
			float minScale = minf( getLocalScale().X, getLocalScale().Y );
			
            sb->drawString(layer, mFont, mText.c_str(), getWorldPosition2D() - mTextBaseOffset + mDrawOffset, angle, mTextScale * minScale, drawColor * mTransparency, just);
		}
		
		// debug outline
		Widget::draw(sb);//, offset);
	}
	
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	void Widget_Label::_setTextScale()
	{
		if(mTextSize.X > getSize().X-(mTextPaddingL+mTextPaddingR))
		{
			mTextScale = (getSize().X-(mTextPaddingL+mTextPaddingR))/mTextSize.X;
		}
		
		if(scaleToFitY || mTextSize.Y > getSize().Y-(mTextPaddingT + mTextPaddingB))
		{
            float yScale = (getSize().Y-(mTextPaddingT + mTextPaddingB))/mTextSize.Y;
            if(yScale < mTextScale)
            {
                mTextScale = yScale;
            }
		}
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	void Widget_Label::_setTextTopLeft()
	{
		switch (mVAlignment) 
		{
			case VA_Top:
				
				mTextBaseOffset = Vector2(0.0f, getSize().Y*.5f - mTextPaddingT);
				
				break;
			case VA_Center:
			{				
				mTextBaseOffset = Vector2(0, mTextSize.Y * mTextScale *.5f);
			}
				break;
			case VA_Bottom:
			{			
				mTextBaseOffset = Vector2(0, -(getSize().Y*.5f - mTextPaddingB - (mTextSize.Y * mTextScale)));
			}
				break;
			default:
				break;
		}	
        
        switch (mHAlignment) 
        {
            case HA_Left:
                mTextBaseOffset += Vector2((getSize().X*.5f)-mTextPaddingL, 0.0f);
                
                break;
            case HA_Right:
                mTextBaseOffset -= Vector2((getSize().X*.5f)-mTextPaddingR, 0.0f);
                
                break;
            default:
                mTextBaseOffset -= Vector2((mTextPaddingL-mTextPaddingR)*.5f, 0.0f);
                break;
        }
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --	
	void Widget_Label::_applyTile()
	{	
		if (_hasTexture(TextureName_BG))
		{
			mTextureRect = _getTexture(TextureName_BG)->getTextureRect();
			mTextureRect = Rect( Vector2( mTextureRect.upper_left.X, mTextureRect.upper_left.Y ),
								Vector2( mTextureRect.size.X * mTextureTile.X, mTextureRect.size.Y * mTextureTile.Y ) );
		}
	}
}


