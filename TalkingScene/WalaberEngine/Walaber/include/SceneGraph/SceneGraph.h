//
//  SceneGraph.h
//  WalaberEngine
//
//  Created by Jason Pecho on 4/26/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef _SCENE_GRAPH_H_
#define _SCENE_GRAPH_H_

#include <string>
#include <vector>
#include <map>
#include <cassert>

#define SCENE_GRAPH_NAME_LENGTH   28

namespace Walaber
{
    class Node;
    class Skeleton;
    
    class  SceneGraph
    { 
    public://interface
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Default Ctor
        // </Summary>
        SceneGraph();
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Default Ctor
        // </Summary>
        SceneGraph(const std::string& name);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Copy ctor
        // </Summary>
        SceneGraph(const SceneGraph& sceneGraph);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // op =
        // </Summary>
        SceneGraph& operator=(const SceneGraph& sceneGraph);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Dtor
        // </Summary>
        ~SceneGraph();
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Get the root node -- to insert, add children to root node
        // </Summary>
        Node* getRootNode()
        {
            return mRoot;
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Get the root node
        // </Summary>
        void setName(const std::string& name)
        {
            assert(name.size() < SCENE_GRAPH_NAME_LENGTH);
            strncpy(mName,name.c_str(),SCENE_GRAPH_NAME_LENGTH-1);
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Get the root node
        // </Summary>
        const char* getName()const
        {
            return mName;
        }        
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // returns a node ptr to a node named name, null if not found
        // </Summary>
        Node* getNodeByName(const std::string& name);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // returnsthe first node ptr to a node named name in groupID, null if not found
        // </Summary>
        Node* getNodeByNameAndGroupID(const std::string& name, const int groupID);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // returns a node ptr to the first node in found in groupID
        // </Summary>
        Node* getFirstNodeWithGroupID(const int groupID);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // returns all nodes marked with groupID
        // </Summary>
        void getNodesWithGroupID(const int groupID, std::vector<Node*>& outVec);         
        
    private://members
        Node*                   mRoot; 
        char                    mName[SCENE_GRAPH_NAME_LENGTH];
        
    };
}

#endif