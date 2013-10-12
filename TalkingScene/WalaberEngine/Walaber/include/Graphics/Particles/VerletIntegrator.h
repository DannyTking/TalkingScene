/*
 *  VerletIntegrator.h
 *  WaterConcept
 *
 *  Created by Jason Pecho on 2/22/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _VERLET_INTEGRATOR_H_
#define _VERLET_INTEGRATOR_H_

#include <Vector2.h>

namespace Walaber
{
	class Particles;
	
    // ----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==
    // ----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==
	// Time Corrected Verlet Integrator
	class VerletIntegrator
	{
	public:
        // ----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==
		VerletIntegrator();
		VerletIntegrator(const VerletIntegrator& tcvIntegrator);
		VerletIntegrator& operator=(const VerletIntegrator& tcvIntegrator);
		~VerletIntegrator();
		
        // ----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==
		inline void integrateParticle(Vector2& position, Vector2& positionOld, Vector2& velocity, const Vector2& acceleration, const float& deltaTime) const
        {
            //Time-Corrected Verlet:
            //xi+1 = xi + (xi - xi-1) * (dti / dti-1) + a * dti * dti
            
            // break this up to prevent stalls
            Vector2 oldPos = position;
            
            
            Vector2 vel = (position - positionOld);
            float damp = 1.0f-mDamping;
            Vector2 accel = acceleration * deltaTime * deltaTime;
            
            position +=  vel*damp;
            
            // assign old position to break up instructions
            positionOld =oldPos;
            
            // now add acceleration
            position += accel;
            
            integrateVelocity(position, positionOld, velocity, deltaTime);
        }
        
        // ----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==
		inline void integrateVelocity(Vector2& position, Vector2& positionOld, Vector2& velocity, const float& deltaTime) const
        {
            // Calculate Velocity
            velocity = (position - positionOld) / deltaTime;
        }        
        
        // ----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==
		// ----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==
		void integrateParticles(Vector2* positions, Vector2* positionsOld, Vector2* velocities, Vector2* accels, float deltaTime, int n) const;
        
        // ----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==
        void integrateVelocities(Vector2* positions, Vector2* positionsOld, Vector2* velocities, float deltaTime, int n) const;
        
        // ----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==
        void setDamping(const float damping)
        {
            mDamping = damping;
        }
        
	private:
		// ----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==
		float		mDamping;
		
		const static float DAMPING;
	};
}

#endif
