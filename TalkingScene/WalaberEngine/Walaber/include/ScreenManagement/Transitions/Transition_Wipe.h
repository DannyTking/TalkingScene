#ifndef _TRANSITION_WIPE_H_
#define _TRANSITION_WIPE_H_

#include <SpriteBatch.h>

#include "Transition.h"

namespace Walaber
{
	
	// --------========--------========--------========--------========--------========--------========--------========
    class Transition_Wipe : public Transition
    {
	public:
		
		Transition_Wipe();
		virtual ~Transition_Wipe();
		
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
		
		bool											mPastHalfway;
		
		Vector2											mPosStr;
		Vector2											mPosMid;
		Vector2											mPosEnd;
		
		Vector2											mPos;
		
		float											mAngle;
		Vector2											mSize;
		Color											mColor;
		
    };
}

#endif	// _TRANSITION_WIPE_H_
