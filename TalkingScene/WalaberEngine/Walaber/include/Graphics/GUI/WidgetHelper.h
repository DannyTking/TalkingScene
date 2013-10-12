/*
 *  WidgetHelper.h
 *  JellyCar3
 *
 *  Created by jpecho on 11/29/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _WIDGET_HELPER_H_
#define _WIDGET_HELPER_H_

#include "GUI.h"
#include "GUIWidgets.h"

#include <XMLHelper.h>
#include <XML.h>

#include <map>
#include <string>
#include <vector>
#include <Color.h>

namespace Walaber
{	
	class WidgetHelper
	{
	public:
		struct FinishedLoadingWidgetsParameters
		{
			FinishedLoadingWidgetsParameters( ResultCode r ) : result(r) { }
			
			ResultCode	result;
		};
		        
        struct LoadPassThroughParams
        {
            WidgetManager*	mWidgetMgr;
            CallbackPtr		mCallback;
        };
		
        typedef std::map<std::string, LoadPassThroughParams> FileParamMap;
		typedef std::vector<std::string> StringVector;
		typedef std::map<std::string, Widget*>	WidgetPrototypeMap;
        
		static bool getTextureNames(const std::string& filename, std::vector<std::string>& textureList);
		
        static void loadWidgetsXML(const std::string& filename, WidgetManager* const widgetMgr, CallbackPtr callback);
                
		static void setTexturePath(const std::string& texPath) { texturePath = texPath; }
		
	protected:	
		
		static std::string textureTags[]; 	
		static int numTextureTags;
		
		static std::string texturePath;
		static xmlDocPtr currentDoc;
		
		static xmlNodePtr _openXMLFile(const std::string& filename);
		static void	_closeXMLFile();
		
		static bool _getTextureName(xmlNodePtr node, std::string& texName);
		static void _getTextureNames(xmlNodePtr node, std::vector<std::string>& textureList);
		
		static void _createAndUpdateWidgetPrototypes(xmlNodePtr root_node);
		
		// pass NULL to WidgetManager to NOT check for other widgets. (used for widget prototypes)
		static Widget* _createBaseWidget(xmlNodePtr node, Widget* parent, int widgetType, WidgetManager* widgetMgr);
		static Widget* _createAndUpdateWidget(xmlNodePtr node, Widget* parent, int widgetType, int widgetID, Vector2 pos, Vector2 size, Widget* theWidget = NULL );
		
		static void _updateBaseWidget(xmlNodePtr node, Widget* widget);
		
		static void _updateWidgetClassicControls(xmlNodePtr node, Widget_ClassicControls* widget, Widget* parent);
		static void _updateWidgetSpinner(xmlNodePtr node, Widget_Spinner* widget, Widget* parent);
		static void _updateWidgetPushButton(xmlNodePtr node, Widget_PushButton* widget, Widget* parent);
		static void _updateWidgetTimedButton(xmlNodePtr node, Widget_TimedButton* widget, Widget* parent);
		static void _updateWidgetColorPicker(xmlNodePtr node, Widget_ColorPicker* widget, Widget* parent);
		static void _updateWidgetIconList(xmlNodePtr node, Widget_IconList* widget, Widget* parent);
		static void _updateWidgetToggle(xmlNodePtr node, Widget_Toggle* widget, Widget* parent);
		static void _updateWidgetSlider(xmlNodePtr node, Widget_Slider* widget, Widget* parent);
		static void _updateWidgetLabel(xmlNodePtr node, Widget_Label* widget, Widget* parent);
        static void _updateWidgetCanvas(xmlNodePtr node, Widget_Canvas* widget, Widget* parent);
        static void _updateWidgetSlideWheel(xmlNodePtr node, Widget_SlideWheel* widget, Widget* parent);
		static void _updateWidgetProgressBar(xmlNodePtr node, Widget_ProgressBar* widget, Widget* parent);
		static void _updateWidgetMovingTextBox(xmlNodePtr node, Widget_MovingTextBox* widget, Widget* parent);
		static void _updateWidgetFingerCatcher(xmlNodePtr node, Widget_FingerCatcher* widget, Widget* parent);
		static void _updateWidgetGroup(xmlNodePtr node, Widget_Group* widget, Widget* parent);
		static void _updateWidgetScrollableSet(xmlNodePtr node, Widget_ScrollableSet* widget, Widget* parent);
		static void _updateWidgetScrollableGroup(xmlNodePtr node, Widget_ScrollableGroup* widget, Widget* parent);
        static void _updateWidgetScoreCounter(xmlNodePtr node, Widget_ScoreCounter* widget, Widget* parent);
		static void _updateWidgetScrollableCamera(xmlNodePtr node, Widget_ScrollableCamera* widget, Widget* parent);
        static void _updateWidgetAnimation(xmlNodePtr node, Widget_Animation* widget, Widget* parent);
        
		static int _extractPriority(xmlNodePtr node);
		static int _extractLayer(xmlNodePtr node);
		static Widget* _extractParent(xmlNodePtr node, WidgetManager* const widgetMgr);
		
		//parsing helpers
		static Vector2 _parsePositionToScreen(xmlNodePtr node, Widget* parent, Vector2 size);
		static Vector2 _parseSizeToScreen( xmlNodePtr node, Widget* w = NULL );
		
		static Vector2 _parseScreenCoordToScreen(xmlNodePtr node, const char* attr);
		static Vector2 _parseRealWorldToScreen(xmlNodePtr node, const char* attr);
		
		static std::string _parseString(xmlNodePtr node, const char* attr);
		static bool _parseBool(xmlNodePtr node, const char* attr);
		static float _parseFloat(xmlNodePtr node, const char* attr);
		static int _parseInt(xmlNodePtr node, const char* attr);
		static Color _parseColor(xmlNodePtr node, const char* attr);
		
        // file read callback
		static void _recurseFileReadCallback(void* params);
        static void _fileReadCallback(void* params); //const std::string& path, char* buffer, size_t length, const std::string& actualPath, const std::string& handlerName, const PropertyList& plist);
        
		static TexturePtr _getTexture(const std::string& name);
		
		static void _clearPrototypes();
		static void _copyFromPrototype( xmlNodePtr node, Widget* newWidget, Widget* prototype, Widget* parent, int widgetType );
        
        static FileParamMap  mCallbackMap;   
		
		static StringVector mFilesToRead;
		
		static WidgetPrototypeMap mPrototypes;
	};
}

#endif

