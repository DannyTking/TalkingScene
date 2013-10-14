#ifndef _COLOR_H
#define _COLOR_H

#include <stdio.h>
#include <stdint.h>

#include <string>

#include <Logger.h>
#include <WalaberMath.h>

namespace Walaber
{
	class Color
	{
	public:
        static const float				MAX_COLOR_DIST_SQR;					
		static const float				CLOSE_ENOUGH_COLOR_DIST;
        
		typedef uint8_t color_channel;
		
		color_channel R;
		color_channel G;
		color_channel B;
		color_channel A;

		Color() : R(0), G(0), B(0), A(255) { }
		Color( float r, float g, float b, float a = 1.0f ) : R(_clipVal(r*255)), G(_clipVal(g*255)), B(_clipVal(b*255)), A(_clipVal(a*255)) { }
		Color( int r, int g, int b, int a = 255 ) : R(_clipVal(r)), G(_clipVal(g)), B(_clipVal(b)), A(_clipVal(a)) { }
		Color( const Color& c ) : R(c.R), G(c.G), B(c.B), A(c.A) { }

		// operators...
		inline Color& operator = (const Color& c )
		{
			R = c.R;
			G = c.G;
			B = c.B;
			A = c.A;
			return *this;
		}

		inline bool operator == (const Color& c) const
		{
			return (R == c.R && G == c.G && B == c.B && A == c.A);
		}

		inline bool operator != (const Color& c) const
		{
			return (R != c.R || G != c.G || B != c.B || A != c.A);
		}

		inline Color operator + (const Color& right ) const 
		{ 
			int r = R + right.R;
			int g = G + right.G;
			int b = B + right.B;
			int a = A + right.A;
			
			return Color(_clipVal(r), _clipVal(g), _clipVal(b), _clipVal(a)); 
		}
		
		inline Color operator += (const Color& right ) { R += right.R; G += right.G; B += right.B; A += right.A; _clip(); return *this; }
		
		inline Color operator - (const Color& right ) const 
		{ 
			int r = R - right.R;
			int g = G - right.G;
			int b = B - right.B;
			int a = A - right.A;
			
			return Color(_clipVal(r), _clipVal(g), _clipVal(b), _clipVal(a)); 
		}
		
		inline Color operator -= (const Color& right ) { R -= right.R; G -= right.G; B -= right.B; A -= right.A; _clip(); return *this; }
		
		inline Color operator * (const float right) const 
		{
			int r = static_cast<int>(R * right);
			int g = static_cast<int>(G * right);
			int b = static_cast<int>(B * right);
			int a = static_cast<int>(A * right);
			return Color(_clipVal(r), _clipVal(g), _clipVal(b), _clipVal(a));
		}
		
		inline Color operator * (const Color& right) const 
		{
			int r = (int)( ( (float)R/255.0f ) * ( (float)right.R/255.0f ) * 255.0f );
			int g = (int)( ( (float)G/255.0f ) * ( (float)right.G/255.0f ) * 255.0f );
			int b = (int)( ( (float)B/255.0f ) * ( (float)right.B/255.0f ) * 255.0f );
			int a = (int)( ( (float)A/255.0f ) * ( (float)right.A/255.0f ) * 255.0f );
			return Color(_clipVal(r), _clipVal(g), _clipVal(b), _clipVal(a));
		}		
		
		inline Color&	operator *= (const float right) { R *= right; G *= right; B *= right; A *= right; return *this; }

		inline const Color& operator + () const { return *this; }
		inline void clip() { _clip(); }

		inline Color lerp( const Color& to, float i) const
		{
            i = clampf(i, 0.0f, 1.0f);
			int toR = static_cast<int>((to.R - R) * i);
			int toG = static_cast<int>((to.G - G) * i);
			int toB = static_cast<int>((to.B - B) * i);
			int toA = static_cast<int>((to.A - A) * i);
			
			return Color( R + toR, G + toG, B + toB, A + toA );
		}
		
		inline Color withAlpha( unsigned char a ) { return Color(R,G,B,a); }
		inline Color withAlpha( float a ) { return Color(R,G,B, _clipVal(a * 255)); }

		static Color fromHSV( float h, float s, float v );

		void inline debugPrint() const { Logger::printf("Walaber", Logger::SV_DEBUG, "Color: [%i][%i][%i][%i]\n", (unsigned int)R, (unsigned int)G,(unsigned int)B, (unsigned int)A); }

        static bool isCloseEnough(const Color& input, const Color& compareTo);
		static Color getClosestColor(const Color& input, int& closestColorIndex, const Color* colors, const int numColors);
        
		static Color parseColorFromString(std::string colorString);
		static Color parseColorFromStringRGBA(const std::string& colorString);
		
	private:

		void inline _clip()
		{
			R = _clipVal(R);
			G = _clipVal(G);
			B = _clipVal(B);
			A = _clipVal(A);
		}

		unsigned char inline _clipVal( int V ) const
        {
            return ((V < 0) ? 0 : ((V > 255) ? 255 : static_cast<unsigned char>(V)));
        }

		unsigned char inline _clipVal( float V ) const
        {
            int iVal = static_cast<int>( V );
            return ((iVal < 0) ? 0 : ((iVal > 255) ? 255 : static_cast<unsigned char>(iVal)));
        }

	public:
		static const Color Red;
		static const Color Green;
		static const Color Blue;
		static const Color White;
		static const Color Black;
		static const Color Cyan;
		static const Color Yellow;
		static const Color Magenta;
		static const Color Grey;
		static const Color Purple;
		static const Color Orange;
	};
}
#endif	//_COLOR_H
