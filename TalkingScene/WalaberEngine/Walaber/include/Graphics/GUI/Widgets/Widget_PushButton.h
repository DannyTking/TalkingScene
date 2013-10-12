#ifndef _WALABER_WIDGET_PUSH_BUTTON_H_
#define _WALABER_WIDGET_PUSH_BUTTON_H_

#include "GUI.h"
#include "QuadHelper.h"
#include "CircleHelper.h"
#include "Rect.h"
#include "Color.h"

#include "Widget_Label.h"
#include "Widget_Group.h"

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
	// basic push button widget
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	class Widget_PushButton : public Widget
	{
	public:
		Widget_PushButton( int name, Vector2 pos, Vector2 size );
		Widget_PushButton( int name, Vector2 pos, Vector2 size, TexturePtr texture, bool reactOnDown );
		Widget_PushButton( int name, Vector2 pos, Vector2 size, TexturePtr texture, bool reactOnDown, std::string text, const std::string& fontID);
		Widget_PushButton( int name, Vector2 pos, Vector2 size, TexturePtr texture, TexturePtr hilightTex, bool reactOnDown, std::string text, const std::string& fontID);
		~Widget_PushButton();
		
		void init();
		
		virtual void reloadFont()
		{
			Widget::reloadFont();
			_resizeFont();
		}
		
		void setTextureColor( Color col ) { mTextureColor = col; }
		Color getTextureColor() { return mTextureColor; }
		
		void setHilightColor( Color col ) { mHilightColor = col; }
		Color getHilightColor() {return mHilightColor; }
		
		void setDisabledColor( Color col ) { mDisabledColor = col; }
		Color getDisabledColor() { return mDisabledColor; }
		
		void setDisabledTextColor( Color col ) { mDisabledTextColor = col; }
		Color getDisabledTextColor() { return mDisabledTextColor; }
		
		void setPressOnEnter( bool v ) { mPressOnEnter = v; }
		
		void setTexture ( TexturePtr tex ) { mTexture = tex; _applyTile(); }
		void setHilightTexture( TexturePtr tex ) { mHilightTexture = tex; _applyTile(); }
		
		TexturePtr getTexture() { return mTexture; }
		TexturePtr getHilightTexture() { return mHilightTexture; }
		
		void setReactOnDown ( bool v ) { mReactOnDown = v; }
		bool getReactOnDown () { return mReactOnDown; }
		
		void setDisplayText ( std::string text ) { mDisplayText = text; _resizeFont(); }
		std::string getDisplayText () { return mDisplayText; }
		
		void setEdgePadding ( Vector2 padding ) { mEdgePadding = padding; _resizeFont(); }
		Vector2 getEdgePadding() { return mEdgePadding; }
		
		void setTextOffset ( Vector2 offset ) { mTextOffset = offset; }
		void setTextHilightOffset ( Vector2 offset ) { mTextHilightOffset = offset; }
		
		Vector2 getTextOffset() { return mTextOffset; }
		Vector2 getTextHilightOffset() { return mTextHilightOffset; }
		
		void setTextScale(float scale){ mTextScale = scale; }
		float getTextScale() { return mTextScale; }
		
		void setTextDisplayColor( const Color& color ) { mTextColor = color; }
		Color getTextDisplayColor() { return mTextColor; }
		
		void setTextHilightColor( const Color& color ) { mHilightTextColor = color; }
		Color getTextHilightColor() { return mHilightTextColor; }
		
		bool update( float elapsedSec, WidgetActionRet& ret );
		void draw( SpriteBatch* sb );//, const Vector2& offset );
		
		void reset() { mFingerID = -1; mGoPush = mGoRelease = mPressed = false; }
		
		void setTile( Vector2 numberTiles );
		void setTile( Vector2 numberTiles, float aspectRatio );
		
		void setTileOffset( Vector2 offset );
		
		void setTileAnimation( Vector2 direction );
		
		void addLabel( Widget_Label* newLabel );
		
		void setLocalPosition2D(const Vector2& pos);
        void applyPositionOffset2D(const Vector2& pos);
		
		void setLocalScale2D(const Vector2& scale);
        
        void setTextureFlipped( bool flipHorizontal, bool flipVertical );
		
		bool acceptNewFingerDown( int fingerID, FingerInfo* info );
		bool acceptNewFingerEntered( int fingerID, FingerInfo* info );
		bool releaseFingerStayed( int fingerID, FingerInfo* info );
		bool releaseFingerMoved( int fingerID, FingerInfo* info );
		void releaseFingerUp( int fingerID, FingerInfo* info );
		bool releaseFingerLeft( int fingerID, FingerInfo* info );
		void notifyFingerLost( int fingerID, FingerInfo* info );
		
	private:
		
		void _resizeFont();
		
		void _applyTile();
		
		int					mFingerID;
		
		TexturePtr			mTexture;
		TexturePtr			mHilightTexture;
		
		Rect				mTextureRect;
		Rect				mHilightTextureRect;
		Vector2				mTextureTile;
		Vector2				mTileAnimation;
		
		bool				mReactOnDown;
		
		bool				mGoPush;
		bool				mGoRelease;
		bool				mPressed;
		bool				mPressOnEnter;
		
		Color				mTextureColor;
		
		Vector2				mTextOffset;
		Vector2				mTextHilightOffset;
		Vector2				mEdgePadding;
		float				mTextScale;
		std::string			mDisplayText;
		Color				mTextColor;
		Color				mHilightTextColor;
		
		Color				mDisabledColor;
		Color				mDisabledTextColor;
		
		Color				mHilightColor;
		
		// TEMP FIX - buttons can have any number of associated labels attached to them
		std::vector< Widget_Label* >		mLabels;
		Widget_Group*						mLabelGroup;
	};
}

#endif	// _WALABER_WIDGET_PUSH_BUTTON_H_
