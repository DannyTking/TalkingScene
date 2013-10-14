/*
 *  Subtexture.h
 *  JellyCar3
 *
 *  Created by jpecho on 12/9/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _SUBTEXTURE_H_
#define _SUBTEXTURE_H_

#include "Texture.h"
#include <cassert>

namespace Walaber
{	
	class Subtexture: public Texture
	{
	public:
		Subtexture(std::string fileName, TexturePtr parent, const TextureData& texData):
		Texture(fileName, TT_Subtexture),
		mParent(parent),
		mParentPath(parent->getFileName())
		{
			initWithTextureData(texData);
			name_ = parent->getName();
		};
		
		~Subtexture(){};
		
		buffer_type_ptr getData(void)
		{
            //FUNCTION_NOT_SUPPORTED_IN_SUBTEXTURE_EXCEPTION();
			assert(0);
		}
			
		void setData(buffer_type_ptr data)
		{
			//FUNCTION_NOT_SUPPORTED_IN_SUBTEXTURE_EXCEPTION();
			assert(0);
		}
		
		void writeToFile(std::string filePath)
		{
			//FUNCTION_NOT_SUPPORTED_IN_SUBTEXTURE_EXCEPTION();
			assert(0);
		}
		
		bool isParentNull()
		{
			if (mParent.use_count() == 0)
				return true;	
			
			return false;
		}
		
		bool parentNotLoaded()
		{
			if (mParent->getName() == NO_TEXTURE)
				return true;
			
			return false;
		}
		
		void nullifyParent()
		{
			mParent.reset();
			name_ = NO_TEXTURE;
		}
        
        bool isTextureParent(const std::string& possibleParent)
        {
            return possibleParent == mParentPath;
        }
		
		void reloadParent(CallbackPtr callback = CallbackPtr());
		
		std::string getParentPath() const { return mParentPath; }
		
	protected:
		void initWithTextureData(const TextureData& texData)
		{
			this->size_= Vector2(texData.width, texData.height);
			this->width_ = texData.width;
			this->height_ = texData.height;
			this->maxS_ = texData.maxS;
			this->maxT_ = texData.maxT;
			this->minS_ = texData.minS;
			this->minT_ = texData.minT;
            this->offset_ = texData.offset;
            this->flags_ = texData.flags;
		}
		
		TexturePtr mParent; // hold onto parent to keep reference	
		std::string mParentPath; // hold onto parent path in case there's a need for reloading
	};	
	
	MAKE_SHARED_PTR(Subtexture);
	
	SubtexturePtr createSubtexture(std::string fileName, TexturePtr parent, const TextureData& texData);
}
#endif
