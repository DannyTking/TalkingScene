#include "Vector2.h"
#include <Vector3.h>

namespace Walaber
{
    Vector2::Vector2(const Vector3& vec) :
    X(vec.X),
    Y(vec.Y)
    { }
    
	const Vector2 Vector2::Zero = Vector2(0,0);
	const Vector2 Vector2::One = Vector2(1,1);
	const Vector2 Vector2::UnitX = Vector2(1,0);
	const Vector2 Vector2::UnitY = Vector2(0,1);
}

