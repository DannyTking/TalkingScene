/*
 *  Property.h
 *  WaterConcept
 *
 *  Created by Jason Pecho on 3/11/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _PROPERTY_H_
#define _PROPERTY_H_

#include <string>
namespace Walaber
{
	class Vector2;
	class Rect;
	class Color;
	class AABB;
	
	class Property
	{
	public:
		enum ValueDataType
		{
			VDT_String,
            VDT_Bool,
			VDT_Int,
			VDT_Float,
			VDT_Vector2,
			VDT_Rect,
			VDT_Color, 
			VDT_AABB
		};	
		
		Property();
		Property( const std::string& v );
		Property( const int v );
		Property( const float v );
		Property( const Vector2& v );
		Property( const Rect& v );
		Property( const Color& v );
		Property( const AABB& v);
		~Property();
		
		void setValue( const std::string& v );
		void setValue( const int v );
		void setValue( const float v );
//		void setValue( const Vector2& v );
//		void setValue( const Rect& v );
//		void setValue( const Color& v );
//		void setValue( const AABB& v );
		
		std::string asString()const;
        bool asBool()const;
		int asInt()const;
		float asFloat()const;
//		Vector2 asVector2()const;
//		Rect asRect()const;
//		Color asColor()const;
//		AABB asAABB()const;
		
	private:
		
		ValueDataType	mNativeType;
		std::string		mValue;
				
	};
}

#endif
