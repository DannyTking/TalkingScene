//
//  Action_CallbackUntilTrue.h
//  WalaberEngine
//
//  Created by Gulliver, Christopher on 5/28/13.
//
//

#ifndef WalaberEngine_Action_CallbackUntilTrue_h
#define WalaberEngine_Action_CallbackUntilTrue_h

#include <Action.h>
#include <Action_Callback.h>
#include <PropertyList.h>

namespace Walaber
{
    class Action_CallbackUntilTrue : public Action_Callback
    {
    public:
        Action_CallbackUntilTrue( bool & conditionVariable,
                                  CallbackPtr callback,
                                  PropertyList * propertyList = NULL ) :
            Action_Callback( callback, propertyList ),
            mConditionVariable( conditionVariable )
        {
        }

        Action_CallbackUntilTrue( bool & conditionVariable,
                                  Callback * callback,
                                  PropertyList * propertyList = NULL ) :
            Action_Callback( callback, propertyList ),
            mConditionVariable( conditionVariable )
        {
        }
        
        /*!
        Broadcast the specified message
        @param elapsedTime the time change since the last time execute was called
        @return true when all actions have been executed
        */
		virtual bool execute( float elapsedTime )
		{
            if ( !mConditionVariable )
            {
                Action_Callback::execute( elapsedTime );
                return false;
            }
			return true;
		}
    private:
        bool & mConditionVariable;
    };
}

#endif
