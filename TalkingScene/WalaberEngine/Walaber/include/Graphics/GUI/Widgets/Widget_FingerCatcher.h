#ifndef _WALABER_WIDGET_FINGER_CATCHER_H_
#define _WALABER_WIDGET_FINGER_CATCHER_H_

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
	// widget for a simple region that accepts and holds a list of all fingers that come it's way.  sort of a low-level finger
    // list, but one that is compatible with the GUI's priority system.
	// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
	class Widget_FingerCatcher : public Widget
	{
	public:
        struct CaughtFingerInfo
        {
            int         age;
            FingerInfo* base_info;
        };
        
        typedef std::map< int, CaughtFingerInfo >    FingerInfoMap;
        
		Widget_FingerCatcher( int name, Vector2 pos, Vector2 size );
		~Widget_FingerCatcher();
		
		void init();
		
		// valid properties:
		// "AcceptsEntered"
		void setProperties( const PropertyList& plist );
		
		
		bool update( float elapsedSec, WidgetActionRet& ret );
		
		void reset() { mFingerInfos.clear(); }
        
        void setAcceptsEntered( bool react ) { mAcceptsEntered = react; }
		
		bool acceptNewFingerDown( int fingerID, FingerInfo* info );
		bool acceptNewFingerEntered( int fingerID, FingerInfo* info );
		bool releaseFingerStayed( int fingerID, FingerInfo* info );
		bool releaseFingerMoved( int fingerID, FingerInfo* info );
        void releaseFingerUp( int fingerID, FingerInfo* info );
		bool releaseFingerLeft( int fingerID, FingerInfo* info );
		void notifyFingerLost( int fingerID, FingerInfo* info );
		
        const FingerInfoMap& getFingerMap() const { return mFingerInfos; }
        const std::list<int>& getLostFingerList() const { return mLostFingerList; }
		
        Vector2 getLostFingerPosition( int fingerID );
        void lostFingerHandled( int fingerID );
        
	private:
		bool _acceptFinger( int fingerID, FingerInfo* info );
		bool _releaseFinger( int fingerID, FingerInfo* info );
        
        FingerInfoMap               mFingerInfos;
        std::map< int, Vector2 >    mFingerLastKnownPosition;
        std::list< int >            mLostFingerList;
		
        bool                        mAcceptsEntered;
	};
}

#endif	// _WALABER_WIDGET_FINGER_CATCHER_H_