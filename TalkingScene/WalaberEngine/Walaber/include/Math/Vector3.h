#ifndef _VECTOR3_H
#define _VECTOR3_H

#include "WalaberMath.h"
#include "Vector2.h"

#include <cassert>

#ifdef TARGET_NDK
#include <cstdio>
#endif

namespace Walaber
{
    // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
    // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
    
    class Vector3
    {
    public:
        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        float	X;
        float	Y;
		float	Z;

        
        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline Vector3() :
        X(0),
        Y(0),
        Z(0)
        { }
        
        inline Vector3(const float x, const float y, const float z) :
        X(x),
        Y(y),
        Z(z)
        { }
        
        inline Vector3( const float scalar ) :
        X(scalar),
        Y(scalar),
        Z(scalar)
        { }
        
        inline explicit Vector3(const Vector2& vec) :
        X(vec.X),
        Y(vec.Y),
        Z(0.0f)
        { }
        
		inline Vector3(const Vector3& vec) :
        X(vec.X),
        Y(vec.Y),
        Z(vec.Z)
        { }
        
        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline void set(const float x, const float y, const float z)
        {
            X=x;
            Y=y;
            Z=z;
        }

        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline Vector3& operator = ( const Vector3& vec )
        {
            X = vec.X;
            Y = vec.Y;
			Z = vec.Z;
            return *this;
        }
		
		// -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- ==
        inline Vector3& operator = ( const Vector2& vec )
        {
            X = vec.X;
            Y = vec.Y;
			Z = 0;
            return *this;
        }

        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline bool operator == (const Vector3& vec ) const
        {
            return ( X == vec.X && Y == vec.Y && Z == vec.Z );
        }

        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline bool operator != (const Vector3& vec ) const
        {
            return ( X != vec.X || Y != vec.Y || Z != vec.Z );
        }

        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        // addition / subtraction
        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline Vector3	operator + (const Vector3& right) const
        { 
            return Vector3(X+right.X, Y+right.Y, Z+right.Z);
        }
        
        inline Vector3&	operator += (const Vector3& right)
        {
            X += right.X;
            Y += right.Y;
			Z += right.Z;
            return *this; 
        }
        
        inline Vector3	operator - (const Vector3& right) const
        { 
            return Vector3(X - right.X, Y - right.Y, Z - right.Z);
        }
        
        inline Vector3&	operator -= (const Vector3& right)
        {
            X -= right.X;
            Y -= right.Y;
			Z -= right.Z;
            return *this;
        }

        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        // scaling!
        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline Vector3	operator * (const float right) const
        { 
            return Vector3(X * right, Y * right, Z * right);
        }
        
        inline Vector3&	operator *= (const float right)
        { 
            X *= right;
            Y *= right;
			Z *= right;
            return *this;
        }
        
        inline Vector3	operator / (const float right) const
        {
            assert( right != 0.0f );
            float inv = 1.0f / right;
            return Vector3( X * inv, Y * inv, Z * inv );
        }

        inline Vector3&	operator /= (const float right)
        { 
            float inv = 1.0f / right;
            X *= inv;
            Y *= inv;
			Z *= inv;
            return *this; 
        }

        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        // per-element multiplication
        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline Vector3	operator * (const Vector3& right) const
        {
            return Vector3( X * right.X, Y * right.Y, Z * right.Z );
        }
        
        inline Vector3&	operator *= (const Vector3& right)
        {
            X *= right.X;
            Y *= right.Y;
			Z *= right.Z;
            return *this; 
        }
        
        inline Vector3	operator / (const Vector3& right) const
        {
            return Vector3( X / right.X, Y / right.Y, Z / right.Z );
        }
        
        inline Vector3&	operator /= (const Vector3& right)
        {
            X /= right.X;
            Y /= right.Y;
			Z /= right.Z;
            return *this;
        }

        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline const Vector3& operator + () const { return *this; }
        inline Vector3 operator - () const { return Vector3(-X, -Y, -Z); }

        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        // helpers
        inline float length() const 
        { 
            return sqrtf( (X*X) + (Y*Y) + (Z*Z) );
        }
        
        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline float fastLength() const
        {
            return fastSqrtf((X*X) + (Y*Y) + (Z*Z));
        }
        
        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- ==
        inline float lengthSquared() const 
        { 
            return ((X*X) + (Y*Y) + (Z*Z));
        }
        
        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline float normalise()
        {
            float l = length();
            if (l > 1e-08f)
            {
                float invL = 1.0f / l;
                
                (*this) *= invL;
            }
            return l;
        }
        
        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline void fastNormalise()
        {
            float invSqr = fastInvSqrtf((X*X)+(Y*Y)+(Z*Z));
            
            (*this) *= invSqr;
        }
        
        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline float fastNormaliseLength()
        {
            float l = fastLength();
            if (l > 1e-08f)
            {
                float invL = 1.0f / l;
                
                (*this) *= invL;
            }
            return l;
        }

        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline Vector3 normalisedCopy() const
        {
            Vector3 ret = *this;
            ret.normalise();
            return ret;
        }
        
        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline Vector3 fastNormalisedCopy() const
        {
            Vector3 ret = *this;
            ret.fastNormalise();
            return ret;
        }

        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline float dotProduct( const Vector3& vec ) const
        {
            return (X * vec.X) + (Y * vec.Y) + (Z * vec.Z);
        }


        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline Vector3 crossProduct( const Vector3& vec ) const
        {
            return Vector3((Y * vec.Z - Z * vec.Y),
						   (Z * vec.X - X * vec.Z),
						   (X * vec.Y - Y * vec.X));
        }

        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline Vector3 lerp( const Vector3& to, float i) const
        {
            return (*this) + ((to - (*this)) * i);
        }

        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline Vector3 slerp( const Vector3& to, float i)
        {
            return lerp( to, 0.5f + (cosf(3.14159265f * i) * -0.5f) );
        }
        

        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline Vector3 slerpin( const Vector3& to, float i)
        {
            return lerp( to, 1-cosf(1.57079633f * i) );
        }	        

        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline Vector3 slerpout( const Vector3& to, float i)
        {
            return lerp( to, sinf(1.57079633f *  i) );
        }
        

        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        static const Vector3 Zero;
        static const Vector3 One;
        static const Vector3 UnitX;
        static const Vector3 UnitY;
		static const Vector3 UnitZ;
        
        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        static inline Vector3 max( Vector3& a, Vector3& b ) { return Vector3( maxf(a.X, b.X), maxf(a.Y, b.Y), maxf(a.Z, b.Z) ); }
        static inline Vector3 min( Vector3& a, Vector3& b ) { return Vector3( minf(a.X, b.X), minf(a.Y, b.Y), minf(a.Z, b.Z) ); }
        

    };
}
#endif	// _VECTOR3_H
