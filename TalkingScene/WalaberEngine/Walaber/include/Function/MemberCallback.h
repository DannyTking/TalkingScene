//
//  MemberCallback.h
//  WalaberEngine
//
//  Created by Jason Pecho on 9/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef _MEMBER_CALLBACK_H_
#define _MEMBER_CALLBACK_H_

#include <Callback.h>
#include <SharedPtr.h>

namespace Walaber
{
    template<typename T>
    class MemberCallback : public Callback
    {
        typedef void(T::*MemberCallbackFunctionPtr)(void*);
        
    public:
        /// <Summary>
        /// Default ctor
        /// </Summary>
        MemberCallback():
        Callback(),
        mClassPtr(0),
        mMemberCallbackFunction(0)
        {}
        
        /// <Summary>
        /// init ctor
        /// </Summary>
        MemberCallback(T* classPtr, MemberCallbackFunctionPtr memberCallbackFunction ):
        Callback(),
        mClassPtr(classPtr),
        mMemberCallbackFunction(memberCallbackFunction)
        {}
        
        /// <Summary>
        /// init ctor
        /// </Summary>
        MemberCallback(const MemberCallback<T>& memberCallback ):
        Callback(memberCallback),
        mClassPtr(memberCallback.mClassPtr),
        mMemberCallbackFunction(memberCallback.mMemberCallbackFunction)
        {}
        
        /// <Summary>
        /// op=
        /// </Summary>
        MemberCallback& operator=(const MemberCallback<T>& memberCallback )
        {
            Callback::operator=(memberCallback);
            
            mClassPtr = memberCallback.mClassPtr;
            mMemberCallbackFunction = memberCallback.mMemberCallbackFunction;
        }
        
        /// <Summary>
        /// dtor
        /// </Summary>
        ~MemberCallback()
        {            
        }
        
        /// <Summary>
        /// invoke the member function passing 'data' as a param
        /// </Summary>
        void invoke(void* data)
        {
            if(mMemberCallbackFunction && mClassPtr)
            {
                (mClassPtr->*mMemberCallbackFunction)(data);
            }
        }
        
    private:
        T*                          mClassPtr;
        MemberCallbackFunctionPtr   mMemberCallbackFunction;
    };
    
    // used to make generalized SharedPtrs to these templates
    template<typename U>
    struct MemberCallbackPtr
    {
        typedef void(U::*MemberCallbackFunctionPtr)(void*);
        typedef SharedPtr< MemberCallback< U > > type;
    };

    /*!
    A helper function for creating member callback pointers, much more user friendly than the previous implementation
    @param instance a reference to the instance, usually a dereferenced "this" pointer
    @param memberCallback a member callback which takes a void*
    @return CallbackPtr representing a shared pointer to the member callback to be invoked later
    */
    template< typename T>
    CallbackPtr CreateMemberCallbackPtr( T & instance, typename MemberCallbackPtr<T>::MemberCallbackFunctionPtr memberCallback )
    {
        MemberCallback<T> * callback = new MemberCallback<T>( & instance, memberCallback) ;
        return typename MemberCallbackPtr<T>::type( callback );
    }
}

#endif