/*
 *  AchievementManager.h
 *  JellyCar3
 *
 *  Created by jpecho on 12/15/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _ACHIEVEMENTS_MANAGER_H_
#define _ACHIEVEMENTS_MANAGER_H_

#include <string>
#include <map>
#include <list>
#include <vector>
#include <DatabaseManager.h>
#include <Callback.h>


namespace Walaber 
{
	// --==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
	// achievement struct for reading and storing achievements from database
	// --==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
	struct Achievement
	{
		std::string		mName;
		std::string		mHowTo;
		std::string		mDesc;
        std::string     mFacebookDescription;
		std::string		mIcon;
		float			mPercentComplete;
		int				mPoints;
		bool			mHidden;
		
		Achievement():
		mName(), mHowTo(), mDesc(), mFacebookDescription(), mIcon(), mPercentComplete(0.0f), mPoints(0), mHidden(false)
		{}
		
		Achievement(std::string name, std::string howTo, std::string desc, std::string facebookDescription, std::string icon, 
					float percentComplete, float points, bool hidden):
		mName(name), mHowTo(howTo), mDesc(desc), mFacebookDescription(facebookDescription), mIcon(icon), mPercentComplete(percentComplete),
		mPoints( static_cast<int>( points ) ), mHidden(hidden)
		{}
		
		Achievement(const Achievement& achievement):
		mName(achievement.mName), mHowTo(achievement.mHowTo), mDesc(achievement.mDesc), 
        mFacebookDescription(achievement.mFacebookDescription),
		mIcon(achievement.mIcon), mPercentComplete(achievement.mPercentComplete),
		mPoints(achievement.mPoints), mHidden(achievement.mHidden)
		{}
		
		Achievement& operator=(const Achievement& achievement)
		{
			mName = achievement.mName;
			mHowTo = achievement.mHowTo;
			mDesc = achievement.mDesc;
            mFacebookDescription = achievement.mFacebookDescription;
			mIcon = achievement.mIcon;
			mPercentComplete = achievement.mPercentComplete;
			mPoints = achievement.mPoints;
			mHidden = achievement.mHidden;
			
			return *this;
		}
	};
	typedef std::map<std::string, Achievement*> AchievementMap;
	
	class AchievementManager 
	{
	public:	
        // --==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
		// Achievement Notification Parameters
		// --==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
		struct AchievementNotificationParameters
        {
            std::string mAchievementText;
			float		mDisplayDuration;
            bool        mResultsAchievement;
        };
        
		// --==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
		// singleton class
		// --==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
		static inline AchievementManager* getInstancePtr()
        {
            static AchievementManager instance;
            return &instance;
        }
		
		// --==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
		// appended to image name in database
		// --==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
		void setImageBasePath(const std::string& imagePath) { mImageBasePath = imagePath; }
		
		// --==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
		// set the duration that the achievement alert should display
		// --==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
		void setDisplayDuration(const float displayDuration) { mDisplayDuration = displayDuration; }
		
		// --==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
		// assumes an open database
		// --==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
		//void setDatabase(DatabaseManager* database) { mDatabase = database; }
	
		// --==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
		// load achievements from database
		// --==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
		DEPRECATED(void loadAchievements(std::string tableName));
		void loadAchievements(int databaseKey, std::string tableName);
		
		// --==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
		// log achievement locally, update database and send a message to any reciever who 
		// wants to be notified about achievements
		// returns whether or not the achievement info changed
		// --==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
		bool reportAchievement(std::string id, float percentComplete=100.0f, bool fireNotificationEvent = true, bool onResultsScreen = false);
		
		// --==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
		// reset all Achievement Progress to 0
		// --==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
		void resetAchievementProgress();
		
        // --==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
		// Get the list of completed achievements that have not yet been published to Facebook
		// --==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
        const std::vector<Achievement*>& getCompletedAchievementsToPublishToFacebook() const { 
            return mCompletedAchievementsToPublishToFacebook; 
        }
        
        // --==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
		// Clear the list of completed achievements that have not yet been published to Facebook
		// --==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
		void clearCompletedAchievementsToPublishToFacebook();
        
		// --==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
		// send message about all achievements to platform
		// --==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
		void sendAchievementsToPlatform();
		
		// --==--==--==--==--==--==--==--==--==--==--==--==--==-==--==--==--==--==--==--==--==--==
		// assignement of callback function
		// --==--==--==--==--==--==--==--==--==--==--==--==--==-==--==--==--==--==--==--==--==--==
		void setAchievementNotificationEvent(CallbackPtr mEvent) { mNotificationEvent = mEvent; }
		
		CallbackPtr getAchievementNotificationEvent() { return mNotificationEvent; }
		
		// --==--==--==--==--==--==--==--==--==--==--==--==--==-==--==--==--==--==--==--==--==--==
		// get a const reference to the achievement map
		// --==--==--==--==--==--==--==--==--==--==--==--==--==-==--==--==--==--==--==--==--==--==
		const AchievementMap& getAchievements() const { return mAchievements; }
		const std::vector<Achievement>& getSortedAchievements() const { return mSortedAchievements; }
		const std::list<Achievement*> getAchievementsSortedByCompletion();
		
	private:
		// --==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
		// private ctor and dtor because class is singleton
		// --==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
		AchievementManager();
		~AchievementManager();
		
		// --==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
		// no implementation of copy ctor and op= to prevent copying a singleton
		// --==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
		AchievementManager(const AchievementManager& acMan);
		AchievementManager& operator=(const AchievementManager& acMan);
		
		// --==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
		// helper function to update percent complete value in database
		// --==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
		bool _setDBPercentComplete(std::string id, float percentComplete);
		
		AchievementMap						mAchievements; // map holding ALL achievements in game
		std::vector<Achievement>			mSortedAchievements; // achievements sorted by group
		std::string							mImageBasePath; // image base path to appaend to image names
		int									mDatabaseKey; // which database to look in
		std::string							mTableName; // name of table in database that holds the achievements
		//DatabaseManager*					mDatabase; // pointer to OPEN database
		
		static const std::string			AchievementDBFieldNames[]; // helper array of database field names
		
		float								mDisplayDuration;

        
		CallbackPtr                         mNotificationEvent; // notification callback function ptr
		
        std::vector<Achievement*>           mCompletedAchievementsToPublishToFacebook;
        
		// --==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
		// enum helper for index into database field name helper array
		// --==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==--==
		enum AchievementFieldNames
		{
			AFN_ID,
			AFN_HIDDEN,
			AFN_HOWTO,
			AFN_DESC,
			AFN_IMAGE,
			AFN_POINTS,
			AFN_PERCENT_COMPLETE,
			AFN_GROUP,
            AFN_FACEBOOK_DESCRIPTION,
		};
	};
}

#endif
