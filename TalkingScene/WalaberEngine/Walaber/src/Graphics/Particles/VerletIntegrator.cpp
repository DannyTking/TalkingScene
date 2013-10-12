/*
 *  VerletIntegrator.cpp
 *  WaterConcept
 *
 *  Created by Jason Pecho on 2/22/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "VerletIntegrator.h"

#include <Vector2.h>

#ifdef __APPLE__
#include <TargetConditionals.h>
#if (TARGET_IPHONE_SIMULATOR == 0) && (TARGET_OS_IPHONE == 1) && (NO_VFP == 0)

#define USE_VFP 0

#else  

#define USE_VFP 0

#endif  // device / sim check
#endif  // APPLE


namespace Walaber
{
	const float VerletIntegrator::DAMPING = .0001f;
	
	// ----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==
    VerletIntegrator::VerletIntegrator():
	mDamping(DAMPING)
	{		
	}
	
	// ----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==
    VerletIntegrator::VerletIntegrator(const VerletIntegrator& tcvIntegrator):
	mDamping(tcvIntegrator.mDamping)
	{		
	}
	
	// ----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==
    VerletIntegrator& VerletIntegrator::operator=(const VerletIntegrator& tcvIntegrator)
	{
		mDamping= tcvIntegrator.mDamping;
		
		return *this;
	}
	
	// ----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==
    VerletIntegrator::~VerletIntegrator()
	{
	}
	
	// ----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==
//    void VerletIntegrator::integrateParticle(Vector2& position, Vector2& positionOld, Vector2& velocity,const Vector2& acceleration, const float& deltaTime)
//	{
//		//Time-Corrected Verlet:
//		//xi+1 = xi + (xi - xi-1) * (dti / dti-1) + a * dti * dti
//		
//		// break this up to prevent stalls
//		Vector2 oldPos = position;
//		
//		
//		Vector2 vel = (position - positionOld);
//		float damp = 1.0f-mDamping;
//		Vector2 accel = acceleration * deltaTime * deltaTime;
//		
//		position +=  vel*damp;
//		
//		// assign old position to break up instructions
//		positionOld =oldPos;
//		
//		// now add acceleration
//		position += accel;
//		
//		integrateVelocity(position, positionOld, velocity, deltaTime);
//	}
	
    // ----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==
//	void VerletIntegrator::integrateVelocity(Vector2& position, Vector2& positionOld, Vector2& velocity, const float& deltaTime)
//	{
//		// Calculate Velocity
//		velocity = (position - positionOld) / deltaTime;
//	}
    
    // ----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==
    void VerletIntegrator::integrateParticles(Vector2* positions, Vector2* positionsOld, Vector2* velocities, Vector2* accels, float deltaTime, int n) const
    {
        float damp = 1.0f - mDamping;
        float dtSqr = deltaTime * deltaTime;
        
/*#if USE_VFP
        
        // vel = (pos - posOld) * damp
        vDSP_vsbsm((float*)positions, 1, (float*)positionsOld, 1, &damp, (float*)velocities, 1, n * 2);
        
        // accel *= (deltaTime * deltaTime)
        vDSP_vsmul((float*)accels, 1, &dtSqr, (float*)accels, 1, n * 2);
        
        // posOld = pos
        memcpy(positionsOld, positions, n * sizeof(Vector2));
        
        // pos += vel
        vDSP_vadd((float*)positions, 1, (float*)velocities, 1, (float*)positions, 1, n * 2);                
        
        // pos += accel
        vDSP_vadd((float*)positions, 1, (float*)accels, 1, (float*)positions, 1, n * 2);
        

#else*/
        
        for (unsigned int i = 0; i < n; i++)
        {
            Vector2 oldPos = positions[i];
            
            Vector2 vel = positions[i] - positionsOld[i];
            
            Vector2 accel = accels[i] * dtSqr;
            
            positions[i] = positions[i] + vel * damp;
            
            positionsOld[i] = oldPos;
            
            positions[i] = positions[i] + accel;
        }
//#endif
        
        
        integrateVelocities(positions, positionsOld, velocities, deltaTime, n);
    }
    
    
    // ----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==----==
    void VerletIntegrator::integrateVelocities(Vector2* positions, Vector2* positionsOld, Vector2* velocities, float deltaTime, int n)const 
    {
        float invDeltaTime = 1.0f / deltaTime;
        
#if USE_VFP
        
        // accelerated version
        //vDSP_vsbsm( (float*)positions, 1, (float*)positionsOld, 1, &invDeltaTime, (float*)velocities, 1, n * 2);
        int loopCount = n >> 2;     // n / 4
        int remainder = n & 3;      // n % 4
        
        //printf("loopCount [%d], remainder [%d] = [%d]\n", loopCount, remainder, (loopCount*4)+remainder);
        
        // main loop, operates on 4 vectors at a time (8 floats at once)...
        /*
        for (unsigned int i = 0; i < loopCount; i++)
        {
            int base = i << 2;
            velocities[base+0] = (positions[base+0] - positionsOld[base+0]) * invDeltaTime;
            velocities[base+1] = (positions[base+1] - positionsOld[base+1]) * invDeltaTime;
            velocities[base+2] = (positions[base+2] - positionsOld[base+2]) * invDeltaTime;
            velocities[base+3] = (positions[base+3] - positionsOld[base+3]) * invDeltaTime;
        }
         */
        
        
        asm volatile ("fmrx     r0, fpscr               \n\t"       /* save old VFP settings */     \
                      "mov      r1, r0                  \n\t"       /* (saving into r1) */          \
                      "mov      r0, #0x00070000         \n\t"       /* set VFP to length of 8 */    \
                      "fmxr     fpscr, r0               \n\t"   \
                      \
                      "mov      r13, #0                 \n\t"       /* r13 holds loop counter */    \
                      "vmov     s1, %[invDT]            \n\t"       /* s1 now holds scalar */       \
                      \
                      "0:                               \n\t"       /* top of our loop */           \
                      \
                      "vldmia   %[pos], {s8-s15}        \n\t"       /* load 4 pos into s8-s15 */    \
                      "vldmia   %[posOld], {s16-s23}    \n\t"       /* loat 8 posOld into s16-23 */ \
                      \
                      "fsubs    s24, s8, s16            \n\t"       /* s24-31 = pos-posOld */       \
                      "fmuls    s24, s24, s1            \n\t"       /* s24-31 *= invDT */           \
                      \
                      "vstmia   %[vel], {s24-s31}       \n\t"       /* store s24-31 into [vel] */   \
                      \
                      "add      %[pos], %[pos], #32     \n\t"   \
                      "add      %[posOld], %[posOld], #32   \n\t"   \
                      "add      %[vel], %[vel], #32     \n\t"   \
                      \
                      "add      r13, r13, #1            \n\t"   \
                      "cmp      r13, %[n]               \n\t"   \
                      "blt      0b                      \n\t"       /* loop until we reach n */     \
                      \
                      "fmxr     fpscr, r1               \n\t"       /* restore VPF settings */      \
                      : \
                      : [vel] "r" (velocities), 
                        [pos] "r" (positions), 
                        [posOld] "r" (positionsOld), 
                        [invDT] "r" (invDeltaTime), 
                        [n] "r" (loopCount)
                      : "r0", "r1", "r13", \
                        "s1", \
                        "s8", "s9", "s10", "s11", "s12", "s13", "s14", "s15", \
                        "s16", "s17", "s18", "s19", "s20", "s21", "s22", "s23", \
                        "s24", "s25", "s26", "s27", "s28", "s29", "s30", "s31" \
                      );
        
        
        
        int base = loopCount << 2;
        for (unsigned int i = 0; i < remainder; i++)
            velocities[base+i] = (positions[base+i] - positionsOld[base+i]) * invDeltaTime;
        
        /*
        asm volatile ("fmrx     r0, fpscr               \n\t"   \
                      "mov      r1, r0                  \n\t"   \
                      "mov      r0, #0x00070000         \n\t"   \
                      "fmxr     fpscr, r0               \n\t"   \
                      \
                      "vldr     s1, [%[s]]              \n\t"   \
                      "vldmia   %[v], {s8-s15}          \n\t"   \
                      "fmuls    s8, s8, s1              \n\t"   \
                      \
                      "vstmia   %[v], {s8-s15}          \n\t"   \
                      \
                      "fmxr     fpscr, r1               \n\t"   \
                      : \
                      : [v] "r" (v), [s] "r" (s) \
                      : "r0", "r1", "s1", "s8", "s9", "s10", "s11", "s12", "s13", "s14", "s15" \
                      );
        */
#else
        // standard version
        for (unsigned int i = 0; i < n; i++)
        {
            velocities[i] = (positions[i] - positionsOld[i]) * invDeltaTime;
        }
#endif
    }
}
