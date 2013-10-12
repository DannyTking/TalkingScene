#ifndef _WALABER_GUI_H_
#define _WALABER_GUI_H_

#include <map>
#include <vector>


#include "Vector2.h"
#include "AABB.h"

#include "DrawableNode.h"
#include "SceneGraph.h"
#include "StringHelper.h"
#include "Texture.h"
#include "FontManager.h"
#include "Camera.h"

namespace Walaber
{
	class Widget;
	class WidgetManager;
	class SpriteBatch;
	class BitmapFont;

	struct TextLineInfo
	{
		std::string mText;
		Vector2 mSize;
		
		TextLineInfo(std::string text, Vector2 size):
		mText(text),
		mSize(size)
		{}
	};
	
	enum WidgetType
	{
		WT_CLASSIC_CONTROLS,
		WT_PUSH_BUTTON,
		WT_TIMED_BUTTON,
		WT_SPINNER,
		WT_COLOR_PICKER,
		WT_ICON_LIST,
		WT_CANVAS,
		WT_MANIPULATOR,
		WT_TOGGLE,
		WT_SLIDER,
		WT_LABEL,
		WT_PROGRESS_BAR,
		WT_MOVING_TEXT_BOX,
        WT_SLIDEWHEEL,
		WT_SCROLLABLE_SET,
		WT_SCROLLABLE_GROUP,
		WT_GROUP,
        WT_FINGER_CATCHER,
        WT_SCORE_COUNTER,
        WT_SCROLLABLE_CAMERA,
        WT_ANIMATION
	};
	typedef std::map<std::string, WidgetType> WidgetTypeMap;
	
	enum WidgetOrientation
	{
		WO_PortraitDown,
		WO_PortraitUp,
		WO_LandscapeDown,
		WO_LandscapeUp
	};
	WidgetOrientation stringToWidgetOrientation( const std::string& s );
	std::string widgetOrientationToString( const WidgetOrientation& wo );
	

	// describes a single finger on the screen, and keeps track of which widget it's currently focused on.
	enum FingerState
	{
		FS_NEW,			// new touch.
		FS_MOVED,		// existing touch moved.
		FS_LOST,		// touch lost.
		FS_SUSTAINED,	// touch still here, but did not move.
	};

	struct FingerInfo
	{
		FingerState state;
		Vector2		curPos;
		Vector2		lastPos;
		Widget*		focusWidget;
	};
	typedef std::map<int, FingerInfo*>	FingerInfoMap;


	enum SnapType
	{
		ST_EDGE,
		ST_CENTER
	};

	static const int WIDGET_SNAP_PADDING = 4;
	static const int WIDGET_SNAP_AMOUNT = 12;

	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	// Widget base class
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	// simple classes for basic UI elements (buttons, etc).
	class Widget : public DrawableNode
	{
	public:
		// typedefs
		typedef		std::vector<TexturePtr>		TextureList;
		
		// ---------------------------------------------
		Widget( int widgetName, unsigned int widgetType, Vector2 pos, Vector2 size, WidgetOrientation o, int layer = 1 );
		virtual ~Widget() {}
		
		// ---------------------------------------------
		// get widget enum from string
		// ---------------------------------------------
		static WidgetTypeMap  widgetEnumMap;
		static bool getWidgetTypeFromString(std::string type, WidgetType& widgetTypeOut);
		
		// ---------------------------------------------
		// widget name
		// ---------------------------------------------
		std::string getWidgetName() { return mName; }
		int getWidgetNameAsInt() { return StringHelper::strToInt(mName); }
		
		// ---------------------------------------------
		// widget layer
		// ---------------------------------------------
		void setLayer(const int layer);
		
		// ---------------------------------------------
		// widget type
		// ---------------------------------------------
		unsigned int getWidgetType() { return mWidgetType; }
		
		// ---------------------------------------------
		// widget orientation
		// ---------------------------------------------
		void setOrientation( WidgetOrientation wo ) { mOrientation = wo; _updateAABB(); }
		WidgetOrientation getOrientation() { return mOrientation; }
		
		// ---------------------------------------------
		// widget visibility
		// ---------------------------------------------
		void setVisible( bool visible ) { mVisible = visible; }
		bool isVisible() { return mVisible; }
        
        // ---------------------------------------------
		// widget transparency
		// ---------------------------------------------
        void setTransparency( float transparency ) {mTransparency = transparency; }
        float getTransparency() { return mTransparency; }
		
		// ---------------------------------------------
		// position and sizing common to all widgets.
		// ---------------------------------------------
		virtual void setLocalPosition2D(const Vector2& pos);
        virtual void applyPositionOffset2D(const Vector2& pos);
		virtual void setLocalScale2D(const Vector2& scale);
		
		// ---------------------------------------------
		// Backwards compatibility (GUI works in rectangles, Nodes in scale)
		virtual void setBaseSize( Vector2 size );
		Vector2 getBaseSize() { return mBaseSize; }
		Vector2 getSize() { return getWorldScale2D() * getBaseSize(); }
		
		// ---------------------------------------------
		// Backwards compatibility (GUI works in degrees, Nodes work in radians)
		virtual void setRotation( const float rotDegrees ) { setLocalAngleZ( degToRad(rotDegrees) ); }
		float getRotation() { return radToDeg( getWorldAngleZ() ); }
		
		AABB getAABB() 
		{
			_updateAABB();
			return mAABB; 
		}
		
		// ---------------------------------------------
		void setSnapType( SnapType t ) { mSnapType = t; }
		SnapType getSnapType() { return mSnapType; }
		
		// ---------------------------------------------
		void setAllowCustomize( bool v ) { mAllowCustomize = v; }
		bool allowCustomize() { return mAllowCustomize; }
		
		// ---------------------------------------------
		void setCustomizeArea( AABB area ) { mCustomizeArea = area; }
		AABB getCustomizeArea() { return mCustomizeArea; }
        
		// ---------------------------------------------
        void setCustomizeAreaTop(Vector2 topLeft)
        {
            mCustomizeAreaTop = topLeft;
            
            setCustomizeArea(AABB(mCustomizeAreaTop, mCustomizeAreaBottom));
        }
        void setCustomizeAreaBottom(Vector2 bottomRight)
        {
            mCustomizeAreaBottom = bottomRight;
            
            setCustomizeArea(AABB(mCustomizeAreaTop, mCustomizeAreaBottom));
        }
		
		// ---------------------------------------------
		virtual void setEnabled(bool v) { mEnabled = v; }
		bool getEnabled() {return mEnabled; }
		
		// ---------------------------------------------
		virtual void reset() { }
		
		// ---------------------------------------------
		void setTexture( int i, TexturePtr tex ) { _setTexture(i, tex); }
		TexturePtr getTexture( int i ) { return _getTexture(i); }
		
		// ---------------------------------------------
		// ---------------------------------------------
		virtual void setFont( const std::string& fontID )
		{
			mFontIdentifier = fontID;
			reloadFont();
		}
		std::string getFontID() { return mFontIdentifier; }
		
		void setDrawOffset( Vector2 newOffset ) { mDrawOffset = newOffset; }
		
		BitmapFont* getFont() { return mFont; }
		
		virtual void reloadFont()
		{
			mFont = FontManager::getInstancePtr()->getFont(mFontIdentifier);
		}
		
		// ---------------------------------------------
		// setting widget mgr
		// ---------------------------------------------
		void setWidgetMgr(WidgetManager* const widgetMgr);
		
		// ---------------------------------------------
		// update, and action results
		// ---------------------------------------------
		// return value when the widget wants to notify a change in it's behavior, etc. (such as a button being pressed, etc)
		struct WidgetActionRet
		{
			WidgetActionRet() { valBool = false; valFloat1 = valFloat2 = 0.0f; valInt1 = valInt2 = 0; }
			
			bool	valBool;
			float	valFloat1;
			float	valFloat2;
			int		valInt1;
			int		valInt2;
		};
		
		
		// ---------------------------------------------
		// called once per frame, after input functions have been called, to allow processing on the input and to
		// notify any resulting actions from the widget.
		virtual bool update( float elapsedSec, WidgetActionRet& ret ) { return false; }
		
		// ---------------------------------------------
		// call to initialize this widget with data from a PropertyList, usually when loading from a data file.
		// the base class implementation handles all of the common properties that all widgets share:
		// Orientation, Visible, Enabled, AllowCustomize, Font, Transparency, Textures.
		virtual void setProperties( const PropertyList& plist );
		
		// ---------------------------------------------
		// position and sizing common to all widgets.
		// ---------------------------------------------
		virtual void draw(SpriteBatch* sb);
		void draw(SpriteBatch* sb, Vector2 offset);
		
		// draw the frame of this widget (for debugging purposes)
		void drawFrame(SpriteBatch* sb);
		
		// ---------------------------------------------
		// handling various input cases.
		// ---------------------------------------------
		// when a finger has just touched the screen, and it intersects with this widget.
		// return TRUE if the widget wants to "accept" this finger, and become the focused widget for it.
		virtual bool acceptNewFingerDown( int fingerID, FingerInfo* info ) { return false; }
		
		// when a finger that was already on the screen has moved, and is now intersecting with this widget.
		// return TRUE if the widget wants to "accept" this finger, and become the fosuced widget for it.
		virtual bool acceptNewFingerEntered( int fingerID, FingerInfo* info ) { return false; }
		
		// when a finger has moved, but remained entirely inside this widget.
		// return TRUE to release focus of the widget
		virtual bool releaseFingerMoved( int fingerID, FingerInfo* info ) { return true; }
		
		// when a finger stays still this frame, on this widget.
		// return TRUE to release focus of the finger.
		virtual bool releaseFingerStayed( int fingerID, FingerInfo* info ) { return true; } 
		
		// when a finger leaves the screen, and it was intersecting with this widget.
		// return TRUE to release focus of the finger.
		virtual void releaseFingerUp( int fingerID, FingerInfo* info ) { }
		
		// when a finger moves off of this widget, but it still on the screen.
		// return TRUE to release focus of the finger.
		virtual bool releaseFingerLeft( int fingerID, FingerInfo* info ) { return true; }
		
		// notification that a finger that used to belog to this widget has been "stolen" by a higher priority widget.
		virtual void notifyFingerLost( int fingerID, FingerInfo* info ) { }

	protected:
		void _updateAABB();
		float _getAngle();
		std::string _generateWidgetName( int index );
		
		bool _hasTexture( int i );
		TexturePtr _getTexture( int i );
		void _setTexture( int i, TexturePtr tex );
		
		// overload if a widget needs to do some special processing / logic when a texture is set.
		virtual void _textureWasSet( int i, TexturePtr tex ) { }
		
		unsigned int		mWidgetType;		// enum for type of widget
		WidgetOrientation	mOrientation;		// orientation on-screen.
		bool				mVisible;			// visibility setting.
		bool				mEnabled;			// is the control enabled for use
		Vector2				mBaseSize;			// size in screen space (it's also affected by world scale)
		AABB				mAABB;				// AABB hitbox based on position and size.
		SnapType			mSnapType;			// how this widget "snaps" to the edges of the screen when customized.
		bool				mAllowCustomize;	// whether or not to let this widget's position be customized.
		AABB				mCustomizeArea;		// area to allow customization.  if set to invalid, use entire screen.
        Vector2             mCustomizeAreaTop;
        Vector2             mCustomizeAreaBottom;
		WidgetManager*		mWidgetMgr;			// widget manager that contains this widget
		
		std::string			mFontIdentifier;	// string used to reload fonts from the font manager
		
		BitmapFont*			mFont;				// pointer to a font that this widget may use
		
		Vector2				mDrawOffset;		// Offset used in draw, but not the actual location
        float               mTransparency;      // transparency between 0 - 1.
		
		TextureList			mTextures;			// vector of textures since nearly all Widgets use textures.
	
	};



	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	// widget manager class
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	// callback class.
	
	enum FocusEvent
	{
		FE_NewFingerDown,
		FE_NewFingerEntered,
		FE_FingerLeft,
		FE_FingerMovedWithin,
		FE_FingerStayedWithin,
		FE_FingerUp,
		FE_FingerStolen
	};
	
	class WidgetManagerEventHandler
	{
	public:
        virtual ~WidgetManagerEventHandler()
        {}
        
		virtual void handleEvent( int widgetName, const Widget::WidgetActionRet& action, Widget* widget ) {}
		virtual void handleFocus( Widget* const widget, FocusEvent e ) {}
	};


	// class for managing widgets, and input into/out of them.
	class WidgetManager : public SceneGraph
	{
	public:
		WidgetManager(WidgetManagerEventHandler* handler,const Vector2& screenSize);
		~WidgetManager();
		
		// -- -- -- -- -- -- -- -- -- -- -- --
		// init variables
		// -- -- -- -- -- -- -- -- -- -- -- --
		void init();
		
		// -- -- -- -- -- -- -- -- -- -- -- --
		// adding and removing widgets...
		// -- -- -- -- -- -- -- -- -- -- -- --
		void addWidget( Widget* widget, int priority );
		void removeWidget( Widget* widget );
		void removeAndDestroyWidget( Widget* widget );
		void clearAndDestroyAllWidgets();
		
		// -- -- -- -- -- -- -- -- -- -- -- --
		// getting widgets...
		// -- -- -- -- -- -- -- -- -- -- -- --
		//Widget* getFirstWidget();
		//Widget* getNextWidget();
		Widget* getWidget( int name );

        /*!
        Get the specified widget
        @param name the id/name of the widget, usually this is the value of an enum in the widget
            manager class
        @tparam T the type of the expected widget, if the widget does not exist it will be NULL
        @return NULL if the widget does not exist. Warning: This does not verify the type of the
            widget.
        @todo add widget type verification, we should consider some more robust form of RTTI for the
            widet system, would make using factories to create UI elements much easier.
        */
        template< typename T >
		T* getSpecificWidget( int name )
        {
            Widget * widget = getWidget( name );
            //if ( widget->getWidgetType() == )
            {
                return static_cast< T * >( widget );
            }
        }
		
		// -- -- -- -- -- -- -- -- -- -- -- --
		// updating layer count
		// -- -- -- -- -- -- -- -- -- -- -- --
		void updateLayerCount(int layer);
		
		// -- -- -- -- -- -- -- -- -- -- -- --
		// updating (this creates events from widgets that want to report something this frame.
		// -- -- -- -- -- -- -- -- -- -- -- --
		void update( float elapsedSec );
		
		// -- -- -- -- -- -- -- -- -- -- -- --
		// drawing things.
		// -- -- -- -- -- -- -- -- -- -- -- --
		void drawAll(SpriteBatch& sb);
		void drawLayerRange(SpriteBatch& sb, int minLayer, int maxLayer);
        void drawLayerRange(SpriteBatch& sb, Camera* cam, int minLayer, int maxLayer);
        
		void drawAllFrames(SpriteBatch& sb);
		
		// -- -- -- -- -- -- -- -- -- -- -- --
		// setting "customize" mode (allows fingers to draw widgets around on-screen)
		// -- -- -- -- -- -- -- -- -- -- -- --
		void setScreenSize( Vector2 size ) { mScreenSize = size; }
		void setCustomizeMode( bool onOff );
		bool isCustomizingMode() { return mCustomizing; }
		void updateCustomize();
		bool customizeShouldSave() { return mCustomizeDirty; }
		void forceCustomizeShouldSave( bool v ) { mCustomizeDirty = v; }
		bool isMovingWidget();
		
		void setMultiTouchEnabled(bool isEnabled) { mMultiTouchEnabled = isEnabled; }
		void setShowFingers(bool newVal) { mShowFingers = newVal; }
		void setFingerTexture(TexturePtr newTex) { mFingerTexture = newTex; }
		
		
		
		int getHighestWidgetIndex() { return mHighestWidgetIndex; }
		
		// -- -- -- -- -- -- -- -- -- -- -- --
		// enable / disable the entire manager
		// -- -- -- -- -- -- -- -- -- -- -- --
		void setEnabled( bool e ) { mEnabled = e; if (!mEnabled) { _clearFingers(); } }
		bool getEnabled() { return mEnabled; }
		
		// -- -- -- -- -- -- -- -- -- -- -- --
		// raw input.
		// -- -- -- -- -- -- -- -- -- -- -- --
		void touchDown( int touchID, Vector2 pos );
		void touchMoved( int touchID, Vector2 curPos, Vector2 lastPos );
		void touchUp( int touchID, Vector2 pos );
		void resetFingers() { _clearFingers(); }
		
		// -- -- -- -- -- -- -- -- -- -- -- --
		// returns AABB that includes all fo the widgets in this mgr
		// -- -- -- -- -- -- -- -- -- -- -- --
		AABB getBoundingArea();
		
		// -- -- -- -- -- -- -- -- -- -- -- --
		// reload all widget's fonts
		// -- -- -- -- -- -- -- -- -- -- -- --
		void reloadFonts();
		
	private:
		
		void _clearFingers();
		
		Vector2 _snapWidget( Widget* w, Vector2 pos, AABB allowedArea );
		
		
		WidgetManagerEventHandler*			mHandler;
		
		//typedef std::multimap<int, Widget*>	WidgetMap;			// multimap so we can sort be priority.
		//WidgetMap							mWidgets;			// sorted by specified priority.
		
		FingerInfoMap						mFingerInfoMap;
		
		bool								mCustomizing;		// customize mode flag.
		bool								mCustomizeDirty;	// flag to notify if a save is necessary
		
		std::map< Widget*, Vector2 >		mWidgetOffsets;
		Vector2								mScreenSize;
		
		//WidgetMap::iterator					mIterator;
		
		bool								mEnabled;
		bool								mMultiTouchEnabled;
		
		int									mNumLayers;
		int									mHighestWidgetIndex;
		
		bool								mShowFingers;
		TexturePtr							mFingerTexture;
	};

	
}


#endif	// _WALABER_GUI_H_
