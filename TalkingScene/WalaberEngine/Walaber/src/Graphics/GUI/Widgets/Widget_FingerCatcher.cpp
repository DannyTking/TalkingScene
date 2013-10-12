#include "Widget_FingerCatcher.h"
#include "VectorTools.h"

#include "Texture.h"

#include "BitmapFont.h"
#include "GraphicsGL.h"
#include "SpriteBatch.h"

namespace Walaber
{
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    Widget_FingerCatcher::Widget_FingerCatcher( int name, Vector2 pos, Vector2 size ) : 
    Widget(name, WT_FINGER_CATCHER, pos, size, WO_PortraitUp)
    {
    }
	
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    Widget_FingerCatcher::~Widget_FingerCatcher()
    {
        
    }
    
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    void Widget_FingerCatcher::init()
    {
		mAcceptsEntered = false;
	}
	
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
    void Widget_FingerCatcher::setProperties(const Walaber::PropertyList &plist)
	{
		Widget::setProperties(plist);

		const Property* p = plist.getValueForKey("AcceptsEntered");
		if (p)
			mAcceptsEntered = p->asBool();
	}
	
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    bool Widget_FingerCatcher::update( float elapsedSec, WidgetActionRet& ret )
    {
        ret.valInt1 = mFingerInfos.size();
        ret.valInt2 = mLostFingerList.size();
        
        //printf("update - fingers[%d] lost[%d]\n", ret.valInt1, ret.valInt2);
        return true;
    }
    
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    bool Widget_FingerCatcher::acceptNewFingerDown( int fingerID, FingerInfo* info )
    {
        return _acceptFinger(fingerID, info);
    }
    
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    bool Widget_FingerCatcher::acceptNewFingerEntered( int fingerID, FingerInfo* info )
    {
        if (mAcceptsEntered)
        {
            return _acceptFinger(fingerID, info);
        }
        
        return false;
    }
    
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    bool Widget_FingerCatcher::releaseFingerStayed( int fingerID, FingerInfo* info )
    {
        mFingerLastKnownPosition[ fingerID ] = info->curPos;
        
        FingerInfoMap::iterator it = mFingerInfos.find(fingerID);
        if (it != mFingerInfos.end())
        {
            it->second.age = it->second.age + 1;
        }
        
        return false;
    }
    
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    bool Widget_FingerCatcher::releaseFingerMoved( int fingerID, FingerInfo* info )
    {
        mFingerLastKnownPosition[ fingerID ] = info->curPos;
        
        FingerInfoMap::iterator it = mFingerInfos.find(fingerID);
        if (it != mFingerInfos.end())
        {
            it->second.age = it->second.age + 1;
        }
        
        return false;
    }
    
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    void Widget_FingerCatcher::releaseFingerUp( int fingerID, FingerInfo* info )
    {
        _releaseFinger( fingerID, info );
    }
    
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    bool Widget_FingerCatcher::releaseFingerLeft( int fingerID, FingerInfo* info )
    {
        return _releaseFinger( fingerID, info );
    }
    
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    void Widget_FingerCatcher::notifyFingerLost( int fingerID, FingerInfo* info )
    {
        _releaseFinger( fingerID, info );
    }
    
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    bool Widget_FingerCatcher::_acceptFinger( int fingerID, FingerInfo* info )
    {
        CaughtFingerInfo i;
        i.age = 0;
        i.base_info = info;
        
        mFingerInfos[ fingerID ] = i;
        mFingerLastKnownPosition[ fingerID ] = info->curPos;
        
        return true;        
    }
    
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    bool Widget_FingerCatcher::_releaseFinger( int fingerID, FingerInfo* info )
    {
        FingerInfoMap::iterator it = mFingerInfos.find(fingerID);
        
        if (it != mFingerInfos.end())
        {
            mFingerInfos.erase(it);
            mLostFingerList.push_back( fingerID );
        }
        
        mFingerLastKnownPosition[ fingerID ] = info->curPos;
        
        return true;
    }
    
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    Vector2 Widget_FingerCatcher::getLostFingerPosition( int fingerID )
    {
        std::map< int, Vector2 >::iterator it = mFingerLastKnownPosition.find( fingerID );
        
        if (it != mFingerLastKnownPosition.end())
            return it->second;
        
        return Vector2::Zero;
    }
    
    // -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
    void Widget_FingerCatcher::lostFingerHandled( int fingerID )
    {
        for (std::list<int>::iterator it = mLostFingerList.begin(); it != mLostFingerList.end(); ++it)
        {
            if ((*it) == fingerID)
            {
                mLostFingerList.erase(it);
                break;
            }
        }
    }
}


