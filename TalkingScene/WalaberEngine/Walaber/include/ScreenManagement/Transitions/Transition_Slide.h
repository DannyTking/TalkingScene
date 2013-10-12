#ifndef _TRANSITION_SLIDE_H_
#define _TRANSITION_SLIDE_H_


#include "Transition.h"
namespace Walaber
{
	class PropertyList;
	
	// --------========--------========--------========--------========--------========--------========--------========
    class Transition_Slide : public Transition
    {
	public:
		
		Transition_Slide();
		virtual ~Transition_Slide();
		
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
		
	protected:
		
		// --------========--------========---- ----========--------========--------========--------========--------========
		void _setupRenderTextureBuffer();
		
		// --------========--------========---- ----========--------========--------========--------========--------========
		void _renderGameScreenToTexture();
		void _goIntro();
		void _goOutro();
		
		// --------========--------========---- ----========--------========--------========--------========--------========
		GLuint								mRenderTextureFrameBuffer;
		GLuint								mRenderTextureTexture;
		
		int									mFb_width;
		int									mFb_height;
		
		float								mFb_max_u;
        float								mFb_max_v;

		
		TransitionDirection					mDirection;
		
		bool								mIgnoreDraw;
		
		
		Vector2								mStartPos_FB;
		Vector2								mEndPos_FB;
		
		Vector2								mStartPos_Mask;
		Vector2								mEndPos_Mask;
		
		TexturePtr							mMaskTex;
		Vector2								mMaskTexSize;
		Color								mMaskColor;
		
		int									mFlushCounter;
		
    };
}

#endif	// _TRANSITION_H_
