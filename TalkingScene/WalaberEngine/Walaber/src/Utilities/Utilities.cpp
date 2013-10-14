//
//  Utilities.cpp
//  WalaberEngine
//
//  Created by Jason Pecho on 9/15/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "Utilities.h"
#include <stdlib.h>

namespace Walaber{
	namespace Utilities{
		
		std::string generateGUID()
		{
			// -----
			
			// Generates a GUID in the following format:
			// 3F2504E0-4F89-11D3-9A0C-0305E82C3301
			
			// 8 hex digits of data1
			// 4 hex digits of data2
			// 4 hex digits of data3
			// 4 first two bytes of data4
			// 12 remaining six bytes of data4
			
			time_t rawtime;
			struct tm* timeComponents;
			
			time ( &rawtime );
			timeComponents = gmtime( &rawtime );
			
			long randomNumber = 0;
			
			unsigned int data1 = 0; // 32 bits (26 bits of the timestamp + 6 bit random number)
			unsigned int data2 = 0; // 16-bit year
			unsigned int data3 = 0; // 16-bit random number
			unsigned int data4_0 = 0; // 16-bit random number
			unsigned int data4_1 = 0; // 24-bit random number
			unsigned int data4_2 = 0; // 24-bit random number
			
			// Data1
			// -----
			// Add 4-bit month
			data1 |= timeComponents->tm_mon;
			data1 <<= 5;
			
			// Add 5-bit day
			data1 |= timeComponents->tm_mday;
			data1 <<= 5;
			
			// Add 5-bit hour
			data1 |= timeComponents->tm_hour;
			data1 <<= 6;
			
			// Add 6-bit minutes
			data1 |= timeComponents->tm_min;
			data1 <<= 6;
			
			// Add 6-bit seconds
			data1 |= timeComponents->tm_sec;
			data1 <<= 6;
			
			// Add 6-bit random number
			randomNumber = ( rand() % 64 );
			data1 |= randomNumber;
			
			
			// Data2
			// -----
			// Add 16-bit year
			data2 = timeComponents->tm_year;
			
			// Data3
			// -----
			// Add 16-bit random number
			randomNumber = ( rand() % 65536 );
			data3 = randomNumber;
			
			// Data4_0 (first two bytes of data4)
			// ----------------------------------
			// Add 16-bit random number
			randomNumber = ( rand() % 65536 );
			data4_0 = randomNumber;
			
			// Data4_1 (next 3 bytes of data4)
			// -------------------------------
			// Add 24-bit random number
			randomNumber = ( rand() % 16777216 );
			data4_1 = randomNumber;
			
			// Data4_2 (final 3 bytes of data4)
			// --------------------------------
			randomNumber = ( rand() % 16777216 );
			data4_2 = randomNumber;
			
			
			char guidStr[ 37 ];
			sprintf( guidStr, "%08X-%04X-%04X-%04X-%06X%06X", data1, data2, data3, data4_0, data4_1, data4_2 );
			
			std::string guid = guidStr;
			
			return guid;
		}
	}
}
