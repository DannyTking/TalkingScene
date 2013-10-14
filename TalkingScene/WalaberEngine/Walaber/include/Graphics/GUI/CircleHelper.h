#ifndef _CIRCLE_HELPER_H_
#define _CIRCLE_HELPER_H_

#include <GraphicsGL.h>
#include <Color.h>


#define CIRCLE_HELPER_SEG_COUNT 32

namespace Walaber
{
	class Vector2;
	
	class CircleHelper
	{
	public:
		
		static void drawFilledCircleSegment( const Vector2& pos, float angle, float rad, float degrees, bool clockwise = true, const Color& col = Color::White );
		
		static void drawCurvedArrow( const Vector2& pos, float angle, float rad, float degrees, bool clockwise, float thickness, const Color& col = Color::White );
		
		
	private:
		
		static void _resetVerts(const Color& col = Color::White); 
		
		static inline unsigned int packColor( unsigned char r, unsigned char g, unsigned char b, unsigned char a )
		{
			return ((a << 24) | (b << 16) | (g << 8) | (r));
		}
								
		static float		mSegStep;
		static GLfloat		mVerts[CIRCLE_HELPER_SEG_COUNT * 6];
		static GLuint		mVertColors[CIRCLE_HELPER_SEG_COUNT * 3];
	};
	
	
	
}


#endif	// _CIRCLE_HELPER_H_