//
//  DrawableNode.h
//  WalaberEngine
//
//  Created by Jason Pecho on 5/27/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef _DRAWABLE_NODE_H
#define _DRAWABLE_NODE_H

#include <Node.h>

namespace Walaber
{
    class SpriteBatch;
    
    class DrawableNode : public Node
    {
    public:
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Private default ctor for no use
        // </Summary>
        DrawableNode();
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Private default ctor for no use
        // </Summary>
        DrawableNode(const std::string& name, const int groupId = -1);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Private default ctor for no use
        // </Summary>
        DrawableNode(const DrawableNode& drawableNode);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Private default ctor for no use
        // </Summary>
        DrawableNode& operator=(const DrawableNode& drawableNode);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Private default ctor for no use
        // </Summary>
        virtual ~DrawableNode();
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Private default ctor for no use
        // </Summary>
        virtual void setLayer(const int layer) { mLayer = layer; }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Private default ctor for no use
        // </Summary>
        int getLayer() { return mLayer; }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Private default ctor for no use
        // </Summary>
        virtual void setVisible(const bool visible) { mVisible = visible; }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Private default ctor for no use
        // </Summary>
        bool getVisible() { return mVisible; }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // draw the node
        // </Summary>
        virtual void draw(SpriteBatch* sb) = 0;
        
    protected:
        
        int         mLayer;
        bool        mVisible;
    };
}

#endif