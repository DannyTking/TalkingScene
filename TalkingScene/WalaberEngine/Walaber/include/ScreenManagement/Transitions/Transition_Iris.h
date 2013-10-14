#ifndef _TRANSITION_IRIS_H_
#define _TRANSITION_IRIS_H_

#include <SpriteBatch.h>

#include "Transition.h"

namespace Walaber
{
	class BitmapFont;
	
	
	// --------========--------========--------========--------========--------========--------========--------========
    class Transition_Iris : public Transition
    {
	public:
		
		Transition_Iris();
		virtual ~Transition_Iris();
		
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


		// --------========--------========---- ----========--------========--------========--------========--------========
		void _initBladeAnchors();

		// --------========--------========---- ----========--------========--------========--------========--------========
		struct BladeInfo
		{
			BladeInfo() : anchor(0,0), base_angle(0.0f) {}

			Vector2		anchor;
			float		base_angle;
		};

		typedef std::vector< BladeInfo >		BladeInfoList;



		
		// --------========--------========---- ----========--------========--------========--------========--------========
		bool							mPastHalfway;

		int								mBladeCount;
		float							mBladeAngle;
		float							mBladeAngleMax;

		float							mStartRadiusScale;
		float							mEndRadiusScale;
		float							mRadiusScale;

		BladeInfoList					mBladeAnchors;

		TexturePtr						mBladeTex;
		Color							mCornerColors[4];
		Rect							mCornerUVs;

    };
}

#endif	// _TRANSITION_WIPE_H_
