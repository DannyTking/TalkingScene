#ifndef _VECTOR_TOOLS_H
#define _VECTOR_TOOLS_H


#include "WalaberMath.h"
#include "Vector2.h"

#include <math.h>
#include <vector>
#include <string>


namespace Walaber 
{
	//! useful functions for working with Vector2 objects.
	class VectorTools
	{
	public:
		
		// --------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==
		/*! rotate a vector by the given angle (in radians) */
		static inline Vector2 rotatePointAroundPoint( const Vector2& point, const Vector2&pointToRotateAround, float angleRadians )
		{
			return pointToRotateAround + rotateVector( point - pointToRotateAround, angleRadians);
		}
		
		// --------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==
		/*! rotate a vector by the given angle (in radians) */
		static inline Vector2 rotateVector( const Vector2& vecIn, float angleRadians )
		{
            float c = (float)cosf(angleRadians);
            float s = (float)sinf(angleRadians);
			return Vector2((c * vecIn.X) - (s * vecIn.Y),(c * vecIn.Y) + (s * vecIn.X));
		}

		// --------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==
		/*! rotate a vector, given the cos and sin values for a given angle.
		 *  this function is a bit faster to use if you already have the sin and cos available,
		 *  or if you are going to rotate many vectors by the same angle, since it avoids calculating
		 *  sin and cos inside the function. */
		static inline Vector2 rotateVector( const Vector2& vecIn, float cosAngle, float sinAngle )
		{
			return Vector2((cosAngle * vecIn.X) - (sinAngle * vecIn.Y),(cosAngle * vecIn.Y) + (sinAngle * vecIn.X)) ;
		}

		// --------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==
		/*! given a starting vector, apply scale (scalar), rotation (radians), and translation (in that order) and return the output. */
		static inline Vector2 scaleRotateTranslateVector( const Vector2& vecIn, const float scale, const float angleRadians, const Vector2& translate )
		{
			Vector2 ret = vecIn;
			ret = rotateVector(ret, angleRadians);
#ifdef _APPLE_VDSP
			vDSP_vsma( (float*)&ret.X, 1, (float*)&scale, (float*)&translate.X, 1, (float*)&ret.X, 1, 2 );
#else
			ret *= scale;
			ret += translate;
#endif		
			return ret;
		}
		
		// --------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==
		/*! given a starting vector, apply scale (scalar), rotation (based on sin and cos of the angle), and translation (in that order) and return the output. */
		static inline Vector2 scaleRotateTranslateVector( const Vector2& vecIn, const float scale, const float cosAngle, const float sinAngle, const Vector2& translate )
		{
			Vector2 ret = vecIn;
			ret = rotateVector(ret, cosAngle, sinAngle);
			
#ifdef _APPLE_VDSP
			vDSP_vsma( (float*)&ret.X, 1, (float*)&scale, (float*)&translate.X, 1, (float*)&ret.X, 1, 2 );
#else
			ret *= scale;
			ret += translate;
#endif		
			return ret;
		}
		
		// --------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==
		/*! given a starting vector, apply scale (Vector2), rotation (radians), and translation (in that order) and return the output. */
		static inline Vector2 scaleRotateTranslateVector( const Vector2& vecIn, const Vector2& scale, const float angleRadians, const Vector2& translate )
		{
			Vector2 ret = vecIn * scale;
			ret = rotateVector(ret, angleRadians);
			ret += translate;
			return ret;
		}
		
		// --------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==
		/*! given a starting vector, apply scale (Vector2), rotation (sin and cos of the angle), and translation (in that order) and return the output. */
		static inline Vector2 scaleRotateTranslateVector( const Vector2& vecIn, const Vector2& scale, const float cosAngle, const float sinAngle, const Vector2& translate )
		{
			Vector2 ret = vecIn * scale;
			ret = rotateVector(ret, cosAngle, sinAngle);
			ret += translate;
			return ret;
		}
		
		// --------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==
		/*! reflect vector V about normal N and return the result. */
		static inline Vector2 reflectVector( const Vector2& V, const Vector2& N )
		{
			return Vector2(V - (N * (V.dotProduct(N) * 2.0f)));
		}

		// --------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==
		/*! reflect vector V about normal N and fill vOut with the result. */
		static inline void reflectVector(const Vector2& V, const Vector2& N, Vector2& vOut)
		{
			float dot = V.dotProduct(N);
            vOut = V - (N * (2.0f * dot));
		}

		// --------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==
		/*! returns true if facing along vector A and then turning to face along Vector B is a counter-clockwise turn. */
		static inline bool isCCW( const Vector2& A, const Vector2& B )
		{
			Vector2 perp = A.getPerpendicular();
			float dot = B.dotProduct(perp);
			return (dot >= 0.0f);
		}
        
        //------------------------------------------------------------------------------------------
        /*! returns the value of the angle between 2 vectors 
            (note: no knowledge of rotation direction)
        */
        static inline float getAngleBetweenVectors(Vector2 vectorA, Vector2 vectorB)
        {
            return acosf( (vectorA.dotProduct(vectorB)) / (vectorA.length()*vectorB.length()) );
        }
        

		// --------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==
		/*! returns true if pt2 is on the LEFT side of the line from pt0 -> pt1. */
		static inline float isLeft( const Vector2& pt0, const Vector2& pt1, const Vector2& pt2 )
		{
			return ( (pt1.X - pt0.X) * (pt2.Y - pt0.Y) - (pt2.X - pt0.X) * (pt1.Y - pt0.Y) );
		}

		// --------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==
		/*! look for intersection between line ptA -> ptB, and ptC -> ptD.
		 \param hitPt filled with the point of intersection (if any)
		 \param Ua returns value in range [0,1] along first line segment where intersection occurs.
		 \param Ub returns value in range [0,1] along second line segment where intersection occurs. */
		static bool lineIntersect( const Vector2& ptA, const Vector2& ptB, const Vector2& ptC, const Vector2& ptD,
								  Vector2& hitPt, float& Ua, float& Ub );

		// --------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==
		/*! look for intersection between line ptA -> ptB, and ptC -> ptD.
		 \param hitPt filled with the point of intersection (if any) */
		static bool lineIntersect( const Vector2& ptA, const Vector2& ptB, const Vector2& ptC, const Vector2& ptD,
								  Vector2& hitPt );

        // --------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==
		/*! given line segment from linePtA -> linePtB, and reference point pt, return the closest point to pt on the line segment.
		 \param outD filled with a value in range [0, 1] along the line segment representing location of the closest point. */
		static Vector2 closestPointOnLineSegment( const Vector2& linePtA, const Vector2& linePtB, const Vector2& pt, float& outD );
		
		// --------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==
		static float distanceBetweenLineSegments( const Vector2& A1, const Vector2& A2, 
												 const Vector2& B1, const Vector2& B2 );
		
		static float distanceBetweenLineSegmentsSquared( const Vector2& A1, const Vector2& A2, 
														const Vector2& B1, const Vector2& B2 );
		
		// --------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==
		/*! given line segment from linePtA -> linePtB, and reference point pt, return the distance from pt to the closest point on the line segment.
		 \param outLineDfilled with a value in range [0, 1] along the line segment representing location of the closest point. */
		static float distToLineSegment( const Vector2& linePtA, const Vector2& linePtB,
										 const Vector2& pt, float& outLineD );
        
		// --------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==
		/*! given line segment from linePtA -> linePtB (as well as the noramlized direction (lineDir) and normalized normal (lineNormal) and line length (lineLength), 
		 * and reference point pt, return the distance from pt to the closest point on the line segment.
		 * This function avoids normalizing and calculating the line normal internally, allowing the caller to pass in
		 * this information instead, when doing so would be beneficial for performance.
		 \param outLineD filled with a value in range [0, 1] along the line segment representing location of the closest point. */
		static float distToLineSegment( const Vector2& linePtA, const Vector2& linePtB, const Vector2& lineDir, const Vector2& lineNormal, float lineLength,
										 const Vector2& pt, float& outLineD );
        
		// --------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==
		static float distToLineSegmentSquared( const Vector2& linePtA, const Vector2& linePtB,
											  const Vector2& pt, float& outLineD );
        
		static float distToLineSegmentSquared( const Vector2& linePtA, const Vector2& linePtB, const Vector2& lineDir, const Vector2& lineNormal, float lineLength,
											  const Vector2& pt, float& outLineD );
        
		// --------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==
		/*! given a point at posA moving at velA, and another point at posB moving at velB, calculate and return the spring force
		 * between these points, given a spring rest distance of springD, a spring constant of springK, and a damping factor of damping.
		 \param posA position of first point.
		 \param velA velocity of first point.
		 \param posB position of second point.
		 \param velB velocity of second point.
		 \param springD spring rest distance.
		 \param springK spring force constant.
		 \param damping spring damping factor. */
		static Vector2 calculateSpringForce( const Vector2& posA, const Vector2& velA,
											const Vector2& posB, const Vector2& velB,
											float springD, float springK, float damping );

		// --------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==
		/*! given a normalized line (dir) and distance (length) between two point masses, and their respective velocities, calculate and return the spring force
		 * between these points, given a spring rest distance of springD, a spring constant of springK, and a damping factor of damping.
		 \param dir normalized vector pointing from ptA -> ptB
		 \param length length of the vector from ptA -> ptB (distance between points)
		 \param velA velocity of first point.
		 \param velB velocity of second point.
		 \param springD spring rest distance.
		 \param springK spring force constant.
		 \param damping spring damping factor. */
		static Vector2 calculateSpringForce( const Vector2& dir, float length, const Vector2& velA, const Vector2& velB,
											float springD, float springK, float damping );
		
		// --------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==
		/*! given a point at posA moving at velA, and another point at posB moving at velB, calculate and return the spring force
		 * between these points, given a spring rest distance of springD, a spring constant of springK, and a damping factor of damping.
		 * This version will return a force of zero if the points are closer than springD.
		 \param posA position of first point.
		 \param velA velocity of first point.
		 \param posB position of second point.
		 \param velB velocity of second point.
		 \param springD spring rest distance.
		 \param springK spring force constant.
		 \param damping spring damping factor. */
		static Vector2 calculateSpringForceAllowSlack( const Vector2& posA, const Vector2& velA,
													  const Vector2& posB, const Vector2& velB,
													  float springD, float springK, float damping );
													  
		
		// --------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==
		/*! given two points (and two handle points), interpolate along the bezier spline described by these points.
		 \param interp position along the spline to interpolate (should be in range [0,1] but is NOT clamped internally in this function.
		*/
		static inline Vector2 bezierInterp( const Vector2& ptA, const Vector2& handleA, const Vector2& ptB, const Vector2& handleB, float interp )
        {
            /*
             Bezier Equation:
             f(t) = p0*(1-x)^3 +
             3*p1*(1-x)^2*x +
             3*p2*(1-x)*x^2 +
             p3*x^3
             */
            float t_sqr = interp * interp;
            float t_cub = t_sqr * interp;
            
            // (1 - t) = "one minus t" = "omt"
            float omt = (1.0f - interp);
            float omt_sqr = omt * omt;
            float omt_cub = omt * omt_sqr;
            
            Vector2 A = ptA * (omt_cub);
            Vector2 B = handleA * (3.0f * omt_sqr * interp);
            Vector2 C = handleB * (3.0f * omt * t_sqr);
            Vector2 D = ptB * (t_cub);
            
            return A + B + C + D;
        }
	};
}
#endif
