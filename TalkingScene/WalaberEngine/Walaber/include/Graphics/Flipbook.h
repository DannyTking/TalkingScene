//
//  Flipbook.h
//  WalaberEngine
//
//  Created by Jason Pecho on 5/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef _FLIPBOOK_H_
#define _FLIPBOOK_H_

#include <string>
#include <vector>

#include <TextureManager.h>
#include <Rect.h>

namespace Walaber
{
    // -=-=-=-=-=-==-=-=-=-==-=-=-=-=-=-===-=-=-=-=-=-=-==-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // -=-=-=-=-=-==-=-=-=-==-=-=-=-=-=-===-=-=-=-=-=-=-==-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    
    class Flipbook
    {
    public:// interface
        // -=-=-=-=-=-==-=-=-=-==-=-=-=-=-=-===-=-=-=-=-=-=-==-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        Flipbook(const std::string& filename);
        
        // -=-=-=-=-=-==-=-=-=-==-=-=-=-=-=-===-=-=-=-=-=-=-==-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        void update(float elapsedSec);
        
        // -=-=-=-=-=-==-=-=-=-==-=-=-=-=-=-===-=-=-=-=-=-=-==-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        TexturePtr getTexture() { return mTexture;}
        
        // -=-=-=-=-=-==-=-=-=-==-=-=-=-=-=-===-=-=-=-=-=-=-==-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        Rect getUVRect() { return mFrames[mCurrentFrame].uvs; }
        
    protected:
        // -=-=-=-=-=-==-=-=-=-==-=-=-=-=-=-===-=-=-=-=-=-=-==-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        struct FlipbookFrame
        {
            float duration;
            Rect uvs;
        };
        
        // -=-=-=-=-=-==-=-=-=-==-=-=-=-=-=-===-=-=-=-=-=-=-==-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
        TexturePtr mTexture;
        std::vector< FlipbookFrame > mFrames;
        int mCurrentFrame;
        float mFrameDurationLeft;
    };
}

#endif // _FLIPBOOK_H_