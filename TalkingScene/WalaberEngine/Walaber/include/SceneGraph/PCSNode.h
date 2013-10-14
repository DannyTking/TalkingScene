//
//  PCSNode.h
//  WalaberEngine
//
//  Created by Jason Pecho on 4/25/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef _PCS_NODE_H_
#define _PCS_NODE_H_

#include <string>
#include <string.h>
#include <cassert>
#include <vector>
#include <stack>
#include <WalaberMath.h>

#include <Logger.h>

#define NODE_NAME_LENGTH 48

namespace Walaber
{
    class PCSNode
    {
    public:// interface
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Default Ctor
        // </Summary>
        PCSNode():
        mParent(NULL),
        mChild(NULL),
        mSibling(NULL),
        mGroupID(-1)
        {
			memset(mName, 0, NODE_NAME_LENGTH);
            setName("default");
        }
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Name Ctor
        // </Summary>
        PCSNode(const std::string& name, const int groupId = -1):
        mParent(NULL),
        mChild(NULL),
        mSibling(NULL),
        mGroupID(groupId)
        {
			memset(mName, 0, NODE_NAME_LENGTH);
            setName(name);
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Name Ctor
        // </Summary>
        PCSNode(const PCSNode& node):
        mParent(node.mParent),
        mChild(node.mChild),
        mSibling(node.mSibling),
        mGroupID(node.mGroupID)
        {
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Name Ctor
        // </Summary>
        PCSNode& operator=(const PCSNode& node)
        {
            mParent = node.mParent;
            mChild = node.mChild;
            mSibling = node.mSibling;
            mGroupID = node.mGroupID;
            
            return *this;
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Name Ctor
        // </Summary>
        virtual ~PCSNode()
        {}
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // add a child to this node
        // </Summary>
        void addChild(PCSNode* child)
        {
            child->mSibling = mChild;
            child->mParent = this;
            mChild = child;
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // removes a child node if it exists in the tree
        // </Summary>
        PCSNode* removeChild(PCSNode* child)
        {
            PCSNode* ret = 0;
            
            if(child->mParent == this)
            {
                ret = child;
                
                if(child == child->mParent->mChild)
                {
                    // this is the first child in the list so remove it 
                    // and change the parents child to its sibling
                    child->mParent->mChild = child->mSibling;
                }
                else
                {
                    // the node is somewhere in the sibling list, find it, remove it, and delete it
                    PCSNode* lastNode = child->mParent->mChild;
                    PCSNode* curNode = child->mParent->mChild->mSibling;
                    
                    while(curNode != child)
                    {
                        lastNode = curNode;
                        curNode = curNode->mSibling;                
                    }
                    
                    lastNode->mSibling = curNode->mSibling;
                    curNode->mSibling = 0;
                }
				
				child->mParent = NULL;
            }
            
            return ret;
        }   		
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // removes a node and all of it's child nodes as well without deleting them based on name
        // </Summary>
        PCSNode* removeChild(const std::string& name)
        {
            PCSNode* node = getChild(name);
            
            if(node)
            {
                removeChild(node);
            }
            
            return node;
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // removes a node and all of it's child nodes as well without deleting them based on name & id
        // </Summary>
        PCSNode* removeChild(const std::string& name, const int groupID)
        {
            PCSNode* node = getChild(name, groupID);
            
            if(node)
            {
                removeChild(node);
            }
            
            return node;
        }
		
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // removes all children (sets their parent to NULL)
        // </Summary>
        void removeChildren()
        {
            PCSNode* node = mChild;
            
            while(node)
            {
                node->mParent = NULL;
				
				node = node->mSibling;
            }
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // finds a node based on a name, returns NULL if not found
        // </Summary>
        PCSNode* getChild(const std::string& name)
        {
            PCSNode* child = mChild;
            
            while(child)
            {
                if(!strncmp(child->mName, name.c_str(), NODE_NAME_LENGTH-1))
                {
                    break;
                }
                
                child = child->mSibling;
            }
            
            return child;
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // finds a node based on a name & groupID, returns NULL if not found, returns first found
        // </Summary>
        PCSNode* getChild(const std::string& name, const int groupID)
        {
            PCSNode* child = mChild;
            
            while(child)
            {
                if(child->mGroupID == groupID && 
                   !strncmp(child->mName, name.c_str(), NODE_NAME_LENGTH-1))
                {
                    break;
                }
                
                child = child->mSibling;
            }
            
            return child;
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // finds a node based on groupID, returns NULL if not found, returns first found
        // </Summary>
        PCSNode* getChild(const int groupID)
        {
            PCSNode* child = mChild;
            
            while(child)
            {
                if(child->mGroupID == groupID)
                {
                    break;
                }
                
                child = child->mSibling;
            }
            
            return child;
        }
		
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // get the parent of this node.
        // </Summary>
        PCSNode* getParent() const
		{
			return mParent;
		}
		
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // get the sibling of this node.
        // </Summary>
        PCSNode* getSibling() const
		{
			return mSibling;
		}
		
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // get the child of this node.
        // </Summary>
        PCSNode* getChild() const
		{
			return mChild;
		}
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // debug print name of node
        // </Summary>
        void printName()
        {
            Logger::printf("Walaber", Logger::SV_INFO, "Node: [%s]", mName);
        };
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // get a const ptr to the name
        // </Summary>
        const char* getName()const
        {
            return mName;
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // get integer group id
        // </Summary>
        int getGroupID()
        {
            return mGroupID;
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // sets the name of a node
        // </Summary>
        void setName(const std::string& name)
        {
			assert(name.size() < NODE_NAME_LENGTH);
            strncpy(mName,name.c_str(),mini((int)name.size()+1,NODE_NAME_LENGTH-1));
			mName[NODE_NAME_LENGTH-1]=0;
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set integer group id
        // </Summary>
        void setGroupID(const int groupID)
        {
            mGroupID = groupID;
        }
    public: // static helpers
        
        //--------------------------------------------------------------------------------------------------
        // convnience functions for getting and removing nodes by name and id in a tree (given a root)
        //--------------------------------------------------------------------------------------------------
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // removes a node and all of it's child nodes as well without deleting them based on name
        // </Summary>
        static PCSNode* removeNodeFromTree(PCSNode* root, const std::string& name); 
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // removes a node and all of it's child nodes as well without deleting them based on name & id
        // </Summary>
        static PCSNode* removeNodeFromTree(PCSNode* root, const std::string& name, const int groupID); 
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // finds a node based on a name, returns NULL if not found
        // </Summary>
        static PCSNode* getNodeFromTree(PCSNode* root, const std::string& name);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // finds a node based on a name & groupID, returns NULL if not found, returns first found
        // </Summary>
        static PCSNode* getNodeFromTree(PCSNode* root, const std::string& name, const int groupID);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // finds a node based on groupID, returns NULL if not found, returns first found
        // </Summary>
        static PCSNode* getNodeFromTree(PCSNode* root, const int groupID);        
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // fills a vector with objects of a given groupID
        // </Summary>
        static void getNodesFromTree(PCSNode* root, const int groupID, std::vector<PCSNode*>& outPCSNodeList);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // fills a vector with objects of a given groupID, casts PCSNode* to template typename 
        // T* - so T* should be a derived class of PCSNode or this will EXPLODE
        // </Summary>
        template< typename T >
        static void getNodesFromTree(PCSNode* root, const int groupID, std::vector<T*>& outVec)
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
                    outVec.push_back(static_cast<T*>(node));    
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
        // <Summary>
        // delete a node [memory] and all of it's child nodes as well
        // </Summary>
        static void deleteNodeAndChildrenFromTree(PCSNode* node);       
        
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // returns number of memebers in the entire tree starting from and including "root"
        // </Summary>
        static unsigned int getTreeSize(PCSNode* root);
		
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // prints the name of all of the nodes in the tree
        // </Summary>
        static void printTree(PCSNode* root);
		
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // checks to see if a tree has a loop in it.
        // </Summary>
		static bool hasLoop(PCSNode* startNode)
		{
			PCSNode* slowNode = 0;
			PCSNode* fastNode1 = 0;
			PCSNode* fastNode2 = 0;
			std::stack<PCSNode*> slowStack;
			std::stack<PCSNode*> fastStack1;
			std::stack<PCSNode*> fastStack2;
			slowStack.push(startNode);
			fastStack1.push(startNode);
			fastStack2.push(startNode);
			
			while(!slowStack.empty() && !fastStack1.empty() && !fastStack2.empty())
			{
				// step the depth first search once for the first fast node
				_stepDepthFirstIteration( fastStack1, fastNode1 );
				
				// step the depth first search twice for the second
				_stepDepthFirstIteration( fastStack2, fastNode2 );
				_stepDepthFirstIteration( fastStack2, fastNode2 );
				
				if( slowNode == fastNode1 || slowNode == fastNode2 )
				{
					// theres a cycle!!!!
					return true;
				}
				
				_stepDepthFirstIteration( slowStack, slowNode );				
			}
			
			return false;
		}

    private: //interface
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Depth-first search removal
        // </Summary>
        static void _recursiveDelete(PCSNode* node);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Depth-first search for name
        // </Summary>
        static PCSNode* _depthFirstNameSearch(PCSNode* root, const std::string& name);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Depth-first search for name with groupID
        // </Summary>
        static PCSNode* _depthFirstNameSearchWithGroupID(PCSNode* root, const std::string& name, const int groupID);      
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Returns first item tagged with groupID
        // </Summary>
        static PCSNode* _depthFirstGroupIDSearch(PCSNode* root, const int groupID);  
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // fills a vector with PCSNode* with a given groupID
        // </Summary>
        static void _depthFirstGroupIDSearchMultipleResults(PCSNode* root, const int groupID, std::vector<PCSNode*>& outVec); 
		
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // take one step through a depthfirst iteration of a tree using the stack and node provided
        // </Summary>
        static void _stepDepthFirstIteration( std::stack<PCSNode*>& stack, PCSNode*& node );
        
    protected: // members
        PCSNode*        mParent;
        PCSNode*        mChild;
        PCSNode*        mSibling;
        int             mGroupID;
        char            mName[NODE_NAME_LENGTH];
    };
}

#endif
