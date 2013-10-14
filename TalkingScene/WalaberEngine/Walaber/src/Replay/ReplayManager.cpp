#include "ReplayManager.h"

/*
#include <WalaberMath.h>

namespace Walaber
{

	// --------========--------========--------========--------========--------========--------========	
	ReplayManager::ReplayManager() : mCurrentTime(0.0f), mEndTime(0.0f), mRecordMode(Recording) { }
	
	
	// --------========--------========--------========--------========--------========--------========	
	ReplayManager::~ReplayManager()
	{
	}
	
	// --------========--------========--------========--------========--------========--------========	
	bool ReplayManager::addReplayObject(ReplayableObject* obj)
	{
		if (mCurrentTime == 0.0f)
		{
			mReplayObjects.push_back(obj);
			
			obj->setReplayManager( this );
			
			return true;
		}
		
		return false;
	}
	
	// --------========--------========--------========--------========--------========--------========	
	void ReplayManager::addKeyframes(float elapsed)
	{
		if (mRecordMode == Recording)
		{
			for (unsigned int i = 0; i < mReplayObjects.size(); i++)
				mReplayObjects[i]->addKeyframe(mCurrentTime);
			
			mCurrentTime += elapsed;
			mEndTime = mCurrentTime;
		}
	}
	
	// --------========--------========--------========--------========--------========--------========	
	void ReplayManager::playbackSetTime( float time )
	{
		if (mRecordMode != Playing) { setRecordState(Playing); }
		
		float oldTime = mCurrentTime;
		mCurrentTime = clampf(time, 0.0f, mEndTime);
		
		for (unsigned int i = 0; i < mReplayObjects.size(); i++)
			mReplayObjects[i]->setTime(oldTime, time);
	}
	
	// --------========--------========--------========--------========--------========--------========	
	void ReplayManager::reset()
	{
		mCurrentTime = mEndTime = 0.0f;
		mReplayObjects.clear();
	}
	
	// --------========--------========--------========--------========--------========--------========	
	void ReplayManager::clearAllKeyframes()
	{
		mCurrentTime = mEndTime = 0.0f;
		for (unsigned int i = 0; i < mReplayObjects.size(); i++)
			mReplayObjects[i]->clearKeyframes();
	}
	
	// --------========--------========--------========--------========--------========--------========	
	void ReplayManager::clearAllKeyframesAfter( float refTime )
	{
		for (unsigned int i = 0; i < mReplayObjects.size(); i++)
			mReplayObjects[i]->clearKeyframesAfter(refTime);
		
		if (mCurrentTime < refTime)
			mCurrentTime = refTime;
		
		if (mEndTime > refTime)
			mEndTime = refTime;
	}
	
	// --------========--------========--------========--------========--------========--------========	
	void ReplayManager::saveToFile( std::string filename )
	{
		// save ALL replay objects...
		saveToFileOnlyThese( mReplayObjects, filename );
	}
	
	void ReplayManager::saveToFileOnlyThese( std::vector<ReplayableObject*>& objects, std::string filename )
	{
#ifdef _DEBUG
		printf("ReplayManager::saveToFileOnlyThese( %s ) - object count: %d\n", filename.c_str(), (int)objects.size());
#endif
		
		std::fstream out(filename.c_str(), std::ios_base::out | std::ios_base::binary);
		
		BinaryWriter writer(out);
		
		std::string header(REPLAY_MANAGER_FILE_HEADER);
		writer.Write(header);
		
		int8_t ver = REPLAY_MANAGER_FILE_VERSION;
		writer.Write(ver);
		
		int count = objects.size();
		writer.Write(count);
		
		for (unsigned int i = 0; i < objects.size(); i++)
			objects[i]->serializeToBinary(writer);
	}
	
	// --------========--------========--------========--------========--------========--------========	
	void ReplayManager::overwriteWithFileData( std::string filename )
	{
#ifdef _DEBUG
		printf("ReplayManager::overwriteWithFileData( %s )\n", filename.c_str());
#endif
		
		std::fstream inStream( filename.c_str(), std::ios_base::in | std::ios_base::binary );
		if (inStream.is_open())
			overwriteWithStreamData(inStream);
	}
	
	void ReplayManager::overwriteWithStreamData( std::istream& inStream )
	{
		if (inStream != NULL)
		{
			// read the file into memory first.
			BinaryReader reader(inStream);
			
			
			std::string header = reader.ReadString();
			if (header != REPLAY_MANAGER_FILE_HEADER)
				return;
			
			int version = (int)reader.ReadByte();
			
			if (version == 1)
				_loadReplayVer1(reader);
			
		}
	}
		
	// --------========--------========--------========--------========--------========--------========	
	void ReplayManager::_loadReplayVer1(BinaryReader& reader)
	{
		mEndTime = mCurrentTime = 0.0f;
		
		int objCount = reader.ReadInt32();
		
		if (objCount != mReplayObjects.size())
		{
			printf("Error!  replay object count does not equal replay object count!\n\n");
			return;
		}
		
#ifdef _DEBUG
		printf("ReplayManager::_loadReplayVer1 - got object count:%d\n", objCount);
#endif
		
		for (int i = 0; i < objCount; i++)
		{
#ifdef _DEBUG
			printf(" -- loading object %d of %d\n", i, objCount);
#endif
			
			float t = mReplayObjects[i]->buildFromBinary(reader);
			
			if (t > mEndTime) { mEndTime = t; }
		}
		
		mCurrentTime = mEndTime;
	}
	
	

}
*/