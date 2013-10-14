#include "Widget_Group.h"

#include "Widget_Label.h"
#include "Widget_PushButton.h"
#include "VectorTools.h"

#include "Texture.h"

#include "BitmapFont.h"
#include "GraphicsGL.h"
#include "SpriteBatch.h"

namespace Walaber
{	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	// CHAD-O-TYPE: widget for selecting from a scrollable section of widgets
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	Widget_Group::Widget_Group( int name, Vector2 pos, Vector2 size ) :
	Widget( name, WT_GROUP, pos, size, WO_PortraitUp )
	{
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	Widget_Group::~Widget_Group()
	{
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget_Group::update( float elapsedSec, WidgetActionRet& ret )
	{
		return false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_Group::draw(SpriteBatch* sb)//, const Vector2& offset )
	{
		// debug outline
		Widget::draw(sb);//, offset);
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_Group::addWidget( Widget* wp )
	{
		// add this widget to the list
		WidgetInfo wi( wp, wp->getLocalPosition2D() - getLocalPosition2D() );
		mWidgets.insert( wi );
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	Widget* Widget_Group::getWidget( int widgetName )
	{
		std::string widgetNameStr = StringHelper::intToStr(widgetName);
		
		// return the widget
		for (WidgetList::iterator it = mWidgets.begin(); it != mWidgets.end(); ++it)
		{
			if ((*it).widget->getWidgetName() == widgetNameStr)
			{
				return (*it).widget;
			}
		}
		
		return NULL;
	}
	
    void Widget_Group::_duplicateWidgets()
    {
        // duplicate the widget list and remove all the old ones
        WidgetList newList = mWidgets;
        mWidgets.clear();
        
        for (WidgetList::iterator it = newList.begin(); it != newList.end(); ++it)
		{
            int widgetType = (*it).widget->getWidgetType();

            Vector2 offset = (*it).offset;
            
            // add this widget to the list
            switch (widgetType)
            {
                case WT_PUSH_BUTTON:
                {
                    Widget_PushButton* widgetToCopy = (Widget_PushButton*)(*it).widget;
                    Widget_PushButton* newWidget = new Widget_PushButton( *(widgetToCopy) );
                    WidgetInfo newWi( newWidget, offset );
                    mWidgets.insert( newWi );
                    break;
                }
                    
                case WT_LABEL:
                {
                    Widget_Label* widgetToCopy = (Widget_Label*)(*it).widget;
                    Widget_Label* newWidget = new Widget_Label( *(widgetToCopy) );
                    WidgetInfo newWi( newWidget, offset );
                    mWidgets.insert( newWi );
                    break;
                }
                    
                default:
                {
                    Widget* widgetToCopy = (*it).widget;
                    Widget* newWidget = new Widget( *(widgetToCopy) );
                    WidgetInfo newWi( newWidget, offset );
                    mWidgets.insert( newWi );
                    break;
                }
            }
		}
    }
    
    std::vector<Widget*> Widget_Group::getWidgets()
    {
        std::vector<Widget*> widgetsInGroup;
        
        _duplicateWidgets();
        
        for (WidgetList::iterator it = mWidgets.begin(); it != mWidgets.end(); ++it)
        {
            widgetsInGroup.push_back((*it).widget);
        }
        
        return widgetsInGroup;
    }
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_Group::setLocalPosition2D(const Vector2& pos )
	{
		// Reposition all of the widgets in the group
		for (WidgetList::iterator it = mWidgets.begin(); it != mWidgets.end(); ++it)
		{
			Vector2 tempOffset = (*it).widget->getWorldPosition2D() - this->getWorldPosition2D();
			
			// Because widgets are stupid (and don't override the default local position,
			//	we need to manually catch that case.
			if ((*it).widget->getWidgetType() == WT_GROUP)
			{
				((Widget_Group*)(*it).widget)->setLocalPosition2D( pos + tempOffset );
			}
			else 
			{
				(*it).widget->setLocalPosition2D( pos + tempOffset );
			}
		}
		
		Widget::setLocalPosition2D( pos );

	}

	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_Group::applyPositionOffset2D( const Vector2& pos )
	{
		// Reposition all of the widgets in the group
		for (WidgetList::iterator it = mWidgets.begin(); it != mWidgets.end(); ++it)
		{
			(*it).widget->applyPositionOffset2D( pos );
		}
		
		Widget::applyPositionOffset2D( pos );
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_Group::setLocalScale2D( const Vector2& scale )
	{
		Widget::setLocalScale2D( scale );
		
		// Reposition all of the widgets in the group
		for (WidgetList::iterator it = mWidgets.begin(); it != mWidgets.end(); ++it)
		{
			Vector2 newPos = getLocalPosition2D() + (*it).offset * getLocalScale2D();
			(*it).widget->setLocalPosition2D(newPos);
			
			(*it).widget->setLocalScale2D( scale );
		}
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_Group::applyScaleOffset2D( const Vector2& scale )
	{
		Widget::applyScaleOffset2D( scale );
		
		// Reposition all of the widgets in the group
		for (WidgetList::iterator it = mWidgets.begin(); it != mWidgets.end(); ++it)
		{
			Vector2 newPos = getLocalPosition2D() + (*it).offset * getLocalScale2D();
			(*it).widget->setLocalPosition2D(newPos);
			
			(*it).widget->applyScaleOffset2D( scale );
		}
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_Group::setVisible( bool visible )
	{
		// Reposition all of the widgets in the group
		for (WidgetList::iterator it = mWidgets.begin(); it != mWidgets.end(); ++it)
		{
			(*it).widget->setVisible( visible );
		}
		
		Widget::setVisible( visible );
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget_Group::setColor( const Color& color )
	{
		// Set the color for every widget in this group
		for (WidgetList::iterator it = mWidgets.begin(); it != mWidgets.end(); ++it)
		{
			// Label
			if ((*it).widget->getWidgetType() == WT_LABEL)
			{
				Widget_Label* temp = (Widget_Label*)(*it).widget;
				
				// Change the text color
				temp->setTextDisplayColor(color);
				
				// Change the texture color, only if a texture color exists
				if (temp->getTexture(Widget_Label::TextureName_BG))
				{
					temp->setBGColorBlend(color);
				}
			}
			// Button
			else if ((*it).widget->getWidgetType() == WT_PUSH_BUTTON)
			{
				// Change the text and texture color
				Widget_PushButton* temp = (Widget_PushButton*)(*it).widget;
				
				temp->setTextureColor(color);
				temp->setTextDisplayColor(color);
			}
			// Group
			else if ((*it).widget->getWidgetType() == WT_GROUP)
			{
				// Change the text and texture color
				Widget_Group* temp = (Widget_Group*)(*it).widget;
				
				temp->setColor(color);
			}
		}
	}
}


