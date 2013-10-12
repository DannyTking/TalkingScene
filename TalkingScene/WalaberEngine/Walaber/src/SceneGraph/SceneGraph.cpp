//
//  SceneGraph.cpp
//  WalaberEngine
//
//  Created by Jason Pecho on 4/26/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "SceneGraph.h"

#include <Node.h>
#include <Skeleton.h>

namespace Walaber
{    
    // -------------==============-------------==============-------------==============-------------===
    SceneGraph::SceneGraph():
    mRoot(new Node("Root"))
    {
        setName("Scene_Graph");
    }
    
    // -------------==============-------------==============-------------==============-------------===
    SceneGraph::SceneGraph(const std::string& name):
    mRoot(new Node("Root"))
    {     
        setName(name);
    }
    
    // -------------==============-------------==============-------------==============-------------===
    SceneGraph::SceneGraph(const SceneGraph& sceneGraph)
    {
        // deep copy
    }
    
    // -------------==============-------------==============-------------==============-------------===
    SceneGraph& SceneGraph::operator=(const SceneGraph& sceneGraph)
    {
        // deep copy
        
        return *this;
    }
    
    // -------------==============-------------==============-------------==============-------------===
    SceneGraph::~SceneGraph()
    {
        PCSNode::deleteNodeAndChildrenFromTree(mRoot);
    }
    
    // -------------==============-------------==============-------------==============-------------===
    Node* SceneGraph::getNodeByName(const std::string& name)
    {
        return static_cast<Node*>(PCSNode::getNodeFromTree(mRoot, name));
    }
    
    // -------------==============-------------==============-------------==============-------------===
    Node* SceneGraph::getNodeByNameAndGroupID(const std::string& name, const int groupID)
    {
        return static_cast<Node*>(PCSNode::getNodeFromTree(mRoot, name, groupID));
    }
    
    // -------------==============-------------==============-------------==============-------------===
    Node* SceneGraph::getFirstNodeWithGroupID(const int groupID)
    {
        return static_cast<Node*>(PCSNode::getNodeFromTree(mRoot, groupID));
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void SceneGraph::getNodesWithGroupID(const int groupID, std::vector<Node*>& outVec)
    {
        PCSNode::getNodesFromTree(mRoot,groupID, outVec);
    }
}