//
//  Action_PlayGroupSound.h
//  WheresMyMickey
//
//  Created by Mathur, Eshan on 5/6/13.
//
//

#ifndef _WALABER_ACTION_PLAY_GROUP_SOUND_
#define _WALABER_ACTION_PLAY_GROUP_SOUND_
#include <IAction.h>
#include <SoundManager.h>
#include <MemberCallback.h>

namespace Walaber
{
    
    class Action_PlayGroupSound : public IAction
    {
    public:
        /*!
         @param groupID Plays a sound from the specified group ID.
         @param waitForFinish If true, action does not complete until group sound is finished.
         */
        Action_PlayGroupSound( int groupID, bool waitForFinish ) :
        mGroupID(groupID),
        mWaitForFinish(waitForFinish),
        mSoundLoaded(false)
        {
            // Get pointer if neccessary
            if ( mWaitForFinish )
            {
                CallbackPtr callback = CreateMemberCallbackPtr(*this, &Action_PlayGroupSound::_gotSound);
                PropertyList plist;
                SoundManager::getInstancePtr()->newSoundFromGroup(mGroupID, callback, plist);
            }
            else
            {
                mSoundLoaded = true; // We're not responsible for the sound.
            }
        }
        
        /*!
         @param elapsedTime the time change since the last time execute was called
         @return true when group sound is finished playing or when immediately, depending.
         */
        virtual bool execute( float elapsedTime )
        {
            if( !mWaitForFinish )
            {
                SoundManager::getInstancePtr()->playSoundFromGroup(mGroupID);
                return true;
            }
            else
            {
                if (mSoundLoaded)
                {
                    if (mSound)
                    {
                        if (mSound->getState() == SoundEffectInstance::Stopped)
                        {
                            // We're done playing!
                            return true;
                        }
                    }
                    else
                    {
                        // We couldn't load the sound properly, so just finish the action.
                        Logger::printf("Actions", Logger::SV_ERROR, "Action_PlayGroupSound - couldn't load the sound properly.");
                        return true;
                    }
                }
                
                return false;
            }
        }
        
        /*! Callback for when the sound is loaded */
        void _gotSound(void *data)
        {
            SoundManager::SoundEffectInstanceLoadedParameters* params = (SoundManager::SoundEffectInstanceLoadedParameters*)data;
            
			// Play it immediately and take note that it's been loaded.
            if (params->sound)
            {
                mSound = params->sound;
                params->sound->play();
            }
            
            mSoundLoaded = true;
        }
        
    protected:
        int mGroupID;
        bool mWaitForFinish;
        SoundEffectInstancePtr mSound;
        bool mSoundLoaded;
        
    private:
        Action_PlayGroupSound();
    };
    MAKE_SHARED_PTR(Action_PlayGroupSound);
}

#endif
