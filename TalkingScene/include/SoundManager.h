#ifndef __SOUND_MANAGER_H__
#define __SOUND_MANAGER_H__

#include <vector>
#include <map>
#include <string>

#include "XMLHelper.h"

#include "macros.h"
#include "Callback.h"

#include "TextManager.h"


#include "PropertyList.h"

namespace FMOD
{
    class System;
    class ChannelGroup;
    class Channel;
    class Sound;
}

namespace Walaber
{
	
	//---------------------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------------------
	class SoundResource
	{
	public:
		SoundResource( FMOD::Sound* sound );
		
		~SoundResource();
		
		FMOD::Sound*	mSound;
	};
	
	MAKE_SHARED_PTR(SoundResource);
	
	
	//---------------------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------------------
	class SoundEffectInstance
	{
	public:
		virtual ~SoundEffectInstance();
		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		enum EffectState
		{
			Playing,
			Paused,
			Stopped
		};
		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		void play(float vol = 1.0f);
		void pause();
		void resume();
		void stop();
		
		void setVolume( float v );
		void setPitch( float p );
		
		void setPosition( Vector2 p );
		void setPosition( float x, float y, float z );
		
		void setPlaybackPosition( float positionInSeconds );
		void setPlaybackPosition( unsigned int positionInSamples );
		unsigned int getPlaybackPosition();
		
		EffectState getState();
		
		unsigned int getLength();
		unsigned int getData( unsigned int start, unsigned int length, uint8_t* buffer );
		
		// returns length in SECONDS (floating point value)
		float getDuration();
		
		void saveToWAV( std::string& filePath, unsigned int startSample = 0, long lengthInSamples = -1 );
		

	private:
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		void _initChannel();
		
		void _writeWavHeader( FILE* file, int length );
		
		friend class SoundManager;
		friend class RecordableSoundEffectInstance;
		
		SoundEffectInstance( SoundResourcePtr sound, FMOD::ChannelGroup* channel_group, float base_vol, std::string subtitleText = "" );
		
		SoundResourcePtr		mSound;
		FMOD::Channel*			mChannel;
		FMOD::ChannelGroup*		mChannelGroup;
		float					mOriginalFrequency;
		float					mBaseVol;
		
		std::string				mSubtitleText;
	};
	
	MAKE_SHARED_PTR(SoundEffectInstance);
	

	//---------------------------------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------------------
	class SoundManager
	{
    private: // private structs and typedefs
        
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        struct SoundInfo
		{
			SoundInfo() : _baseVol(1.0f), _loop(false), _isMusic(false) { }
			
			std::string			_sourceFile;
			SoundResourcePtr	_sound;
			float				_baseVol;
			bool				_loop;
			bool				_isMusic;
			bool				_isVO;
			std::string			_subtitleText;			// this is the key into the TextManager for the text for this sound.
            
            bool operator==(const SoundInfo& si)
            {
                return (_sourceFile == si._sourceFile);
            }
		};
		
		typedef std::vector< SoundInfo >									SoundList;
        
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        struct SoundGroup
        {
            SoundList   liveFiles;
            SoundList   missingFiles;
        };
        
		typedef std::map< unsigned int, SoundGroup >						SoundGroupMap;
		typedef std::set< unsigned int >									InvulnerableSoundGroupList;
		
		typedef std::vector< std::string >									MusicList;
		
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        struct MusicGroup
        {
            MusicList liveFiles;
            MusicList missingFiles;
            int currentTrackID;
            
            MusicGroup():
            liveFiles(),
            missingFiles(),
            currentTrackID(-1)
            {}
        };
        
        typedef std::map< unsigned int, MusicGroup >                        MusicGroupMap;
        
        typedef std::vector< FMOD::Channel* >                               MusicChannelList;
        typedef std::vector< FMOD::Sound* >                                 MusicSoundList;
        typedef std::vector< unsigned int >                                 MusicGroupList;
		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        struct GotSoundCallbackData
		{
			PropertyList						plist;
			CallbackPtr                         callback;
		};
		
		typedef std::multimap< std::pair<int,int>, GotSoundCallbackData >	SoundInstanceCallbackMap;
		
		typedef std::multimap< std::string, GotSoundCallbackData >			SoundFileCallbackMap;
		
//		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        typedef std::map< Walaber::Language, std::string >					LanguageFolderMap;
		
		
	public:
		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        struct SoundEffectInstanceLoadedParameters
        {
            int groupID;
            int assetID;
            SoundEffectInstancePtr sound;
            PropertyList userData;
            
            SoundEffectInstanceLoadedParameters(int pgroupID, int passetID, SoundEffectInstancePtr psound, const PropertyList& puserData ):
            groupID(pgroupID),
            assetID(passetID),
            sound(psound),
            userData(puserData)
            {}            
        };	
		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        struct SoundFileInstanceLoadedParameters
        {
			ResultCode result;
			std::string filename;
            SoundEffectInstancePtr sound;
            PropertyList userData;
            
            SoundFileInstanceLoadedParameters(std::string pfile, SoundEffectInstancePtr psound, const PropertyList& puserData ):
			result(ResultCode_OK),
			filename(pfile),
            sound(psound),
            userData(puserData)
            {}            
        };	

		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        struct SubtitleCallbackData
		{
			std::string					text;		// this is the text to display (usually a key into the TextManager).
			float						duration;	// duration to display the text.
		};
		
		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // return false if the init fails, so we can clean up
		bool init( std::string& soundListFile, unsigned int sample_rate = 24000 );
        
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        bool isFMODSoundSystemInitialized() { return mSoundSystemInitialized; }
		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// set a localized asset folder mapping.  when these exist, the system will prefer 
		// to load files for the current language from the folder specified here.
		// for example, if the folderName is "es" for Spanish, and the game
		// asks for the asset "/Audio/Sounds/UI/wtf.wav", the system will first search for
		// "/Audio/Sounds/UI/es/wtf.wav", and prefer that file (if it exists).
		void setLocalizedFolderMapping( Walaber::Language lang, const std::string& folderName );
		
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // refresh a sound that has already been parsed into a group 
        // if it's a live sound, unload it and reload it
        // if it's a missing sound, move it to live sounds and load it
        // This does a linear search and string compare for every item if group is not specified
        void refreshSound(const std::string& filename, const bool load_now, const int group = -1);
        
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // refresh a sound that has already been parsed into a group 
        // if it's a live track, change its path to the user store
        // if it's a missing sound, move it to live sounds 
        // This does a linear search and string compare for every item if group is not specified
        void refreshStreamedTrack(const std::string& filename, const int group = -1);
        
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // This does a linear search and string compare for every item
        int getGroupForSoundWithFilename(const std::string& filename, bool &isMissing);
        
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		void addSound( int group, std::string filename, bool loop, float base_vol = 1.0f, bool load_now = false, bool isMusic = false, bool isVO = false );
		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		void preloadSoundGroup( int group );
		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		void playSoundFromGroup( int group, float volume = 1.0f, float pitch = 1.0f );
		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		void newSoundFromGroup( int group, CallbackPtr callback, const PropertyList& plist );
		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		unsigned int soundCountForGroup( int group );
		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		void playSoundSpecific( int groupID, int assetID, float volume = 1.0f );
        
		void playSoundSpecific( int groupID, int assetID, float volume, float pitch );        
		
		void newSoundSpecific( int groupID, int assetID, CallbackPtr callback, const PropertyList& plist );
		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		void playSoundFile( const std::string& file, float volume = 1.0f, float pitch = 1.0f, bool isVO = false );
		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		void getSoundFile( const std::string& file, CallbackPtr callback, const PropertyList& plist, bool isVO = false );
		
		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		bool systemMusicIsPlaying();
	    
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// checks is system music is playing, and if so, it
		// mutes the internal "music" sound group, so we don't interrupt
		// system music with game music.  should be called whenever you think
		// the system music setting might have changed (app launch, regaining foreground, etc)
		void updateMusicMuteStatus();
		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        bool playMusicFromGroup(int groupID, bool restartIfPlaying = false);
        bool playNextTrackInGroup(int groupID);
		bool playCurrentTrackInGroup(int groupID);
        bool playSpecificMusicFromGroup( int groupID, int track);
        
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        bool playHubMusicFromGroup(int groupID, float fadeLength);
        void clearHubMusicChannels();
        
        bool isHubMusicPlaying();
		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        void stopMusic();
		void pauseMusic();
		void resumeMusic();
		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        bool isMusicPlaying();
		bool isMusicPaused();
		
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        int getCurrentMusicGroup() { return mCurrentMusicGroup; }
		int getCurrentMusicTrack() 
        { 
            int ret = -1;
            if(mCurrentMusicGroup != -1)
            {
                ret = mMusicGroups[mCurrentMusicGroup].currentTrackID;
            }
            
            return ret;
        }
        
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        unsigned int getLiveTracksInGroup(int groupID)
        {
            unsigned int ret = 0;
            
            MusicGroupMap::iterator it = mMusicGroups.find(groupID);
            if(it != mMusicGroups.end())
            {
                ret = it->second.liveFiles.size();
            }
            
            return ret;
        }
		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        unsigned int getLiveTracksInSoundGroup(int groupID)
        {
            unsigned int ret = 0;
            
            SoundGroupMap::iterator it = mSoundGroups.find(groupID);
            if(it != mSoundGroups.end())
            {
                ret = it->second.liveFiles.size();
            }
            
            return ret;
        }
        
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		void update( float elapsed );
		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		void setAllPaused( bool paused );
		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		void setSoundsOn( bool onOff ) { mSoundsOn = onOff; }
		bool getSoundsOn() { return mSoundsOn; }
		void setSoundsPaused( bool paused );
		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		void setVoicesOn( bool onOff ) { mVoicesOn = onOff; }
		bool getVoicesOn() { return mVoicesOn; }
		void setVoicesPaused( bool paused );
        void stopVoices();
		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		void setSoundVolume(float vol);
		float getSoundVolume() { return mSoundVolume; }

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		void setVoiceVolume(float vol);
		float getVoiceVolume() { return mVoiceVolume; }

		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		void setMusicVolume(float vol);
		float getMusicVolume() { return mMusicVolume; }
		void setMusicPaused( bool paused );
		
		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		void pushVolumeSettings();
		
		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		void optimize( bool forceAll = false );
		void freeGroup( int groupID, bool force = false );
		void removeGroup( int groupID );
		void unloadAllSounds();
		
		
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		void setMusicBasePath( const std::string& path ) { mMusicBasePath = path; }
		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		void debugLogMemory();
		
		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		static SoundManager* getInstancePtr();
		void teardown();
        
   		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        void clearHubMusicGroups();
		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// callbacks
		void _initConfigFileRead( void* params ); // std::string path, char* buffer, size_t length, std::string actualPath, std::string handlerName, PropertyList userData );
		void _loadSoundFileRead( void* params ); // std::string path, char* buffer, size_t length, std::string actualPath, std::string handlerName, PropertyList userData );
		void _loadSoundFileExists( void* params );

		void _loadSoundFileDirectRead( void* params );
		
#ifdef TARGET_NDK
		void _resumeStreamingMusicAtPath ( void* params );
#endif
		
		SoundResourcePtr _loadSoundResource( FileManager::ReadFileCallbackParameters* rfc, bool loop );
		
		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Subtitle system!
		void setSubtitleDisplayCallback( CallbackPtr callback ) { mSubtitleCallback = callback; }
		
		// goes through each sound that is "VO", and looks for an entry in the TextManager for this sound (based on it's _sourceFile filename).
		void initDefaultSubtitleText();
		
		void subtitlePlayed( const std::string& text, float duration );
		
        void modifyMusicFilePath( const std::string& oldPath, const std::string& newPath );
        
        // Filter ()
        void                            addUnderWaterMusicFilter();
        void                            removeUnderWaterMusicFilter();
        
    private:
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		SoundManager();
		~SoundManager();		
		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		friend class SoundEffectInstance;
		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		void _loadSound( int groupID, int assetID, CallbackPtr callback, const PropertyList& plist );

        inline bool _checkMusicPlayability();
        inline bool _streamMusicAtPath(const std::string& path, const int groupID, const int trackID);
		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		static void _oneShotCallback( void* data)
		{
            SoundEffectInstanceLoadedParameters* params = (SoundEffectInstanceLoadedParameters*)data;
            
			float vol = 1.0f;
			float pitch = 1.0f;
			
			if (params->userData.keyExists("vol"))
				vol = params->userData["vol"].asFloat();

			if (params->userData.keyExists("pitch"))
				pitch = params->userData["pitch"].asFloat();
			
			params->sound->play( vol );

			if (pitch != 1.0f)
				params->sound->setPitch( pitch );
		}
		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		static void _oneShotCallbackDirectFile( void* data)
		{
            SoundFileInstanceLoadedParameters* params = (SoundFileInstanceLoadedParameters*)data;
            
			float vol = 1.0f;
			float pitch = 1.0f;
			
			if (params->userData.keyExists("vol"))
				vol = params->userData["vol"].asFloat();
			
			if (params->userData.keyExists("pitch"))
				pitch = params->userData["pitch"].asFloat();
			
			params->sound->play( vol );
			
			if (pitch != 1.0f)
				params->sound->setPitch( pitch );
		}

		
		
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		static SoundManager*			mInstancePtr;
		
		LanguageFolderMap				mFolderMap;
		
		SoundGroupMap					mSoundGroups;
		InvulnerableSoundGroupList		mInvulnerableSoundGroups;	// list of sound groups that will never be "evicted" from memory
		SoundList						mDirectLoadedSounds;
		
        MusicGroupMap                   mMusicGroups;
		MusicList						mMusicList;
		
		bool							mSoundsOn;
		bool							mVoicesOn;
		
		float							mSoundVolume;
		float							mVoiceVolume;
        float							mMusicVolume;
		
		FMOD::System*					mSystem;
		FMOD::ChannelGroup*				mGroupSFX;			// sound effects that do NOT have subtitles or potential localization 
		FMOD::ChannelGroup*				mGroupVO;			// sounds files that CAN have subtitles and/or localized versions
		FMOD::ChannelGroup*				mGroupMusic;		// music!
        
        bool                            mSoundSystemInitialized;    // flag to check if the FMOD Sound system initialization failed
		
		
		FMOD::Sound*					mMusicSound;
		FMOD::Channel*					mMusicChannel;
		
        std::string                     mMusicBasePath;
		
        int                             mCurrentMusicGroup;
        
        int                             mCurrentIndex;
		
		// for loading sounds
		SoundInstanceCallbackMap		mSoundInstanceCallbackMap;	// maps a group ID / asset ID pair to a callback for when the sound is loaded.
        
		// for loading sounds from files.
		SoundFileCallbackMap			mSoundFileCallbackMap;
		
		// subtitles
		CallbackPtr						mSubtitleCallback;
        
        // WMW specific- Hub music logic. Supports playing multiple music files using a list of channels
        
        MusicChannelList                mHubMusicChannels;
        MusicSoundList                  mHubMusicSounds;
        MusicGroupList                  mHubMusicGroups;
	};
	
}


#endif	// __SOUND_MANAGER_H__

