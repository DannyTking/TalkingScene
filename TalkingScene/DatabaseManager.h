//
//  DatabaseManager.h
//  WalaberEngine
//
//  Created by Chad Cable on 4/15/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef _DATABASE_MANAGER_H_
#define _DATABASE_MANAGER_H_

#include <sqlite3.h>
#include <string>
#include <vector>
#include <map>
//#include <Color.h>
#include "Callback.h"

namespace Walaber 
{
    
    class DatabaseManager
    {
    public:
        
		struct RunSqlScriptErrorCallbackParameters
		{
			unsigned int errorCode;
			std::string sqlQuery;
			std::string errorDescription;
			bool outContinueExecuting;		// callback can set this to flag if we should continue executing lines or not.  default is false.
			bool outIgnoreError;			// callback can set this flag if we should not log this error torwards the total.   default is false.
		};
		
        struct RunSqlScriptCompletedCallbackParameters
        {
            std::string path;
            std::string actualPath;
			int databaseKey;
            int errorCount;
        };
        
        //------------------------------------------------------------------------------------------
        /// this is the main database for all game data, opened at launch, and closed on quit.
		typedef std::map< int, sqlite3* >		IntegerToDatabaseMap;
		static IntegerToDatabaseMap				databaseMap;
        /*
         #ifdef __GNUC__
         #define DEPRECATED(func) func __attribute__ ((deprecated))
         #elif defined(_MSC_VER)
         #define DEPRECATED(func) __declspec(deprecated) func
         #else
         #pragma message("WARNING: You need to implement DEPRECATED for this compiler")
         #define DEPRECATED(func) func
         #endif
         DEPRECATED(static bool openDatabase( const std::string& filename ));
         static bool openDatabase(const std::string& filename) __attribute__ ((deprecated))
         */
        //------------------------------------------------------------------------------------------
        DEPRECATED(static bool openDatabase( const std::string& filename ));
        DEPRECATED(static void closeDatabase());
		
        static bool openDatabase( const std::string& filename, int databaseKey );
        static void closeDatabase( int databaseKey );
        static sqlite3* getDatabase(int databaseKey);//chase 2013-05-28,for encrypt
        //------------------------------------------------------------------------------------------
        DEPRECATED(static bool insertEntry( const std::string& tableName, const std::string& data ));
        DEPRECATED(static bool insertEntry( const std::string& tableName, const std::string& colNames, const std::string& data ));
		
        static bool insertEntry( int databaseKey, const std::string& tableName, const std::string& data );
        static bool insertEntry( int databaseKey, const std::string& tableName, const std::string& colNames, const std::string& data );
        
        //------------------------------------------------------------------------------------------
        DEPRECATED(static bool updateEntry( const std::string& tableName, const std::string& set, const std::string& where ));
        DEPRECATED(static bool incrementValue( const std::string& tableName, const std::string& valToInc, const std::string& where)); 
		
        static bool updateEntry( int databaseKey, const std::string& tableName, const std::string& set, const std::string& where );
        static bool incrementValue( int databaseKey, const std::string& tableName, const std::string& valToInc, const std::string& where); 
        
		//------------------------------------------------------------------------------------------
		DEPRECATED(static void runSQL(const std::string& scriptFile, CallbackPtr doneCallback = CallbackPtr(), CallbackPtr errorCallback = CallbackPtr() ));
		
		static void runSQL( int databaseKey, const std::string& scriptFile, CallbackPtr doneCallback = CallbackPtr(), CallbackPtr errorCallback = CallbackPtr() );
		
        //------------------------------------------------------------------------------------------
        DEPRECATED(static bool deleteEntry( const std::string& tableName, const std::string& where ));
        DEPRECATED(static bool clearTable( const std::string& tableName ));
		
        static bool deleteEntry( int databaseKey, const std::string& tableName, const std::string& where );
        static bool clearTable( int databaseKey, const std::string& tableName );
        
        //------------------------------------------------------------------------------------------
        DEPRECATED(static int sumColAsInt( const std::string& query ));
        DEPRECATED(static int sumColAsInt( const std::string& select, const std::string& from, const std::string& where ));
        DEPRECATED(static float sumColAsFloat( const std::string& select, const std::string& from ));
	
        static int sumColAsInt( int databaseKey, const std::string& query );
        static int sumColAsInt( int databaseKey, const std::string& select, const std::string& from, const std::string& where );
        static float sumColAsFloat( int databaseKey, const std::string& select, const std::string& from );
		
		
        
        DEPRECATED(static int numResults( const std::string& select, const std::string& from, const std::string& where ));
		
        static int numResults( int databaseKey, const std::string& select, const std::string& from, const std::string& where );
		
		
        
        DEPRECATED(static bool updateEntryViaAddition( const std::string& tableName, const std::string& select, const int& val, const std::string& where ));
        DEPRECATED(static bool updateEntryWithLargestValue( const std::string& tableName, const std::string& select, const int& val, const std::string& where ));
        static int getPlayerPlayedTime( int databaseKey, const std::string& tableName, const std::string& select, const std::string& where );
		
        static bool updateEntryViaAddition( int databaseKey, const std::string& tableName, const std::string& select, const int& val, const std::string& where );
        static bool updateEntryWithLargestValue( int databaseKey, const std::string& tableName, const std::string& select, const int& val, const std::string& where );
		
		
        //------------------------------------------------------------------------------------------
        static std::string constructQuery( const std::string& select, const std::string& from, const std::string& where, const std::string& orderby );
        static std::string constructInsertQuery( const std::string& tableName, const std::string& colNames, const std::string& data );
        static std::string constructUpdateQuery( const std::string& tableName, const std::string& set, const std::string& where );
        static std::string constructDeleteQuery( const std::string& tableName, const std::string& where );
        
		
		//------------------------------------------------------------------------------------------
        static int firstEntryInMap();
		
    private:
        //------------------------------------------------------------------------------------------
        DEPRECATED(static bool queryDatabase( const std::string& query ));
		
        static bool queryDatabase( int databaseKey, const std::string& query );
		
		
		static void _readSQL(void* params);
        
        typedef std::multimap< std::string, CallbackPtr > CallbackMultiMap;
        static CallbackMultiMap mSqlScriptCallbacks;
		static CallbackMultiMap mSqlScriptErrorCallbacks;
    };
    
    
    class DatabaseIterator
    {
    public:
        //------------------------------------------------------------------------------------------
        DatabaseIterator();
		
        DEPRECATED(DatabaseIterator( const std::string& query ));
        DEPRECATED(DatabaseIterator( const std::string& select, const std::string& from ));
        DEPRECATED(DatabaseIterator( const std::string& select, const std::string& from, const std::string& where ));
        DEPRECATED(DatabaseIterator( const std::string& select, const std::string& from, const std::string& where, const std::string& orderby ));
		
        DatabaseIterator( int databaseKey, const std::string& query );
        DatabaseIterator( int databaseKey, const std::string& select, const std::string& from );
        DatabaseIterator( int databaseKey, const std::string& select, const std::string& from, const std::string& where );
        DatabaseIterator( int databaseKey, const std::string& select, const std::string& from, const std::string& where, const std::string& orderby );
		
        ~DatabaseIterator();
        
        //------------------------------------------------------------------------------------------
        bool next();
		
		//------------------------------------------------------------------------------------------
        bool reset();
        
        //------------------------------------------------------------------------------------------
        int getIntAtIndex( const int index );
        long getLongAtIndex( const int index );
        float getFloatAtIndex( const int index );
        bool getBoolAtIndex( const int index );
        std::string getStringAtIndex( const int index );
//        Color getColorAtIndex( const int index );
		bool isNullAtIndex( const int index );
        
    private:
        //------------------------------------------------------------------------------------------
        DEPRECATED(sqlite3_stmt* queryDatabase( const std::string& query ));
		
        sqlite3_stmt* queryDatabase( int databaseKey, const std::string& query );
        
        //------------------------------------------------------------------------------------------
        sqlite3_stmt*                           mStmt;
    };
    
}

#endif