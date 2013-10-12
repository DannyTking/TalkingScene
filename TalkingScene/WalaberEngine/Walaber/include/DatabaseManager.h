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
#include <Color.h>
#include <Callback.h>

namespace Walaber 
{
    /**
     * This is the class containning all the basic operation related to database.
     */
    class DatabaseManager
    {
    public:
        /**
         * A struct.
         * It contains the parameters needed for SQL script error callback function.
         */
		struct RunSqlScriptErrorCallbackParameters
		{
			unsigned int errorCode;
			std::string sqlQuery;
			std::string errorDescription;
			bool outContinueExecuting;		/**< callback can set this to flag if we should continue executing lines or not.  default is false. */
			bool outIgnoreError;			/**< callback can set this flag if we should not log this error torwards the total.   default is false. */
		};
		
        /**
         * A struct.
         * It contains the parameters needed for SQL script completed callback function.
         */
        struct RunSqlScriptCompletedCallbackParameters
        {
            std::string path;
            std::string actualPath;
			int databaseKey;
            int errorCount;
        };
        
        //------------------------------------------------------------------------------------------
        /**
         * This is the main database for all game data, opened at launch, and closed on quit.
         */
		typedef std::map< int, sqlite3* >		IntegerToDatabaseMap;
		static IntegerToDatabaseMap				databaseMap;
        
        //------------------------------------------------------------------------------------------
        DEPRECATED(static bool openDatabase( const std::string& filename ));
        DEPRECATED(static void closeDatabase());
		
        /**
         * Open database from filename.
         * @param filename the path of the database file.
         * @return bool value to indicate whether database is opened succesfully or not.
         */
        static bool openDatabase( const std::string& filename, int databaseKey );
        
        /**
         * Close database.
         */
        static void closeDatabase( int databaseKey );
        
        //------------------------------------------------------------------------------------------
        DEPRECATED(static bool insertEntry( const std::string& tableName, const std::string& data ));
        DEPRECATED(static bool insertEntry( const std::string& tableName, const std::string& colNames, const std::string& data ));

        /**
         * Insert a new row into a table.
         * @param tableName name of the table.
         * @param data values to insert into the table.
         * @return bool value to indicate whether this operation is successfully done.
         * @see insertEntry(const std::string&, const std::string&, const std::string&)
         */
        static bool insertEntry( int databaseKey, const std::string& tableName, const std::string& data );
        
        /**
         * Insert a new row into a table with column names.
         * @param tableName name of the table.
         * @param colNames names of columns.
         * @param data values to insert into the table.
         * @return bool value to indicate whether this operation is successfully done.
         * @see insertEntry(const std::string&, const std::string&)
         */
        static bool insertEntry( int databaseKey, const std::string& tableName, const std::string& colNames, const std::string& data );
        
        //------------------------------------------------------------------------------------------
        DEPRECATED(static bool updateEntry( const std::string& tableName, const std::string& set, const std::string& where ));
        DEPRECATED(static bool incrementValue( const std::string& tableName, const std::string& valToInc, const std::string& where)); 
		
        /**
         * Update existing records in a table.
         * @param tableName name of the table.
         * @param set values to update.
         * @param where specifies which record or records to be updated.
         * @return bool value to indicate whether this operation is successfully done.
         */
         static bool updateEntry( int databaseKey, const std::string& tableName, const std::string& set, const std::string& where );
        
        /**
         * Increase the value of selected cell by one.
         * @param tableName name of the table.
         * @param valToInc name of the column.
         * @param where specifies which record to be updated.
         * @return bool value to indicate whether this operation is successfully done.
         */
        static bool incrementValue( int databaseKey, const std::string& tableName, const std::string& valToInc, const std::string& where); 
        
		//------------------------------------------------------------------------------------------
		DEPRECATED(static void runSQL(const std::string& scriptFile, CallbackPtr doneCallback = CallbackPtr(), CallbackPtr errorCallback = CallbackPtr() ));
				
		/**
         * Execute the SQL statements contained in a file.
         * @param scriptFile name of the file which contains those SQL statements
         * @param doneCallback (optinal) callback function called after the SQL operations are done.
         * @param errorCallback (optional) callback function called when errors are happened during executing those SQL statements. 
         */
        static void runSQL( int databaseKey, const std::string& scriptFile, CallbackPtr doneCallback = CallbackPtr(), CallbackPtr errorCallback = CallbackPtr() );
		
        //------------------------------------------------------------------------------------------
        DEPRECATED(static bool deleteEntry( const std::string& tableName, const std::string& where ));
        DEPRECATED(static bool clearTable( const std::string& tableName ));
		
        /**
         * Delete a row or multple rows from table with conditional statement.
         * @param tableName name of the table
         * @param where conditional statement.
         * @return bool value to indicate whether this operation is successfully done.
         */
        static bool deleteEntry( int databaseKey, const std::string& tableName, const std::string& where );
        
        /**
         * Delete all the rows from a table.
         * @param tableName name of the table.
         * @return bool value to indicate whether this operation is successfully done.
         */
        static bool clearTable( int databaseKey, const std::string& tableName );
        
        //------------------------------------------------------------------------------------------
        DEPRECATED(static int sumColAsInt( const std::string& query ));
        DEPRECATED(static int sumColAsInt( const std::string& select, const std::string& from, const std::string& where ));
        DEPRECATED(static float sumColAsFloat( const std::string& select, const std::string& from ));

        /**
         * Return the int sum of values in the first column of the result-set.
         * @param query the SQL statement used to get the result-set.
         * @return int the sum of values in the first column of the result-set.
         * @see sumColAsInt( const std::string&, const std::string&, const std::string& )
         * @see sumColAsFloat( const std::string&, const std::string& )
         */
        static int sumColAsInt( int databaseKey, const std::string& query );
        
        /**
         * Return the int sum of values in the first column of the result-set.
         * @param select select statement.
         * @param from name of the table.
         * @param where conditional statement.
         * @return int the sum of values in the first column of the result-set.
         * @see sumColAsInt( const std::string& )
         * @see sumColAsFloat( const std::string&, const std::string& )
         */
        static int sumColAsInt( int databaseKey, const std::string& select, const std::string& from, const std::string& where );
        
        /**
         * Return the float sum of values in the first column of the result-set.
         * @param select select statement.
         * @param from name of the table.
         * @return int the sum of values in the first column of the result-set.
         * @see sumColAsInt( const std::string& )
         * @see sumColAsInt( const std::string&, const std::string&, const std::string& )
         */
        static float sumColAsFloat( int databaseKey, const std::string& select, const std::string& from );
        
        DEPRECATED(static int numResults( const std::string& select, const std::string& from, const std::string& where ));		

        /**
         * Return number of rows in the result-set.
         * @param select select statement.
         * @param from name of the table.
         * @param conditional statement.
         * @return number of rows in the result-set.
         */
        static int numResults( int databaseKey, const std::string& select, const std::string& from, const std::string& where );
        
        DEPRECATED(static bool updateEntryViaAddition( const std::string& tableName, const std::string& select, const int& val, const std::string& where ));
        DEPRECATED(static bool updateEntryWithLargestValue( const std::string& tableName, const std::string& select, const int& val, const std::string& where ));
		
        /**
         * Add value to specific cell or cells. The select statement can only contain one column.
         * @param tableName name of the table.
         * @param select select statement.
         * @param val int value to add to each cell in the column of result-set.
         * @param where conditional statement.
         * @return bool value to indicate whether this operation is successfully done.
         */
        static bool updateEntryViaAddition( int databaseKey, const std::string& tableName, const std::string& select, const int& val, const std::string& where );
        
        /**
         * Update cell values which are smaller than input value. In result-set, if the value is smaller than input value, update the original value with the input value.
         * @param tableName name of the table.
         * @param select select statement.
         * @param val int. Update values which are smaller than this value.
         * @param where conditional statement.
         * @return bool value to indicate whether we have replaced those values. True means that at least one value has been updated. False means no value has been updated.
         */
 		static bool updateEntryWithLargestValue( int databaseKey, const std::string& tableName, const std::string& select, const int& val, const std::string& where );
		
        //------------------------------------------------------------------------------------------
        /**
         * Construct a SQL query statement with input statement.
         * @param select select statement.
         * @param from name of the table.
         * @param where conditional statement.
         * @param orderby Order by statement.
         * @return a SQL statement constructed by the input string.
         */
        static std::string constructQuery( const std::string& select, const std::string& from, const std::string& where, const std::string& orderby );
        
        /**
         * Construct a SQL insert query statement with input statement.
         * @param tableName name of the table.
         * @param colNames name of the columns, which are seperated by ','.
         * @param data data to insert into table, which are seperated by ','. And it should have the same sequance as the input columns.
         * @return a SQL statement constructed by the input string.
         */
        static std::string constructInsertQuery( const std::string& tableName, const std::string& colNames, const std::string& data );
        
        /**
         * Construct a SQL update query statement with input statement.
         * @param tableName name of the table.
         * @param set set statement. For example, 'column1=value, colum2=value2'.
         * @param where conditional statement.
         * @return a SQL statement constructed by the input string.
         */
        static std::string constructUpdateQuery( const std::string& tableName, const std::string& set, const std::string& where );
        
        /**
         * Construct a SQL delete query statement with input statement.
         * @param tableName name of the table.
         * @param where conditional statement.
         * @return a SQL statement constructed by the input string.
         */
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
    
    
    /**
     * An object points to some row in database result-set. It can interate through the rows of the result-set by using a set of operators. Also, it provides methods to access data cell in current row.
     */
    class DatabaseIterator
    {
    public:
        //------------------------------------------------------------------------------------------
        /**
         * Default constructor.
         */
        DatabaseIterator();
		
        DEPRECATED(DatabaseIterator( const std::string& query ));
        DEPRECATED(DatabaseIterator( const std::string& select, const std::string& from ));
        DEPRECATED(DatabaseIterator( const std::string& select, const std::string& from, const std::string& where ));
        DEPRECATED(DatabaseIterator( const std::string& select, const std::string& from, const std::string& where, const std::string& orderby ));
        
        /**
         * Initial iterator with query statement.
         */
        DatabaseIterator( int databaseKey, const std::string& query );
        
        /**
         * Initial iterator with query statement.
         * @param select string which will be appended to SELECT command in the statement.
         * @param from name of the table.
         */
        DatabaseIterator( int databaseKey, const std::string& select, const std::string& from );
        
        /**
         * Initial iterator with query statement.
         * @param select string which will be appended to SELECT command in the statement.
         * @param from name of the table.
         * @param where string which will be appended to WHERE command in the statement.
         */
        DatabaseIterator( int databaseKey, const std::string& select, const std::string& from, const std::string& where );
        
        /**
         * Initial iterator with query statement.
         * @param select string which will be appended to SELECT command in the statement.
         * @param from name of the table.
         * @param where string which will be appended to WHERE command in the statement.
         * @param orderby string which will be appended to ORDER BY command in teh statement.
         */
        DatabaseIterator( int databaseKey, const std::string& select, const std::string& from, const std::string& where, const std::string& orderby );
        
        /**
         * Deconstructor.
         */
        ~DatabaseIterator();
        
        //------------------------------------------------------------------------------------------
        /**
         *  Move interator to point to next row in result-set. If there is no next row return false.
         *  @return True the object currently points to next row.
         *  @return False there is no next row in result-set.
         */
        bool next();
		
		//------------------------------------------------------------------------------------------
        bool reset();
        
        //------------------------------------------------------------------------------------------
        /**
         * Get the int value of specific column from current row.
         * @param index index of column for which information should be returned. The left column of the result-set has the index 0.
         * @return int value of the column. If iterator doesn't point to any row, return -1.
         */
        int getIntAtIndex( const int index );
        
        /**
         * Get the long int value of specific column from current row.
         * @param index index of column for which information should be returned. The left column of the result-set has the index 0.
         * @return long int value of the column. If iterator doesn't point to any row, return -1.
         */
        long getLongAtIndex( const int index );
        
        /**
         * Get the float value of specific column from current row.
         * @param index index of column for which information should be returned. The left column of the result-set has the index 0.
         * @return float value of the column. If iterator doesn't point to any row, return -1.0f.
         */
        float getFloatAtIndex( const int index );
        
        /**
         * Get the bool value of specific column from current row.
         * @param index index of column for which information should be returned. The left column of the result-set has the index 0.
         * @return bool value of the column. If iterator doesn't point to any row, return false.
         */
        bool getBoolAtIndex( const int index );
        
        /**
         * Get the string value of specific column from current row.
         * @param index index of column for which information should be returned. The left column of the result-set has the index 0.
         * @return string value of the column. If iterator doesn't point to any row, return blank string.
         */
        std::string getStringAtIndex( const int index );
        
        /**
         * Get the Color value of specific column from current row.
         * @param index index of column for which information should be returned. The left column of the result-set has the index 0.
         * @return Color value of the column. If iterator doesn't point to any row, return Color::White.
         */
        Color getColorAtIndex( const int index );
        
        /**
         * Check whether the type of the value in specific column from current row is null.
         * @param index index of column for which value will be checked. The left column of the result-set has the index 0.
         * @return Bool value to indicate whether its is null. If it is null return true.
         */
		bool isNullAtIndex( const int index );
        
    private:
        //------------------------------------------------------------------------------------------
        DEPRECATED(sqlite3_stmt* queryDatabase( const std::string& query ));
		
        /**
         * Compile the SQL query statement.
         * @param query SQL statement.
         * @return compiled SQL statement.
         */
        sqlite3_stmt* queryDatabase( int databaseKey, const std::string& query );
        
        //------------------------------------------------------------------------------------------
        sqlite3_stmt*                           mStmt;      /**< member to strore the compiled SQL statement.*/
    };
    
}

#endif
