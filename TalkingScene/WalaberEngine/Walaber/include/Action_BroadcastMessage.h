//
//  Action_BroadcastMessage.h
//  WalaberEngine
//
//  Created by Gulliver, Christopher on 5/28/13.
//
//

#ifndef WalaberEngine_Action_BroadcastMessage_h
#define WalaberEngine_Action_BroadcastMessage_h

#include <IAction.h>
#include <BroadcastManager.h>

namespace Walaber
{
    /*!
    Action to broadcast a message
    */
    class Action_BroadcastMessage : public IAction
    {
	public:
		Action_BroadcastMessage( Message message ) :
            mMessage( message )
		{
		}

        /*!
        Broadcast the specified message
        @param elapsedTime the time change since the last time execute was called
        @return true when all actions have been executed
        */
		virtual bool execute( float elapsedTime )
		{
            BroadcastManager::getInstancePtr()->messageTx( mMessage );
			return true;
		}

	private:
		Action_BroadcastMessage();

        Message     mMessage;
    };
    MAKE_SHARED_PTR( Action_BroadcastMessage );
}


#endif
