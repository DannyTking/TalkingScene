#include "BroadcastManager.h"
#include "Logger.h"

namespace Walaber
{

	const std::string Message_AchievementUnlocked::kPropAchievementIdentifier = "ach_identifier";
    const std::string Message_AchievementUnlocked::kPropAchievementPercent = "ach_percent";
	
//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------
MessageReceiver::MessageReceiver( MessageCategory subscribeTo )
{
	// remember what type of messages we want to receive.
	mSubscribedMessages = subscribeTo;
	
	// register ourselves with the singleton.
	BroadcastManager::getInstancePtr()->_addReceiver( this );
}

//------------------------------------------------------------------------
MessageReceiver::~MessageReceiver()
{
	//Logger::printf("Walaber", Logger::SV_DEBUG, "MessageReceiver::~MessageReceiver()\n");

	// remove ourselves from the singleton.
	BroadcastManager::getInstancePtr()->_removeReceiver( this );
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------
BroadcastManager* BroadcastManager::mInstancePtr = 0;

BroadcastManager::BroadcastManager()
{
	Logger::printf("Walaber", Logger::SV_DEBUG, "BroadcastManager::BroadcastManager()\n");
}

//------------------------------------------------------------------------
BroadcastManager::~BroadcastManager()
{
	mInstancePtr = 0;
}

//------------------------------------------------------------------------
BroadcastManager* BroadcastManager::getInstancePtr()
{
	if (mInstancePtr == NULL)
	{
		mInstancePtr = new BroadcastManager();
	}
	
	return mInstancePtr;
}

//------------------------------------------------------------------------
void BroadcastManager::messageTx( const Message& msg )
{
    // This function is called A LOT. Only add spew if you are currently debugging.
	// Logger::printf("Walaber", Logger::SV_VERBOSE, "BroadcastManager::messageTx( cat:%d, id:%d )...\n", msg.Category, msg.ID );

	// broadcast this message to all receivers who want to receive it's category.
	for (MessageReceiverList::iterator it = mReceivers.begin(); it != mReceivers.end(); it++)
	{
		//Logger::printf("Walaber", Logger::SV_VERBOSE, "  cat:%d, id:%d  >> receiver(%d) cat:%d\n", msg.mCategory, msg.mID, (*it), (*it)->mSubscribedMessages);
		MessageReceiver* msg_rcvr = (*it);

		if (msg_rcvr->mSubscribedMessages & msg.Category)
		{
			if (msg_rcvr->messageRx( msg )) { break; }
		}
	}
}

//------------------------------------------------------------------------
void BroadcastManager::_addReceiver( MessageReceiver* rx )
{
	Logger::printf("Walaber", Logger::SV_VERBOSE, "BroadcastManager::_addReceiver( %p ) count %d\n", rx, (int)mReceivers.size() );

	mReceivers.push_back( rx );
}

//------------------------------------------------------------------------
void BroadcastManager::_removeReceiver( MessageReceiver* rx )
{
	Logger::printf("Walaber", Logger::SV_VERBOSE, "BroadcastManager::_removeReceiver( %p )...", rx );
	
	for (MessageReceiverList::iterator it = mReceivers.begin(); it != mReceivers.end(); it++)
	{
		if ((*it) == rx)
		{
			Logger::printf("Walaber", Logger::SV_VERBOSE, "found.  new count %d\n", (int)mReceivers.size());

			mReceivers.erase( it );
			return;
		}
	}
	
	Logger::printf("Walaber", Logger::SV_FATAL, "not found!  new count %d\n", (int)mReceivers.size());
}


}