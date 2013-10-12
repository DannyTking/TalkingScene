#ifndef _WALABER_WIDGET_SCORE_COUNTER_H_
#define _WALABER_WIDGET_SCORE_COUNTER_H_

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
	class Widget_ScoreCounter : public Widget
	{
	public:
		
        enum MovementDirection
        {
            MD_INCREASE,
            MD_DECREASE,
        };
        
        enum MovementStatus
        {
            MS_NORMAL,
            MS_SPEED_DOWN,
            MS_SPEED_UP_AND_DOWN,
        };
        
		Widget_ScoreCounter( int name, const Vector2& pos, const Vector2& size );
		Widget_ScoreCounter( int name, const Vector2& pos, const Vector2& size, const TexturePtr bgTexture, const std::string& fontID, const MovementDirection direction, int targetScore, const float textScale = 0.6f, const float leftPad = 0.0f, const float rightPad = 0.0f, const float topPad = 0.0f, const float bottomPad= 0.0f, const float speed=DEFAULT_SPEED);
		~Widget_ScoreCounter();
		
		void init();
		
		void setTextDisplayColor(Color color) { mTextDisplayColor = color; }
		Color getTextDisplayColor() { return mTextDisplayColor; }
		
		void setBGTexture(TexturePtr bgTexture); 
		
		void setBGColorBlend(const Color& color) { mBGColorBlend = color; }
		
		void setPadding( const float leftPad, const float rightPad, const float topPad, const float bottomPad );
		
		void setTextScale(float scale){ mTextScale = scale; }
		float getTextScale() { return mTextScale; }
		
		void clearText() 
		{
			mTextQueues.clear(); 
            for (int i = 0; i < mCurrentStrings.size(); ++i) {
                mCurrentStrings[i] = 0;
            } 
		}
		
		void advanceToNextString();
        
        void setMovementDirection(MovementDirection direction){mMovementDirection = direction;}
		
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
        int  _getDigitAtIndex(int pos);
		
		TexturePtr									mBGTexture;
		Color										mBGColorBlend;
		
		Rect										mTextureRect;
		
		Rect										mBorders;
		Vector2										mTextCenterOffsetFromPos; 
        std::vector<TextLineInfo>                   mTextQueues;
        std::vector<int>                            mCurrentStrings;
        int                                         mCurrentChangingColumn;
        int                                         mTargetScore;
        int                                         mColumnNum;
		
		Color										mTextDisplayColor;
				
		float										mPaddingLeft;
		float										mPaddingRight;
		float										mPaddingTop;
		float										mPaddingBottom;
		float										mTextScale;
		
        MovementDirection                           mCounterDirection;
		MovementDirection							mMovementDirection;
        MovementStatus                              mMovementStatus;
		float										mSpeed;
        float                                       mDeltaTime;
        float                                       mDeltaSpeed;
        float                                       mAnimationTime;
		static const float							DEFAULT_SPEED;
		
		int											mFingerID;
		bool										mFireTouched;
	};
}

#endif	// _WALABER_WIDGET_MOVING_TEXT_BOX_H_
