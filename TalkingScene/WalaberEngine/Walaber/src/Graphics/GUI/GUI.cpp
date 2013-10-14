#include "GUI.h"
#include "VectorTools.h"

#include "GUIWidgets.h"

#include "SoundManager.h"
#include "Utilities.h"
#include "StringHelper.h"

// debug
#include "QuadHelper.h"
#include "Color.h"
#include "SpriteBatch.h"

#include <queue>

namespace Walaber
{
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	WidgetOrientation stringToWidgetOrientation( const std::string& s )
	{
		std::string l = StringHelper::toLower(s);
		WidgetOrientation ret = WO_PortraitUp;
		
		if (l == "portraitdown")
			ret = WO_PortraitDown;
		else if (l == "landscapedown")
			ret = WO_LandscapeDown;
		else if (l == "landscapeup")
			ret = WO_LandscapeUp;
		
		return ret;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	std::string widgetOrientationToString( const WidgetOrientation& wo )
	{
		std::string ret = "PortraitUp";
		
		switch (wo)
		{
			case WO_LandscapeDown:
				ret = "LandscapeDown";
				break;
				
			case WO_LandscapeUp:
				ret = "LandscapeUp";
				break;
				
			case WO_PortraitDown:
				ret = "PortraitDown";
				break;
				
			default:
			case WO_PortraitUp:
				ret = "PortraitUp";
				break;
		}
		
		return ret;
	}
	
	
	
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	// Widget base class
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	Widget::Widget( int widgetName, unsigned int widgetType, Vector2 pos, Vector2 size, WidgetOrientation o, int layer ) : 
		DrawableNode( _generateWidgetName( widgetName ) ),
		mWidgetType(widgetType),
		mOrientation(o),
		mVisible(true),
		mEnabled(true),
		mBaseSize(size), 
		mSnapType(ST_EDGE),
		mAllowCustomize(true),
		mCustomizeArea(),
        mCustomizeAreaTop(Vector2::Zero),
        mCustomizeAreaBottom(Vector2::One),
		mWidgetMgr(NULL),
		mFontIdentifier(),
		mFont(NULL),
		mDrawOffset(Vector2::Zero),
        mTransparency(1),
		mTextures(4)
	{
		setLayer(layer);
		setLocalPosition2D(pos);	// forces AABB to update.
		setLocalScale2D( Vector2(1.0f, 1.0f) );
	}
		
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	// static initialization of the map
	WidgetTypeMap  Widget::widgetEnumMap = 
		Utilities::create_map< std::string, WidgetType >(std::string("WT_CLASSIC_CONTROLS"),WT_CLASSIC_CONTROLS)
		(std::string("WT_PUSH_BUTTON"),WT_PUSH_BUTTON)
		(std::string("WT_TIMED_BUTTON"), WT_TIMED_BUTTON)
		(std::string("WT_SPINNER"),WT_SPINNER)
		(std::string("WT_COLOR_PICKER"),WT_COLOR_PICKER)
		(std::string("WT_ICON_LIST"),WT_ICON_LIST)
		(std::string("WT_CANVAS"),WT_CANVAS)
		(std::string("WT_MANIPULATOR"),WT_MANIPULATOR)
		(std::string("WT_TOGGLE"),WT_TOGGLE)
		(std::string("WT_SLIDER"),WT_SLIDER)
		(std::string("WT_LABEL"), WT_LABEL)
		(std::string("WT_PROGRESS_BAR"), WT_PROGRESS_BAR)
		(std::string("WT_SLIDEWHEEL"), WT_SLIDEWHEEL)
		(std::string("WT_MOVING_TEXT_BOX"), WT_MOVING_TEXT_BOX)
		(std::string("WT_FINGER_CATCHER"), WT_FINGER_CATCHER)
		(std::string("WT_GROUP"), WT_GROUP)
		(std::string("WT_SCROLLABLE_GROUP"), WT_SCROLLABLE_GROUP)
		(std::string("WT_SCROLLABLE_SET"), WT_SCROLLABLE_SET)
		(std::string("WT_SCORE_COUNTER"),WT_SCORE_COUNTER)
		(std::string("WT_SCROLLABLE_CAMERA"),WT_SCROLLABLE_CAMERA)
		(std::string("WT_ANIMATION"),WT_ANIMATION);


	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	//statics
	bool Widget::getWidgetTypeFromString(std::string type, WidgetType& widgetTypeOut)
	{
		bool ret = false;
		
		WidgetTypeMap::iterator it = widgetEnumMap.find(type);
		if(it != widgetEnumMap.end())
		{
			widgetTypeOut = it->second;
			ret = true;
		}
		
		return ret;		
	}

		
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget::setLayer(const int layer)
	{
		DrawableNode::setLayer(layer );
		
		if(mWidgetMgr)
		{ 
			mWidgetMgr->updateLayerCount(mLayer);
		} 
	}
		
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget::setLocalPosition2D(const Vector2& pos)
	{
		Node::setLocalPosition2D( pos );
		_updateAABB();
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget::applyPositionOffset2D(const Vector2& pos)
	{
		Node::applyPositionOffset2D( pos );
		_updateAABB();
	}
    
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget::setLocalScale2D(const Vector2& scale)
	{
		Node::setLocalScale2D( scale );
		_updateAABB();
	}

	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget::setBaseSize( Vector2 size )
	{
		mBaseSize = size;
		_updateAABB();
	}
		
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget::setWidgetMgr(WidgetManager* const widgetMgr)
	{ 
		mWidgetMgr = widgetMgr; 
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget::setProperties( const PropertyList& plist )
	{
		const Property* p = plist.getValueForKey("Orientation");
		if (p)
			mOrientation = stringToWidgetOrientation(p->asString());
		
		p = plist.getValueForKey("Visible");
		if (p)
			mVisible = (p->asInt() == 1);
		
		p = plist.getValueForKey("Enabled");
		if (p)
			mEnabled = (p->asInt() == 1);
		
		p = plist.getValueForKey("AllowCustomize");
		if (p)
			mAllowCustomize = (p->asInt() == 1);
		
		p = plist.getValueForKey("Font");
		if (p)
			setFont(p->asString());
		
		p = plist.getValueForKey("Transparency");
		if (p)
			mTransparency = p->asFloat();
		
		// textures...
		p = plist.getValueForKey("Textures");
		if (p)
		{
			std::vector<std::string> texList = StringHelper::split(p->asString(), ',');
			
			for (int i = 0; i < texList.size(); i++)
			{
				_setTexture(i, TextureManager::getManager().getTexture(texList[i]));
			}
		}
	}
		
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget::draw(SpriteBatch* sb)
	{
	#if _DEBUG 
		drawFrame(sb);
        if(mCustomizeArea.Validity == AABB::Valid)
        {
            Vector2 topLeft = mAABB.Min;
            Vector2 bottomRight = mAABB.Max;
            Vector2 topRight = Vector2(mAABB.Max.X, mAABB.Min.Y);
            Vector2 bottomLeft = Vector2(mAABB.Min.X, mAABB.Max.Y);

            sb->drawLineColored(1000, topLeft, topRight, 2.0f, Color::Red);
            sb->drawLineColored(1000, bottomRight, topRight, 2.0f, Color::Red);
            sb->drawLineColored(1000, bottomRight, bottomLeft, 2.0f, Color::Red);
            sb->drawLineColored(1000, topLeft, bottomLeft, 2.0f, Color::Red);
        }
	#endif
	}
		
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget::draw(SpriteBatch* sb, Vector2 offset)
	{
		mDrawOffset = offset;
		draw(sb);
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget::drawFrame(SpriteBatch* sb)
	{
		float angle = _getAngle();
		
		Vector2 realSize = getSize();
		
		sb->drawQuadColored(mLayer+1, getWorldPosition2D()-VectorTools::rotateVector(Vector2(0.0f,realSize.Y*.5f), angle), angle, Vector2(realSize.X, 2.0f), Color::Red);
		sb->drawQuadColored(mLayer+1, getWorldPosition2D()+VectorTools::rotateVector(Vector2(0.0f,realSize.Y*.5f), angle), angle, Vector2(realSize.X, 2.0f), Color::Red);
		sb->drawQuadColored(mLayer+1, getWorldPosition2D()-VectorTools::rotateVector(Vector2(realSize.X*.5f,0.0f), angle), angle, Vector2(2.0f, realSize.Y), Color::Red);
		sb->drawQuadColored(mLayer+1, getWorldPosition2D()+VectorTools::rotateVector(Vector2(realSize.X*.5f,0.0f), angle), angle, Vector2(2.0f, realSize.Y), Color::Red);
		sb->drawQuadColored(mLayer+1, getWorldPosition2D(), angle, Vector2(10,10), Color::Red);
	}


	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget::_updateAABB()
	{
        Vector2 size = getSize();
		Vector2 halfSize = size * 0.5f;
		
		if ((mOrientation == WO_LandscapeUp) || (mOrientation == WO_LandscapeDown))
		{
			halfSize = Vector2(halfSize.Y, halfSize.X);
		}
		
        Vector2 topLeft = getWorldPosition2D() - halfSize;
        Vector2 bottomRight = getWorldPosition2D() + halfSize;
        if(mCustomizeArea.Validity == AABB::Valid)
        {
            bottomRight = topLeft + mCustomizeArea.Max * size;            
            topLeft += mCustomizeArea.Min * size;
        }
        mAABB = AABB(topLeft, bottomRight);
	}

	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	float Widget::_getAngle()
	{
		float angle = 90.0f;
		if (mOrientation == WO_LandscapeDown) { angle = 270.0f; }
		else if (mOrientation == WO_PortraitUp) { angle = 0.0f; }
		else if (mOrientation == WO_PortraitDown) { angle = 180.0f; }
		
		angle += getRotation();
		if(angle > 360.0f)
		{
			angle -= 360.0f;
		}
		else if(angle < 0.0f)
		{
			angle += 360.0f;
		}	
		
		return Walaber::degToRad(angle);
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	std::string Widget::_generateWidgetName( int index )
	{
		// Turn the index into a string
		std::string returnString = StringHelper::intToStr(index);
		
		// And to guarentee a unique name (some widgets can have unspecified names) - generate a random string
		// (The problem remains - how do I find it after this??)
		
		//returnString += "-" + StringHelper::intToStr( mWidgetMgr->getNumWidgetsCreated() );
		//returnString += "-" + StringHelper::intToStr( randomRange( 0, 10000 ) );
		
		return returnString;
	}
	
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	bool Widget::_hasTexture( int i )
	{
		if ((i < 0) || (i >= mTextures.size()))
			return false;
		
		return (mTextures[i]);
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	TexturePtr Widget::_getTexture( int i )
	{
		if (_hasTexture(i))
			return mTextures[i];
		
		return TexturePtr();
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	void Widget::_setTexture( int i, TexturePtr tex )
	{
		while (mTextures.size() <= i)
			mTextures.push_back(TexturePtr());
		
		mTextures[i] = tex;
		
		_textureWasSet(i, tex);
	}
	
	
	
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	// widget manager class
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	WidgetManager::WidgetManager(WidgetManagerEventHandler* handler, const Vector2& screenSize) : 
		SceneGraph(), 
		mHandler(handler), 
		mCustomizing(false), 
		mScreenSize(screenSize),
		mEnabled(true),
		mMultiTouchEnabled(false),
		mNumLayers(1),
		mHighestWidgetIndex(-1),
		mShowFingers(false),
		mFingerTexture(NULL)
	{
	}

	WidgetManager::~WidgetManager()
	{
		for (FingerInfoMap::iterator it = mFingerInfoMap.begin(); it != mFingerInfoMap.end(); ++it)
			delete it->second;
		
		mFingerInfoMap.clear();
	}

	// -- -- -- -- -- -- -- -- -- -- -- --
	// adding and removing widgets...
	// -- -- -- -- -- -- -- -- -- -- -- --
	void WidgetManager::addWidget( Widget* widget, int priority )
	{
		widget->setWidgetMgr(this);
		this->updateLayerCount(widget->getLayer());
		
		getRootNode()->addChild( widget );
		widget->setGroupID( priority );
		
		// Is this a higher index?
		mHighestWidgetIndex = maxi( mHighestWidgetIndex, widget->getWidgetNameAsInt() );
		
		//mWidgets.insert( std::pair<int, Widget*>( priority, widget ) );
	}

	// -- -- -- -- -- -- -- -- -- -- -- --
	void WidgetManager::removeWidget( Widget* widget )
	{
		getRootNode()->removeChild( widget );
		/*
		for (WidgetMap::iterator it = mWidgets.begin(); it != mWidgets.end(); ++it)
		{
			if (it->second == widget)
			{
				mWidgets.erase( it );
				return;
			}
		}
		 */
	}
		
	// -- -- -- -- -- -- -- -- -- -- -- --
	void WidgetManager::removeAndDestroyWidget( Widget* widget )
	{
		PCSNode::deleteNodeAndChildrenFromTree( widget );
		/*
		for (WidgetMap::iterator it = mWidgets.begin(); it != mWidgets.end(); ++it)
		{
			if (it->second == widget)
			{
				delete widget;
				mWidgets.erase( it );
				return;
			}
		}
		 */
	}	

	// -- -- -- -- -- -- -- -- -- -- -- --
	void WidgetManager::clearAndDestroyAllWidgets()
	{
		PCSNode* tempNode = getRootNode()->getChild();
		while ( tempNode != NULL )
		{
			PCSNode::deleteNodeAndChildrenFromTree( tempNode );
			tempNode = getRootNode()->getChild();
		}
		
		/*
		for (WidgetMap::iterator it = mWidgets.begin(); it != mWidgets.end(); ++it)
			delete it->second;
		
		mWidgets.clear();
		 */
	}

	/*
	// -- -- -- -- -- -- -- -- -- -- -- --
	// getting widgets...
	// -- -- -- -- -- -- -- -- -- -- -- --
	Widget* WidgetManager::getFirstWidget()
	{
		if (mWidgets.empty())
			return NULL;
		
		mIterator = mWidgets.begin();
		return mIterator->second;
	}
		
	// -- -- -- -- -- -- -- -- -- -- -- --
	Widget* WidgetManager::getNextWidget()
	{
		++mIterator;
		if (mIterator != mWidgets.end())
			return mIterator->second;
		else
			return NULL;
	}
	 */

	// -- -- -- -- -- -- -- -- -- -- -- --
	Widget* WidgetManager::getWidget( int name )
	{
		return (Widget*) getNodeByName( StringHelper::intToStr(name) );
		
		/*
		for (WidgetMap::iterator it = mWidgets.begin(); it != mWidgets.end(); ++it)
		{
			if (it->second->getWidgetName() == name)
				return it->second;
		}
		
		return NULL;
		 */
	}
		
	// -- -- -- -- -- -- -- -- -- -- -- --
	void WidgetManager::updateLayerCount(int layer)
	{
		if(layer >= mNumLayers)
		{
			// zero based layer system
			mNumLayers = layer+1;
		}
	}	

	// -- -- -- -- -- -- -- -- -- -- -- --
	// updating (this creates events from widgets that want to report something this frame.
	// -- -- -- -- -- -- -- -- -- -- -- --
	void WidgetManager::update( float elapsedSec )
	{
		if (!mEnabled)
			return;
		
		//Logger::printf("Walaber", Logger::SV_DEBUG, "WidgetManager::beginUpdate\n");
	
		if (mCustomizing)
		{
			updateCustomize();
			return;
		}
		
		
		// first handle any fingers that already have a focused widget...
		for (FingerInfoMap::iterator it = mFingerInfoMap.begin(); it != mFingerInfoMap.end(); ++it)
		{
			if (it->second->focusWidget != NULL)
			{
				Widget* w = it->second->focusWidget;
				
				bool stolen;
				//WidgetMap::iterator fw;
				
				switch (it->second->state)
				{
					case FS_MOVED:
						// find an iterator for the focused widget.
						stolen = false;
						
						if (getRootNode()->getChild() != NULL)
						{
							PCSNode* node;
							std::stack<PCSNode*> nodeStack;
							nodeStack.push((Widget*)getRootNode()->getChild());
							
							std::multimap<int, Widget*> widgetsWithClaim;
							
							// moved finger, first check if we've moved onto a widget with a higher priority.
							while(!nodeStack.empty())
							{
								node = nodeStack.top();
								nodeStack.pop();
								
								// stop searching if we've reached widgets of similar, or "lower" priority than the focused widget.
								if (node->getGroupID() < w->getGroupID())
								{
									// Widget at this position?
									if (((Widget*)node)->isVisible())
									{
										if (((Widget*)node)->getAABB().contains( it->second->curPos ))
										{
											widgetsWithClaim.insert(std::pair< int, Widget* >( node->getGroupID(), (Widget*)node ));
										}
									}
								}
								
								// Otherwise, check remaining relations
								if(node->getSibling())
								{
									nodeStack.push(node->getSibling());
								}
								
								if(node->getChild())
								{
									nodeStack.push(node->getChild());
								}     
									
								node = 0;          
							}
							
							// Now, move through all of the claims, and see who wants it
							for (std::multimap< int, Widget* >::iterator wit = widgetsWithClaim.begin(); wit != widgetsWithClaim.end(); wit++)
							{
								if (wit->second->acceptNewFingerEntered( it->first, it->second ))
								{
									// this widget has "stolen" this finger.
									stolen = true;
									w->notifyFingerLost( it->first, it->second );
									it->second->focusWidget = wit->second;
									
									mHandler->handleFocus(w, FE_FingerStolen);
									mHandler->handleFocus(wit->second, FE_NewFingerEntered);
									break;
								}
							}
						}
						
						
						//std::vector< Widgets* > mWidgets = getNodesFromTree( getRootNode(), const int groupID, std::vector<PCSNode*>& outPCSNodeList)
						/*
						fw = mWidgets.begin();
						while (fw != mWidgets.end())
						{
							if (fw->second == w)
								break;
							
							fw++;
						}
						
						// Here, we want to traverse the entire tree...so we have a special recursive function
						
						// moved finger, first check if we've moved onto a widget with a higher priority.
						for (WidgetMap::iterator stealer = mWidgets.begin(); stealer != mWidgets.end(); stealer++)
						{
							// stop searching if we've reached widgets of similar, or "lower" priority than the focused widget.
							//if (stealer->first >= fw->first)
							if (stealer->first >= w->getGroupID())
								break;
							
							if (!stealer->second->isVisible())
								continue;
							
							if (stealer->second->getAABB().contains( it->second->curPos ))
							{
								if (stealer->second->acceptNewFingerEntered( it->first, it->second ))
								{
									// this widget has "stolen" this finger.
									stolen = true;
									w->notifyFingerLost( it->first, it->second );
									it->second->focusWidget = stealer->second;
									
									mHandler->handleFocus(w, FE_FingerStolen);
									mHandler->handleFocus(stealer->second, FE_NewFingerEntered);
									break;
								}
							}
						 }
						 */
									
						if (!stolen)
						{
							if (w->getAABB().contains(it->second->curPos))
							{
								// still on the widget.
								if (w->releaseFingerMoved( it->first, it->second ))
								{
									mHandler->handleFocus(w, FE_FingerMovedWithin);
									
									it->second->focusWidget = NULL;
								}
								
							}
							else
							{
								// no longer on the widget.
								if (w->releaseFingerLeft( it->first, it->second ))
								{
									mHandler->handleFocus(w, FE_FingerLeft);
									
									it->second->focusWidget = NULL;
								}
							}
						}
						break;
						
					case FS_SUSTAINED:
						// finger has not moved this frame.
						if (w->getAABB().contains(it->second->curPos))
						{
							// still on the widget.
							if (w->releaseFingerStayed( it->first, it->second ))
							{
								mHandler->handleFocus(w, FE_FingerStayedWithin);
								
								it->second->focusWidget = NULL;
							}
						}
						else
						{
							// no longer on the widget.
							if (w->releaseFingerLeft( it->first, it->second ))
							{
								mHandler->handleFocus(w, FE_FingerLeft);
								
								it->second->focusWidget = NULL;
							}
						}
						break;
						
					case FS_LOST:
						// lost finger...
						w->releaseFingerUp( it->first, it->second );
						mHandler->handleFocus(w, FE_FingerUp);
						break;
					default:
						break;
				}
			}
		}
		
		// now clean up any fingers that have been lost.
		for (FingerInfoMap::iterator it = mFingerInfoMap.begin(); it != mFingerInfoMap.end(); ++it)
		{
			if (it->second->state  == FS_LOST)
			{
				delete it->second;
				mFingerInfoMap.erase( it );
				if (mFingerInfoMap.empty()) { break; }
				
				it = mFingerInfoMap.begin();
			}
		}
		
		// okay, now handle any fingers that are not focused on a specific widget.
		for (FingerInfoMap::iterator it = mFingerInfoMap.begin(); it != mFingerInfoMap.end(); ++it)
		{
			// focus widgets have already been handled, we're only interested in fingers that don't have a "home" yet :)
			if (it->second->focusWidget != NULL)
				continue;
			
			if ((it->second->state == FS_NEW) || (it->second->state == FS_MOVED))
			{
				if (getRootNode()->getChild() != NULL)
				{
					PCSNode* node;
					std::stack<PCSNode*> nodeStack;
					nodeStack.push((Widget*)getRootNode()->getChild());
					
					std::multimap<int, Widget*> widgetsWithClaim;
					
					// moved finger, first check if we've moved onto a widget with a higher priority.
					while(!nodeStack.empty())
					{
						node = nodeStack.top();
						nodeStack.pop();
						
						// Widget at this position?
						if (((Widget*)node)->isVisible())
						{
							if (((Widget*)node)->getAABB().contains( it->second->curPos ))
							{
								widgetsWithClaim.insert(std::pair< int, Widget* >( node->getGroupID(), (Widget*)node ));
							}
						}
						
						// Otherwise, check remaining relations
						if(node->getSibling())
						{
							nodeStack.push(node->getSibling());
						}
						
						if(node->getChild())
						{
							nodeStack.push(node->getChild());
						}     
						
						node = 0;          
					}
					
					
					// Now, move through all of the claims, and see who wants it
					for (std::multimap< int, Widget* >::iterator wit = widgetsWithClaim.begin(); wit != widgetsWithClaim.end(); wit++)
					{
						if (it->second->state == FS_NEW)
						{
							if (wit->second->acceptNewFingerDown( it->first, it->second ))
							{
								it->second->focusWidget = wit->second;
								it->second->state = FS_SUSTAINED;
								
								mHandler->handleFocus(wit->second, FE_NewFingerDown);
								break;
							}
						}
						else
						{
							if (wit->second->acceptNewFingerEntered( it->first, it->second ))
							{
								it->second->focusWidget = wit->second;
								mHandler->handleFocus(wit->second, FE_NewFingerEntered);
								break;
							}
						}
					}
				}
				
				/*
				// this is a new finger that wasn't claimed by any widgets in the previous frame.
				for (WidgetMap::iterator wi = mWidgets.begin(); wi != mWidgets.end(); ++wi)
				{
					if (!wi->second->isVisible())
						continue;
					
					if (wi->second->getAABB().contains( it->second->curPos ))
					{
						if (wi->second->acceptNewFingerDown( it->first, it->second ))
						{
							it->second->focusWidget = wi->second;
							it->second->state = FS_SUSTAINED;
							
							mHandler->handleFocus(wi->second, FE_NewFingerDown);
							break;
						}
					}
				}
				 */
				
				if (it->second->state == FS_NEW)
				{
					it->second->state = FS_SUSTAINED;
				}
			}
			
			
			/*
			else if (it->second->state == FS_MOVED)
			{
				if (getRootNode()->getChild() != NULL)
				{
					PCSNode* node;
					std::stack<PCSNode*> nodeStack;
					nodeStack.push((Widget*)getRootNode()->getChild());
					
					// moved finger, first check if we've moved onto a widget with a higher priority.
					while(!nodeStack.empty())
					{
						node = nodeStack.top();
						nodeStack.pop();
						
						// Widget at this position?
						if (((Widget*)node)->isVisible())
						{
							if (((Widget*)node)->getAABB().contains( it->second->curPos ))
							{
								if (((Widget*)node)->acceptNewFingerEntered( it->first, it->second ))
								{
									it->second->focusWidget = (Widget*)node;
									mHandler->handleFocus((Widget*)node, FE_NewFingerEntered);
									break;
								}
							}
						}
						
						// Otherwise, check remaining relations
						if(node->getSibling())
						{
							nodeStack.push(node->getSibling());
						}
						
						if(node->getChild())
						{
							nodeStack.push(node->getChild());
						}     
						
						node = 0;          
					}
				}
				
				// finger moved, but didn't have a focused widget, or it was lost since it moved off the widget
				// that had focus previously.
				for (WidgetMap::iterator wi = mWidgets.begin(); wi != mWidgets.end(); ++wi)
				{
					if (!wi->second->isVisible())
						continue;
					
					if (wi->second->getAABB().contains( it->second->curPos ))
					{
						if (wi->second->acceptNewFingerEntered( it->first, it->second ))
						{
							it->second->focusWidget = wi->second;
							mHandler->handleFocus(wi->second, FE_NewFingerEntered);
							break;
						}
					}
				}
				 */
			//}
		}
		
		
		if (getRootNode()->getChild() != NULL)
		{
			Widget::WidgetActionRet ret;
			
			PCSNode* node;
			std::stack<PCSNode*> nodeStack;
			nodeStack.push((Widget*)getRootNode()->getChild());
			
			// moved finger, first check if we've moved onto a widget with a higher priority.
			while(!nodeStack.empty())
			{
				node = nodeStack.top();
				nodeStack.pop();
				
				// Widget at this position?
				if (((Widget*)node)->isVisible())
				{
					if (((Widget*)node)->update( elapsedSec, ret ))
					{
						if (mHandler != NULL)
							mHandler->handleEvent( ((Widget*)node)->getWidgetNameAsInt(), ret, ((Widget*)node) );
					}
				}
				
				// Otherwise, check remaining relations
				if(node->getSibling())
				{
					nodeStack.push(node->getSibling());
				}
				
				if(node->getChild())
				{
					nodeStack.push(node->getChild());
				}     
				
				node = 0;          
			}
		}
		/*
		// now update all widgets.
		for (WidgetMap::iterator wi = mWidgets.begin(); wi != mWidgets.end(); ++wi)
		{
			Widget::WidgetActionRet ret;
			
			if (!wi->second->isVisible())
				continue;
			
			if (wi->second->update( elapsedSec, ret ))
			{
				if (mHandler != NULL)
					mHandler->handleEvent( wi->second->getWidgetName(), ret, wi->second );
			}
		}
		*/
		
		for (FingerInfoMap::iterator it = mFingerInfoMap.begin(); it != mFingerInfoMap.end(); ++it)
		{
			if (it->second->state == FS_MOVED)
			{
				it->second->lastPos = it->second->curPos;
				it->second->state = FS_SUSTAINED;
			}
		}
		

		
		/*Logger::printf("Walaber", Logger::SV_DEBUG, "-- WidgetManager::update() - finger log --\n");
		for (FingerInfoMap::iterator it = mFingerInfoMap.begin(); it != mFingerInfoMap.end(); ++it)
		{
			Logger::printf("Walaber", Logger::SV_DEBUG, "   --> finger [%d][%p]  state[%d]  focusWidget[%p]\n", it->first, it->second, it->second->state, it->second->focusWidget );
		}
		Logger::printf("Walaber", Logger::SV_DEBUG, " -- -- -- \n");*/
	
	}

	// -- -- -- -- -- -- -- -- -- -- -- --
	// drawing things.
	// -- -- -- -- -- -- -- -- -- -- -- --
	void WidgetManager::drawAll(SpriteBatch& sb)
	{
		if (getRootNode()->getChild() != NULL)
		{
			AABB screenAABB( Vector2::Zero, mScreenSize );

			PCSNode* node;
			std::stack<PCSNode*> nodeStack;
			nodeStack.push((Widget*)getRootNode()->getChild());
			
			// moved finger, first check if we've moved onto a widget with a higher priority.
			while(!nodeStack.empty())
			{
				node = nodeStack.top();
				nodeStack.pop();
				
				// Widget at this position?
				if (((Widget*)node)->isVisible())
				{
					if (mCustomizing && (!mCustomizeDirty) && (((Widget*)node)->allowCustomize()))
					{
						float offset = ((float)(rand() & 100) / 100.0f) * 1.0f;
						float angle = ((float)(rand() % 100) / 100.0f) * TWO_PI;
						((Widget*)node)->draw(&sb, VectorTools::rotateVector( Vector2(0,offset), angle ) );
					}
					else
					{
						// AABB check...
						Vector2 p = ((Widget*)node)->getWorldPosition2D();
						Vector2 s = ((Widget*)node)->getSize();

						AABB wAABB( p - (s * 0.5f), p + (s * 0.5f) );

						if (screenAABB.intersects( wAABB ))
							((Widget*)node)->draw(&sb);
					}
				}
				
				// Otherwise, check remaining relations
				if(node->getSibling())
				{
					nodeStack.push(node->getSibling());
				}
				
				if(node->getChild())
				{
					nodeStack.push(node->getChild());
				}     
				
				node = 0;          
			}
			
			// Optionally draw the finger positions
			if (mShowFingers)
			{
				for (FingerInfoMap::iterator it = mFingerInfoMap.begin(); it != mFingerInfoMap.end(); ++it)
				{
					Vector2 tPos = it->second->curPos;
					Vector2 tSize = ScreenCoord(0.05f, 0.0f).toScreen();
					tSize.Y = tSize.X;
					
					sb.drawQuad( mFingerTexture, tPos, 0.0f, tSize );
				}
			}
		}
	}
    
    void WidgetManager::drawLayerRange(SpriteBatch& sb, Camera* cam, int minLayer, int maxLayer)
    {
        if (getRootNode()->getChild() != NULL)
		{
			Vector2 camPos = cam->getPosition();
            Vector2 camSize = cam->getSize();
            
            AABB cameraAABB( camPos, camSize );
            
			PCSNode* node;
			std::stack<PCSNode*> nodeStack;
			nodeStack.push((Widget*)getRootNode()->getChild());
			
			// moved finger, first check if we've moved onto a widget with a higher priority.
			while(!nodeStack.empty())
			{
				node = nodeStack.top();
				nodeStack.pop();
				
				// Widget at this position?
				if (((Widget*)node)->isVisible())
				{
                    if((((Widget*)node)->getLayer() >= minLayer) || (minLayer == -1))
					{
						if((((Widget*)node)->getLayer() <= maxLayer) || (maxLayer == -1))
						{
                            if (mCustomizing && (!mCustomizeDirty) && (((Widget*)node)->allowCustomize()))
                            {
                                float offset = ((float)(rand() & 100) / 100.0f) * 1.0f;
                                float angle = ((float)(rand() % 100) / 100.0f) * TWO_PI;
                                ((Widget*)node)->draw(&sb, VectorTools::rotateVector( Vector2(0,offset), angle ) );
                            }
                            else
                            {
                                // AABB check...
                                Vector2 p = ((Widget*)node)->getWorldPosition2D();
                                Vector2 s = ((Widget*)node)->getSize();
                                
                                AABB wAABB( p - (s * 0.5f), p + (s * 0.5f) );
                                
                                //if (cameraAABB.intersects( wAABB ))
                                    ((Widget*)node)->draw(&sb);
                            }
                        }
                    }
				}
				
				// Otherwise, check remaining relations
				if(node->getSibling())
				{
					nodeStack.push(node->getSibling());
				}
				
				if(node->getChild())
				{
					nodeStack.push(node->getChild());
				}     
				
				node = 0;          
			}
			
			// Optionally draw the finger positions
			if (mShowFingers)
			{
				for (FingerInfoMap::iterator it = mFingerInfoMap.begin(); it != mFingerInfoMap.end(); ++it)
				{
					Vector2 tPos = it->second->curPos;
					Vector2 tSize = ScreenCoord(0.05f, 0.0f).toScreen();
					tSize.Y = tSize.X;
					
					sb.drawQuad( mFingerTexture, tPos, 0.0f, tSize );
				}
			}
		}
    }
		
	void WidgetManager::drawLayerRange(SpriteBatch& sb, int minLayer, int maxLayer)
	{
		if (getRootNode()->getChild() != NULL)
		{
			AABB screenAABB( Vector2::Zero, mScreenSize );

			PCSNode* node;
			std::stack<PCSNode*> nodeStack;
			nodeStack.push((Widget*)getRootNode()->getChild());
			
			// moved finger, first check if we've moved onto a widget with a higher priority.
			while(!nodeStack.empty())
			{
				node = nodeStack.top();
				nodeStack.pop();
				
				// Widget at this position?
				if (((Widget*)node)->isVisible())
				{
					if((((Widget*)node)->getLayer() >= minLayer) || (minLayer == -1))
					{
						if((((Widget*)node)->getLayer() <= maxLayer) || (maxLayer == -1))
						{
							if (mCustomizing && (!mCustomizeDirty) && (((Widget*)node)->allowCustomize()))
							{
								float offset = ((float)(rand() & 100) / 100.0f) * 1.0f;
								float angle = ((float)(rand() % 100) / 100.0f) * TWO_PI;
								((Widget*)node)->draw(&sb, VectorTools::rotateVector( Vector2(0,offset), angle ) );
							}
							else
							{
								// AABB check...
								Vector2 p = ((Widget*)node)->getWorldPosition2D();								Vector2 s = ((Widget*)node)->getSize();

								AABB wAABB( p - (s * 0.5f), p + (s * 0.5f) );

								if (screenAABB.intersects( wAABB ))
									((Widget*)node)->draw(&sb);
							}
						}
					}
				}
				
				// Otherwise, check remaining relations
				if(node->getSibling())
				{
					nodeStack.push(node->getSibling());
				}
				
				if(node->getChild())
				{
					nodeStack.push(node->getChild());
				}     
				
				node = 0;          
			}
		}
	}
	
	
	// -- -- -- -- -- -- -- -- -- -- -- --
	// -- -- -- -- -- -- -- -- -- -- -- --
	void WidgetManager::drawAllFrames(SpriteBatch& sb)
	{
		if (getRootNode()->getChild() != NULL)
		{
			AABB screenAABB( Vector2::Zero, mScreenSize );
			
			PCSNode* node;
			std::stack<PCSNode*> nodeStack;
			nodeStack.push((Widget*)getRootNode()->getChild());
			
			// moved finger, first check if we've moved onto a widget with a higher priority.
			while(!nodeStack.empty())
			{
				node = nodeStack.top();
				nodeStack.pop();
				
				// Widget at this position?
				if (((Widget*)node)->isVisible())
				{
					// AABB check...
					Vector2 p = ((Widget*)node)->getWorldPosition2D();
					Vector2 s = ((Widget*)node)->getSize();
						
					AABB wAABB( p - (s * 0.5f), p + (s * 0.5f) );
						
					if (screenAABB.intersects( wAABB ))
						((Widget*)node)->drawFrame(&sb);
				}
				
				// Otherwise, check remaining relations
				if(node->getSibling())
				{
					nodeStack.push(node->getSibling());
				}
				
				if(node->getChild())
				{
					nodeStack.push(node->getChild());
				}
				
				node = 0;
			}
		}
	}
	
		
	// -- -- -- -- -- -- -- -- -- -- -- --
	// setting "customize" mode (allows fingers to draw widgets around on-screen)
	// -- -- -- -- -- -- -- -- -- -- -- --
	void WidgetManager::setCustomizeMode( bool onOff )
	{
		mCustomizing = onOff;
		
		// forget all fingers currently on the screen and start fresh.
		_clearFingers();
		
		mWidgetOffsets.clear();
		mCustomizeDirty = false;
	}

	// -- -- -- -- -- -- -- -- -- -- -- --
	bool WidgetManager::isMovingWidget()
	{
		if (mCustomizing)
		{
			for (FingerInfoMap::iterator it = mFingerInfoMap.begin(); it != mFingerInfoMap.end(); ++it)
				if (it->second->focusWidget != NULL) return true;
		}
		
		return false;
	}


	// -- -- -- -- -- -- -- -- -- -- -- --
	void WidgetManager::updateCustomize()
	{
		for (FingerInfoMap::iterator it = mFingerInfoMap.begin(); it != mFingerInfoMap.end(); ++it)
		{
			if (it->second->state == FS_NEW)
			{
				if (getRootNode()->getChild() != NULL)
				{
					PCSNode* node;
					std::stack<PCSNode*> nodeStack;
					nodeStack.push((Widget*)getRootNode()->getChild());
					
					// moved finger, first check if we've moved onto a widget with a higher priority.
					while(!nodeStack.empty())
					{
						node = nodeStack.top();
						nodeStack.pop();
						
						// Widget at this position?
						if ((((Widget*)node)->allowCustomize()) && (((Widget*)node)->isVisible()))
						{
							if (((Widget*)node)->getAABB().contains( it->second->curPos ))
							{
								it->second->focusWidget = ((Widget*)node);
								it->second->state = FS_SUSTAINED;
								mWidgetOffsets[ ((Widget*)node) ] = it->second->curPos - ((Widget*)node)->getLocalPosition2D();
								mCustomizeDirty = true;
								break;
							}
						}
						
						// Otherwise, check remaining relations
						if(node->getSibling())
						{
							nodeStack.push(node->getSibling());
						}
						
						if(node->getChild())
						{
							nodeStack.push(node->getChild());
						}     
						
						node = 0;          
					}
				}
				
				/*
				for (WidgetMap::iterator wi = mWidgets.begin(); wi != mWidgets.end(); wi++)
				{
					if (!wi->second->allowCustomize())
						continue;
					
					if (!wi->second->isVisible())
						continue;
					
					if (wi->second->getAABB().contains( it->second->curPos ))
					{
						it->second->focusWidget = wi->second;
						it->second->state = FS_SUSTAINED;
						mWidgetOffsets[ wi->second ] = it->second->curPos - wi->second->getLocalPosition2D();
						mCustomizeDirty = true;
						break;
					}
				}
				 */
			}
			else if (it->second->state == FS_MOVED)
			{
				if (it->second->focusWidget != NULL)
				{
					std::map< Widget*, Vector2 >::iterator oi = mWidgetOffsets.find( it->second->focusWidget );
					if (oi != mWidgetOffsets.end())
					{
						AABB allowedArea = it->second->focusWidget->getCustomizeArea();
						if (allowedArea.Validity == AABB::Invalid) { allowedArea = AABB(Vector2::Zero,mScreenSize); }
						
						Vector2 finalPos = _snapWidget( it->second->focusWidget, it->second->curPos - oi->second, allowedArea );
						
						it->second->focusWidget->setLocalPosition2D( finalPos );
						mCustomizeDirty = true;
					}
				}
			}
		}
		
		// now clean up any fingers that have been lost.
		for (FingerInfoMap::iterator it = mFingerInfoMap.begin(); it != mFingerInfoMap.end(); ++it)
		{
			if (it->second->state == FS_LOST)
			{
				delete it->second;
				mFingerInfoMap.erase( it );
				if (mFingerInfoMap.empty()) { break; }
				
				it = mFingerInfoMap.begin();
			}
		}
		
	}

	// -- -- -- -- -- -- -- -- -- -- -- --
	void WidgetManager::_clearFingers()
	{
		Logger::printf("Walaber", Logger::SV_DEBUG, "_clearFingers()\n");
	
		// destroy all fingers.
		for (FingerInfoMap::iterator it = mFingerInfoMap.begin(); it != mFingerInfoMap.end(); ++it)
		{
			if (it->second->focusWidget != NULL)
			{
				Logger::printf("Walaber", Logger::SV_DEBUG, "  finger [%d][%p] had focus widget!\n", it->first, it->second);
	
				it->second->focusWidget->notifyFingerLost( it->first, it->second );
			}
			
			delete it->second;
		}
		
		// reset all widgets as well...
		if (getRootNode()->getChild() != NULL)
		{
			PCSNode* node;
			std::stack<PCSNode*> nodeStack;
			nodeStack.push((Widget*)getRootNode()->getChild());
			
			// moved finger, first check if we've moved onto a widget with a higher priority.
			while(!nodeStack.empty())
			{
				node = nodeStack.top();
				nodeStack.pop();
				
				// Widget at this position?
				((Widget*)node)->reset();
				
				// Otherwise, check remaining relations
				if(node->getSibling())
				{
					nodeStack.push(node->getSibling());
				}
				
				if(node->getChild())
				{
					nodeStack.push(node->getChild());
				}     
				
				node = 0;          
			}
		}
		
		/*
		for (WidgetMap::iterator it = mWidgets.begin(); it != mWidgets.end(); ++it)
			it->second->reset();
		*/
		
		
		mFingerInfoMap.clear();
	}

	// -- -- -- -- -- -- -- -- -- -- -- --
	Vector2 WidgetManager::_snapWidget( Widget* w, Vector2 pos, AABB allowedArea )
	{
		Vector2 ret = pos;
		
		// distance to left side of allowed area.
		float baseX = ((w->getSnapType() == ST_EDGE) ? ((w->getSize().X * 0.5f) + WIDGET_SNAP_PADDING) : 0.0f);
		float goalX = allowedArea.Min.X + baseX;
		float dist = pos.X - goalX; 
		if (abs(dist) <= WIDGET_SNAP_AMOUNT)
		{
			ret.X = goalX;
		}
		else 
		{
			// check right edge.
			goalX = allowedArea.Max.X - baseX;
			dist = pos.X - goalX;
			if (abs(dist) <= WIDGET_SNAP_AMOUNT)
				ret.X = goalX;
		}
		
		// distance to top of the screen (y=0)
		float baseY = ((w->getSnapType() == ST_EDGE) ? ((w->getSize().Y * 0.5f) + WIDGET_SNAP_PADDING) : 0.0f);
		float goalY = allowedArea.Min.Y + baseY;
		dist = pos.Y - goalY; 
		if (abs(dist) <= WIDGET_SNAP_AMOUNT)
		{
			ret.Y = goalY;
		}
		else 
		{
			// check right edge.
			goalY = allowedArea.Max.Y - baseY;
			dist = pos.Y - goalY;
			if (abs(dist) <= WIDGET_SNAP_AMOUNT)
				ret.Y = goalY;
		}
		
		// clip into allowed area...
		if (ret.X < allowedArea.Min.X) { ret.X = allowedArea.Min.X; }
		if (ret.X > allowedArea.Max.X) { ret.X = allowedArea.Max.X; }
		if (ret.Y < allowedArea.Min.Y) { ret.Y = allowedArea.Min.Y; }
		if (ret.Y > allowedArea.Max.Y) { ret.Y = allowedArea.Max.Y; }
		
		return ret;
	}

	// -- -- -- -- -- -- -- -- -- -- -- --
	// raw input.
	// -- -- -- -- -- -- -- -- -- -- -- --
	void WidgetManager::touchDown( int touchID, Vector2 pos )
	{
		if (!mEnabled)
			return;
		
		if ( !mMultiTouchEnabled && !mFingerInfoMap.empty() )
		{
			return;
		}
		
		// do we already have a finger registered under this ID?
		FingerInfoMap::iterator it = mFingerInfoMap.find( touchID );
		
		if (it != mFingerInfoMap.end())
		{
			Logger::printf("Walaber", Logger::SV_DEBUG, "WidgetManager::touchDown WARNING - new finger with same ID [%d] as existing finger!  removing old finger!\n", touchID);
			
			if (it->second->focusWidget == NULL)
			{
				if (getRootNode()->getChild() != NULL)
				{
					PCSNode* node;
					std::stack<PCSNode*> nodeStack;
					nodeStack.push((Widget*)getRootNode()->getChild());
					
					// moved finger, first check if we've moved onto a widget with a higher priority.
					while(!nodeStack.empty())
					{
						node = nodeStack.top();
						nodeStack.pop();
						
						// Widget at this position?
						if (((Widget*)node)->isVisible())
						{
							if (((Widget*)node)->getAABB().contains( it->second->curPos ))
							{
								if (((Widget*)node)->acceptNewFingerDown( it->first, it->second ))
								{
									it->second->focusWidget = ((Widget*)node);
									it->second->state = FS_SUSTAINED;
									
									mHandler->handleFocus(((Widget*)node), FE_NewFingerDown);
									break;
								}
							}
						}
						
						// Otherwise, check remaining relations
						if(node->getSibling())
						{
							nodeStack.push(node->getSibling());
						}
						
						if(node->getChild())
						{
							nodeStack.push(node->getChild());
						}     
						
						node = 0;          
					}
				}
				/*
				// this is a new finger
				for (WidgetMap::iterator wi = mWidgets.begin(); wi != mWidgets.end(); ++wi)
				{
					if (!wi->second->isVisible())
						continue;
					
					if (wi->second->getAABB().contains( it->second->curPos ))
					{
						if (wi->second->acceptNewFingerDown( it->first, it->second ))
						{
							it->second->focusWidget = wi->second;
							it->second->state = FS_SUSTAINED;
							
							mHandler->handleFocus(wi->second, FE_NewFingerDown);
							break;
						}
					}
				}
				 */
			}
			
			// okay, we might have got a widget focus from the above loop, now handle releasing that widget/finger!
			
			// now handle releasing!
			if (it->second->focusWidget != NULL)
			{
				it->second->focusWidget->releaseFingerUp( it->first, it->second );
				mHandler->handleFocus(it->second->focusWidget, FE_FingerUp);
			}

			delete it->second;
			
			mFingerInfoMap.erase( it );
		}
		
		
		FingerInfo* fi = new FingerInfo();
		fi->state = FS_NEW;
		fi->curPos = pos;
		fi->lastPos = pos;
		fi->focusWidget = NULL;
		
		mFingerInfoMap[ touchID ] = fi;
			
		//Logger::printf("Walaber", Logger::SV_DEBUG, "WidgetManager::touchDown( [%d][%p], [%4.2f,%4.2f] )  touchCount:%d\n", touchID, fi, pos.X, pos.Y, (int)mFingerInfoMap.size() );
	
	}


	// -- -- -- -- -- -- -- -- -- -- -- --
	void WidgetManager::touchMoved( int touchID, Vector2 curPos, Vector2 lastPos )
	{
		if (!mEnabled)
			return;
		
		// look for the finger info.
		FingerInfoMap::iterator it = mFingerInfoMap.find( touchID );
		
		if (it == mFingerInfoMap.end())
		{
			Logger::printf("Walaber", Logger::SV_DEBUG, "touchMoved ERROR - moved finger not found in map! %d\n", touchID);
			return;
		}
		
		// sometimes we touch and move a finger before we get a chance to update.  in that case,
		// keep the finger marked as "new", but update it to the latest position.
		if (it->second->state != FS_NEW)
			it->second->state = FS_MOVED;
		
		it->second->curPos = curPos;
		//it->second->lastPos = lastPos;
		
		//Logger::printf("Walaber", Logger::SV_DEBUG, "WidgetManager::touchMoved( [%d][%p], [%4.2f,%4.2f], [%4.2f,%4.2f] )  touchCount:%d\n", touchID, it->second, curPos.X, curPos.Y, lastPos.X, lastPos.Y, (int)mFingerInfoMap.size() );

	}

	// -- -- -- -- -- -- -- -- -- -- -- --
	void WidgetManager::touchUp( int touchID, Vector2 pos )
	{
		if (!mEnabled)
			return;
		
		// look for the finger info.
		FingerInfoMap::iterator it = mFingerInfoMap.find( touchID );
		
		if (it == mFingerInfoMap.end())
		{
			Logger::printf("Walaber", Logger::SV_DEBUG, "touchUp WARNING - removed finger not found in map! [%d]  fingerCount:%d\n", touchID, (int)mFingerInfoMap.size());

			return;
		}

		//Logger::printf("Walaber", Logger::SV_DEBUG, "WidgetManager::touchUp( [%d][%p], [%4.2f,%4.2f] )  fingerCount:%d\n", touchID, it->second, pos.X, pos.Y, (int)mFingerInfoMap.size() );
	
		
		// kill this object right now, otherwise a new finger might come along and stomp on us!
		// now clean up any fingers that have been lost.
		if (it->second->focusWidget != NULL)
		{
			it->second->focusWidget->releaseFingerUp( it->first, it->second );
			mHandler->handleFocus(it->second->focusWidget, FE_FingerUp);
		}
		else if (it->second->state == FS_NEW)
		{
			if (getRootNode()->getChild() != NULL)
			{
				PCSNode* node;
				std::stack<PCSNode*> nodeStack;
				nodeStack.push((Widget*)getRootNode()->getChild());
				
				// moved finger, first check if we've moved onto a widget with a higher priority.
				while(!nodeStack.empty())
				{
					node = nodeStack.top();
					nodeStack.pop();
					
					// Widget at this position?
					if (((Widget*)node)->isVisible())
					{
						if (((Widget*)node)->getAABB().contains( it->second->curPos ))
						{
							if (((Widget*)node)->acceptNewFingerDown( it->first, it->second ))
							{
								it->second->focusWidget = ((Widget*)node);
								it->second->state = FS_SUSTAINED;
								
								mHandler->handleFocus(((Widget*)node), FE_NewFingerDown);
								break;
							}
						}
					}
					
					// Otherwise, check remaining relations
					if(node->getSibling())
					{
						nodeStack.push(node->getSibling());
					}
					
					if(node->getChild())
					{
						nodeStack.push(node->getChild());
					}     
					
					node = 0;          
				}
			}
			
			/*
			// this finger came and went before we had a chance to process it!  process it now, so we don't miss it!
			// treat as new finger
			for (WidgetMap::iterator wi = mWidgets.begin(); wi != mWidgets.end(); ++wi)
			{
				if (!wi->second->isVisible())
					continue;
				
				if (wi->second->getAABB().contains( it->second->curPos ))
				{
					if (wi->second->acceptNewFingerDown( it->first, it->second ))
					{
						it->second->focusWidget = wi->second;
						it->second->state = FS_SUSTAINED;
						
						mHandler->handleFocus(wi->second, FE_NewFingerDown);
						break;
					}
				}
			}
			 */
		
			// okay, we might have got a widget focus from the above loop, now handle releasing that widget/finger!
			// now handle releasing!
			if (it->second->focusWidget != NULL)
			{
				it->second->focusWidget->releaseFingerUp( it->first, it->second );
				mHandler->handleFocus(it->second->focusWidget, FE_FingerUp);
			}
		}
		
		// NOW delete this finger for real...
		delete it->second;
		mFingerInfoMap.erase( it );
		
		Logger::printf("Walaber", Logger::SV_DEBUG, "WidgetManager::touchUp - removed finger, fingerCount:%d\n", (int)mFingerInfoMap.size());

	}
	
	// -- -- -- -- -- -- -- -- -- -- -- --
	AABB WidgetManager::getBoundingArea()
	{
		std::queue<PCSNode*> nodeQueue;        
		
		PCSNode* child = getRootNode()->getChild();
		while( child )
		{
			nodeQueue.push(child);
			child = child->getSibling();
		}
		
		AABB aabb;
		
        while(!nodeQueue.empty())
        {
            PCSNode* node = nodeQueue.front();
            nodeQueue.pop();
            
			aabb.expandToInclude( ((Widget*)node)->getAABB() );
			
            PCSNode* child_ = node->getChild();
            while(child_)
            {
                nodeQueue.push(child_);
                child_ = child_->getSibling();
            }
        }
		
		return aabb;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- --
	void WidgetManager::reloadFonts()
	{
		std::queue<PCSNode*> nodeQueue;        
		
		PCSNode* child = getRootNode()->getChild();
		while( child )
		{
			nodeQueue.push(child);
			child = child->getSibling();
		}
				
        while(!nodeQueue.empty())
        {
            PCSNode* node = nodeQueue.front();
            nodeQueue.pop();
            
			((Widget*)node)->reloadFont();
			
            PCSNode* child_ = node->getChild();
            while(child_)
            {
                nodeQueue.push(child_);
                child_ = child_->getSibling();
            }
        }		
	}
}
