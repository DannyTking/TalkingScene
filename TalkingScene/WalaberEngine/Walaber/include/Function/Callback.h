//
//  Callback.h
//  WalaberEngine
//
//  Created by Jason Pecho on 9/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef _CALLBACK_H_
#define _CALLBACK_H_

#include <macros.h>

namespace
{
    typedef void(*CallbackFunctionPtr)(void*);
}

namespace Walaber
{
    class Callback
    {
    public:
        /// <Summary>
        /// Default ctor
        /// </Summary>
        Callback():
        mCallbackFunction(0)
        {}
        
        /// <Summary>
        /// init callback
        /// </Summary>
        Callback(CallbackFunctionPtr callbackFunctionPtr):
        mCallbackFunction(callbackFunctionPtr)
        {}
        
        /// <Summary>
        /// copy ctor
        /// </Summary>
        Callback(const Callback& callback):
        mCallbackFunction(callback.mCallbackFunction)
        {}
        
        /// <Summary>
        /// op=
        /// </Summary>
        Callback& operator=(const Callback& callback)
        {            
            mCallbackFunction = callback.mCallbackFunction;
            return *this;
        }        
        
        /// <Summary>
        /// dtor 
        /// </Summary>
        virtual ~Callback()
        {}
        
        /// <Summary>
        /// virtual because child function will allow a member function to be invoked
        /// </Summary>
        virtual void invoke(void* data)
        {
            if(mCallbackFunction)
            {
                mCallbackFunction(data);
            }
        }        
        
    private:        
        
        CallbackFunctionPtr    mCallbackFunction;
    };
    
    MAKE_SHARED_PTR(Callback);

    CallbackPtr CreateCallbackPtr( CallbackFunctionPtr callbackFunctionPtr );
    
}
#endif