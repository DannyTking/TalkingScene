//
//  Particle.h
//  WalaberEngine
//
//  Created by Jason Pecho on 6/12/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include <Vector2.h>
#include "BaseParticle.h"

namespace Walaber
{
    class Particle : public BaseParticle
    {
    public: // interface
        // <Summary>
		// default ctor - default everything
		// </Summary>
		Particle():
		BaseParticle(),
        mOffset(),
        mStartSize(),
        mEndSize(),
        mAngleDeg(0.0f),
        mAlphaMultiplier(1.0f),
        mOrigLifetime(-1.0f),
		mOmega(0.0f),
		mFadeLength(0.1f),
		mFadeInLength(0.0f),
		mTextureIndex(0),
        mSizeApexStart(0.0f),
        mSizeApexEnd(0.0f),
        mSizeSlerpUp(false),
        mSizeSlerpDown(false)
        {}
		
    
    public:
        
		Vector2         mOffset;
        Vector2			mStartSize;
        Vector2         mEndSize;
        
        float			mAngleDeg;
        float           mAlphaMultiplier; 
        float           mOrigLifetime;
		float			mOmega;
        float           mFadeLength;		// duration for "fading out" at end of lifetime
		float			mFadeInLength;		// duration for "fading in" at beginning of lifetime.

		int             mTextureIndex;
        
        float			mSizeApexStart;
        float 			mSizeApexEnd;

        bool			mSizeSlerpUp;
        bool			mSizeSlerpDown;
    };
}
#endif
