//
//  PCSNode.cpp
//  WalaberEngine
//
//  Created by Jason Pecho on 5/4/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "PCSNode.h"

#include <queue>

namespace Walaber
{
    // -------------==============-------------==============-------------==============-------------===
    PCSNode* PCSNode::removeNodeFromTree(PCSNode* root, const std::string& name)
    {
        PCSNode* node = PCSNode::getNodeFromTree(root, name);
        
        if(node)
        {
            if(node->mParent)
            {
                node->mParent->removeChild(node);
            }
        }
        
        return node;
    }
    
    // -------------==============-------------==============-------------==============-------------===
    PCSNode* PCSNode::removeNodeFromTree(PCSNode* root, const std::string& name, const int groupID)
    {
        PCSNode* node = PCSNode::getNodeFromTree(root, name, groupID);
        
        if(node)
        {
            if(node->mParent)
            {
                node->mParent->removeChild(node);
            }
        }
        
        return node;
    }
    
    // -------------==============-------------==============-------------==============-------------===
    PCSNode* PCSNode::getNodeFromTree(PCSNode* root, const std::string& name)
    {
        return PCSNode::_depthFirstNameSearch(root, name);
    }
    
    // -------------==============-------------==============-------------==============-------------===
    PCSNode* PCSNode::getNodeFromTree(PCSNode* root, const std::string& name, const int groupID)
    {
        return PCSNode::_depthFirstNameSearchWithGroupID(root, name, groupID);
    }
    
    // -------------==============-------------==============-------------==============-------------===
    PCSNode* PCSNode::getNodeFromTree(PCSNode* root, const int groupID)
    {
        return _depthFirstGroupIDSearch(root, groupID);
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void PCSNode::getNodesFromTree(PCSNode* root, const int groupID, std::vector<PCSNode*>& outPCSNodeList)
    {
        _depthFirstGroupIDSearchMultipleResults(root, groupID, outPCSNodeList);
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void PCSNode::deleteNodeAndChildrenFromTree(PCSNode* node)
    {
        if(node->mParent)
        {
            node->mParent->removeChild(node);
        }
        
		if (node->mChild)
		{
			PCSNode::_recursiveDelete(node->mChild);
		}
		
		delete node;
		node=0;
    }
    
	// -------------==============-------------==============-------------==============-------------===
    unsigned int PCSNode::getTreeSize(PCSNode* root)
    {
        std::queue<PCSNode*> nodeQueue;
        nodeQueue.push(root);
		unsigned int num = 0;
		
        while(!nodeQueue.empty())
        {
            PCSNode* node = nodeQueue.front();
            nodeQueue.pop();
            
            ++num;			
            
            PCSNode* child = node->mChild;
            while(child)
            {
                nodeQueue.push(child);
                child = child->mSibling;
            }
        }
		
		return num;
    }
	
    // -------------==============-------------==============-------------==============-------------===
    void PCSNode::printTree(PCSNode* root)
    {
        std::queue<PCSNode*> nodeQueue;
        nodeQueue.push(root);
        
        while(!nodeQueue.empty())
        {
            PCSNode* node = nodeQueue.front();
            nodeQueue.pop();
            
            node->printName();
            Logger::printf("Walaber", Logger::SV_INFO, " Children: ");
            
            PCSNode* child = node->mChild;
            while(child)
            {
                child->printName();
                Logger::printf("Walaber", Logger::SV_INFO, ", ");
                nodeQueue.push(child);
                child = child->mSibling;
            }
            
            Logger::printf("Walaber", Logger::SV_INFO, "\n");
        } 
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void  PCSNode::_recursiveDelete(PCSNode* node)
    {
        if(node->mChild)
        {
            _recursiveDelete(node->mChild);
        }
        
        if(node->mSibling)
        {
            _recursiveDelete(node->mSibling);
        }
        
        delete node;
		node = 0;
    }
    
    // -------------==============-------------==============-------------==============-------------===
    PCSNode*  PCSNode::_depthFirstNameSearch(PCSNode* root, const std::string& name)
    {
        PCSNode* node = 0;
        const char* searchName = name.c_str();
        std::stack<PCSNode*> nodeStack;
        nodeStack.push(root);
        
        while(!nodeStack.empty())
        {
            node = nodeStack.top();
            nodeStack.pop();
            
            if(!strncmp(node->getName(), searchName, mini(maxi(name.size(), strlen(node->getName())),NODE_NAME_LENGTH-1)))
            {
                return node;
            }
            else
            {      
                if(node->mSibling)
                {
                    nodeStack.push(node->mSibling);
                }
                
                if(node->mChild)
                {
                    nodeStack.push(node->mChild);
                }     
                
                node = 0;
            }            
        }
        return node;
    }
    
    // -------------==============-------------==============-------------==============-------------===
    PCSNode*  PCSNode::_depthFirstNameSearchWithGroupID(PCSNode* root, const std::string& name, const int groupID)
    {
        PCSNode* node = 0;
        const char* searchName = name.c_str();
        std::stack<PCSNode*> nodeStack;
        nodeStack.push(root);
        
        while(!nodeStack.empty())
        {
            node = nodeStack.top();
            nodeStack.pop();
            
            if(node->getGroupID() == groupID && 
               !strncmp(node->getName(), searchName, mini(maxi(name.size(), strlen(node->getName())),NODE_NAME_LENGTH-1)))
            {
                return node;
            }
            else
            {      
                if(node->mSibling)
                {
                    nodeStack.push(node->mSibling);
                }
                
                if(node->mChild)
                {
                    nodeStack.push(node->mChild);
                }     
                
                node = 0;
            }            
        }
        return node;
    }
    
    // -------------==============-------------==============-------------==============-------------===
    PCSNode*  PCSNode::_depthFirstGroupIDSearch(PCSNode* root, const int groupID)
    {
        PCSNode* node = 0;
        std::stack<PCSNode*> nodeStack;
        nodeStack.push(root);
        
        while(!nodeStack.empty())
        {
            node = nodeStack.top();
            nodeStack.pop();
            
            if(node->getGroupID() == groupID)
            {
                return node;
            }
            else
            {      
                if(node->mSibling)
                {
                    nodeStack.push(node->mSibling);
                }
                
                if(node->mChild)
                {
                    nodeStack.push(node->mChild);
                }     
                
                node = 0;
            }            
        }
        return node;
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void PCSNode::_depthFirstGroupIDSearchMultipleResults(PCSNode* root, const int groupID, std::vector<PCSNode*>& outVec)
    {
        PCSNode* node;
        std::stack<PCSNode*> nodeStack;
        nodeStack.push(root);
        
        while(!nodeStack.empty())
        {
            node = nodeStack.top();
            nodeStack.pop();
            
            if(node->getGroupID() == groupID)
            {
                outVec.push_back(node);    
            }
            else
            {      
                if(node->mSibling)
                {
                    nodeStack.push(node->mSibling);
                }
                
                if(node->mChild)
                {
                    nodeStack.push(node->mChild);
                }     
                
                node = 0;
            }            
        }
    }
	
	// -------------==============-------------==============-------------==============-------------===
    void PCSNode::_stepDepthFirstIteration(std::stack<PCSNode *> &stack , Walaber::PCSNode *&node)
	{
		node = stack.top();
		stack.pop();
		
		     
		if(node->mSibling)
		{
			stack.push(node->mSibling);
		}
		
		if(node->mChild)
		{
			stack.push(node->mChild);
		}          
	}
}
