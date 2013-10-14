#ifndef _TRANSITION_BLOCK_H_
#define _TRANSITION_BLOCK_H_

#include <SpriteBatch.h>
#include <vector>

#include "Transition.h"

namespace Walaber
{
	// --------========--------========--------========--------========--------========--------========--------========
    class Transition_Block : public Transition
	{
	public:
		
		Transition_Block();
		virtual ~Transition_Block();
		
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
		
		std::vector<bool>								mBlockPresent;
		int												mBlocks;
		
		int												mBlocksDone;
		
		int												mBlocksX;
		int												mBlocksY;
    };
	
}

#endif	// _TRANSITION_BLOCK_H_
