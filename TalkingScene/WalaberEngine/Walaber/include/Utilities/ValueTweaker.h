#ifndef _VALUE_TWEAKER_H_
#define _VALUE_TWEAKER_H_

#include <string>
#include <map>
#include <vector>

#include <Vector2.h>
#include <Rect.h>
#include <Color.h>

#include "macros.h"

namespace Walaber
{
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
	/// class that holds a variety of values that are constants, or other widely-shared values.
	/// supports signing up for automatic "pushing" of new values to a destination variable.
	class ValueTweaker
	{
	public:
		// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
		// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
		enum ValueDataType
		{
			VDT_String,
			VDT_Int,
			VDT_Float,
			VDT_Vector2,
			VDT_Rect,
			VDT_Color
		};
		
		
		// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
		// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
		// sets (and adds if it doesn't already exist) a value for a given tweakable value.  if datatype does not match the existing entry,
		// value is not changed.
		static void setTweakableValue( std::string name, std::string val, ValueDataType dataType = VDT_String );
		static void setTweakableValue( std::string name, int val );
		static void setTweakableValue( std::string name, float val );
		static void setTweakableValue( std::string name, Vector2 val );
		static void setTweakableValue( std::string name, Rect val );
		static void setTweakableValue( std::string name, Color val );
		
		// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
		// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
		// add a mapping from a given tweakable value, to an external variable.  after mapping, any time the tweakable
		// value is changed, these mapped variables will also be updated with the new value.
		static void mapTweakableValue( std::string name, std::string* mappedVariable, void* owner );
		static void mapTweakableValue( std::string name, int* mappedVariable, void* owner );
		static void mapTweakableValue( std::string name, float* mappedVariable, void* owner );
		static void mapTweakableValue( std::string name, Vector2* mappedVariable, void* owner );
		static void mapTweakableValue( std::string name, Rect* mappedVariable, void* owner );
		static void mapTweakableValue( std::string name, Color* mappedVariable, void* owner );
		
		// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
		// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
		// remove all mappings for a given owner.
		static void removeMappingsForOwner( void* owner );
		
		
		// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
		// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
		// query tweakable values.
		static std::string getTweakableValueString( std::string name );
		static int getTweakableValueInt( std::string name );
		static float getTweakableValueFloat( std::string name );
		static Vector2 getTweakableValueVector2( std::string name );
		static Rect getTweakableValueRect( std::string name );
		static Color getTweakableValueColor( std::string name );
		
		
		// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
		// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
		// load tweakable values from a file.
		static void loadFromFile( std::string filename );
		
		
	private:
		// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
		// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
		// represents a "tweakable" value.  stored in memory as a string, but can be interpreted as many other common types.
		struct TweakableValue
		{
			ValueDataType	NativeType;
			
			void setValue( std::string );
			void setValue( int v );
			void setValue( float v );
			void setValue( Vector2 v );
			void setValue( Rect v );
			void setValue( Color v );
			
			std::string asString();
			int asInt();
			float asFloat();
			Vector2 asVector2();
			Rect asRect();
			Color asColor();
			
			void removeMappingForOwner( void* owner );
			void _fireMappings();
			
			typedef std::map< void*, void* >		Mappings;	// first pointer is "owner", second is the variable that's mapped to this.
			
			Mappings		MappedValues;
			
		private:
			
			
			std::string		Value;
			
			
		};
		
		static std::string _ValueTypeToString( ValueDataType v );
		static ValueDataType _StringToValueType( std::string s );
		
		// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
		// --~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~--~
		typedef std::map< std::string, TweakableValue >		TweakableValueMap;
		
		
		static TweakableValueMap		mTweakables;
		
		
	};
	
}

#endif	// _VALUE_TWEAKER_H_
