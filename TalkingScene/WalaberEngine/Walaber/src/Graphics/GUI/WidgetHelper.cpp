/*
 *  WidgetHelper.cpp
 *  JellyCar3
 *
 *  Created by jpecho on 11/29/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "WidgetHelper.h"

#include <TextureManager.h>
#include <TextManager.h>
#include <StringHelper.h>
#include <FileManager.h>
#include <FontManager.h>
#include <assert.h>

namespace Walaber
{
	// --------------------------------------------------##--------------------------------------------------##
	// --------------------------------------------------##--------------------------------------------------##
    WidgetHelper::FileParamMap  WidgetHelper::mCallbackMap;      
	WidgetHelper::StringVector	WidgetHelper::mFilesToRead;
	WidgetHelper::WidgetPrototypeMap	WidgetHelper::mPrototypes;

	// --------------------------------------------------##--------------------------------------------------##
    std::string WidgetHelper::texturePath;
	xmlDocPtr WidgetHelper::currentDoc;
	
	// --------------------------------------------------##--------------------------------------------------##
    std::string WidgetHelper::textureTags[] = {
		"texName",
		"texNormalName",
		"texHilightName",
		"texActiveName",
		"texInactiveName",
		"texBGName",
		"texSwatchName",
		"texSwatchHilightName",
		"texScrollBarName",
		"texIconHilightName",
		"texOnName",
		"texOffName",
		"texHandleName"
	};
	int WidgetHelper::numTextureTags = 13;
		
	
	
	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    void WidgetHelper::_clearPrototypes()
	{
		for (WidgetPrototypeMap::iterator it = mPrototypes.begin(); it != mPrototypes.end(); ++it)
		{
			Widget* w = it->second;
			delete w;
		}
		
		mPrototypes.clear();
	}
	
	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    void WidgetHelper::_copyFromPrototype( xmlNodePtr node, Widget* newWidget, Widget* prototype, Widget* parent, int widgetType )
	{
		if (newWidget == NULL)
			return;
		
		if (prototype == NULL)
			return;
		
		if (prototype->getWidgetType() != newWidget->getWidgetType())
		{
			Walaber::Logger::printf("Walaber", Logger::SV_ERROR, "Error! widget and its prototype are different types: %d is %d, prototype is %d\n",
									newWidget->getWidgetNameAsInt(), newWidget->getWidgetType(), prototype->getWidgetType());
			return;
		}
		
		// preserve name.
		std::string prevName = newWidget->getWidgetName();
		
		// copy!
		switch(widgetType)
		{
				
			case WT_CLASSIC_CONTROLS:	
				(*(Widget_ClassicControls*)newWidget) = (*(Widget_ClassicControls*)prototype);
				break;
				
			case WT_PUSH_BUTTON:
				(*(Widget_PushButton*)newWidget) = (*(Widget_PushButton*)prototype);
				break;
				
			case WT_TIMED_BUTTON:
				(*(Widget_TimedButton*)newWidget) = (*(Widget_TimedButton*)prototype);
				break;
				
			case WT_SPINNER:	
				(*(Widget_Spinner*)newWidget) = (*(Widget_Spinner*)prototype);
				break;
				
			case WT_COLOR_PICKER:
				(*(Widget_ColorPicker*)newWidget) = (*(Widget_ColorPicker*)prototype);
				break;
				
			case WT_ICON_LIST:
				(*(Widget_IconList*)newWidget) = (*(Widget_IconList*)prototype);
				break;
				
			case WT_CANVAS:		
				(*(Widget_Canvas*)newWidget) = (*(Widget_Canvas*)prototype);
				break;
				
			case WT_MANIPULATOR:	
				(*(Widget_Manipulator*)newWidget) = (*(Widget_Manipulator*)prototype);
				break;
				
			case WT_TOGGLE:		
				(*(Widget_Toggle*)newWidget) = (*(Widget_Toggle*)prototype);
				break;
				
			case WT_SLIDER:	
				(*(Widget_Slider*)newWidget) = (*(Widget_Slider*)prototype);
				break;
				
			case WT_LABEL:
				(*(Widget_Label*)newWidget) = (*(Widget_Label*)prototype);
				break;
				
			case WT_SLIDEWHEEL:	
				(*(Widget_SlideWheel*)newWidget) = (*(Widget_SlideWheel*)prototype);
				break;
				
			case WT_PROGRESS_BAR:
				(*(Widget_ProgressBar*)newWidget) = (*(Widget_ProgressBar*)prototype);
				break;
				
			case WT_MOVING_TEXT_BOX:
				(*(Widget_MovingTextBox*)newWidget) = (*(Widget_MovingTextBox*)prototype);
				break;
				
			case WT_FINGER_CATCHER:	
				(*(Widget_FingerCatcher*)newWidget) = (*(Widget_FingerCatcher*)prototype);
				break;
				
			case WT_GROUP:
				(*(Widget_Group*)newWidget) = (*(Widget_Group*)prototype);
				break;
				
			case WT_SCROLLABLE_GROUP:
				(*(Widget_ScrollableGroup*)newWidget) = (*(Widget_ScrollableGroup*)prototype);
				break;
				
			case WT_SCROLLABLE_SET:	
				(*(Widget_ScrollableSet*)newWidget) = (*(Widget_ScrollableSet*)prototype);
				break;
                
            case WT_SCORE_COUNTER:
                (*(Widget_ScoreCounter*)newWidget) = (*(Widget_ScoreCounter*)prototype);
                break;
                
            case WT_SCROLLABLE_CAMERA:
                (*(Widget_ScrollableCamera*)newWidget) = (*(Widget_ScrollableCamera*)prototype);
                break;
            case WT_ANIMATION:
                (*(Widget_Animation*)newWidget) = (*(Widget_Animation*)prototype);
                break;
				
			default:
				break;
		}
		
		//*newWidget = *prototype;
		
		// restore values
		newWidget->setName(prevName);
		
		
		
		// Set the size and position again (if specified for this node)
		Vector2 size = _parseSizeToScreen(node, newWidget);
		Vector2 position = _parsePositionToScreen(node, parent, size);
		
		if (XML::attrExists(node,"pos"))
			newWidget->setLocalPosition2D( position );
		
		if (XML::attrExists(node,"size"))
			newWidget->setBaseSize( size );
	}
	
		
	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    bool WidgetHelper::getTextureNames(const std::string& filename, std::vector<std::string>& textureList)
	{
		bool ret = true;
		
		xmlNodePtr root_element = _openXMLFile(filename);
		if(!root_element)
		{
			ret = false;
		}
		else
		{
#ifdef _DEBUG
			printf("root element: %s\n", root_element->name);
#endif
			xmlNodePtr widget = root_element->children;
			
#ifdef _DEBUG
			printf("\nWidgetHelper getTextureNames - parsing widgets\n");
#endif
			std::string textureName;
			while (widget)
			{
#if _DEBUG
				printf("node name: %s\n", widget->name);
#endif
				if (xmlStrcmp(widget->name, (const xmlChar*) "Widget") == 0)
				{
#ifdef _DEBUG
					printf("\nWidgetHelper getTextureNames - found widget");
#endif				
					
					if(_getTextureName(widget, textureName))
					{
						textureList.push_back(texturePath+textureName);
					}						
				}
				
				widget = widget->next;
				
			}
		}
		
		return ret;
	}
	
	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    void WidgetHelper::loadWidgetsXML(const std::string& xmlFile, WidgetManager* widgetMgr, CallbackPtr callback)
	{
        LoadPassThroughParams params;
        params.mWidgetMgr = widgetMgr;
        params.mCallback = callback;
        
        // make sure we arent already reading this same file, why would you want to read it twice?
		if (mCallbackMap.find(xmlFile) == mCallbackMap.end())
        {
			mCallbackMap[xmlFile] = params;
        
#ifdef _DEBUG
			printf("\nloadWidgetsXML - filename:%s\n", xmlFile.c_str());
#endif
        
			PropertyList plist;
			FileManager::getInstancePtr()->readFile(xmlFile, CallbackPtr( new Callback(&WidgetHelper::_recurseFileReadCallback) ), plist);
		}
		else
		{
			printf("Already trying to load widget file: %s\n", xmlFile.c_str());
		}
	}

	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    xmlNodePtr WidgetHelper::_openXMLFile(const std::string& filename)
	{
		// find out which device you are on and load the corresponding xml file
		std::string platFile = filename; 
		
		// parse the XML document.
		currentDoc = NULL;
		xmlNodePtr root_element = NULL;
		
	#ifdef _DEBUG
		printf("\nloadWidgetsXML - filename:%s\n", platFile.c_str());
	#endif
		
		// read in the document into memory!
		root_element = XML::loadDoc( platFile, "Widgets", currentDoc );
		
		if (root_element != NULL)
			setTexturePath(_parseString(root_element, "texturePath"));
		
		return root_element;
	}

	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    void WidgetHelper::_closeXMLFile()
	{
		XML::cleanupDoc( currentDoc );
	}
					   
	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    bool WidgetHelper::_getTextureName(xmlNodePtr node, std::string& texName)
	{
		bool ret = false;
		int i = 0;
		while(!ret && (i < numTextureTags))
		{
			texName = _parseString(node, textureTags[i].c_str());
			if(texName.size())
			{
				ret = true;
			}
			
			++i;
		}
		
		return ret;
	}
	
	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    void WidgetHelper::_createAndUpdateWidgetPrototypes(xmlNodePtr root_node)
	{
		// loop through looking for "WidgetPrototype" entries...
		for (xmlNodePtr kid = root_node->children; kid != NULL; kid = kid->next)
		{
			if (xmlStrcmp(kid->name, (const xmlChar*)"WidgetPrototype") == 0)
			{
				// this is a WidgetPrototype!
				std::string name = _parseString(kid, "name");
				
				std::string widgetTypeStr = XML::parseString(kid, "type");
				WidgetType widgetType;
				Widget::getWidgetTypeFromString(widgetTypeStr, widgetType);
				
				Walaber::Logger::printf("Walaber", Logger::SV_DEBUG, "Loading widget prototype: %s\n", name.c_str());
				
				
				// see if it exists in the map already...
				Widget* w = NULL;
				
				if (mPrototypes.find(name) != mPrototypes.end())
				{
					// already exists, just update it's values!
					w = mPrototypes[name];
				}
				
				
				Vector2 size = _parseSizeToScreen(kid, NULL);
				Vector2 pos = _parsePositionToScreen(kid, NULL, size);
				
				w = _createAndUpdateWidget(kid, NULL, widgetType, -1, pos, size, w);
				
				
				// make sure it's in the map.
				mPrototypes[name] = w;
			}
		}
	}
	
	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    Widget* WidgetHelper::_createBaseWidget(xmlNodePtr node, Widget* parent, int widgetType, WidgetManager* widgetMgr)
	{
#ifdef _DEBUG
		printf("type: %d\n", widgetType);
#endif
		
		// Create the widget if it doesn't already exist
		Widget* baseWidget = NULL;
		
		int id = _parseInt(node, "id");
		
		bool foundBase = false;
		
		if (widgetMgr != NULL)
		{
			if ((widgetMgr->getWidget(id) != NULL) && (id >= 0))
			{
				baseWidget = widgetMgr->getWidget(id);
				foundBase = true;
				
				if (baseWidget->getWidgetType() != widgetType)
				{
					printf("Error: trying to override a widget using a different widget type!!\n");
					return NULL;
				}
			}
		}
		
		
		// Set the size and position if this widget already exists (these values impact specific data) 
		Vector2 size = _parseSizeToScreen(node, baseWidget);
		Vector2 position = _parsePositionToScreen(node, parent, size);
		
		if (baseWidget)
		{
			if (XML::attrExists(node,"pos"))
			{
				baseWidget->setLocalPosition2D( position );
			}
			if (XML::attrExists(node,"size"))
			{
				baseWidget->setBaseSize( size );
			}
		}
		
		baseWidget = _createAndUpdateWidget(node, parent, widgetType, id, position, size, baseWidget);
		
		if (foundBase)
			baseWidget = NULL;
		
		return baseWidget;
	}	
		
	
	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    Widget* WidgetHelper::_createAndUpdateWidget(xmlNodePtr node, Widget* parent, int widgetType,
												 int id, Vector2 position, Vector2 size, Widget* baseWidget )
	{
		// look for a prototype...
		std::string prototypeName = "NONE";
		if (XML::attrExists(node, "prototype"))
			prototypeName = _parseString(node, "prototype");
		
		Widget* protWidget = NULL;
		if (mPrototypes.find(prototypeName) != mPrototypes.end())
			protWidget = mPrototypes[prototypeName];
		
		
		// Then update the widget with the specific data!
		switch(widgetType)
		{
				
			case WT_CLASSIC_CONTROLS:	
				if (!baseWidget)
				{
					baseWidget = new Widget_ClassicControls(id, position, size);	
					_copyFromPrototype(node, baseWidget, protWidget, parent, widgetType);
				}
				_updateWidgetClassicControls(node, (Widget_ClassicControls*)baseWidget, parent);
				break;
				
			case WT_PUSH_BUTTON:
				if (!baseWidget)
				{
					baseWidget = new Widget_PushButton(id, position, size);
					_copyFromPrototype(node, baseWidget, protWidget, parent, widgetType);
				}
				_updateWidgetPushButton(node, (Widget_PushButton*)baseWidget, parent);																
				break;
				
			case WT_TIMED_BUTTON:		
				if (!baseWidget)
				{
					baseWidget = new Widget_TimedButton(id, position, size);	
					_copyFromPrototype(node, baseWidget, protWidget, parent, widgetType);
				}
				_updateWidgetTimedButton(node, (Widget_TimedButton*)baseWidget, parent);
				break;
				
			case WT_SPINNER:											
				if (!baseWidget)
				{
					baseWidget = new Widget_Spinner(id, position, size);	
					_copyFromPrototype(node, baseWidget, protWidget, parent, widgetType);
				}
				_updateWidgetSpinner(node, (Widget_Spinner*)baseWidget, parent);															
				break;
				
			case WT_COLOR_PICKER:		
				if (!baseWidget)
				{
					baseWidget = new Widget_ColorPicker(id, position, size);	
					_copyFromPrototype(node, baseWidget, protWidget, parent, widgetType);
				}
				_updateWidgetColorPicker(node, (Widget_ColorPicker*)baseWidget, parent);
				break;
				
			case WT_ICON_LIST:		
				if (!baseWidget)
				{
					baseWidget = new Widget_IconList(id, position, size);	
					_copyFromPrototype(node, baseWidget, protWidget, parent, widgetType);
				}
				_updateWidgetIconList(node, (Widget_IconList*)baseWidget, parent);
				break;
				
			case WT_CANVAS:			
				if (!baseWidget)
				{
					baseWidget = new Widget_Canvas(id, position, size);	
					_copyFromPrototype(node, baseWidget, protWidget, parent, widgetType);
				}
				_updateWidgetCanvas(node, (Widget_Canvas*)baseWidget, parent);
				break;
				
			case WT_MANIPULATOR:								
				break;
				
			case WT_TOGGLE:		
				if (!baseWidget)
				{
					baseWidget = new Widget_Toggle(id, position, size);
					_copyFromPrototype(node, baseWidget, protWidget, parent, widgetType);
				}
				_updateWidgetToggle(node, (Widget_Toggle*)baseWidget, parent);	
				break;
				
			case WT_SLIDER:		
				if (!baseWidget)
				{
					baseWidget = new Widget_Slider(id, position, size);
					_copyFromPrototype(node, baseWidget, protWidget, parent, widgetType);
				}
				_updateWidgetSlider(node, (Widget_Slider*)baseWidget, parent);	
				break;

			case WT_LABEL:
				if (!baseWidget)
				{
					baseWidget = new Widget_Label(id, position, size);	
					_copyFromPrototype(node, baseWidget, protWidget, parent, widgetType);
				}
				_updateWidgetLabel(node, (Widget_Label*)baseWidget, parent);
				break;

			case WT_SLIDEWHEEL:		
				if (!baseWidget)
				{
					baseWidget = new Widget_SlideWheel(id, position, size);	
					_copyFromPrototype(node, baseWidget, protWidget, parent, widgetType);
				}
				_updateWidgetSlideWheel(node, (Widget_SlideWheel*)baseWidget, parent);
				break;
				
			case WT_PROGRESS_BAR:		
				if (!baseWidget)
				{
					baseWidget = new Widget_ProgressBar(id, position, size);	
					_copyFromPrototype(node, baseWidget, protWidget, parent, widgetType);
				}
				_updateWidgetProgressBar(node, (Widget_ProgressBar*)baseWidget, parent);
				break;
				
			case WT_MOVING_TEXT_BOX:		
				if (!baseWidget)
				{
					baseWidget = new Widget_MovingTextBox(id, position, size);	
					_copyFromPrototype(node, baseWidget, protWidget, parent, widgetType);
				}
				_updateWidgetMovingTextBox(node, (Widget_MovingTextBox*)baseWidget, parent);	
				break;
				
			case WT_FINGER_CATCHER:		
				if (!baseWidget)
				{
					baseWidget = new Widget_FingerCatcher(id, position, size);	
					_copyFromPrototype(node, baseWidget, protWidget, parent, widgetType);
				}
				_updateWidgetFingerCatcher(node, (Widget_FingerCatcher*)baseWidget, parent);
				break;
				
			case WT_GROUP:		
				if (!baseWidget)
				{
					baseWidget = new Widget_Group(id, position, size);	
					_copyFromPrototype(node, baseWidget, protWidget, parent, widgetType);
				}
				_updateWidgetGroup(node, (Widget_Group*)baseWidget, parent);
				break;
				
			case WT_SCROLLABLE_GROUP:		
				if (!baseWidget)
				{
					baseWidget = new Widget_ScrollableGroup(id, position, size);	
					_copyFromPrototype(node, baseWidget, protWidget, parent, widgetType);
				}
				_updateWidgetScrollableGroup(node, (Widget_ScrollableGroup*)baseWidget, parent);
				break;
				
			case WT_SCROLLABLE_SET:		
				if (!baseWidget)
				{
					baseWidget = new Widget_ScrollableSet(id, position, size);
					_copyFromPrototype(node, baseWidget, protWidget, parent, widgetType);
				}
				_updateWidgetScrollableSet(node, (Widget_ScrollableSet*)baseWidget, parent);	
				break;
                
            case WT_SCROLLABLE_CAMERA:
                if (!baseWidget)
                {
                    baseWidget = new Widget_ScrollableCamera(id, position, size);
                    _copyFromPrototype(node, baseWidget, protWidget, parent, widgetType);
                }
                _updateWidgetScrollableCamera(node, (Widget_ScrollableCamera*)baseWidget, parent);
                break;
                
            case WT_SCORE_COUNTER:
				if (!baseWidget)
				{
					baseWidget = new Widget_ScoreCounter(id, position, size);
					_copyFromPrototype(node, baseWidget, protWidget, parent, widgetType);
				}
				_updateWidgetScoreCounter(node, (Widget_ScoreCounter*)baseWidget, parent);	
				break;
            case WT_ANIMATION:
				if (!baseWidget)
				{
					baseWidget = new Widget_Animation(id, position, size);
					_copyFromPrototype(node, baseWidget, protWidget, parent, widgetType);
				}
				_updateWidgetAnimation(node, (Widget_Animation*)baseWidget, parent);
				break;
                
				
			default:
				break;
		}
		
		// And finally, at long last, the remaining everyman data
		_updateBaseWidget(node, baseWidget);

		return baseWidget;
	}
	
	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    void WidgetHelper::_updateBaseWidget(xmlNodePtr node, Widget* widget)
	{
		// visibility is a case where if someone forgets the tag, default is that it should be visible
		// so we do this one separately
		bool visible = true;
		if(XML::attrExists(node, "visible"))
		{
			visible = XML::parseBool(node, "visible");
			widget->setVisible(visible);
		}
		
		// enabled is a case where if someone forgets the tag, default is that it should be visible
		// so we do this one separately
		bool enabled = true;
		if(XML::attrExists(node, "enabled"))
		{
			enabled = XML::parseBool(node, "enabled");
			widget->setEnabled(enabled);
		}
		
		// get the orientation
		WidgetOrientation orientation = WO_PortraitUp;
		if(XML::attrExists(node, "orientation"))
		{
			std::string orientationStr = _parseString(node, "orientation");
			if (StringHelper::toLower(orientationStr) == "portrait_down") 
			{
				orientation = WO_PortraitDown;
			}
			else if (StringHelper::toLower(orientationStr) == "landscape_up") 
			{
				orientation = WO_LandscapeUp;
			}
			else if (StringHelper::toLower(orientationStr) == "landscape_down") 
			{
				orientation = WO_LandscapeDown;
			}
			widget->setOrientation(orientation);
		}
		
		// get layer
		int layer = _extractLayer(node);
		if(XML::attrExists(node, "layer"))
		{
			widget->setLayer(layer);
		}
	}
	
	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    void WidgetHelper::_updateWidgetClassicControls(xmlNodePtr node, Widget_ClassicControls* widget, Widget* parent)
	{
		// do any post construction magic
		
	}
	
	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    void WidgetHelper::_updateWidgetSpinner(xmlNodePtr node, Widget_Spinner* widget, Widget* parent)
	{
		// do any post constructor modifications here
		
		// Texture
		std::string textureName = _parseString(node, "texName");
		TexturePtr tex = _getTexture(textureName);
		widget->setTexture(tex);
	}
	
	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    void WidgetHelper::_updateWidgetPushButton(xmlNodePtr node, Widget_PushButton* widget, Widget* parent)
	{
		// do any post constructor modifications here
		
		// Textures
		if(XML::attrExists(node, "texNormalName"))
		{
			std::string textureNormal = _parseString(node, "texNormalName");	
			TexturePtr texNorm = _getTexture(textureNormal);	
			widget->setTexture( texNorm );
		}
		
		if(XML::attrExists(node, "texHilightName"))
		{
			std::string textureHilight = _parseString(node, "texHilightName");
			TexturePtr texHilight = _getTexture(textureHilight);
			widget->setHilightTexture( texHilight );
		}
		
		// get font		
		std::string fontStr = _parseString(node, "font");
		if(fontStr.size())
		{
			widget->setFont( fontStr );
		}
		
		// Text
		std::string text = _parseString(node, "text");
        if(!text.empty())
        {
            text = TextManager::getString(text);
			widget->setDisplayText(text);
        }
		
		text = _parseString(node, "textNT");
        if(!text.empty())
        {
			widget->setDisplayText(text);
        }
		
		// React on down
		bool reactOnDown = _parseBool(node, "reactOnDown");
		if(XML::attrExists(node, "reactOnDown"))
		{
			widget->setReactOnDown(reactOnDown);
		}
		
		// rotation (in degrees)
		if(XML::attrExists(node, "angleDegrees"))
		{
			float angle = XML::parseFloat(node, "angleDegrees");
			widget->setRotation(angle);
		}
		
		//set highlight color
		if(XML::attrExists(node, "hilightColorRGB"))
		{
			Color hilightColor = _parseColor(node, "hilightColorRGB");
			widget->setHilightColor(hilightColor);
			widget->setTextHilightColor(hilightColor);	
		}
		
		// Text coloring
		Color textColor = Color::Black;
		if(XML::attrExists(node,"textColorRGB"))
		{
			textColor = XML::parseColor8bit(node, "textColorRGB");
			widget->setTextDisplayColor(textColor);
		}		
		
		if(XML::attrExists(node,"textHilightColorRGB"))
		{
			Color textHilightColor = XML::parseColor8bit(node, "textHilightColorRGB");
			widget->setTextHilightColor(textHilightColor);	
		}	
		
		// set disabled color
		if (XML::attrExists(node, "disabledColorRGB"))
		{
			Color disabledColor = _parseColor(node, "disabledColorRGB");
			widget->setDisabledColor(disabledColor);
		}
		
		if (XML::attrExists(node, "disabledTextColorRGB"))
		{
			Color disabledColor = _parseColor(node, "disabledTextColorRGB");
			widget->setDisabledTextColor(disabledColor);
		}
		
		// text padding
		if(XML::attrExists(node,"edgePadding"))
		{
			float edgePadding = _parseFloat(node, "edgePadding");
			widget->setEdgePadding(Vector2(edgePadding, edgePadding));
		}
		else if(XML::attrExists(node,"edgePaddingRelative"))
		{
			float edgePadding = _parseFloat(node, "edgePaddingRelative");
			widget->setEdgePadding( Vector2( edgePadding * widget->getSize().X, edgePadding * widget->getSize().Y) );
		}
		
		// More fine tuned text padding
		if (XML::attrExists(node, "edgePaddingH"))
		{
			float edgePadding = _parseFloat(node, "edgePaddingH");
			Vector2 origEdgePadding = widget->getEdgePadding();
			
			widget->setEdgePadding(Vector2(edgePadding, origEdgePadding.Y));
		}
		else if (XML::attrExists(node, "edgePaddingRelativeH"))
		{
			float edgePadding = _parseFloat(node, "edgePaddingRelativeH");
			Vector2 origEdgePadding = widget->getEdgePadding();
			
			widget->setEdgePadding(Vector2(edgePadding * widget->getSize().X, origEdgePadding.Y));
		}
		
		if (XML::attrExists(node, "edgePaddingV"))
		{
			float edgePadding = _parseFloat(node, "edgePaddingV");
			Vector2 origEdgePadding = widget->getEdgePadding();
			
			widget->setEdgePadding(Vector2(origEdgePadding.X, edgePadding));
		}
		else if (XML::attrExists(node, "edgePaddingRelativeV"))
		{
			float edgePadding = _parseFloat(node, "edgePaddingRelativeV");
			Vector2 origEdgePadding = widget->getEdgePadding();
			
			widget->setEdgePadding(Vector2(origEdgePadding.X, edgePadding * widget->getSize().Y));
		}
		
		
		// Text offset
		if (XML::attrExists(node,"textOffset"))
		{
			Vector2 textOffset = XML::parseVector2(node, "textOffset");
			widget->setTextOffset( textOffset );
		}
		else if (XML::attrExists(node,"textOffsetRelative"))
		{
			Vector2 textOffset = XML::parseVector2(node, "textOffsetRelative");
			widget->setTextOffset( textOffset * widget->getSize() );
		}
		
		if (XML::attrExists(node,"textHilightOffset"))
		{
			Vector2 textOffset = XML::parseVector2(node, "textHilightOffset");
			widget->setTextHilightOffset( textOffset );
		}
		else if (XML::attrExists(node,"textHilightOffsetRelative"))
		{
			Vector2 textOffset = XML::parseVector2(node, "textHilightOffsetRelative");
			widget->setTextHilightOffset( textOffset * widget->getSize() );
		}
	}
	
	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    void WidgetHelper::_updateWidgetTimedButton(xmlNodePtr node, Widget_TimedButton* widget, Widget* parent)
	{
		// do any post constructor modifications here
		
		// Textures
		std::string textureActive = _parseString(node, "texActiveName");
		std::string textureInactive = _parseString(node, "texInactiveName");
		
		//get textures
		TexturePtr activeTex = _getTexture(textureActive);
		TexturePtr inactiveTex = _getTexture(textureInactive);
		
		widget->setActiveTexture( activeTex );
		widget->setInactiveTexture( inactiveTex );
		
		// Time
		float maxTime = _parseFloat(node, "maxTime");
		widget->setMaxTime( maxTime );
	}
	
	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    void WidgetHelper::_updateWidgetColorPicker(xmlNodePtr node, Widget_ColorPicker* widget, Widget* parent)
	{
		// do any post constructor modifications here
		
		// Padding and column counts
		int columnCount = _parseInt(node, "columnCount");
		int rowCount = _parseInt(node, "rowCount");
		float edgeAmt = _parseFloat(node, "edgeAmount");
		int edgePadding = _parseFloat(node, "edgePadding");
		
		widget->setColumnCount( columnCount );
		widget->setRowCount( rowCount );
		widget->setEdgeAmount( edgeAmt );
		widget->setEdgePadding( edgePadding );
		
		// Textures
		std::string bgTexture = _parseString(node, "texBGName");
		std::string swatchTexture = _parseString(node, "texSwatchName");
		std::string swatchHilightTexture = _parseString(node, "texSwatchHilightName");
		
		//get textures
		TexturePtr bgTex = _getTexture(bgTexture);
		TexturePtr swatchTex = _getTexture(swatchTexture);
		TexturePtr swatchHilightTex = _getTexture(swatchHilightTexture);
		
		widget->setTexture( Widget_ColorPicker::TextureName_BG, bgTex );
		widget->setTexture( Widget_ColorPicker::TextureName_Swatch, swatchTex );
		widget->setTexture( Widget_ColorPicker::TextureName_SwatchHilight,  swatchHilightTex );
		
		bool fireOnUp = _parseBool(node, "fireOnUp");
		widget->setFireOnUp(fireOnUp);
	}
	
	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    void WidgetHelper::_updateWidgetIconList(xmlNodePtr node, Widget_IconList* widget, Widget* parent)
	{
		// do post constructor operations here
		
		// Widget list does some crazy size work
		Vector2 iconSize = _parseRealWorldToScreen(node,"iconSizeMM");
		if(iconSize == Vector2::Zero)
		{
			iconSize = _parseScreenCoordToScreen(node,"iconSize");
			
			// check if there is a "forceAspect" property, which will override the HEIGHT setting
			// from above, and instead set a specific aspect ratio.
			if (XML::attrExists(node, "forceIconAspect"))
			{
				float aspectRatio = XML::parseAspectRatio(node, "forceIconAspect");
				iconSize.Y = iconSize.X / aspectRatio;
			}
		}
		
		int iconsPerRow = _parseInt(node, "iconsPerRow");
		
		int height = _parseInt(node, "listHeightMM");
		if (height < 0) 
		{
			float floatHeight = _parseFloat(node, "listHeight");
			height = ScreenCoord(0, floatHeight).toScreen().Y;
		}
		else 
		{
			height = ScreenCoord::screenSizeFromRealWorldSize(Vector2(0,height)).Y;
		}
		
		Vector2 position = _parsePositionToScreen(node, parent, iconSize);
		widget->setLocalPosition2D( position );
		
		int edgePadding = _parseInt(node, "edgePadding");
		int iconPadding = _parseInt(node, "iconPadding");
		
		widget->setIconListSizes( iconSize, iconsPerRow, height, edgePadding, iconPadding );
		
		// Textures
		std::string textureBG = _parseString(node, "texBGName");
		std::string textureScrollBar = _parseString(node, "texScrollBarName");
		std::string textureIconHi = _parseString(node, "texIconHilightName");
		
		//get textures
		TexturePtr texBG = _getTexture(textureBG);
		TexturePtr texScrollBar = _getTexture(textureScrollBar);
		TexturePtr texIconHi = _getTexture(textureIconHi);
		
		widget->setTexture(Widget_IconList::TextureName_BG, texBG );
		widget->setTexture(Widget_IconList::TextureName_ScrollBar, texScrollBar );
		widget->setTexture(Widget_IconList::TextureName_IconHilight, texIconHi );
		
		// add icons if any defined
		int i = 0;
		char iconAttr[64];
		TexturePtr icon;
		std::string iconName;
		do
		{
			snprintf(iconAttr, 64, "icon%d", i);
			iconName = _parseString(node, iconAttr);
			icon = _getTexture(iconName);
			if(icon.use_count())
			{
				widget->addIcon(icon, i);
			}

			++i;
		} while (icon.use_count());	
	}
	
	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    void WidgetHelper::_updateWidgetToggle(xmlNodePtr node, Widget_Toggle* widget, Widget* parent)
	{
		// do post constructor operations here
		
		// Font
		if(XML::attrExists(node,"font"))
		{
			std::string fontStr = _parseString(node, "font");
			widget->setFont( fontStr );
		}
		
		
		// get text
		if(XML::attrExists(node,"textOn"))
		{
			std::string onText = _parseString(node, "textOn");
			widget->setTextOn( onText );
		}
		if(XML::attrExists(node,"textOff"))
		{
			std::string offText = _parseString(node, "textOff");
			widget->setTextOff( offText );
		}
		
		
		// get textures
		if(XML::attrExists(node,"texOnName"))
		{
			std::string textureOn = _parseString(node, "texOnName");
			TexturePtr texOn = _getTexture(textureOn);
			widget->setTextureOn( texOn );
		}
		if(XML::attrExists(node,"texOffName"))
		{
			std::string textureOff = _parseString(node, "texOffName");
			TexturePtr texOff = _getTexture(textureOff);
			widget->setTextureOff( texOff );
		}
		
		
		// Alignment
		if(XML::attrExists(node,"align"))
		{
			std::string alignment = _parseString(node, "align");
			Widget_Toggle::TextAlignment tAlign= Widget_Toggle::TA_Right;
			if(alignment.size())
			{
				if(StringHelper::toLower(alignment) == "leftright")
				{
					tAlign = Widget_Toggle::TA_LeftRight;
				}
			}
			widget->setAlignment(tAlign);
		}
        
        if(XML::attrExists(node, "interactionRectTopLeft"))
        {
            Vector2 topLeft = XML::parseVector2(node, "interactionRectTopLeft");
            widget->setCustomizeAreaTop(topLeft);
        }
        
        if(XML::attrExists(node, "interactionRectBottomRight"))
        {
            Vector2 bottomRight = XML::parseVector2(node, "interactionRectBottomRight");
            widget->setCustomizeAreaBottom(bottomRight);
        }
		
		// Text color
		if(XML::attrExists(node,"textColorRGB"))
		{
			Color textColor = Color::Black;
			if(XML::attrExists(node,"textColorRGB"))
			{
				textColor = XML::parseColor8bit(node, "textColorRGB");
			}
			widget->setTextDisplayColor(textColor);
		}
		
		
		// Text scale
		if(XML::attrExists(node,"textScale"))
		{
			float scale = _parseFloat(node, "textScale");
			if(scale == 0.0f)
			{
				scale = 1.0f;
			}				
			widget->setTextScale(scale);
		}
		
		// Start on?
		if(XML::attrExists(node,"startOn"))
		{
			bool startOn = _parseBool(node, "startOn");
			widget->setOn(startOn);
		}
        
        // Delay before reset?
		if(XML::attrExists(node,"resetDelay"))
		{
			float resetDelay = _parseFloat(node, "resetDelay");
            widget->setResetDelay(resetDelay);
		}
        
        // Delay before reset?
		if(XML::attrExists(node,"sound"))
		{
			int soundID = _parseInt(node, "sound");
            widget->setSoundID(soundID);
		}
	}
	
	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    void WidgetHelper::_updateWidgetSlider(xmlNodePtr node, Widget_Slider* widget, Widget* parent)
	{
		// do post constructor operations here
		
		// textures
		if (XML::attrExists(node, "texBGName"))
		{
			std::string textureBG = _parseString(node, "texBGName");
			TexturePtr texBG = _getTexture(textureBG);
			widget->setBGTexture( texBG );
		}
		
		if (XML::attrExists(node, "texHandleName"))
		{
			std::string textureHandle = _parseString(node, "texHandleName");
			TexturePtr texHandle = _getTexture(textureHandle);
			widget->setHandleTexture( texHandle );
		}
		
		if (XML::attrExists(node, "texStepName"))
		{
			std::string textureStep = _parseString(node, "texStepName");
			TexturePtr texStep = _getTexture(textureStep);
			widget->setStepTexture( texStep );
			
			widget->setDrawSteps( true );
		}
		
		if (XML::attrExists(node, "texLineName"))
		{
			std::string textureLine = _parseString(node, "texLineName");
			TexturePtr texLine = _getTexture(textureLine);
			widget->setLineTexture( texLine );
		}
		
		// Values
		if (XML::attrExists(node, "minSlideVal"))
		{
			float minSlide = _parseFloat(node, "minSlideVal");
			widget->setMinSlide( minSlide );
		}
		if (XML::attrExists(node, "maxSlideVal"))
		{
			float maxSlide = _parseFloat(node, "maxSlideVal");
			widget->setMaxSlide( maxSlide );
		}
		if (XML::attrExists(node, "slideStep"))
		{
			float stepSlide = _parseFloat(node, "slideStep");
			widget->setStepSlide( stepSlide );
		}
		
		// Handle size
		if ((XML::attrExists(node, "handleSizeMM")) || (XML::attrExists(node, "handleSize")))
		{
			Vector2 handleSize = _parseRealWorldToScreen(node,"handleSizeMM");
			if(handleSize == Vector2::Zero)
			{
				handleSize = _parseScreenCoordToScreen(node,"handleSize");
				
				// check if there is a "forceAspect" property, which will override the HEIGHT setting
				// from above, and instead set a specific aspect ratio.
				if (XML::attrExists(node, "forceHandleAspect"))
				{
					float aspectRatio = XML::parseAspectRatio(node, "forceHandleAspect");
					handleSize.Y = handleSize.X / aspectRatio;
				}
			}	
			widget->setHandleSize(handleSize);
		}
		
		// Step size
		if ((XML::attrExists(node, "stepSizeMM")) || (XML::attrExists(node, "stepSize")))
		{
			Vector2 stepSize = _parseRealWorldToScreen(node,"stepSizeMM");
			if(stepSize == Vector2::Zero)
			{
				stepSize = _parseScreenCoordToScreen(node,"stepSize");
				
				// check if there is a "forceAspect" property, which will override the HEIGHT setting
				// from above, and instead set a specific aspect ratio.
				if (XML::attrExists(node, "forceStepAspect"))
				{
					float aspectRatio = XML::parseAspectRatio(node, "forceStepAspect");
					stepSize.Y = stepSize.X / aspectRatio;
				}
			}	
			widget->setStepSize(stepSize);
		}
		
		// angle
		if (XML::attrExists(node, "handleAngle"))
			widget->setHandleAngle( Walaber::degToRad(_parseFloat(node, "handleAngle")) );
	}
	
	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    void WidgetHelper::_updateWidgetLabel(xmlNodePtr node, Widget_Label* widget, Widget* parent)
	{
		// do post constructor operations here
		
		// font
		std::string fontStr = _parseString(node, "font");		
		if(fontStr.size())
		{
			widget->setFont( fontStr );
		}
		
		// text
		std::string text = _parseString(node, "text");
        if(!text.empty())
        {
            text = TextManager::getString(text);
			widget->setText(text);
        }
		
		text = _parseString(node, "textNT");
        if(!text.empty())
        {
			widget->setText(text);
        }
		
		// get textures
        if (XML::attrExists(node, "texBGName"))
		{
			std::string textureBG = _parseString(node, "texBGName");
			TexturePtr texBG = _getTexture(textureBG);
			widget->setTexture( Widget_Label::TextureName_BG, texBG );
		}
		
		// Alignment
		std::string alignment = _parseString(node, "valign");
		Widget_Label::VAlign vAlign= Widget_Label::VA_Center;
		if(alignment.size())
		{
			if(StringHelper::toLower(alignment) == "top")
			{
				vAlign = Widget_Label::VA_Top;
			}
			else if(StringHelper::toLower(alignment) == "bottom")
			{
				vAlign = Widget_Label::VA_Bottom;
			}
			
			widget->setVAlignment(vAlign);
		}
		
		alignment = _parseString(node, "halign");
		Widget_Label::HAlign hAlign= Widget_Label::HA_Center;
		if(alignment.size())
		{
			if(StringHelper::toLower(alignment) == "left")
			{
				hAlign = Widget_Label::HA_Left;
			}
			else if(StringHelper::toLower(alignment) == "right")
			{
				hAlign = Widget_Label::HA_Right;
			}
			
			widget->setHAlignment(hAlign);
		}
		
		// Padding
		// no helper because we have our own default value
        float padL = 0.0f;
		float padR = 0.0f;
		float padT = 0.0f;
		float padB = 0.0f;
		
		bool paddingChanged = false;
		
		if (XML::attrExists(node, "textPadding"))
		{
			float textPadding = _parseFloat(node, "textPadding");
			
			padL = textPadding;
			padR = textPadding;
			padT = textPadding;
			padB = textPadding;
			
			paddingChanged = true;
		}
		
        if (XML::attrExists(node, "paddingL"))
		{
            padL = XML::parseFloat(node, "paddingL");
			paddingChanged = true;
		}
        
        if (XML::attrExists(node, "paddingR"))
		{
            padR = XML::parseFloat(node, "paddingR");
			paddingChanged = true;
		}
        
        if (XML::attrExists(node, "paddingT"))
		{
            padT = XML::parseFloat(node, "paddingT");
			paddingChanged = true;
		}
        
        if (XML::attrExists(node, "paddingB"))
		{
            padB = XML::parseFloat(node, "paddingB");
			paddingChanged = true;
		}
		
		if (XML::attrExists(node, "paddingV"))
		{
			padT = XML::parseFloat(node, "paddingV");
			padB = XML::parseFloat(node, "paddingV");
			paddingChanged = true;
		}
		
		if (XML::attrExists(node, "paddingH"))
		{
			padL = XML::parseFloat(node, "paddingH");
			padR = XML::parseFloat(node, "paddingH");
			paddingChanged = true;
		}
		
		if (paddingChanged)
			widget->setPadding( padL, padR, padT, padB );
		
		// rotation (in degrees)
		if(XML::attrExists(node, "angleDegrees"))
		{
			float angle = XML::parseFloat(node, "angleDegrees");
			widget->setRotation(angle);
		}
		
		if(XML::attrExists(node, "bgBlendColorRGBA"))
		{
			Color color = XML::parseColor8bitRGBA(node, "bgBlendColorRGBA");
			widget->setBGColorBlend(color);
		}
		
		// Tiling the texture
		if(XML::attrExists(node, "numTiles"))
		{
			Vector2 tile = XML::parseVector2(node, "numTiles");
			
			// Force an aspect for the tile
			if(XML::attrExists(node, "tileForceAspect"))
			{
				// check if there is a "forceAspect" property, which will override the HEIGHT setting
				// from above, and instead set a specific aspect ratio.
				float aspectRatio = XML::parseAspectRatio(node, "tileForceAspect");
				widget->setTile(tile, aspectRatio);
			}
			else
			{
				widget->setTile(tile);
			}
			
			// Set the original offset of the tile
			if (XML::attrExists(node, "tileOffset"))
			{
				Vector2 tileOffset = XML::parseVector2(node, "tileOffset");
				widget->setTileOffset( tileOffset );
			}
		}
		
		Color textColor = Color::Black;
		if(XML::attrExists(node,"textColorRGB"))
		{
			textColor = XML::parseColor8bit(node, "textColorRGB");
			widget->setTextDisplayColor(textColor);
		}
		
		
		// text scale defaults to one or is scaled according to size on construction
		// so for this tag, we only want to set the scale only if the tag is specified
		float textScale = _parseFloat(node, "textScale");
		if(textScale != 0.0f)
		{
			widget->setTextScale(textScale);
		}
	}
    
    // --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    void WidgetHelper::_updateWidgetCanvas(xmlNodePtr node, Widget_Canvas* widget, Widget* parent)
    {
		// do post constructor operations here
		if (XML::attrExists(node, "tapDuration"))
		{
			float dur = XML::parseFloat(node, "tapDuration");
			
			widget->setTapThresholds( dur, widget->getTapDistance() );
		}
		
		if (XML::attrExists(node, "tapDistance"))
		{
			float dis = XML::parseFloat(node, "tapDistance");
			
			widget->setTapThresholds(widget->getTapDuration(), dis);
		}
    }
    
    
    // --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    void WidgetHelper::_updateWidgetSlideWheel(xmlNodePtr node, Widget_SlideWheel* widget, Widget* parent)
    {
		//get textures
        std::string textureName = _parseString(node, "texName");
		TexturePtr tex = _getTexture(textureName);
		widget->setTexture( tex );
        
		if (XML::attrExists(node, "uv_scale"))
			widget->setUVScale( XML::parseVector2(node, "uv_scale") );
		
		// do post constructor operations here
		if (XML::attrExists(node, "warp_uv"))
			widget->setUVWarping( XML::parseBool(node, "warp_uv") );
		
		float padL = 0.0f;
		float padR = 0.0f;
		float padT = 0.0f;
		float padB = 0.0f;
		
		if (XML::attrExists(node, "paddingL"))
			padL = XML::parseFloat(node, "paddingL");
		
		if (XML::attrExists(node, "paddingR"))
			padR = XML::parseFloat(node, "paddingR");

		if (XML::attrExists(node, "paddingT"))
			padT = XML::parseFloat(node, "paddingT");
		
		if (XML::attrExists(node, "paddingB"))
			padB = XML::parseFloat(node, "paddingB");
		
		widget->setPadding( padL, padR, padT, padB );
		
    }
    
	
	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    void WidgetHelper::_updateWidgetProgressBar(xmlNodePtr node, Widget_ProgressBar* widget, Widget* parent)
	{
		// textures
		std::vector<std::string> textures;
		_getTextureNames(node, textures);
		
		unsigned int numTexs = textures.size();
		TexturePtr bgTex;
		if(numTexs > 0)
		{
			bgTex = _getTexture(textures[0]);
		}

		if (bgTex)
			widget->setTexture(Widget_ProgressBar::TextureName_BG, bgTex );
		
		TexturePtr fillTex;
		if(numTexs > 1)
		{
			fillTex = _getTexture(textures[1]);
		}
		
		if (fillTex)
			widget->setTexture(Widget_ProgressBar::TextureName_Fill, fillTex );
		
		//-------------------------------------- 
		// padding!!!
		float fillPadding = 10.0f;
		float fillPaddingLRTB[4];
		// clear for initialization
		memset(fillPaddingLRTB, 0, sizeof(fillPaddingLRTB));
		if(XML::attrExists(node, "fillPadding"))
		{
			fillPadding = XML::parseFloat(node,"fillPadding");
			fillPaddingLRTB[0] = fillPadding;
			fillPaddingLRTB[1] = fillPadding;
			fillPaddingLRTB[2] = fillPadding;
			fillPaddingLRTB[3] = fillPadding;
		}
		else if(XML::attrExists(node, "fillPaddingL"))
		{
			fillPaddingLRTB[0] = XML::parseFloat(node,"fillPaddingL");
			
			if(XML::attrExists(node, "fillPaddingR"))
			{
				fillPaddingLRTB[1] = XML::parseFloat(node,"fillPaddingR");
			}
			else 
			{
				printf("ERROR: To use separate padding for each side, you must assign padding to every side");
				assert(0);
			}
			
			if(XML::attrExists(node, "fillPaddingT"))
			{
				fillPaddingLRTB[2] = XML::parseFloat(node,"fillPaddingT");
			}
			else 
			{
				printf("ERROR: To use separate padding for each side, you must assign padding to every side");
				assert(0);
			}
			
			if(XML::attrExists(node, "fillPaddingB"))
			{
				fillPaddingLRTB[3] = XML::parseFloat(node,"fillPaddingB");
			}
			else 
			{
				printf("ERROR: To use separate padding for each side, you must assign padding to every side");
				assert(0);
			}
			
		}
		widget->setPadding( fillPaddingLRTB[0], fillPaddingLRTB[1], fillPaddingLRTB[2], fillPaddingLRTB[3] );
		
		//--------------------------------------
		// fill mode
		Widget_ProgressBar::FillMode fm = Widget_ProgressBar::FM_CLAMP;
		std::string fillMode = _parseString(node, "fillMode");
		if(fillMode == "stretch")
		{
			fm = Widget_ProgressBar::FM_STRETCH;
		}
		else if (fillMode == "repeat")
		{
			fm = Widget_ProgressBar::FM_REPEAT;
		}
		widget->setFillMode( fm );
		
		float repeatStep = .1f;
		if(XML::attrExists(node, "repeatStep"))
		{
			repeatStep = XML::parseFloat(node, "repeatStep");
		}
		widget->setRepeatStep(repeatStep);
	}

	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    void WidgetHelper::_updateWidgetMovingTextBox(xmlNodePtr node, Widget_MovingTextBox* widget, Widget* parent)
	{
		// do post constructor operations here
		
		// get textures
		std::string textureBG = _parseString(node, "texBGName");
		TexturePtr texBG = _getTexture(textureBG);
		//widget->setBGTexture( texBG );
		widget->setTexture(Widget_MovingTextBox::TextureName_BG, texBG);
		
		// get font		
		std::string fontStr = _parseString(node, "font");
		if(fontStr.size())
		{			
			fontStr = "normal";
		}
		widget->setFont( fontStr );
		
		// direction
		std::string direction = _parseString(node, "textMoveDirection");
		Widget_MovingTextBox::MovementDirection dir = Widget_MovingTextBox::MD_Default;
		if(direction.size())
		{
			if(StringHelper::toLower(direction) == "right")
			{
				dir = Widget_MovingTextBox::MD_Right;
			}
			else if(StringHelper::toLower(direction) == "up")
			{
				dir = Widget_MovingTextBox::MD_Up;
			}
			else if(StringHelper::toLower(direction) == "down")
			{
				dir = Widget_MovingTextBox::MD_Down;
			}
			else
			{
				dir = Widget_MovingTextBox::MD_Left;
			}
		}
		widget->setMovementDirection( dir );
		
		//-------------------------------------- 
		// padding!!!
		float fillPadding = 0.0f;
		float fillPaddingLRTB[4];
		// clear for initialization
		memset(fillPaddingLRTB, 0, sizeof(fillPaddingLRTB));
		if(XML::attrExists(node, "textPadding"))
		{
			fillPadding = XML::parseFloat(node,"textPadding");
			fillPaddingLRTB[0] = fillPadding;
			fillPaddingLRTB[1] = fillPadding;
			fillPaddingLRTB[2] = fillPadding;
			fillPaddingLRTB[3] = fillPadding;
		}
		else if(XML::attrExists(node, "textPaddingL"))
		{
			fillPaddingLRTB[0] = XML::parseFloat(node,"textPaddingL");
			
			if(XML::attrExists(node, "textPaddingR"))
			{
				fillPaddingLRTB[1] = XML::parseFloat(node,"textPaddingR");
			}
			else 
			{
				printf("ERROR: To use separate padding for each side, you must assign padding to every side");
				assert(0);
			}
			
			if(XML::attrExists(node, "textPaddingT"))
			{
				fillPaddingLRTB[2] = XML::parseFloat(node,"textPaddingT");
			}
			else 
			{
				printf("ERROR: To use separate padding for each side, you must assign padding to every side");
				assert(0);
			}
			
			if(XML::attrExists(node, "textPaddingB"))
			{
				fillPaddingLRTB[3] = XML::parseFloat(node,"textPaddingB");
			}
			else 
			{
				printf("ERROR: To use separate padding for each side, you must assign padding to every side");
				assert(0);
			}
			
		}
		widget->setPadding( fillPaddingLRTB[0], fillPaddingLRTB[1], fillPaddingLRTB[2], fillPaddingLRTB[3] );
		
		// speed
		float speed = _parseFloat(node, "speed");
		if(abs(speed) >= 1e-6)
		{
			widget->setSpeed( speed );
		}
		
		Color textColor = Color::Black;
		if(XML::attrExists(node,"textColorRGB"))
		{
			textColor = XML::parseColor8bit(node, "textColorRGB");
		}
		widget->setTextDisplayColor(textColor);
		
		if(XML::attrExists(node,"bgBlendColorRGB"))
		{
			textColor = XML::parseColor8bit(node, "bgBlendColorRGB");
			widget->setBGColorBlend(textColor);
		}
		
		// text scale defaults to one or is scaled according to size on construction
		// so for this tag, we only want to set the scale only if the tag is specified
		float textScale = _parseFloat(node, "textScale");
		if(textScale != 0.0f)
		{
			widget->setTextScale(textScale);
		}			
	}
    
    
    // --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    void WidgetHelper::_updateWidgetFingerCatcher(xmlNodePtr node, Widget_FingerCatcher* widget, Widget* parent)
    {
		// do post constructor operations here
		
    }
	
	
	
	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    void WidgetHelper::_updateWidgetGroup(xmlNodePtr node, Widget_Group* widget, Widget* parent)
    {
		
    }
	
	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    void WidgetHelper::_updateWidgetScrollableSet(xmlNodePtr node, Widget_ScrollableSet* widget, Widget* parent)
    {
		
    }
    
	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    void WidgetHelper::_updateWidgetScrollableCamera(xmlNodePtr node, Widget_ScrollableCamera* widget, Widget* parent)
    {
        
    }

	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    void WidgetHelper::_updateWidgetScoreCounter(xmlNodePtr node, Widget_ScoreCounter* widget, 
                                                 Widget* parent)
    {
        
    }

	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    void WidgetHelper::_updateWidgetAnimation(xmlNodePtr node, Widget_Animation* widget, 
                                                 Widget* parent)
    {
		if( XML::attrExists(node,"idleAnimationGroup") )
        {
            // Animation to play, should be looping, can be a group, when it loops it will try to play
            // from this group
			std::string idleAnimation = _parseString(node, "idleAnimationGroup");
			widget->setIdleAnimationOrGroup( idleAnimation );
        }

        if ( XML::attrExists(node,"tappedAnimationGroup") )
        {
            // Animation to play when the user taps the widget, can be a group
			std::string tappedAnimation = _parseString(node, "tappedAnimationGroup");
			widget->setTappedAnimationOrGroup( tappedAnimation );
		}

		if( XML::attrExists(node,"skeleton") && XML::attrExists(node, "animationFile") )
		{
            // The skeleton for the animation
			std::string skeletonFile = _parseString(node, "skeleton");
            
            // File containing animation
            std::string animationFile = _parseString(node, "animationFile");

            // File containing animation
            std::string scenesToLoad = "";
            if ( XML::attrExists(node,"scenesToLoad") )
            {
                scenesToLoad = _parseString(node, "scenesToLoad");
            }

            // File containing animation
            std::string groupsToLoad = "";

            if ( XML::attrExists(node,"groupsToLoad") )
            {
                groupsToLoad = _parseString(node, "groupsToLoad");
            }

            std::string sceneName = "";
            if ( XML::attrExists(node,"tappedAnimationGroup") )
            {
                sceneName = _parseString(node, "scene");
            }
			widget->loadSkeletonActor( skeletonFile, animationFile, sceneName, scenesToLoad, groupsToLoad );
        }
    }
	
	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    void WidgetHelper::_updateWidgetScrollableGroup(xmlNodePtr node, Widget_ScrollableGroup* widget, Widget* parent)
    {
		
    }
	
	
	
	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    void WidgetHelper::_getTextureNames(xmlNodePtr node, std::vector<std::string>& textureList)
	{
		// add icons if any defined
		int i = 0;
		char textureAttr[64];
		std::string textureName;
		bool go = false;
		do 
		{
			snprintf(textureAttr, 64, "texName%d", i);
			textureName = _parseString(node, textureAttr);
			if(textureName.size())
			{
				textureList.push_back(textureName);
				go=true;
			}
			else 
			{
				go=false;
			}
			++i;
			
		} while (go);
	}
	
	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    int WidgetHelper::_extractPriority(xmlNodePtr node)
	{
		int priority = _parseInt(node, "priority");
		
		if(priority < 0)
		{
			priority = 0;
		}
		
		return priority;		
	}
	
	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    int WidgetHelper::_extractLayer(xmlNodePtr node)
	{
		int layer = _parseInt(node, "layer");
		
		if(layer < 0)
		{
			layer = 1;
		}
		
		return layer;
	}
	
	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    Widget* WidgetHelper::_extractParent(xmlNodePtr node, WidgetManager* const widgetMgr)
	{
		int parent = _parseInt(node, "parent");
		Widget* parentWidget = NULL;
		if(parent >= 0 && widgetMgr != NULL)
		{
			parentWidget = widgetMgr->getWidget(parent);
			if(parentWidget == NULL)
			{
				printf("Parent with id [%d] has not yet been loaded. Place parent entry above child entry!", parent);
			}
		}
		
		return parentWidget;		
	}
	
	
	
	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    Vector2 WidgetHelper::_parsePositionToScreen(xmlNodePtr node, Widget* parent, Vector2 size)
	{
		Vector2 position;
		
		if(parent)
		{
			position = parent->getLocalPosition2D();
			
			// For identically sized elements
			bool parYOff = _parseBool(node, "parentSizeOffsetY");
			bool parXOff = _parseBool(node, "parentSizeOffsetX");
			
			if (parYOff && parXOff) 
			{
				position += parent->getSize();
			}
			else if (parYOff)
			{
				position += Vector2(0, parent->getSize().Y);
			}
			else if (parXOff)
			{
				position += Vector2(parent->getSize().X, 0);
			}
			
			// For differently size elements
			bool lineUpY = _parseBool(node, "parentLineUpY");
			bool lineUpX = _parseBool(node, "parentLineUpX");
			
			if (lineUpY && lineUpX) 
			{
				position += (parent->getSize() + size) / 2.0f;
			}
			else if (lineUpY)
			{
				position += Vector2(0, (parent->getSize() + size).Y / 2.0f);
			}
			else if (lineUpX)
			{
				position += Vector2((parent->getSize() + size).X / 2.0f, 0);
			}
			
			// Or, we want to do something crazy! - i.e. position based upon parent size
			if(XML::attrExists(node,"posByParentSize"))
			{
				Vector2 parentPosition = XML::parseVector2(node, "posByParentSize");
				position = parent->getWorldPosition2D() + (parent->getSize() * parentPosition) - (parent->getSize() / 2.0f);
			}
			// Or justify using the parent's size
			else if (XML::attrExists(node, "justifyWidgetByParent"))
			{
				// Stored as a Vector2
				Vector2 justification = XML::parseVector2(node, "justifyWidgetByParent");
				
				Vector2 newJust = justification - Vector2(0.5f, 0.5f);
				
				newJust.X *= -1.0f;
				newJust.Y *= -1.0f;
				
				Vector2 parentSize = parent->getSize();
				Vector2 parentUpperLeft = parent->getWorldPosition2D() - parentSize / 2.0f;
				
				position = parentUpperLeft + justification * parentSize + newJust * size;
			}
		}			
		
		// Or, we want to 'stick' the object to a side of the screen
		if (XML::attrExists(node,"justifyWidget"))
		{
			// Stored as a Vector2
			Vector2 justification = XML::parseVector2(node, "justifyWidget");
			
			Vector2 newJust = justification - Vector2(0.5f, 0.5f);
			
			newJust.X *= -1.0f;
			newJust.Y *= -1.0f;
			
			position = ScreenCoord( justification ).toScreen() + size * newJust;
		}
		
		position += _parseScreenCoordToScreen(node,"pos");	
		
		return position;
	}
	
	
	
	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    Vector2 WidgetHelper::_parseSizeToScreen( xmlNodePtr node, Widget* w )
	{
		// look for "real world" size first...
		Vector2 size = _parseRealWorldToScreen(node,"sizeMM");
		
		if(size == Vector2::Zero)
		{
			size = _parseScreenCoordToScreen(node,"size");
		
			// check if there is a "forceAspect" property, which will override the HEIGHT setting
			// from above, and instead set a specific aspect ratio.
			if (XML::attrExists(node, "forceAspect") || XML::attrExists(node, "keepAspect"))
			{
				float aspectRatio = 1.0f;
				
				if (XML::attrExists(node, "forceAspect"))
				{
					aspectRatio = XML::parseAspectRatio(node, "forceAspect");
				}
				
				bool keepAspect = _parseBool(node, "keepAspect");
				if (keepAspect && (w != NULL))
				{
					Vector2 s = w->getBaseSize();
					
					aspectRatio = s.X / s.Y;
				}
				
				
				bool useWidth = true;
				
				if (XML::attrExists(node, "aspectBasis"))
				{
					std::string basis = StringHelper::toLower( XML::parseString(node, "aspectBasis") );
					if ((basis == "height") || (basis == "y"))
						useWidth = false;
				}
				
				if (useWidth)
					size.Y = size.X / aspectRatio;
				else
					size.X = size.Y * aspectRatio;
			}
		}
		
		return size;
	}
	
	
	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    Vector2 WidgetHelper::_parseScreenCoordToScreen(xmlNodePtr node, const char* attr)
	{
		ScreenCoord coord(Vector2::Zero,Vector2::Zero);
		if(XML::attrExists(node, attr))
		{		   
			coord = XML::parseScreenCoord(node, attr);
		}
#if _DEBUG
		else 
		{
			printf("Attr '%s' not found!\n", attr);
		}
#endif
	
		return coord.toScreen();
	}
	
	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    Vector2 WidgetHelper::_parseRealWorldToScreen(xmlNodePtr node, const char* attr)
	{
		Vector2 screen(Vector2::Zero);
		if(XML::attrExists(node, attr))
		{		   
			screen = ScreenCoord::screenSizeFromRealWorldSize(XML::parseVector2(node, attr));
		}
#if _DEBUG
		else 
		{
			printf("Attr '%s' not found!\n", attr);
		}
#endif
			
		return screen;
	}
	
	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    std::string WidgetHelper::_parseString(xmlNodePtr node, const char* attr)
	{
		std::string ret;
		if(XML::attrExists(node, attr))
		{
			ret = XML::parseString(node, attr);
		}
#if _DEBUG
		else 
		{
			printf("Attr '%s' not found!\n", attr);
		}
#endif
		
		return ret;
	}
	
	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    bool WidgetHelper::_parseBool(xmlNodePtr node, const char* attr)
	{
		bool ret = false;
		if(XML::attrExists(node, attr))
		{
			ret = XML::parseBool(node, attr);
		}
#if _DEBUG
		else 
		{
			printf("Attr '%s' not found!\n", attr);
		}
#endif
		
		return ret;
	}
	
	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    float WidgetHelper::_parseFloat(xmlNodePtr node, const char* attr)
	{
		float ret = 0.0f;
		if(XML::attrExists(node, attr))
		{
			ret = XML::parseFloat(node, attr);
		}
#if _DEBUG
		else 
		{
			printf("Attr '%s' not found!\n", attr);
		}
#endif
		
		return ret;
	}
	
	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    int WidgetHelper::_parseInt(xmlNodePtr node, const char* attr)
	{
		int ret = -1;
		if(XML::attrExists(node, attr))
		{
			ret = XML::parseInt(node, attr);
		}
#if _DEBUG
		else 
		{
			printf("Attr '%s' not found!\n", attr);
		}
#endif
		
		return ret;
	}
		
	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    Color WidgetHelper::_parseColor(xmlNodePtr node, const char* attr)
	{
		Color ret = Color::Grey;
		if(XML::attrExists(node, attr))
		{
			ret = XML::parseColor8bit(node, attr);
		}
#if _DEBUG
		else 
		{
			printf("Attr '%s' not found!\n", attr);
		}
#endif
		
		return ret;
	}																 
	
	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    TexturePtr WidgetHelper::_getTexture(const std::string& name)
	{
		TexturePtr ret;
		
		if(name.size())
		{
			ret = TextureManager::getManager().getTexture(texturePath + name);
		}
		
		return ret;
	}
		
	// --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    void WidgetHelper::_recurseFileReadCallback(void* fileParams) 
	{
		FileManager::ReadFileCallbackParameters* rcp = (FileManager::ReadFileCallbackParameters*)fileParams;
		
		mFilesToRead.push_back(rcp->path);
		std::string currentFile = rcp->path;
        
		std::string recurseFile = "";
		
		if(rcp->result == ResultCode_OK)
		{
			// parse the XML document.
			xmlDocPtr currentDoc_ = NULL;
			xmlNodePtr root_element = XML::loadDocMemory(rcp->buffer, rcp->length, "Widgets", currentDoc_ );
			
			if(root_element)
			{ 
				// If there's a base file, we need to add it to the stack 
				if (XML::attrExists(root_element, "baseLayoutFile"))
				{
					recurseFile = _parseString(root_element, "baseLayoutFile");
				}
			}
			
			// Cleanup
			XML::cleanupDoc(currentDoc_);
            delete [] rcp->buffer;
		}
		
		// Finally, see if we need to go deeper into the rabbit hole
		if (recurseFile != "")
		{
			PropertyList plist;
			FileManager::getInstancePtr()->readFile(recurseFile, CallbackPtr( new Callback(&WidgetHelper::_recurseFileReadCallback) ), plist, FileManager::PP_NoAbstraction);
		}
		else
		{
			PropertyList plist;
			std::string filename = mFilesToRead[mFilesToRead.size() - 1];
			mFilesToRead.erase(mFilesToRead.end());
			FileManager::getInstancePtr()->readFile(filename, CallbackPtr( new Callback(&WidgetHelper::_fileReadCallback) ), plist, FileManager::PP_NoAbstraction);
		}
	}
	
    // --------------------------------------------------##--------------------------------------------------##
    // --------------------------------------------------##--------------------------------------------------##
    void WidgetHelper::_fileReadCallback(void* fileParams) //const std::string& path, char* buffer, size_t length, const std::string& actualPath, const std::string& handlerName, const PropertyList& plist)
    {		
		FileManager::ReadFileCallbackParameters* rcp = (FileManager::ReadFileCallbackParameters*)fileParams;
		
        ResultCode ret = ResultCode_OK;
		
        LoadPassThroughParams* params;
        WidgetManager* widgetMgr;
        
        std::string strippedPath = PlatformManager::getInstancePtr()->stripPlatformSpecificFilename(rcp->path);
		
		if (!mFilesToRead.empty())
		{
			strippedPath = PlatformManager::getInstancePtr()->stripPlatformSpecificFilename(mFilesToRead[0]);
		}
		
        FileParamMap::iterator it = mCallbackMap.find(strippedPath);
        if (it == mCallbackMap.end())
        {
            ret = ResultCode_Error;
        }
        else
        {
            params = &it->second;
            widgetMgr = params->mWidgetMgr;
        }
        
        if(ret == ResultCode_OK)
        {
            if(rcp->result == ResultCode_OK)
            {
                // parse the XML document.
                xmlDocPtr currentDoc_ = NULL;
                xmlNodePtr root_element = XML::loadDocMemory(rcp->buffer, rcp->length, "Widgets", currentDoc_ );
                
                if(root_element)
                { 
                    
                    setTexturePath(_parseString(root_element, "texturePath"));
                    
					Walaber::Logger::printf("Walaber", Logger::SV_DEBUG, "root element: %s\n", root_element->name);
                    xmlNodePtr widget = root_element->children;
					
					
					_createAndUpdateWidgetPrototypes(root_element);
                    
					Walaber::Logger::printf("Walaber", Logger::SV_DEBUG, "WidgetHelper loadWidgetsXML - parsing widgets\n");
                    
                    while (widget)
                    {
						Walaber::Logger::printf("Walaber", Logger::SV_DEBUG, "node name: %s\n", widget->name);

                        if (xmlStrcmp(widget->name, (const xmlChar*) "Widget") == 0)
                        {
							Walaber::Logger::printf("Walaber", Logger::SV_DEBUG, "WidgetHelper loadWidgetsXML - found widget");

                            std::string widgetTypeStr = XML::parseString(widget, "type");
                            WidgetType widgetType;
                            Widget::getWidgetTypeFromString(widgetTypeStr, widgetType);
                            
							Widget* w = _createBaseWidget(widget, _extractParent(widget,widgetMgr), widgetType, widgetMgr);
							int p = _extractPriority(widget);
							
							if (w && widgetMgr != NULL)
								widgetMgr->addWidget(w,p);	
							                            
                        }
                        widget = widget->next;
                    }
                }
                else
                {
                    ret = ResultCode_ErrorXMLParseError;
                }
                
                XML::cleanupDoc(currentDoc_);
            }
			else
			{
				ret = ResultCode_ErrorFileNotFound;
			}
        }
        
		
        if(rcp->buffer)
        {
            delete [] rcp->buffer;
        }
		
		
		// We're out of files to read, let the screen know we're done
		if (mFilesToRead.empty())
		{
			if(params->mCallback)
			{
				FinishedLoadingWidgetsParameters callback_params(ret);
				params->mCallback->invoke(&callback_params);
			}
			mCallbackMap.erase(it);
			_clearPrototypes();
			
		}
		// Otherwise keep looking for files to load
		else
		{
			PropertyList plist;
			std::string filename = mFilesToRead[mFilesToRead.size() - 1];
			mFilesToRead.erase(mFilesToRead.end());
			FileManager::getInstancePtr()->readFile(filename, CallbackPtr( new Callback(&WidgetHelper::_fileReadCallback) ), plist, FileManager::PP_NoAbstraction);
		}
    }
}
