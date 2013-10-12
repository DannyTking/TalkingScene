//
//  XMLIterator.h
//  WalaberEngine
//
//  Created by Jason Pecho on 10/11/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef _XML_H_
#define _XML_H_

#include <XMLHelper.h>
#include <Callback.h>

#define XML_NODE_VALUE_NAME "text"

namespace Walaber
{
    class XMLDocument
    {
    public:// typedefs structs enums classes
        
        class Iterator;
		
		
		/// <Summary>
        /// used to iterator over a set of XML attributes for a given node
        /// </Summary>
		class AttributeIterator
		{
		public:
			/// <Summary>
            /// only ctor is for initialization
            /// </Summary>
			AttributeIterator(xmlNodePtr node);
			
			/// <Summary>
            /// copy ctor
            /// </Summary>
			AttributeIterator(const AttributeIterator& iter);
			
			/// <Summary>
            /// assignment operator
            /// </Summary>
			AttributeIterator& operator=(const AttributeIterator& iter);
			
			/// <Summary>
            /// destructor
            /// </Summary>
			~AttributeIterator();
			
			/// <Summary>
            /// operator bool
            /// </Summary>
			operator bool() const
			{
				return mCurrentAttribute != 0;
			}
			
			/// <Summary>
            /// prefix incrementer.  moves to next node.
            /// </Summary>
			void operator++()
			{
				assert(mCurrentAttribute);
				
				if (mCurrentAttribute)
					mCurrentAttribute = mCurrentAttribute->next;
			}
			
			/// <Summary>
            /// postfix incrementer.  moves to next node.
            /// </Summary>
			void operator++(int)
			{
				assert(mCurrentAttribute);
				
				if (mCurrentAttribute)
					mCurrentAttribute = mCurrentAttribute->next;
			}
			
			/// <Summary>
            /// prefix decrementer.  moves to previous node.
            /// </Summary>
			void operator--()
			{
				assert(mCurrentAttribute);
				
				if (mCurrentAttribute)
					mCurrentAttribute = mCurrentAttribute->prev;
			}
			
			/// <Summary>
            /// postfix decrementer.  moves to previous node.
            /// </Summary>
			void operator--(int)
			{
				assert(mCurrentAttribute);
				
				if (mCurrentAttribute)
					mCurrentAttribute = mCurrentAttribute->prev;
			}
			
			/// <Summary>
            /// get the name of this attribute.
            /// </Summary>
			std::string getName();
			
			/// <Summary>
            /// get the value of this attribute (as a Property).
            /// </Summary>
			Property getValue();
			
			/// <Summary>
            /// prefix decrementer.  moves to previous node.
            /// </Summary>
			void reset()
			{
				assert(mStartAttribute);
				mCurrentAttribute = mStartAttribute;
			}
			
			
		private:
			AttributeIterator() {}
			
		private:
			xmlAttrPtr			mCurrentAttribute;
			xmlAttrPtr			mStartAttribute;
			
		};
		
        
		
        /// <Summary>
        /// used to iterator over a set of XML Node Siblings with a given name
        /// </Summary>
        class NamedIterator
        {
        public: // interface
            /// <Summary>
            /// default ctor for initialization
            /// </Summary>
            NamedIterator():
            mCurrentNode(0),
            mStartNode(0),
            mNodeName()
            {};
            
            /// <Summary>
            /// ctor is for initialization
            /// </Summary>
            NamedIterator(xmlNodePtr node, const std::string& nodeName);
            
            /// <Summary>
            /// copy ctor
            /// </Summary>
            NamedIterator(const NamedIterator& iter);
            
            /// <Summary>
            /// moves the iterator to the next sibling node
            /// </Summary>
            NamedIterator& operator=(const NamedIterator& iter);
            
            /// <Summary>
            /// dtor
            /// </Summary>
            ~NamedIterator()
            {};
            
            /// <Summary>
            /// operator bool
            /// </Summary>
            operator bool() const
            {
                return mCurrentNode != 0;
            }
            
            /// <Summary>
            /// prefix moves the iterator to the next sibling node
            /// </Summary>
            void operator++()
            {
                assert(mCurrentNode);
                
                if(mCurrentNode)
                {
                    mCurrentNode = mCurrentNode->next;
                    const xmlChar* nodeName = (const xmlChar*)mNodeName.c_str();
                    while(mCurrentNode && xmlStrcmp(mCurrentNode->name, nodeName))
                    {
                        mCurrentNode = mCurrentNode->next;
                    }
                }
            }
            
            /// <Summary>
            /// postfix moves the iterator to the next sibling node
            /// </Summary>
            void operator++(int)
            {
                assert(mCurrentNode);
                
                if(mCurrentNode)
                {
                    mCurrentNode = mCurrentNode->next;
                    const xmlChar* nodeName = (const xmlChar*)mNodeName.c_str();
                    while(mCurrentNode && xmlStrcmp(mCurrentNode->name, nodeName))
                    {
                        mCurrentNode = mCurrentNode->next;
                    }
                }
            }
            
            /// <Summary>
            /// prefix moves the iterator to the previous node
            /// </Summary>
            void operator--()
            {
                assert(mCurrentNode);
                
                if(mCurrentNode)
                {
                    mCurrentNode = mCurrentNode->prev;
                    const xmlChar* nodeName = (const xmlChar*)mNodeName.c_str();
                    while(mCurrentNode && xmlStrcmp(mCurrentNode->name, nodeName))
                    {
                        mCurrentNode = mCurrentNode->prev;
                    }
                }
            }
            
            /// <Summary>
            /// postfix moves the iterator to the previous node
            /// </Summary>
            void operator--(int)
            {
                assert(mCurrentNode);
                
                if(mCurrentNode)
                {
                    mCurrentNode = mCurrentNode->prev;
                    const xmlChar* nodeName = (const xmlChar*)mNodeName.c_str();
                    while(mCurrentNode && xmlStrcmp(mCurrentNode->name, nodeName))
                    {
                        mCurrentNode = mCurrentNode->prev;
                    }
                }
            }
            
            /// <Summary>
            /// returns an iterator pointing to first child with name "nodeName"
            /// </Summary>
            NamedIterator getChildNamedIterator(const std::string& nodeName);
            
            /// <Summary>
            /// returns an iterator pointing to first child
            /// </Summary>
            Iterator getChildIterator();
            
			/// <Summary>
            /// returns an iterator pointing to first child with name "nodeName"
            /// </Summary>
            NamedIterator getNamedIteratorToDescendant(const std::string& pNodeName);
			
            xmlNodePtr getCurrentNode()
            {
                return mCurrentNode;
            }
            
            /// <Summary>
            /// fills a Property object (property) with the attribute named "attributeName"
            /// </Summary>
            ///
            /// <ReturnValue>
            /// returns true if the attribute exists and the Property object is filled, false otherwise
            /// </ReturnValue>
            bool getAttribute(const std::string& attributeName, Property& property );
            
			/// <Summary>
            /// creates an iterator over attributes of this node.
            /// </Summary>
            ///
            /// <ReturnValue>
            /// attribute iterator
            /// </ReturnValue>
            AttributeIterator getAttributeIterator()
			{
				assert( mCurrentNode );
				
				return AttributeIterator( mCurrentNode );
			}
			
            /// <Summary>
            /// fills a Property object (property) with the value of the node aka <Node> Value </Node>
            /// </Summary>
            ///
            /// <ReturnValue>
            /// returns false if value is empty, true otherwise
            /// </ReturnValue>
            bool getNodeValue(Property& property);
            
            /// <Summary>
            /// returns the name of the node
            /// </Summary>
            std::string getNodeName()
            {
                assert(mCurrentNode);
                return std::string((const char*)mCurrentNode->name);
            }
            
            /// <Summary>
            /// resets the iterator to the node it started on
            /// </Summary>
            void reset()
            {
                assert(mStartNode);
                mCurrentNode = mStartNode;
            }
            
        private: // members
            
            xmlNodePtr              mCurrentNode;
            xmlNodePtr              mStartNode;
            std::string             mNodeName;
        };
        
        /// <Summary>
        /// used to iterator over a any set of XML Node Siblings
        /// </Summary>
        class Iterator
        {
        public: // interface
            
            Iterator():
            mCurrentNode(0),
            mStartNode(0)
            {};
            
            /// <Summary>
            /// only ctor is for initialization
            /// </Summary>
            explicit Iterator(xmlNodePtr node);
            
            /// <Summary>
            /// copy ctor
            /// </Summary>
            Iterator(const Iterator& iter);
            
            /// <Summary>
            /// moves the iterator to the next sibling node
            /// </Summary>
            Iterator& operator=(const Iterator& iter);
            
            /// <Summary>
            /// dtor
            /// </Summary>
            ~Iterator()
            {};
            
            /// <Summary>
            /// operator bool
            /// </Summary>
            operator bool() const
            {
                return mCurrentNode != 0;
            }
            
            /// <Summary>
            /// prefix moves the iterator to the next sibling node
            /// </Summary>
            void operator++()
            {
                assert(mCurrentNode);
                
                if(mCurrentNode)
                {
                    mCurrentNode = mCurrentNode->next;
                    while(mCurrentNode && !xmlStrcmp(mCurrentNode->name, (const xmlChar*)XML_NODE_VALUE_NAME))
                    {
                        mCurrentNode = mCurrentNode->next;
                    }
                }
            }
            
            /// <Summary>
            /// postfix moves the iterator to the next sibling node
            /// </Summary>
            void operator++(int)
            {
                assert(mCurrentNode);
                
                if(mCurrentNode)
                {
                    mCurrentNode = mCurrentNode->next;
                    while(mCurrentNode && !xmlStrcmp(mCurrentNode->name, (const xmlChar*)XML_NODE_VALUE_NAME))
                    {
                        mCurrentNode = mCurrentNode->next;
                    }
                }
            }
            
            /// <Summary>
            /// prefix moves the iterator to the previous node
            /// </Summary>
            void operator--()
            {
                assert(mCurrentNode);
                
                if(mCurrentNode)
                {
                    mCurrentNode = mCurrentNode->prev;
                    while(mCurrentNode && !xmlStrcmp(mCurrentNode->name, (const xmlChar*)XML_NODE_VALUE_NAME))
                    {
                        mCurrentNode = mCurrentNode->prev;
                    }
                }
            }
            
            /// <Summary>
            /// prefix moves the iterator to the previous node
            /// </Summary>
            void operator--(int)
            {
                assert(mCurrentNode);
                
                if(mCurrentNode)
                {
                    mCurrentNode = mCurrentNode->prev;
                    while(mCurrentNode && !xmlStrcmp(mCurrentNode->name, (const xmlChar*)XML_NODE_VALUE_NAME))
                    {
                        mCurrentNode = mCurrentNode->prev;
                    }
                }
            }
            
            /// <Summary>
            /// returns an iterator pointing to first child with name "nodeName"
            /// </Summary>
            NamedIterator getChildNamedIterator(const std::string& nodeName);
            
            /// <Summary>
            /// returns an iterator pointing to first child
            /// </Summary>
            Iterator getChildIterator();
            
            /// <Summary>
            /// fills a Property object (property) with the attribute named "attributeName"
            /// </Summary>
            ///
            /// <ReturnValue>
            /// returns true if the attribute exists and the Property object is filled, false otherwise
            /// </ReturnValue>
            bool getAttribute(const std::string& attributeName, Property& property );            
            
			/// <Summary>
            /// creates an iterator over attributes of this node.
            /// </Summary>
            ///
            /// <ReturnValue>
            /// attribute iterator
            /// </ReturnValue>
            AttributeIterator getAttributeIterator()
			{
				assert( mCurrentNode );
				
				return AttributeIterator( mCurrentNode );
			}
			
            /// <Summary>
            /// fills a Property object (property) with the value of the node aka <Node> Value </Node>
            /// </Summary>
            ///
            /// <ReturnValue>
            /// returns false if value is empty, true otherwise
            /// </ReturnValue>
            bool getNodeValue(Property& property);
            
            /// <Summary>
            /// returns the name of the node
            /// </Summary>
            std::string getNodeName()
            {
                assert(mCurrentNode);
                return std::string((const char*)mCurrentNode->name);
            }
            
            /// <Summary>
            /// resets the iterator to the node it started on
            /// </Summary>
            void reset()
            {
                assert(mStartNode);
                mCurrentNode = mStartNode;
            }
                        
        private: // members
            
            xmlNodePtr              mCurrentNode;
            xmlNodePtr              mStartNode;
        };
        
        struct XMLDocumentLoadedCallbackParameters
        {
            bool success;
        };
        
    public:// interface
        
        /// <Summary>
        /// Default ctor
        /// </Summary>
        XMLDocument():
        mXMLDocument(0),
        mXMLRootNode(0)
        {};
        
        /// <Summary>
        /// ctor that takes a path to an xml file
        /// </Summary>
        XMLDocument(const std::string& xmlPath, const CallbackPtr callback = CallbackPtr());
        
        /// <Summary>
        /// ctor that takes a path to an xml file and a root node name
        /// </Summary>
        XMLDocument(const std::string& xmlPath, const std::string& rootNodeName, const CallbackPtr callback = CallbackPtr());
        
        /// <Summary>
        /// ctor that takes the file buffer for an xml file
        /// </Summary>
        XMLDocument(char* fileBuffer, size_t length);
        
        /// <Summary>
        /// ctor that takes the file buffer for an xml file and a root node name
        /// </Summary>
        XMLDocument(char* fileBuffer, size_t length, const std::string& rootNodeName);
        
        /// <Summary>
        /// Default ctor
        /// </Summary>
        ~XMLDocument();
        
        /// <Summary>
        /// Default ctor
        /// </Summary>
        operator bool() const
        {
            return mXMLDocument != 0;
        }
        
        /// <Summary>
        /// moves the iterator to the first node named "nodeName"
        /// </Summary>
        XMLDocument::NamedIterator getNamedIteratorTo(const std::string& nodeName)
        {
            assert(mXMLDocument);
            
            return XMLDocument::NamedIterator(XML::getNode(mXMLRootNode, nodeName), nodeName);
        }
        
        /// <Summary>
        /// moves the iterator to the first node named "nodeName"
        /// </Summary>
        XMLDocument::Iterator getIteratorTo(const std::string& nodeName)
        {
            assert(mXMLDocument);

            return XMLDocument::Iterator(XML::getNode(mXMLRootNode, nodeName));
        }
        
        /// <Summary>
        /// returns a Itertaor
        /// </Summary>
        XMLDocument::NamedIterator getRootNamedIterator()
        {
            assert(mXMLDocument);

            return XMLDocument::NamedIterator(mXMLRootNode, std::string((const char*)mXMLRootNode->name));
        }
        
        /// <Summary>
        /// returns a Itertaor
        /// </Summary>
        XMLDocument::Iterator getRootIterator()
        {
            assert(mXMLDocument);

            return XMLDocument::Iterator(mXMLRootNode);
        }
        
    private:// interface
        
        /// <Summary>
        /// build the xml object 
        /// </Summary>
        void _buildXMLObject(char* fileBuffer, size_t length);
        
        /// <Summary>
        /// callback fro reading the data from the xmlfile path
        /// </Summary>
        void _buildXMLObject(char* fileBuffer, size_t length, const std::string& rootNodeName);
        
        /// <Summary>
        /// callback fro reading the data from the xmlfile path
        /// </Summary>
        void _readFileCallback(void* data);
        
    private:// members
        
        xmlDocPtr                       mXMLDocument;
        xmlNodePtr                      mXMLRootNode;
        
        CallbackPtr                     mCallback;
        
        static std::string              ROOT_NODE_NAME_KEY;
    };
}

#endif
