#include "VectorTools.h"


namespace Walaber 
{
	// --------========--------========--------========--------========--------========--------========--------========--------========
	bool VectorTools::lineIntersect( const Vector2& ptA, const Vector2& ptB, const Vector2& ptC, const Vector2& ptD,
							  Vector2& hitPt, float& Ua, float& Ub )
	{
		hitPt = Vector2::Zero;
		Ua = 0.0f;
		Ub = 0.0f;
		float denom = ((ptD.Y - ptC.Y) * (ptB.X - ptA.X)) - ((ptD.X - ptC.X) * (ptB.Y - ptA.Y));

		// if denom == 0, lines are parallel - being a bit generous on this one..
		if (absf(denom) < 0.000001f)
			return false;

		float UaTop = ((ptD.X - ptC.X) * (ptA.Y - ptC.Y)) - ((ptD.Y - ptC.Y) * (ptA.X - ptC.X));
		float UbTop = ((ptB.X - ptA.X) * (ptA.Y - ptC.Y)) - ((ptB.Y - ptA.Y) * (ptA.X - ptC.X));

		Ua = UaTop / denom;
		Ub = UbTop / denom;

		if ((Ua >= 0.0f) && (Ua <= 1.0f) && (Ub >= 0.0f) && (Ub <= 1.0f))
		{
			// these lines intersect!
			hitPt = ptA + ((ptB - ptA) * Ua);
			return true;
		}
		return false;
	}

	// --------========--------========--------========--------========--------========--------========--------========--------========
	bool VectorTools::lineIntersect( const Vector2& ptA, const Vector2& ptB, const Vector2& ptC, const Vector2& ptD,
							  Vector2& hitPt )
	{
		float Ua, Ub;
		return lineIntersect( ptA, ptB, ptC, ptD, hitPt, Ua, Ub );
	}
    
    // --------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==
    Vector2 VectorTools::closestPointOnLineSegment( const Vector2& linePtA, const Vector2& linePtB, const Vector2& pt, float& outD )
    {
        Vector2 d = (linePtB - linePtA);
        float l = d.normalise();
        
        float x = d.dotProduct((pt - linePtA));
        
        if (x <= 0.0f)
        {
            outD = 0.0f;
            return linePtA;
        }
        else if ( x >= l)
        {
            outD = 1.0f;
            return linePtB;
        }
        else
        {
            outD = x / l;
            return linePtA + (d * x);
        }        
    }
	
	// --------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==
	float VectorTools::distanceBetweenLineSegments( const Vector2& A1, const Vector2& A2, const Vector2& B1, const Vector2& B2 )
	{
		// method - find the intersection of these 2 lines (if any), and then constrain to the limits of the
		// line segments. and find the shortest distance.
		
		Vector2 hitPt;
		float uA, uB;
		
		bool intersect = lineIntersect(A1, A2, B1, B2, hitPt, uA, uB);
		if (!intersect)
		{
			// (approximately) parallel lines, just return distance from A1 -> B1.
			return (B1 - A1).length();
		}
		
		if (isInRangeExclusive(uA, 0.0f, 1.0f) && isInRangeExclusive(uB, 0.0f, 1.0f))
		{
			// intersection is within both segment limits.  distance is zero!
			return 0.0f;
		}
		
		// otherwise, we need to try 4 possible combinations and return the minimum result.
		float outD;
		Vector2 aDir = (A2 - A1);
		float aL = aDir.normalise();
		Vector2 aN = aDir.getPerpendicular();
		
		Vector2 bDir = (B2 - B1);
		float bL = bDir.normalise();
		Vector2 bN = aDir.getPerpendicular();
		
		
		// test distance from B1 -> line A
		float ret = distToLineSegment(A1, A2, aDir, aN, aL, B1, outD);
		
		// test distance from B2 -> line A
		ret = minf( ret, distToLineSegment(A1, A2, aDir, aN, aL, B2, outD) );
		
		// test distance from A1 -> line B
		ret = minf( ret, distToLineSegment(B1, B2, bDir, bN, bL, A1, outD) );
		
		// test distance from A2 -> line B
		ret = minf( ret, distToLineSegment(B1, B2, bDir, bN, bL, A2, outD) );
		
		return ret;		
	}
	
	// --------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==
	float VectorTools::distanceBetweenLineSegmentsSquared( const Vector2& A1, const Vector2& A2, const Vector2& B1, const Vector2& B2 )
	{
		// method - find the intersection of these 2 lines (if any), and then constrain to the limits of the
		// line segments. and find the shortest distance.
		
		Vector2 hitPt;
		float uA, uB;
		
		bool intersect = lineIntersect(A1, A2, B1, B2, hitPt, uA, uB);
		if (!intersect)
		{
			// (approximately) parallel lines, just return distance from A1 -> B1.
			return (B1 - A1).lengthSquared();
		}
		
		if (isInRangeExclusive(uA, 0.0f, 1.0f) && isInRangeExclusive(uB, 0.0f, 1.0f))
		{
			// intersection is within both segment limits.  distance is zero!
			return 0.0f;
		}
		
		// otherwise, we need to try 4 possible combinations and return the minimum result.
		float outD;
		Vector2 aDir = (A2 - A1);
		float aL = aDir.normalise();
		Vector2 aN = aDir.getPerpendicular();
		
		Vector2 bDir = (B2 - B1);
		float bL = bDir.normalise();
		Vector2 bN = aDir.getPerpendicular();
		
		
		// test distance from B1 -> line A
		float ret = distToLineSegmentSquared(A1, A2, aDir, aN, aL, B1, outD);
		
		// test distance from B2 -> line A
		ret = minf( ret, distToLineSegmentSquared(A1, A2, aDir, aN, aL, B2, outD) );
		
		// test distance from A1 -> line B
		ret = minf( ret, distToLineSegmentSquared(B1, B2, bDir, bN, bL, A1, outD) );
		
		// test distance from A2 -> line B
		ret = minf( ret, distToLineSegmentSquared(B1, B2, bDir, bN, bL, A2, outD) );
		
		return ret;		
	}
	
	
	// --------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==
	float VectorTools::distToLineSegment( const Vector2& linePtA, const Vector2& linePtB ,
									 const Vector2& pt, float& outLineD )
	{
		Vector2 lineDir = (linePtB - linePtA);
		float lineLength = lineDir.normalise();
		Vector2 lineNorm = lineDir.getPerpendicular();
		
		return distToLineSegment(linePtA, linePtB, lineDir, lineNorm, lineLength, pt, outLineD);
	}
	
	// --------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==
    float VectorTools::distToLineSegment( const Vector2& linePtA, const Vector2& linePtB, 
										 const Vector2& lineDir, const Vector2& lineNormal, float lineLength,
									 const Vector2& pt,float& outLineD )
	{
		Vector2 toPt = (pt - linePtA);
		
		float x = lineDir.dotProduct(toPt);

		if (x <= 0.0f)
		{
			outLineD = 0.0f;
			return (toPt.length());
		}
		else if (x >= lineLength)
		{
			outLineD = 1.0f;
			return (pt - linePtB).length();
		}
		else
		{
			outLineD = x / lineLength;
			return lineNormal.dotProduct( toPt );
		}
	}
	
	// --------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==
	float VectorTools::distToLineSegmentSquared( const Vector2& linePtA, const Vector2& linePtB ,
												const Vector2& pt, float& outLineD )
	{
		Vector2 lineDir = (linePtB - linePtA);
		float lineLength = lineDir.normalise();
		Vector2 lineNorm = lineDir.getPerpendicular();
		
		return distToLineSegmentSquared(linePtA, linePtB, lineDir, lineNorm, lineLength, pt, outLineD);
	}
	
	// --------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==
    float VectorTools::distToLineSegmentSquared( const Vector2& linePtA, const Vector2& linePtB, 
												const Vector2& lineDir, const Vector2& lineNormal, float lineLength,
												const Vector2& pt,float& outLineD )
	{
		Vector2 toPt = (pt - linePtA);
		
		float x = lineDir.dotProduct(toPt);
		
		if (x <= 0.0f)
		{
			outLineD = 0.0f;
			return (toPt.lengthSquared());
		}
		else if (x >= lineLength)
		{
			outLineD = 1.0f;
			return (pt - linePtB).lengthSquared();
		}
		else
		{
			outLineD = x / lineLength;
			float d = lineNormal.dotProduct( toPt );

			return d * d;
		}
	}

	// --------========--------========--------========--------========--------========--------========--------========--------========
	Vector2 VectorTools::calculateSpringForce( const Vector2& posA, const Vector2& velA,
										const Vector2& posB, const Vector2& velB,
										float springD, float springK, float damping)
	{
		Vector2 BtoA = (posA - posB);
		float dist = BtoA.length();

		if (dist > 0.0001f)
			BtoA /= dist;
		else
			BtoA = Vector2::Zero;

		dist = springD - dist;

		Vector2 relVel = velA - velB;
		float totalRelVel = relVel.dotProduct(BtoA);

		//printf("calculateSpringForce: dist[%f] k[%f]  totalRelVel[%f] damp[%f]\n", dist, springK, totalRelVel, damping);
		return BtoA * ((dist * springK) - (totalRelVel * damping));  
	}

	// --------========--------========--------========--------========--------========--------========--------========--------========
	Vector2 VectorTools::calculateSpringForce( const Vector2& dir, float length, const Vector2& velA, const Vector2& velB,
											  float springD, float springK, float damping )
	{
		float dist = springD - length;

		Vector2 relVel = velA - velB;
		float totalRelVel = relVel.dotProduct(dir);

		//printf("calculateSpringForce: dist[%f] k[%f]  totalRelVel[%f] damp[%f]\n", dist, springK, totalRelVel, damping);
		return dir * ((dist * springK) - (totalRelVel * damping)); 
	}
	
	// --------========--------========--------========--------========--------========--------========--------========--------========
	Vector2 VectorTools::calculateSpringForceAllowSlack( const Vector2& posA, const Vector2& velA,
											  const Vector2& posB, const Vector2& velB,
											  float springD, float springK, float damping)
	{
		Vector2 BtoA = (posA - posB);
		float dist = BtoA.length();
		
		// not a slack situation, so continue calculating the force...
		if (dist > 0.0001f)
			BtoA /= dist;
		else
			BtoA = Vector2::Zero;
		
		dist = springD - dist;
		
		if (dist > 0.0f)
		{
			// this is a "slack" situation (we're closer to each other than the goal distance.  in this case the force is ZERO!
			return Vector2::Zero;
		}
		
		Vector2 relVel = velA - velB;
		float totalRelVel = relVel.dotProduct(BtoA);
		
		//printf("calculateSpringForce: dist[%f] k[%f]  totalRelVel[%f] damp[%f]\n", dist, springK, totalRelVel, damping);
		return BtoA * ((dist * springK) - (totalRelVel * damping));  
	}
	
}
