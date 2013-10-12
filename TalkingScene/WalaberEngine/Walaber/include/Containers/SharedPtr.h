//
//  SharedPtr.h
//  WalaberEngine
//
//  Created by Jason Pecho on 9/16/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef _SHARED_PTR_H_
#define _SHARED_PTR_H_

#include <cstdio>
#include <cassert>

#include <Logger.h>

namespace Walaber
{
    namespace detail
    {        
        struct static_cast_tag{};
    }
    
    template<typename T>
    class SharedPtr
    {
    public:
        /// <Summary>
        /// default ctor
        /// </Summary>
        SharedPtr():
        mResource(0),
        mRefCount(0)
        {
            //Logger::printf("Walaber", Logger::SV_DEBUG, "SharedPtr -- default ctor\n");
        }
        
        /// <Summary>
        /// ctor that takes a resource
        /// </Summary>
        explicit SharedPtr(T* res):
        mResource(res),
        mRefCount( (res ? new int(1) : 0) )
        {
            //Logger::printf("Walaber", Logger::SV_DEBUG, "SharedPtr -- init ctor, refCount: %i\n", res ? (*mRefCount) : 0);
        }
        
        /// <Summary>
        /// copy ctor, increase refcount
        /// </Summary>
        explicit SharedPtr(const SharedPtr<T>& s):
        mResource(s.mResource),
        mRefCount(s.mRefCount)
        {
            //Logger::printf("Walaber", Logger::SV_DEBUG, "SharedPtr -- copy same type ctor\n");

            if(mResource)
            {
                ++(*mRefCount);
            }
        }
        
        /// <Summary>
        /// ctor to intialize from other convertible types
        /// </Summary>
        template<typename U>
        SharedPtr(const SharedPtr<U>& r ): 
        mResource( r.mResource ), 
        mRefCount( r.mRefCount )
        {
            //Logger::printf("Walaber", Logger::SV_DEBUG, "SharedPtr -- copy other type ctor\n");

            if(mResource)
            {
                ++(*mRefCount);
            }
        }
        
        /// <Summary>
        /// static cast constructor
        /// </Summary>
        template <typename U>
        SharedPtr(const SharedPtr<U>& s, detail::static_cast_tag sc):
        mResource(static_cast<T*>(s.mResource)),
        mRefCount(s.mRefCount)
        {
            //Logger::printf("Walaber", Logger::SV_DEBUG, "SharedPtr -- static ptr cast ctor\n");

            if(mResource)
            {
                ++(*mRefCount);
            }
        }
        
        /// <Summary>
        /// operator= passes the pointer, increases ref count and decreases previous refcount
        /// </Summary>
        SharedPtr& operator=(const SharedPtr<T>& s)
        {
            //Logger::printf("Walaber", Logger::SV_DEBUG, "SharedPtr -- same type op=\n");

            // check to make sure that we're not assigning the same resource
            if(mResource == s.mResource)
            {
                return *this;
            }
            
            if(mResource)
            {
                if(--(*mRefCount) == 0)
                {
                    //Logger::printf("Walaber", Logger::SV_DEBUG, "SharedPtr op=  -- destroy resource\n");

                    delete mResource; 
                    delete mRefCount;
                }
            }
            
            mResource = s.mResource;
            mRefCount = s.mRefCount;
            
            // increment ref count
            if(mResource)
            {
                ++(*mRefCount);
            }
            return *this;
        }
        
        template<class Y>
        SharedPtr& operator=(const SharedPtr<Y>& s) // never throws
        {
            //Logger::printf("Walaber", Logger::SV_DEBUG, "SharedPtr -- different type op=\n");

            // check to make sure that we're not assigning the same resource
            if(mResource == s.mResource)
            {
                return *this;
            }
            
            if(mResource)
            {
                if(--(*mRefCount) == 0)
                {
                    //Logger::printf("Walaber", Logger::SV_DEBUG, "SharedPtr op=  -- destroy resource\n");

                    delete mResource; 
                    delete mRefCount;
                }
            }
            
            mResource = s.mResource;
            mRefCount = s.mRefCount;
            
            // increment ref count
            if(mResource)
            {
                ++(*mRefCount);
            }
            return *this;
        }
        
        /// <Summary>
        /// decrease the ref count and delete resource if it drops to zero
        /// </Summary>
        ~SharedPtr()
        {
            //Logger::printf("Walaber", Logger::SV_DEBUG, ("SharedPtr -- dtor\n");
     
            if(mResource)
            {
                if(--(*mRefCount) == 0)
                {
                    //Logger::printf("Walaber", Logger::SV_DEBUG, "SharedPtr default dtor -- destroy resource\n");

                    delete mResource; 
                    delete mRefCount;
                }
            }
        }
        
        /// <Summary>
        /// give access to the resource through op->
        /// </Summary>
        T* operator->()const
        {
            //Logger::printf("Walaber", Logger::SV_DEBUG, "SharedPtr -- arrow\n");

            assert(mResource != 0);
            return mResource;
        }
        
        /// <Summary>
        /// dereference operator
        /// </Summary>
        T& operator*()
        {
            //Logger::printf("Walaber", Logger::SV_DEBUG, "SharedPtr -- deref \n");

            assert(mResource != 0);
            return *mResource;
        }
        
        /// <Summary>
        /// implicit conversion to bool
        /// </Summary>
        operator bool () const
        {
            //Logger::printf("Walaber", Logger::SV_DEBUG, "SharedPtr -- bool cast\n");

            return mResource != 0;
        }
        
        /// <Summary>
        /// not operator
        /// </Summary>
        bool operator! () const
        {
            //Logger::printf("Walaber", Logger::SV_DEBUG, "SharedPtr -- operator! \n");

            return mResource == 0;
        }
        
        /// <Summary>
        /// resets the shared ptr hold nothing
        /// </Summary>
        void reset()
        {
            //Logger::printf("Walaber", Logger::SV_DEBUG, "SharedPtr -- reset\n");

            SharedPtr<T>().swap(*this);
        }
        
        /// <Summary>
        /// get the resource
        /// </Summary>
        T* get() const
        {
            //Logger::printf("Walaber", Logger::SV_DEBUG, "SharedPtr -- get\n");

            return mResource;
        }
        
        bool unique() const
        {
            //Logger::printf("Walaber", Logger::SV_DEBUG, "SharedPtr -- unique\n");

            if(mResource)
            {
                return (*mRefCount) == 1;
            }
            
            return true;
        }
        
        int use_count()const
        {
            //Logger::printf("Walaber", Logger::SV_DEBUG, "SharedPtr --n use count\n");

            if(mResource)
            {
                return (*mRefCount);
            }
            
            return 0;
        }
        
        void swap(SharedPtr<T>& b)
        {
            //Logger::printf("Walaber", Logger::SV_DEBUG, "SharedPtr -- swap\n");

            SharedPtr<T> temp(*this);
            *this = b;
            b = temp;
        }
        
    private:
        
        // friends
        template<class U> 
        friend class SharedPtr;
        
        // members
        T*          mResource;
        int*        mRefCount;
    };
    
    template< typename T, typename U >
    void operator==(const SharedPtr<T>& lhs, const SharedPtr<U>& rhs)
    {
        return lhs.get() == rhs.get();
    }
    
    template< typename T, typename U >
    void operator!=(const SharedPtr<T>& lhs, const SharedPtr<U>& rhs)
    {
        return lhs.get() != rhs.get();
    }
    
    template< typename T, typename U >
    SharedPtr<T> static_pointer_cast(const SharedPtr<U>& sp)
    {
        return SharedPtr<T>(sp, detail::static_cast_tag());
    }
}

#endif
