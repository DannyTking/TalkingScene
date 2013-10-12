//
//  ParticleSet.h
//  WalaberEngine
//
//  Created by Jason Pecho on 6/15/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef _PARTICLE_SET_H_
#define _PARTICLE_SET_H_

#include <Particle.h>
#include <vector>
#include <list>

#include <TextureManager.h>
#include <VectorTools.h>
#include <VerletIntegrator.h>
#include <Callback.h>

namespace Walaber
{    
    class Color;
    class SpriteBatch; 
    
    class ParticleSet
    {
    public:
        struct PostUpdateParameters
        {
            ParticleSet* particleSet;
            
            PostUpdateParameters():
            particleSet(0)
            {}
            
            PostUpdateParameters(ParticleSet* parts):
            particleSet(parts)
            {}
        };
        
    public:
        
        // <Summary>
		// create a set of particles
		// </Summary>
        ParticleSet():
        mParticles(),
        mUnusedParticles(),        
        mHighestLiveIndex(-1),
		mNumLiveParticles(0),
		mNumParticles(0),
        mOmegaDamping(1.0f),
		mIntegrator(),        
        mPostParticleUpdateCallback(NULL)
        {
            
        }
        
        // <Summary>
		// create and a init # of particles
		// </Summary>
        explicit ParticleSet(unsigned int numParticles):
        mParticles(numParticles),
        mUnusedParticles(),        
        mHighestLiveIndex(-1),
		mNumLiveParticles(0),
		mNumParticles(numParticles),
        mOmegaDamping(1.0f),
		mIntegrator(),        
        mPostParticleUpdateCallback(NULL)
        {
            for(int i = 0; i < numParticles; ++i)
            {
                mUnusedParticles.push_back(i);
            }
        }
        
        // <Summary>
		// dtor
		// </Summary>
        ~ParticleSet()
        {}
        
        // <Summary>
		// init # of particles
		// </Summary>
        void initParticles(unsigned int numParticles);
        
        // <Summary>
		// give life to a particle with args
		// </Summary>
        inline bool addParticle(const float mass, const Vector2& position, const float angleDeg, const Vector2& size, int& outIndex)
		{
			if(mUnusedParticles.empty())
			{
				//Logger::printf("Walaber", Logger::SV_INFO, "No more free particles!!!\n");
                outIndex = -1;
				return false;
			}
			
			// get next unused particle index
			int j = mUnusedParticles.front();
			mUnusedParticles.pop_front();
			
			mParticles[j].mMass = mass;
			
            if(mass != 0.0f)
            {
                mParticles[j].mInvMass = (1.0f/mass);
            }
            else
            {
                mParticles[j].mInvMass = 0.0f;
            }
            
			mParticles[j].mAngleDeg = angleDeg;
			mParticles[j].mPosition = position;			
			mParticles[j].mPositionOld = position;
			mParticles[j].mVelocity = Vector2::Zero;
			mParticles[j].mForceAccum = Vector2::Zero;
			mParticles[j].mSize = size;
			
			mParticles[j].mLifetime = -1.0f;
			mParticles[j].mOrigLifetime = -1.0f;
			mParticles[j].mOmega = 0.0f;
			mParticles[j].mTextureIndex = 0;
    
			mParticles[j].mAlive = true;
            
            // Default values
            mParticles[j].mVisible = true;
            mParticles[j].mUseGravity = true;     
            mParticles[j].mEndSize = mParticles[j].mSize;
            mParticles[j].mFadeLength = 0.5f;
			
			++mNumLiveParticles;
			
			if(j > mHighestLiveIndex)
			{
				mHighestLiveIndex = j;
			}
			
            outIndex = j;
			return true;
		}

        // <Summary>
		// give life to a particle with more args
		// </Summary>
        inline bool addParticle(const float mass, const Vector2& position, const Vector2& positionOld, const float angleDeg, const Vector2& size, const Vector2& velocity, const Vector2& forces, int& outIndex)
		{
			if(mUnusedParticles.empty())
			{
				//Logger::printf("Walaber", Logger::SV_INFO, "No more free particles!!!\n");
                outIndex = -1;
				return false;
			}
			
			// get next unused particle index
			int j = mUnusedParticles.front();
			mUnusedParticles.pop_front();
			
			mParticles[j].mMass = mass;
			
            if(mass != 0.0f)
            {
                mParticles[j].mInvMass = (1.0f/mass);
            }
            else
            {
                mParticles[j].mInvMass = 0.0f;
            }
            
			mParticles[j].mAngleDeg = angleDeg;
			mParticles[j].mPosition = position;			
			mParticles[j].mPositionOld = positionOld;
			mParticles[j].mVelocity = velocity;
			mParticles[j].mForceAccum = forces;
			mParticles[j].mSize = size;
			
			mParticles[j].mLifetime = -1.0f;
			mParticles[j].mOrigLifetime = -1.0f;
			mParticles[j].mOmega = 0.0f;
			mParticles[j].mTextureIndex = 0;
            
			mParticles[j].mAlive = true;
            
            // Default values
            mParticles[j].mVisible = true;
            mParticles[j].mUseGravity = true;
            mParticles[j].mEndSize = mParticles[j].mSize;
            mParticles[j].mFadeLength = 0.5f;
			
			++mNumLiveParticles;
			
			if(j > mHighestLiveIndex)
			{
				mHighestLiveIndex = j;
			}
			
            outIndex = j;
			return true;
		}
        
        // <Summary>
		// give life to a particle with even more args
		// </Summary>
        inline bool addParticle(const float mass, const Vector2& position, const Vector2& positionOld, const float angleDeg, const Vector2& size, const Vector2& velocity, const Vector2& forces, const float newLifetime, const float newOmega, const int newTextureIndex, int& outIndex)
		{
			if(mUnusedParticles.empty())
			{
				//Logger::printf("Walaber", Logger::SV_INFO, "No more free particles!!!\n");
                outIndex = -1;
				return false;
			}
			
			// get next unused particle index
			int j = mUnusedParticles.front();
			mUnusedParticles.pop_front();
			
			mParticles[j].mMass = mass;
			
            if(mass != 0.0f)
            {
                mParticles[j].mInvMass = (1.0f/mass);
            }
            else
            {
                mParticles[j].mInvMass = 0.0f;
            }
            
			mParticles[j].mAngleDeg = angleDeg;
			mParticles[j].mPosition = position;			
			mParticles[j].mPositionOld = positionOld;
			mParticles[j].mVelocity = velocity;
			mParticles[j].mForceAccum = forces;
			mParticles[j].mSize = size;
			
			mParticles[j].mLifetime = newLifetime;
			mParticles[j].mOrigLifetime = newLifetime;
			mParticles[j].mOmega = newOmega;
			mParticles[j].mTextureIndex = newTextureIndex;
            
			mParticles[j].mAlive = true;
            
            // Default values
            mParticles[j].mVisible = true;
            mParticles[j].mUseGravity = true;
            mParticles[j].mEndSize = mParticles[j].mSize;
            mParticles[j].mFadeLength = 0.5f;
			
			++mNumLiveParticles;
			
			if(j > mHighestLiveIndex)
			{
				mHighestLiveIndex = j;
			}
			
            outIndex = j;
			return true;
		}
        
        // <Summary>
		// update our particles by integration and such
		// </Summary>
        void updateParticles(const float deltaTime);
        
        // <Summary>
		// set the angle of a particle in degrees
		// </Summary>
        inline void setParticleAngleDeg(const int index, const float angleDeg)
		{
			assert(index < mNumParticles);
			
			mParticles[index].mAngleDeg = angleDeg;
		}
        
        // <Summary>
		// set the size of a particle
		// </Summary>
        inline void setParticleSize(const int index, const Vector2& size)
		{
			assert(index < mNumParticles);
			
			mParticles[index].mSize = size;
		}
        
        // <Summary>
		// set the lifetime of a particle
		// </Summary>
        inline void setParticleLifetime(const int index, const float newLifetime)
		{
			assert(index < mNumParticles);
			
			mParticles[index].mLifetime = newLifetime;
		}
        
		// <Summary>
		// set the damping of the integrator
		// </Summary>
        void setParticleDamping(const float damping)
        {
            mIntegrator.setDamping(damping);
        }
		
        // <Summary>
		// set the damping of the SET of particles
		// </Summary>
        inline void setFluidParticleSetOmegaDamping(const float newOmegaDamping)
		{
			mOmegaDamping = newOmegaDamping;
		}
        
        // <Summary>
		// set the texture index of a particle
		// </Summary>
        inline void setParticleTextureIndex(const int index, const int newTextureIndex)
		{
			assert(index < mNumParticles);
			
			mParticles[index].mTextureIndex= newTextureIndex;
		}
        
        // <Summary>
		// set whether the particle uses gravity or not
		// </Summary>
        inline void setParticleUseGravity(const int index, const bool newUseGravity)
		{
			assert(index < mNumParticles);
			
			mParticles[index].mUseGravity = newUseGravity;
		}
         
           
        // <Summary>
		// set the start size of a particle
		// </Summary>
        inline void setParticleStartSize(const int index, const Vector2 newStartSize)
		{
			assert(index < mNumParticles);
			
			mParticles[index].mStartSize = newStartSize;
		}                
                      
                                  
        // <Summary>
		// set the end size of a particle
		// </Summary>
        inline void setParticleEndSize(const int index, const Vector2 newEndSize)
		{
			assert(index < mNumParticles);
			
			mParticles[index].mEndSize = newEndSize;
		}
        
        // <Summary>
		// set the fade length of a particle
		// </Summary>
        inline void setParticleFadeLength(const int index, const float newFadeLength)
		{
			assert(index < mNumParticles);
			
			mParticles[index].mFadeLength = newFadeLength;
		}
		
		// <Summary>
		// set the fade length of a particle
		// </Summary>
        inline void setParticleFadeInLength(const int index, const float newFadeLength)
		{
			assert(index < mNumParticles);
			
			mParticles[index].mFadeInLength = newFadeLength;
		}
        
		// <Summary>
		// set the timespan in which the particle is largest
		// </Summary>
        inline void setParticleSizeApex(const int index, const float newSizeApexStart, const float newSizeApexEnd)
        {
			assert(index < mNumParticles);
        	
            mParticles[index].mSizeApexStart = newSizeApexStart;
            mParticles[index].mSizeApexEnd   = newSizeApexEnd;            
        }
        
        
		// <Summary>
		// set the time at which the particle is largest
		// </Summary>
        inline void setParticleSizeApex(const int index, const float newSizeApex)
        {
			assert(index < mNumParticles);
        	
            mParticles[index].mSizeApexStart = newSizeApex;
            mParticles[index].mSizeApexEnd   = newSizeApex;            
        }        
        
        
		// <Summary>
		// set whether the particle slerps up to its apex size
		// </Summary>
        inline void setParticleSizeSlerp(const int index, const bool newSizeSlerpUp, const bool newSizeSlerpDown)
        {
			assert(index < mNumParticles);
        	
            mParticles[index].mSizeSlerpUp   = newSizeSlerpUp;
            mParticles[index].mSizeSlerpDown = newSizeSlerpDown;            
        }           
        
        
		// <Summary>
		// set whether the particle slerps up to its apex size
		// </Summary>
        inline void setParticleSizeSlerp(const int index, const bool newSizeSlerp)
        {
			assert(index < mNumParticles);
        	
            mParticles[index].mSizeSlerpUp   = newSizeSlerp;
            mParticles[index].mSizeSlerpDown = newSizeSlerp;            
        }
        
        
        inline void setParticleAlphaMultiplier(const int index, const float alpha)
        {
            mParticles[index].mAlphaMultiplier = alpha;
        }
        
        
        // <Summary>
		// murder a particle
		// </Summary>
		void removeParticle(int index) 
		{ 			
			if(mParticles[index].mAlive)
			{
				mParticles[index].mAlive = false;
				--mNumLiveParticles;
				
				// add to recycling list
				mUnusedParticles.push_front(index);
				
				if(index && index == mHighestLiveIndex)
				{
					int i = index-1;
					
					// iterate backwards from the dead particle to find the largest alive particle
					while(i > 0 && !mParticles[i].mAlive)
					{
						--i;
					}			
					
					mHighestLiveIndex = (unsigned int)i;
				}	
			}
		}
        
        // <Summary>
		// get the number of particles this set can contain
		// </Summary>
        inline unsigned int getNumParticles()const { return mNumParticles; }
		
		// <Summary>
        // get the number of particles currently alive
		// </Summary>
		inline unsigned int getNumLiveParticles()const { return mNumLiveParticles; }
		
		// <Summary>
		// get the index of the highest alive particle
		// </Summary>
		inline int getHighestLiveIndex()const { return mHighestLiveIndex; }		
		
		// <Summary>
        // set the callback to call after all the particles have updated
		// </Summary>
		inline void setPostParticleUpdateCallback(CallbackPtr postParticleUpdateCallback) 
		{
			mPostParticleUpdateCallback = postParticleUpdateCallback;
		}      
        
        // <Summary>
		// draw each particle to screen
		// </Summary>
		void drawParticles( SpriteBatch& sb, const std::vector<TexturePtr> textures, const Color& color, const int layer );
        
        // <Summary>
		// draw each particle to screen, a single texture
		// </Summary>
		void drawParticles( SpriteBatch& sb, const TexturePtr texture, const Color& color, const int layer );
        
		// <Summary>
		// draw each particle to screen, using single texture, and size scalar, and control over fade or not.
		// </Summary>
		void drawParticles( SpriteBatch& sb, const TexturePtr texture, const Color& color, const int layer, const float sizeFactor, bool doFade = true );
        
        
		// <Summary>
		// get the current size of a particle
		// </Summary>        
        Vector2 getParticleSize( const int index )
        {
        	Particle *p = &mParticles[index];
        	float progress = p->mOrigLifetime - p->mLifetime;
            
            //size up
        	if(progress < p->mSizeApexStart)
            {
            	if(!p->mSizeSlerpUp)
	            	return p->mStartSize.    lerp(p->mSize, progress/p->mSizeApexStart);
                else
                	return p->mStartSize.slerpout(p->mSize, progress/p->mSizeApexStart);
            }
            
            //apex
            else if(progress < p->mSizeApexEnd)
            {
	        	return p->mSize;
            }
            
            //size down
            else
            {
	          	if(!p->mSizeSlerpDown)
					return p->mSize.   lerp(p->mEndSize, (progress - p->mSizeApexEnd)/(p->mOrigLifetime - p->mSizeApexEnd) );            
                else
					return p->mSize.slerpin(p->mEndSize, (progress - p->mSizeApexEnd)/(p->mOrigLifetime - p->mSizeApexEnd) ); 
            }
            
        }
        
		
    public:
        std::vector<Particle>       mParticles;
        
    private:
		Color _getParticleColor( const Particle* p, const Color& c );
		
        std::list< int >			mUnusedParticles;
        
        int							mHighestLiveIndex;
		unsigned int				mNumLiveParticles;
		unsigned int				mNumParticles;
        float                       mOmegaDamping;
		VerletIntegrator			mIntegrator;
        
        CallbackPtr	mPostParticleUpdateCallback;

    };
}

#endif
