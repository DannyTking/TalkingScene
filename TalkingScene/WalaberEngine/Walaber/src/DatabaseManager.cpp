//
//  DatabaseManager.cpp
//  WalaberEngine
//
//  Created by Chad Cable on 4/15/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include <PropertyList.h>
#include <FileManager.h>
#include <Logger.h>

#include "DatabaseManager.h"
#include "StringHelper.h"

namespace Walaber 
{
	DatabaseManager::IntegerToDatabaseMap DatabaseManager::databaseMap;
    DatabaseManager::CallbackMultiMap DatabaseManager::mSqlScriptCallbacks;
    DatabaseManager::CallbackMultiMap DatabaseManager::mSqlScriptErrorCallbacks;
    
    //------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	bool DatabaseManager::openDatabase( const std::string& filename )
	{
		// Simply use the same key for all inserts
		return openDatabase( filename, 0 );
	}
	
	//------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	void DatabaseManager::closeDatabase()
	{
		closeDatabase( firstEntryInMap() );
	}
	
	
	
	//------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	bool DatabaseManager::openDatabase( const std::string& filename, int databaseKey )
	{
		sqlite3* db;
		
		int rc;
		
		rc = sqlite3_open_v2(filename.c_str(), &db, SQLITE_OPEN_READWRITE, NULL);
		
		Logger::printf("Walaber", Logger::SV_DEBUG, "opening database: %s", filename.c_str());
		
		if( rc )
		{
			Walaber::Logger::printf("WMW",Logger::SV_FATAL, "Can't open database: %s %s\n", sqlite3_errmsg(db), filename.c_str());
			sqlite3_close(db);
			db = 0;
			return false;
		} 
		
		// Success! Insert the database into the map
		databaseMap[ databaseKey ] = db;
		
		
		// Misc. info about the database
		const char* libVer = sqlite3_libversion();
		Logger::printf("Walaber", Logger::SV_DEBUG, "SQLite Library version: %s  libversion no: %d\n", libVer, sqlite3_libversion_number());
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	void DatabaseManager::closeDatabase( int databaseKey )
	{
		// Iterator position
		IntegerToDatabaseMap::iterator it = databaseMap.find( databaseKey );
		
		// We have a database to close
		if (it != databaseMap.end())
		{
			sqlite3_close( it->second );
			
			// Also delete this from the map
			databaseMap.erase( it );
		}		
	}
	
	
	
	
    
    //------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	bool DatabaseManager::insertEntry( const std::string& tableName, const std::string& data )
    {
        return insertEntry( tableName, "", data );
    }
    
    //------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	bool DatabaseManager::insertEntry( const std::string& tableName, const std::string& colNames, const std::string& data )
    {
        return queryDatabase( constructInsertQuery( tableName, colNames, data ));
    }
    
	
	
	//------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	bool DatabaseManager::insertEntry( int databaseKey, const std::string& tableName, const std::string& data )
    {
        return insertEntry( databaseKey, tableName, "", data );
    }
    
    //------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	bool DatabaseManager::insertEntry( int databaseKey, const std::string& tableName, const std::string& colNames, const std::string& data )
    {
        return queryDatabase( databaseKey, constructInsertQuery( tableName, colNames, data ));
    }
	
	
	
	
	
	
	
    //------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	bool DatabaseManager::updateEntry( const std::string& tableName, const std::string& set, const std::string& where )
    {
        return queryDatabase( firstEntryInMap(), constructUpdateQuery( tableName, set, where ));
    }
    
	//------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	bool DatabaseManager::incrementValue( const std::string& tableName, const std::string& valToInc, const std::string& where)
    {
		return incrementValue( firstEntryInMap(), tableName, valToInc, where );
    }
	
	
	
	//------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	bool DatabaseManager::updateEntry( int databaseKey, const std::string& tableName, const std::string& set, const std::string& where )
    {
        return queryDatabase( databaseKey, constructUpdateQuery( tableName, set, where ));
    }
    
	//------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	bool DatabaseManager::incrementValue( int databaseKey, const std::string& tableName, const std::string& valToInc, const std::string& where)
    {
        bool ret = false;
        
        int num = -1;
        DatabaseIterator dit(databaseKey, valToInc, tableName, where);
        while(dit.next())
        {
            num = dit.getIntAtIndex(0);
            break;
        }
        
        if(num != -1)
        {
            char t[5];
            snprintf(t, 5, "%d", num+1);
            ret = updateEntry(databaseKey, tableName, (valToInc + "=" + std::string(t)), where);
        }
        
        return ret;
    }
	
	
	
	
	
	
	
	
	//------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	void DatabaseManager::runSQL( const std::string& sqlScriptFile, CallbackPtr doneCallback, CallbackPtr errorCallback )
	{
        runSQL( firstEntryInMap(), sqlScriptFile, doneCallback, errorCallback );
	}
	
	//------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	void DatabaseManager::runSQL( int databaseKey, const std::string& sqlScriptFile, CallbackPtr doneCallback, CallbackPtr errorCallback )
	{
        std::multimap<std::string, CallbackPtr>::iterator it = mSqlScriptCallbacks.find(sqlScriptFile);
        
		bool isFirst = false;
        bool sameCallback = false;
        
		if(it == mSqlScriptCallbacks.end())
        {
            isFirst = true;
        }
        else
        {
            if(doneCallback.get() == it->second.get())
            {
                sameCallback = true;
            }
        }
        
        if(!sameCallback)
        {
            mSqlScriptCallbacks.insert(std::make_pair(sqlScriptFile, doneCallback));
			mSqlScriptErrorCallbacks.insert(std::make_pair(sqlScriptFile, errorCallback));
        }
        
        if(isFirst)
        {
            PropertyList plist;
			
			// Pass along the database key
			plist.setValueForKey("DatabaseKey", databaseKey);
			
            FileManager::getInstancePtr()->readFile(sqlScriptFile, CallbackPtr( new Callback(&DatabaseManager::_readSQL) ), plist);
        }
	}
	
	
	
	
	
    
    
    //------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	bool DatabaseManager::deleteEntry( const std::string& tableName, const std::string& where )
    {
        return deleteEntry( firstEntryInMap(), tableName, where );
    }
	
    //------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	bool DatabaseManager::clearTable( const std::string& tableName )
    {
        return clearTable( firstEntryInMap(), tableName );
    }
	
	
	
	
	
    
    //------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	bool DatabaseManager::deleteEntry( int databaseKey, const std::string& tableName, const std::string& where )
    {
        return queryDatabase( databaseKey, constructDeleteQuery( tableName, where ));
    }
	
	//------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	bool DatabaseManager::clearTable( int databaseKey, const std::string& tableName )
    {
        return queryDatabase( databaseKey, constructDeleteQuery( tableName, "" ));
    }
    
	
	
	
	
	
    
    //------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
    bool DatabaseManager::queryDatabase( const std::string& query )
    {
		// Query using the first entry in the map
		return queryDatabase( firstEntryInMap(), query );
    }
	
	
	//------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
    bool DatabaseManager::queryDatabase( int databaseKey, const std::string& query )
    {
		// Find the database to query
		IntegerToDatabaseMap::iterator it = databaseMap.find( databaseKey );
		
		if (it != databaseMap.end())
		{
			// Now query for the database
			int rc = 0;
			sqlite3_stmt* stmt = NULL;
			
			rc = sqlite3_prepare_v2(it->second, query.c_str(), query.size()+1, &stmt, NULL);
			
			
			if (rc == SQLITE_OK)
			{
				rc = sqlite3_step(stmt);
				
				Logger::printf("Walaber", Logger::SV_DEBUG, "result [%d]\n", rc);
				
				if (rc == SQLITE_DONE)
				{
					Logger::printf("Walaber", Logger::SV_DEBUG, "SUCCESS!\n");
				}
			}
			
			sqlite3_finalize(stmt);
			
			if ((rc == SQLITE_OK) || (rc == SQLITE_DONE))
			{
				return true;
			}
			else 
			{
				Logger::printf("Walaber", Logger::SV_ERROR, "Database error: %s\n", sqlite3_errmsg(it->second));
				return false;
			}
		}
		
		// No database, return error
		return false;
    }
	
	
    
    //------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	void DatabaseManager::_readSQL(void* params)
    {
		FileManager::ReadFileCallbackParameters* rcp = (FileManager::ReadFileCallbackParameters*)params;
				
		
		// Let's make sure this database actually exists
		int databaseKey = -1;
		
		if (rcp->userData.keyExists("DatabaseKey"))
		{
			databaseKey = rcp->userData.getValueForKey("DatabaseKey")->asInt();
		}
		
		IntegerToDatabaseMap::iterator it = databaseMap.find( databaseKey );
		
		if (it != databaseMap.end())
		{
			// run this SQL script on the database!
			std::pair< CallbackMultiMap::iterator, CallbackMultiMap::iterator > errorCBrange = mSqlScriptErrorCallbacks.equal_range(rcp->path);
			
			
			RunSqlScriptCompletedCallbackParameters callbackParams;
			callbackParams.errorCount = 0;
			callbackParams.path = rcp->path;
			callbackParams.actualPath = rcp->actualPath;
			
			std::string str(rcp->buffer,rcp->length);
			// run through this file line-by-line, executing each statement...
			std::vector< std::string > queries = StringHelper::split( str, ';' );
			
			for (unsigned int i = 0; i < queries.size(); i++)
			{
				std::string& query = queries[i];
				
				Logger::printf("Walaber", Logger::SV_DEBUG, "got query: %s", query.c_str());

				if (query.length() > 5)
				{
					Logger::printf("Walaber", Logger::SV_DEBUG, "...executing...");
					
					sqlite3_stmt* stmt = NULL;
					bool gotError = false;
					
					// prepare the statement
					int rc = sqlite3_prepare_v2(it->second, query.c_str(), query.size()+1, &stmt, NULL);
					
					
					if (rc == SQLITE_OK)
					{
						// step this statement.
						while(true) 
						{
							// step the statement
							rc = sqlite3_step(stmt);
							
							if ((rc == SQLITE_DONE) || (rc == SQLITE_OK))
							{
								break;
							}
							else if (rc != SQLITE_ROW)
							{
								// must be an error!
								gotError = true;
								break;
							}
							
							Logger::printf("Walaber", Logger::SV_DEBUG, "ret[%d] ", rc);
						}
					}
					else
					{
						gotError = true;
					}
					
					std::string errDesc( sqlite3_errmsg(it->second) );
					
					
					
					// finalize the statement.
					sqlite3_finalize(stmt);
					
					
					
					if (gotError)
					{
						
						// report error preparing statement!
						RunSqlScriptErrorCallbackParameters sqlScriptErrorParameters;
						sqlScriptErrorParameters.errorCode = rc;
						sqlScriptErrorParameters.sqlQuery = query;
						sqlScriptErrorParameters.errorDescription = errDesc;
						sqlScriptErrorParameters.outContinueExecuting = false;
						sqlScriptErrorParameters.outIgnoreError = false;
						
						Logger::printf("Walaber", Logger::SV_DEBUG, "ERROR! [%s]", sqlScriptErrorParameters.errorDescription.c_str());
						
						for(CallbackMultiMap::iterator it_ = errorCBrange.first; it_ != errorCBrange.second; ++it_)
						{
							if(it_->second)
							{
								it_->second->invoke(&sqlScriptErrorParameters);
							}
						}
						
						if (!sqlScriptErrorParameters.outIgnoreError)
						{
							Logger::printf("Walaber", Logger::SV_DEBUG, "\n ignoring error\n");
							callbackParams.errorCount++;
						}
						
						if (!sqlScriptErrorParameters.outContinueExecuting)
						{
							Logger::printf("Walaber", Logger::SV_DEBUG, "\n not continuing after error, aborting!\n");
							break;
						}
					}
				}
		
				Logger::printf("Walaber", Logger::SV_DEBUG, "\n");
			}
			
			
			// cleanup file buffer...
			if (rcp->buffer)
			{
				delete[] rcp->buffer;
			}
			
			// final callback...
			std::pair< CallbackMultiMap::iterator, CallbackMultiMap::iterator > range = mSqlScriptCallbacks.equal_range(rcp->path);
			for(CallbackMultiMap::iterator it_ = range.first; it_ != range.second; )
			{
				if(it_->second)
				{
					it_->second->invoke(&callbackParams);
				}
				
				std::map< std::string, CallbackPtr >::iterator toErase = it_;
				++it_;
				
				mSqlScriptCallbacks.erase(toErase);
			}
			
			// cleanup error callback list as well.
			for(CallbackMultiMap::iterator it_ = errorCBrange.first; it_ != errorCBrange.second; )
			{
				std::map< std::string, CallbackPtr >::iterator toErase = it_;
				++it_;
				
				mSqlScriptErrorCallbacks.erase(toErase);
			}
		}
	}
	
	
	
	
	
	//------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	int DatabaseManager::firstEntryInMap()
    {
		if (!databaseMap.empty())
		{
			return databaseMap.begin()->first;
		}
		
		return -1;
	}
	
	
	
	
	
	
    
	//------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
    std::string DatabaseManager::constructQuery( const std::string& select, const std::string& from, const std::string& where, const std::string& orderby ) 
    {
        std::string returnQuery = std::string("SELECT ") + select;
        returnQuery += std::string(" FROM ") + from;
        
        if (where.length() != 0)
        {
            returnQuery += std::string(" WHERE ") + where;
        }
        
        if (orderby.length() != 0)
        {
            returnQuery += std::string(" ORDER BY ") + orderby;
        }
        
        return returnQuery;
    }

    //------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
    std::string DatabaseManager::constructInsertQuery( const std::string& tableName, const std::string& colNames, const std::string& data )
    {
        std::string returnQuery = std::string("INSERT INTO ") + tableName;
        
        if (colNames != "") 
        {
            returnQuery += std::string(" (") + colNames + std::string(")");
        }
        
        returnQuery += std::string(" VALUES (") + data + std::string(")");
        
        return returnQuery;
    }
    
    //------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
    std::string DatabaseManager::constructUpdateQuery( const std::string& tableName, const std::string& set, const std::string& where )
    {
        std::string returnQuery = std::string("UPDATE ") + tableName;
        
        returnQuery += std::string(" SET ") + set;
        
        if (where != "") 
        {
            returnQuery += std::string(" WHERE ") + where;
        }
        
        return returnQuery;
    }
    
    //------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
    std::string DatabaseManager::constructDeleteQuery( const std::string& tableName, const std::string& where )
    {
        std::string returnQuery = std::string("DELETE FROM ") + tableName;
        
        if (where != "")
        {
            returnQuery += std::string(" WHERE ") + where;
        }
        
        return returnQuery;
    }
    
    
	
	
	
	
	//------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
    int DatabaseManager::sumColAsInt( const std::string& select, const std::string& from, const std::string& where )
    {
        return sumColAsInt( firstEntryInMap(), select, from, where );
    }
    
    
	//------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
    int DatabaseManager::sumColAsInt( const std::string& query )
    {
        return sumColAsInt( firstEntryInMap(), query );
    }
    
    //------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
    float DatabaseManager::sumColAsFloat( const std::string& select, const std::string& from )
    {
        return sumColAsFloat( firstEntryInMap(), select, from );
    }      
    
	
	
	
	//------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
    int DatabaseManager::sumColAsInt( int databaseKey, const std::string& select, const std::string& from, const std::string& where )
    {
        return sumColAsInt( databaseKey, constructQuery( select, from, where, std::string("") ));
    }
    
    
	//------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
    int DatabaseManager::sumColAsInt( int databaseKey, const std::string& query )
    {
        DatabaseIterator dit( databaseKey, query );
        int sum = 0;
        
        while (dit.next())
        {
            sum += dit.getIntAtIndex(0);
        }
        
		return sum;
    }
    
    //------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
    float DatabaseManager::sumColAsFloat( int databaseKey, const std::string& select, const std::string& from )
    {
        DatabaseIterator dit( databaseKey, select, from );
        float sum = 0.0f;
        
        while (dit.next())
        {
            sum += dit.getFloatAtIndex(0);
        }
        
		return sum;
    }   
	
	
	
	
	
	
	
	
	
    //------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
    int DatabaseManager::numResults( const std::string& select, const std::string& from, const std::string& where )
    {
        return numResults( firstEntryInMap(), select, from, where );
    }
	
	//------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
    int DatabaseManager::numResults( int databaseKey, const std::string& select, const std::string& from, const std::string& where )
    {
        DatabaseIterator dit( databaseKey, DatabaseManager::constructQuery( select, from, where, std::string("") ));
        int sum = 0;
        
        while (dit.next())
        {
            sum += 1;
        }
        
		return sum;
    }
	
	
	
	
	
	
	
	//------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	bool DatabaseManager::updateEntryViaAddition( const std::string& tableName, const std::string& select, const int& val, const std::string& where )
	{
		return updateEntryViaAddition( firstEntryInMap(), tableName, select, val, where );
	}
	
	//------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	bool DatabaseManager::updateEntryWithLargestValue( const std::string& tableName, const std::string& select, const int& val, const std::string& where )
	{
		return updateEntryWithLargestValue( firstEntryInMap(), tableName, select, val, where );
	}
	
	
	
	//------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	bool DatabaseManager::updateEntryViaAddition( int databaseKey, const std::string& tableName, const std::string& select, const int& val, const std::string& where )
	{
		// Find the data currently in the database
		DatabaseIterator dit( databaseKey, DatabaseManager::constructQuery( select, tableName, where, std::string("") ) );
		
		if (dit.next()) 
		{
			int oldVal = dit.getIntAtIndex(0);
			
			// Update entry
			std::string set = select + "=" + StringHelper::intToStr(oldVal + val);
			DatabaseManager::updateEntry( databaseKey, tableName, set, where);
			
			return true;
		}				
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	bool DatabaseManager::updateEntryWithLargestValue( int databaseKey, const std::string& tableName, const std::string& select, const int& val, const std::string& where )
	{
		// Find the data currently in the database
		DatabaseIterator dit( databaseKey,  DatabaseManager::constructQuery( select, tableName, where, std::string("") ) );
		
		if (dit.next()) 
		{
			int oldVal = dit.getIntAtIndex(0);
			
			// Update entry with largest value
			if (oldVal < val)
			{
				std::string set = select + "=" + StringHelper::intToStr(val);
				DatabaseManager::updateEntry( databaseKey, tableName, set, where);
				
				return true;
			}
		}				
		
		// We didn't replace the value
		return false;
	}
	
	
	
	
	
	
	

	//------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	DatabaseIterator::DatabaseIterator():
    mStmt(0)
    {
    }
    
    //------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	DatabaseIterator::DatabaseIterator( const std::string& query ):
    mStmt(0)
    {
        // Now query for the database
        mStmt = queryDatabase( query );
    }
    
    //------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	DatabaseIterator::DatabaseIterator( const std::string& select, const std::string& from ):
    mStmt(0)
    {
        // Now query for the database
        mStmt = queryDatabase( DatabaseManager::constructQuery( select, from, std::string(""), std::string("") ));
    }
    
    //------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	DatabaseIterator::DatabaseIterator( const std::string& select, const std::string& from, const std::string& where ):
    mStmt(0)
    {
        // Now query for the database
        mStmt = queryDatabase( DatabaseManager::constructQuery( select, from, where, std::string("") ));
    }
    
    //------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	DatabaseIterator::DatabaseIterator( const std::string& select, const std::string& from, const std::string& where, const std::string& orderby ):
    mStmt(0)
    {
        // Now query for the database
        mStmt = queryDatabase( DatabaseManager::constructQuery( select, from, where, orderby ));
    }
	
	
	
	
	
	
	
	
	//------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	DatabaseIterator::DatabaseIterator( int databaseKey, const std::string& query ):
    mStmt(0)
    {
        // Now query for the database
        mStmt = queryDatabase( databaseKey, query );
    }
    
    //------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	DatabaseIterator::DatabaseIterator( int databaseKey, const std::string& select, const std::string& from ):
    mStmt(0)
    {
        // Now query for the database
        mStmt = queryDatabase( databaseKey, DatabaseManager::constructQuery( select, from, std::string(""), std::string("") ));
    }
    
    //------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	DatabaseIterator::DatabaseIterator( int databaseKey, const std::string& select, const std::string& from, const std::string& where ):
    mStmt(0)
    {
        // Now query for the database
        mStmt = queryDatabase( databaseKey, DatabaseManager::constructQuery( select, from, where, std::string("") ));
    }
    
    //------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	DatabaseIterator::DatabaseIterator( int databaseKey, const std::string& select, const std::string& from, const std::string& where, const std::string& orderby ):
    mStmt(0)
    {
        // Now query for the database
        mStmt = queryDatabase( databaseKey, DatabaseManager::constructQuery( select, from, where, orderby ));
    }

	
	
	
	
    
    //------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	DatabaseIterator::~DatabaseIterator()
    {
        if(mStmt)
        {
            sqlite3_finalize(mStmt);
        }
    }
    
    
    
    //------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	sqlite3_stmt* DatabaseIterator::queryDatabase( const std::string& query )
    {
        return queryDatabase( DatabaseManager::firstEntryInMap(), query );
    }
	
	
	
    //------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	sqlite3_stmt* DatabaseIterator::queryDatabase( int databaseKey, const std::string& query )
    {
		// Find the right database!
		DatabaseManager::IntegerToDatabaseMap::iterator it = DatabaseManager::databaseMap.find( databaseKey );
		
		if (it != DatabaseManager::databaseMap.end())
		{
			// Now query for the database
			int rc = 0;
			sqlite3_stmt* stmt = NULL;
			
			rc = sqlite3_prepare_v2(it->second, query.c_str(), query.size()+1, &stmt, NULL);
			
			if (rc == SQLITE_OK)
			{
				return stmt;
			}
			else 
			{
				Logger::printf("Walaber", Logger::SV_ERROR, "Database error: %s\n", sqlite3_errmsg(it->second));
			}
		}
		
		// That was a bad query (or no such database exists) - return null
		return NULL;
    }
	
	
	
    //------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	bool DatabaseIterator::next()
    {
        // Return if we haven't queried properly yet, or if the next element doesn't exist
        if (sqlite3_step(mStmt) == SQLITE_ROW)
        {
            return true;
        }
        
        return false;
    }
	
	//------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
	bool DatabaseIterator::reset()
	{
		if (sqlite3_reset(mStmt) == SQLITE_OK)
		{
			return true;
		}
		
		return false;
	}
    
    //------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
    int DatabaseIterator::getIntAtIndex( const int index )
    {
        if (mStmt == NULL)
        {
            return -1;
        }
        
        return sqlite3_column_int(mStmt, index);
    }
    
    //------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
    long DatabaseIterator::getLongAtIndex( const int index )
    {
        if (mStmt == NULL)
        {
            return -1;
        }
        
        return (long)sqlite3_column_int64(mStmt, index);
    }
    
    //------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
    float DatabaseIterator::getFloatAtIndex( const int index )
    {
        if (mStmt == NULL)
        {
            return -1.0f;
        }
        
        return sqlite3_column_double(mStmt, index);
    }
    
    //------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
    bool DatabaseIterator::getBoolAtIndex( const int index )
    {
        if (mStmt == NULL)
        {
            return false;
        }
        
        return (bool) sqlite3_column_int(mStmt, index);
    }
    
    //------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
    std::string DatabaseIterator::getStringAtIndex( const int index )
    {
        if (mStmt == NULL)
        {
            return std::string("");
        }
        
        return (const char*) sqlite3_column_text(mStmt, index);
    }
	
	//------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
    Color DatabaseIterator::getColorAtIndex( const int index )
    {
        if (mStmt == NULL)
        {
			return Color::White;
        }
        
        return Color::parseColorFromString( (const char*) sqlite3_column_text(mStmt, index) );
    }
	
	//------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------
    bool DatabaseIterator::isNullAtIndex( const int index )
	{
		return (sqlite3_column_type(mStmt, index) == SQLITE_NULL);
	}
}
