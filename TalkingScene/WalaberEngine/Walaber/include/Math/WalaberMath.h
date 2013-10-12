#ifndef _WALABER_MATH_
#define _WALABER_MATH_

#include <vector>
#include <math.h>
#include <cassert>

#ifdef TARGET_NDK

// to get rand
#include <cstdlib>
#endif

#define FLOATING_POINT_ONE_POINT_ZERO 0x3f800000


namespace Walaber 
{
    class Vector2;  
	
	/*! Simple typedef for a vector of Vector2 objects */
	typedef std::vector<Vector2> Vector2List;

	
	static const float PI = 3.14159265f;
	static const float TWO_PI = (3.14159265f * 2.0f);
	static const float HALF_PI = (3.14159265f * 0.5f);
	static const float PI_OVER_FOUR = (3.14159265f * 0.25f);
	static const float PI_OVER_ONE_EIGHTY = (3.14159265f / 180.0f);
	static const float ONE_EIGHTY_OVER_PI = (180.0f / 3.14159265f);
	static const float THREE_PI_OVER_TWO = (3.0f * 3.14159265f) / (2.0f);
    static const float EPSILON_IS_EQUAL = 1e-05f; /*! < if diff is less than this, values are considered equal */
	
    
	/*! absolute value of a float. */
	static inline float absf( float v ) { return (v >= 0.0f) ? v : -v; }	
	
	/*! max value between two floats. */
	static inline float maxf( float a, float b ) { return (a > b) ? a : b; }
	
	/*! min value between two floats. */
	static inline float minf( float a, float b ) { return (a < b) ? a : b; }
	
	/*! clamp x to range [min, max] */
	static inline float clampf( float x, float min, float max ) { return (x <= min) ? min : ((x >= max) ? max : x); }
	static inline float round( float v ) { return v - (int)v < 0.5f ? (float)((int)v) : (float)((int)v+1); }    
	
	/*! returns (integer) sign for a float.  if > 0, returns +1, < 0 returns -1, otherwise returns 0. */
	static inline int signf( float v ) { return ((v > 0.0f) ? 1 : ((v < 0.0f) ? -1 : 0)); }
	
    /*! returns the largest floating point number in a n number set */
    static inline float maxsetf( const std::vector< float >& set )
    {
        assert( set.size() );
        
        float max = set[0];
        
        unsigned int size = static_cast<unsigned int>( set.size() );
        for( unsigned int i = 1; i < size; ++i )
        {
            max = maxf( max, set[i] );
        }
        
        return max;
    }
    
	/*! linear interpolation, from --> to.  val t should be in range [0,1], but is NOT clamped to that range. */
    static inline float lerp(const float from, const float to, const float t)
	{
		return (from) + ((to - (from)) * t);
	}

	/*! smoothly interpolate (based on sin curve) from --> to.  val t should be in range [0,1], but is NOT clamped to that range. */
	static inline float slerp(const float from, const float to, const float t)
	{
		return (from) + ((to - (from)) * (0.5f + (sinf((3.141592654f * t) - 1.570796f) * 0.5f)));
	}	

    static inline float slerpin(const float from, const float to, const float t)
    {
        return lerp( from, to, 1.0f-cosf(1.57079633f * t) );
    }	        


    static inline float slerpout(const float from, const float to, const float t)
    {
		return lerp( from, to, sinf(1.57079633f *  t) );
    }    
    
    /*! given a min and a max, and a current value, returns a value clamped to range [0, 1] where 0 means min, and 1 means max. */
    static inline float getInterpVal( float val, float min, float max )
    {
        return clampf( (val-min)/(max-min), 0.0f, 1.0f );
    }
    
	/*! returns an integer value in range [min, max] */
    static inline int randomRange( int min, int max )
    {
        return (rand() % ((max + 1) - min)) + min;
    }
    
	/*! returns a float value in range [min, max] */
    static inline float randomRange( float min, float max )
    {
        // Granularity of the range
        int granularity = 10000;
        return (rand() % (granularity + 1)) * ((max - min) / ((float) granularity)) + min;
    }
	
	static inline bool isInRangeInclusive( float x, float min, float max )
	{
		return ((x >= min) && (x <= max));
	}
	
	static inline bool isInRangeExclusive( float x, float min, float max )
	{
		return ((x > min) && (x < max));
	}
    
	/*! absolute value for an integer. */
	static inline int absi( int i ) { return (i >= 0) ? i : -i; }
	
	/*! max value between two integers. */
	static inline int maxi( int i, int m ) { return (i > m) ? i : m; }
	
	/*! min value between two integers. */
	static inline int mini( int i, int m ) { return (i < m) ? i : m; }
	
	/*! clamp integer to range [min, max]. */
	static inline int clampi( int i, int min, int max ) { return (i <= min) ? min : ((i >= max) ? max : i); }
	
	/*! returns sign of a signed integer. > 0 returns +1, < 0 returns -1, 0 returns 0. */
	static inline int signi( int i ) { return ((i > 0) ? 1 : ((i < 0) ? -1 : 0)); }
	
	/*! get the next highest Power of Two for a given integer.  for example given 38, returns 64.  given 128, returns 128. */
	static inline unsigned int nextPOT(unsigned int x)
	{
		x = x - 1;
        x = x | (x >> 1);
        x = x | (x >> 2);
        x = x | (x >> 4);
        x = x | (x >> 8);
        x = x | (x >>16);
        return x + 1;
	}
	
	/*! given degrees, returns radians */
	static inline float degToRad( float deg ) { return deg * (PI_OVER_ONE_EIGHTY); }
	
	/*! given radians, returns degress  */
	static inline float radToDeg( float rad ) { return rad * (ONE_EIGHTY_OVER_PI); }
	
    /*! optimized square root calculation (float). */
    static inline float fastSqrtf( float x )
    {
        
        int temp = *(int*)&x;
        temp -= FLOATING_POINT_ONE_POINT_ZERO;
        temp >>= 1;
        temp += FLOATING_POINT_ONE_POINT_ZERO;
        return *(float*)&temp;
    }
    
    /*! optimized inverse square root calculation (float) */
    static inline float fastInvSqrtf( float x )
    {
        float xHalf = 0.5f * x;
        int i = *(int*)&x;
        i = 0x5f3759df - (i>>1);
        x = *(float*)&i;
        x = x * (1.5f - xHalf*x*x);
        return x;
    }
    
	/*! normalizes an angle (in radians) into range [-PI, PI] */
    static float normalizeAngle_PI(float angle)
    {
        while (angle > PI)
        {
            angle -= TWO_PI;
        }
         
        while (angle <= -PI)
        {
            angle += TWO_PI;
        }
        return angle;
    }
    
	/*! normalize an angle (in radians) into range [0, 2 * PI] */
    static float normalizeAngle_Zero_TWO_PI(float angle)
    {
        if(angle < 0.0f)
        {
            while(angle < 0)
            {
                angle += TWO_PI;
            }
            
            // if floating pt erros knock it over
            if(angle >= TWO_PI)
            {
                angle = 0.0f;
            }
        }
        else if(angle >= TWO_PI)
        {
            while(angle >= TWO_PI)
            {
                angle -= TWO_PI;
            }
            
            if(angle < 0.0f)
            {
                angle = 0.0f;
            }
        }
          
        return angle;
    }
    
    /*! calculates angle1 - angle2 and normalizes to [ -PI, PI ] range. */
    static inline float differenceBetweenAngles(float angle1, float angle2)
    {
        return normalizeAngle_PI(angle1 - angle2);
    }
    
    /*! 
     Checks if two values are within a certain epsilon
     @param value1 first value to compare for equality
     @param value2 second value to compare for equality
     @param epsilon if the difference of value1 and value2 are less than this, they are equal
    */
    bool inline isEqualf( float value1, float value2, float epsilon = EPSILON_IS_EQUAL )
    {
        return absf( value1 - value2 ) < EPSILON_IS_EQUAL;
    }
    
    // --------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==
    /*! given two points (and two handle points), interpolate along the bezier spline described by these points.
     \param interp position along the spline to interpolate (should be in range [0,1] but is NOT clamped internally in this function.
     */
    inline float bezierInterp( const float ptA, const float handleA, const float ptB, const float handleB, float interp )
    {
        /*
         Bezier Equation:
         f(t) = p0*(1-x)^3 +
         3*p1*(1-x)^2*x +
         3*p2*(1-x)*x^2 +
         p3*x^3
         
        float t_sqr = interp * interp;
        float t_cub = t_sqr * interp;
        
        // (1 - t) = "one minus t" = "omt"
        float omt = (1.0f - interp);
        float omt_sqr = omt * omt;
        float omt_cub = omt * omt_sqr;
        
        return ( ptA * (omt_cub) ) +
               ( handleA * (3.0f * omt_sqr * interp) ) +
               ( handleB * (3.0f * omt * t_sqr) ) +
               ( ptB * (t_cub) );
         
        */
        
        float a = 3.0f * ptA;
        float b = 3.0f * handleA;
        float c = 3.0f * handleB;
        return ( ( ptB - ptA + b - c ) * interp ) + ( ( a - 2*b + c ) * interp  ) + ( ( b - a ) * interp ) + ptA;
    }
}
#endif	// _WALABER_MATH_
