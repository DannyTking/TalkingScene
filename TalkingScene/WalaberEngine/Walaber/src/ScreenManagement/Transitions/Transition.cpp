#include "Transition.h"

#include "FontManager.h"

namespace Walaber
{
	
	// --------========--------========--------========--------========--------========--------========--------========
	// GLOBAL
	const std::string Transition::tk_color					= "color";
	const std::string Transition::tk_pauseLength			= "pauseLength";
	const std::string Transition::tk_fontName				= "fontName";
	const std::string Transition::tk_fontText				= "fontText";
	const std::string Transition::tk_fontPos				= "fontPosition";
	const std::string Transition::tk_fontScale				= "fontScale";
	const std::string Transition::tk_fontColor				= "fontColor";
	const std::string Transition::tk_fontJustification		= "fontJustification";
	// - BLOCK
	const std::string Transition::tk_blockDimensions		= "blockDimensions";
	// - IRIS
	const std::string Transition::tk_irisBladeCount			= "irisBladeCount";
	const std::string Transition::tk_irisTextureName		= "irisTextureName";
	const std::string Transition::tk_irisMaxAngle			= "irisMaxAngle";
	// - SLIDE
	const std::string Transition::tk_slideMaskTexture		= "slideMaskTexture";
	const std::string Transition::tk_slideMaskSize			= "slideMaskSize";
	const std::string Transition::tk_slideDirection			= "slideDirection";
	// - WIPE
	const std::string Transition::tk_wipeDirection			= "wipeDirection";
	const std::string Transition::tk_wipeDirectionIn		= "wipeDirectionIn";
	const std::string Transition::tk_wipeDirectionOut		= "wipeDirectionOut";
	// - WIDGET SLIDE
	const std::string Transition::tk_highestWidgetIndex		= "highestWidgetIndex";
	const std::string Transition::tk_numScreensInStack		= "numScreensInStack";
	const std::string Transition::tk_slideDirectionIn		= "startPos";
	const std::string Transition::tk_slideDirectionOut		= "startPos";
	const std::string Transition::tk_slideStagger			= "slideStagger";
	
	
	// - IN/OUT PROPERTIES (REQUIRED FOR EACH TYPE)
	const std::string Transition::tk_blockIn				= "blockIn";
	const std::string Transition::tk_fadeIn					= "fadeIn";
	const std::string Transition::tk_irisIn					= "irisIn";
	const std::string Transition::tk_slideIn				= "slideIn";
	const std::string Transition::tk_wipeIn					= "wipeIn";
	const std::string Transition::tk_widgetSlideIn			= "widgetSlideIn";
	
	const std::string Transition::tk_blockOut				= "blockOut";
	const std::string Transition::tk_fadeOut				= "fadeOut";
	const std::string Transition::tk_irisOut				= "irisOut";
	const std::string Transition::tk_slideOut				= "slideOut";
	const std::string Transition::tk_wipeOut				= "wipeOut";
	const std::string Transition::tk_widgetSlideOut			= "widgetSlideOut";
	
	
	// --------========--------========--------========--------========--------========--------========--------========
	// --------========--------========--------========--------========--------========--------========--------========
	Transition::Transition() : mFont(NULL),
	mIsExtraFlushFrame(false),
	mTransitionIn(true),
	mTransitionOut(true),
	mCurrentTime(0.0f)
	{
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	Transition::~Transition()
	{
	}
	
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Transition::setup( float duration ) 
	{
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Transition::setup( float duration, const PropertyList& plist ) 
	{		
		setup( duration );

		// pause duration
		if (plist.keyExists(tk_pauseLength))
		{
			mPauseTime = plist[tk_pauseLength].asFloat();
		}

		// color
		if(plist.keyExists(tk_color))
		{
			mColor = plist[tk_color].asColor();
		} 


		// optional text drawing
		if (plist.keyExists(tk_fontName))
		{
			mFont = FontManager::getInstancePtr()->getFont( plist[tk_fontName].asString() );
			
			mDisplayText = plist[tk_fontText].asString();
			
			mTextPos = ScreenCoord(0.5f, 0.5f).toScreen();
			
			if (plist.keyExists(tk_fontPos))
				mTextPos = ScreenCoord( plist[tk_fontPos].asVector2() ).toScreen();
			
			
			mTextScale = 1.0f;			
			if (plist.keyExists(tk_fontScale))
				mTextScale = plist[tk_fontScale].asFloat();
			

			mTextJustification = SpriteBatch::JUSTIFICATION_CENTER;
			
			if (plist.keyExists(tk_fontJustification))
			{
				std::string lower = StringHelper::toLower( plist[tk_fontJustification].asString() );
				if (lower == "left")
					mTextJustification = SpriteBatch::JUSTIFICATION_LEFT;
				else if (lower == "right")
					mTextJustification = SpriteBatch::JUSTIFICATION_RIGHT;
			}
			
			mStartTextColor = Color( 0, 0, 0, 0 );
			mFinalTextColor = Color::White;
			
			mTextColor = mStartTextColor;
			
			if (plist.keyExists(tk_fontColor))
			{
				mFinalTextColor = plist[tk_fontColor].asColor();
			}

				
		}
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	int Transition::update( float elapsedSec ) 
	{
		return TP_Finished;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Transition::draw()
	{	
	}


	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Transition::_drawFont()
	{
		if (mFont)
		{
			mSpriteBatch.drawString(mFont, mDisplayText, mTextPos, 0.0f, mTextScale, mTextColor, mTextJustification ); 
		}
	}
}
