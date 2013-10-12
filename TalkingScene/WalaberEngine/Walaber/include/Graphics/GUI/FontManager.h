/*
 *  FontManager.h
 *  JellyCar3
 *
 *  Created by jpecho on 12/3/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _FONT_MANAGER_H_
#define _FONT_MANAGER_H_

#include <string>
#include <map>

namespace Walaber
{
	class BitmapFont;
	
	class FontManager
	{
	public:		
		//-----------------------------------------------------------------------~~-------
		static FontManager* getInstancePtr();
		
		//-----------------------------------------------------------------------~~-------
		BitmapFont* getFont(std::string name);
		
		//-----------------------------------------------------------------------~~-------
		void addFont(std::string name, std::string fontFile);
		
		//-----------------------------------------------------------------------~~-------
		void setImageBasePath(std::string basepath) { mImageBasePath = basepath; }
		
		//-----------------------------------------------------------------------~~-------
		void removeAllFonts();
		
    public: //callbacks
        
        //-----------------------------------------------------------------------~~-------
		void _fontLoaded( void* data );
        
	protected:		
		//-----------------------------------------------------------------------~~-------
		FontManager(){};
		explicit FontManager(const FontManager& fontMgr){};
		FontManager& operator=(const FontManager& fontMgr){return *this;}
		
		//-----------------------------------------------------------------------~~-------
		~FontManager()
		{
			// free bitmap font pointers
			removeAllFonts();
		}
		
		//-----------------------------------------------------------------------~~-------
		static FontManager						*instance;
		std::string								mImageBasePath;
		std::map<std::string, BitmapFont*>		mFontMap;
	};
}
#endif
