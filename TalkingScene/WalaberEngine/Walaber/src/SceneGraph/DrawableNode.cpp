//
//  DrawableNode.cpp
//  WalaberEngine
//
//  Created by Jason Pecho on 5/27/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#include "DrawableNode.h"

namespace Walaber
{
    // -------------==============-------------==============-------------==============-------------===
    DrawableNode::DrawableNode():
    Node("defaultNode"),
    mLayer(0),
    mVisible( true )
    {}
    
    // -------------==============-------------==============-------------==============-------------===
    // <Summary>
    // Private default ctor for no use
    // </Summary>
    DrawableNode::DrawableNode(const std::string& name, const int groupId):
    Node(name, groupId),
    mLayer(0),
    mVisible( true )
    {
        
    }
    
    
    // -------------==============-------------==============-------------==============-------------===
    // <Summary>
    // Private default ctor for no use
    // </Summary>
    DrawableNode::DrawableNode(const DrawableNode& drawableNode):
    Node(drawableNode),
    mLayer(drawableNode.mLayer),
    mVisible(drawableNode.mVisible)
    {}
    
    // -------------==============-------------==============-------------==============-------------===
    // <Summary>
    // Private default ctor for no use
    // </Summary>
    DrawableNode& DrawableNode::operator=(const DrawableNode& drawableNode)
    {
        Node::operator=(drawableNode);
        
        mLayer = drawableNode.mLayer;
        mVisible = drawableNode.mVisible;
        
        return *this;
    }
    
    // -------------==============-------------==============-------------==============-------------===
    // <Summary>
    // Private default ctor for no use
    // </Summary>
    DrawableNode::~DrawableNode()
    {
        
    }
}