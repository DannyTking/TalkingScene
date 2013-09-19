#include "StringHelper.h"

#include <ctype.h>

namespace Walaber{
namespace StringHelper {
    
    // ------------------------------------------------------------------------------
	const int mDefaultFloatToStrPrecision = 8;

	// ------------------------------------------------------------------------------
	std::vector<std::string>& split(const std::string &s, char delim, std::vector<std::string> &elems)
	{
		std::stringstream ss(s);
		std::string item;
		while(std::getline(ss, item, delim)) 
		{
			elems.push_back(item);
		}
		return elems;
	}


	// ------------------------------------------------------------------------------
	std::vector<std::string> split(const std::string &s, char delim)
	{
		std::vector<std::string> elems;
		return split(s, delim, elems);
	}
	
	
	// ------------------------------------------------------------------------------
	std::string cleanLineEnding( const std::string &s )
	{
		// look for '\r' at the end of a line, and clip that off!
		size_t pos = s.find("\r");
		if (pos == s.length()-1)
			return s.substr(0, pos);
		
		return s;
	}
	
	// ------------------------------------------------------------------------------
	std::string getExtension(const std::string& inPath)
	{
		size_t pos = inPath.find_last_of( '.' );
		if (pos == std::string::npos)
			return inPath; 
		else
			return inPath.substr( pos );
	}
	
	// ------------------------------------------------------------------------------
	std::string removeExtension(const std::string& inPath)
	{
		size_t pos = inPath.find_last_of( '.' );
		if (pos == std::string::npos)
			return inPath;
		else
			return inPath.substr( 0, pos );
	}
	
	// ------------------------------------------------------------------------------
	std::string changeExtension(const std::string& inPath, const std::string& newExtension)
	{
		return removeExtension(inPath) + newExtension;
	}
	
	// ------------------------------------------------------------------------------
	bool stringContains(const std::string& str, const std::string& test)
	{
		return str.find(test) != std::string::npos;
	}

	// ------------------------------------------------------------------------------
	std::string getPath(const std::string& inPath)
	{
		size_t pos = inPath.find_last_of( '/' );
		if (pos == std::string::npos)
			return std::string("");
		else
			return inPath.substr( 0, pos + 1 );
	}
    
    // ------------------------------------------------------------------------------
	std::string removePath(const std::string& inPath)
	{
		size_t pos = inPath.find_last_of( '/' );
		if (pos == std::string::npos)
			return inPath;
		else
			return inPath.substr( pos + 1 );
	}
	
	// ------------------------------------------------------------------------------
	std::string removeLastPathComponent(const std::string& inPath)
	{
		size_t pos = inPath.find_last_of( '/' );
		if (pos == std::string::npos)
			return inPath;
		else 
		{
			// if we have characters after the '/', remove them.
			if (pos < (inPath.size()-1))
			{
				return inPath.substr(0, pos+1);
			}
			else
			{
				// trim last "/" and call this function again.
				std::string removedSeparator = inPath.substr( 0, pos );
				return removeLastPathComponent(removedSeparator);
			}			
		}
	}
	
	
	// ------------------------------------------------------------------------------
	std::string appendPath(const std::string& basePath, const std::string& appendPath)
	{
		// check if there is a "/" at the end of basePath, if so remove it.
		size_t _baseLastP = basePath.find_last_of('/');
		
		std::string _baseP = basePath.substr( 0, (_baseLastP == basePath.size()-1) ? _baseLastP : basePath.size() );
		
		// check if there is a "/" at the beginning of appendPath, if so remove it.
		std::string _appendP = appendPath;
		
		size_t firstSlash = _appendP.find_first_of('/');
		if (firstSlash == 0)
			_appendP = _appendP.substr(1);
		
		
		return _baseP + "/" + _appendP;	
	}

	// ------------------------------------------------------------------------------
	std::string toLower(const std::string& s)
	{
		const char* cstr= s.c_str();
		
		char* newStr = new char[s.length() + 1];
		
		int size = s.length();
		int i;
		for(i=0; i < size; ++i)
		{
			newStr[i] = (unsigned char)tolower(cstr[i]);
		}
		newStr[i] = 0;
		
		std::string ret(newStr);
		
		delete [] newStr;
		return ret;
	}

	
	// ------------------------------------------------------------------------------
	std::string formatString( const std::string& input, const PropertyList& formatPlist )
	{
		std::string ret;
	
		std::vector< std::string > parts = StringHelper::split( input, '%' );
		
		for (int i = 0; i < parts.size(); i++)
		{
			std::string part = parts[i];
			
			if (part.empty())
				continue;
			
			// look for a character after the '%' that matches one in the plist...
			for (PropertyList::PropertyMap::const_iterator it = formatPlist.begin(); it != formatPlist.end(); it++)
			{
				if ( part[0] == ' ' )
				{
					if (i == 0)
					{
						if (input[0] != ' ')
						{
							break;
						}
					}
					
					ret += std::string("%");
					break;
				}
				else if ( part[0] == (*it).first[0] )
				{
					// Lets make sure we're not the first character in the string (without a preceding '%')
					if (i == 0)
					{
						if (input[0] != '%')
						{
							break;
						}
					}
					
					
					// found something!
					part = part.substr(1);
					
					ret += (*it).second.asString();
					
					break;
				}
			}
			
			// append this part...
			ret += part;
		}
		
		return ret;
	}

}
}
