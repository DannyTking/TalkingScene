#ifndef _TRANSITION_H_
#define _TRANSITION_H_

#include <SpriteBatch.h>

namespace Walaber
{
	class PropertyList;
	
	// --------========--------========--------========--------========--------========--------========--------========
    class Transition
    {
	public:
		
		Transition();
		virtual ~Transition();
		
		// --------========--------========--------========--------========--------========--------========--------========
		enum TransitionType 
		{
			TT_Wipe,
			TT_Fade,
			TT_Block,
			TT_Slide,
			TT_Iris,
			TT_WidgetSlide,
			TT_SIZE
		};
		
		enum TransitionDirection 
		{
			TD_Left,
			TD_Right,
			TD_Up,
			TD_Down
		};
		
		enum TransitionPhase {
			TP_Started,
			TP_Intro,
			TP_Flush,
			TP_Outro,
			TP_Finished,
			TP_NONE
		};
		
		// --------========--------========--------========--------========--------========--------========--------========
		/// setup the transition with duration
		virtual void setup( float duration );
		virtual void setup( float duration, const PropertyList& plist );
		
		// --------========--------========---- ----========--------========--------========--------========--------========
		/// update the game by 1 frame.
        virtual int update( float elapsedSec );
		
		// --------========--------========---- ----========--------========--------========--------========--------========
		/// draw all of the game screens, once for each given layer. [0, LayerCount)
        virtual void draw();
		
		// --------========--------========---- ----========--------========--------========--------========--------========
		/// plist parameters
		// - GLOBAL
		static const std::string 	tk_color;
		static const std::string 	tk_pauseLength;
		static const std::string 	tk_fontName;
		static const std::string 	tk_fontText;
		static const std::string 	tk_fontPos;
		static const std::string 	tk_fontScale;
		static const std::string 	tk_fontColor;
		static const std::string 	tk_fontJustification;
		// - BLOCK
		static const std::string 	tk_blockDimensions;
		// - IRIS
		static const std::string	tk_irisBladeCount;
		static const std::string	tk_irisTextureName;
		static const std::string	tk_irisMaxAngle;
		// - SLIDE
		static const std::string	tk_slideMaskTexture;
		static const std::string	tk_slideMaskSize;
		static const std::string 	tk_slideDirection;
		// - WIPE
		static const std::string 	tk_wipeDirection;
		static const std::string 	tk_wipeDirectionIn;
		static const std::string 	tk_wipeDirectionOut;
		// - WIDGET SLIDE
		static const std::string	tk_highestWidgetIndex;
		static const std::string	tk_numScreensInStack;
		static const std::string	tk_slideDirectionIn;
		static const std::string	tk_slideDirectionOut;
		static const std::string	tk_slideStagger;
		
		// - IN/OUT PROPERTIES (REQUIRED FOR EACH TYPE)
		static const std::string	tk_blockIn;
		static const std::string	tk_fadeIn;
		static const std::string	tk_irisIn;
		static const std::string	tk_slideIn;
		static const std::string	tk_wipeIn;
		static const std::string	tk_widgetSlideIn;
		
		static const std::string	tk_blockOut;
		static const std::string	tk_fadeOut;
		static const std::string	tk_irisOut;
		static const std::string	tk_slideOut;
		static const std::string	tk_wipeOut;
		static const std::string	tk_widgetSlideOut;
		
		
	protected:

		void _drawFont();
		
		SpriteBatch										mSpriteBatch;
		
		float											mCurrentTime;
		float											mDuration;
		float											mPauseTime;
		
		bool											mIsExtraFlushFrame;

		Color											mColor;

		bool											mTransitionIn;
		bool											mTransitionOut;

		// optional text member variables
		Color											mStartTextColor;
		Color											mFinalTextColor;
		
		BitmapFont*										mFont;
		std::string										mDisplayText;
		Vector2											mTextPos;
		float											mTextScale;
		SpriteBatch::Justification						mTextJustification;
		Color											mTextColor;
    };
}

#endif	// _TRANSITION_H_
