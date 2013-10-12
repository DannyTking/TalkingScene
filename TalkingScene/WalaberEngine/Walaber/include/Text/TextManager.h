/*
 *  TextManager.h
 *
 *  Created by jpecho on 11/17/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _TEXT_MANAGER_H_
#define _TEXT_MANAGER_H_

#include <string>
#include <map>
#include <vector>

#include "PropertyList.h"

namespace Walaber
{
	/**
     *Enums used to represent different languages Walaber Engine supports.
     */
	enum Language 
	{ 
		ENGLISH_NTSC,               
		ENGLISH_PAL,
		FRENCH_NTSC,
		FRENCH_PAL,
		ITALIAN,
		GERMAN,
		SPANISH_NTSC,
		AMERICAN,
		JAPANESE,
        KOREAN,
		CHINESE_SIMPLE,
		CHINESE_TRADITIONAL,
		RUSSIAN,
        PORTUGUESE_BRAZILIAN,
		UNRECOGNIZED
	};	
	
	class TextManager
	{
	public:
		//typedef
		struct SubtitleInfo
		{
			std::string		text;
			std::string		character;
		};
		
		typedef std::map< Language, std::map<std::string,std::string> >		LanguageMap;
		typedef std::map< Language, std::map<std::string, SubtitleInfo > >	SubtitleMap;
		static void clearAll();
		
		// standard in-game text
		static std::string getString(std::string name, Language l);
		static std::string getString(std::string name);
		
		static bool stringExists(std::string name, Language l);
		static bool stringExists(std::string name);
		
		// subtitle text
		static SubtitleInfo getSubtitle(std::string assetName, Language l);
		static SubtitleInfo getSubtitle(std::string assetName);
		
		static bool subtitleExists(std::string assetName, Language l);
		static bool subtitleExists(std::string assetName);
		
		
		static void setCurrentLanguage( Language l ) { mCurrentLanguage = l; }
		static Language getCurrentLanguage() { return mCurrentLanguage; }
		
		
		// append the contents of this file into the string list.
		static void loadScriptFromTSV(std::string filename, std::vector<Language>& languagesToLoad);
		static void loadedScriptFile( void* params );
		
		static void loadSubtitlesFromTSV(std::string filename, std::vector<Language>& languagesToLoad);
		static void loadedSubtitleFile( void* params );
		
		static void clearAllLanguageStrings() { mDictionary.clear(); }
		static void clearAllSubtitleStrings() { mSubtitles.clear(); }
		
        /**
         * Converts a string into a Language enum.
         * @param p A string specify which language it is.
         * @param lOut A enum #Language which will be returned.
         */
		static bool stringToLanguage(std::string p, Language& lOut);
		
        /**
         * Converts an abbreviation identifier of each language to the enum #Language.
         * @param country The abbr. identifier of some language.
         * @return enum #Language.
         */
		static Language CountryInfoToLanguage(std::string country);
        
        /**
         * Return ISO code for current language. Here uses ISO 639-1 Code.
         * @return A string contains the ISO code of the current language.
         * @see getISOCodeForLanguage(const Language).
         */
        static std::string getISOCodeForCurrentLanguage();
        
        /**
         * Return ISO code for input #Language. Here uses ISO 639-1 Code.
         * @param lang A #Language enum.
         * @return A string contains the ISO code.
         * @see getISOCodeForCurrentLanguage().
         */
        static std::string getISOCodeForLanguage(const Language lang);
		
        /**
         * Return ISO language code and ISO contry code for current language. The return value is in format of [language code]_[contry code]. For example, en_US.
         * @return A string contains the language code and contry code.
         * @see getISOLanguageCountryCodeForCurrentLanguage(const Language).
         */
        static std::string getISOLanguageCountryCodeForCurrentLanguage();
        
        /**
         * Return ISO language code and ISO contry code for input language. The return value is in format of [language code]_[contry code]. For example, en_US.
         * @param lang A #Language enum.
         * @return A string contains the language code and contry code.
         * @see getISOLanguageCountryCodeForCurrentLanguage().
         */
        static std::string getISOLanguageCountryCodeForLanguage(const Language lang);
        
        /**
         * Converts a #Language enum into a string.
         */
		static std::string languageToString(Language l);
		
        /**
         * Get a list of available languages as an array of #Language enum.
         */
		static void getAvailableLanguages(std::vector<Language>& lOut);
		
        /**
         * Get available languages as an array of strings.  Each entry is the language name.
         */
		static void getAvailableLanguagesAsStrings(std::vector<std::string> lOut);		
		
		static const std::map< std::string, std::string >* getLanguageMap( Language l );
		static const std::map< std::string, SubtitleInfo >* getSubtitleMap( Language l );
		
	protected:
		
		static bool containsLanguage(const std::vector<Language>& langList, const Language& lang);
		
		static LanguageMap		    mDictionary;
		static SubtitleMap			mSubtitles;
		
		static Language				mCurrentLanguage;
		
		static std::vector<Language>	mLanguagesToLoad;
	};
	
}

#endif
