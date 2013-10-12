//
//  BaseParticle.h
//  WalaberEngine
//
//  Created by Sebastian Janisz on 5/22/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#ifndef _BASEPARTICLE_H_
#define _BASEPARTICLE_H_

#include <Vector2.h>

namespace Walaber
{
    class BaseParticle
    {
    public: // interface
        // <Summary>
		// default ctor - default everything
		// </Summary>
		BaseParticle():
        mPosition(),
		mPositionOld(),
		mVelocity(),
		mForceAccum(),		
		mSize(Vector2::One),
        mMass(0.0f),
		mInvMass(0.0f),
        mLifetime(-1.0f),
        mAlive(false),
		mUseGravity(false),
        mVisible(true)
        {}
		
		// <Summary>
		// initialize a BaseParticle
		// </Summary>
		
		// <Summary>
		// copy ctor
		// </Summary>
		BaseParticle(const BaseParticle& particle):
        mPosition(particle.mPosition),
		mPositionOld(particle.mPositionOld),
		mVelocity(particle.mVelocity),
		mForceAccum(particle.mForceAccum),		
		mSize(particle.mSize),
        mMass(particle.mMass),
		mInvMass(particle.mInvMass),
        mLifetime(particle.mLifetime),
        mAlive(particle.mAlive),
		mUseGravity(particle.mUseGravity),
        mVisible(particle.mVisible)
        {}
		
		// <Summary>
		// dtor
		// </Summary>
		virtual ~BaseParticle()
        {}
    
    public:
        
        Vector2         mPosition;
		Vector2         mPositionOld;
		Vector2         mVelocity;
		Vector2         mForceAccum;		
		Vector2         mSize;
        
        float			mMass;
		float			mInvMass;
        float			mLifetime;

        bool			mAlive;
        bool            mUseGravity;
        bool            mVisible;

    };
}
#endif
