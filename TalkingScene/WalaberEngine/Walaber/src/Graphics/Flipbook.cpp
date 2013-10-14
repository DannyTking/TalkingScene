//
//  Flipbook.cpp
//  WalaberEngine
//
//  Created by Jason Pecho on 5/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#include "Flipbook.h"

#include <XMLHelper.h>

namespace Walaber 
{
    // -=-=-=-=-=-==-=-=-=-==-=-=-=-=-=-===-=-=-=-=-=-=-==-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    Flipbook::Flipbook(const std::string& filename):
    mCurrentFrame(0),
    mFrameDurationLeft(0.0f)
    {
		// parse the XML document.
		xmlDocPtr doc = NULL;
		xmlNodePtr root_element = XML::loadDoc( filename, "Flipbook", doc );
		
		if (root_element == NULL)
			return;
		
		float defaultDur = 0.1f;
		if (XML::attrExists(root_element, "defaultDuration"))
			defaultDur = XML::parseFloat(root_element, "defaultDuration");
			
		Vector2 tiling = XML::parseVector2(root_element, "tiling");
		int columns = (int)tiling.X;
		int rows = (int)tiling.Y;
		
		Vector2 uvSize( 1.0f / columns, 1.0f / rows );
		
		mTexture = Walaber::TextureManager::getManager().getTexture( XML::parseString(root_element, "texture") );


		for (xmlNodePtr frame = root_element->children; frame; frame = frame->next)
		{
			if (xmlStrcmp(frame->name, (const xmlChar*)"Frame") == 0)
			{
				FlipbookFrame f;
				
				int frameID = XML::parseInt(frame, "img");

				int frameX = 0;
				int frameY = 0;
				
				while ((frameID - (frameY * columns) >= columns))
				{
					frameY++;
				}
				
				frameX = frameID - (frameY * columns);
				
				Logger::printf("Walaber", Logger::SV_DEBUG, "id[%d] maps to [%d][%d]\n", frameID, frameX, frameY);
				
				f.uvs = Rect( uvSize * Vector2(frameX,frameY), uvSize );				
				
				if (XML::attrExists(frame, "duration"))
					f.duration = XML::parseFloat(frame, "duration");
				else
					f.duration = defaultDur;
				
				mFrames.push_back(f);
			}
        }        
    }
    
    void Flipbook::update( float elapsedSec )
    {
        while (elapsedSec > mFrameDurationLeft)
        {
            elapsedSec -= mFrameDurationLeft;
            mCurrentFrame++;
            if (mCurrentFrame >= mFrames.size()) { mCurrentFrame = 0; }
            mFrameDurationLeft = mFrames[mCurrentFrame].duration;
        }
        
        mFrameDurationLeft -= elapsedSec;
    }
}