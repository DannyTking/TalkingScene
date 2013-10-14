/*
 *  TextManager.cpp
 *  JellyCar3
 *
 *  Created by jpecho on 11/17/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "TextManager.h"

#include "StringHelper.h"
#include "FileManager.h"

#include <iostream>
#include <fstream>
#include <cassert>

namespace Walaber
{
	TextManager::LanguageMap TextManager::mDictionary;
	TextManager::SubtitleMap TextManager::mSubtitles;
	Language TextManager::mCurrentLanguage = ENGLISH_NTSC;
	std::vector< Language > TextManager::mLanguagesToLoad;
	
	/// <summary>
	/// completely remove all languages and strings from the dictionary.
	/// </summary>
	void TextManager::clearAll()
	{
		mDictionary.clear();		
	}
	
	/// <summary>
	/// returns a language-specific string, given the name for that string.
	/// </summary>
	/// <param name="name">string name (ID)</param>
	/// <param name="l">desired language</param>
	/// <returns>localized string (if exists), otherwise name + "*S*" when the string name does not exist, or name + "*L*" when language does not exist.</returns>
	std::string TextManager::getString(std::string name, Language l)
	{
		std::string ret;
		
		if(name.size())
		{
			LanguageMap::const_iterator it= mDictionary.find(l);
			
			if (it != mDictionary.end())
			{
				std::map<std::string, std::string>::const_iterator stringIt = it->second.find(name);
				if (stringIt != it->second.end())
				{
					ret = stringIt->second;
				}
				else
				{
					ret = name + "*S*";
				}
			}
			else
			{
				ret = name + "*L*";
			}	
		}
		
		return ret;
	}
	
	/// <summary>
	/// Get a localize string, using the currently set language.
	/// </summary>
	/// <param name="name">string name (ID)</param>
	/// <returns>localized string (if exists), otherwise "STRING ERROR" when the string name does not exist, or "LANG_ERR" when language does not exist.</returns>
	std::string TextManager::getString(std::string name)
	{
		return getString(name, mCurrentLanguage);
	}
	
	/// <summary>
	/// returns a boolean, given the name for that string.
	/// </summary>
	/// <param name="name">string name (ID)</param>
	/// <param name="l">desired language</param>
	/// <returns>true (if exists), otherwise false.</returns>
	bool TextManager::stringExists(std::string name, Language l)
	{
		if(name.size())
		{
			LanguageMap::const_iterator it = mDictionary.find(l);
			
			if (it != mDictionary.end())
			{
				std::map<std::string, std::string>::const_iterator stringIt = it->second.find(name);
				if (stringIt != it->second.end())
				{
					return true;
				}
			}
		}
		
		return false;
	}
	
	/// <summary>
	/// returns a boolean, given the name for that string.
	/// </summary>
	/// <param name="name">string name (ID)</param>
	/// <returns>true (if exists), otherwise false.</returns>
	bool TextManager::stringExists(std::string name)
	{
		return stringExists(name, mCurrentLanguage);
	}
	
	
	TextManager::SubtitleInfo TextManager::getSubtitle(std::string assetName, Language l)
	{
		SubtitleInfo ret;
		
		if(assetName.size())
		{
			SubtitleMap::const_iterator it= mSubtitles.find(l);
			
			if (it != mSubtitles.end())
			{
				std::map<std::string, SubtitleInfo>::const_iterator stringIt = it->second.find(assetName);
				if (stringIt != it->second.end())
				{
					ret = stringIt->second;
				}
				else
				{
					ret.text = assetName + "*S*";
					ret.character = "ERR *S*";
				}
			}
			else
			{
				ret.text = assetName + "*L*";
				ret.character = "ERR *L*";
			}	
		}
		
		return ret;
	}
	
	TextManager::SubtitleInfo TextManager::getSubtitle(std::string assetName)
	{
		return getSubtitle(assetName, mCurrentLanguage);
	}
	
	bool TextManager::subtitleExists(std::string assetName, Language l)
	{
		if(assetName.size())
		{
			SubtitleMap::const_iterator it = mSubtitles.find(l);
			
			if (it != mSubtitles.end())
			{
				std::map<std::string, SubtitleInfo>::const_iterator stringIt = it->second.find(assetName);
				if (stringIt != it->second.end())
				{
					return true;
				}
			}
		}
		
		return false;
	}
	
	bool TextManager::subtitleExists(std::string assetName)
	{
		return subtitleExists(assetName, mCurrentLanguage);
	}
	

	
	/// <summary>
	/// load in a text script (in tab-separated value format)
	/// </summary>
	/// <param name="filename">tab-separated value format text file to load.</param>
	/// <param name="languagesToLoad">list of languages to load from the file (use null to load all languages in the file into memory)</param>
	void TextManager::loadScriptFromTSV(std::string filename, std::vector<Language>& languagesToLoad)
	{
		mLanguagesToLoad = languagesToLoad;
		
		PropertyList plist;
		FileManager::getInstancePtr()->readFile( filename, CallbackPtr( new Callback(&TextManager::loadedScriptFile) ), plist );
	}
	
	
	void TextManager::loadedScriptFile( void* params ) //const std::string& path, char* buffer, size_t length, const std::string& actualPath, const std::string& handlerName, const PropertyList& userData )
	{
		FileManager::ReadFileCallbackParameters* rcp = (FileManager::ReadFileCallbackParameters*)params;
		
		if(rcp->buffer != NULL)
		{
			// the first line of the file is the HEADERS, indicating the various columns of the file.  we are looking for what language is located at what position in the file.  the "TEXT_ID" is required to be at the start of each line.
			int totalColumns = 0;
			std::map<int, Language> columnLanguageMap;
			
			std::string firstLine;
			//getline(stream, firstLine);
			
			unsigned int i = 0;
			char hc = rcp->buffer[i];
			
			do
			{
				hc = rcp->buffer[i++];
				
				if ((hc == '\n') || (hc == '\r'))
				{
					break;
				}
				else
				{
					firstLine.append( &hc, 1 );
				}
				
			} while ((hc != '\n') && (hc != '\r'));
			
				   
			if(StringHelper::stringContains(firstLine, std::string("\xef\xbb\xbf")))
			{
				firstLine = firstLine.substr(3);
			}
			
			// remove any "\r" at the end of the line...
			firstLine = StringHelper::cleanLineEnding(firstLine);
			
			std::vector<std::string> bits = StringHelper::split(firstLine,'\t');
			
			if (bits[0] != "TEXT_ID")
			{
				//("ERROR!  \"TEXT_ID\" must be first entry in each line!"));
				assert(0);
			}			
			
			int size = bits.size();
			for (int i_ = 1; i_ < size; i_++)
			{
				// this is probably a language, try to get it.
				Language l;
				if (stringToLanguage(bits[i_], l))
				{
					columnLanguageMap[i_] = l;
				}
				else
				{
					columnLanguageMap[i_] = UNRECOGNIZED;
				}
			}
			
			totalColumns = columnLanguageMap.size() + 1;
			
			Logger::printf("Walaber", Logger::SV_DEBUG, "===PARSED COLUMNS===\n");
			std::map<int, Language>::iterator it = columnLanguageMap.begin();
			std::map<int, Language>::iterator end = columnLanguageMap.end();
			for(; it != end; ++it)
			{
				Logger::printf("Walaber", Logger::SV_DEBUG, "column [%d] is %s\n", it->first, languageToString(it->second).c_str());
				Logger::printf("Walaber", Logger::SV_DEBUG, "====================\n");
			}

			// now parse each line, putting the text into the proper dictionary.
			int currentColumn = 0;
			int c;
			std::string sb;
			//StringBuilder sb = new StringBuilder();
			std::string currentID = "ERROR";
			while (i < rcp->length)
			{
				c = rcp->buffer[i++];
				
				switch (c)
				{
					case '\t':
					{
						if (currentColumn == 0)
						{
							currentID = sb;
							while ((currentID[0] == '\n') || (currentID[0] == '\r'))
								currentID = currentID.substr(1);
						}
						else
						{
							if ((mLanguagesToLoad.size() == 0) || (TextManager::containsLanguage(mLanguagesToLoad,columnLanguageMap[currentColumn])))
							{
								// add here.
								std::string newString = sb;
								while ((newString[0] == '\n') || (newString[0] == '\r'))
									newString = newString.substr(1);
								
								//LanguageMap::iterator it = mDictionary.find(columnLanguageMap[currentColumn]);
								//if (it == mDictionary.end()))
								//{
								//	mDictionary[columnLanguageMap[currentColumn]] = new Dictionary<string, string>();
								
								mDictionary[columnLanguageMap[currentColumn]][currentID] = newString;

								Logger::printf("Walaber", Logger::SV_DEBUG, "Inserted \"%s\" into %s at key %s\n", mDictionary[columnLanguageMap[currentColumn]][currentID].c_str(),languageToString(columnLanguageMap[currentColumn]).c_str(),currentID.c_str());
							}
						}
						
						currentColumn++;
						if (currentColumn >= totalColumns)
							currentColumn = 0;
						
						sb.clear();
					}
						break;
						
					case '\r':
						break;
						
					default:
						sb.append(1,(char)c);
						break;                        
				}
				
				if(mLanguagesToLoad.size() == 1)
				{
					mCurrentLanguage = mLanguagesToLoad[0];
				}
			}
			
			delete[] rcp->buffer;
			
			mLanguagesToLoad.clear();
		}
	}
	
	
	
	void TextManager::loadSubtitlesFromTSV(std::string filename, std::vector<Language>& languagesToLoad)
	{
		mLanguagesToLoad = languagesToLoad;
		
		PropertyList plist;
		FileManager::getInstancePtr()->readFile( filename, CallbackPtr( new Callback(&TextManager::loadedSubtitleFile) ), plist );
	}
	
	void TextManager::loadedSubtitleFile( void* params )
	{
		FileManager::ReadFileCallbackParameters* rcp = (FileManager::ReadFileCallbackParameters*)params;
		
		if(rcp->buffer != NULL)
		{
			// the first line of the file is the HEADERS, indicating the various columns of the file.  we are looking for what language is located at what position in the file.  the "TEXT_ID" is required to be at the start of each line.
			int totalColumns = 0;
			std::map<int, Language> columnLanguageMap;
			int characterColumn;
			
			std::string firstLine;
			//getline(stream, firstLine);
			
			unsigned int i = 0;
			char hc = rcp->buffer[i];
			
			do
			{
				hc = rcp->buffer[i++];
				
				if ((hc == '\n') || (hc == '\r'))
				{
					break;
				}
				else
				{
					firstLine.append( &hc, 1 );
				}
				
			} while ((hc != '\n') && (hc != '\r'));
			
			
			if(StringHelper::stringContains(firstLine, std::string("\xef\xbb\xbf")))
			{
				firstLine = firstLine.substr(3);
			}
			
			// remove any "\r" at the end of the line...
			firstLine = StringHelper::cleanLineEnding(firstLine);
			
			std::vector<std::string> bits = StringHelper::split(firstLine,'\t');
			
			if (bits[0] != "SOUND_FILE_REFERENCE")
			{
				//("ERROR!  \"TEXT_ID\" must be first entry in each line!"));
				assert(0);
			}			
			
			int size = bits.size();
			for (int i_ = 1; i_ < size; i_++)
			{
				// this is probably a language, try to get it.
				Language l;
				if (stringToLanguage(bits[i_], l))
				{
					columnLanguageMap[i_] = l;
				}
				else if (StringHelper::toLower(bits[i_]) == "character_name")
				{
					characterColumn = i_;
				}
				else
				{
					columnLanguageMap[i_] = UNRECOGNIZED;
				}
			}
			
			totalColumns = columnLanguageMap.size() + 2;
			
			Logger::printf("Walaber", Logger::SV_DEBUG, "===PARSED COLUMNS===\n");
			std::map<int, Language>::iterator it = columnLanguageMap.begin();
			std::map<int, Language>::iterator end = columnLanguageMap.end();
			for(; it != end; ++it)
			{
				Logger::printf("Walaber", Logger::SV_DEBUG, "column [%d] is %s\n", it->first, languageToString(it->second).c_str());
				Logger::printf("Walaber", Logger::SV_DEBUG, "====================\n");
			}
			
			// now parse each line, putting the text into the proper dictionary.
			int currentColumn = 0;
			int c;
			std::string sb;
			SubtitleInfo si;
			//StringBuilder sb = new StringBuilder();
			std::string currentID = "ERROR";
			while (i < rcp->length)
			{
				c = rcp->buffer[i++];
				
				switch (c)
				{
					case '\t':
						{
							if (currentColumn == 0)
							{
								currentID = sb;
								while ((currentID[0] == '\n') || (currentID[0] == '\r'))
									currentID = currentID.substr(1);
							}
							else if (currentColumn == characterColumn)
							{
								// character!
								si.character = sb;
								while ((si.character[0] == '\n') || (si.character[0] == '\r'))
									si.character = si.character.substr(1);
							}
							else
							{
								if ((mLanguagesToLoad.size() == 0) || (TextManager::containsLanguage(mLanguagesToLoad,columnLanguageMap[currentColumn])))
								{
									// add here.
									si.text = sb;
									while ((si.text[0] == '\n') || (si.text[0] == '\r'))
										si.text = si.text.substr(1);
									
									mSubtitles[columnLanguageMap[currentColumn]][currentID] = si;
									
									Logger::printf("Walaber", Logger::SV_DEBUG, "Got subtitle for %s ['%s'] spoken by %s\n", currentID.c_str(), si.text.c_str(), si.character.c_str());
								}
							}
							
							currentColumn++;
							if (currentColumn >= totalColumns)
								currentColumn = 0;
							
							sb.clear();
						}
						break;
						
					case '\r':
						break;
						
					default:
						sb.append(1,(char)c);
						break;                        
				}
				
				if(mLanguagesToLoad.size() == 1)
				{
					mCurrentLanguage = mLanguagesToLoad[0];
				}
			}
			
			delete[] rcp->buffer;
			
			mLanguagesToLoad.clear();
		}
	}
	
	
	bool TextManager::containsLanguage(const std::vector<Language>& langList, const Language& lang)
	{
		bool ret = false;
		
		int size = langList.size();
		for(int i=0; i<size; ++i)
		{
			if(langList[i] == lang)
			{
				ret = true;
				break;
			}
		}
		
		return ret;
	}
	
	/// <summary>
	/// Converts a string into a Language enum.
	/// </summary>
	/// <param name="p">string</param>
	/// <returns>Language</returns>
	bool TextManager::stringToLanguage(std::string p, Language& lOut)
	{
		bool ret = true;
		
		std::string lower = StringHelper::toLower(p);
		
		
		if( lower == "text_string" )
		{
			lOut = ENGLISH_NTSC;
		}	
		else if (lower == "dialogue")
		{
			lOut = ENGLISH_NTSC;
		}
		else if (( lower == "english_ntsc" ) || ( lower == "english"))
		{
			lOut = ENGLISH_NTSC;
		}	
		else if( lower == "english_pal" )
		{
			lOut = ENGLISH_PAL;
		}	
		else if (( lower == "french_ntsc" ) || ( lower == "french" ))
		{
			lOut = FRENCH_NTSC;
		}	
		else if( lower ==  "french_pal" )
		{
			lOut = FRENCH_PAL;
		}	
		else if( lower ==  "italian" )
		{
			lOut = ITALIAN;
		}	
		else if( lower ==  "german" )
		{
			lOut = GERMAN;
		}
		else if (( lower == "spanish_ntsc" ) || ( lower == "spanish") )
		{
			lOut = SPANISH_NTSC;
		}
		/*else if( (lower == "spanish_pal") || ( lower == "castilianspanish") )
		{
			lOut = CASTILIANSPANISH;
		}*/
		else if ( lower == "american" )
		{
			lOut = AMERICAN;
		}
		else if ( lower == "japanese" )
		{
			lOut = JAPANESE;
		}
        else if ( lower == "korean" )
		{
			lOut = KOREAN;
		}
		else if (( lower == "chinesesimple" ) || ( lower == "chinese - simplified" ))
		{
			lOut = CHINESE_SIMPLE;
		}
		else if ((lower == "chinesetraditional") || ( lower == "chinese - traditional" ))
		{
			lOut = CHINESE_TRADITIONAL;
		}
		else if (lower == "russian")
		{
			lOut = RUSSIAN;
		}
		/*else if (lower == "dutch")
		{
			lOut = DUTCH;
		}
        else if (lower == "czech")
		{
			lOut = CZECH;
		}
        else if (lower == "polish")
		{
			lOut = POLISH;
		}*/
        else if (lower =="brazilianportuguese" || lower == "portuguese_brazilian")
        {
            lOut = PORTUGUESE_BRAZILIAN;
        }
		else 
		{
			lOut  = UNRECOGNIZED;
			ret = false;
		}
		
		return ret;
	}
	
	Language TextManager::CountryInfoToLanguage(std::string country)
	{
		std::string lower = StringHelper::toLower(country);
		
		Language ret = ENGLISH_NTSC;
		
		if( lower == "es" )
		{
			ret = SPANISH_NTSC;
		}	
        /*else if( lower == "es-sp")
        {
            ret = CASTILIANSPANISH;
        }*/
		else if( lower == "fr" )
		{
			ret = FRENCH_NTSC;
		}	
		else if( lower == "de" )
		{
			ret = GERMAN;
		}	
		else if( lower == "it" )
		{
			ret = ITALIAN;
		}
		else if (lower == "ja" )
		{
			ret = JAPANESE;
		}
        else if (lower == "ko" )
		{
			ret = KOREAN;
		}
		else if (lower == "zh-hans")
		{
			ret = CHINESE_SIMPLE;
		}
		else if (lower == "zh-hant")
		{
			ret = CHINESE_TRADITIONAL;
		}
		else if (lower == "ru")
		{
			ret = RUSSIAN;
		}
		/*else if (lower =="nl")
		{
			ret = DUTCH;
		}
        else if (lower =="cs")
		{
			ret = CZECH;
		}
        else if (lower =="pl")
		{
			ret = POLISH;
		}*/
        else if (lower =="pt")
        {
            ret = PORTUGUESE_BRAZILIAN;
        }
		
		return ret;		
	}
    
    std::string TextManager::getISOCodeForCurrentLanguage()
    {
        return getISOCodeForLanguage(mCurrentLanguage);
    }
                                                   
    std::string TextManager::getISOCodeForLanguage(const Language lang)
    {
        std::string ret;        
            
        switch (lang)
		{
			case ENGLISH_NTSC:
				ret = "en";
				break;
			case ENGLISH_PAL:
				ret = "en";
				break;
			case FRENCH_NTSC:
				ret = "fr";
				break;
			case FRENCH_PAL:
				ret = "fr";
				break;
			case ITALIAN:
				ret = "it";
				break;
			case GERMAN:
				ret = "de";
				break;
			case SPANISH_NTSC:
				ret = "es";
				break;
			/*case CASTILIANSPANISH:
				ret = "es-sp";
				break;*/
			case AMERICAN:
				ret = "en";
				break;
			case JAPANESE:
				ret = "ja";
				break;
            case KOREAN:
				ret = "ko";
				break;
			case CHINESE_SIMPLE:
				ret = "zh-Hans";
				break;
			case CHINESE_TRADITIONAL:
				ret = "zh-Hant";
				break;
			case RUSSIAN:
				ret = "ru";
				break;
			/*case DUTCH:
				ret = "nl";
				break;
            case CZECH:
                ret = "cs";
                break;
            case POLISH:
                ret = "pl";
                break;*/
            case PORTUGUESE_BRAZILIAN:
                ret = "pt";
                break;
			default:
				ret = "en";
				break;
		}
        
        return ret;
    }
    
    std::string TextManager::getISOLanguageCountryCodeForCurrentLanguage()
    {
        return getISOLanguageCountryCodeForLanguage(mCurrentLanguage);
    }
    
    std::string TextManager::getISOLanguageCountryCodeForLanguage(const Language lang)
    {
        std::string ret;        
        
        switch (lang)
		{
			case ENGLISH_NTSC:
				ret = "en_US";
				break;
			case ENGLISH_PAL:
				ret = "en_US";
				break;
			case FRENCH_NTSC:
				ret = "fr_FR";
				break;
			case FRENCH_PAL:
				ret = "fr_FR";
				break;
			case ITALIAN:
				ret = "it_IT";
				break;
			case GERMAN:
				ret = "de_DE";
				break;
			case SPANISH_NTSC:
				ret = "es_ES";
				break;
			/*case CASTILIANSPANISH:
				ret = "es_ES";
				break;*/
			case AMERICAN:
				ret = "en_US";
				break;
			case JAPANESE:
				ret = "ja_JP";
				break;
            case KOREAN:
				ret = "ko_KR";
				break;
			case CHINESE_SIMPLE:
				ret = "zh_CN";
				break;
			case CHINESE_TRADITIONAL:
				ret = "zh_TW";
				break;
			case RUSSIAN:
				ret = "ru_RU";
				break;
			/*case DUTCH:
				ret = "nl_NL";
				break;
            case CZECH:
                ret = "cs_CZ";
                break;
            case POLISH:
                ret = "pl_PL";
                break;*/
            case PORTUGUESE_BRAZILIAN:
                ret = "pt_BR";
                break;
			default:
				ret = "en-US";
				break;
		}
        
        return ret;
    }
	
	/// <summary>
	/// Converts a Language enum into a string.
	/// </summary>
	/// <param name="l">Language</param>
	/// <returns>language name string.</returns>
	std::string TextManager::languageToString(Language l)
	{
		std::string ret;
		
		switch (l)
		{
			case ENGLISH_NTSC:
				ret = "ENGLISH_NTSC";
				break;
			case ENGLISH_PAL:
				ret = "ENGLISH_PAL";
				break;
			case FRENCH_NTSC:
				ret = "FRENCH_NTSC";
				break;
			case FRENCH_PAL:
				ret = "FRENCH_PAL";
				break;
			case ITALIAN:
				ret = "ITALIAN";
				break;
			case GERMAN:
				ret = "GERMAN";
				break;
			case SPANISH_NTSC:
				ret = "SPANISH_NTSC";
				break;
			/*case CASTILIANSPANISH:
				ret = "CASTILIANSPANISH";
				break;*/
			case AMERICAN:
				ret = "AMERICAN";
				break;
			case JAPANESE:
				ret = "JAPANESE";
				break;
            case KOREAN:
				ret = "KOREAN";
				break;
			case CHINESE_SIMPLE:
				ret = "CHINESE - SIMPLIFIED";
				break;
			case CHINESE_TRADITIONAL:
				ret = "CHINESE - TRADITIONAL";
				break;
			case RUSSIAN:
				ret = "RUSSIAN";
				break;
			/*case DUTCH:
				ret = "DUTCH";
				break;
            case CZECH:
                ret = "CZECH";
                break;
            case POLISH:
                ret = "POLISH";
                break;*/
            case PORTUGUESE_BRAZILIAN:
                ret = "PORTUGUESE_BRAZILIAN";
                break;
			default:
				ret = "UNKNOWN LANGUAGE";
				break;
		}
		
		return ret;
	}
	
	/// <summary>
	/// get a list of available languages as an array of Language enums.
	/// </summary>
	/// <returns></returns>
	void TextManager::getAvailableLanguages(std::vector<Language>& lOut)
	{
		LanguageMap::iterator end = mDictionary.end();
		for(LanguageMap::iterator it = mDictionary.begin(); it != end; ++it)
		{
			lOut.push_back(it->first);
		}		
	}
	
	/// <summary>
	/// get available languages as an array of strings.  Each entry is the language name.
	/// </summary>
	/// <returns></returns>
	void TextManager::getAvailableLanguagesAsStrings(std::vector<std::string> lOut)
	{
		LanguageMap::iterator end = mDictionary.end();
		for(LanguageMap::iterator it = mDictionary.begin(); it != end; ++it)
		{
			lOut.push_back(languageToString(it->first));
		}	
	}	
	
	
	const std::map< std::string, std::string >* TextManager::getLanguageMap( Language l )	
	{
		if (mDictionary.find( l ) != mDictionary.end())
			return &mDictionary[ l ];
		
		return NULL;
	}
	
	const std::map< std::string, TextManager::SubtitleInfo >* TextManager::getSubtitleMap( Language l )
	{
		if (mSubtitles.find( l ) != mSubtitles.end())
			return &mSubtitles[ l ];
		
		return NULL;
	}
	
}
