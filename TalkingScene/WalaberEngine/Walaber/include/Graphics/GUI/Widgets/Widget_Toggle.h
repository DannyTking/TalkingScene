#ifndef _WALABER_WIDGET_TOGGLE_H_
#define _WALABER_WIDGET_TOGGLE_H_

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
#include <ActionTimeline.h>

namespace Walaber
{
	class SpriteBatch; 
	class BitmapFont;

	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	// widget for a simple checkbox.
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	class Widget_Toggle : public Widget
	{
	public:		
		enum TextAlignment
        {
            TA_LeftRight,
            TA_Right
        };
		
		Widget_Toggle( int name, Vector2 pos, Vector2 size);
        
        /*!
         Note: If a reset delay is specified, Widget_Toggle will simply trigger the sound and respect
         the delay. If a reset delay is not specified but a sound is, Widget_Toggle will turn off
         only when the sound has finally stopped.
         */
		Widget_Toggle( int name, Vector2 pos, Vector2 size, TexturePtr texOn, TexturePtr texOff, std:: string textOn, std::string textOff, const std::string& fontID, float resetDelay = -1.0f, float soundGroupID = -1);
		~Widget_Toggle();
		
		void init();
		
		void setIcon( TexturePtr iconTex, Vector2 drawSize);
		
		void setTextOn( std::string onText ) { mDisplayTextOn = onText; }
		void setTextOff( std::string offText ) { mDisplayTextOff = offText; }
		
		void setTextureOn( TexturePtr onTex );
		void setTextureOff( TexturePtr offTex );
				
		void setOn( bool on, bool sendEvent = false ) { mOn = on; if (sendEvent) { mChanged = true; } }
		bool isOn() { return mOn; }
		
		void setAlignment(TextAlignment value) { mAlignment = value; }
		TextAlignment getAlignment() { return mAlignment; }
		
		void setTextScale(float scale) { mTextScale = scale; }
		float getTextScale() { return mTextScale; }
		
		void setTextDisplayColor( Color c ) { mTextColor = c; }
		Color getTextDisplayColor() { return mTextColor; }
        
        void setResetDelay( float delay );
        float getResetDelay() { return mResetDelay; }
        
        void setSoundID( int groupID ) { mSoundToPlay = groupID; }
		
		bool update( float elapsedSec, WidgetActionRet& ret );
		void draw( SpriteBatch* sb );//, const Vector2& offset );
		
		bool acceptNewFingerDown( int fingerID, FingerInfo* info );
		bool acceptNewFingerEntered( int fingerID, FingerInfo* info );
		bool releaseFingerStayed( int fingerID, FingerInfo* info );
		bool releaseFingerMoved( int fingerID, FingerInfo* info );
		void releaseFingerUp( int fingerID, FingerInfo* info );
		bool releaseFingerLeft( int fingerID, FingerInfo* info );
		void notifyFingerLost( int fingerID, FingerInfo* info );
		
	private:
        void _delayCallback( void* ); ///< Called when our delay is up either from reset or sound
        
		TexturePtr			mTextureOn;
		TexturePtr			mTextureOff;
		Rect				mOnTextureRect;
		Rect				mOffTextureRect;
		
		TexturePtr			mIconTexture;
		Vector2				mIconTextureSize;
		Rect				mIconTextureRect;
		
		std::string			mDisplayTextOn;
		std::string			mDisplayTextOff;
				
		float				mTextScale;
		Color				mTextColor;
		
		TextAlignment		mAlignment;
		
		bool				mOn;
		
		bool				mChanged;
        
        float               mResetDelay; ///< A period of time that, if specified, the toggle widget will wait to automatically switch it's state back.
        
        int                 mSoundToPlay;
        
        ActionTimelinePtr   mActionTimeline; ///< An action timeline.

        enum ActionTracks
        {
            ActionTrack_INVALID = -1,
            ActionTrack_DelayTimer,
            ActionTrack_SIZE
        };
	};
}

#endif	// _WALABER_WIDGET_TOGGLE_H_