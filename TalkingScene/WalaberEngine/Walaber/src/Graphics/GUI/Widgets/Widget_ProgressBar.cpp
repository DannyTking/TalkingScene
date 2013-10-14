#include "Widget_ProgressBar.h"
#include "VectorTools.h"

#include "Texture.h"

#include "BitmapFont.h"
#include "GraphicsGL.h"
#include "SpriteBatch.h"

namespace Walaber
{
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	Widget_ProgressBar::FillMode Widget_ProgressBar::stringToFillMode( const std::string& s )
	{
		std::string l = StringHelper::toLower(s);
		
		if (l == "clamp")
			return FM_CLAMP;
		else if (l == "repeat")
			return FM_REPEAT;
		
		return FM_STRETCH;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	std::string Widget_ProgressBar::fillModeToString( Widget_ProgressBar::FillMode m )
	{
		switch (m)
		{
			default:
			case FM_STRETCH:
				return "Stretch";
				break;
				
			case FM_CLAMP:
				return "Clamp";
				break;
				
			case FM_REPEAT:
				return "Repeat";
				break;
		}
		
		return "Stretch";
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	// widget for a progress bar
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	Widget_ProgressBar::Widget_ProgressBar( int name, Vector2 pos, Vector2 size ) : 
	Widget( name, WT_SLIDER, pos, size, WO_PortraitUp )
	{	
		init();
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	Widget_ProgressBar::Widget_ProgressBar( int name, Vector2 pos, Vector2 size, TexturePtr bg, TexturePtr fill, float fillPadding, FillMode fillMode) : 
	Widget( name, WT_SLIDER, pos, size, WO_PortraitUp ),
	mFillMode(fillMode)
	{	
		init();
		setTexture(TextureName_BG, bg);
		setTexture(TextureName_Fill, fill);
		
		Vector2 padding = getSize()*fillPadding;
		mFillScale =Vector2::One-Vector2((padding.X)/size.X, (padding.Y)/size.Y);
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	Widget_ProgressBar::Widget_ProgressBar( int name, Vector2 pos, Vector2 size, TexturePtr bg, TexturePtr fill, float fillPaddingL, float fillPaddingR, 
										   float fillPaddingT, float fillPaddingB, FillMode fillMode ):
	Widget( name, WT_SLIDER, pos, size, WO_PortraitUp ),
	mFillMode(fillMode)
	{
		init();
		
		setTexture(TextureName_BG, bg);
		setTexture(TextureName_Fill, fill);
		
		setPadding( fillPaddingL, fillPaddingR, fillPaddingT, fillPaddingB );
		
		updateValue(0.0f);
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	Widget_ProgressBar::~Widget_ProgressBar()
	{
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	void Widget_ProgressBar::init()
	{
		mFillOffset = Vector2::Zero;
		mFillPos = Vector2::Zero;
		mMin = 0.0f;
		mMax = 1.0f;
		mVal = 0.0f;
		mValChanged = false;
		mRepeatStep = 0.1f;
		mColor = Color::White;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_ProgressBar::setProperties( const PropertyList& plist )
	{
		Widget::setProperties(plist);
		
		const Property* p = plist.getValueForKey("Min");
		if (p)
			mMin = p->asFloat();
		
		p = plist.getValueForKey("Max");
		if (p)
			mMax = p->asFloat();
		
		p = plist.getValueForKey("RepeatStep");
		if (p)
			mRepeatStep = p->asFloat();
		
		p = plist.getValueForKey("FillMode");
		if (p)
			mFillMode = stringToFillMode( p->asString() );
		
		p = plist.getValueForKey("Color");
		if (p)
			mColor = p->asColor();
	}
	
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	bool Widget_ProgressBar::update( float elapsedSec, WidgetActionRet& ret )
	{
		if (mValChanged)
		{
			mValChanged = false;
			
			ret.valFloat1 = getValue();
			return true;
		}
		
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	void Widget_ProgressBar::draw( SpriteBatch* sb )//, const Vector2& offset )
	{
		float angle = _getAngle();
		
		if (_hasTexture(TextureName_BG))
		{
			sb->drawQuad(mLayer, _getTexture(TextureName_BG), getWorldPosition2D() + mDrawOffset, angle, Vector2(getSize().X, getSize().Y), _getTexture(TextureName_BG)->getTextureRect(), mColor  );
		}
		
		if (_hasTexture(TextureName_Fill))
		{
			Rect drawRect = _getTexture(TextureName_Fill)->getTextureRect();
			
			switch(mFillMode)
			{
				case FM_STRETCH:
					break;
					
				case FM_CLAMP:
					drawRect.size.X *= mVal;
					break;
					
				case FM_REPEAT:
					drawRect.size.X *= (mVal/mRepeatStep);
					break;
					
				default:
					break;
			}
			
			sb->drawQuad( mLayer+1, _getTexture(TextureName_Fill), (getWorldPosition2D() + mFillPos)+mFillOffset, angle, Vector2(getSize().X*mVal, getSize().Y)*mFillScale, drawRect, mColor);
		}
		
		// debug outline
		Widget::draw(sb);//, offset);
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	void Widget_ProgressBar::setPadding(float fillPaddingL, float fillPaddingR, float fillPaddingT, float fillPaddingB)
	{
		fillPaddingL *= getSize().X;
		fillPaddingR *= getSize().X;
		fillPaddingT *= getSize().Y;
		fillPaddingB *= getSize().Y;
		
		mFillScale = Vector2::One-Vector2(((fillPaddingL+fillPaddingR))/getSize().X, ((fillPaddingT+fillPaddingB))/getSize().Y);
		mFillOffset.X = fillPaddingL-fillPaddingR;
		mFillOffset.Y = fillPaddingT-fillPaddingB;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	float Widget_ProgressBar::getValue()
	{
		return mVal;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	void Widget_ProgressBar::updateValue(float val)
	{
		mVal = (val < 0.0f) ? 0.0f : ( (val > 1.0f) ? 1.0f : val );
		
		mFillPos.X = -(((getSize().X*mFillScale.X)-((getSize().X*mFillScale.X)*mVal)) *.5f);
		mValChanged = true;
	}
}


