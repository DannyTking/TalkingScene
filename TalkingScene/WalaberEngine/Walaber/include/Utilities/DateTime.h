//
//  DateTime.h
//  WalaberEngine
//
//  Created by Jason Pecho on 11/30/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef _DATE_TIME_H_
#define _DATE_TIME_H_

#include <ctime>

namespace Walaber
{
	class DateTime
	{
	public:
		
		static void printCurrentDateTimeStruct()
		{
			// get local time struct
			time_t rawtime;
			struct tm * timeinfo;
			
			time ( &rawtime );
			timeinfo = localtime ( &rawtime );
			
			// print it out
			Logger::printf("Walaber", Logger::SV_INFO, "=========================================================\n");
			Logger::printf("Walaber", Logger::SV_INFO, "Current local time and date: %s", asctime (timeinfo) );
			Logger::printf("Walaber", Logger::SV_INFO, "=========================================================\n");
			Logger::printf("Walaber", Logger::SV_INFO, "Seconds:		%i\n", timeinfo->tm_sec);
			Logger::printf("Walaber", Logger::SV_INFO, "Minutes:		%i\n", timeinfo->tm_min);
			Logger::printf("Walaber", Logger::SV_INFO, "Hours:			%i\n", timeinfo->tm_hour);
			Logger::printf("Walaber", Logger::SV_INFO, "Day:			%i\n", timeinfo->tm_mday);
			Logger::printf("Walaber", Logger::SV_INFO, "Month:			%i\n", timeinfo->tm_mon);
			Logger::printf("Walaber", Logger::SV_INFO, "Year:			%i\n", timeinfo->tm_year);
			Logger::printf("Walaber", Logger::SV_INFO, "Weekday:		%i\n", timeinfo->tm_wday);
			Logger::printf("Walaber", Logger::SV_INFO, "Yearday:		%i\n", timeinfo->tm_yday);
			Logger::printf("Walaber", Logger::SV_INFO, "DST:			%i\n", timeinfo->tm_isdst);
		}
		
		static long getCurrentUnixTime()
		{			
			return time ( NULL );
		}
		
		static long getCurrentUnixLocalTime()
		{
			struct tm * tm;
			time_t t = time( NULL );
			tm = gmtime(&t);
			time_t t2 = mktime(tm);
			
			return t - (t2-t);
		}
		
		static void formatTimeString(char* buffer, size_t bufferSize, const std::string& formatString)
		{
			// get local time struct
			time_t rawtime;
			struct tm * timeinfo;
			
			time ( &rawtime );
			timeinfo = localtime ( &rawtime );
			
			strftime(buffer, bufferSize, formatString.c_str(), timeinfo);
		}
		
		static void formatUnixTimeString(char* buffer, size_t bufferSize, const std::string& formatString, long unixTime)
		{
			time_t t = (time_t)unixTime;
			struct tm* timeinfo = gmtime(&t);
			
			strftime(buffer, bufferSize, formatString.c_str(), timeinfo);
		}
	
		// returns true if the date represented by the unixTime (in local time) has already passed 
		static bool hasDatePassedUnixTime(long unixTime)
		{
			struct tm * tm;
			time_t t = time( NULL );
			tm = gmtime(&t);
			time_t t2 = mktime(tm);
			
			return (t - (t2-t) > unixTime);
		}
		
		
		static void getLocalTimeHourMinutesSeconds( int& hour, int& min, int& seconds )
		{
			// get local time struct
			time_t rawtime;
			struct tm * timeinfo;
			
			time ( &rawtime );
			timeinfo = localtime ( &rawtime );
			
			hour = timeinfo->tm_hour;
			min = timeinfo->tm_min;
			seconds = timeinfo->tm_sec;
		}
	};
}

#endif
