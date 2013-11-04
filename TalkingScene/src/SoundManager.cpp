#include "SoundManager.h"

#include "PlatformManager.h"
#include "WalaberMath.h"
#include "MemberCallback.h"

#include "FileHelper.h"
#include "StringHelper.h"

#ifdef TARGET_NDK
#include <fmod.h>
#elif defined(TARGET_IPHONE)
#include <fmodiphone.h>
#else
#include <fmod.h>
#endif

#include <fmod_memoryinfo.h>

#include "fmod.hpp"
#include "fmod_errors.h"


#define __PACKED __attribute__((packed))    /* gcc packed */

namespace Walaber
{
    // --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
    SoundResource::SoundResource( FMOD::Sound* sound ) : mSound(sound) 
    { 
        char namebuffer[64];
        mSound->getName(namebuffer, 64);
    }
    
    SoundResource::~SoundResource() 
    { 
        char namebuffer[64];
        mSound->getName(namebuffer, 64);
        
        if (mSound)
            mSound->release(); 
		
        mSound = NULL; 
    }
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	SoundEffectInstance::SoundEffectInstance( SoundResourcePtr sound, FMOD::ChannelGroup* channel_group, float base_vol, std::string subtitleText ) : 
	mSound(sound), 
	mChannel(NULL), 
	mChannelGroup(channel_group),
	mBaseVol(base_vol),
	mSubtitleText(subtitleText)
	{
	}
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	SoundEffectInstance::~SoundEffectInstance()
	{
		if (mChannel != NULL)
		{
			FMOD_MODE mode;
			mChannel->getMode(&mode);
			if (mode & FMOD_LOOP_NORMAL)
			{
				// this is a looping sound...
				mChannel->setMode( FMOD_LOOP_OFF );
				stop();
			}
		}
	}
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundEffectInstance::play(float vol)
	{	
		if (vol == 0)
		{
			stop();
			return;
		}
		
		if (getState() != Playing)
		{
			_initChannel();
			mChannel->setPaused(false);
			mChannel->setVolume(vol * mBaseVol);
		}
		
		if (mSubtitleText.length() > 0)
			SoundManager::getInstancePtr()->subtitlePlayed(mSubtitleText, getDuration());
	}
	
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundEffectInstance::pause()
	{
		bool paused = true;
		
		if (mChannel != NULL)
			mChannel->setPaused(&paused);
	}
	
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundEffectInstance::resume()
	{
		if (getState() == Paused)
		{
			if (mChannel != NULL)
				mChannel->setPaused(false);
			else
				play();
		}
		else
		{
			play();
		}
	}
	
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundEffectInstance::stop()
	{
		if (mChannel != NULL)
		{
			mChannel->stop();
			mChannel = NULL;
		}
	}
	
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundEffectInstance::setVolume( float v )
	{
		if (mChannel != NULL)
		{
			mChannel->setVolume(v * mBaseVol);
		}
	}
	
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundEffectInstance::setPitch( float p )
	{
		if (mChannel != NULL)
		{
			mChannel->setFrequency(mOriginalFrequency * p);
		}
	}
	
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
//	void SoundEffectInstance::setPosition( Vector2 p )
//	{
//	}
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundEffectInstance::setPosition( float x, float y, float z )
	{
	}
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundEffectInstance::setPlaybackPosition( float positionInSeconds )
	{
		if  (mChannel != NULL)
		{
			FMOD_RESULT res = mChannel->setPosition( (int)(positionInSeconds * 1000.0f) , FMOD_TIMEUNIT_MS );
			if (res != FMOD_OK)
			{
				Logger::printf("Walaber", Logger::SV_ERROR, "ERROR setting position on sound [%s]\n", FMOD_ErrorString(res));
			}
		}
	}
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundEffectInstance::setPlaybackPosition( unsigned int positionInSamples )
	{
		if  (mChannel != NULL)
		{
			FMOD_RESULT res = mChannel->setPosition( positionInSamples , FMOD_TIMEUNIT_PCM );
			if (res != FMOD_OK)
			{
				Logger::printf("Walaber", Logger::SV_ERROR, "ERROR setting position on sound [%s]\n", FMOD_ErrorString(res));
			}
		}
	}
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	unsigned int SoundEffectInstance::getPlaybackPosition()
	{
		unsigned int positionInSamples = 0;
		if (mChannel != NULL)
		{
			FMOD_RESULT res = mChannel->getPosition( &positionInSamples, FMOD_TIMEUNIT_PCM );
			if (res != FMOD_OK)
			{
				Logger::printf("Walaber", Logger::SV_ERROR, "ERROR getting position! [%s]\n", FMOD_ErrorString(res));
			}
		}
		
		return positionInSamples;
	}
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	SoundEffectInstance::EffectState SoundEffectInstance::getState()
	{
		if (mChannel != NULL)
		{
			bool res;
			mChannel->getPaused(&res);
			if (res)
				return Paused;
			
			mChannel->isPlaying(&res);
			if (res)
				return Playing;
		}
		
		return Stopped;
	}
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	unsigned int SoundEffectInstance::getLength()
	{
		unsigned int ret = 0;
		
		FMOD_RESULT res = (*mSound).mSound->getLength( &ret, FMOD_TIMEUNIT_PCM );
		if (res != FMOD_OK)
		{
			Logger::printf("Walaber", Logger::SV_ERROR, "ERROR getting sound length! [%s]\n", FMOD_ErrorString(res));
			return 0;
		}
		
		return ret;
	}
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	float SoundEffectInstance::getDuration()
	{
		unsigned int ms;
		FMOD_RESULT res = (*mSound).mSound->getLength( &ms, FMOD_TIMEUNIT_MS );
		if (res != FMOD_OK)
		{
			Logger::printf("Walaber", Logger::SV_ERROR, "ERROR getting sound length! [%s]\n", FMOD_ErrorString(res));
			return 0.0f;
		}
		
		return ((float)ms / 1000.0f);
	}
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	unsigned int SoundEffectInstance::getData( unsigned int start, unsigned int length, uint8_t* buffer )
	{
		void*			ptr1 = NULL;
		void*			ptr2 = NULL;
		unsigned int	len1 = 0;
		unsigned int	len2 = 0;
		
		FMOD_SOUND_TYPE		sType;
		FMOD_SOUND_FORMAT	sFormat;
		int					sChannels;
		int					sBits;
		
		FMOD_RESULT res = (*mSound).mSound->getFormat( &sType, &sFormat, &sChannels, &sBits );
		if (res != FMOD_OK)
		{
			Logger::printf("Walaber", Logger::SV_ERROR, "ERROR getting sound format! [%s]\n", FMOD_ErrorString(res));
			return 0;
		}
		
		if (sFormat != FMOD_SOUND_FORMAT_PCM16)
		{
			Logger::printf("Walaber", Logger::SV_ERROR, "ERROR! Only PCM16 is supported for getData at this time!\n");
			return 0;
		}
		
		if (sChannels > 1)
		{
			Logger::printf("Walaber", Logger::SV_ERROR, "ERROR!  only single channel sound is supported for getData at this time!\n");
			return 0;
		}
		
		//modify start and length into bytes (we were given samples)...
		unsigned int bytesPerSample = sizeof(short);
		
		start *= bytesPerSample;
		length *= bytesPerSample;
		
		// lock sound.
		res = (*mSound).mSound->lock( start, length, &ptr1, &ptr2, &len1, &len2 );
		if (res != FMOD_OK)
		{
			Logger::printf("Walaber", Logger::SV_ERROR, "ERROR locking sound! [%s]\n", FMOD_ErrorString(res));
			return 0;
		}

		Logger::printf("Walaber", Logger::SV_DEBUG, "len1[%d] len2[%d]\n", len1, len2);
		
		// copy data into buffer.
		memcpy(&buffer[0], ptr1, len1);
		memcpy(&buffer[len1], ptr2, len2);
		
		// unlock.
		res = (*mSound).mSound->unlock( ptr1, ptr2, len1, len2 );
		if (res != FMOD_OK)
		{
			Logger::printf("Walaber", Logger::SV_ERROR, "ERROR unlocking sound! [%s]\n", FMOD_ErrorString(res));
			return 0;
		}
		
		return len1 + len2;
	}
	
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundEffectInstance::saveToWAV( std::string& filePath, unsigned int startSample, long lengthInSamples  )
	{
		void*			ptr1;
		void*			ptr2;
		unsigned int	len1;
		unsigned int	len2;
		
		FMOD_SOUND_TYPE		sType;
		FMOD_SOUND_FORMAT	sFormat;
		int					sChannels;
		int					sBits;
		unsigned int		sLengthPCMs;
		
		FMOD_RESULT res = (*mSound).mSound->getFormat( &sType, &sFormat, &sChannels, &sBits );
		if (res != FMOD_OK)
		{
			Logger::printf("Walaber", Logger::SV_ERROR, "ERROR getting sound format! [%s]\n", FMOD_ErrorString(res));
			return;
		}
		
		if (sFormat != FMOD_SOUND_FORMAT_PCM16)
		{
			Logger::printf("Walaber", Logger::SV_ERROR, "ERROR! Only PCM16 is supported for saveToWAV at this time!\n");
			return;
		}
		
		if (sChannels > 1)
		{
			Logger::printf("Walaber", Logger::SV_ERROR, "ERROR!  only single channel sound is supported for saveToWAV at this time!\n");
			return;
		}
		

		unsigned int bytesPerSample = sizeof(short);

		
		// get length (in samples)
		res = (*mSound).mSound->getLength( &sLengthPCMs, FMOD_TIMEUNIT_PCM );
		if (res != FMOD_OK)
		{
			Logger::printf("Walaber", Logger::SV_ERROR, "ERROR getting sound length! [%s]\n", FMOD_ErrorString(res));
			return;
		}
		
		unsigned int startByte = startSample * bytesPerSample;
		unsigned int maxWrite = (sLengthPCMs-startSample) * bytesPerSample;
		
		unsigned int byteCount = (lengthInSamples > 0) ? (lengthInSamples * bytesPerSample) : ((sLengthPCMs-startSample) * bytesPerSample);
		if (byteCount > maxWrite) { byteCount = maxWrite; }
		
		// lock the section of the sound we want...
		res = (*mSound).mSound->lock( startByte, byteCount, &ptr1, &ptr2, &len1, &len2 );
		if (res != FMOD_OK)
		{
			Logger::printf("Walaber", Logger::SV_ERROR, "ERROR locking sound! [%s]\n", FMOD_ErrorString(res));
			return;
		}
		
		// OK... now write this to disk!
		FILE* file = fopen( filePath.c_str(), "wb" );
		if (file != NULL)
		{
			_writeWavHeader(file, byteCount );
			
			// now write the data.
			fwrite(ptr1, 1, len1, file);
			fwrite(ptr2, 1, len2, file);
			
			fclose(file);
		}
		else
		{
			Logger::printf("Walaber", Logger::SV_ERROR, "ERROR opening file!\n");
		}
		
		// unlock the sound.
		res = (*mSound).mSound->unlock( ptr1, ptr2, len1, len2 );
		if (res != FMOD_OK)
		{
			Logger::printf("Walaber", Logger::SV_ERROR, "ERROR unlocking sound! [%s]\n", FMOD_ErrorString(res));
			return ;
		}
	}
	
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundEffectInstance::_initChannel()
	{
		if (mChannel == NULL)
		{
			// get the system object for this sound via FMOD.
			FMOD::System* system = NULL;
			FMOD_RESULT res = (*mSound).mSound->getSystemObject(&system);
			if (res != FMOD_OK)
			{
				Logger::printf("Walaber", Logger::SV_ERROR, "ERROR getting FMOD System object! [%s]\n", FMOD_ErrorString(res));
				return;
			}
			
			res = system->playSound(FMOD_CHANNEL_FREE, (*mSound).mSound, true, &mChannel);
			if (res != FMOD_OK)
			{
				Logger::printf("Walaber", Logger::SV_ERROR, "ERROR playing sound! [%s]\n", FMOD_ErrorString(res));
				mChannel = NULL;
				return;
			}
			
			// add to the requested channel group.
			mChannel->setChannelGroup( mChannelGroup );
			
			res = mChannel->getFrequency( &mOriginalFrequency );
			if (res != FMOD_OK)
			{
				Logger::printf("Walaber", Logger::SV_ERROR, "ERROR getting frequency for channel! [%s]\n", FMOD_ErrorString(res));
			}
		}
	}
	
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundEffectInstance::_writeWavHeader( FILE* file, int length )
	{
		int               channels;
		int               bits;
		float             rate;
		FMOD_SOUND_FORMAT format;
		
		if (!mSound)
		{
			return;
		}
		
		fseek(file, 0, SEEK_SET);
		
		(*mSound).mSound->getFormat(NULL, &format, &channels, &bits);
		(*mSound).mSound->getDefaults(&rate, NULL, NULL, NULL);
		
		{
			/*
			 WAV Structures
			 */
			typedef struct
			{
				signed char id[4];
				int 		size;
			} RiffChunk;
			
			struct
			{
				RiffChunk       chunk           __PACKED;
				unsigned short	wFormatTag      __PACKED;    /* format type  */
				unsigned short	nChannels       __PACKED;    /* number of channels (i.e. mono, stereo...)  */
				unsigned int	nSamplesPerSec  __PACKED;    /* sample rate  */
				unsigned int	nAvgBytesPerSec __PACKED;    /* for buffer estimation  */
				unsigned short	nBlockAlign     __PACKED;    /* block size of data  */
				unsigned short	wBitsPerSample  __PACKED;    /* number of bits per sample of mono data */
			} __PACKED FmtChunk  = { {{'f','m','t',' '}, sizeof(FmtChunk) - sizeof(RiffChunk) }, 1, channels, (int)rate, (int)rate * channels * bits / 8, 1 * channels * bits / 8, bits };
			
			if (format == FMOD_SOUND_FORMAT_PCMFLOAT)
			{
				FmtChunk.wFormatTag = 3;
			}
			
			struct
			{
				RiffChunk   chunk;
			} DataChunk = { {{'d','a','t','a'}, length } };
			
			struct
			{
				RiffChunk   chunk;
				signed char rifftype[4];
			} WavHeader = { {{'R','I','F','F'}, sizeof(FmtChunk) + sizeof(RiffChunk) + length }, {'W','A','V','E'} };
			
			/*
			 Write out the WAV header
			 */
			fwrite(&WavHeader, sizeof(WavHeader), 1, file);
			fwrite(&FmtChunk, sizeof(FmtChunk), 1, file);
			fwrite(&DataChunk, sizeof(DataChunk), 1, file);
		}
	}
	
	
	
	
	SoundManager* SoundManager::mInstancePtr = 0;

	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	SoundManager::SoundManager() : 
	mSoundsOn(false), 
	mSystem(NULL), 
	mGroupSFX(NULL), 
	mGroupVO(NULL),
	mGroupMusic(NULL), 
	mMusicSound(NULL), 
	mMusicChannel(NULL),
    mCurrentMusicGroup(-1),
    mCurrentIndex(0)
	{
		setSoundVolume(1.0f);
		setVoiceVolume(1.0f);
		setMusicVolume(0.3f);
		
		setSoundsOn(true);
		setVoicesOn(true);
	}


	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	SoundManager::~SoundManager()
	{
		// stop all sounds...
		mGroupVO->stop();
		mGroupSFX->stop();
		mGroupMusic->stop();

		unloadAllSounds();
		
		if (mSystem != NULL)
		{
			stopMusic();
			
			mGroupVO->release();
			mGroupSFX->release();
			mGroupMusic->release();

			
			mSystem->close();
			mSystem->release();
			mSystem = NULL;
		}
	}

	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	bool SoundManager::init( std::string& soundListFile, unsigned int sample_rate )
	{		
		// init sound engine.
		FMOD_RESULT result = FMOD_OK;
        mSoundSystemInitialized = false;
		
		result = FMOD::System_Create(&mSystem); 
		if (result != FMOD_OK)
		{
			Logger::printf("Walaber", Logger::SV_ERROR, "ERROR creating FMOD sound system! [%s]\n", FMOD_ErrorString(result));
			return false;
		}

		unsigned int version = 0;
		result = mSystem->getVersion(&version);
		if (version < FMOD_VERSION)
		{
			Logger::printf("Walaber", Logger::SV_ERROR, "ERROR You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
			return false;
		}

#ifdef TARGET_IPHONE
		// set iPhone-specific stuff here...
		FMOD_IPHONE_EXTRADRIVERDATA exData;
		memset(&exData, 0x0, sizeof(FMOD_IPHONE_EXTRADRIVERDATA));
		exData.sessionCategory = FMOD_IPHONE_SESSIONCATEGORY_AMBIENTSOUND;
		exData.forceSpeakerOutput = true;
		
		result = mSystem->init(64, FMOD_INIT_NORMAL, &exData);
#else
		result = mSystem->init(64, FMOD_INIT_NORMAL, 0);
#endif
		if (result != FMOD_OK)
		{
			Logger::printf("Walaber", Logger::SV_ERROR, "ERROR initializing FMOD sound system! [%s]\n", FMOD_ErrorString(result));
			return false;
		}
		
		// set default sample rate...
		int sampleRate;
		FMOD_SOUND_FORMAT format;
		int outputChannels;
		int maxInputChannels;
		FMOD_DSP_RESAMPLER resampleMethod;
		int bits;
		
		mSystem->getSoftwareFormat(&sampleRate, &format, &outputChannels, &maxInputChannels, &resampleMethod, &bits);
		
		Logger::printf("Walaber", Logger::SV_DEBUG, "sampleRate[%d]  format[%d]  outChannels[%d] inputChannels[%d] resampleMethod[%d] bits[%d]\n", sampleRate, format, outputChannels, maxInputChannels, resampleMethod, bits);

		sampleRate = sample_rate;
		mSystem->setSoftwareFormat(sampleRate, format, outputChannels, maxInputChannels, resampleMethod);
		
		
		// create sound groups...
		result = mSystem->createChannelGroup("SFX", &mGroupSFX);
		if (result != FMOD_OK)
		{
			Logger::printf("Walaber", Logger::SV_ERROR, "ERROR creating channel group for SFX! [%s]\n", FMOD_ErrorString(result));
			return false;
		}	
		
		result = mSystem->createChannelGroup("VOICE", &mGroupVO);
		if (result != FMOD_OK)
		{
			Logger::printf("Walaber", Logger::SV_ERROR, "ERROR creating channel group for VOICES! [%s]\n", FMOD_ErrorString(result));
			return false;
		}
		
		result = mSystem->createChannelGroup("MUSIC", &mGroupMusic);
		if (result != FMOD_OK)
		{
			Logger::printf("Walaber", Logger::SV_ERROR, "ERROR creating channel group for MUSIC! [%s]\n", FMOD_ErrorString(result));
			return false;
		}
        
        // Set the flag, otherwise init will be called again on App Resume
        mSoundSystemInitialized = true;
		
		// load all the sounds in the list.
		PropertyList plist;
		
		MemberCallbackPtr<SoundManager>::type memberCallback( new MemberCallback<SoundManager>( this, &SoundManager::_initConfigFileRead ) );
		CallbackPtr callback = static_pointer_cast<Callback>(memberCallback);
		
		FileManager::getInstancePtr()->readFile( soundListFile, callback, plist );
        
        return true;
	}
	
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundManager::_initConfigFileRead( void* params ) // std::string path, char* buffer, size_t length, std::string actualPath, std::string handlerName, PropertyList userData )
	{
		FileManager::ReadFileCallbackParameters* rcp = (FileManager::ReadFileCallbackParameters*)params;
		
		xmlDocPtr doc;
		xmlNodePtr root_elem = XML::loadDocMemory( rcp->buffer, rcp->length, "SoundList", doc );
		
		if (rcp->buffer)
			delete[] rcp->buffer;
		
		if (root_elem == NULL)
			return;
		
		Logger::printf("Walaber", Logger::SV_DEBUG, "SoundManager::init - filename:%s\n", rcp->path.c_str());
		
		// parse available outfits!
		for (xmlNodePtr kids = root_elem->children; kids; kids = kids->next)
		{
			// Group!
			if (xmlStrcmp(kids->name, (const xmlChar*)"Group") == 0)
			{
				int groupID = XML::parseInt(kids, "id");
				bool loop = false;
				bool load_now = false;
				bool isMusic = false;
				bool isVO = false;
				
				if (XML::attrExists(kids, "loop"))
				{
					loop = XML::parseBool(kids, "loop");
				}
				
				if (XML::attrExists(kids, "alwaysInMemory"))
				{
					mInvulnerableSoundGroups.insert( groupID );
					load_now = true;
				}
				
				if (XML::attrExists(kids, "isMusic"))
				{
					isMusic = true;
				}
				
				if (XML::attrExists(kids, "isVO"))
				{
					isVO = true;
				}
				
				// parse all sounds in this set, and add them.
				for (xmlNodePtr sound = kids->children; sound; sound = sound->next)
				{
					if (xmlStrcmp(sound->name, (const xmlChar*)"Sound") == 0)
					{
						std::string filename = XML::parseString(sound, "filename");
						
						float base_vol = 1.0f;
						if (XML::attrExists(sound, "volume"))
							base_vol = XML::parseFloat(sound, "volume");
						
						addSound(groupID, filename, loop, base_vol, load_now, isMusic, isVO);
					}					
				}
			}
			// Music
			else if (xmlStrcmp(kids->name, (const xmlChar*)"Music") == 0)
			{
                // parse available outfits!
                for (xmlNodePtr groups = kids->children; groups; groups = groups->next)
                {
                    // Group!
                    if (xmlStrcmp(groups->name, (const xmlChar*)"Group") == 0)
                    {
                        int groupID = XML::parseInt(groups, "id");
                        
                        for (xmlNodePtr track = groups->children; track; track = track->next)
                        {
                            if (xmlStrcmp(track->name, (const xmlChar*)"Track") == 0)
                            {
                                std::string trackName = XML::parseString(track, "filename");
                                
                                // check if file exists
                                // check to see if the file exists
                                PropertyList plist;
                                plist.setValueForKey("group", Property( groupID ));
                                plist.setValueForKey("is_stream", Property( 1 )); 
                                
                                MemberCallbackPtr<SoundManager>::type memberCallback(new MemberCallback<SoundManager>(this, &SoundManager::_loadSoundFileExists));
                                CallbackPtr callback = static_pointer_cast<Callback>(memberCallback);
                                FileManager::getInstancePtr()->fileExists(trackName, callback, plist);
                            }
                        }
                    }
                }
			}
			// Folder mappings
			else if (xmlStrcmp(kids->name, (const xmlChar*)"FolderMappings") == 0)
			{
				for (xmlNodePtr map = kids->children; map; map = map->next)
				{
					if (xmlStrcmp(map->name, (const xmlChar*)"Mapping") == 0)
					{
						Walaber::Language l = Walaber::ENGLISH_NTSC;
						if (Walaber::TextManager::stringToLanguage(XML::parseString(map, "language"), l))
						{
							std::string folderName = XML::parseString(map, "folder");
							
							mFolderMap[ l ] = folderName;
							
							Walaber::Logger::printf("Walaber", Logger::SV_DEBUG, "SoundManager::init - found mapping for language '%s' is folder '%s'\n",
													TextManager::languageToString(l).c_str(), folderName.c_str());
							
						}
					}
				}
			}
		}
		
		XML::cleanupDoc( doc );
		
		mSoundsOn = true;
	}
    
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundManager::setLocalizedFolderMapping( Walaber::Language lang, const std::string& folderName )
	{
		mFolderMap[ lang ] = folderName;
	}
	

	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundManager::setSoundVolume(float vol)
	{
		mSoundVolume = clampf(vol, 0.0f, 1.0f);
		
		if (mGroupSFX != NULL)
			mGroupSFX->setVolume(vol);
	}
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundManager::setVoiceVolume(float vol)
	{
		mVoiceVolume = clampf(vol, 0.0f, 1.0f);
		
		if (mGroupVO != NULL)
			mGroupVO->setVolume(vol);
	}
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundManager::setMusicVolume(float vol)
	{
		mMusicVolume = clampf(vol, 0.0f, 1.0f);
		
		if (mGroupMusic != NULL)
			mGroupMusic->setVolume(vol);
	}
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundManager::pushVolumeSettings()
	{
		mGroupSFX->setVolume(mSoundVolume);
		mGroupVO->setVolume(mVoiceVolume);
		mGroupMusic->setVolume(mMusicVolume);
	}
	
    // --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
    void SoundManager::refreshSound(const std::string& filename,const bool load_now, const int group)
    {
        if(group == -1)
        {
            SoundGroupMap::iterator it = mSoundGroups.begin();
            SoundGroupMap::iterator itEnd = mSoundGroups.end();
            
            while(it != itEnd)
            {
                SoundList::iterator sndIt = it->second.liveFiles.begin();
                SoundList::iterator sndItEnd = it->second.liveFiles.end();
                unsigned int i = 0;
                while(sndIt != sndItEnd)
                {
                    if((*sndIt)._sourceFile == filename)
                    {
                        // the sound is live, so we want to reload it 
                        PropertyList plist;
                        _loadSound(it->first, i, CallbackPtr(), plist);
                        
                        return;
                    }
                    
                    ++i;
                    ++sndIt;
                }
                
                sndIt = it->second.missingFiles.begin();
                sndItEnd = it->second.missingFiles.end();
                while(sndIt != sndItEnd)
                {
                    if((*sndIt)._sourceFile == filename)
                    {
                        // the sound is missing, so we want to move it to live and potentially load it 
                        it->second.liveFiles.push_back((*sndIt));
                        
                        it->second.missingFiles.erase(sndIt);
                        
                        if(load_now)
                        {
                            PropertyList plist;
                            _loadSound(it->first, it->second.liveFiles.size()-1, CallbackPtr(), plist);
                        }
                        
                        return;
                    }
                    
                    ++sndIt;
                }
                
                ++it;
            }
        }
        else
        {
            SoundGroupMap::iterator it = mSoundGroups.find(group);
            if(it != mSoundGroups.end())
            {
                // found group, now find which set the sound is in
                SoundList::iterator sndIt = it->second.liveFiles.begin();
                SoundList::iterator sndItEnd = it->second.liveFiles.end();
                unsigned int i = 0;
                while(sndIt != sndItEnd)
                {
                    if((*sndIt)._sourceFile == filename)
                    {
                        // the sound is live, so we want to reload it 
                        PropertyList plist;
                        _loadSound(it->first, i, CallbackPtr(), plist);
                        
                        return;
                    }
                    
                    ++i;
                    ++sndIt;
                }
                
                sndIt = it->second.missingFiles.begin();
                sndItEnd = it->second.missingFiles.end();
                while(sndIt != sndItEnd)
                {
                    if((*sndIt)._sourceFile == filename)
                    {
                        // the sound is missing, so we want to move it to live and potentially load it 
                        it->second.liveFiles.push_back((*sndIt));
                        
                        it->second.missingFiles.erase(sndIt);
                        
                        if(load_now)
                        {
                            PropertyList plist;
                            _loadSound(it->first, it->second.liveFiles.size()-1, CallbackPtr(), plist);
                        }
                        
                        return;
                    }
                    
                    ++sndIt;
                }

            }
        }
    }
    
    // --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
    void SoundManager::refreshStreamedTrack(const std::string& filename, const int group)
    {
        unsigned int searchLastSlash = filename.find_last_of('/');
        std::string file;
        if( searchLastSlash != std::string::npos)
        {
            file = filename.substr(searchLastSlash+1);
        }
        else
        { 
            file = filename;
        }
        
        if(group == -1)
        {
            MusicGroupMap::iterator it = mMusicGroups.begin();
            MusicGroupMap::iterator itEnd = mMusicGroups.end();
            
            while(it != itEnd)
            {
                MusicList::iterator sndIt = it->second.missingFiles.begin();
                MusicList::iterator sndItEnd = it->second.missingFiles.end();
                while(sndIt != sndItEnd)
                {
                    // find the last "/"
                    unsigned int lastSlash = (*sndIt).find_last_of('/');
                    unsigned int soundNameStart;
                
                    if(lastSlash == std::string::npos)
                    {
                        // no slash found, search from beginning
                        soundNameStart = (*sndIt).find(file);
                    }
                    else
                    {
                        soundNameStart = (*sndIt).find(file, lastSlash);
                    }
                    
                    if(soundNameStart != std::string::npos)
                    {
                        // the sound is missing, so we want to move it to live and potentially load it 
                        it->second.liveFiles.push_back(filename);
                        
                        it->second.missingFiles.erase(sndIt);
                        
                        return;
                    }
                    
                    ++sndIt;
                }
                
                
                sndIt = it->second.liveFiles.begin();
                sndItEnd = it->second.liveFiles.end();
                while(sndIt != sndItEnd)
                {
                    // find the last "/"
                    unsigned int lastSlash = (*sndIt).find_last_of('/');
                    unsigned int soundNameStart;
                    
                    if(lastSlash == std::string::npos)
                    {
                        // no slash found, search from beginning
                        soundNameStart = (*sndIt).find(file);
                    }
                    else
                    {
                        soundNameStart = (*sndIt).find(file, lastSlash);
                    }
                    
                    if(soundNameStart != std::string::npos)
                    {
                        // the sound is missing, so we want to move it to live and potentially load it 
                        it->second.liveFiles.erase(sndIt);
                        
                        it->second.liveFiles.push_back(filename);
                        
                        return;
                    }
                    
                    ++sndIt;
                }
                
                ++it;
            }
        }
        else
        {
            MusicGroupMap::iterator it = mMusicGroups.find(group);
            if(it != mMusicGroups.end())
            {
                // found group, now find which set the sound is in
                MusicList::iterator sndIt = it->second.missingFiles.begin();
                MusicList::iterator sndItEnd = it->second.missingFiles.end();
                while(sndIt != sndItEnd)
                {
                    // find the last "/"
                    unsigned int lastSlash = (*sndIt).find_last_of('/');
                    unsigned int soundNameStart;
                    
                    if(lastSlash == std::string::npos)
                    {
                        // no slash found, search from beginning
                        soundNameStart = (*sndIt).find(file);
                    }
                    else
                    {
                        soundNameStart = (*sndIt).find(file, lastSlash);
                    }
                    
                    if(soundNameStart != std::string::npos)
                    {
                        // the sound is missing, so we want to move it to live and potentially load it 
                        it->second.liveFiles.push_back(filename);
                        
                        it->second.missingFiles.erase(sndIt);
                        
                        return;
                    }
                    
                    ++sndIt;
                }
                
                
                sndIt = it->second.liveFiles.begin();
                sndItEnd = it->second.liveFiles.end();
                while(sndIt != sndItEnd)
                {
                    // find the last "/"
                    unsigned int lastSlash = (*sndIt).find_last_of('/');
                    unsigned int soundNameStart;
                    
                    if(lastSlash == std::string::npos)
                    {
                        // no slash found, search from beginning
                        soundNameStart = (*sndIt).find(file);
                    }
                    else
                    {
                        soundNameStart = (*sndIt).find(file, lastSlash);
                    }
                    
                    if(soundNameStart != std::string::npos)
                    {
                        // the sound is missing, so we want to move it to live and potentially load it 
                        it->second.liveFiles.push_back(filename);
                        
                        it->second.liveFiles.erase(sndIt);
                        
                        return;
                    }
                    
                    ++sndIt;
                }                
            }
        }
    }
  
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundManager::addSound( int group, std::string filename, bool loop, float base_vol, bool load_now, bool isMusic, bool isVO )
	{
		// load the file first...
		Logger::printf("Walaber", Logger::SV_DEBUG, "SoundManager::addSound( %d, %s )\n", group, filename.c_str());
        
		SoundGroupMap::iterator it = mSoundGroups.find( group );
        if(it == mSoundGroups.end())
		{			
			mSoundGroups[ group ] = SoundGroup();
		}	
        
        // check to see if the file exists
        PropertyList plist;
        plist.setValueForKey("group", Property( group ));
        plist.setValueForKey("loop", Property(loop ? 1 : 0) );
        plist.setValueForKey("base_vol", Property(base_vol) );
        plist.setValueForKey("load_now", Property(load_now ? 1 : 0) );
        plist.setValueForKey("is_music", Property(isMusic ? 1 : 0) );
		plist.setValueForKey("is_vo", Property(isVO ? 1 : 0));
		        
        MemberCallbackPtr<SoundManager>::type memberCallback(new MemberCallback<SoundManager>(this, &SoundManager::_loadSoundFileExists));
        CallbackPtr callback = static_pointer_cast<Callback>(memberCallback);
        FileManager::getInstancePtr()->fileExists(filename, callback, plist);
	}
	
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundManager::_loadSound( int groupID, int assetID, CallbackPtr callback, const PropertyList& plist  )
	{
		SoundGroupMap::iterator it = mSoundGroups.find( groupID );
		
		if (it == mSoundGroups.end())
		{
			Logger::printf("Walaber", Logger::SV_ERROR, "_loadSound ERROR - no such group: %d\n", groupID);
			return;
		}

		if ((assetID < 0) || (assetID >= it->second.liveFiles.size()))
		{
			Logger::printf("Walaber", Logger::SV_ERROR, "_loadSound ERROR - assetID %d not within range [0,%d]\n", assetID, (int)it->second.liveFiles.size());
			return;
		}
		
		// is this file already loading?
		std::pair<int,int> key(groupID, assetID);
		
		bool first = false;
		
		if (mSoundInstanceCallbackMap.find(key) == mSoundInstanceCallbackMap.end())
		{
			// no one is loading this file yet...
			first = true;
		}
		
		
		// add to list
		GotSoundCallbackData cd;
		cd.callback = callback;
		cd.plist = plist;
		
		mSoundInstanceCallbackMap.insert( std::make_pair( key, cd ) );	
		
		if (first)
		{
			PropertyList plist;
			plist.setValueForKey("group", Property( groupID ));
			plist.setValueForKey("id", Property( assetID ));
			plist.setValueForKey("loop", Property( it->second.liveFiles[assetID]._loop ? 1 : 0 ));
			plist.setValueForKey("filename", Property( it->second.liveFiles[assetID]._sourceFile ) );

			
			std::string filename = it->second.liveFiles[assetID]._sourceFile;
			
			
			// do we have a mapping for the current language?
			Walaber::Language cl = TextManager::getCurrentLanguage();
			
			LanguageFolderMap::iterator li = mFolderMap.find( cl );
			if (li != mFolderMap.end())
			{
				// localized folder map exists... swap out the file path  (original_path/loc_folder/original_filename)
				filename = StringHelper::appendPath(StringHelper::getPath(filename),
													StringHelper::appendPath(li->second, 
																			 StringHelper::removePath(filename)));
				
				plist.setValueForKey("is_loc_test", Property(1));
			}

			
			MemberCallbackPtr<SoundManager>::type memberCallback( new MemberCallback<SoundManager>( this, &SoundManager::_loadSoundFileRead ) );
			CallbackPtr callback = static_pointer_cast<Callback>( memberCallback );
			
			FileManager::getInstancePtr()->readFile( filename, callback, plist );
		}
	}
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundManager::_loadSoundFileRead( void* params ) //std::string path, char* buffer, size_t length, std::string actualPath, std::string handlerName, PropertyList userData )
	{
		FileManager::ReadFileCallbackParameters* rfc = (FileManager::ReadFileCallbackParameters*)params;
		
		int group = rfc->userData.getValueForKey("group")->asInt();
		int id = rfc->userData.getValueForKey("id")->asInt();
		bool loop = (rfc->userData.getValueForKey("loop")->asInt() == 1);

		bool isLocTest = false;
		const Property* prop = rfc->userData.getValueForKey("is_loc_test");
		if (prop)
			isLocTest = (prop->asInt() == 1);
		
		
		
		if (isLocTest && (!rfc->buffer))
		{
			// retry with the non-localized file...
			std::string filename = StringHelper::removePath( rfc->path );
			std::string path = StringHelper::removeLastPathComponent(StringHelper::getPath( rfc->path ));
			
			std::string newPath = StringHelper::appendPath(path, filename);
			
			PropertyList plist = rfc->userData;
			plist.removeValueForKey("is_loc_test");
			
			MemberCallbackPtr<SoundManager>::type memberCallback(new MemberCallback<SoundManager>(this, &SoundManager::_loadSoundFileRead));
			CallbackPtr callback = static_pointer_cast<Callback>(memberCallback);
			FileManager::getInstancePtr()->readFile(newPath, callback, plist);
			return;
		}
		
		
		SoundGroupMap::iterator it = mSoundGroups.find( group );
		
		if (it == mSoundGroups.end())
		{
			Logger::printf("Walaber", Logger::SV_ERROR, "_addSoundFileRead ERROR - no such group: %d\n", group);
			return;
		}
		
		if ((id < 0) || (id >= it->second.liveFiles.size()))
		{
			Logger::printf("Walaber", Logger::SV_ERROR, "_addSoundFileRead ERROR - assetID %d not within range [0,%d]\n", id, (int)it->second.liveFiles.size());
			return;
		}
		
		SoundResourcePtr sr = _loadSoundResource( rfc, loop);
		
		if (!sr)
		{
			Walaber::Logger::printf("Walaber", Logger::SV_ERROR, " --> sound was for group[%d] asset[%d]\n", group, id);
			sr = SoundResourcePtr();
		}
		
		// store this!
		it->second.liveFiles[id]._sound = sr;
		
		// look for any callbacks associated with this group and asset IDs
		std::pair<SoundInstanceCallbackMap::iterator, SoundInstanceCallbackMap::iterator> its;

		its = mSoundInstanceCallbackMap.equal_range( std::make_pair(group,id) );
		for (SoundInstanceCallbackMap::iterator it_ca = its.first; it_ca != its.second; it_ca++)
		{
			// call the callback!
			if (it_ca->second.callback)
			{
				if (it->second.liveFiles[id]._isMusic)
				{
                    SoundEffectInstanceLoadedParameters params(group, id, SoundEffectInstancePtr(new SoundEffectInstance( it->second.liveFiles[id]._sound, mGroupMusic, it->second.liveFiles[id]._baseVol, it->second.liveFiles[id]._subtitleText )), it_ca->second.plist);					
					it_ca->second.callback->invoke(&params);
				}
				else
				{
					bool isVO = it->second.liveFiles[id]._isVO;
					
                    SoundEffectInstanceLoadedParameters params(group, id, SoundEffectInstancePtr(new SoundEffectInstance( it->second.liveFiles[id]._sound, (isVO ? mGroupVO : mGroupSFX), it->second.liveFiles[id]._baseVol, it->second.liveFiles[id]._subtitleText )), it_ca->second.plist );
					it_ca->second.callback->invoke(&params);
				}
			}
		}	
		
		// clear callbacks...
		mSoundInstanceCallbackMap.erase( its.first, its.second );
	}
	
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
    SoundResourcePtr SoundManager::_loadSoundResource( FileManager::ReadFileCallbackParameters* rfc, bool loop )
	{
		FMOD::Sound* sound = NULL;
		
		FMOD_RESULT result = FMOD_OK;
		
		FMOD_CREATESOUNDEXINFO exinfo;
		
		memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
		exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
		exinfo.length = rfc->length;
		
		result = mSystem->createSound(rfc->buffer, FMOD_SOFTWARE | FMOD_OPENMEMORY | FMOD_LOWMEM | FMOD_CREATECOMPRESSEDSAMPLE, &exinfo, &sound);
		
		if (rfc->buffer)
			delete[] rfc->buffer;
		
		
		if (result != FMOD_OK)
		{
			Logger::printf("Walaber", Logger::SV_ERROR, "ERROR loading sound {%s} -- [%s]\n", 
						   rfc->path.c_str(), FMOD_ErrorString(result));
		}
		
		if (loop)
			sound->setMode( FMOD_LOOP_NORMAL );
		else
			sound->setMode( FMOD_LOOP_OFF );
		
		
#ifdef _DEBUG
		FMOD_SOUND_TYPE type;
		FMOD_SOUND_FORMAT fmt;
		int channels, bits;
		
		sound->getFormat( &type, &fmt, &channels, &bits );
		
		unsigned int memused = 0;
		sound->getMemoryInfo( FMOD_MEMBITS_ALL, 0, &memused, NULL );
		Logger::printf("Walaber", Logger::SV_DEBUG, "got sound (%s) : mem usage %d  ", rfc->path.c_str(), memused );
		
		switch (fmt)
		{
			case FMOD_SOUND_FORMAT_PCM16:
				Logger::printf("Walaber", Logger::SV_DEBUG, "PCM16 ");
				break;
				
			case FMOD_SOUND_FORMAT_PCM24:
				Logger::printf("Walaber", Logger::SV_DEBUG, "PCM24 ");
				break;
				
			case FMOD_SOUND_FORMAT_PCM8:
				Logger::printf("Walaber", Logger::SV_DEBUG, "PCM8 ");
				break;
				
			case FMOD_SOUND_FORMAT_MPEG:
				Logger::printf("Walaber", Logger::SV_DEBUG, "MPEG ");
				break;
				
			case FMOD_SOUND_FORMAT_PCMFLOAT:
				Logger::printf("Walaber", Logger::SV_DEBUG, "PCMFLOAT ");
				break;
				
			default:
				Logger::printf("Walaber", Logger::SV_DEBUG, "FORMAT??? ");
				break;
		}
		
		Logger::printf("Walaber", Logger::SV_DEBUG, " %dCHAN %dBIT ", channels, bits);
		
		
		float freq, vol, pan;
		int priority;
		
		sound->getDefaults( &freq, &vol, &pan, &priority);
		Logger::printf("Walaber", Logger::SV_DEBUG, " %4.2fHz, %4.2fVOL %4.2fPAN %dPRIORITY\n", freq, vol, pan, priority);
#endif
		
		
		return SoundResourcePtr( new SoundResource( sound ) );
	}
	
    // --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
    void SoundManager::_loadSoundFileExists( void* params )
    {
        FileManager::FileExistsCallbackParameters* fcp = (FileManager::FileExistsCallbackParameters*)params;
		
        const Property* prop = fcp->userData.getValueForKey("is_stream");
        unsigned int group = (unsigned int)fcp->userData.getValueForKey("group")->asInt();
        
        if(!prop)
        {
            bool loops = (fcp->userData.getValueForKey("loop")->asInt() == 1);
            float base_vol = fcp->userData.getValueForKey("base_vol")->asFloat();
            bool load_now = (fcp->userData.getValueForKey("load_now")->asInt() == 1);
            bool is_music = (fcp->userData.getValueForKey("is_music")->asInt() == 1);
            bool is_vo = (fcp->userData.getValueForKey("is_vo")->asInt() == 1);
			
            SoundInfo si;
            si._sound = SoundResourcePtr();
            si._baseVol = base_vol;
            si._sourceFile = fcp->path;
            si._loop = loops;
            si._isMusic = is_music;
            si._isVO = is_vo;
			
            if(fcp->exists)
            {
                // file exists, go through the normal steps
                SoundGroup& sndGroup = mSoundGroups[ group ];
                sndGroup.liveFiles.push_back(si);
                
                if (load_now)
                {
                    PropertyList plist;
                    _loadSound( group, sndGroup.liveFiles.size()-1, CallbackPtr(), plist );
                }
            }
            else
            {
                // file does not exist, load data but put it somewhere else
                SoundGroup& sndGroup = mSoundGroups[ group ];
                sndGroup.missingFiles.push_back(si);
            }
        }
        else
        {
            if(fcp->exists)
            {
                // file exists, go through the normal steps
                MusicGroup& musicGroup = mMusicGroups[ group ];
                musicGroup.liveFiles.push_back(fcp->actualPath);
            }
            else
            {
                // file exists, go through the normal steps
                // this will put the search path into the missing list, 
                // since there is no actual path
                MusicGroup& musicGroup = mMusicGroups[ group ];
                musicGroup.missingFiles.push_back(fcp->actualPath);
            }
        }
    }
    
    // --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
    // modifyMusicFilePath()
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
    // Used by the Where's My Perry Demo, whose content is stored in a zip archive
    // We can't stream mp3s from within the zip file, so we extract them, and then
    // we call "modifyMusicFilePath()" on each of the mp3 files that was moved from 
    // "/Perry/Audio/" to "<device's library>/Music"
    void SoundManager::modifyMusicFilePath( const std::string& oldPath, const std::string& newPath )
    {
        
        bool fileWasFound = false;
        
        // Iterate on all the groups
        for ( int groupIndex = 0; groupIndex < mMusicGroups.size(); groupIndex ++ ) 
        {
            
            MusicGroup& musicGroup = mMusicGroups[ groupIndex ];
            
            // Iterate on the liveFiles in this group, looking for the one whose path matches "oldPath"
            for ( int fileIndex = 0; fileIndex < musicGroup.liveFiles.size(); fileIndex ++ ) 
            {
                
                if ( musicGroup.liveFiles[ fileIndex ].compare( oldPath ) == 0 ) 
                {
                    // Found the file...set its path to "newPath"
                    musicGroup.liveFiles[ fileIndex ] = newPath;
                    
                    fileWasFound = true;
                    
                    break;
                }
                
            }
            
            if ( fileWasFound == true ) {
                break;
            }
            
        }
        
    }
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundManager::preloadSoundGroup( int group )
	{
		SoundGroupMap::iterator it = mSoundGroups.find( group );
		
		if (it == mSoundGroups.end())
			return;

		for (unsigned int i = 0; i < it->second.liveFiles.size(); i++)
		{
			SoundInfo& s = it->second.liveFiles[i];
			
			if (!s._sound.use_count())
			{
				// load this one!
				PropertyList plist;
				
				_loadSound( group, i, CallbackPtr(), plist );
			}
		}
	}
	

	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundManager::playSoundFromGroup( int group, float volume, float pitch )
	{
		PropertyList plist;
		plist.setValueForKey("vol", Property(volume));
		plist.setValueForKey("pitch", Property(pitch));
		
        CallbackPtr callback(new Callback(&SoundManager::_oneShotCallback));
		newSoundFromGroup(group, callback, plist);
	}
	

	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundManager::newSoundFromGroup( int group, CallbackPtr callback, const PropertyList& plist )
	{
		// get a random sound from the group.
		SoundGroupMap::iterator it = mSoundGroups.find( group );
		
		if (it == mSoundGroups.end() || !it->second.liveFiles.size())
			return;
		
		int assetID = rand() % it->second.liveFiles.size();
		
		newSoundSpecific(group, assetID, callback, plist);
	}
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	unsigned int SoundManager::soundCountForGroup( int group )
	{
		SoundGroupMap::iterator it = mSoundGroups.find( group );
		
		if (it == mSoundGroups.end())
			return 0;
		else
			return it->second.liveFiles.size();		
	}

	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundManager::playSoundSpecific( int groupID, int assetID, float volume )
	{
		PropertyList plist;
		plist.setValueForKey("vol", Property(volume));
		
        CallbackPtr callback(new Callback(&SoundManager::_oneShotCallback));
		newSoundSpecific(groupID, assetID, callback, plist);
	}
    
    
	void SoundManager::playSoundSpecific( int groupID, int assetID, float volume, float pitch ) 
    {
		PropertyList plist;
		plist.setValueForKey("vol", Property(volume));
		plist.setValueForKey("pitch", Property(pitch));        
		
        CallbackPtr callback(new Callback(&SoundManager::_oneShotCallback));
		newSoundSpecific(groupID, assetID, callback, plist);    	
    }

	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundManager::newSoundSpecific( int groupID, int assetID, CallbackPtr callback, const PropertyList& plist )
	{
		// get a specific sound from the group.
		SoundGroupMap::iterator it = mSoundGroups.find( groupID );
		
		if (it == mSoundGroups.end())
			return;
		
		if ((assetID < 0) || (assetID >= it->second.liveFiles.size()))
			return;
		
		bool is_vo = it->second.liveFiles[assetID]._isVO;
		
		if ((is_vo) && (!mVoicesOn))
			return;
		
		if ((!is_vo) && (!mSoundsOn))
			return;
		
		// is this sound already loaded?
		if (it->second.liveFiles[assetID]._sound.use_count())
		{
			// just call the callback with a new SoundEffectInstance!
			if (callback)
			{
				if (it->second.liveFiles[assetID]._isMusic)
				{
                    SoundEffectInstanceLoadedParameters params(groupID, 
                                                               assetID, 
                                                               SoundEffectInstancePtr(new SoundEffectInstance(it->second.liveFiles[ assetID ]._sound, mGroupMusic, it->second.liveFiles[ assetID ]._baseVol, it->second.liveFiles[ assetID ]._subtitleText)), 
                                                               plist );
					callback->invoke(&params);
				}
				else
				{
                    SoundEffectInstanceLoadedParameters params(groupID, 
                                                               assetID, 
                                                               SoundEffectInstancePtr(new SoundEffectInstance(it->second.liveFiles[ assetID ]._sound, (is_vo ? mGroupVO : mGroupSFX), it->second.liveFiles[ assetID ]._baseVol, it->second.liveFiles[ assetID ]._subtitleText)), 
                                                               plist);
					callback->invoke( &params );
				}
			}
		}
		else
		{
			// sound needs loading!
			_loadSound( groupID, assetID, callback, plist );
		}
	}
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundManager::playSoundFile( const std::string& file, float volume, float pitch, bool isVO )
	{
		PropertyList plist;
		plist.setValueForKey("vol", Property(volume));
		plist.setValueForKey("pitch", Property(pitch));
		
		// use one-shot callback...
		CallbackPtr callback(new Callback(&SoundManager::_oneShotCallbackDirectFile));
		
		getSoundFile(file, callback, plist, isVO);
	}
	
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	void SoundManager::getSoundFile( const std::string& file, CallbackPtr callback, const PropertyList& plist, bool isVO  )
	{
		std::string filename = file;
		
		PropertyList loadPlist;
		loadPlist.setValueForKey("is_vo", Property(isVO ? 1 : 0));
		loadPlist.setValueForKey("filename", Property(file));
		
        // try loading this file...
		
		// do we have a mapping for the current language?
		Walaber::Language cl = TextManager::getCurrentLanguage();
		
		LanguageFolderMap::iterator li = mFolderMap.find( cl );
		if (li != mFolderMap.end())
		{
			// localized folder map exists... swap out the file path  (original_path/loc_folder/original_filename)
			filename = StringHelper::appendPath(StringHelper::getPath(filename),
												StringHelper::appendPath(li->second, 
																		 StringHelper::removePath(filename)));
			
			loadPlist.setValueForKey("is_loc_test", Property(1));
		}
		
		bool isfirst = true;
		
		if (mSoundFileCallbackMap.find( file ) != mSoundFileCallbackMap.end())
		{
			isfirst = false;
		}
		
		// insert callback data into map.
		GotSoundCallbackData data;
		data.plist = plist;
		data.callback = callback;
		mSoundFileCallbackMap.insert( std::make_pair(file, data) );
		
		// if this is the first request for the file, load it!
		if (isfirst)
		{
			MemberCallbackPtr<SoundManager>::type memberCallback( new MemberCallback<SoundManager>( this, &SoundManager::_loadSoundFileDirectRead ) );
			CallbackPtr loadCallback = static_pointer_cast<Callback>( memberCallback );
			
			FileManager::getInstancePtr()->readFile( filename, loadCallback, loadPlist );
		}
	}
	
	
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundManager::_loadSoundFileDirectRead(void *params)
	{
		FileManager::ReadFileCallbackParameters* rfc = (FileManager::ReadFileCallbackParameters*)params;
		
		bool isVO = (rfc->userData["is_vo"].asInt() == 1);
		std::string origFile = rfc->userData["filename"].asString();
		
		bool isLocTest = false;
		const Property* prop = rfc->userData.getValueForKey("is_loc_test");
		if (prop)
			isLocTest = (prop->asInt() == 1);
		
		
		ResultCode result = ResultCode_OK;
		
		
		SoundInfo si;
		si._sourceFile = rfc->path;
		si._baseVol = 1.0f;
		si._isMusic = false;
		si._isVO = isVO;
		si._loop = false;
		
		
		
		if (rfc->result != ResultCode_OK)
		{
			if (isLocTest)
			{
				// retry with the non-localized file...
				std::string filename = StringHelper::removePath( rfc->path );
				std::string path = StringHelper::removeLastPathComponent(StringHelper::getPath( rfc->path ));
				
				std::string newPath = StringHelper::appendPath(path, filename);
				
				PropertyList plist = rfc->userData;
				plist.removeValueForKey("is_loc_test");
				
				MemberCallbackPtr<SoundManager>::type memberCallback(new MemberCallback<SoundManager>(this, &SoundManager::_loadSoundFileDirectRead));
				CallbackPtr callback = static_pointer_cast<Callback>(memberCallback);
				FileManager::getInstancePtr()->readFile(newPath, callback, plist);
				return;
			}
			else
			{
				// complete fail trying to load this sound... just call the callback(s) and signify an error...
				result = ResultCode_ErrorFileNotFound;
			}
		}
		
		// load up this sound...
		if (result == ResultCode_OK)
		{
			SoundResourcePtr sr = _loadSoundResource( rfc, false);
			
			if (!sr)
			{
				result = ResultCode_Error;
			}
			
			// store this in our direct-load list...
			si._sound = sr;
		
			
			if (si._isVO)
			{
				// this is a voice... see if TextManager has a key for it.
				std::string key = StringHelper::removeExtension(StringHelper::removePath(si._sourceFile));
				
				if (TextManager::subtitleExists(key))
					si._subtitleText = key;
			}
			else 
			{
				si._subtitleText = "";
			}
			
			mDirectLoadedSounds.push_back(si);
				
		}
		
		
		// look for any callbacks associated with this group and asset IDs
		std::pair<SoundFileCallbackMap::iterator, SoundFileCallbackMap::iterator> its;
		
		its = mSoundFileCallbackMap.equal_range( origFile );
		for (SoundFileCallbackMap::iterator it_ca = its.first; it_ca != its.second; it_ca++)
		{
			// call the callback!
			if (it_ca->second.callback)
			{
				SoundFileInstanceLoadedParameters params(origFile, SoundEffectInstancePtr(new SoundEffectInstance( si._sound, (isVO ? mGroupVO : mGroupSFX), si._baseVol, si._subtitleText )), it_ca->second.plist );
				params.result = result;
				
				it_ca->second.callback->invoke(&params);
			}
		}	
		
		// clear callbacks...
		mSoundFileCallbackMap.erase( its.first, its.second );
	}
	
	/*
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	RecordableSoundEffectInstance* SoundManager::createRecordableSound( int lenghInSeconds, int defaultFrequency )
	{
		FMOD_CREATESOUNDEXINFO  exinfo = {0};
		
		memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
		
		exinfo.cbsize           = sizeof(FMOD_CREATESOUNDEXINFO);
		exinfo.numchannels      = 1;
		exinfo.format           = FMOD_SOUND_FORMAT_PCM16;
		exinfo.defaultfrequency = defaultFrequency;
		exinfo.length           = exinfo.defaultfrequency * sizeof(short) * exinfo.numchannels * lenghInSeconds;
		
		FMOD::Sound* sound = NULL;
		
		
		FMOD_RESULT result = mSystem->createSound(NULL, FMOD_2D | FMOD_SOFTWARE | FMOD_OPENUSER, &exinfo, &sound);
		if (result != FMOD_OK)
		{
			printf("ERROR creating recordable sound! [%s]\n", FMOD_ErrorString(result));
			return NULL;
		}
		
		return new RecordableSoundEffectInstance( FMODSoundPtrPtr(sound), mGroupSFX, lenghInSeconds );
		
	}
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	RecordableSoundEffectInstance* SoundManager::createRecordableSoundWithWAV( int lenghInSeconds, std::string& filename, int defaultFrequency )
	{
		// first, load the WAV file we will get source data from...
		FMOD::Sound* sound = NULL;
		FMOD_RESULT result = FMOD_OK;
		
		result = mSystem->createSound(filename.c_str(), FMOD_SOFTWARE, NULL, &sound);
		if (result != FMOD_OK)
		{
			printf("ERROR loading sound {%s} -- [%s]\n", filename.c_str(), FMOD_ErrorString(result));
			return NULL;
		}
		
		unsigned int srcLength = 0;
		result = sound->getLength(&srcLength, FMOD_TIMEUNIT_PCM);
		
		
		// now create
		FMOD_CREATESOUNDEXINFO  exinfo = {0};
		
		memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
		
		exinfo.cbsize           = sizeof(FMOD_CREATESOUNDEXINFO);
		exinfo.numchannels      = 1;
		exinfo.format           = FMOD_SOUND_FORMAT_PCM16;
		exinfo.defaultfrequency = defaultFrequency;
		exinfo.length           = exinfo.defaultfrequency * sizeof(short) * exinfo.numchannels * lenghInSeconds;
		
		FMOD::Sound* recSound = NULL;
		
		
		result = mSystem->createSound(NULL, FMOD_2D | FMOD_SOFTWARE | FMOD_OPENUSER, &exinfo, &recSound);
		if (result != FMOD_OK)
		{
			printf("ERROR creating recordable sound! [%s]\n", FMOD_ErrorString(result));
			return NULL;
		}
		
		// now fill the record sound with data from the WAV file...
		int copyLength = (exinfo.length < srcLength) ? exinfo.length : srcLength;
		
		void* ptr1;
		void* ptr2;
		unsigned int len1;
		unsigned int len2;
		sound->lock(0, copyLength, &ptr1, &ptr2, &len1, &len2);
		
		void* dst1;
		void* dst2;
		unsigned int destL1;
		unsigned int destL2;
		recSound->lock(0, copyLength, &dst1, &dst2, &destL1, &destL2);
		
		memcpy(dst1, ptr1, len1);
		memcpy(dst2, ptr2, len2);
		
		RecordableSoundEffectInstance* finalSound = new RecordableSoundEffectInstance( FMODSoundPtrPtr(recSound), mGroupSFX, lenghInSeconds );
		
		sound->release();
		
		return finalSound;
	}
	
	
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	bool SoundManager::canRecord()
	{
		int recCount = 0;
		
		FMOD_RESULT res = mSystem->getRecordNumDrivers( &recCount );
		if (res != FMOD_OK)
		{
			printf("ERROR getting number of record drivers! [%s]\n", FMOD_ErrorString(res));
			return false;
		}
		
		return (recCount > 0);
	}
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundManager::startRecording( RecordableSoundEffectInstance* sound, bool loop )
	{
		FMOD_RESULT result = mSystem->recordStart( 0, (*sound->mSound), loop );
		if (result != FMOD_OK)
		{
			printf("ERROR could not start recording! [%s]\n", FMOD_ErrorString(result));
		}
	}
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundManager::stopRecording( RecordableSoundEffectInstance* sound )
	{
		FMOD_RESULT result = mSystem->recordStop( 0 );
		if (result != FMOD_OK)
		{
			printf("ERROR could not stop recording! [%s]\n", FMOD_ErrorString(result));
		}
	}
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	bool SoundManager::isRecording( RecordableSoundEffectInstance* sound )
	{
		bool rec = false;
		FMOD_RESULT result = mSystem->isRecording( 0, &rec );
		if (result != FMOD_OK)
		{
			printf("ERROR could not retrieve recording state! [%s]\n", FMOD_ErrorString(result));
		}
		
		return rec;
	}
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	unsigned int SoundManager::getRecordPosition()
	{
		unsigned int pos = 0;
		FMOD_RESULT res = mSystem->getRecordPosition( 0, &pos );
		if (res != FMOD_OK)
		{
			printf("ERROR getting record position [%s]\n", FMOD_ErrorString(res));
			return 0;
		}
		
		return pos;
	}
	*/
	
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundManager::optimize(bool forceAll)
	{
		// look for any groups that are not in use, and flush them from memory!
		for (SoundGroupMap::iterator it = mSoundGroups.begin(); it != mSoundGroups.end(); it++)
			freeGroup( it->first, forceAll );
		
		// free up directly loaded sounds...
		for (SoundList::iterator it = mDirectLoadedSounds.begin(); it != mDirectLoadedSounds.end(); ++it)
		{
			if ((*it)._sound.unique())
				(*it)._sound.reset();
		}
	}
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundManager::freeGroup( int groupID, bool force )
	{
		SoundGroupMap::iterator it = mSoundGroups.find(groupID);
		
		if (it == mSoundGroups.end())
		{
			Logger::printf("Walaber", Logger::SV_DEBUG, "SoundManager::freeGroup( %d ) ERROR -> group does not exist.\n", groupID );

			return;
		}
	
		
		if (!force)
		{
			if (mInvulnerableSoundGroups.find( it->first ) != mInvulnerableSoundGroups.end())
			{
				// this group is invulnerable!
				Logger::printf("Walaber", Logger::SV_DEBUG, "SoundManager::freeGroup() -- sound group %d is set to always stay in memory, skipping it!\n", it->first);

				return;
			}
		}
		
		
		bool allClear = true;
		
		
		if (!force)
		{
			for (SoundList::iterator s = it->second.liveFiles.begin(); s != it->second.liveFiles.end(); s++)
			{
				if ((*s)._sound.use_count())
				{
					if (!(*s)._sound.unique())
					{
						allClear = false;
						break;
					}
				}
			}
		}
		
		
		if (allClear)
		{
			// remove sounds from memory.
			Logger::printf("Walaber", Logger::SV_DEBUG, "SoundManager::freeGroup() ==> removing group [%d] from memory...\n", it->first);

			for (unsigned int i = 0; i < it->second.liveFiles.size(); i++)
			{
				Logger::printf("Walaber", Logger::SV_DEBUG, "  -> sound index [%d] removed\n", i);

				it->second.liveFiles[i]._sound.reset();
			}
		}
	}
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundManager::removeGroup( int groupID )
	{
		SoundGroupMap::iterator it = mSoundGroups.find( groupID );
		if (it != mSoundGroups.end())
		{
			it->second.liveFiles.clear();
            it->second.missingFiles.clear();
			mSoundGroups.erase( it );
		}
		
		mInvulnerableSoundGroups.erase( groupID );
	}

	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundManager::unloadAllSounds()
	{
		mSoundGroups.clear();	
		mInvulnerableSoundGroups.clear();
        mDirectLoadedSounds.clear();
	}
	
	
		
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundManager::update( float elapsedSec )
	{		
		mSystem->update();
		
#ifdef _DEBUG
		/*int channelsPlaying = 0;
		mSystem->getChannelsPlaying(&channelsPlaying);
		printf("channels: [%d]\n", channelsPlaying);
		
		for (int i = 0; i < 64; i++)
		{
			FMOD::Channel* channel;
			mSystem->getChannel( i, &channel );
			
			FMOD::Sound* sound;
			channel->getCurrentSound( &sound );
			
			char namebuffer[64];
			sound->getName(namebuffer, 64);
			
			bool playing;
			channel->isPlaying(&playing);
			if (playing)
				printf("[%d] [playing:%d] -- [%s]\n", i, playing, namebuffer);
		}*/
#endif
        
	}
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundManager::setAllPaused(bool paused)
	{
		setSoundsPaused(paused);
		setVoicesPaused(paused);
		setMusicPaused(paused);
	}
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundManager::setSoundsPaused(bool paused)
	{
		if (mGroupSFX)
			mGroupSFX->setPaused(paused);
	}
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundManager::setVoicesPaused(bool paused)
	{
		if (mGroupVO)
			mGroupVO->setPaused(paused);
	}
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundManager::setMusicPaused(bool paused)
	{
		if (mGroupMusic)
			mGroupMusic->setPaused(paused);
	}
    
    // --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundManager::stopVoices()
	{
		if (mGroupVO)
			mGroupVO->stop();
	}
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	bool SoundManager::systemMusicIsPlaying()
	{
		bool ret = false;

#ifdef TARGET_IPHONE
		FMOD_RESULT res = FMOD_IPhone_OtherAudioIsPlaying(&ret);
		if (res != FMOD_OK)
		{
			Logger::printf("Walaber", Logger::SV_ERROR, "ERROR checking for system music: [%s]\n", FMOD_ErrorString(res));
		}
#endif
		
		// Simulator roundabout
		if (PlatformManager::getInstancePtr()->getPlatformType() == PlatformManager::PT_Simulator)
		{
			ret = false;
		}
		
		return ret;
	}
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundManager::updateMusicMuteStatus()
	{
		if (mGroupMusic)
		{
			mGroupMusic->setMute( systemMusicIsPlaying() );
		}
	}
	
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
//	void SoundManager::playMusic( int trackID )
//	{
//		if (systemMusicIsPlaying())
//			return;
//		
//		if (mMusicChannel != NULL)
//			stopMusic();
//		
//		if (mMusicList.empty())
//			return;
//		
//		int finalID = trackID;
//		
//		if (finalID == -1)
//			finalID = rand() % mMusicList.size();
//			
//		std::string fullFile = mMusicBasePath + mMusicList[finalID];
//
//#ifdef _DEBUG
//		printf("music track: %s\n", fullFile.c_str());
//#endif
//		
//		FMOD_RESULT result = mSystem->createStream(fullFile.c_str(), FMOD_SOFTWARE | FMOD_LOOP_NORMAL, NULL, &mMusicSound);
//		if (result != FMOD_OK)
//		{
//			printf("ERROR loading streamed audio file {%s} - [%s]\n", fullFile.c_str(), FMOD_ErrorString(result));
//			mMusicSound = NULL;
//			return;
//		}
//		
//		mCurrentMusicTrack = trackID;
//		result = mSystem->playSound(FMOD_CHANNEL_FREE, mMusicSound, false, &mMusicChannel);
//		mMusicChannel->setChannelGroup( mGroupMusic );
//	}
    
    // --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
    bool SoundManager::playMusicFromGroup(int groupID, bool restartIfPlaying)
    {
        
        bool ret = _checkMusicPlayability();
            
        bool playMusic = false;
        
        if( !isMusicPlaying() ||
            (isMusicPlaying() && ((mCurrentMusicGroup == groupID && restartIfPlaying) || mCurrentMusicGroup != groupID)))			   
        {
            playMusic = true;
        }
        
        if(playMusic)
        {
            MusicGroupMap::iterator it = mMusicGroups.find(groupID);
            
            if(it != mMusicGroups.end() && it->second.liveFiles.size())
            {
                it->second.currentTrackID = rand() % it->second.liveFiles.size();
                
                ret = _streamMusicAtPath(it->second.liveFiles[it->second.currentTrackID].c_str(), groupID, it->second.currentTrackID );
                setMusicPaused(!ret);
            }
            else
            {
                ret = false;
            }

        }
                
        return ret;
    }
        
    // --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
    bool SoundManager::playHubMusicFromGroup(int groupID, float fadeLength)
    {
        bool ret = _checkMusicPlayability();
        
        if (isMusicPlaying())
            stopMusic();
        
        bool playMusic = false;
        
        if (mCurrentMusicGroup != groupID || mCurrentMusicGroup == 0)
        {
            playMusic = true;
        }
        
        if (playMusic)
        {
            for (int i=0; i< mHubMusicChannels.size(); i++)
            {
                bool musicPlaying = false;
                bool musicPaused = false;
                mHubMusicChannels[i]->isPlaying(&musicPlaying);
                mHubMusicChannels[i]->getPaused(&musicPaused);
                
                if (musicPlaying && !musicPaused)
                {
                    mHubMusicChannels[i]->setPaused(true);
                    break;
                }
            }
            
            for (int j=0; j< mHubMusicGroups.size(); j++)
            {
                if (groupID == mHubMusicGroups[j])
                {
                    mHubMusicChannels[j]->setPaused(false);
                    mCurrentMusicGroup = groupID;
                    mCurrentIndex = j;
                    return true;
                }
            }
            
            // play music
            MusicGroupMap::iterator it = mMusicGroups.find(groupID);
            
            if (it != mMusicGroups.end() && it->second.liveFiles.size())
            {
                it->second.currentTrackID = rand() % it->second.liveFiles.size();
                
                FMOD::Sound* mHubMusicSound;
                
                mHubMusicSound = NULL;
                
#ifdef TARGET_NDK
                PropertyList plist;
                
                MemberCallbackPtr<SoundManager>::type memberCallback( new MemberCallback<SoundManager>( this, &SoundManager::_resumeStreamingMusicAtPath ) );
                CallbackPtr callback = static_pointer_cast<Callback>(memberCallback);
                
                FileManager::getInstancePtr()->readFile( it->second.liveFiles[it->second.currentTrackID].c_str(), callback, plist );
                
                mCurrentMusicGroup = groupID;
                mMusicGroups[groupID].currentTrackID = it->second.currentTrackID;
                
                // NDK - we can't get correct return value here with asynchronous reading...
                return true;
                
#else

                
                FMOD_RESULT result = mSystem->createStream(it->second.liveFiles[it->second.currentTrackID].c_str(), FMOD_SOFTWARE | FMOD_LOOP_NORMAL, NULL, &mHubMusicSound);
                
                //                FMOD_RESULT result = mSystem->createStream(path.c_str(), FMOD_SOFTWARE | FMOD_LOOP_NORMAL, NULL, &mMusicSound);
                
                if (result != FMOD_OK)
                {
                    Logger::printf("Walaber", Logger::SV_ERROR, "ERROR loading streamed audio file {%s} - [%s]\n", it->second.liveFiles[it->second.currentTrackID].c_str(), FMOD_ErrorString(result));
                    mHubMusicSound = NULL;
                    ret = false;
                }
                else
                {
                    FMOD::Channel* mHubMusicChannel;
                    mHubMusicChannel = NULL;
                    mCurrentMusicGroup = groupID;
                    mMusicGroups[groupID].currentTrackID = it->second.currentTrackID;
                    result = mSystem->playSound(FMOD_CHANNEL_FREE, mHubMusicSound, false, &mHubMusicChannel);
                    //result = mSystem->playSound(FMOD_CHANNEL_FREE, mMusicSound, false, &mMusicSound);
                    mHubMusicChannel->setChannelGroup( mGroupMusic );
                    
                    mHubMusicChannels.push_back( mHubMusicChannel );
                    mHubMusicSounds.push_back( mHubMusicSound );
                    mHubMusicGroups.push_back( groupID );
                    
                    mCurrentIndex = mHubMusicGroups.size() - 1;
                    
                    ret = true;
                }
#endif
            }
            else
            {
                ret = false;
            }
            
        }
        
        return ret;

    }
    
    // --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=

    void SoundManager::clearHubMusicChannels()
    {
        for (int i=0; i<mHubMusicChannels.size(); i++)
        {
            if (mHubMusicChannels[i] != NULL)
            {
                mHubMusicChannels[i]->stop();
            }
            
            if (mHubMusicSounds[i] != NULL)
            {
                mHubMusicSounds[i]->release();
                mHubMusicSounds[i] = NULL;
            }
            
            mHubMusicChannels[i] = NULL;
        }
        
        mHubMusicGroups.clear();
        mHubMusicChannels.clear();
        mHubMusicSounds.clear();
        mCurrentMusicGroup = -1;
    }
    
    // --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
    bool SoundManager::playNextTrackInGroup(int groupID)
    {
        bool ret = _checkMusicPlayability();
        if(ret)
        {             
            MusicGroupMap::iterator it = mMusicGroups.find(groupID);
            
            if(it != mMusicGroups.end() && it->second.liveFiles.size())
            {                
                it->second.currentTrackID = (it->second.currentTrackID+1) % it->second.liveFiles.size();
                ret = _streamMusicAtPath(it->second.liveFiles[it->second.currentTrackID].c_str(), groupID, it->second.currentTrackID );
            }
            else
            {
                ret = false;
            }
        }
        
        return ret;
    }
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
    bool SoundManager::playCurrentTrackInGroup(int groupID)
    {
        bool ret = _checkMusicPlayability();
        if(ret)
        {             
            MusicGroupMap::iterator it = mMusicGroups.find(groupID);
            
            if(it != mMusicGroups.end() && it->second.liveFiles.size())
            {                
                ret = _streamMusicAtPath(it->second.liveFiles[it->second.currentTrackID].c_str(), groupID, it->second.currentTrackID );
            }
            else
            {
                ret = false;
            }
        }
        
        return ret;
    }
    
    // --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	bool SoundManager::playSpecificMusicFromGroup( int groupID, int trackID )
	{
        bool ret = _checkMusicPlayability();
        if(ret)
        {       
            MusicGroupMap::iterator it = mMusicGroups.find(groupID);
            if (it != mMusicGroups.end() )
            {            
                if(trackID >= it->second.liveFiles.size())
                {
                    ret = false;
					Logger::printf("Walaber", Logger::SV_INFO, "not in live music group: %i  track id: %i\n", groupID, trackID );
                }
                else
                {
                    Logger::printf("Walaber", Logger::SV_INFO, "music track: %s\n", it->second.liveFiles[trackID].c_str());
                    
                    ret = _streamMusicAtPath(it->second.liveFiles[trackID], groupID, trackID);
                }
            }
            else
            {
                ret = false;
				Logger::printf("Walaber", Logger::SV_INFO, "Muisc group does not exist group: %i\n", groupID );
            }
        }
        
        return ret;
	}
    
    // --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	bool SoundManager::_checkMusicPlayability()
    {
        if (systemMusicIsPlaying())
			return false;		
		
		if (mMusicGroups.empty())
			return false;
        
        return true;
    }
    
    // --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	bool SoundManager::_streamMusicAtPath(const std::string& path, const int groupID, const int trackID)
    {
        bool ret = true;
        
        if (mMusicChannel != NULL)
            stopMusic();
        
#ifdef TARGET_NDK
        PropertyList plist;

        MemberCallbackPtr<SoundManager>::type memberCallback( new MemberCallback<SoundManager>( this, &SoundManager::_resumeStreamingMusicAtPath ) );
        CallbackPtr callback = static_pointer_cast<Callback>(memberCallback);

        FileManager::getInstancePtr()->readFile( path, callback, plist );

        mCurrentMusicGroup = groupID;
        mMusicGroups[groupID].currentTrackID = trackID;

        // NDK - we can't get correct return value here with asynchronous reading...
        return true;
        
        
#else
        FMOD_RESULT result = mSystem->createStream(path.c_str(), FMOD_SOFTWARE | FMOD_LOOP_NORMAL, NULL, &mMusicSound);
		if (result != FMOD_OK)
		{
			Logger::printf("Walaber", Logger::SV_ERROR, "ERROR loading streamed audio file {%s} - [%s]\n", path.c_str(), FMOD_ErrorString(result));
			mMusicSound = NULL;
            ret = false;
		}
        else
        {	            
            mCurrentMusicGroup = groupID;
            mMusicGroups[groupID].currentTrackID = trackID;
            result = mSystem->playSound(FMOD_CHANNEL_FREE, mMusicSound, false, &mMusicChannel);
            mMusicChannel->setChannelGroup( mGroupMusic );
        }
        
        return ret;
#endif
    }
	
#ifdef TARGET_NDK
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundManager::_resumeStreamingMusicAtPath( void *params )
	{
		FileManager::ReadFileCallbackParameters *fileParams = reinterpret_cast<FileManager::ReadFileCallbackParameters*>(params);

		FMOD_CREATESOUNDEXINFO soundInfo;
		memset(&soundInfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
		soundInfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
		soundInfo.length = fileParams->length;

		FMOD_RESULT result = mSystem->createStream(fileParams->buffer, FMOD_SOFTWARE | FMOD_LOOP_NORMAL | FMOD_OPENMEMORY, &soundInfo, &mMusicSound);
		if (result != FMOD_OK)
		{
			Logger::printf("Walaber", Logger::SV_ERROR, "ERROR loading streamed audio file {%s} - [%s]\n", fileParams->path.c_str(), FMOD_ErrorString(result));
			mMusicSound = NULL;
		}
		else
		{
			// mCurrentMusicGroup = groupID;
			// mMusicGroups[groupID].currentTrackID = trackID;
			result = mSystem->playSound(FMOD_CHANNEL_FREE, mMusicSound, false, &mMusicChannel);
			mMusicChannel->setChannelGroup( mGroupMusic );
		}

		//if (fileParams->buffer)
		//	delete[] fileParams->buffer;
	}
#endif

    // --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundManager::initDefaultSubtitleText()
	{
		// go through all sounds that are marked as VO, and see if they have a matching entry in the TextManager.
		for (SoundGroupMap::iterator it = mSoundGroups.begin(); it != mSoundGroups.end(); ++it)
		{
			if (it->second.liveFiles.empty())
				continue;
			
			for (SoundList::iterator j = it->second.liveFiles.begin(); j != it->second.liveFiles.end(); ++j)
			{
				if ((*j)._isVO)
				{
					// this is a voice... see if TextManager has a key for it.
					std::string key = StringHelper::removeExtension(StringHelper::removePath((*j)._sourceFile));
					
					if (TextManager::subtitleExists(key))
						(*j)._subtitleText = key;
					
					Walaber::Logger::printf("Walaber", Logger::SV_DEBUG, "Subtitle found for [%s] --> '%s'\n",
											key.c_str(), TextManager::getString(key).c_str());
					
				}
			}
		}
	}
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundManager::subtitlePlayed( const std::string& text, float duration )
	{
		// a sound has played, we should show a subtitle for it.
		if (mSubtitleCallback)
		{
			SubtitleCallbackData data;
			data.text = text;
			data.duration = duration;
			
			mSubtitleCallback->invoke((void*)&data);
		}
	}
	
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundManager::stopMusic()
	{		
		if (mMusicChannel != NULL) 
		{
			mMusicChannel->stop();
		}
		
		if (mMusicSound != NULL)
		{
			mMusicSound->release();
			mMusicSound = NULL;
		}
		
        mCurrentMusicGroup = -1;
		mMusicChannel = NULL;
	}
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundManager::pauseMusic()
	{
		if (mMusicChannel != NULL)
			mMusicChannel->setPaused(true);
        
        for (int i=0; i<mHubMusicChannels.size(); i++)
        {
            if (mHubMusicChannels[i] != NULL)
            {
                mHubMusicChannels[i]->setPaused(true);
            }
        }
	}
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundManager::resumeMusic()
	{
		if (mMusicChannel != NULL)
			mMusicChannel->setPaused(false);
        
        for (int i=0; i<mHubMusicChannels.size(); i++)
        {
            if (mHubMusicChannels[i] != NULL)
            {
                if (i == mCurrentIndex)
                {
                    mHubMusicChannels[i]->setPaused(false);
                    break;
                }
            }
        }
	}
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	bool SoundManager::isMusicPlaying()
	{
		if (!mMusicChannel)
			return false;
		
		bool playing = false;
		mMusicChannel->isPlaying(&playing);
		
		if (playing)
		{
			bool paused = false;
			mMusicChannel->getPaused(&paused);
			
			if (paused)
				playing = false;
		}
		
		return playing;
	}
    
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=    
    bool SoundManager::isHubMusicPlaying()
    {
        bool musicPlaying = false;
        
        for (int i=0; i< mHubMusicChannels.size(); i++)
        {
            if (mHubMusicChannels[i] != NULL)
            {
                mHubMusicChannels[i]->isPlaying(&musicPlaying);
                
                if (musicPlaying)
                    break;
            }
        }
        
        return musicPlaying;

    }
    

	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	bool SoundManager::isMusicPaused()
	{
		if (!mMusicChannel)
			return false;
		
		bool playing = false;
		mMusicChannel->isPlaying(&playing);
		
		if (playing)
		{
			bool paused = false;
			mMusicChannel->getPaused(&paused);
			
			if (paused)
				return true;
		}
		
		return false;
	}
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	void SoundManager::debugLogMemory()
	{
		int alloced, maxalloced;
		
		FMOD::Memory_GetStats(&alloced, &maxalloced);
		
		Logger::printf("Walaber", Logger::SV_DEBUG, "SoundManager::debugLogMemory--- allocated[%d]bytes, max allocated[%d]bytes\n", alloced, maxalloced);
		
		
		// detailed info...
		FMOD_MEMORY_USAGE_DETAILS details;
		unsigned int memused;
		mSystem->getMemoryInfo(FMOD_MEMBITS_ALL, 0, &memused, &details);
		
		Logger::printf("Walaber", Logger::SV_DEBUG, "---------------------------\n");
		Logger::printf("Walaber", Logger::SV_DEBUG, "NAME\tBYTES:\n");
		Logger::printf("Walaber", Logger::SV_DEBUG, "---------------------------\n");
		Logger::printf("Walaber", Logger::SV_DEBUG, "SYSTEM\t%d\n", details.system);
		Logger::printf("Walaber", Logger::SV_DEBUG, "OTHER\t%d\n", details.other);
		Logger::printf("Walaber", Logger::SV_DEBUG, "PLUGINS\t%d\n", details.plugins);
		Logger::printf("Walaber", Logger::SV_DEBUG, "CODEC\t%d\n", details.codec);
		Logger::printf("Walaber", Logger::SV_DEBUG, "CHANNEL\t%d\n", details.channel);
		Logger::printf("Walaber", Logger::SV_DEBUG, "CHANNEL GROUP\t%d\n", details.channelgroup);
		Logger::printf("Walaber", Logger::SV_DEBUG, "FILE\t%d\n", details.file);
		Logger::printf("Walaber", Logger::SV_DEBUG, "SOUND\t%d\n", details.sound);
		Logger::printf("Walaber", Logger::SV_DEBUG, "STREAM BUFFER\t%d\n", details.streambuffer);
		Logger::printf("Walaber", Logger::SV_DEBUG, "TOTAL\t%d\n", memused);
		Logger::printf("Walaber", Logger::SV_DEBUG, "---------------------------\n");
	}
	
	
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	// --=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=--=
	SoundManager* SoundManager::getInstancePtr()
	{
		if (mInstancePtr == NULL)
		{
			mInstancePtr = new SoundManager();
		}
		
		return mInstancePtr;
	}
    
    void SoundManager::teardown()
    {
        if ( mInstancePtr != NULL )
        {
            delete mInstancePtr;
            mInstancePtr = NULL;
        }
    }
    
    FMOD_REVERB_PROPERTIES mDefaultProps;
    
    void SoundManager::addUnderWaterMusicFilter()
    {
        FMOD_RESULT result = FMOD_OK;
        FMOD_REVERB_PROPERTIES prop = FMOD_PRESET_BATHROOM;
        
        result = mSystem->getReverbProperties(&mDefaultProps);
        
        if (result != FMOD_OK)
        {
            Logger::printf("Walaber", Logger::SV_ERROR, "ERROR Getting Default Reverb properties [%s]\n", FMOD_ErrorString(result));
            return;
        }
        else
        {
            result = mSystem->setReverbProperties(&prop);
            
            if (result != FMOD_OK)
            {
                Logger::printf("Walaber", Logger::SV_ERROR, "ERROR Adding Underwater sound filter [%s]\n", FMOD_ErrorString(result));
                return;
            }
            else
            {
                Logger::printf("Walaber", Logger::SV_DEBUG, "Added Underwater sound filter \n");
            }
            
            mSystem->update();
        }
        
    }
    
    void SoundManager::removeUnderWaterMusicFilter()
    {
		FMOD_RESULT result = FMOD_OK;
        
        result = mSystem->setReverbProperties(&mDefaultProps);
        
        if (result != FMOD_OK)
		{
			Logger::printf("Walaber", Logger::SV_ERROR, "ERROR Removing Underwater sound filter [%s]\n", FMOD_ErrorString(result));
            return;
		}
		else
        {
            Logger::printf("Walaber", Logger::SV_DEBUG, "Removed Underwater sound filter \n");
        }
        
        mSystem->update();

    }
	
}

