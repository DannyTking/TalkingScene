#ifndef _WALABER_WIDGET_LABEL_H_
#define _WALABER_WIDGET_LABEL_H_

#include "GUI.h"
#include "QuadHelper.h"
#include "CircleHelper.h"
#include "Rect.h"
#include "Color.h"

#include "FontManager.h"

#include <Texture.h>
#include <string>
#include <vector>
#include <list>

namespace Walaber
{
	class SpriteBatch; 
	class BitmapFont; 
	
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	// basiclabel widget
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	class Widget_Label : public Widget
	{
	public:
		enum TextureName
		{
			TextureName_BG
		};
		
		enum VAlign
		{
			VA_Top,
			VA_Center,
			VA_Bottom,
            VA_SHORT_INT = 32767,
		};
		static VAlign stringToVAlign( const std::string& s );
		static std::string VAlignToString( VAlign v );
		
		
		enum HAlign
		{
			HA_Left,
			HA_Center,
			HA_Right,
            HA_SHORT_INT = 32767,
		};
		static HAlign stringToHAlign( const std::string& s );
		static std::string HAlignToString( HAlign h );
		
		
		Widget_Label( int name, const Vector2& pos, const Vector2& size );
		Widget_Label( int name, const Vector2& pos, const Vector2& size, const TexturePtr bgTexture);
		Widget_Label( int name, const Vector2& pos, const Vector2& size, const TexturePtr bgTexture, const std::string& text,
					 const std::string& fontID, const float textPadding = 0.0f, const VAlign valign = VA_Center, const HAlign halign = HA_Center);
        Widget_Label( int name, const Vector2& pos, const Vector2& size, const TexturePtr bgTexture, const std::string& text, const std::string& fontID, const float textPaddingR, const float textPaddingL, const float textPaddingT, const float textPaddingB, const VAlign valign = VA_Center, const HAlign halign = HA_Center);
		
        virtual ~Widget_Label();
		
		void init();
		
		// valid properties:
		// "TextPaddingPercent", "TextPaddingPercentL", "TextPaddingPercentR", "TextPaddingPercentT", "TextPaddingPercentB"
		// "Tiles", "TileAspect", "TileOffset", "TileAnimation", "TextScale",
		// "TextAlignV", "TextAlignH", "Text"
		void setProperties( const PropertyList& plist );
		
		
		// pass in padding as a percentage of the size (LR is perscent of mSize.X, TB is percent of mSize.Y)
        void setTextEdgePaddingPercentSize(const float edgePadding) 
        { 
            mTextPaddingL = edgePadding*getSize().X; 
            mTextPaddingR = edgePadding*getSize().X; 
            mTextPaddingT = edgePadding*getSize().Y; 
            mTextPaddingB = edgePadding*getSize().Y; 
        }
        
        // pass in padding as a percentage of the size (LR is perscent of mSize.X, TB is percent of mSize.Y)
        void setTextEdgePaddingPercentSize(const float edgePaddingL, const float edgePaddingR,
                                           const float edgePaddingT, const float edgePaddingB) 
        { 
            mTextPaddingL = edgePaddingL*getSize().X; 
            mTextPaddingR = edgePaddingR*getSize().X; 
            mTextPaddingT = edgePaddingT*getSize().Y; 
            mTextPaddingB = edgePaddingB*getSize().Y; 
        }
        
        void setTextEdgeLRPaddingPercentSize(const float edgePaddingL, const float edgePaddingR) 
        { 
            mTextPaddingL = edgePaddingL*getSize().X; 
            mTextPaddingR = edgePaddingR*getSize().X; 
        }
        
        void setTextEdgeTBPaddingPercentSize(const float edgePaddingT, const float edgePaddingB) 
        { 
            mTextPaddingT = edgePaddingT*getSize().Y; 
            mTextPaddingB = edgePaddingB*getSize().Y; 
        }
		
		float getTextPaddingH()
		{
			return (mTextPaddingL + mTextPaddingR) / 2.0f;
		}
		
		float getTotalPaddingH()
		{
			return mTextPaddingL + mTextPaddingR;
		}
		
		float getTextPaddingV()
		{
			return (mTextPaddingT + mTextPaddingB) / 2.0f;
		}
		
		float getTotalPaddingV()
		{
			return mTextPaddingT + mTextPaddingB;
		}
		
		void setPadding(const float padL, const float padR, const float padT, const float padB)
		{
			mTextPaddingL = padL*getSize().X; 
            mTextPaddingR = padR*getSize().X; 
            mTextPaddingT = padT*getSize().Y; 
            mTextPaddingB = padB*getSize().Y; 
			
			_setTextScale(); 
			_setTextTopLeft();
		}
        
		virtual void reloadFont();
		
		void setTextDisplayColor(Color color) { mTextDisplayColor = color; }
		Color getTextDisplayColor() { return mTextDisplayColor; }
		
		void setBGColorBlend(const Color& color) { mBGColorBlend = color; }
		Color getBGColorBlend() { return mBGColorBlend; }
		
		void setTile( const Vector2& numberTiles );
		void setTile( const Vector2& numberTiles, float aspectRatio );
        Vector2 getTextureTile();
		
		void setTileOffset( const Vector2& offset );
		Vector2 getTileOffset() { return mTextureRect.upper_left; }
		Vector2 getTileOffsetBySize();
		
		void setTileAnimation( const Vector2& direction );
        
		void setVAlignment(VAlign alignment) { mVAlignment = alignment;  _setTextTopLeft(); }
        VAlign getVAlignment() { return mVAlignment; }
		void setHAlignment(HAlign alignment) { mHAlignment = alignment;  _setTextTopLeft(); }
        HAlign getHAlignment() { return mHAlignment; }

        void setScaleToFitY( bool scaleToFit ) { scaleToFitY = scaleToFit; }
		void setTextScale(float scale) { mTextScale = scale; mAuthoredTextScale = scale; _setTextTopLeft(); }
		float getTextScale() { return mTextScale; }
		float getAuthoredTextScale() { return mAuthoredTextScale; }
		
        void setText(const std::string& text);
		void addText(const std::string& text);
		
		void clearText() { mText.clear(); mTextScale = 1.0f; }
		
		const std::string& getText() const { return mText; }
		std::string getText() { return mText; }
        std::string& getTextReference() { return mText; }
		
		bool update( float elapsedSec, WidgetActionRet& ret );
		void draw( SpriteBatch* sb );//, const Vector2& offset );
        
		virtual void setLocalPosition2D(const Vector2& pos ) { Widget::setLocalPosition2D(pos); _setTextTopLeft(); }

		void setBaseSize(Vector2 size) { Widget::setBaseSize(size); _setTextScale(); _setTextTopLeft(); }
		
	protected:
		
		void _setTextTopLeft();
		void _setTextScale();
		
		void _applyTile();
		
		void _textureWasSet( int i, TexturePtr tex );
		
		Rect						mTextureRect;
		
        Vector2						mTextBaseOffset; // middle point x,  uppermost Y
        Vector2                     mTextSize;
		
		Vector2						mTextureTile;
		Vector2						mTileAnimation;
		
        Color						mBGColorBlend;
		Color						mTextDisplayColor;
			
		float						mTextPaddingL; // pixels
        float						mTextPaddingR; // pixels
        float						mTextPaddingT; // pixels
        float						mTextPaddingB; // pixels
		
		float						mAuthoredTextScale;	// text scale "as authored", may differ from actual scale (in order to fit text)
		float						mTextScale;
		VAlign						mVAlignment;
		HAlign						mHAlignment;
        std::string                 mText;
        bool                        scaleToFitY;
		
		Vector2						mOffset;
		
	};
}

#endif	// _WALABER_WIDGET_LABEL_H_
