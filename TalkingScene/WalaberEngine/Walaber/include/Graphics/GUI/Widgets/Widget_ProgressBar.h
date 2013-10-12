#ifndef _WALABER_WIDGET_PROGRESS_BAR_H_
#define _WALABER_WIDGET_PROGRESS_BAR_H_

#include "GUI.h"
#include "QuadHelper.h"
#include "CircleHelper.h"
#include "Rect.h"
#include "Color.h"


#include <Texture.h>
#include <string>
#include <vector>
#include <list>

namespace Walaber
{
	class SpriteBatch; 
	class BitmapFont; 
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	// widget for a progress bar
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	class Widget_ProgressBar : public Widget
	{
	public:
		enum TextureName
		{
			TextureName_BG,
			TextureName_Fill
		};
		
		enum FillMode 
		{
			FM_STRETCH,
			FM_CLAMP,
			FM_REPEAT
		};
		FillMode stringToFillMode( const std::string& s );
		std::string fillModeToString( FillMode m );
		
		// padding is percent of widget size
		Widget_ProgressBar( int name, Vector2 pos, Vector2 size );
		Widget_ProgressBar( int name, Vector2 pos, Vector2 size, TexturePtr bg, TexturePtr fill, float fillPadding = 0.0f, FillMode fillMode = FM_CLAMP );
		Widget_ProgressBar( int name, Vector2 pos, Vector2 size, TexturePtr bg, TexturePtr fill, float fillPaddingL, float fillPaddingR, 
						   float fillPaddingT, float fillPaddingB, FillMode fillMode = FM_CLAMP );
		~Widget_ProgressBar();
		
		void init();
		
		// valid properties
		// "Min", "Max", "RepeatStep", "FillMode", "Color"
		void setProperties( const PropertyList& plist );
		
		bool update( float elapsedSec, WidgetActionRet& ret );
		void draw( SpriteBatch* sb );//, const Vector2& offset );
		
		void setPadding( float fillPaddingL, float fillPaddingR, float fillPaddingT, float fillPaddingB );
		
		void setFillMode( FillMode fillMode ) { mFillMode = fillMode; }
		
		void setColor( Color c ) { mColor = c; }
		
		void setRepeatStep(float val){ mRepeatStep = val; }
		
		void updateValue(float val); // between 0 and 1
		float getValue();
		
	private:
		void _setValue();
		
		Vector2				mFillOffset;
		Vector2				mFillPos;
		Vector2				mFillScale;
		
		float				mMin;
		float				mMax;
		float				mVal;
		float				mRepeatStep;
		
		FillMode			mFillMode;
		
		bool				mValChanged;
		
		Color				mColor;
	};
}

#endif	// _WALABER_WIDGET_PROGRESS_BAR_H_