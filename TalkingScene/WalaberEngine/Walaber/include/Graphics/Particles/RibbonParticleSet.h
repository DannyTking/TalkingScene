//
//  RibbonParticleSet.h
//  WalaberEngine
//
//  Created by Jason Pecho on 6/16/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef _RIBBON_PARTICLE_SET_H_
#define _RIBBON_PARTICLE_SET_H_

#include <RibbonParticle.h>
#include <vector>
#include <list>

#include <TextureManager.h>
#include <Color.h>
#include <SpriteBatch.h>
#include <VectorTools.h>
#include <VerletIntegrator.h>
#include <Callback.h>
   
namespace Walaber
{    
	template <int T>
    class RibbonParticleSet
    {
    public:
        struct PostUpdateParameters
        {
            RibbonParticleSet* ribbonParticleSet;
            
            PostUpdateParameters():
            ribbonParticleSet(0)
            {}
            
            PostUpdateParameters(RibbonParticleSet* parts):
            ribbonParticleSet(parts)
            {}
        };
        
    public:
        // <Summary>
		// create a set of particles
		// </Summary>
        RibbonParticleSet():
        mRibbonParticles(),
        mUnusedParticles(),        
        mHighestLiveIndex(0),
        mNumLiveParticles(0),
        mNumParticles(0),
        mOmegaDamping(0.0f),
        mIntegrator(),        
        mPostParticleUpdateCallback()
        {
        }
        
        // <Summary>
		// create and a init # of particles
		// </Summary>
        explicit RibbonParticleSet(unsigned int numParticles):
        mRibbonParticles(numParticles),
        mUnusedParticles(),        
        mHighestLiveIndex(0),
        mNumLiveParticles(0),
        mNumParticles(numParticles),
        mOmegaDamping(0.0f),
        mIntegrator(),        
        mPostParticleUpdateCallback()
        {
            for(int i = 0; i < numParticles; ++i)
            {
                mUnusedParticles.push_back(i);
            }
			
			// Ribbon history information
			// 2 for double endpoints, 2 for drawing the original height of the sprite
			mVerts = T * 2 + 2;
			mVertPositions = new Vector2[mVerts];
			mVertUVs = new Vector2[mVerts];
			mVertColors = new Color[mVerts];
        }
        
        // <Summary>
		// dtor
		// </Summary>
        ~RibbonParticleSet()
        {
			if (mVertPositions)
			{
				delete[] mVertPositions;
			}
			
			if (mVertUVs)
			{
				delete[] mVertUVs;
			}
			
			if (mVertColors)
			{
				delete[] mVertColors;
			}
		}
        
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
                Logger::printf("Walaber", Logger::SV_INFO, "No more free particles!!!\n");
                outIndex = -1;
                return false;
            }
            
            // get next unused particle index
            int j = mUnusedParticles.front();
            mUnusedParticles.pop_front();
            
            mRibbonParticles[j].mMass = mass;
            mRibbonParticles[j].mInvMass = (1.0f/mass);
            mRibbonParticles[j].mAngleDeg = angleDeg;
            mRibbonParticles[j].mPosition = position;			
            mRibbonParticles[j].mPositionOld = position;
            mRibbonParticles[j].mVelocity = Vector2::Zero;
            mRibbonParticles[j].mForceAccum = Vector2::Zero;
            mRibbonParticles[j].mSize = size;
            
            mRibbonParticles[j].mLifetime = -1.0f;
            mRibbonParticles[j].mOrigLifetime = -1.0f;
            mRibbonParticles[j].mOmega = 0.0f;
            mRibbonParticles[j].mTextureIndex = 0;
			
			for (unsigned int i = 0; i < mRibbonParticles[j].getNumOldFramePositions(); i++)
				mRibbonParticles[j].mPositionOldFrame[i] = position;

            
            mRibbonParticles[j].mAlive = true;
            
            // Default values
            mRibbonParticles[j].mVisible = true;
            mRibbonParticles[j].mUseGravity = true;
            mRibbonParticles[j].mEndSize = mRibbonParticles[j].mSize;
            mRibbonParticles[j].mFadeLength = 0.5f;
            mRibbonParticles[j].mRotationPerUpdate = 0.0f;
            
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
        inline bool addParticle(const float mass, const Vector2& position, const Vector2& positionOld, const float angleDeg, const Vector2& size,
                                const Vector2& velocity, const Vector2& forces, int& outIndex)
        {
            if(mUnusedParticles.empty())
            {
                Logger::printf("Walaber", Logger::SV_INFO, "No more free particles!!!\n");
                outIndex = -1;
                return false;
            }
            
            // get next unused particle index
            int j = mUnusedParticles.front();
            mUnusedParticles.pop_front();
            
            mRibbonParticles[j].mMass = mass;
            mRibbonParticles[j].mInvMass = (1.0f/mass);
            mRibbonParticles[j].mAngleDeg = angleDeg;
            mRibbonParticles[j].mPosition = position;			
            mRibbonParticles[j].mPositionOld = positionOld;
            mRibbonParticles[j].mVelocity = velocity;
            mRibbonParticles[j].mForceAccum = forces;
            mRibbonParticles[j].mSize = size;
            
            mRibbonParticles[j].mLifetime = -1.0f;
            mRibbonParticles[j].mOrigLifetime = -1.0f;
            mRibbonParticles[j].mOmega = 0.0f;
            mRibbonParticles[j].mTextureIndex = 0;
            
			for (unsigned int i = 0; i < mRibbonParticles[j].getNumOldFramePositions(); i++)
				mRibbonParticles[j].mPositionOldFrame[i] = position;
			

            mRibbonParticles[j].mAlive = true;
            
            // Default values
            mRibbonParticles[j].mVisible = true;
            mRibbonParticles[j].mUseGravity = true;
            mRibbonParticles[j].mEndSize = mRibbonParticles[j].mSize;
            mRibbonParticles[j].mFadeLength = 0.5f;
            mRibbonParticles[j].mRotationPerUpdate = 0.0f;
            
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
        inline bool addParticle(const float mass, const Vector2& position, const Vector2& positionOld, const float angleDeg, const Vector2& size,
                                const Vector2& velocity, const Vector2& forces, const float newLifetime, const float newOmega, const int newTextureIndex, int& outIndex)
        {
            if(mUnusedParticles.empty())
            {
                Logger::printf("Walaber", Logger::SV_INFO, "No more free particles!!!\n");
                outIndex = -1;
                return false;
            }
            
            // get next unused particle index
            int j = mUnusedParticles.front();
            mUnusedParticles.pop_front();
            
            mRibbonParticles[j].mMass = mass;
            mRibbonParticles[j].mInvMass = (1.0f/mass);
            mRibbonParticles[j].mAngleDeg = angleDeg;
            mRibbonParticles[j].mPosition = position;			
            mRibbonParticles[j].mPositionOld = positionOld;
            mRibbonParticles[j].mVelocity = velocity;
            mRibbonParticles[j].mForceAccum = forces;
            mRibbonParticles[j].mSize = size;
            
            mRibbonParticles[j].mLifetime = newLifetime;
            mRibbonParticles[j].mOrigLifetime = newLifetime;
            mRibbonParticles[j].mOmega = newOmega;
            mRibbonParticles[j].mTextureIndex = newTextureIndex;
            
			for (unsigned int i = 0; i < mRibbonParticles[j].getNumOldFramePositions(); i++)
				mRibbonParticles[j].mPositionOldFrame[i] = position;
			

            mRibbonParticles[j].mAlive = true;
            
            // Default values
            mRibbonParticles[j].mVisible = true;
            mRibbonParticles[j].mUseGravity = true;
            mRibbonParticles[j].mEndSize = mRibbonParticles[j].mSize;
            mRibbonParticles[j].mFadeLength = 0.5f;
            mRibbonParticles[j].mRotationPerUpdate = 0.0f;
            
            ++mNumLiveParticles;
            
            if(j > mHighestLiveIndex)
            {
                mHighestLiveIndex = j;
            }
            
            outIndex = j;
            return true;
        }
        
        // <Summary>
		// set the damping of the integrator
		// </Summary>
        void setParticleDamping(const float damping)
        {
            mIntegrator.setDamping(damping);
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
            
            mRibbonParticles[index].mAngleDeg = angleDeg;
        }
        
        // <Summary>
		// set the size of a particle
		// </Summary>
        inline void setParticleSize(const int index, const Vector2& size)
        {
            assert(index < mNumParticles);
            
            mRibbonParticles[index].mSize = size;
        }
        
        // <Summary>
		// set the lifetime of a particle
		// </Summary>
        inline void setParticleLifetime(const int index, const float newLifetime)
        {
            assert(index < mNumParticles);
            
            mRibbonParticles[index].mLifetime = newLifetime;
        }
        
        // <Summary>
		// set the damping of the SET of particles
		// </Summary>
        inline void setRibbonParticleSetOmegaDamping(const float newOmegaDamping)
        {
            mOmegaDamping = newOmegaDamping;
        }
        
        // <Summary>
		// set the texture index of a particle
		// </Summary>
        inline void setParticleTextureIndex(const int index, const int newTextureIndex)
        {
            assert(index < mNumParticles);
            
            mRibbonParticles[index].mTextureIndex= newTextureIndex;
        }
        
        // <Summary>
		// set whether the particle uses gravity or not
		// </Summary>
        inline void setParticleUseGravity(const int index, const bool newUseGravity)
        {
            assert(index < mNumParticles);
            
            mRibbonParticles[index].mUseGravity = newUseGravity;
        }
        
        // <Summary>
		// set the end size of a particle
		// </Summary>
        inline void setParticleEndSize(const int index, const Vector2 newEndSize)
        {
            assert(index < mNumParticles);
            
            mRibbonParticles[index].mEndSize = newEndSize;
        }
        
        // <Summary>
		// set the fade length of a particle
		// </Summary>
        inline void setParticleFadeLength(const int index, const float newFadeLength)
        {
            assert(index < mNumParticles);
            
            mRibbonParticles[index].mFadeLength = newFadeLength;
        }
        
        // <Summary>
        // kill a particle
        // </Summary>
        void removeParticle(int index) 
        { 			
            if(mRibbonParticles[index].mAlive)
            {
                mRibbonParticles[index].mAlive = false;
                --mNumLiveParticles;
                
                // add to recycling list
                mUnusedParticles.push_front(index);
                
                if(index && index == mHighestLiveIndex)
                {
                    int i = index-1;
                    
                    // iterate backwards from the dead particle to find the largest alive particle
                    while(i > 0 && !mRibbonParticles[i].mAlive)
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
        // returns the highest index of an alive particle
        // </Summary>
        inline unsigned int getHighestLiveIndex()const { return mHighestLiveIndex; }		
        
        // <Summary>
        // get the index of the highest alive particle
        // </Summary>
        inline void setPostParticleUpdateCallback(CallbackPtr postParticleUpdateCallback) 
        {
            mPostParticleUpdateCallback = postParticleUpdateCallback;
        }
        
        // <Summary>
		// draw each particle to screen lerping between colors based on velocity
		// </Summary>
        void drawParticles( SpriteBatch& sb, const TexturePtr texture, const Color& color, const int layer );
        
        // <Summary>
		// draw each particle to screen lerping between particles NOTE: valueArray is ognored right now infavor Density
		// </Summary>
        void drawParticles( SpriteBatch& sb, const TexturePtr texture, const Color& color, const Color& color2, const int layer, float* valueArray, float minV, float maxV );
        
    public:
	
        std::vector< RibbonParticle<T> >	mRibbonParticles;
        std::list< int >					mUnusedParticles;
        
        unsigned int				mHighestLiveIndex;
        unsigned int				mNumLiveParticles;
        unsigned int				mNumParticles;
        float                       mOmegaDamping;
        VerletIntegrator			mIntegrator;
		
		int							mVerts;
		Vector2*					mVertPositions;
		Vector2*					mVertUVs;
		Color*						mVertColors;
        
        CallbackPtr                 mPostParticleUpdateCallback;
    };
	
    //=====-----=======---------=========---------==========---------=========----------=============
	template <int T>
    void RibbonParticleSet<T>::initParticles(unsigned int numParticles)
    {
        mRibbonParticles.insert(mRibbonParticles.begin(), numParticles, RibbonParticle<10>());
        
        // unroll for optimizing
        for(int i = 0; i < numParticles; ++i)
        {
            mUnusedParticles.push_back(i);
        }
        
        mNumParticles = numParticles;
    }
	
	//=====-----=======---------=========---------==========---------=========----------=============
	template <int T>
    void RibbonParticleSet<T>::updateParticles(const float deltaTime)
    {		
        for(int i = 0; i <= mHighestLiveIndex; ++i)
        {
            if(!mRibbonParticles[i].mAlive)
            {
                continue;
            }
			
			if (mRibbonParticles[i].mLifetime > 0) 
			{
				mRibbonParticles[i].mLifetime -= deltaTime;
				
				if (mRibbonParticles[i].mLifetime <= 0)
				{
					removeParticle(i);
					continue;
				}
			}
			
			//            // Check to see if the particle has expired
			//            if (mRibbonParticles[i].mLifetime > 0) 
			//            {
			//                mRibbonParticles[i].mLifetime -= deltaTime;
			//            }
			//            
			//            if (mRibbonParticles[i].mOmega != 0) 
			//            {
			//                mRibbonParticles[i].mOmega *= mOmegaDamping;
			//                mRibbonParticles[i].mAngleDeg += mRibbonParticles[i].mOmega;
			//            }
			//            
			//            
            mRibbonParticles[i].mForceAccum = mRibbonParticles[i].mForceAccum * mRibbonParticles[i].mInvMass;
            
            // update particles
            mIntegrator.integrateParticle(mRibbonParticles[i].mPosition, mRibbonParticles[i].mPositionOld, mRibbonParticles[i].mVelocity, mRibbonParticles[i].mForceAccum, deltaTime);
            
            memset( &mRibbonParticles[i].mForceAccum, 0, sizeof(Vector2) );              
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
	template <int T>
    void RibbonParticleSet<T>::drawParticles( SpriteBatch& sb, const TexturePtr texture, const Color& color, const int layer )
    {
        for(int i = 0; i <= mHighestLiveIndex; ++i)
        {
			/*
            if(!mRibbonParticles[i].mAlive)
            {
                continue;
            }
            
            float vl = mRibbonParticles[i].mVelocity.fastLength();
            Color c = color.lerp(color2, (vl - minV) / vScale);
            
            sb.drawQuad(layer, texture, mRibbonParticles[i].mPosition + VectorTools::rotateVector(mRibbonParticles[i].mOffset, Walaber::degToRad(mRibbonParticles[i].mAngleDeg)), Walaber::degToRad(mRibbonParticles[i].mAngleDeg), mRibbonParticles[i].mEndSize.lerp(mRibbonParticles[i].mSize, mRibbonParticles[i].mLifetime / mRibbonParticles[i].mOrigLifetime), c);
            */
			
			
			if(!mRibbonParticles[i].mAlive)
			{
				continue;
			}
			
			if (mRibbonParticles[i].mLifetime == -1.0f) 
			{
				sb.drawQuad(layer, texture, mRibbonParticles[i].mPosition + VectorTools::rotateVector(mRibbonParticles[i].mOffset, Walaber::degToRad(mRibbonParticles[i].mAngleDeg)), Walaber::degToRad(mRibbonParticles[i].mAngleDeg), mRibbonParticles[i].mSize, color);
			}
			else 
			{
				// We need to create the list of points for drawing verts in a triangle strip
				int curVert = 1;
				
				Color fadeOut = color * lerp(0.0f, 1.0f, mRibbonParticles[i].mLifetime / mRibbonParticles[i].mFadeLength);
                
				for (int j = 0; j < mRibbonParticles[i].getNumOldFramePositions(); j++)
				{
					// Determine the line from the next position to the current position
					Vector2 curPos = mRibbonParticles[i].mPositionOldFrame[j];
					Vector2 nextPos;
					
					if (j == mRibbonParticles[i].getNumOldFramePositions() - 1)
					{
						nextPos = curPos;
						curPos = mRibbonParticles[i].mPositionOldFrame[j - 1];
					}
					else
					{
						nextPos = mRibbonParticles[i].mPositionOldFrame[j + 1];
					}
					
					
					Vector2 normalizedLineDir = ( curPos - nextPos ).fastNormalisedCopy();
					Vector2 normalizedPerp = normalizedLineDir.getPerpendicular();
					
					// Center
					Vector2 center = mRibbonParticles[i].mPositionOldFrame[j] + VectorTools::rotateVector(mRibbonParticles[i].mOffset, Walaber::degToRad(mRibbonParticles[i].mAngleDeg));
					
					// 'Right' pos (width / 2)
					mVertPositions[curVert] = center;
					mVertPositions[curVert] += normalizedPerp * mRibbonParticles[i].mSize.X * 0.5f;
					
					// Front/back get extended to height of particle
					if (j == 0)
						mVertPositions[curVert] += normalizedLineDir * mRibbonParticles[i].mSize.Y * 0.5f;
					else if (j == mRibbonParticles[i].getNumOldFramePositions() - 1)
						mVertPositions[curVert] += normalizedLineDir * mRibbonParticles[i].mSize.Y * -0.5f;
					
					mVertUVs[curVert] = Vector2( 1.0f, j / (mRibbonParticles[i].getNumOldFramePositions() - 1.0f) );
					mVertColors[curVert] = fadeOut;
					
					curVert++;
					
					// 'Left' pos (-width / 2)
					mVertPositions[curVert] = center;
					mVertPositions[curVert] += normalizedPerp * mRibbonParticles[i].mSize.X * -0.5f;
					
					// Front/back get extended to height of particle
					if (j == 0)
						mVertPositions[curVert] += normalizedLineDir * mRibbonParticles[i].mSize.Y * 0.5f;
					else if (j == mRibbonParticles[i].getNumOldFramePositions() - 1)
						mVertPositions[curVert] += normalizedLineDir * mRibbonParticles[i].mSize.Y * -0.5f;
					
					mVertUVs[curVert] = Vector2( 0.0f, j / (mRibbonParticles[i].getNumOldFramePositions() - 1.0f) );
					mVertColors[curVert] = fadeOut;
					
					curVert++;
				}
				
				// Double the very first and very last verts
				mVertPositions[0] = mVertPositions[1];
				mVertPositions[mVerts - 1] = mVertPositions[mVerts - 2];
				
				mVertUVs[0] = mVertUVs[1];
				mVertUVs[mVerts - 1] = mVertUVs[mVerts - 2];
				
				mVertColors[0] = fadeOut;
				mVertColors[mVerts - 1] = fadeOut;
				
				// Finally, hand it all over
				sb.drawVerts(layer, texture, 
							 mVerts, false, 
							 mVertPositions, mVertUVs, mVertColors	);
			}
        }
    }
    
    //=====-----=======---------=========---------==========---------=========----------=============
	template <int T>
    void RibbonParticleSet<T>::drawParticles( SpriteBatch& sb, const TexturePtr texture, const Color& color, const Color& color2, const int layer, 
											  float* valueArray, float minV, float maxV )
    {
        RibbonParticle<10>* partArray = &mRibbonParticles[0];
        RibbonParticle<10>* currPart = partArray; ++partArray;
        
        for(int i = 0; i <= mHighestLiveIndex; ++i)
        {
            RibbonParticle<10>* nextPart = partArray; ++partArray;
            
            if(!(*currPart).mAlive)
            {
                currPart = nextPart;
                continue;
            }
            
            //sb.drawQuad(layer, texture, mPartPositions[i], Walaber::degToRad(mPartAngleDeg[i]), mPartSize[i], c);
            //sb.drawCircle( layer, texture, mPartPositions[i], mPartSize[i].X * 0.5f, c);
            sb.drawTeardrop( layer, texture, (*currPart).mPosition, (*currPart).mPositionOldFrame[1], (*currPart).mSize.X * 0.5f, Color::White );
            
            currPart = nextPart;
        }
    }
}
    
#endif