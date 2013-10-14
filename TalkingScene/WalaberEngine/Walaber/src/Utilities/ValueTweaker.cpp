#include "ValueTweaker.h"

#include <StringHelper.h>
#include <iostream>
#include <fstream>
#include <cassert>

namespace Walaber
{
	ValueTweaker::TweakableValueMap ValueTweaker::mTweakables;
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	void ValueTweaker::setTweakableValue( std::string name, std::string val, ValueDataType dataType )
	{
		TweakableValueMap::iterator it = mTweakables.find(name);
		if (it != mTweakables.end())
		{
			if (it->second.NativeType == dataType)
				it->second.setValue( val );
		}
		else
		{
			// new entry!
			TweakableValue tv;
			tv.NativeType = dataType;
			tv.setValue( val );
			mTweakables[name] = tv;
		}
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	void ValueTweaker::setTweakableValue( std::string name, int val )
	{
		TweakableValueMap::iterator it = mTweakables.find(name);
		if (it != mTweakables.end())
		{
			it->second.setValue( val );
		}
		else
		{
			// new entry!
			TweakableValue tv;
			tv.NativeType = VDT_Int;
			tv.setValue( val );
			mTweakables[name] = tv;
		}
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	void ValueTweaker::setTweakableValue( std::string name, float val )
	{
		TweakableValueMap::iterator it = mTweakables.find(name);
		if (it != mTweakables.end())
		{
			it->second.setValue( val );
		}
		else
		{
			// new entry!
			TweakableValue tv;
			tv.NativeType = VDT_Float;
			tv.setValue( val );
			mTweakables[name] = tv;
		}
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	void ValueTweaker::setTweakableValue( std::string name, Vector2 val )
	{
		TweakableValueMap::iterator it = mTweakables.find(name);
		if (it != mTweakables.end())
		{
			it->second.setValue( val );
		}
		else
		{
			// new entry!
			TweakableValue tv;
			tv.NativeType = VDT_Vector2;
			tv.setValue( val );
			mTweakables[name] = tv;
		}
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	void ValueTweaker::setTweakableValue( std::string name, Rect val )
	{
		TweakableValueMap::iterator it = mTweakables.find(name);
		if (it != mTweakables.end())
		{
			it->second.setValue( val );
		}
		else
		{
			// new entry!
			TweakableValue tv;
			tv.NativeType = VDT_Rect;
			tv.setValue( val );
			mTweakables[name] = tv;
		}
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	void ValueTweaker::setTweakableValue( std::string name, Color val )
	{
		TweakableValueMap::iterator it = mTweakables.find(name);
		if (it != mTweakables.end())
		{
			it->second.setValue( val );
		}
		else
		{
			// new entry!
			TweakableValue tv;
			tv.NativeType = VDT_Color;
			tv.setValue( val );
			mTweakables[name] = tv;
		}
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	void ValueTweaker::mapTweakableValue( std::string name, std::string* mappedVariable, void* owner )
	{
		TweakableValueMap::iterator it = mTweakables.find(name);
		if (it != mTweakables.end())
		{
			if (it->second.NativeType == VDT_String)
			{
				it->second.MappedValues[ owner ] = mappedVariable;
				it->second._fireMappings();
			}
			else
			{
				printf("Error!  trying to map a string to a tweakable (%s) with mismatched type: %d\n", name.c_str(), it->second.NativeType);
			}
		}
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	void ValueTweaker::mapTweakableValue( std::string name, int* mappedVariable, void* owner )
	{
		TweakableValueMap::iterator it = mTweakables.find(name);
		if (it != mTweakables.end())
		{
			if (it->second.NativeType == VDT_Int)
			{
				it->second.MappedValues[ owner ] = mappedVariable;
				it->second._fireMappings();
			}
			else
			{
				printf("Error!  trying to map a int to a tweakable (%s) with mismatched type: %d\n", name.c_str(), it->second.NativeType);
			}
		}
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	void ValueTweaker::mapTweakableValue( std::string name, float* mappedVariable, void* owner )
	{
		TweakableValueMap::iterator it = mTweakables.find(name);
		if (it != mTweakables.end())
		{
			if (it->second.NativeType == VDT_Float)
			{
				it->second.MappedValues[ owner ] = mappedVariable;
				it->second._fireMappings();
			}
			else
			{
				printf("Error!  trying to map a float to a tweakable (%s) with mismatched type: %d\n", name.c_str(), it->second.NativeType);
			}
		}
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	void ValueTweaker::mapTweakableValue( std::string name, Vector2* mappedVariable, void* owner )
	{
		TweakableValueMap::iterator it = mTweakables.find(name);
		if (it != mTweakables.end())
		{
			if (it->second.NativeType == VDT_Vector2)
			{
				it->second.MappedValues[ owner ] = mappedVariable;
				it->second._fireMappings();
			}
			else
			{
				printf("Error!  trying to map a Vector2 to a tweakable (%s) with mismatched type: %d\n", name.c_str(), it->second.NativeType);
			}
		}
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	void ValueTweaker::mapTweakableValue( std::string name, Rect* mappedVariable, void* owner )
	{
		TweakableValueMap::iterator it = mTweakables.find(name);
		if (it != mTweakables.end())
		{
			if (it->second.NativeType == VDT_Rect)
			{
				it->second.MappedValues[ owner ] = mappedVariable;
				it->second._fireMappings();
			}
			else
			{
				printf("Error!  trying to map a Rect to a tweakable (%s) with mismatched type: %d\n", name.c_str(), it->second.NativeType);
			}
		}
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	void ValueTweaker::mapTweakableValue( std::string name, Color* mappedVariable, void* owner )
	{
		TweakableValueMap::iterator it = mTweakables.find(name);
		if (it != mTweakables.end())
		{
			if (it->second.NativeType == VDT_Color)
			{
				it->second.MappedValues[ owner ] = mappedVariable;
				it->second._fireMappings();
			}
			else
			{
				printf("Error!  trying to map a Color to a tweakable (%s) with mismatched type: %d\n", name.c_str(), it->second.NativeType);
			}
		}
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	void ValueTweaker::removeMappingsForOwner( void* owner )
	{
		for (TweakableValueMap::iterator it = mTweakables.begin(); it != mTweakables.end(); ++it)
			it->second.removeMappingForOwner(owner);
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	std::string ValueTweaker::getTweakableValueString( std::string name )
	{
		TweakableValueMap::iterator it = mTweakables.find(name);
		if (it != mTweakables.end())
			return it->second.asString();
		
		return "ERR";
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	int ValueTweaker::getTweakableValueInt( std::string name )
	{
		TweakableValueMap::iterator it = mTweakables.find(name);
		if (it != mTweakables.end())
			return it->second.asInt();
		
		return 0;
		
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	float ValueTweaker::getTweakableValueFloat( std::string name )
	{
		TweakableValueMap::iterator it = mTweakables.find(name);
		if (it != mTweakables.end())
			return it->second.asFloat();
		
		return 0.0f;
		
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	Vector2 ValueTweaker::getTweakableValueVector2( std::string name )
	{
		TweakableValueMap::iterator it = mTweakables.find(name);
		if (it != mTweakables.end())
			return it->second.asVector2();
		
		return Vector2::Zero;
		
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	Rect ValueTweaker::getTweakableValueRect( std::string name )
	{
		TweakableValueMap::iterator it = mTweakables.find(name);
		if (it != mTweakables.end())
			return it->second.asRect();
		
		return Rect(Vector2::Zero, Vector2::One);
		
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	Color ValueTweaker::getTweakableValueColor( std::string name )
	{
		TweakableValueMap::iterator it = mTweakables.find(name);
		if (it != mTweakables.end())
			return it->second.asColor();
		
		return Color::White;
	}
	
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	void ValueTweaker::loadFromFile( std::string filename )
	{
		// loads from a simple CSV file format.  expected format of the file is:
		// NAME;DATATYPE;VALUE
		std::ifstream fileStream( filename.c_str(), std::ios_base::in );
		if (fileStream.is_open())
		{
			bool firstLine = true;
			while (!fileStream.eof())
			{
				std::string line;
				getline(fileStream, line);
				
				if (firstLine)
				{
					// first line is assumed to be the column names.
					firstLine = false;
					continue;
				}
				
				std::vector< std::string > elements = StringHelper::split( line, ';' );
				if (elements.size() == 3)
				{
					printf("got elements! name[%s]  datatype[%s]  value[%s]\n", elements[0].c_str(), elements[1].c_str(), elements[2].c_str());
					
					setTweakableValue( elements[0], elements[2], _StringToValueType(elements[1]) );
				}
			}		
		}
	}
	
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	void ValueTweaker::TweakableValue::setValue( std::string v )
	{
		Value = v;
		_fireMappings();
	}
	
	void ValueTweaker::TweakableValue::setValue( int v )
	{
		if (NativeType == VDT_Int)
		{
			std::stringstream ss;
			ss << v;
			ss >> Value;
			_fireMappings();
		}
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	void ValueTweaker::TweakableValue::setValue( float v )
	{
		if (NativeType == VDT_Float)
		{
			std::stringstream ss;
			ss << v;
			ss >> Value;
			_fireMappings();
		}
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	void ValueTweaker::TweakableValue::setValue( Vector2 v )
	{
		if (NativeType == VDT_Vector2)
		{
			std::stringstream ss;
			ss << v.X << " " << v.Y;
			ss >> Value;
			_fireMappings();
		}
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	void ValueTweaker::TweakableValue::setValue( Rect v )
	{
		if (NativeType == VDT_Rect)
		{
			std::stringstream ss;
			ss << v.upper_left.X << " " << v.upper_left.Y << " " << v.size.X << " " << v.size.Y;
			ss >> Value;
			_fireMappings();
		}
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	void ValueTweaker::TweakableValue::setValue( Color v )
	{
		if (NativeType == VDT_Color)
		{
			std::stringstream ss;
			ss << v.R << " " << v.G << " " << v.B << " " << v.A;
			ss >> Value;
			_fireMappings();
		}
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	std::string ValueTweaker::TweakableValue::asString()
	{
		return Value;
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	int ValueTweaker::TweakableValue::asInt()
	{
		int ret = 0;
		if (NativeType == VDT_Int)
			sscanf((const char*)Value.c_str(), "%d", &ret);
		return ret;
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	float ValueTweaker::TweakableValue::asFloat()
	{
		float ret = 0.0f;
		if (NativeType == VDT_Float)
			sscanf((const char*)Value.c_str(), "%f", &ret);
		return ret;
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	Vector2 ValueTweaker::TweakableValue::asVector2()
	{
		Vector2 ret = Vector2::Zero;
		if (NativeType == VDT_Vector2)
			sscanf((const char*)Value.c_str(), "%f %f", &ret.X, &ret.Y);
		return ret;
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	Rect ValueTweaker::TweakableValue::asRect()
	{
		Rect ret = Walaber::Rect(Vector2::Zero, Vector2::One);
		if (NativeType == VDT_Rect)
			sscanf((const char*)Value.c_str(), "%f %f %f %f", &ret.upper_left.X, &ret.upper_left.Y, &ret.size.X, &ret.size.Y);
		return ret;
		
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	Color ValueTweaker::TweakableValue::asColor()
	{
		Color ret = Walaber::Color::White;
		if (NativeType == VDT_Color)
		{
			int r, g, b, a;
			sscanf((const char*)Value.c_str(), "%d %d %d %d", &r, &g, &b, &a);
			ret = Color(r,g,b,a);
		}
		return ret;
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	void ValueTweaker::TweakableValue::removeMappingForOwner( void* owner )
	{
		for (Mappings::iterator it = MappedValues.begin(); it != MappedValues.end(); ++it)
		{
			if (it->first == owner)
			{
				MappedValues.erase(it);
				return;
			}
		}
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	void ValueTweaker::TweakableValue::_fireMappings()
	{
		for (Mappings::iterator it = MappedValues.begin(); it != MappedValues.end(); ++it)
		{
			if (NativeType == VDT_String)
			{
				std::string* s = (std::string*)it->second;
				(*s) = Value;
			}
			else if (NativeType == VDT_Int)
			{
				int* i = (int*)it->second;
				(*i) = asInt();
			}
			else if (NativeType == VDT_Float)
			{
				float* f = (float*)it->second;
				(*f) = asFloat();
			}
			else if (NativeType == VDT_Vector2)
			{
				Vector2* i = (Vector2*)it->second;
				(*i) = asVector2();
			}
			else if (NativeType == VDT_Rect)
			{
				Rect* i = (Rect*)it->second;
				(*i) = asRect();
			}
			else if (NativeType == VDT_Color)
			{
				Color* i = (Color*)it->second;
				(*i) = asColor();
			}
		}
	}
	
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	std::string ValueTweaker::_ValueTypeToString( ValueDataType v )
	{
		switch (v)
		{
			case VDT_String:
				return "String";
				
			case VDT_Int:
				return "Int";
				
			case VDT_Float:
				return "Float";
				
			case VDT_Vector2:
				return "Vector2";
				
			case VDT_Rect:
				return "Rect";
				
			case VDT_Color:
				return "Color";
		}
		
		return "UNSUPORTED";
	}
	
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	ValueTweaker::ValueDataType ValueTweaker::_StringToValueType( std::string s )
	{
		s = StringHelper::toLower(s);
		
		if (s == "int")
			return VDT_Int;
		
		if (s == "float")
			return VDT_Float;
		
		if (s == "vector2")
			return VDT_Vector2;
		
		if (s == "rect")
			return VDT_Rect;
		
		if (s == "color")
			return VDT_Color;
		
		return VDT_String;
	}

	
}