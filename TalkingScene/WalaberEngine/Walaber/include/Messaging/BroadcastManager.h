#ifndef _BROADCAST_MANAGER_H_
#define _BROADCAST_MANAGER_H_

#include <vector>
#include <PropertyList.h>

namespace Walaber
{
	
/*
	BroadcastManager - a singleton class for passing messages between different	
	senders and receivers.  All receivers inherit from MessageReceiver class.
*/
	
// --==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
enum MessageCategory
{
	MC_Resources = 1,
	MC_Audio = 2,
	MC_Graphics = 4,
	MC_Gameplay = 8,
	MC_System = 16,
	MC_Analytics = 32,
	MC_ALL = MC_Resources | MC_Audio | MC_Graphics | MC_Gameplay | MC_System | MC_Analytics
};
	
enum BaseMessageID
{
	MID_AchievementNotification,
	MID_ShowAchievements,
    MID_NoAuthenticatedPlayer,
	MID_SIZE
};
	

typedef unsigned int MessageID;				// id for identifying payload.

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// base message.  messages that need to send data that cannot be contained in a PropertyList can inherit.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
class Message
{
public:
	Message( MessageCategory category, MessageID msg_id ) : Category(category), ID(msg_id) { }
	virtual ~Message() { }
	
	
	MessageCategory		Category;
	MessageID			ID;
    
    PropertyList        Properties;
};
	
// --------========--------========--------========--------========--------========--------========--------========
// Achievements have been generalized so that the message can be as well
// --------========--------========--------========--------========--------========--------========--------========
class Message_AchievementUnlocked : public Walaber::Message
{
public:
    static const std::string kPropAchievementIdentifier;
    static const std::string kPropAchievementPercent;
    
	Message_AchievementUnlocked(std::string achievement):
	Message( MC_System, MID_AchievementNotification )
	{
        Properties.setValueForKey(kPropAchievementIdentifier, Property(achievement));
        Properties.setValueForKey(kPropAchievementPercent, Property(100.0f));
    }
	
	Message_AchievementUnlocked(std::string achievement, float percent):
	Message( MC_System, MID_AchievementNotification )
	{
        Properties.setValueForKey(kPropAchievementIdentifier, Property(achievement));
        Properties.setValueForKey(kPropAchievementPercent, Property(percent));
    }
	
	~Message_AchievementUnlocked() {};
};

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
class MessageReceiver
{
public:
	MessageReceiver( MessageCategory subscribeTo );
	virtual ~MessageReceiver();
	
	void setMessageCategory( MessageCategory subscribeTo ) { mSubscribedMessages = subscribeTo; }
	
	// used when this receiver gets a message.  return true to "consume" this message, and prevent any further receivers from receiving it.
	virtual bool messageRx( const Message& msg ) { return false; }
	
protected:
	friend class BroadcastManager;

	MessageCategory	mSubscribedMessages;
};

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
class BroadcastManager
{
public:
	
	static BroadcastManager* getInstancePtr();
	
	// send a message!
	void messageTx( const Message& msg );
	
private:
	BroadcastManager();
	~BroadcastManager();
	
	
	void _addReceiver( MessageReceiver* rx );
	void _removeReceiver( MessageReceiver* rx );
	
	friend class MessageReceiver;
	
	static BroadcastManager*	mInstancePtr;
	
	
	typedef std::vector<MessageReceiver*> MessageReceiverList;
	
	MessageReceiverList	mReceivers;
	
};
	
}

#endif	// _BROADCAST_MANAGER_H_