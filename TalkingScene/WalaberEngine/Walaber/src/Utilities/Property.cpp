/*
 *  Property.cpp
 *  WaterConcept
 *
 *  Created by Jason Pecho on 3/11/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "Property.h"

#include <sstream>

#include <Vector2.h>
#include <Rect.h>
#include <Color.h>
#include <AABB.h>

namespace Walaber
{
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	Property::Property():
	mNativeType(VDT_String),
	mValue()
	{
	}	
		
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	Property::Property( const std::string& v )
	{
		setValue(v);
	}
	
    // --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	Property::Property( const char* v )
	{
		setValue( std::string( v ) );
	}
    
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	Property::Property( const bool b )
	{
		setValue(b);
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	Property::Property( const int v )
	{
		setValue(v);
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	Property::Property( const float v )
	{		
		setValue(v);
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	Property::Property( const Vector2& v )
	{
		setValue(v);
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	Property::Property( const Rect& v )
	{
		setValue(v);
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	Property::Property(const Color& v )
	{
		setValue(v);
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	Property::Property( const AABB& v )
	{
		setValue(v);
	}
	
    // --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	Property::Property( const void* v )
	{
		setValue(v);
	}
    
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	Property::~Property()
	{
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	void Property::setValue( const std::string& v )
	{
		mNativeType = VDT_String;
		mValue = v;
	}
    
    // --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	void Property::setValue( const char* v )
    {
		setValue( std::string( v ) );
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	void Property::setValue( const bool b )
	{
		mNativeType = VDT_Bool;
		
		std::stringstream ss;
		ss << ((int)(b ? 1 : 0));
		mValue = ss.str();
	}

	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	void Property::setValue( const int v )
	{
		mNativeType = VDT_Int;
		
		std::stringstream ss;
		ss << v;
		mValue = ss.str();
	}

	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	void Property::setValue( const float v )
	{
		mNativeType = VDT_Float;
		std::stringstream ss;
		ss << v;
		mValue = ss.str();
	}

	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	void Property::setValue( const Vector2& v )
	{
		mNativeType = VDT_Vector2;
		
		std::stringstream ss;
		ss << v.X << " " << v.Y;
		mValue = ss.str();
	}

	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	void Property::setValue( const Rect& v )
	{
		mNativeType = VDT_Rect;
		
		std::stringstream ss;
		ss << v.upper_left.X << " " << v.upper_left.Y << " " << v.size.X << " " << v.size.Y;
		mValue = ss.str();
	}

	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	void Property::setValue( const Color& v )
	{
		mNativeType = VDT_Color;
		
		std::stringstream ss;
		ss << (int)v.R << " " << (int)v.G << " " << (int)v.B << " " << (int)v.A;
		mValue = ss.str();
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	void Property::setValue( const AABB& v )
	{
		mNativeType = VDT_AABB;
		
		std::stringstream ss;
		ss << v.Min.X << " " << v.Min.Y << " " << v.Max.X << " " << v.Max.Y;
		mValue = ss.str();
	}

    // --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	void Property::setValue( const void* v )
	{
		mNativeType = VDT_Pointer;
		
		std::stringstream ss;
		ss << v;
		mValue = ss.str();
	}
    
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	std::string Property::asString()const
	{
		return mValue;
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	bool Property::asBool()const
	{
		bool ret = false;
		if (mNativeType == VDT_Bool || mNativeType == VDT_Int || mNativeType == VDT_String)
        {
            ret = (mValue == "1" || mValue == "true" || mValue == "True" );
        }
		return ret;
	}

	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	int Property::asInt()const
	{
		int ret = 0;
		if (mNativeType == VDT_Int || mNativeType == VDT_Bool || mNativeType == VDT_String)
			sscanf((const char*)mValue.c_str(), "%d", &ret);
		return ret;
	}

	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	float Property::asFloat()const
	{
		float ret = 0.0f;
		if (mNativeType == VDT_Float || mNativeType == VDT_String)
			sscanf((const char*)mValue.c_str(), "%f", &ret);
		return ret;
	}

	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	Vector2 Property::asVector2()const
	{
		Vector2 ret = Vector2::Zero;
		if (mNativeType == VDT_Vector2 || mNativeType == VDT_String)
			sscanf((const char*)mValue.c_str(), "%f %f", &ret.X, &ret.Y);
		return ret;
	}

	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	Rect Property::asRect()const
	{
		Rect ret = Walaber::Rect(Vector2::Zero, Vector2::One);
		if (mNativeType == VDT_Rect || mNativeType == VDT_String)
			sscanf((const char*)mValue.c_str(), "%f %f %f %f", &ret.upper_left.X, &ret.upper_left.Y, &ret.size.X, &ret.size.Y);
		return ret;
		
	}

	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	Color Property::asColor()const
	{
		Color ret = Walaber::Color::White;
		if (mNativeType == VDT_Color || mNativeType == VDT_String)
		{
			int r, g, b, a;
			sscanf((const char*)mValue.c_str(), "%d %d %d %d", &r, &g, &b, &a);
			ret = Color(r,g,b,a);
		}
		return ret;
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	AABB Property::asAABB()const
	{
		AABB ret = Walaber::AABB(Vector2::Zero, Vector2::One);
		if (mNativeType == VDT_AABB || mNativeType == VDT_String)
			sscanf((const char*)mValue.c_str(), "%f %f %f %f", &ret.Min.X, &ret.Min.Y, &ret.Max.X, &ret.Max.Y);
		return ret;		
	}

}
