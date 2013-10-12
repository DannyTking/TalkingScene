#ifndef _WALABER_WIDGET_COLOR_PICKER_H_
#define _WALABER_WIDGET_COLOR_PICKER_H_

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
	// widget for selecting colors.
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	class Widget_ColorPicker : public Widget
	{
	public:
		enum TextureName
		{
			TextureName_BG,				// 0 = BG
			TextureName_Swatch,			// 1 = Swatch
			TextureName_SwatchHilight,	// 2 = swatch hilight
			TextureName_Icon			// 3 = icon
		};
		
		Widget_ColorPicker( int name, Vector2 pos, Vector2 size );
		Widget_ColorPicker( int name, Vector2 pos, Vector2 size, int columnCount, int rowCount, float edgeAmt = 0.6f );
		Widget_ColorPicker( int name, Vector2 pos, Vector2 size, int columnCount, int rowCount, float edgeAmt, 
						   int edgePadding, TexturePtr bgTexture, TexturePtr swatchTexture, TexturePtr swatchHilightTexture );
		~Widget_ColorPicker();
		
		void init();
		
		// valid properties:
		// "Dimensions" -> string "r:c" format, r = rows, c = columns
		// "EdgeAmount", "EdgePadding", "IconDrawSize", "Selection" (color32), "FireOnUp"
		void setProperties( const PropertyList& plist );
		
		
		void setColumnCount(int columnCount) { mColumnCount = columnCount; _setupColorSwatches(); }
		void setRowCount(int rowCount) { mRowCount = rowCount; _setupColorSwatches(); }
		void setEdgeAmount(float edgeAmt) { mEdgeAmount = edgeAmt; _setupColorSwatches(); }
		void setEdgePadding(int edgePadding) { mEdgePadding = edgePadding; _setupColorSwatches(); }
		
		bool update( float elapsedSec, WidgetActionRet& ret );
		void draw( SpriteBatch* sb );//, const Vector2& offset );
		
		void setIconDrawSize( Vector2 drawSize);
		
		void setSelection( const Color& col, bool fireEvent = true );
		
		void setFireOnUp( const bool v ) { mFireOnUp = v; }
		
		Color getColorFromIndex( int i ) { return mColors[i]; }
		
		bool acceptNewFingerDown( int fingerID, FingerInfo* info );
		bool acceptNewFingerEntered( int fingerID, FingerInfo* info );
		bool releaseFingerStayed( int fingerID, FingerInfo* info );
		bool releaseFingerMoved( int fingerID, FingerInfo* info );
		void releaseFingerUp( int fingerID, FingerInfo* info );
		bool releaseFingerLeft( int fingerID, FingerInfo* info );
		void notifyFingerLost( int fingerID, FingerInfo* info );
		
	private:
		
		void _setupColorSwatches();
		void _updateFinger( FingerInfo* fi );
		
		int					mRowCount;
		int					mColumnCount;
		Color*				mColors;
		int					mEdgePadding;
		float				mEdgeAmount;
		Vector2				mSwatchSize;
		bool				mColorChanged;
		int					mColorColumn;
		int					mColorRow;
		bool				mFireOnUp;
        bool				mGoFire;
		
		Vector2				mIconTextureSize;
	};
}

#endif	// _WALABER_WIDGET_COLOR_PICKER_H_