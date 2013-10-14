#ifndef _WALABER_GAME_H_
#define _WALABER_GAME_H_

#include "BroadcastManager.h"
#include "PropertyList.h"

#include <string>
using std::string;

namespace Walaber
{
    class GameScreen;
    
    class WalaberGame : public MessageReceiver
    {
    public:
        
        virtual void initialize() = 0;
        virtual void update(float secondsSinceLastUpdate) = 0;
        virtual void draw() = 0;
        
        virtual bool messageRx(const Message& receivedMessage);
        
        virtual void onRelaunch() = 0;
        virtual void onActivated() = 0;
        
        virtual void loadingThreadFunction(GameScreen* screenToEnter,
                                           PropertyList& screenProperties) = 0;
        
        virtual void launchLoadingThreadForGameScreen(GameScreen* screenToEnter,
                                                      PropertyList& screenProperties);
        
        virtual void killLoadingScreenThread();
        
        // WalaberGame Events that must be implemented for each platform
        virtual void sendAnalyticsEvent(const char * eventName,
                                        PropertyList eventProperties) = 0;
        
        virtual void launchReferralStore() = 0;
        
        virtual void authenticatePlayerWithPlatform() = 0;
        
        virtual void showPlatformAchievementsView() = 0;
        
        virtual void sendAchievementToServer(const PropertyList& achievementProperties) = 0;
        
        void    uploadFileToCloud();
        void    resetCloudCache();
        void    requestCloudLoggedInStatus();
        void    logoutCloud();
        
        // Game Database methods
        void resetUserDatabase(int databaseKey);
        void openUserDatabase(int databaseKey);
        void setDatabaseName(const std::string & databaseName);
        void setDatabasePaths(const std::string & userPath,
                              const std::string & packagePath,
                              const std::string & contentDataPath,
                              const std::string & userStoreBasePath);
        void copyDatabaseFromBundle(const std::string& sourcePath,
                                    const std::string& destinationPath);
        virtual void updateDatabase(int databaseKey);
        virtual int  getDatabaseVersion(int databaseKey, const std::string & database) const = 0;
        virtual void onDatabaseUpdatingComplete() = 0;
        virtual void revertDatabaseVersion(int databaseKey, int version) = 0;
        
        const std::string & getDatabaseName() const;
        const std::string getUpdateScriptFilename(int updateFromVersion) const;
        const std::string getAmpsUpdateScriptFilename(int updateFromVersion) const;
        int  getNumberOfDatabaseUpdateSteps(int databaseKey);
        
        static void * LoadingScreenThreadEntryFunc(void * context);
        static bool IsLoadingThreadRunning()
        {
            return mLoadingThreadIsRunning;
        }
        
        // ads
        
        /*!
         Tell the platform to show ads for a specified screen type.
         
         @param screenId  The unique ID of the screen that needs to show ads.
         */
        virtual void showAds(int screenId) = 0;
        
        /*!
         Hide ads belonging to a particular screen from display
         
         @param screenId  The unique ID of the screen that needs to hide ads.
         */
        virtual void hideAds(int screenId) = 0;
        
        /*!
         Hide all ads from display
         */
        virtual void hideAds() = 0;
        
        /*!
         Query the device for amount of memory available
         
         @return    The number of bytes free on the device.
         */
        virtual unsigned int getFreeMemory() = 0;
        
        /*!
         Query the device for amount of memory used by the application.
         
         @return    The number of bytes used by the application.
         */
        virtual unsigned int getMemoryUsedByGame() = 0;
        
        /*!
         Query the game for current FPS.
         
         @return    FPS of the game
         */
        virtual unsigned int getFPS() = 0;
        
        /*!
         Manually sync any cloud storage services, if applicable.
         */
        virtual void cloudSync() = 0;
        
    protected:
        
        WalaberGame();
        virtual ~WalaberGame();
        
        struct LoadingThreadParameters
        {
            WalaberGame* game;
            GameScreen* screenToEnter;
            PropertyList screenProperties;
        };
        
        virtual void _applyThreadGLContext() {}
        
        std::string         mDatabaseName;      ///< the file name of the database file, including the extension e.g. "perry.db"
        std::string         mUserDataPath;      ///< the absolute path (only) to the .app file, expected to end with "/"
        std::string         mUserDatabase;      ///< the absolute path and filename of the user's database file
        std::string         mPackagePath;       ///< the absolute path to the "package", this is the userPath including the .app file
        std::string         mPackageContentPath;///< the absolute path to content data folder in the "package"
        std::string         mBundleDatabase;    ///< the absolute path and filename of the bundle's database file
        std::string         mUserStoreBasePath; ///< this is the base path to the "Documents" directory, where we can save user data and files
        
        bool mLoadingThreadLoaded;
        
        static bool mLoadingThreadShouldTerminate;
        static bool mLoadingThreadIsRunning;
    };
    
    class DatabaseCallbackClass
    {
    public:
        DatabaseCallbackClass(WalaberGame & GameInstance);
        ~DatabaseCallbackClass();
        
        // Callbacks
        void databaseCompletedCallback(void* data);
        void databaseErrorCallback(void* data);
        
        // Getters and setters
        void setNumErrors(int i)	{ numErrors = i; }
        int	getNumErrors()			{ return numErrors; }
        
    private:
        int numErrors;
        WalaberGame & mGameInstance;
    };
}

#endif