#ifndef _VECTOR2_H
#define _VECTOR2_H

#include "WalaberMath.h"

#if USE_ACCELERATE
#include <Accelerate/Accelerate.h>
#endif

#ifdef TARGET_NDK
#include <cstdio>
#endif

namespace Walaber
{
    class Vector3;
    
    // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
    // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
    
    class Vector2
    {
    public:
        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        float	X;
        float	Y;
        
        static void DebugPrint(const char* s)
        {
            printf("DEBUG %s\n",s );
        }
        
        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline Vector2() : X(0), Y(0) { }
        inline Vector2(const float x, const float y) :
        X(x),
        Y(y)
        { }
        
        inline Vector2(const Vector2& vec) :
        X(vec.X),
        Y(vec.Y)
        { }
        
        /*!
         Takes in a Vector3 and drops the Z component
         @param vec three componenet vector
         @see Vector3
        */
        explicit Vector2(const Vector3& vec);
        
        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline void set(const float x, const float y) { X=x; Y=y; }

        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline Vector2& operator = ( const Vector2& vec )
        {
            X = vec.X;
            Y = vec.Y;
            return *this;
        }

        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline bool operator == (const Vector2& vec ) const
        {
            return ( X == vec.X && Y == vec.Y );
        }

        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- ==
        inline bool operator != (const Vector2& vec ) const
        {
            return ( X != vec.X || Y != vec.Y );
        }

        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- ==
        inline bool operator < (const Vector2& right ) const
        {
            return X < right.X || ( X == right.X && Y < right.Y );
        }
        
        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        // addition / subtraction
        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline Vector2	operator + (const Vector2& right) const 
        { 
#if USE_ACCELERATE            
            Vector2 ret;
            vDSP_vadd(const_cast<float*>(&X), 1, const_cast<float*>(&right.X), 1, &ret.X, 1, 2);
            
            return ret;
#else
            return Vector2(X+right.X, Y+right.Y);
#endif
        }
        
        inline Vector2&	operator += (const Vector2& right) 
        { 
#if USE_ACCELERATE
                      
            vDSP_vadd(&X, 1, &right.X, 1, &X, 1, 2);             
            
#else
            X += right.X;
            Y += right.Y;
#endif
            return *this; 
        }
        
        inline Vector2	operator - (const Vector2& right) const 
        { 
#if USE_ACCELERATE
            Vector2 ret;
            vDSP_vsub(const_cast<float*>(&right.X), 1, const_cast<float*>(&X), 1, &ret.X, 1, 2);
            
            return ret;
            
#else
            return Vector2(X - right.X, Y - right.Y); 
#endif
        }
        
        inline Vector2&	operator -= (const Vector2& right) 
        {
#if USE_ACCELERATE

            vDSP_vsub(&right.X, 1, &X, 1, &X, 1, 2);
            
#else
            X -= right.X; 
            Y -= right.Y; 
#endif
            return *this; 
        }

        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        // scaling!
        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline Vector2	operator * (const float right) const 
        { 
#if USE_ACCELERATE
            
            Vector2 ret;
            vDSP_vsmul(const_cast<float*>(&X), 1, const_cast<float*>(&right), &ret.X, 1, 2);
            
            return ret;
#else
            return Vector2(X * right, Y * right); 
#endif
        }
        
        inline Vector2&	operator *= (const float right) 
        { 
#if USE_ACCELERATE
            
            vDSP_vsmul(&X, 1, const_cast<float*>(&right), &X, 1, 2);
            
#else
            X *= right; 
            Y *= right; 
#endif
            return *this; 
        }
        
        inline Vector2	operator / (const float right) const
        { 
#if USE_ACCELERATE
            
            Vector2 ret;
            vDSP_vsdiv(const_cast<float*>(&X), 1, const_cast<float*>(&right), &ret.X, 1, 2);
            
            return ret;
#else
            float inv = 1.0f / right;
            return Vector2( X * inv, Y * inv );
#endif
        }

        inline Vector2&	operator /= (const float right) 
        { 
#if USE_ACCELERATE
            
            vDSP_vsdiv(const_cast<float*>(&X), 1, const_cast<float*>(&right), &X, 1, 2);

#else
            float inv = 1.0f / right;
            X *= inv; 
            Y *= inv; 
#endif
            return *this; 
        }

        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        // per-element multiplication
        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline Vector2	operator * (const Vector2& right) const 
        {
#if USE_ACCELERATE
            
            Vector2 ret;
            vDSP_vmul(&X, 1, &right.X, 1, &ret.X, 1, 2);
            
            return ret;
#else
            return Vector2( X * right.X, Y * right.Y ); 
#endif
        }
        
        inline Vector2&	operator *= (const Vector2& right) 
        {
#if USE_ACCELERATE
            
            vDSP_vmul(&X, 1, const_cast<float*>(&right.X), 1, &X, 1, 2);

#else
            X *= right.X; 
            Y *= right.Y; 
#endif    
            return *this; 
        }
        
        inline Vector2	operator / (const Vector2& right) const 
        {
#if USE_ACCELERATE
           
            Vector2 ret;
            vDSP_vdiv(const_cast<float*>(&right.X), 1, const_cast<float*>(&X), 1, &ret.X, 1, 2);
            
            return ret;
#else
            return Vector2( X / right.X, Y / right.Y ); 
#endif
        }
        
        inline Vector2&	operator /= (const Vector2& right) 
        {
#if USE_ACCELERATE
            
            vDSP_vdiv(const_cast<float*>(&right.X), 1, const_cast<float*>(&X), 1, &X, 1, 2);
            
#else
            X /= right.X; 
            Y /= right.Y; 
#endif       
            return *this; 
        }

        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline const Vector2& operator + () const { return *this; }
        inline Vector2 operator - () const { return Vector2(-X, -Y); }

        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        // helpers
        inline float length() const 
        { 
#if USE_ACCELERATE
            
            float r;
            vDSP_vdist(const_cast<float*>(&X), 1, const_cast<float*>(&Y), 1, &r, 1, 1);
            
            return r;
#else
            return sqrtf( (X*X) + (Y*Y) );
#endif
        }
        
        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline float fastLength() const
        {
            return fastSqrtf((X*X) + (Y*Y));
        }
        
        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline float lengthSquared() const 
        { 
#if USE_ACCELERATE
            
            float r;
            vDSP_svesq(const_cast<float*>(&X), 1, &r, 2); 

            return r;
#else
            return ((X*X) + (Y*Y)); 
#endif
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
            float invSqr = fastInvSqrtf((X*X)+(Y*Y));
            
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
        inline Vector2 normalisedCopy() const
        {
            Vector2 ret = *this;
            ret.normalise();
            return ret;
        }
        
        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline Vector2 fastNormalisedCopy() const
        {
            Vector2 ret = *this;
            ret.fastNormalise();
            return ret;
        }

        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline float dotProduct( const Vector2& vec ) const
        {
#if USE_ACCELERATE
            
            float r;
            vDSP_dotpr(const_cast<float*>(&X), 1, const_cast<float*>(&vec.X), 1, &r, 2);

            return r;
#else
            return (X * vec.X) + (Y * vec.Y);
#endif
        }

        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline Vector2 getPerpendicular() const
        {
            return Vector2(-Y, X);
        }

        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline void makePerpendicular()
        {
            float tempX = X;
            X = -Y;
            Y = tempX;
        }

        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline float crossProduct( const Vector2& vec ) const
        {
            return (X * vec.Y) - (Y * vec.X);
        }

        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline Vector2 lerp( const Vector2& to, float i) const
        {
#if USE_ACCELERATE
            
            Vector2 R;
            vDSP_vintb(const_cast<float*>(&X), 1, const_cast<float*>(&to.X), 1, &i, &R.X, 1, 2 );
            
            return R;
#else
            return (*this) + ((to - (*this)) * i);
#endif
        }

        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline Vector2 slerp( const Vector2& to, float i)
        {
#if USE_ACCELERATE
            
            Vector2 R;
            float iA = 0.5f + (cosf(3.14159265f * i) * -0.5f) ;
            vDSP_vintb( &X, 1, const_cast<float*>(&to.X), 1, &iA, &R.X, 1, 2 );

            return R;
#else
            return lerp( to, 0.5f + (cosf(3.14159265f * i) * -0.5f) );
#endif
        }
        

        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline Vector2 slerpin( const Vector2& to, float i)
        {
            return lerp( to, 1-cosf(1.57079633f * i) );
        }	        

        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        inline Vector2 slerpout( const Vector2& to, float i)
        {
            return lerp( to, sinf(1.57079633f *  i) );
        }
        

        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        static const Vector2 Zero;
        static const Vector2 One;
        static const Vector2 UnitX;
        static const Vector2 UnitY;
        
        // -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == -- == 
        static inline Vector2 max( Vector2& a, Vector2& b ) { return Vector2( maxf(a.X, b.X), maxf(a.Y, b.Y) ); }
        static inline Vector2 min( Vector2& a, Vector2& b ) { return Vector2( minf(a.X, b.X), minf(a.Y, b.Y) ); }
        

    };
}
#endif	// _VECTOR2_H
