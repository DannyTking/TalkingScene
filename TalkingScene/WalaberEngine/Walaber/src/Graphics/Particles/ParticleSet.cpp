//
//  ParticleSet.cpp
//  WalaberEngine
//
//  Created by Jason Pecho on 6/16/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "ParticleSet.h"

#include <SpriteBatch.h>
#include <Color.h>

namespace Walaber
{
	// JTP 03 JAN 2012
	// TODO: Make sure initParticles clears out any existing particles
    //=====-----=======---------=========---------==========---------=========----------=============
    void ParticleSet::initParticles(unsigned int numParticles)
    {
        mParticles.insert(mParticles.begin(), numParticles, Particle());
        
        // unroll for optimizing
        for(int i = 0; i < numParticles; ++i)
        {
            mUnusedParticles.push_back(i);
        }
        
        mNumParticles = numParticles;
    }
    
    //=====-----=======---------=========---------==========---------=========----------=============
    void ParticleSet::updateParticles(const float deltaTime)
    {		
        for(int i = 0; i <= mHighestLiveIndex; ++i)
        {
            if(!mParticles[i].mAlive)
            {
                continue;
            }
            
            // Check to see if the particle has expired
            if (mParticles[i].mLifetime > 0) 
            {
                mParticles[i].mLifetime -= deltaTime;
                // Death is knocking?
                if (mParticles[i].mLifetime <= 0.0f)
                {
                    removeParticle(i);
                    continue;
                }
            }
            
            if (mParticles[i].mOmega != 0) 
            {
                mParticles[i].mOmega *= mOmegaDamping;
                mParticles[i].mAngleDeg += mParticles[i].mOmega;
            }
            
            
            mParticles[i].mForceAccum = mParticles[i].mForceAccum * mParticles[i].mInvMass;
            
            // update particles
            mIntegrator.integrateParticle(mParticles[i].mPosition, mParticles[i].mPositionOld, mParticles[i].mVelocity, mParticles[i].mForceAccum, deltaTime);
            
            memset( &mParticles[i].mForceAccum, 0, sizeof(Vector2) );              
        }           
        
        // call the per particle post callback if it exists
        if(mPostParticleUpdateCallback)
        {
            PostUpdateParameters params(this);
            mPostParticleUpdateCallback->invoke(&params);
        }            
        //memset( &mPartForces[0], 0, sizeof(Vector2) * mNumParticles );
    }
    
    //=====-----=======---------=========---------==========---------=========----------=============
    void ParticleSet::drawParticles( SpriteBatch& sb, const std::vector<TexturePtr> textures, const Color& color, const int layer )
	{
		for(int i = 0; i <= mHighestLiveIndex; ++i)
		{
			if(!mParticles[i].mAlive)
			{
				continue;
			}
			
			if (mParticles[i].mLifetime == -1.0f) 
			{
                Color finalColor( color );
                finalColor *= mParticles[i].mAlphaMultiplier;
				sb.drawQuad(layer, textures[mParticles[i].mTextureIndex], mParticles[i].mPosition + VectorTools::rotateVector(mParticles[i].mOffset, Walaber::degToRad(mParticles[i].mAngleDeg)), Walaber::degToRad(mParticles[i].mAngleDeg), mParticles[i].mSize, finalColor);
			}
			else 
			{	
				Color fadeOut = _getParticleColor(&mParticles[i], color); // color * lerp(0.0f, 1.0f, mParticles[i].mLifetime / mParticles[i].mFadeLength);
                
				sb.drawQuad(layer, textures[mParticles[i].mTextureIndex], mParticles[i].mPosition + VectorTools::rotateVector(mParticles[i].mOffset, Walaber::degToRad(mParticles[i].mAngleDeg)), Walaber::degToRad(mParticles[i].mAngleDeg), getParticleSize(i), fadeOut);
			}
		}
	}
    
    //=====-----=======---------=========---------==========---------=========----------=============
    void ParticleSet::drawParticles( SpriteBatch& sb, const TexturePtr texture, const Color& color, const int layer )
	{
		for(int i = 0; i <= mHighestLiveIndex; ++i)
		{
			if(!mParticles[i].mAlive)
			{
				continue;
			}
			
			if (mParticles[i].mLifetime == -1.0f) 
			{
                Color finalColor( color );
                finalColor *= mParticles[i].mAlphaMultiplier;
				sb.drawQuad(layer, texture, mParticles[i].mPosition + VectorTools::rotateVector(mParticles[i].mOffset, Walaber::degToRad(mParticles[i].mAngleDeg)), Walaber::degToRad(mParticles[i].mAngleDeg), mParticles[i].mSize, finalColor);
			}
			else 
			{
				Color fadeOut = _getParticleColor(&mParticles[i], color);	// color * lerp(0.0f, 1.0f, mParticles[i].mLifetime / mParticles[i].mFadeLength);
                
				sb.drawQuad(layer, texture, mParticles[i].mPosition + VectorTools::rotateVector(mParticles[i].mOffset, Walaber::degToRad(mParticles[i].mAngleDeg)), Walaber::degToRad(mParticles[i].mAngleDeg), getParticleSize(i), fadeOut);
			}
		}
	}
	
	//=====-----=======---------=========---------==========---------=========----------=============
    void ParticleSet::drawParticles( SpriteBatch& sb, const TexturePtr texture, const Color& color, const int layer, const float sizeFactor, bool doFade )
	{
		for(int i = 0; i <= mHighestLiveIndex; ++i)
		{
			if(!mParticles[i].mAlive)
			{
				continue;
			}
			
			if (mParticles[i].mLifetime == -1.0f) 
			{
                Color finalColor( color );
                finalColor *= mParticles[i].mAlphaMultiplier;
				sb.drawQuad(layer, texture, mParticles[i].mPosition + VectorTools::rotateVector(mParticles[i].mOffset, Walaber::degToRad(mParticles[i].mAngleDeg)), Walaber::degToRad(mParticles[i].mAngleDeg), mParticles[i].mSize * sizeFactor, finalColor);
			}
			else 
			{
				Color finalCol = color;
				
				if (doFade)
					finalCol = _getParticleColor(&mParticles[i], color); // color * lerp(0.0f, 1.0f, mParticles[i].mLifetime / mParticles[i].mFadeLength);
                
				sb.drawQuad(layer, texture, mParticles[i].mPosition + VectorTools::rotateVector(mParticles[i].mOffset, Walaber::degToRad(mParticles[i].mAngleDeg)), Walaber::degToRad(mParticles[i].mAngleDeg), getParticleSize(i) * sizeFactor, finalCol);
			}
		}
	}
	
	//=====-----=======---------=========---------==========---------=========----------=============
    Color ParticleSet::_getParticleColor(const Walaber::Particle *p, const Walaber::Color &c)
	{
		float fadeInPercent = clampf( (p->mOrigLifetime - p->mLifetime) / p->mFadeInLength, 0.0f, 1.0f );
		
		if (fadeInPercent < 1.0f)
		{
			// fading in!
			return c * fadeInPercent * p->mAlphaMultiplier;
		}
		else if(p->mFadeLength != 0)
		{
			// potentially fading out...
			return c * p->mAlphaMultiplier * lerp(0.0f, 1.0f, p->mLifetime / p->mFadeLength);
		}
        return c;
	}
	
    
    
}