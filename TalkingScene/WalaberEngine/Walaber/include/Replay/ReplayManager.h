#ifndef _REPLAY_MANAGER_H_
#define _REPLAY_MANAGER_H_

/*
#include <vector>
#include <iostream>
#include <fstream>

#include <BinaryWriter.h>


#define REPLAY_MANAGER_FILE_VERSION 1
#define REPLAY_MANAGER_FILE_HEADER "RPLY"

namespace Walaber 
{

	// --------========--------========--------========--------========--------========--------========
	class ReplayManager;
	
	
	// --------========--------========--------========--------========--------========--------========
	/// base class for all objects that can be replayed.
	class ReplayableObject
	{
	public:
		virtual void setTime( float previoustime, float currenttime ) = 0;
		
        virtual void addKeyframe( float currentTime ) = 0;
		
        virtual void clearKeyframes() = 0;
		
		virtual void clearKeyframesAfter( float refTime ) = 0;
		
        virtual void serializeToBinary(BinaryWriter& writer) {}
		
        virtual float buildFromBinary(BinaryReader& reader) { return 0.0f; }
		
		void setReplayManager( ReplayManager* r ) { mReplayManager = r; }
		ReplayManager* getReplayManager() { return mReplayManager; }
		
	protected:
		ReplayManager*			mReplayManager;
	};
	
	
	
	// --------========--------========--------========--------========--------========--------========
	// --------========--------========--------========--------========--------========--------========
	// Manager class for handling replay state, playback, saving, loading, etc.
	class ReplayManager
    {
	public:
		// --------========--------========--------========--------========--------========--------========	
        enum RecordMode { Recording, Playing };
		
		
		// --------========--------========--------========--------========--------========--------========	
        ReplayManager();
		~ReplayManager();
		
		
		
		// --------========--------========--------========--------========--------========--------========	
        bool addReplayObject(ReplayableObject* obj);
		
		
		// --------========--------========--------========--------========--------========--------========	
        void addKeyframes(float elapsed);
		
		// --------========--------========--------========--------========--------========--------========	
        void playbackSetTime( float time );
		
		// --------========--------========--------========--------========--------========--------========	
        void reset();
		
		// --------========--------========--------========--------========--------========--------========	
        void clearAllKeyframes();
		
		// --------========--------========--------========--------========--------========--------========	
        void clearAllKeyframesAfter( float refTime );
		
		// --------========--------========--------========--------========--------========--------========	
        void setRecordState( RecordMode rm ) { mRecordMode = rm; }
		RecordMode getRecordState() { return mRecordMode; }
		
		// --------========--------========--------========--------========--------========--------========	
        float getCurrentTime() { return mCurrentTime; }
		float getTotalTime() { return mEndTime; }
		
		// --------========--------========--------========--------========--------========--------========	
        void saveToFile( std::string filename );
		void saveToFileOnlyThese( std::vector<ReplayableObject*>& objects, std::string filename );
		
		// --------========--------========--------========--------========--------========--------========	
        void overwriteWithFileData( std::string filename );
		void overwriteWithStreamData( std::istream& inStream );
		
		
	protected:
		
		// --------========--------========--------========--------========--------========--------========	
        void _loadReplayVer1(BinaryReader& reader);
		
		
		// --------========--------========--------========--------========--------========--------========	
        std::vector<ReplayableObject*>		mReplayObjects;
        float								mCurrentTime;
        float								mEndTime;
		
        RecordMode							mRecordMode;

    };
}

*/
#endif		// _REPLAY_MANAGER_H_