#ifndef _WALABER_WIDGET_MOVING_TEXT_BOX_H_
#define _WALABER_WIDGET_MOVING_TEXT_BOX_H_

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
	// moving text box -- for news tickers...etc
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	class Widget_MovingTextBox : public Widget
	{
	public:
		enum TextureName
		{
			TextureName_BG
		};
		
		enum MovementDirection {
			MD_Left,
			MD_Right,
			MD_Up,
			MD_Down, 
			MD_Default = MD_Left,
			MD_DWORD = 0xffffffff
		};
		MovementDirection stringToMovementDirection( const std::string& s );
		std::string movementDirectionToString( MovementDirection m );
		
		Widget_MovingTextBox( int name, const Vector2& pos, const Vector2& size );
		Widget_MovingTextBox( int name, const Vector2& pos, const Vector2& size, const TexturePtr bgTexture, const std::string& fontID, const MovementDirection moveDir, const float leftPad = 0.0f, const float rightPad = 0.0f, const float topPad = 0.0f, const float bottomPad= 0.0f, const float speed=DEFAULT_SPEED);
		~Widget_MovingTextBox();
		
		void init();
		
		// valid properties
		// "TextDisplayColor", "BGColorBlend", "TextScale", "Text", "MovementDirection", "Speed", "Padding" (format is 'l r t b')
		void setProperties( const PropertyList& plist );
		
		void setTextDisplayColor(Color color) { mTextDisplayColor = color; }
		Color getTextDisplayColor() { return mTextDisplayColor; }
		
		void setBGColorBlend(const Color& color) { mBGColorBlend = color; }
		
		void setPadding( const float leftPad, const float rightPad, const float topPad, const float bottomPad );
		
		void setTextScale(float scale){ mTextScale = scale; }
		float getTextScale() { return mTextScale; }
		
		void addString(const std::string& str);
		
		void clearText() 
		{
			mTextQueue.clear(); 
			mCurrentString = mTextQueue.begin();  
		}
		
		void advanceToNextString();
		
		void setMovementDirection(const MovementDirection moveDir) { mMovementDirection = moveDir; }
		
		void setSpeed(const float speed) { mSpeed = speed; }
		
		bool update( float elapsedSec, WidgetActionRet& ret );
		void draw( SpriteBatch* sb );//, const Vector2& offset );
		
		void setLocalPosition2D(const Vector2& pos ) { Widget::setLocalPosition2D(pos); _updateBorders(); }
		void setBaseSize(Vector2 size) { Widget::setBaseSize(size); _updateBorders(); }
		
		void reset() { mFingerID = -1; }
		
		bool acceptNewFingerDown( int fingerID, FingerInfo* info );
		bool releaseFingerMoved( int fingerID, FingerInfo* info ) { if (mFingerID == fingerID) { return false; } else { return true; } }
		bool releaseFingerStayed( int fingerID, FingerInfo* info ) { if (mFingerID == fingerID) { return false; } else { return true; } }
		void releaseFingerUp( int fingerID, FingerInfo* info );
		bool releaseFingerLeft( int fingerID, FingerInfo* info );
		void notifyFingerLost( int fingerID, FingerInfo* info );
		
	protected:
		
		void _updateBorders();
		void _setTextStartingPos(); // assumes we have a non-empty set of strings
		
		Color										mBGColorBlend;
		
		Rect										mBorders;
		Vector2										mTextCenterOffsetFromPos; 
		std::list<TextLineInfo>						mTextQueue;
		std::list<TextLineInfo>::const_iterator		mCurrentString;
		
		Color										mTextDisplayColor;
				
		float										mPaddingLeft;
		float										mPaddingRight;
		float										mPaddingTop;
		float										mPaddingBottom;
		float										mTextScale;
		
		MovementDirection							mMovementDirection;
		float										mSpeed;
		static const float							DEFAULT_SPEED;
		
		int											mFingerID;
		bool										mFireTouched;
	};
}

#endif	// _WALABER_WIDGET_MOVING_TEXT_BOX_H_
