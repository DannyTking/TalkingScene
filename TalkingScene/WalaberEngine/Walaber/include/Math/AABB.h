#ifndef _AABB_H
#define _AABB_H

#include <Vector2.h>
#include <stdio.h>
#include <Logger.h>

namespace Walaber 
{

	class AABB
	{
	public:
		enum PointValidity { Invalid, Valid };

		Vector2				Min;
		Vector2				Max;

		PointValidity		Validity;

		AABB() : Min(Vector2::Zero), Max(Vector2::Zero), Validity(Invalid) { }
		AABB( const Vector2& minPt, const Vector2& maxPt );

		void clear();

		void expandToInclude( const Vector2& pt );
		void expandToInclude( const AABB& aabb );

		bool contains( const Vector2& pt ) const;
		bool intersects( const AABB& box ) const;

		Vector2 getSize() const { return Max - Min; }
		Vector2 getCenter() const { return Min + (getSize() * 0.5f); }

		void _debugLog() { Logger::printf("Walaber", Logger::SV_DEBUG, "[%4.2f, %4.2f >> %4.2f, %4.2f]", Min.X, Min.Y, Max.X, Max.Y); }
	};
}
#endif	// _AABB_H
