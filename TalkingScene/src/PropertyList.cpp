/*
 *  PropertyList.cpp
 *  WaterConcept
 *
 *  Created by Jason Pecho on 3/14/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "PropertyList.h"

#include <cassert>

namespace Walaber
{
	// -------------==============-------------==============-------------==============-------------===
    PropertyList::PropertyList()
	{
		
	}
	
	// -------------==============-------------==============-------------==============-------------===
    PropertyList::~PropertyList()
	{
		
	}
	
	// -------------==============-------------==============-------------==============-------------===
    PropertyList& PropertyList::operator=(const PropertyList& plist)
    {
        mPropertyMap = plist.mPropertyMap;
        return *this;
    }
	
	// -------------==============-------------==============-------------==============-------------===
    bool PropertyList::keyExists( const std::string& key )const
	{
		return (mPropertyMap.find(key) != mPropertyMap.end());		
	}
	
	// -------------==============-------------==============-------------==============-------------===
    void PropertyList::setValueForKey( const std::string& key, const Property& value )
	{
		mPropertyMap[key] = value;
	}
	
	// -------------==============-------------==============-------------==============-------------===
    const Property* PropertyList::getValueForKey( const std::string& key )const
	{
		const Property* property=0;
		
		PropertyMap::const_iterator it = mPropertyMap.find(key);
		if (it != mPropertyMap.end()) 
		{
			property = &(*it).second;
		}
		
		return property;
	}
	
	// -------------==============-------------==============-------------==============-------------===
    Property* PropertyList::getValueForKey( const std::string& key )
	{
		Property* property=0;
		
		PropertyMap::iterator it = mPropertyMap.find(key);
		if (it != mPropertyMap.end()) 
		{
			property = &(*it).second;
		}
		
		return property;
	}
	
	// -------------==============-------------==============-------------==============-------------===
    void PropertyList::removeValueForKey( const std::string& key )
	{
		PropertyMap::iterator it = mPropertyMap.find(key);
		if (it != mPropertyMap.end())
		{
			mPropertyMap.erase(it);
		}
	}
	
	// -------------==============-------------==============-------------==============-------------===
    Property& PropertyList::operator[](std::string& key)
	{
		return mPropertyMap[key];
	}
	
	// -------------==============-------------==============-------------==============-------------===
    const Property& PropertyList::operator[](const std::string& key)const
	{
		assert(mPropertyMap.find(key) != mPropertyMap.end());
		PropertyMap::const_iterator it = mPropertyMap.find(key);
		return (*it).second;		
	}
}