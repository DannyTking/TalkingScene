//
//  ParticleEmitter.h
//  WalaberEngine
//
//  Created by Jason Pecho on 1/3/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef _PARTICLE_EMITTER_H_
#define _PARTICLE_EMITTER_H_

#include <vector>
#include <cassert>
#include <utility>

#include <Node.h>
#include <Vector2.h>
#include <TextureManager.h>
#include <WalaberMath.h>
#include <Callback.h>
#include <ParticleSet.h>

namespace Walaber 
{
	class SpriteBatch;
	
	// TODO: Add particle emitter zone types - line, capsule, etc.
	// TODO: Make this a base class for different types of emitters
	// Emitter types - Burst, Continuous
	
	class ParticleEmitter : public Node
	{
	public: // structs, typedefs, enums
		
       
		struct EmitterZone
		{
			Vector2				mPosition;
			float				mRadius;
            Vector2				mRect;
            int					mRingParticles;     //instead of spawning one, we can spawn multiple pointing outwards
            float				mRingRadius;   //ring particles can start a distance away from the origin
            
            
            EmitterZone() : mRadius(0.0f), mRingParticles(1), mRingRadius(0.0f){}
		};
		
		struct ParticleEmitterLoadedParameters
		{
			ResultCode result;
			
			ParticleEmitterLoadedParameters(ResultCode r):
			result( r )
			{}
		};
		
		typedef std::pair< float, float >				FloatPair;
		
	public:
		//<Summary>
		// The big 2
		//</Summary>
		ParticleEmitter(const int maxParticles = 0); 
		~ParticleEmitter();
		
		//<Summary>
		// load the emitter data from xml
		//</Summary>
		void loadFromXML(const std::string& xmlFile, CallbackPtr callback = CallbackPtr());
				
		//<Summary>
		// update the emitter
		//</Summary>
		void update( float deltaTime );
		
		//<Summary>
		// draw particles in the emitter
		//</Summary>
		void draw( SpriteBatch* sb );
		
		//<Summary>
		// add a particle texture  to the rendered particles
		//</Summary>
		void addParticleTexture( const TexturePtr texture)
		{
			assert(texture);
			
			mParticleTextures.push_back(texture);
		}
		
		//<Summary>
		// set the base position of the emitter
		//</Summary>
		void addEmitterZone( const EmitterZone& emitterZone)
		{
			mEmitterZones.push_back(emitterZone);
		}
		
		//<Summary>
		// set the random size range
		//</Summary>
		void setSizeRange(const Vector2& minSize, const Vector2& maxSize)
		{
			//assert( minSize.X <= maxSize.X );
			//assert( minSize.Y <= maxSize.Y );
			mSizeRange = std::make_pair(minSize, maxSize);
		}
        
		//<Summary>
		// set the random pre-apex size range
		//</Summary>
		void setStartSizeRange(const Vector2& minSize, const Vector2& maxSize)
		{
			//assert( minSize.X <= maxSize.X );
			//assert( minSize.Y <= maxSize.Y );
			mStartSizeRange = std::make_pair(minSize, maxSize);
		}        
		
		//<Summary>
		// set the random grow to size range
		//</Summary>
		void setGrowToRange(const Vector2& minSize, const Vector2& maxSize)
		{
			//assert( minSize.X <= maxSize.X );
			//assert( minSize.Y <= maxSize.Y );
			mGrowToRange = std::make_pair(minSize, maxSize);
		}
		
		//<Summary>
		// Sets the random Direction range in radian
		//</Summary>
		void addDirectionRangeRadians(const float minDir, const float maxDir)
		{
			assert(minDir >= 0 && minDir < Walaber::TWO_PI);
			assert(maxDir >= 0 && maxDir < Walaber::TWO_PI);
			assert(minDir <= maxDir);
			mDirectionRanges.push_back( std::make_pair(minDir, maxDir) );
		}
		
		//<Summary>
		// Sets the random Direction range in degrees
		//</Summary>
		void addDirectionRangeDegrees(const float minDir, const float maxDir)
		{
			assert(minDir >= 0 && minDir < 360.0f);
			assert(maxDir >= 0 && maxDir < 360.0f);
			assert(minDir <= maxDir);
			mDirectionRanges.push_back( std::make_pair( Walaber::degToRad(minDir), Walaber::degToRad(maxDir) ) );
		}
		
		//<Summary>
		// Sets the random speed range
		//</Summary>
		void setSpeedRange(const float minSpeed, const float maxSpeed)
		{
			assert(minSpeed <= maxSpeed);
			mSpeedRange = std::make_pair(minSpeed, maxSpeed);
		}
		
		//<Summary>
		// Sets the random speed range
		//</Summary>
		void setScreenSpeedRange(const float minSpeed, const float maxSpeed)
		{
			assert(minSpeed <= maxSpeed);
			mScreenSpeedRange = std::make_pair(minSpeed, maxSpeed);
		}
		
		//<Summary>
		// Sets the random time between particles range
		//</Summary>
		void setParticlesPerSecond(const float minPPS, const float maxPPS)
		{
			assert(minPPS <= maxPPS);
			mParticlesPerSecondRange = std::make_pair(minPPS, minPPS);
		}
		
		//<Summary>
		// Sets the random mass range
		//</Summary>
		void setMassRange(const float minMass, const float maxMass)
		{
			assert(minMass <= maxMass);
			mMassRange = std::make_pair(minMass, maxMass);
		}
		
		//<Summary>
		// Sets the random angle range [0-360)
		//</Summary>
		void setAngleRange(const float minAngle, const float maxAngle)
		{
			assert(minAngle >= 0 && minAngle < 360.0f);
			assert(maxAngle >= 0 && maxAngle < 360.0f);
			assert(minAngle <= maxAngle);
			mAngleRange = std::make_pair(minAngle, maxAngle);
		}
		
		//<Summary>
		// Sets the random omega range (float)
		//</Summary>
		void setOmegaRange(const float minOmega, const float maxOmega)
		{
			assert(minOmega <= maxOmega);
			mOmegaRange = std::make_pair(minOmega, maxOmega);
		}
		
		//<Summary>
		// set the random life time range
		//</Summary>
		void setLifeTimeRange(const float minLifeTime, const float maxLifeTime)
		{
			assert(minLifeTime <= maxLifeTime);
			mLifeTimeRange = std::make_pair(minLifeTime, maxLifeTime);
		}
		
		//<Summary>
		// set the random fade time range
		//</Summary>
		void setFadeTimeRange(const float minFadeTime, const float maxFadeTime)
		{
			assert(minFadeTime <= maxFadeTime);
			mFadeTimeRange = std::make_pair(minFadeTime, maxFadeTime);
		}
		
		//<Summary>
		// set number of particles to spawn. -1 is infinite
		//</Summary>
		void setNumParticles(const int numParticles)
		{
			mNumParticles = numParticles;
		}
		
		//<Summary>
		// set whether the particles in this emitter use gravity
		//</Summary>
		void setParticlesUseGravity(const bool useGravity)
		{
			mParticlesUseGravity = useGravity;
		}
		
		//<Summary>
		// set number of particles to spawn. -1 is infinite
		//</Summary>
		void setActive(const bool active)
		{
			mActive = active;
		}
		
		//<Summary>
		// set whether or not the emitter is in screen space
		//</Summary>
		void setScreenSpace(const bool screenSpace)
		{
			if( screenSpace != mIsScreenSpace )
			{
				mMultiplier *= -1.0f;
				mIsScreenSpace = screenSpace;
			}
		}
		
		//<Summary>
		// set the non-random apex timespan as a percentage of lifetime.
		//</Summary>
		void setApexTimeSpan(const float startApexTime, const float endApexTime)
		{
			assert(startApexTime <= endApexTime);
			mApexTimeSpan = std::make_pair(startApexTime, endApexTime);
		}        
        
		//<Summary>
		// set whether we should slerp the size at the start/end
		//</Summary>        
        void setSizeSlerp(const bool startSlerp, const bool endSlerp)
        {
        	mSizeSlerpStart = startSlerp;
            mSizeSlerpEnd = endSlerp;
        }
        
		//<Summary>
		// returns whether active or not
		//</Summary>
		bool getActive()
		{
			return mActive;
		}
		
		
	private: // interface
		
		//<Summary>
		// callback for reading an xml file
		//</Summary>
		void _xmlFileLoaded(void* data);
						
	private:
		
		//<Summary>
		// Textures for the particles in this emitter
		//</Summary>
		std::vector< TexturePtr >				mParticleTextures;		
		
		//<Summary>
		// Base position of the emitter
		//</Summary>
		std::vector< EmitterZone >				mEmitterZones;
		
		//<Summary>
		// Range for the starting size of each particle
		//</Summary>
		std::pair< Vector2, Vector2 >			mSizeRange;
		
        
		//<Summary>
		// Range for the pre-apex size of each particle
		//</Summary>
		std::pair< Vector2, Vector2 >			mStartSizeRange;
                
        
		//<Summary>
		// Range for the size particles will grow to
		//</Summary>
		std::pair< Vector2, Vector2 >			mGrowToRange;
		
		//<Summary>
		// Range for angle (in radians) the emitter will give velocity to a particle towards
		//</Summary>
		std::vector< FloatPair >				mDirectionRanges;
		
		//<Summary>
		// Range for the speed at which particles will be emitted in world space
		//</Summary>
		FloatPair								mSpeedRange;
		
		//<Summary>
		// Range for the speed at which particles will be emitted in screen space
		//</Summary>
		FloatPair								mScreenSpeedRange;
		
		//<Summary>
		// Range for the time between particles generated
		//</Summary>
		FloatPair								mParticlesPerSecondRange;
		
		//<Summary>
		// Range for the mass of each particle
		//</Summary>
		FloatPair								mMassRange;
		
		//<Summary>
		// Range for the angle of each particle
		//</Summary>
		FloatPair								mAngleRange;
		
		//<Summary>
		// Range for particle omega
		//</Summary>
		FloatPair								mOmegaRange;
		
		//<Summary>
		// Range for how long the particle lasts
		//</Summary>
		FloatPair								mLifeTimeRange;
		
		//<Summary>
		// Range for how long into life does the particle start to fade
		//</Summary>
		FloatPair								mFadeTimeRange;
		
		//<Summary>
		// Force to apply for gravity
		//</Summary>
		Vector2									mGravityForce;

		//<Summary>
		// Non-random timespan for the particle size apex as a percentage of its lifetime.
		//</Summary>
		FloatPair								mApexTimeSpan;


		//<Summary>
		// Should we slerp size at the start
		//</Summary>
		bool									mSizeSlerpStart;

		//<Summary>
		// Should we slerp size at the end
		//</Summary>
		bool									mSizeSlerpEnd;

		//<Summary>
		// Callback for when the Emitter finishes loading
		//</Summary>
		CallbackPtr								mFinishedLoadingCallback;
		
		//<Summary>
		// Amount of time between each particle
		//</Summary>
		float									mCurrentTimePerParticle;	
		
		//<Summary>
		// Timer to keep track of when to create particles
		//</Summary>
		float									mParticleTimer;
		
		//<Summary>
		// multiplier for negating screen space values;
		//</Summary>
		float									mMultiplier;
		
		//<Summary>
		// Number of particles to be generated. -1 means infinte particles
		//</Summary>
		int										mNumParticles;
		
		//<Summary>
		// particle set to do particle physics
		//</Summary>
		ParticleSet								mParticleSet;
	
		//<Summary>
		// Should particles use gravity
		//</Summary>
		bool									mParticlesUseGravity;
		
		//<Summary>
		// True if active. if inactive, emitter will not emit
		//</Summary>
		bool									mActive;
		
		//<Summary>
		// Used to keep track of whether or not the emitter is in screenspace
		//</Summary>
		bool									mIsScreenSpace;
	};
}


#endif
