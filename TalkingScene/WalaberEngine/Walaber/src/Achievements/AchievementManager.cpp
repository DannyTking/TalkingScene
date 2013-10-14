/*
 *  AchievementManager.cpp
 *  JellyCar3
 *
 *  Created by jpecho on 12/15/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "AchievementManager.h"

#include <BroadcastManager.h>
#include <TextManager.h>
#include <StringHelper.h>

namespace Walaber
{
	const std::string AchievementManager::AchievementDBFieldNames[]= 
	{
		"ID",
		"Hidden",
		"PreEarnedDescription",
		"EarnedDescription",
		"Image",
		"Points",
		"PercentComplete",
		"SortingGroup",
        "FacebookDescription",
	};
	
	AchievementManager::AchievementManager():
	//mDatabase(NULL),
	mDisplayDuration(3.0f),
	mNotificationEvent(NULL)
	{}
	
	AchievementManager::~AchievementManager()
	{}
	
	void AchievementManager::loadAchievements(std::string tableName)
	{
		loadAchievements(DatabaseManager::firstEntryInMap(), tableName);
	}
	
	void AchievementManager::loadAchievements(int databaseKey, std::string tableName)
	{
		mDatabaseKey = databaseKey;
		mTableName = tableName;
		
		// Query the database for a list of menu items
		//std::string query = std::string("SELECT ") + 
        std::string select = AchievementDBFieldNames[AFN_ID] + std::string(", ") +
		AchievementDBFieldNames[AFN_HIDDEN] + std::string(", ") +
		AchievementDBFieldNames[AFN_HOWTO] + std::string(", ") +
		AchievementDBFieldNames[AFN_DESC] + std::string(", ") +
        AchievementDBFieldNames[AFN_FACEBOOK_DESCRIPTION] + std::string(", ") +
		AchievementDBFieldNames[AFN_IMAGE] + std::string(", ") +
		AchievementDBFieldNames[AFN_POINTS] + std::string(", ") +
		AchievementDBFieldNames[AFN_PERCENT_COMPLETE];
        std::string from = tableName;
        std::string orderBy = AchievementDBFieldNames[AFN_GROUP] + std::string(" ASC");

        DatabaseIterator dbIt(databaseKey, select, from, "", orderBy);
        
		std::vector<std::string> mKeys;
        while(dbIt.next())
        {
            Achievement ach;
            // got all of the rows we needed!
            std::string id = dbIt.getStringAtIndex(0);//std::string( (const char*)sqlite3_column_text(stmt, 0) );

            ach.mName = id;
            
            ach.mHidden = dbIt.getBoolAtIndex(1); 
            
            ach.mHowTo = dbIt.getStringAtIndex(2);
    
            ach.mDesc = dbIt.getStringAtIndex(3);
                        
            ach.mFacebookDescription = dbIt.getStringAtIndex(4);
            
            ach.mIcon = mImageBasePath + dbIt.getStringAtIndex(5);
            
            ach.mPoints = dbIt.getIntAtIndex(6);
            
            ach.mPercentComplete = dbIt.getFloatAtIndex(7);		
            
            mSortedAchievements.push_back(ach);
            mKeys.push_back(id);
        }
        
		// create map
        unsigned int size=mSortedAchievements.size();
        for(int i = 0; i < size; ++i)
        {
            mAchievements[mKeys[i]] = &mSortedAchievements[i];
        }
	}
	
	bool AchievementManager::reportAchievement(std::string id, float percentComplete, bool fireNotificationEvent, bool onResultsScreen)
	{
		bool ret = false;
		AchievementMap::iterator it = mAchievements.find(id);
		if (it != mAchievements.end()) 
		{
			if(it->second->mPercentComplete != percentComplete)
			{
				it->second->mPercentComplete = percentComplete;
				
				// update database
				_setDBPercentComplete(id, percentComplete);
				
				// send message to any listeners
				Message_AchievementUnlocked am(id, percentComplete);
				BroadcastManager::getInstancePtr()->messageTx(am);
                
                // add achievement to the queue
                if(mNotificationEvent.use_count() && fireNotificationEvent)
                {
                    AchievementNotificationParameters params;
                    params.mAchievementText = TextManager::getString("ACHIEVEMENT_UNLOCKED_PREFIX") + std::string(" ") + TextManager::getString(it->second->mName);
					params.mDisplayDuration = mDisplayDuration;
                    params.mResultsAchievement = false;
                    
                    // If fired on the results screen, use a specially positioned notification
                    if (onResultsScreen)
                    {
                        params.mResultsAchievement = true;
                    }
                    
                    mNotificationEvent->invoke(&params);
                }
                
                // Add the achievement to the "Achievements to publish to Facebook" vector
                mCompletedAchievementsToPublishToFacebook.push_back( it->second );
				
				ret = true;
			}
		}
		
		return ret;
	}
    
    void AchievementManager::clearCompletedAchievementsToPublishToFacebook()
    {
        mCompletedAchievementsToPublishToFacebook.clear();
    }
		
	void AchievementManager::resetAchievementProgress()
	{
		AchievementMap::iterator it = mAchievements.begin();
		for (; it != mAchievements.end(); ++it) 
		{
			it->second->mPercentComplete = 0.0f;
			_setDBPercentComplete(it->first, 0.0f);
		}		
	}
	
	void AchievementManager::sendAchievementsToPlatform()
	{
		AchievementMap::iterator it = mAchievements.begin();
		for (; it != mAchievements.end(); ++it) 
		{
			if(it->second->mPercentComplete > 0.0f)
			{
				// send message to any listeners
				Message_AchievementUnlocked am(it->first, it->second->mPercentComplete);
				BroadcastManager::getInstancePtr()->messageTx(am);
			}
		}
	}
		
	bool AchievementManager::_setDBPercentComplete(std::string id, float percentComplete)
	{	        
        std::string valString = StringHelper::floatToStr(percentComplete);
		
        std::string set = AchievementDBFieldNames[AFN_PERCENT_COMPLETE] + 
		std::string(" = ") + valString; 
        
        std::string where = AchievementDBFieldNames[AFN_ID] + 
		std::string(" = '") + id + std::string("'"); 
        
        bool retVal = DatabaseManager::updateEntry(mDatabaseKey, mTableName, set, where);
		
		return retVal;
	}
	
	// --==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
	// sorts achievements in order of those completed, followed by group #
	// --==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
	const std::list<Achievement*> AchievementManager::getAchievementsSortedByCompletion()
	{
		std::list<Achievement*> tmp;
		
		// get complete achievements, then add the rest
		unsigned int size = mSortedAchievements.size();
		for(unsigned int i=0; i < size; ++i)
		{
			if (mSortedAchievements[i].mPercentComplete > 0.0f) 
			{
				tmp.push_back(&mSortedAchievements[i]);
			}
		}
		for(unsigned int i=0; i < size; ++i)
		{
			if (mSortedAchievements[i].mPercentComplete < 100.0f) 
			{
				tmp.push_back(&mSortedAchievements[i]);
			}
		}
		
		return tmp;
	}
}
