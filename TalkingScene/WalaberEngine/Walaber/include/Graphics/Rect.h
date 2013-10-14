#ifndef _RECT_H
#define _RECT_H

#include <Vector2.h>

namespace Walaber
{
	class Rect
	{
	public:
		Vector2 upper_left;
		Vector2 size;

		inline Rect() : upper_left(Vector2::Zero), size(Vector2::Zero) { }
		inline Rect(const Rect& rect): upper_left(rect.upper_left), size(rect.size) { }
		inline Rect( const Vector2& ul, const Vector2& sz ) : upper_left(ul), size(sz) { }
		inline Rect( float left, float top, float width, float height ) : upper_left(left,top), size(width,height) { }
		inline Rect& operator = ( const Rect& r )
		{
			upper_left = r.upper_left;
			size = r.size;
			return *this;
		}

		inline bool operator == (const Rect& r ) const
		{
			return ( upper_left == r.upper_left && size == r.size );
		}

		inline bool operator != (const Rect& r ) const
		{
			return ( upper_left != r.upper_left || size != r.size );
		}

		inline Vector2 getBottomRight() const { return upper_left + size; }
        
        static const Rect UnitRect;
	};
}
#endif	// _RECT_H
