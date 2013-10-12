//
//  RibbonParticle.h
//  WalaberEngine
//
//  Created by Chad Cable on 02/01/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef _RIBBON_PARTICLE_H_
#define _RIBBON_PARTICLE_H_

#include <Particle.h>

namespace Walaber 
{
	enum RibbonCurlDirection
	{
		RCD_Left,
		RCD_None,
		RCD_Right
	};
	
	template <int T>
    class RibbonParticle : public Particle
    {
    public:
        RibbonParticle():
		Particle(),
		mNumOldPositions(T),
		mRotationPerUpdate(0.0f),
		mCurlDirection(RCD_None)
        {}
        
        ~RibbonParticle()
        {}
		
        int							mNumOldPositions;
        Vector2                     mPositionOldFrame[T];
		float						mRotationPerUpdate;
		
		RibbonCurlDirection				mCurlDirection;
		
		int getNumOldFramePositions() { return mNumOldPositions; }
		void setRotationPerUpdate( float newRotation ) { mRotationPerUpdate = newRotation; }
    };
}



#endif
