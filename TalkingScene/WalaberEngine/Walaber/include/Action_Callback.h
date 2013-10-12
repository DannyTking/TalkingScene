//
//  Action_Callback.h
//  WalaberEngine
//
//  Created by Gulliver, Christopher on 5/28/13.
//
//

#ifndef WalaberEngine_Action_Callback_h
#define WalaberEngine_Action_Callback_h

// Walaber Includes
#include <IAction.h>
#include <Callback.h>
#include <MemberCallback.h>
#include <PropertyList.h>

namespace Walaber
{

    /*! An action that performs a callback when it is executed
    */
    class Action_Callback : public IAction
    {
    public:
        Action_Callback( CallbackPtr callback, PropertyList * propertyList = NULL ) :
            mCallback( callback )
        {
            if ( propertyList != NULL )
            {
                mPropertyList = *propertyList;
            }
        }
        
        Action_Callback( Callback * callback, PropertyList * propertyList = NULL ) :
            mCallback( CallbackPtr( callback ) )
        {
            if ( propertyList != NULL )
            {
                mPropertyList = *propertyList;
            }
        }

        /*!
        Execute the callback
        @param elapsedTime the time change since the last time execute was called
        @return always true 
        */
        virtual bool execute( float elapsedTime )
        {
            if ( mCallback )
            {
                mCallback->invoke( &mPropertyList );
            }
            return true;
        }

    private:
        Action_Callback();
        
        CallbackPtr  mCallback;
        PropertyList mPropertyList;
        
    };
    MAKE_SHARED_PTR( Action_Callback );
}

#endif
