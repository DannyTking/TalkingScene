#include "WalaberGame.h"
#include "Logger.h"
#include "DatabaseManager.h"
#include "StringHelper.h"
#include "FileHelper.h"
#include "ZipArchive.h"
#include "ZipArchiveUtilities.h"
#include <MemberCallback.h>
#include "ScreenManager.h"
#include "GameScreen.h"

#include <assert.h>
#include <pthread.h>

#ifdef TARGET_NDK
#include <memory>
#endif // #ifdef TARGET_NDK
namespace Walaber
{
    //static const char *DBG_TAG = "com.disney.Walaber.WalaberGame";
#if _DEBUG
    static const char *DBG_TAG_INIT = "com.disney.Walaber.WalaberGame.Init";
#endif
    
    bool WalaberGame::mLoadingThreadShouldTerminate = false;
    bool WalaberGame::mLoadingThreadIsRunning = false;
    
    void * WalaberGame::LoadingScreenThreadEntryFunc(void * context)
    {
        LoadingThreadParameters* loadingParameters = static_cast<LoadingThreadParameters*>(context);
        
        WalaberGame * game = loadingParameters->game;
        
        game->loadingThreadFunction(loadingParameters->screenToEnter,
                                    loadingParameters->screenProperties);
        
        game->mLoadingThreadIsRunning = false;
        
        return 0;
    }
    
    WalaberGame::WalaberGame()
            : MessageReceiver(Walaber::MC_ALL),
              mLoadingThreadLoaded(false)
    {
        Logger::printf("Walaber", Logger::SV_DEBUG, "WalaberGame::WalaberGame()\n");
    }

    WalaberGame::~WalaberGame()
    {
        
    }

    bool WalaberGame::messageRx(const Message& receivedMessage)
    {
        // If we've handled this message and don't want anyone else to handle it,
        // we can tell it to stop broadcasting by returning true from this function.
        bool stopBroadcastingMessage = false;

        if (receivedMessage.ID == (int) Walaber::MID_AchievementNotification)
        {
            sendAchievementToServer(receivedMessage.Properties);
            stopBroadcastingMessage = true;
        }

        return stopBroadcastingMessage;
    }
    
    void WalaberGame::launchLoadingThreadForGameScreen(GameScreen* screenToEnter,
                                                       PropertyList& screenProperties)
    {
        // Create the thread using POSIX routines.
        pthread_attr_t  attr;
        pthread_t       posixThreadID;
        int             returnVal;
        
        returnVal = pthread_attr_init(&attr);
        assert(!returnVal);
        
        returnVal = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        assert(!returnVal);
        
        mLoadingThreadIsRunning = true;
        mLoadingThreadShouldTerminate = false;
        
        static LoadingThreadParameters loadingParameters;
        loadingParameters.game = this;
        loadingParameters.screenToEnter = screenToEnter;
        loadingParameters.screenProperties = screenProperties;
        
        int     threadError = pthread_create(&posixThreadID,
                                             &attr,
                                             &WalaberGame::LoadingScreenThreadEntryFunc,
                                             &loadingParameters);
        
        returnVal = pthread_attr_destroy(&attr);
        
        assert(!returnVal);
        assert(threadError == 0);
    }

    void WalaberGame::killLoadingScreenThread()
    {
        mLoadingThreadShouldTerminate = true;
    }
    
    /*! Accessor to get the Database file name, name and extension
     @return the database file name
     */
    const std::string & WalaberGame::getDatabaseName() const
    {
        return mDatabaseName;
    }

    /*! Get the path and filename for the update script to update a database from the specified
     version
     @param updateFromVersion the version of the database to update from, the target will be
     updateFromVersion + 1
     */
    const std::string WalaberGame::getUpdateScriptFilename(int updateFromVersion) const
    {
        std::string filename;
#ifdef _LITE
        filename = mPackageContentPath + "UpdateScripts/update_db_" +
        Walaber::StringHelper::intToStr(updateFromVersion) + "_" +
        Walaber::StringHelper::intToStr(updateFromVersion+1) + "_lite.sql";
#else
        filename = mPackageContentPath + "UpdateScripts/update_db_"
                + Walaber::StringHelper::intToStr(updateFromVersion) + "_"
                + Walaber::StringHelper::intToStr(updateFromVersion + 1) + ".sql";
#endif
        return filename;
    }

    /*! Get the path and filename for the amps update script to update a database from the
     specified version
     @param updateFromVersion the version of the database to update from, the target will be
     updateFromVersion + 1
     */
    const std::string WalaberGame::getAmpsUpdateScriptFilename(int updateFromVersion) const
    {
        std::string filename;
#ifdef _LITE
        filename = mPackageContentPath + "UpdateScripts/amps_update_lite_" +
        StringHelper::intToStr(updateFromVersion) + "_" +
        StringHelper::intToStr(updateFromVersion+1) + ".xml";
#else
        filename = mPackageContentPath + "UpdateScripts/amps_update_"
                + StringHelper::intToStr(updateFromVersion) + "_"
                + StringHelper::intToStr(updateFromVersion + 1) + ".xml";
#endif
        return filename;
    }

    /*! Mutator to set the database file name
     @param databaseName filename (including extension) of the database file
     */
    void WalaberGame::setDatabaseName(const std::string & databaseName)
    {
        mDatabaseName = databaseName;
    }

    /*! Set the file paths to be used by the database
     * <p/>
     * <hr/>
     * <h3>Notes for Android</h3>
     * This gets a bit confusing - particularly between platforms where terminology
     * is slightly different and the path locations have somewhat different interpretations.
     * Furthermore, this changed slightly(!) in moving from Perry to Mickey
     * - in particular, both platforms now share the same database copy/update/reset code
     * (access via SQL statements is also common but handled elsewhere).
     * <p/>
     * <h3>Quick Summary</h3>
     * <ul>
     * <li><b>userPath</b> - Application's private storage directory path
     *         - NOTE: Walaber engine expects trailing slash
     * <li><b>packagePath</b> - fully qualified filename of Android APK
     * <li><b>contentDataPath</b> - path to the location of the game database (eg, perry.db) <b><i>within the bundle</i></b>
     * <li><b>userStoreBasePath</b> - root path to the location of where the <i>extracted</i> game database
     * should be located. Namely, it ends up being placed at the root of the private storage area
     * </ul>
     *
     * <h3>More detailed explanation</h3>
     * <p/>
     * To better understand this function's input parameters ...
     * <p/>
     * Fundamentally, we want at least one filename and two distinct "paths" as input:
     * <ol>
     * <li>"PackagePath" is to the filename of the installed application file itself - the "PackagePath"
     * (This is not a path but fully qualified filename).
     * <p/>
     * We don't care about the "application" aspect of it. But it happens to be the bundle of our asset data too.
     * Note: In the future, we might [unlikely] have multiple APK bundles to deal with - thus the
     * extra clarification on purpose of the APK filename.
     * <li>"UserPath" a path where we can store and extract private data - ie, not visible to the world
     * <li>"ContentDataPath" is a path too. But it is not a <i>file system</i> path.
     * Rather, it indicates where to look within the <i>bundle</i> for the database.
     * <p>
     * NOTE: The name is a bit misleading. It is not where game content assets are located (ie, textures, etc).
     * Instead, it's specifically the location of the database (and other game engine data settings and configuration info).
     * Historically, this directory path was "/Content/Data/" ... and, thus, the origin of the variable name.
     * That path has since changed but lives on in the name of the variable and function argument.
     * </ol>
     * <p/>
     * For both iOS and Android, it is assumed the bundled game database will be <i>extracted</i> to the root
     * of the private path (wherever it happens to be defined). For example:
     * <pre>
     *   Post Extracted database =          /data/data/com.disney.Mickey_Google/perry.db
     * </pre>
     * <p/>
     * Which leads to the next obvious question - From where is the original database extracted?
     * <p/>
     * The original database is stored in the Android APK ... so we need to know where the APK located.
     * In addition, we <i>also</i> need to know <i>where</i>, within that APK bundle, the original database can be found.
     * This info is obtained from the PackagePath and ContentDataPath parameters. For instance
     * <pre>
     *   Package Filename =                 /mnt/asec/com.disney.Mickey_Google-1/pkg.apk
     *   Content Data Path inside bundle =  /assets/Mickey/Data
     * </pre>
     * <p/>
     * And, finally, where do we place the post-extracted database?
     * <p/>
     * This is provided with the UserStoreBasePath parameter.
     * At present, we simply place the database at the root of this directory.
     * So, for Android, this is the same as the private storage directory (<i>but without the trailing '/'</i>).
     * <p/>
     * [On iOS there is a distinction between userPath and userStoreBasePath for the document storage.
     * On Android we just use the same as "UserPath" for our purposes.]
     * <pre>
     *   UserStoreBasePath =    /data/data/com.disney.Mickey_Google/
     * </pre>
     * </ul>
     * <p/>
     * Next is ...what do we do with those inputs?
     * <p/>
     * From the input parameters, {@link setDatabasePaths} creates six items used later by the database processing routines.
     * <ol>
     * <li>{@link #mPackagePath} - Stored "as is" (unmodified) from input parameter "packagePath"
     * <li>{@link #mUserDataPath} - Stored "as is" (unmodified) from input parameter "userPath"<br/>
     * This is the <i><b>root</b></i> of where we store various chunks of data. It is used as the basis
     * for creating additional directory path and file locations - eg mPackageContentPath and mUserDatabase, etc.
     * This is where we will extract the game database (eg, perry.db), asset files, and so forth.
     * <li>{@link #mPackageContentPath} - This is the <i>path</i> location of the <i>original</i>
     * game database (to be extracted from the bundled).
     * The path is created by appending the database name to the "contentDataPath".
     * <li>{@link #mUserStoreBasePath} - For Android, this is essentially synonymous with "UserPath" [and mUserDataPath].
     * It has a slightly different meaning and role from a code standpoint.
     * Android uses the same 'userPath' private data path location ... <i>but does not end in a '/'</i> (subtle difference).
     * <li>{@link #mUserDatabase} - This is the fully qualified filename to the "user copy" of the game database (/some/extracted/path/perry.db).
     * This is the database we modify during gameplay with player progress, achievements, configuration settings, etc.
     * It is copied to the root of the private data path (more specifically, a "mUserStoreBasePath" derivative).
     * In practical terms, this database is located at the root of the UserPath.
     * <li>{@link #mBundleDatabase} - This is the fully qualified filename to the "pristine" database <i>within</i> the packaged bundle
     * (/Mickey/Data/perry.db).
     * In other words, it is where we can find the raw, unmolested, original game database from the bundled package file (ie, the APK).
     * It is used during database reset and is never modified.
     * </ol>
     * <p/>
     * Some of the result paths look a bit funky. But this is intended.
     * Specifically, location of assets within the bundle is "fully qualified"
     * by prefixing that path with the bundle's filename as well.
     * <pre>
     *      D/Init.FileIO.Setup(30283): Path Setup (A:Input)
     *      D/Init.FileIO.Setup(30283):   Package Path: /mnt/asec/com.disney.MickeyGameApp-1/pkg.apk
     *      D/Init.FileIO.Setup(30283):   UserData Path: /data/data/com.disney.MickeyGameApp/
     *      D/Init.FileIO.Setup(30283):   ContentDataPath: /assets/Mickey/Data/
     *      D/Init.FileIO.Setup(30283):   UserStore Path: /data/data/com.disney.MickeyGameApp
     *      D/Init.FileIO.Setup(30283): Path Setup (B:Adjusted)
     *      D/Init.FileIO.Setup(30283):   Package Path: /mnt/asec/com.disney.MickeyGameApp-1/pkg.apk
     *      D/Init.FileIO.Setup(30283):   UserData Path: /data/data/com.disney.MickeyGameApp/
     *      D/Init.FileIO.Setup(30283):   ContentDataPath: /data/data/com.disney.MickeyGameApp/assets/Mickey/Data/
     *      D/Init.FileIO.Setup(30283):   UserStore Path: /data/data/com.disney.MickeyGameApp
     *      D/Init.FileIO.Setup(30283):   User Database: /data/data/com.disney.MickeyGameApp/perry.db
     *      D/Init.FileIO.Setup(30283):   Bundle Database: /data/data/com.disney.MickeyGameApp/assets/Mickey/Data/perry.db
     * </pre>
     * <p/>
     * For reference, here is the iOS equivalent output for the above:<br/>
     * Note the difference in the content data path<br/>
     * - iOS caller supplies "/Content/Mickey/Data/<br/>
     * - Android caller supplies "/assets/Mickey/Data/
     * <p/>
     * <pre>
     *   [com.disney.Walaber.WalaberGame.Init] DEBUG:Path Setup (A:Input)
     *      Package Path: /.../iPhone Simulator/5.1/Applications/9C279B6F-927E-4CE0-A9EA-EFBEAEBE79C7/WheresMyMickey.app
     *      UserData Path: /.../iPhone Simulator/5.1/Applications/9C279B6F-927E-4CE0-A9EA-EFBEAEBE79C7/Library/
     *      ContentDataPath: /Content/Mickey/Data/
     *      UserStore Path: /.../iPhone Simulator/5.1/Applications/9C279B6F-927E-4CE0-A9EA-EFBEAEBE79C7/Library
     *   [com.disney.Walaber.WalaberGame.Init] DEBUG:Path Setup (B:Adjusted)
     *      Package Path: /.../iPhone Simulator/5.1/Applications/9C279B6F-927E-4CE0-A9EA-EFBEAEBE79C7/WheresMyMickey.app
     *      UserData Path: /.../iPhone Simulator/5.1/Applications/9C279B6F-927E-4CE0-A9EA-EFBEAEBE79C7/Library/
     *      ContentDataPath: /.../iPhone Simulator/5.1/Applications/9C279B6F-927E-4CE0-A9EA-EFBEAEBE79C7/WheresMyMickey.app/Content/Mickey/Data/
     *      UserStore Path: /.../iPhone Simulator/5.1/Applications/9C279B6F-927E-4CE0-A9EA-EFBEAEBE79C7/Library
     *      User Database: /.../iPhone Simulator/5.1/Applications/9C279B6F-927E-4CE0-A9EA-EFBEAEBE79C7/Library/perry.db
     *      Bundle Database: /.../iPhone Simulator/5.1/Applications/9C279B6F-927E-4CE0-A9EA-EFBEAEBE79C7/WheresMyMickey.app/Content/Mickey/Data/perry.db
     * </pre>
     * <hr/>
     *
     * @param userPath the absolute path (only) to the .app file, expected to end with "/"
     * @param packagePath the absolute path to the "package", this is the userPath including
     * the .app/.apk file extension
     * @param contentDataPath the relative path to the content data folder where the database
     * file resides
     * @param userStoreBasePath - Location on the device where app can store user data
     * @param databaseKey the key into the database
     *
     */
    void WalaberGame::setDatabasePaths(const std::string & userPath,
            const std::string & packagePath, const std::string & contentDataPath,
            const std::string & userStoreBasePath)
    {

        // Setup path information
        mUserDataPath = userPath;      // example: /app/private/
        mPackagePath = packagePath;    // example: /some/path/pkg.apk (android)

        // Fully qualified path to user's database [extracted copy]
        mUserDatabase = mUserDataPath + mDatabaseName;
        // Super-Extra-Fully qualified _path_ to the location of the original database
        mPackageContentPath = mPackagePath + contentDataPath;
        // Fully qualified _filename_ of the original database inside bundle
        mBundleDatabase = mPackageContentPath + mDatabaseName;
        // Path to where app can place extracted data
        mUserStoreBasePath = userStoreBasePath;

#ifdef _DEBUG
        {
            // Output the paths to log. Useful info since impacts everything (game database, asset loading, etc)
            Walaber::Logger::printf(DBG_TAG_INIT, Walaber::Logger::SV_DEBUG,
                    "Path Setup (A:Input)\n  Package Path: %s\n  UserData Path: %s\n  ContentDataPath: %s\n  UserStore Path: %s\n",
                    packagePath.c_str(), userPath.c_str(), contentDataPath.c_str(),
                    userStoreBasePath.c_str());
            Walaber::Logger::printf(DBG_TAG_INIT, Walaber::Logger::SV_DEBUG,
                    "Path Setup (B:Adjusted)\n  Package Path: %s\n  UserData Path: %s\n  ContentDataPath: %s\n  UserStore Path: %s\n  User Database: %s\n  Bundle Database: %s\n",
                    mPackagePath.c_str(), mUserDataPath.c_str(), mPackageContentPath.c_str(),
                    mUserStoreBasePath.c_str(), mUserDatabase.c_str(), mBundleDatabase.c_str());
        }
#endif // #ifdef _DEBUG

    }

    /*! Open the database
     @param databaseKey the key into the database
     */
    void WalaberGame::openUserDatabase(int databaseKey)
    {
        Walaber::Logger::printf("WMW_CONTEXT", Walaber::Logger::SV_VERBOSE, "Database Path: %s",
                mUserDatabase.c_str());

        Walaber::DatabaseManager::closeDatabase(databaseKey);
        Walaber::DatabaseManager::openDatabase(mUserDatabase, databaseKey);
    }

    /*! Get the number of steps required to fully update the database. This is calculated based on
     the difference of the database version.
     @param databaseKey the key into the database
     @return the number of steps required to fully update the database
     */
    int WalaberGame::getNumberOfDatabaseUpdateSteps(int databaseKey)
    {
        int numSteps = 0;

        // Otherwise we need to check if a database update has occurred
        int userDatabaseVersion = getDatabaseVersion(databaseKey, mUserDatabase);
        int bundleDatabaseVersion = -1;

        std::string checkedUserPath(mBundleDatabase);

        // Check to see if the package path is a zip
        std::auto_ptr<Walaber::ZipArchiveReader> bundle(
                new Walaber::ZipArchiveReader(mPackagePath));
        std::string relativeBundlePath = mBundleDatabase.substr(mPackagePath.size() + 1);
        bool bundleIsZipped = false;
        if (bundle->openFile(relativeBundlePath))
        {
            // extract the database from zip bundle to user store to get its current version
            checkedUserPath = mUserDataPath + "/checked_tmp.db";
            copyDatabaseFromBundle(mBundleDatabase, checkedUserPath);
            bundleIsZipped = true;
        }

        bundleDatabaseVersion = getDatabaseVersion(databaseKey, checkedUserPath);

        if (bundleIsZipped)
        {
            FileHelper::deleteFile(checkedUserPath);
        }

        if ((userDatabaseVersion == -1) || (bundleDatabaseVersion == -1))
        {

            // error, just copy over the bundle database in this case!
            numSteps = 1;
        }
        else
        {
            numSteps = (bundleDatabaseVersion - userDatabaseVersion);
        }

        Walaber::Logger::printf("WMW_CONTEXT", Walaber::Logger::SV_FATAL, "Num Steps Path: %i",
                numSteps);
        return numSteps;
    }

    /*! copy the database from bundle, if the bundle is a zip file then extract it from the bundle,
     otherwise copy from the source path
     @param sourcePath the source path for the database file to be copied from
     @param destinationPath the destination path to copy the database file to
     */
    void WalaberGame::copyDatabaseFromBundle(const std::string& sourcePath,
            const std::string& destinationPath)
    {
        // First check to see if a zip exists, if it does extract the file from the zip
        // else copy the file from its source
        std::auto_ptr<ZipArchiveReader> bundle(new ZipArchiveReader(mPackagePath));

        // strip down the bundle path part
        std::string relativeBundlePath = sourcePath.substr(mPackagePath.size() + 1);

        if (bundle->openFile(relativeBundlePath))
        {
            bundle->extractFile(relativeBundlePath, destinationPath);
        }
        else
        {
            FileHelper::copyFile(sourcePath, destinationPath);
        }
    }

    /*! Updates the specified database performing any update steps that might be required
     @param databaseKey the key into the database
     @todo Move me to WalaberGame when game specific code has been removed
     */
    void WalaberGame::updateDatabase(int databaseKey)
    {
        bool copyFallback = false;

        // first, check if there is a database at all in the user store path.
        if (FileHelper::fileExists(mUserDatabase))
        {
            // we need to check if a database update has occurred
            int userDatabaseVersion = getDatabaseVersion(databaseKey, mUserDatabase);
            int bundleDatabaseVersion = -1;

            std::string checkedUserPath(mBundleDatabase);

            // Check to see if the package path is a zip
            std::auto_ptr<Walaber::ZipArchiveReader> bundle(
                    new Walaber::ZipArchiveReader(mPackagePath));
            std::string relativeBundlePath = mBundleDatabase.substr(mPackagePath.size() + 1);
            bool bundleIsZipped = false;
            if (bundle->openFile(relativeBundlePath))
            {
                // extract the database from zip bundle to user store to get its current version
                checkedUserPath = mUserDataPath + "/checked_tmp.db";
                copyDatabaseFromBundle(mBundleDatabase, checkedUserPath);
                bundleIsZipped = true;
            }

            bundleDatabaseVersion = getDatabaseVersion(databaseKey, checkedUserPath);

            if (bundleIsZipped)
            {
                FileHelper::deleteFile(checkedUserPath);
            }

            if ((userDatabaseVersion == -1) || (bundleDatabaseVersion == -1))
            {
                // error, just copy over the bundle database in this case!

                Walaber::Logger::printf("WMP_INIT", Walaber::Logger::SV_FATAL,
                        "updateDatabaseAt: version comparison failure");

                copyFallback = true;
            }
            else if (userDatabaseVersion < bundleDatabaseVersion)
            {
                // attempt upgrade of user database to new version!
                // re-open the user database...
                if (Walaber::DatabaseManager::openDatabase(std::string(mUserDatabase),
                        databaseKey))
                {
                    int updateFromVersion = userDatabaseVersion;

                    // Set up callbacks for reading SQL updates
                    DatabaseCallbackClass* dcc = new Walaber::DatabaseCallbackClass(*this);

                    // Reset dcc to no errors
                    dcc->setNumErrors(0);

                    // determine the upgrade sql script filename programatically.
                    std::string updateFile = getUpdateScriptFilename(updateFromVersion);
                    printf("attempting to run update script: %s\n", updateFile.c_str());

                    // Callbacks
                    Walaber::MemberCallbackPtr<DatabaseCallbackClass>::type memcallback(
                            new Walaber::MemberCallback<DatabaseCallbackClass>(dcc,
                                    &Walaber::DatabaseCallbackClass::databaseCompletedCallback));
                    Walaber::CallbackPtr completeCallback = Walaber::static_pointer_cast<
                            Walaber::Callback>(memcallback);

                    Walaber::MemberCallbackPtr<DatabaseCallbackClass>::type memcallback2(
                            new Walaber::MemberCallback<DatabaseCallbackClass>(dcc,
                                    &Walaber::DatabaseCallbackClass::databaseErrorCallback));
                    Walaber::CallbackPtr errorCallback = Walaber::static_pointer_cast<
                            Walaber::Callback>(memcallback2);

                    // GO GO GO!!
                    Walaber::DatabaseManager::runSQL(databaseKey, updateFile);

                    // And now AMPS it up!
                    updateFile = getAmpsUpdateScriptFilename(updateFromVersion);
                    Walaber::ZipUtilities::LoadArchiveDescription(updateFile,
                            mUserStoreBasePath + "/AMPS/Content", false);

                    // We stored all of the information in dcc, so let's see what happened...
                    if (dcc->getNumErrors() == 0)
                    {
                        // Keep going!
                        updateFromVersion++;
                    }
                    else
                    {
                        revertDatabaseVersion(databaseKey, updateFromVersion);
                        //break;
                    }

                    if (updateFromVersion == bundleDatabaseVersion)
                    {
                        onDatabaseUpdatingComplete();
                    }

                    Walaber::DatabaseManager::closeDatabase(databaseKey);
                }
                else
                {
                    copyFallback = true;
                }

            }
        }
        else
        {
            // if the database wasn't there, it's probably first run of the app.
            // Just copy bundled DB from app package
            copyFallback = true;
        }

        if (copyFallback)
        {
            if (FileHelper::fileExists(mUserDatabase))
            {
                FileHelper::deleteFile(mUserDatabase);
            }
            else
            {
                perror(
                        "ApplicationContext::updateDatabase() - previous database couldn't be deleted");
            }

            copyDatabaseFromBundle(mBundleDatabase, mUserDatabase);
        }
    }

    /*! Replace the database with the original
     @param databaseKey the key into the database
     @todo Move me to WalaberGame when game specific code has been removed
     */
    void WalaberGame::resetUserDatabase(int databaseKey)
    {
        // now delete the database and replace with the one in the bundle...
        Walaber::DatabaseManager::closeDatabase(databaseKey);

        // Delete and copy over again
        if (FileHelper::fileExists(mUserDatabase))
        {
            FileHelper::deleteFile(mUserDatabase);
        }

        updateDatabase(databaseKey);

        Walaber::DatabaseManager::openDatabase(mUserDatabase, databaseKey);
    }

    DatabaseCallbackClass::DatabaseCallbackClass(WalaberGame & gameInstance)
            : mGameInstance(gameInstance)
    {
    }

    DatabaseCallbackClass::~DatabaseCallbackClass()
    {
    }

    void DatabaseCallbackClass::databaseCompletedCallback(void* data)
    {
        // We did it!!
        DatabaseManager::RunSqlScriptCompletedCallbackParameters& params =
                *((DatabaseManager::RunSqlScriptCompletedCallbackParameters*) data);

        // Get the number of errors, and that's it!
        numErrors = params.errorCount;
    }

    void DatabaseCallbackClass::databaseErrorCallback(void* data)
    {
        DatabaseManager::RunSqlScriptErrorCallbackParameters& params =
                *((DatabaseManager::RunSqlScriptErrorCallbackParameters*) data);

        // Certain errors can be overlooked (specifically ALTER commands)
        if (params.errorCode == SQLITE_ERROR)
        {
            if ((StringHelper::stringContains(params.sqlQuery, "ALTER"))
                    || (StringHelper::stringContains(params.sqlQuery, "CREATE")))
            {
                params.outIgnoreError = true;
            }
        }

        // Only report "real" errors.
        if (!params.outIgnoreError)
        {
            // Ran into an error - report it in anayltics
            // ANALYTICS
            {
                // DEBUG print
                Walaber::Logger::printf("WMW", Walaber::Logger::SV_DEBUG,
                        "Update Script Error -> code: [%i] -> query: [%s] -> desc: [%s] \n",
                        (int) params.errorCode, params.sqlQuery.c_str(),
                        params.errorDescription.c_str());

                PropertyList eventProperties;
                eventProperties.setValueForKey("context", Property("update_script"));
                eventProperties.setValueForKey("action", Property("error"));
                eventProperties.setValueForKey("message", Property(params.errorDescription));

                mGameInstance.sendAnalyticsEvent("game_action", eventProperties);

            }
        }

        // Keep running
        params.outContinueExecuting = true;
    }

}
