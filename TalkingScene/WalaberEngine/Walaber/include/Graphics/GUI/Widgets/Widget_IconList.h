#ifndef _WALABER_WIDGET_ICON_LIST_H_
#define _WALABER_WIDGET_ICON_LIST_H_

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
	// widget for a scrollable list of icons.
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	class Widget_IconList : public Widget
	{
	public:
		enum TextureName
		{
			TextureName_BG,
			TextureName_ScrollBar,
			TextureName_IconHilight
		};
		
		Widget_IconList( int name, Vector2 pos, Vector2 size );
		Widget_IconList( int name, Vector2 pos, Vector2 iconSize, int iconsPerRow, int height, int edgePadding, int iconPadding );
		Widget_IconList( int name, Vector2 pos, Vector2 iconSize, int iconsPerRow, int height, int edgePadding, int iconPadding,
						TexturePtr bgTexture, TexturePtr scrollBarTexture, TexturePtr iconHilightTexture);
		~Widget_IconList();
		
		void init();
		
		// valid properties:
		// "IconSize", "IconsPerRow", "EdgePadding", "IconPadding"
		void setProperties( const PropertyList& plist );
		
		void clearIcons();
		void addIcon( TexturePtr texture, int icon_id );
		void addIcon( std::string textureName, int icon_id );
		
		void setIconListSizes( Vector2 iconSize, int iconsPerRow, int height, int edgePadding, int iconPadding );
		
		bool update( float elapsedSec, WidgetActionRet& ret );
		void draw( SpriteBatch* sb );//, const Vector2& offset );
		
		void selectIconWithID(int iconID);
		
		void reset() { mFinger = NULL; }
		
		bool acceptNewFingerDown( int fingerID, FingerInfo* info );
		bool acceptNewFingerEntered( int fingerID, FingerInfo* info );
		bool releaseFingerStayed( int fingerID, FingerInfo* info );
		bool releaseFingerMoved( int fingerID, FingerInfo* info );
		void releaseFingerUp( int fingerID, FingerInfo* info );
		bool releaseFingerLeft( int fingerID, FingerInfo* info );
		void notifyFingerLost( int fingerID, FingerInfo* info );
		
	private:
		struct Icon
		{
			Icon() { identifier = -1; }
			Icon( TexturePtr tex, int _id );
			Icon( std::string texName, int _id );
			
			TexturePtr			texture;
			std::string			textureName;
			int					identifier;
		};
		
		typedef std::vector<Icon>	IconList;
		
		
		void _updateFinger();
		
		int _iconFromPos( Vector2 localTouchPos );
		
		IconList			mIcons;
		int					mIconsPerRow;
		int					mEdgePadding;
		int					mIconPadding;
		
		Vector2				mIconSize;
		
		float				mVel;
		float				mVelDamping;
		
		float				mOffsetPos;
		float				mLastOffsetDelta;
		
		float				mMaxScrollAmt;
		
		int					mTouchedIcon;
		int					mCurrentSelection;
		bool				mGoSelect;
		bool				mScrollingToSelection;
		
		FingerInfo*			mFinger;
	};
}

#endif	// _WALABER_WIDGET_ICON_LIST_H_