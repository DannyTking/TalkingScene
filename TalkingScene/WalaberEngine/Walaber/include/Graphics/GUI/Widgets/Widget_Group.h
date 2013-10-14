#ifndef _WALABER_WIDGET_GROUP_H_
#define _WALABER_WIDGET_GROUP_H_

#include "GUI.h"
#include "QuadHelper.h"
#include "CircleHelper.h"
#include "Rect.h"
#include "Color.h"
#include "StringHelper.h"


#include <Texture.h>
#include <string>
#include <vector>
#include <list>

namespace Walaber
{
	class SpriteBatch; 
	class BitmapFont; 
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	// CHAD-O-TYPE: widget for a conatiner/group of widgets
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	class Widget_Group : public Widget
	{
	public:
		Widget_Group( int name, Vector2 pos, Vector2 size );
		~Widget_Group();
		
		void init();
		
		bool update( float elapsedSec, WidgetActionRet& ret );
		void draw( SpriteBatch* sb );//, const Vector2& offset );
		
		void addWidget( Widget* wp );
		Widget* getWidget( int widgetName );
        
        /*!
         @return return the list of widgets that are on this group
         */
        std::vector<Widget*> getWidgets();
		
		void setLocalPosition2D(const Vector2& pos);
		void applyPositionOffset2D(const Vector2& pos);
		
		void setLocalScale2D(const Vector2& scale);
		void applyScaleOffset2D(const Vector2& scale);
		
		void setVisible( bool visible );
		void setColor( const Color& color );
		
	private:
		
		struct WidgetInfo
		{
			WidgetInfo() : widget(NULL), offset(Vector2::Zero) {}
			WidgetInfo( Widget* w, const Vector2& o ) : widget(w), offset(o) {}
			
			bool operator<( const WidgetInfo& widgetInfo )const
			{
				return (this->widget < widgetInfo.widget);
			}
			
			Widget*			widget;
			Vector2			offset;
		};
		
		typedef std::set< WidgetInfo >		WidgetList;
		
		void _updateFinger();
        
        /*!
         Duplicate the list of widgets on this group
         */
        void _duplicateWidgets();
		
		FingerInfo*						mFinger;
		
		WidgetList						mWidgets;
	};
}

#endif	// _WALABER_WIDGET_GROUP_H_