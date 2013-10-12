/*
 *  FontManager.cpp
 *  JellyCar3
 *
 *  Created by jpecho on 12/3/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#include "FontManager.h"
#include "BitmapFont.h"
#include <TextureManager.h>
#include <MemberCallback.h>

namespace Walaber
{
	//-----------------------------------------------------------------------~~-------
	//-----------------------------------------------------------------------~~-------
	FontManager* FontManager::instance=0;

	//-----------------------------------------------------------------------~~-------
	//-----------------------------------------------------------------------~~-------
	FontManager* FontManager::getInstancePtr()
	{
		if(!instance)
		{
			instance = new FontManager();
		}
		return instance;
	}

	//-----------------------------------------------------------------------~~-------
	//-----------------------------------------------------------------------~~-------
	BitmapFont* FontManager::getFont(std::string name)
	{
		BitmapFont* font = NULL;
		
		std::map<std::string,BitmapFont*>::iterator it = mFontMap.find(name);
		if(it != mFontMap.end())
		{
			font = it->second;
		}
		
		return font;
	}

	//-----------------------------------------------------------------------~~-------
	//-----------------------------------------------------------------------~~-------
	void FontManager::addFont(std::string name, std::string fontFile)
	{
		// create the font and setup callback...
		BitmapFont* fnt = new BitmapFont();
		
		// add to map right now...
		mFontMap[ name ] = fnt;
		
		// load it, and then notify textures once it's loaded...
        MemberCallbackPtr<FontManager>::type memcallback(new MemberCallback<FontManager>(this, &FontManager::_fontLoaded));
        CallbackPtr callback = static_pointer_cast<Callback>(memcallback);
        
		fnt->load( fontFile, callback );
		
	}
	
	//-----------------------------------------------------------------------~~-------
	//-----------------------------------------------------------------------~~-------
	void FontManager::_fontLoaded( void* data )
	{
        BitmapFont::BitmapFontCallbackParameters* params = (BitmapFont::BitmapFontCallbackParameters*)data;
		
		if (params->result == ResultCode_OK)
		{
			BitmapFont* font = params->bitmapFont;
			
			
			// ----------------------------------------------------------------
			// load font textures
			int numPages = font->getNumTexturePages();
			
			std::string filepath;
			Walaber::TextureManager& texMan = Walaber::TextureManager::getManager();
			
			for(int i = 0; i < numPages; ++i)
			{
				filepath = mImageBasePath + font->getTextureFilename(i);

				Logger::printf("Walaber", Logger::SV_DEBUG, "BitmapFont texture Filename: [%s]\n", filepath.c_str());

				font->notifyTexture(texMan.getTexture(filepath));
			}
		}
	}
	
	//-----------------------------------------------------------------------~~-------
	//-----------------------------------------------------------------------~~-------
	void FontManager::removeAllFonts()
	{
		for (std::map<std::string, BitmapFont*>::iterator it = mFontMap.begin(); it != mFontMap.end(); it++)
		{
			// delete the font!
			BitmapFont* font = it->second;
			delete font;
		}
		
		mFontMap.clear();
	}
}
