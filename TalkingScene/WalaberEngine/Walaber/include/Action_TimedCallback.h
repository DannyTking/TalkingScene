//
//  Action_TimedCallback.h
//  WalaberEngine
//
//  Created by Gulliver, Christopher on 5/28/13.
//
//

#ifndef WalaberEngine_Action_TimedCallback_h
#define WalaberEngine_Action_TimedCallback_h

// Walaber Includes
#include <IAction.h>
#include <ActionList.h>
#include <Action_Callback.h>
#include <Action_Delay.h>
#include <PropertyList.h>

namespace Walaber
{

    /*!
    Helper action which performs an ActionDelay, then an ActionCallback
    */
    class Action_TimedCallback : public ActionSequence
    {
    public:
        Action_TimedCallback( float time, CallbackPtr callback, PropertyList * propertyList = NULL ) :
            mPropertyList   ( propertyList )
        {
            addAction( CreateActionPtr( new Action_Delay( time ) ) );
            addAction( CreateActionPtr( new Action_Callback( callback, mPropertyList ) ) );
        }

    private:
        Action_TimedCallback();

        PropertyList *      mPropertyList;
    };
    MAKE_SHARED_PTR( Action_TimedCallback );
}

#endif
