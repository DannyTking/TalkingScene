#ifndef _TRANSITION_FADE_H_
#define _TRANSITION_FADE_H_

#include <SpriteBatch.h>

#include "Transition.h"

namespace Walaber
{
	class BitmapFont;
	
	
	// --------========--------========--------========--------========--------========--------========--------========
    class Transition_Fade : public Transition
    {
	public:
		
		Transition_Fade();
		virtual ~Transition_Fade();
		
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
		
		float											mAngle;
		Vector2											mSize;
		Vector2											mPos;
		
		Color											mColor;
		
		Color											mColorStr;
		Color											mColorEnd;
		
		
    };
}

#endif	// _TRANSITION_WIPE_H_
