/*
 *  Subtexture.cpp
 *  JellyCar3
 *
 *  Created by jpecho on 12/14/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#include "Subtexture.h"
#include "Texture2D.h"
#include <TextureManager.h>

namespace Walaber 
{
	void Subtexture::reloadParent(CallbackPtr callback)
	{
		this->mParent = TextureManager::getManager().getTexture(mParentPath, callback);

		name_ = mParent->getName();
	}
	
	SubtexturePtr createSubtexture(std::string fileName, TexturePtr parent, const TextureData& texData)
	{
		SubtexturePtr newTexture(new Subtexture(fileName, parent, texData));
		return newTexture;
	}
	
}

