//
//  Utilities.h
//  WalaberEngine
//
//  Created by Jason Pecho on 9/15/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef _UTILITIES_H_
#define _UTILITIES_H_

#include <map>
#include <string>

namespace Walaber{
namespace Utilities{

    template <class Key, class Value>
    class create_map
    {
    public:
        create_map(const Key& k, const Value& v)
        {
            mMap[k] = v;
        }
        
        create_map<Key, Value>& operator()(const Key& k, const Value& v)
        {
            mMap[k] = v;
            return *this;
        }
        
        operator std::map<Key, Value>()
        {
            return mMap;
        }
        
    private:
        std::map<Key, Value> mMap;
    };
    
    namespace noncopyable_
    {
    class noncopyable
    {
    protected:
        noncopyable() {};
        ~noncopyable() {};
    private: // emphasize the following members are private
        noncopyable( const noncopyable& );
        const noncopyable& operator=( const noncopyable& );        
    };
    }
    
    typedef noncopyable_::noncopyable noncopyable;
	
	std::string generateGUID();
}
}

#endif
