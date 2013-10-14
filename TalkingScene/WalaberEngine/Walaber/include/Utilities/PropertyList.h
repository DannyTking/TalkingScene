/*
 *  PropertyList.h
 *  WaterConcept
 *
 *  Created by Jason Pecho on 3/14/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _PROPERTY_LIST_H_
#define _PROPERTY_LIST_H_

#include <Property.h>
#include <string>
#include <map>

namespace Walaber 
{
	// -------------==============-------------==============-------------==============-------------===
    // -------------==============-------------==============-------------==============-------------===
    class PropertyList 
	{
	public:
		// -------------==============-------------==============-------------==============-------------===
		// typedefs
		typedef std::map< std::string, Property >			PropertyMap;
		
		
		// -------------==============-------------==============-------------==============-------------===
		PropertyList();
		~PropertyList();
		
		// -------------==============-------------==============-------------==============-------------===
		PropertyList& operator=(const PropertyList& plist);
		
		// -------------==============-------------==============-------------==============-------------===
		// check if a key exists
		bool keyExists( const std::string& key )const;
		
		// -------------==============-------------==============-------------==============-------------===
		size_t size() const { return mPropertyMap.size(); }
		
		// -------------==============-------------==============-------------==============-------------===
		void setValueForKey( const std::string& key, const Property& value );
		
		// -------------==============-------------==============-------------==============-------------===
		// these check if it exists and return NULL if it doesn't		
		const Property* getValueForKey( const std::string& key )const;
		Property* getValueForKey( const std::string& key );
		
		// -------------==============-------------==============-------------==============-------------===
		void removeValueForKey( const std::string& key );
		
		// -------------==============-------------==============-------------==============-------------===
		// these do not do bounds checks but return default objects of key is not found
		Property& operator[](std::string& key);
		const Property& operator[](const std::string& key)const;
		
		
		// -------------==============-------------==============-------------==============-------------===
		PropertyMap::const_iterator begin() const { return mPropertyMap.begin(); }
		PropertyMap::const_iterator end() const { return mPropertyMap.end(); }
		
		
		
	private:
		
		// -------------==============-------------==============-------------==============-------------===
		PropertyMap			mPropertyMap;
	};
}

#endif
