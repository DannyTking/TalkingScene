#include "Color.h"
#include <math.h>

namespace Walaber
{
    const float Color::MAX_COLOR_DIST_SQR = 200000.0f;
    const float	Color::CLOSE_ENOUGH_COLOR_DIST = 75.0f;
    
	Color Color::fromHSV( float h, float s, float v )
	{
		Color ret(0,0,0);

		if (s == 0.0f)
		{
			ret = Color(v,v,v);
			return ret;
		}

		h /= 60.0f;
		int i = floor(h);
		float f = h - i;
		float p = v * (1 - s);
		float q = v * (1 - s * f);
		float t = v * (1 - s * (1 - f));

		switch(i)
		{
			case 0:
				ret = Color(v, t, p);
				break;
			case 1:
				ret = Color(q, v, p);
				break;
			case 2:
				ret = Color(p, v, t);
				break;
			case 3:
				ret = Color(p, q, v);
				break;
			case 4:
				ret = Color(t, p, v);
				break;
			default:
				ret = Color(v, p, q);
		}
		return ret;
	}
    
    bool Color::isCloseEnough(const Color& input, const Color& compareTo)
	{
		float r = (float)compareTo.R - (float)input.R;
		float g = (float)compareTo.G - (float)input.G;
		float b = (float)compareTo.B - (float)input.B;
		
		r *= r;
		g *= g;
		b *= b;
		
		float lenSqr = r + g + b;
		
		return lenSqr < CLOSE_ENOUGH_COLOR_DIST;		
	}
	
	Color Color::getClosestColor(const Color& input, int& closestColorIndex, const Color* colors, const int numColors)
	{
		float inpR = (float)input.R;
		float inpG = (float)input.G;
		float inpB = (float)input.B;
		
		Color nearest;
		float distSqr = MAX_COLOR_DIST_SQR;
		
		for(int i = 0; i < numColors; ++i) 
		{
			float compR = (float)colors[i].R;
			float compG = (float)colors[i].G;
			float compB = (float)colors[i].B;
			
			float lenSqr = ((compR - inpR)*(compR - inpR) + (compG - inpG)*(compG - inpG) + (compB - inpB)*(compB - inpB));
			
			if(lenSqr == 0.0f)
			{
				nearest = colors[i];
				closestColorIndex = i;
				break;
			}
			else if ( lenSqr < distSqr)
			{
				distSqr = lenSqr;
				nearest = colors[i];
				closestColorIndex = i;
			}
		}
		
		return nearest;		
	}
	
	Color Color::parseColorFromString(std::string colorString)
	{
		Color ret;
		int r, g, b;
		sscanf(colorString.c_str(), "%d %d %d", &r, &g, &b);
		ret = Color( r, g, b );
		return ret;
	}
	
	Color Color::parseColorFromStringRGBA(const std::string& colorString)
	{
		Color ret;
		int r, g, b, a;
		sscanf(colorString.c_str(), "%d %d %d %d", &r, &g, &b, &a);
		ret = Color( r, g, b, a );
		return ret;
	}

	const Color Color::Red = Color(255,0,0);
	const Color Color::Green = Color(0,255,0);
	const Color Color::Blue = Color(0,0,255);
	const Color Color::White = Color(255,255,255);
	const Color Color::Black = Color(0,0,0);
	const Color Color::Cyan = Color(0,255,255);
	const Color Color::Yellow = Color(255,255,0);
	const Color Color::Magenta = Color(255,0,255);
	const Color Color::Grey = Color(128,128,128);
	const Color Color::Purple = Color(155, 0, 255);
	const Color Color::Orange = Color(255, 150, 0);
}
