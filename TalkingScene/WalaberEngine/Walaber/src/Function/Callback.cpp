//
//  Callback.cpp
//  WalaberEngine
//
//  Created by Jason Pecho on 9/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "Callback.h"

namespace Walaber
{
    /*!
    A helper function for creating callback pointers more user friendly
    @param callbackFunctionPtr a callback which takes a void*
    @return CallbackPtr representing a shared pointer to the callback to be invoked later
    */
    CallbackPtr CreateCallbackPtr( CallbackFunctionPtr callbackFunctionPtr )
    {
        return CallbackPtr( new Callback( callbackFunctionPtr ) );
    }    
}