#ifndef _STRING_HELPER_H_
#define _STRING_HELPER_H_

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <cmath>
#include <cstdio>

//#include "Vector2.h"
#include "PropertyList.h"

namespace Walaber {
namespace StringHelper {

	// ------------------------------------------------------------------------------
    extern const int mDefaultFloatToStrPrecision;
	
	// ------------------------------------------------------------------------------
	std::string join( const std::vector< std::string > toJoin, const char delim = 0);

    
	// ------------------------------------------------------------------------------
    // generic solution
    template <class T>
    inline int numDigits(T number)
    {
        int digits = 1;
        if (number < 0) digits++; // remove this line if '-' counts as a digit
        while (abs(number) > 9) {
            number /= 10;
            digits++;
        }
        return digits;
    }
    
    // ------------------------------------------------------------------------------
	// overload for unsigned int since it can never be less than zero
    inline int numDigits(unsigned int number)
    {
        int digits = 1;
        while (abs(number) > 9) {
            number /= 10;
            digits++;
        }
        return digits;
    }
    
	// ------------------------------------------------------------------------------
	std::vector<std::string>& split(const std::string &s, char delim, std::vector<std::string> &elems);
	std::vector<std::string> split(const std::string &s, char delim);
	
	// ------------------------------------------------------------------------------
	std::string cleanLineEnding( const std::string &s );
	
	// ------------------------------------------------------------------------------
	std::string getExtension(const std::string& inPath);
	std::string removeExtension(const std::string& inPath);
	std::string changeExtension(const std::string& inPath, const std::string& newExtension);
	
	// ------------------------------------------------------------------------------
	std::string getPath(const std::string& inPath);
    std::string removePath(const std::string& inPath);
	std::string removeLastPathComponent(const std::string& inPath);
	
	// ------------------------------------------------------------------------------
	std::string appendPath(const std::string& basePath, const std::string& appendPath);
	
	// ------------------------------------------------------------------------------
	bool stringContains(const std::string& str, const std::string& test);
	
	// ------------------------------------------------------------------------------
	std::string toLower(const std::string& s);
    
    // ------------------------------------------------------------------------------
	inline float strToFloat(const std::string& s)
    {
        float ret = 0;
        sscanf(s.c_str(),"%f", &ret);
        return ret;
    }
    
    // ------------------------------------------------------------------------------
	inline int strToInt(const std::string& s)
    {
        int ret = 0;
        sscanf(s.c_str(),"%i", &ret);
        return ret;
    }
    
    // ------------------------------------------------------------------------------
	inline unsigned int strToUnsignedInt(const std::string& s)
    {
        unsigned int ret = 0;
        sscanf(s.c_str(),"%u", &ret);
        return ret;
    }
    
    // ------------------------------------------------------------------------------
	inline std::string floatToStr(const float fl, const int precision=mDefaultFloatToStrPrecision )
    {
        int numDig = (int)fl;
        numDig = numDigits(numDig);
        char buf[numDig + 1 + mDefaultFloatToStrPrecision];
        char buf2[16];
        sprintf(buf2, "%%.%if",mDefaultFloatToStrPrecision);
        sprintf(buf, buf2, fl);
        return (std::string(buf));
    }
    
    // ------------------------------------------------------------------------------
	inline std::string intToStr(const int i)
    {
        int numDig = numDigits(i);
        char buf[numDig];
        sprintf(buf, "%i", i);
        return (std::string(buf));
    }
    
    // ------------------------------------------------------------------------------
	inline std::string longToStr(const long l)
    {
        int numDig = numDigits(l);
        char buf[numDig];
        sprintf(buf, "%li", l);
        return (std::string(buf));
    }
    
    // ------------------------------------------------------------------------------
	inline std::string uIntToStr(const unsigned int ui)
    {
        int numDig = numDigits(ui);
        char buf[numDig];
        sprintf(buf, "%u", ui);
        return (std::string(buf));
    }
	
//	// --------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==
//	inline Vector2 strToVector2( const std::string& s )
//	{
//		Vector2 ret;
//		sscanf(s.c_str(), "%f %f", &ret.X, &ret.Y);
//		return ret;
//	}
//	
//	// --------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==--------==
//	inline std::string vector2ToStr( const Vector2& v )
//	{
//		std::string ret;
//		ret = floatToStr( v.X ) + " " + floatToStr( v.Y );
//		return ret;
//	}
	// ------------------------------------------------------------------------------
	inline std::string getExportSafeFilename(const std::string& inFilename)
	{
		char delimArray[] = {'/', ' '};
		unsigned int numDelims = 2;
		
		printf("Outgoing filename: %s\n", inFilename.c_str());
		std::string outFilename = inFilename;
		std::vector< std::string > splitString;
		
		for(unsigned int i = 0; i < numDelims; ++i)
		{
			printf("Splitting by: %c\n", delimArray[i]);
			splitString.clear();
			StringHelper::split(outFilename, delimArray[i], splitString);
			outFilename = join(splitString,'_');	
		}
		
		printf("Outgoing filename: %s\n", outFilename.c_str());
		return outFilename;	
	}
	
	// ------------------------------------------------------------------------------
	inline std::string join( const std::vector<std::string> toJoin, const char delim)
	{
		std::string ret;
		unsigned int size = toJoin.size();
		for(unsigned int i=0; i < size; ++i)
		{
			ret += toJoin[i];
			if(delim && ( i != (size - 1) ) )
			{
				ret += std::string(1, delim);
			}
		}
		
		return ret;
		
	}
	
	
	// ------------------------------------------------------------------------------
	// takes in string 'input', and replaces occurences of keys in formatPlist with their values.
	// keys in formatPlist should be single character ASCII values (e.g. 's', 'd', etc).
	// in the string, preceed the keys with a '%' character.
	//
	// for example if you have the string "I am %d years old, and my name is %s",
	// you will need formatPlist to have the following keys and values:
	//		'd'		==>		35
	//		's'		==>		'timmy'
	std::string formatString( const std::string& input, const PropertyList& formatPlist );

#ifndef TARGET_NDK
	// ------------------------------------------------------------------------------
	inline std::wstring stringToWideString( const std::string& str )
	{
		std::wstringstream ws;
		ws << str.c_str();		
		
		return ws.str();		
	}
	
	// ------------------------------------------------------------------------------
	inline std::string wideStringToString( const std::wstring& wstr )
	{
		std::stringstream ss;
		ss << wstr.c_str();		
		
		return ss.str();	
	}

	// ------------------------------------------------------------------------------
	// this function returns a buffer that the caller will need to free
	// ------------------------------------------------------------------------------
	inline std::vector<unsigned short> stringToShortWCharCStringASCII( const std::string& string )
	{
		unsigned int stringSize = string.size();
		std::vector<unsigned short> shortWChar;
		shortWChar.resize(stringSize+1);
		memset(&shortWChar[0], 0, ( stringSize + 1 ) * sizeof(unsigned short));
		const char* cstr = string.c_str();		
		unsigned short* buffer = &shortWChar[0];
		for(unsigned int i = 0; i < stringSize; ++i)
		{
			memcpy( (char*)(buffer + i), cstr + i, sizeof(char) );
		}
		
		return shortWChar;
	}
	
	// ------------------------------------------------------------------------------
	inline std::string shortWCharStringToStringASCII( const unsigned short* wstring, unsigned int strlen )
	{
		char* cstr = new char[strlen+1];
		memset(cstr, 0, strlen+1);
		
		for(unsigned int i = 0; i < strlen; ++i)
		{
			memcpy( cstr + i, (char*)(wstring + i), sizeof(char) );
		}
		std::string ret(cstr);
		
		delete [] cstr;
		
		return ret;
	}	
	
	// Needs null terminated short wstrings
	// ------------------------------------------------------------------------------
	inline unsigned int shortWCharStrLen(unsigned short* wcstr)
	{
		unsigned int i = 0;
		while(wcstr[i])
		{
			++i;
		}
		return i;
	}
#endif
}	
}


#endif	// _STRING_HELPER_H_
