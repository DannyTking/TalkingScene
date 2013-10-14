#ifndef _TRANSITION_WIDGET_SLIDE_H_
#define _TRANSITION_WIDGET_SLIDE_H_

#include <SpriteBatch.h>
#include <AABB.h>
#include <ScreenManager.h>

#include "Transition.h"

namespace Walaber
{
	
	// --------========--------========--------========--------========--------========--------========--------========
    class Transition_Widget_Slide : public Transition
    {
	public:
		
		Transition_Widget_Slide();
		virtual ~Transition_Widget_Slide();
		
		// --------========--------========--------========--------========--------========--------========--------========
		/// setup the transition with duration
		void setup( float duration );
		void setup( float duration, const PropertyList& plist );
		
		// --------========--------========---- ----========--------========--------========--------========--------========
		/// update the game by 1 frame.
        int update( float elapsedSec );
		
		// --------========--------========---- ----========--------========--------========--------========--------========
		/// draw all of the game screens, once for each given layer. [0, LayerCount)
        void draw();
	
		// --------========--------========---- ----========--------========--------========--------========--------========
		/// plist parameters
		
		
	private:
		
		void _setPositionsByDirection( AABB extents );
		
		void _populateWidgetManagerMap();
		void _findWidgetsInAABB();
		
		void _updateWidgetManagers(const float elapsedSec);
		void _offsetWidgetManagers(const Vector2& diff);
		
		bool											mIsIntro;
		bool											mPastHalfway;
		bool											mOffsetNewScreen;
		
		Vector2											mPosStr;
		Vector2											mPosMid;
		Vector2											mPosEnd;
		
		Vector2											mDirectionIn;
		Vector2											mDirectionOut;
		
		Vector2											mPos;
		
		AABB											mTransBox;
		
		int												mHighestWidgetIndex;
		int												mNumScreensInStack;
		
		bool											mDoStagger;
		
		std::map< Widget*, float >						mWidgetsToTimers;
    };
}

#endif	// _TRANSITION_WIDGET_SLIDE_H_
