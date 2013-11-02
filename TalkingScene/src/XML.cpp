//
//  XMLIterator.cpp
//  WalaberEngine
//
//  Created by Jason Pecho on 10/11/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "XML.h"

#include "XMLHelper.h"
#include "FileManager.h"
#include "MemberCallback.h"

namespace Walaber 
{
	/// -------===========----------==================---------===========------=========-----------:):):):):)
    /// XMLDocument::AttributeIterator
    /// -------===========----------==================---------===========------=========-----------:):):):):)
    XMLDocument::AttributeIterator::AttributeIterator(xmlNodePtr node):
	mCurrentAttribute(node->properties),
	mStartAttribute(node->properties)
	{}
	
	/// -------===========----------==================---------===========------=========-----------:):):):):)
    XMLDocument::AttributeIterator::AttributeIterator(const AttributeIterator& iter):
	mCurrentAttribute(iter.mCurrentAttribute),
	mStartAttribute(iter.mStartAttribute)
	{}
	
	/// -------===========----------==================---------===========------=========-----------:):):):):)
    XMLDocument::AttributeIterator& XMLDocument::AttributeIterator::operator=(const AttributeIterator& iter)
	{
		mCurrentAttribute = iter.mCurrentAttribute;
		mStartAttribute = iter.mStartAttribute;
		
		return *this;
	}
	
	/// -------===========----------==================---------===========------=========-----------:):):):):)
    XMLDocument::AttributeIterator::~AttributeIterator()
	{}
	
	
	/// -------===========----------==================---------===========------=========-----------:):):):):)
    std::string XMLDocument::AttributeIterator::getName()
	{
		assert( mCurrentAttribute );
		
		return std::string( (char*)mCurrentAttribute->name );
	}
	
	/// -------===========----------==================---------===========------=========-----------:):):):):)
    Property XMLDocument::AttributeIterator::getValue()
	{
		assert( mCurrentAttribute );
		
		return Property( std::string( (char*)mCurrentAttribute->children->content ) );
	}
	
	
	
    /// -------===========----------==================---------===========------=========-----------:):):):):)
    /// XMLDocument::NamedIterator
    /// -------===========----------==================---------===========------=========-----------:):):):):)
    XMLDocument::NamedIterator::NamedIterator(xmlNodePtr node, const std::string& nodeName):
    mCurrentNode(node),
    mStartNode(node),
    mNodeName(nodeName)
    {}
    
    /// -------===========----------==================---------===========------=========-----------:):):):):)
    XMLDocument::NamedIterator::NamedIterator(const NamedIterator& iter):
    mCurrentNode(iter.mCurrentNode),
    mStartNode(iter.mStartNode),
    mNodeName(iter.mNodeName)
    {}
    
    /// -------===========----------==================---------===========------=========-----------:):):):):)
    XMLDocument::NamedIterator& XMLDocument::NamedIterator::operator=(const NamedIterator& iter)
    {
        mCurrentNode = iter.mCurrentNode;
        mStartNode = iter.mStartNode;
        mNodeName = iter.mNodeName;
        
        return *this;
    }
    
    /// -------===========----------==================---------===========------=========-----------:):):):):)
    XMLDocument::NamedIterator XMLDocument::NamedIterator::getChildNamedIterator(const std::string& pNodeName)
    {
        assert(mCurrentNode);
        xmlNodePtr child = 0;
        if(mCurrentNode)
        {
            child = mCurrentNode->children;
            const xmlChar* nodeName = (const xmlChar*)pNodeName.c_str();
            while(child && xmlStrcmp(child->name, nodeName))
            {
                child = child->next;
            }
        }
        
        return XMLDocument::NamedIterator(child, pNodeName);
    }
	
	/// -------===========----------==================---------===========------=========-----------:):):):):)
    XMLDocument::NamedIterator XMLDocument::NamedIterator::getNamedIteratorToDescendant(const std::string& pNodeName)
    {
        assert(mCurrentNode);
        
        return XMLDocument::NamedIterator( XML::getNode(mCurrentNode, pNodeName), pNodeName);
    }
	
    /// -------===========----------==================---------===========------=========-----------:):):):):)
    XMLDocument::Iterator XMLDocument::NamedIterator::getChildIterator()
    {           
        assert(mCurrentNode);
        xmlNodePtr child = 0;
        if(mCurrentNode)
        {
            child = mCurrentNode->children;
            while(child && !xmlStrcmp(child->name, (const xmlChar*)XML_NODE_VALUE_NAME))
            {
                child = child->next;
            }
        }
        
        return XMLDocument::Iterator(child);
    }
    
    /// -------===========----------==================---------===========------=========-----------:):):):):)
    bool XMLDocument::NamedIterator::getAttribute(const std::string& attributeName, Property& property )
    {
        assert(mCurrentNode);
        
        bool ret = false;
        if(mCurrentNode)
        {
            if(XML::attrExists(mCurrentNode, attributeName.c_str()))
            {
                property.setValue(XML::parseString(mCurrentNode, attributeName.c_str()));
                ret = true;
            }
            else
            {
#if _DEBUG
                //printf("Attribute %s does not exists: ", attributeName.c_str());
#endif
            }
        }
        
        return ret;
    }
    
    /// -------===========----------==================---------===========------=========-----------:):):):):)
    bool XMLDocument::NamedIterator::getNodeValue(Property& property)
    {
        assert(mCurrentNode);
        
        bool ret = false;
        if(mCurrentNode)
        {
            xmlNodePtr value = mCurrentNode->next;
            if(!xmlStrcmp(value->name, (const xmlChar*)XML_NODE_VALUE_NAME))
            {
                property.setValue(std::string((const char*)value->content));
                ret = true;
            }
            else
            {
#if _DEBUG
                printf("Node value does not exist for %s: ", (const char*)mCurrentNode->name);
#endif
            }
        }
        
        return ret;
    }
    
    /// -------===========----------==================---------===========------=========-----------:):):):):)
    /// XMLDocument::Iterator
    /// -------===========----------==================---------===========------=========-----------:):):):):)
    XMLDocument::Iterator::Iterator(xmlNodePtr node):
    mCurrentNode(node),
    mStartNode(node)
    {}
    
    /// -------===========----------==================---------===========------=========-----------:):):):):)
    XMLDocument::Iterator::Iterator(const Iterator& iter):
    mCurrentNode(iter.mCurrentNode),
    mStartNode(iter.mStartNode)
    {}
    
    /// -------===========----------==================---------===========------=========-----------:):):):):)
    XMLDocument::Iterator& XMLDocument::Iterator::operator=(const Iterator& iter)
    {
        mCurrentNode = iter.mCurrentNode;
        mStartNode = iter.mStartNode;
        
        return *this;
    }
    
    /// -------===========----------==================---------===========------=========-----------:):):):):)
    XMLDocument::NamedIterator XMLDocument::Iterator::getChildNamedIterator(const std::string& pNodeName)
    {
        assert(mCurrentNode);
        xmlNodePtr child = 0;
        if(mCurrentNode)
        {
            child = mCurrentNode->children;
            const xmlChar* nodeName = (const xmlChar*)pNodeName.c_str();
            while(child && xmlStrcmp(child->name, nodeName))
            {
                child = child->next;
            }
        }
        
        return XMLDocument::NamedIterator(child, pNodeName);
    }
    
    /// -------===========----------==================---------===========------=========-----------:):):):):)
    XMLDocument::Iterator XMLDocument::Iterator::getChildIterator()
    {           
        assert(mCurrentNode);
        xmlNodePtr child = 0;
        if(mCurrentNode)
        {
            child = mCurrentNode->children;
            while(child && !xmlStrcmp(child->name, (const xmlChar*)XML_NODE_VALUE_NAME))
            {
                child = child->next;
            }
        }
        
        return XMLDocument::Iterator(child);
    }
    
    /// -------===========----------==================---------===========------=========-----------:):):):):)
    bool XMLDocument::Iterator::getAttribute(const std::string& attributeName, Property& property )
    {
        assert(mCurrentNode);
        
        bool ret = false;
        if(mCurrentNode)
        {
            if(XML::attrExists(mCurrentNode, attributeName.c_str()))
            {
                property.setValue(XML::parseString(mCurrentNode, attributeName.c_str()));
                ret = true;
            }
            else
            {
#if _DEBUG
                //printf("Attribute %s does not exists: ", attributeName.c_str());
#endif
            }
        }
        
        return ret;
    }
    
    /// -------===========----------==================---------===========------=========-----------:):):):):)
    bool XMLDocument::Iterator::getNodeValue(Property& property)
    {
        assert(mCurrentNode);
        
        bool ret = false;
        if(mCurrentNode)
        {
            xmlNodePtr value = mCurrentNode->next;
            if(!xmlStrcmp(value->name, (const xmlChar*)XML_NODE_VALUE_NAME))
            {
                property.setValue(std::string((const char*)value->content));
                ret = true;
            }
            else
            {
#if _DEBUG
                printf("Node value does not exist for %s: ", (const char*)mCurrentNode->name);
#endif
            }
        }
        
        return ret;
    }
    
    /// -------===========----------==================---------===========------=========-----------:):):):):)
    /// XML Document
    /// -------===========----------==================---------===========------=========-----------:):):):):)
    std::string XMLDocument::ROOT_NODE_NAME_KEY = "root_node";

    /// -------===========----------==================---------===========------=========-----------:):):):):)
    XMLDocument::XMLDocument(const std::string& xmlPath, const CallbackPtr callback):
    mXMLDocument(0),
    mXMLRootNode(0),
    mCallback(callback)
    {
        PropertyList plist;
        
        MemberCallbackPtr<XMLDocument>::type memberCallback( new MemberCallback<XMLDocument>(this, &XMLDocument::_readFileCallback) );
        CallbackPtr tCallback = static_pointer_cast<Callback>(memberCallback);
        FileManager::getInstancePtr()->readFile(xmlPath, tCallback, plist);
    }
    
    
    /// -------===========----------==================---------===========------=========-----------:):):):):)
    XMLDocument::XMLDocument(const std::string& xmlPath, const std::string& rootNodeName, const CallbackPtr callback):
    mXMLDocument(0),
    mXMLRootNode(0),
    mCallback(callback)
    {
        PropertyList plist;
        plist.setValueForKey(ROOT_NODE_NAME_KEY, Property(rootNodeName));
        
        MemberCallbackPtr<XMLDocument>::type memberCallback( new MemberCallback<XMLDocument>(this, &XMLDocument::_readFileCallback) );
        CallbackPtr tCallback = static_pointer_cast<Callback>(memberCallback);
        FileManager::getInstancePtr()->readFile(xmlPath, tCallback, plist);
    }
    
    /// -------===========----------==================---------===========------=========-----------:):):):):)
    XMLDocument::XMLDocument(char* fileBuffer, size_t length):
    mXMLDocument(0),
    mXMLRootNode(0),
    mCallback()
    {
        _buildXMLObject(fileBuffer, length);
    }
    
    /// -------===========----------==================---------===========------=========-----------:):):):):)
    XMLDocument::XMLDocument(char* fileBuffer, size_t length, const std::string& rootNodeName):
    mXMLDocument(0),
    mXMLRootNode(0),
    mCallback()
    {
        _buildXMLObject(fileBuffer, length, rootNodeName);
    }
    
    /// -------===========----------==================---------===========------=========-----------:):):):):)
    XMLDocument::~XMLDocument()
    {
       if(mXMLDocument)
       {
           XML::cleanupDoc(mXMLDocument);
       }
    }
    
    /// -------===========----------==================---------===========------=========-----------:):):):):)
    void XMLDocument::_buildXMLObject(char* fileBuffer, size_t length)
    {
        mXMLRootNode = XML::loadDocMemory(fileBuffer, length, mXMLDocument);
    }
    
    /// -------===========----------==================---------===========------=========-----------:):):):):)
    void XMLDocument::_buildXMLObject(char* fileBuffer, size_t length, const std::string& rootNodeName)
    {
        mXMLRootNode = XML::loadDocMemory(fileBuffer, length, rootNodeName, mXMLDocument);
    }
    
    /// -------===========----------==================---------===========------=========-----------:):):):):)
    void XMLDocument::_readFileCallback(void* data)
    {
        FileManager::ReadFileCallbackParameters* params = (FileManager::ReadFileCallbackParameters*)data;
        
        if(params->buffer)
        {
            if(params->userData.keyExists(ROOT_NODE_NAME_KEY))
            {
                _buildXMLObject(params->buffer, params->length, params->userData.getValueForKey(ROOT_NODE_NAME_KEY)->asString());
            }
            else
            {
                _buildXMLObject(params->buffer, params->length);
            }
            
            if(mCallback)
            {
                XMLDocumentLoadedCallbackParameters params;
                if(mXMLRootNode)
                {
                    params.success = true;
                }
                else
                {
                    params.success = false;
                }
                
                mCallback->invoke(&params);
            }
            delete [] params->buffer;
        }
    }
}
