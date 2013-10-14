//
//  Skeleton.cpp
//  WalaberEngine
//
//  Created by Jason Pecho on 5/3/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "Skeleton.h"

#include <stack>
#include <queue>

#include <Bone.h>
#include <Sprite.h>
#include <SpriteBatch.h>
#include <WalaberMath.h>
#include <CurveManager.h>

#include <MemberCallback.h>
#include <SpriteManager.h>

//#define _DRAW_DEBUG_SKELETON

namespace Walaber
{
    Skeleton::SkeletonLoadedCallbackMap          Skeleton::mCallbackMap;

    // -------------==============-------------==============-------------==============-------------===
    Skeleton::Skeleton():
    DrawableNode(),
    mBindPose(),
    mBones(),
    mSprites(),
    mSpriteRelativeLayers(),
    mAnimationManager( this ),
    mColor(),
    mCurveCollection(),
    mIsScreenSpace( false )
    {
        mGroupID = NG_Skeleton;
    }
    
    // -------------==============-------------==============-------------==============-------------===
    Skeleton::Skeleton(const std::string& name, const int groupID):
    DrawableNode(name, groupID),
    mBindPose(),
    mBones(),
    mSprites(),
    mSpriteRelativeLayers(),
    mAnimationManager( this ),
    mColor(),
    mCurveCollection(),
    mIsScreenSpace( false )
    {
        
    }
    
    // -------------==============-------------==============-------------==============-------------===
    // <Summary>
    // Positional ctor
    // </Summary>
    Skeleton::Skeleton(const std::string& name, const Vector2& pos, const float angle,
                       const Vector2& scale):
    DrawableNode(name, NG_Skeleton),
    mBindPose(),
    mBones(),
    mSprites(),
    mSpriteRelativeLayers(),
    mAnimationManager( this ),
    mColor(),
    mCurveCollection(),
    mIsScreenSpace( false )
    {
        setLocalPosition2D( pos );
        setLocalAngleZ( angle );
        setLocalScale2D( scale );
    }
    
    // -------------==============-------------==============-------------==============-------------===
    Skeleton::Skeleton(const Skeleton& skeleton):
    DrawableNode(skeleton),
    mBindPose(skeleton.mBindPose),
    mBones(skeleton.mBones),
    mSprites(skeleton.mSprites),
    mSpriteRelativeLayers(skeleton.mSpriteRelativeLayers),
    mAnimationManager(skeleton.mAnimationManager),
    mColor(skeleton.mColor),
	mCurveCollection(skeleton.mCurveCollection),
    mIsScreenSpace(skeleton.mIsScreenSpace)
    {
        
    }
    
    // -------------==============-------------==============-------------==============-------------===
    Skeleton& Skeleton::operator=(const Skeleton& skeleton)
    {
        DrawableNode::operator=(skeleton);
        mBindPose = skeleton.mBindPose;
        mBones = skeleton.mBones;
        mSprites = skeleton.mSprites;
        mSpriteRelativeLayers = skeleton.mSpriteRelativeLayers;
        mAnimationManager = skeleton.mAnimationManager;
        mColor = skeleton.mColor;
		mCurveCollection = skeleton.mCurveCollection;
        mIsScreenSpace = skeleton.mIsScreenSpace;
        
        return *this;
    }
    
    // -------------==============-------------==============-------------==============-------------===
    Skeleton::~Skeleton()
    {	
		BoneMap::iterator itBones = mBones.begin();
		BoneMap::iterator itBonesEnd = mBones.end();
		while ( itBones != itBonesEnd )
		{
			delete itBones->second;
			++itBones;
		}

		SpriteMap::iterator itSprites = mSprites.begin();
		SpriteMap::iterator itSpritesEnd = mSprites.end();
		while ( itSprites != itSpritesEnd )
		{
			delete itSprites->second;
			++itSprites;
		}
		
        // curve collections in the skeleton have been deprecated, remove this too
		if( mCurveCollection.size() )
		{
			CurveManager::getManager().unloadCurveCollection( mCurveCollection );
		}
        
        // curve collections in the skeleton have been deprecated, remove this too
		if( mSpriteCollection.size() )
		{
			SpriteManager::getInstancePtr()->unloadSpriteCollection( mSpriteCollection );
		}
    }
    
    // -------------==============-------------==============-------------==============-------------===
    Skeleton* Skeleton::createFromXmlNode(xmlNodePtr rootNode)
    {        
        Skeleton* ret = 0;
        
        if(rootNode)
        {
            // get name and create skeleton
            std::string skelName = XML::parseString(rootNode,"name");
            
            ret = new Skeleton(skelName);
			
            ret->_buildSkeleton( rootNode );
        }
        
        return ret;
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void Skeleton::loadSkeletonWithCallback(const std::string& xmlPath, CallbackPtr callback, 
											const Vector2& pos, const float angle, const Vector2& scale, const std::string& name)
    {
        std::pair< SkeletonLoadedCallbackMap::iterator, SkeletonLoadedCallbackMap::iterator > ret = mCallbackMap.equal_range(xmlPath);
        
        // count how many are in the map
        int i =0;
        for(SkeletonLoadedCallbackMap::iterator itr = ret.first; itr != ret.second; ++itr)
        {
          ++i;  
        }
        
        char n[4];
        memset(n, 0, 4* sizeof(char));
        PropertyList plist;
        
        if(i)
        {
            // put the number into a buffer for appending onto the path name
            snprintf(n, 4, "%i", i);
            plist.setValueForKey("file_num", Property(i));
        }
		
		plist.setValueForKey("pos", Property(pos));
		plist.setValueForKey("angle", Property(angle));
		plist.setValueForKey("name", Property(name));
        plist.setValueForKey("scale", Property(scale));
		
        mCallbackMap.insert(std::make_pair(xmlPath+std::string(n), callback));

        FileManager::getInstancePtr()->readFile(xmlPath, CallbackPtr( new Callback( &Skeleton::_skeletonHasLoaded ) ), plist, FileManager::PP_NoAbstraction);
    }
    
    void Skeleton::loadFromXMLFile( const std::string& xmlPath)
    {
        PropertyList plist;
		
		MemberCallbackPtr<Skeleton>::type memberCallback( new MemberCallback<Skeleton>(this, &Skeleton::_skeletonFileLoaded) );
		CallbackPtr callback = static_pointer_cast<Callback>(memberCallback);
		
        FileManager::getInstancePtr()->readFile(xmlPath, callback, plist, FileManager::PP_NoAbstraction);
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void Skeleton::setLayer(const int layer)
    {
        DrawableNode::setLayer(layer);
        
        SpriteMap::iterator it = mSprites.begin();
        SpriteMap::iterator itEnd = mSprites.end();
        
		
        while (it != itEnd) 
        {
            Sprite* sprite = (*it).second;
            sprite->setLayer( layer + mSpriteRelativeLayers[sprite] );
            ++it;
        }
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void Skeleton::update(float deltaTime)
    {
		SpriteMap::iterator it = mSprites.begin();
        SpriteMap::iterator itEnd = mSprites.end();
        
        while (it != itEnd) 
        {
            (*it).second->update(deltaTime);
            ++it;
        }
		
        mAnimationManager.update(deltaTime);
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void Skeleton::draw(SpriteBatch* sb)
    {
        //SpriteBatch::BatchMode bm = sb->getBatchMode();
		
		//if (bm == SpriteBatch::BM_LAYERS)
		//{
		//	sb->flush();
		//	sb->start(SpriteBatch::BM_IMMEDIATE);	
		//}
        
		if( ! getVisible() )
        {
            return;
        }
        
        SpriteMap::iterator it = mSprites.begin();
        SpriteMap::iterator itEnd = mSprites.end();
		
        while (it != itEnd) 
        {
            (*it).second->draw(sb);
            ++it;
        }
        
        //if (bm == SpriteBatch::BM_LAYERS)
		//{
		//	sb->flush();
		//	sb->start(bm);
		//}
        
#ifdef _DRAW_DEBUG_SKELETON
        drawSkeleton( *sb );
#endif
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void Skeleton::addSpriteAsChildToBone(Bone* bone, Sprite* sprite, const int spriteLayer)
    {
        bone->addChild(sprite);
        
        SpriteMap::iterator it = mSprites.find(sprite->getName());
        if(it == mSprites.end())
        {
            mSprites[sprite->getName()] = sprite;
            mSpriteRelativeLayers[sprite] = spriteLayer;
            sprite->setLayer( getLayer() + spriteLayer );
        }
        else 
        {
            // skeletons require uniquely named sprites 
            assert(0);
        }
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void Skeleton::addRootBone(Bone* bone)
    {
        addChild(bone);
        mBindPose[bone] = BoneLocalData( bone->getLocalPosition(),
                                         bone->getLocalScale(),
                                         bone->getLocalAngles() );
        bone->setSkeleton(this);
        
        BoneMap::iterator it = mBones.find(bone->getName());
        if(it == mBones.end())
        {
            mBones[bone->getName()] = bone;
        }
        else 
        {
            // skeletons require uniquely named bones 
            assert(0);
        }
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void Skeleton::addChildBone(Bone* parentBone, Bone* childBone)
    {
        parentBone->addChild(childBone);
        mBindPose[childBone] = BoneLocalData(
                                              childBone->getLocalPosition(),
                                              childBone->getLocalScale(),
                                              childBone->getLocalAngles() );
        childBone->setSkeleton(this);
        
        BoneMap::iterator it = mBones.find(childBone->getName());
        if(it == mBones.end())
        {
            mBones[childBone->getName()] = childBone;
        }
        else 
        {
            // skeletons require uniquely named bones 
            assert(0);
        }
    }
    
    // -------------==============-------------==============-------------==============-------------===
    Bone* Skeleton::getBone(const std::string& name)
    {
        Bone* ret = 0;
        
        BoneMap::iterator it = mBones.find(name);
        if(it != mBones.end())
        {
            ret = (*it).second;
        }      
        
        return ret;
    }
    
    // -------------==============-------------==============-------------==============-------------===
    Sprite* Skeleton::getSprite(const std::string& name)
    {
        Sprite* ret = 0;
        
        SpriteMap::iterator it = mSprites.find(name);
        if(it != mSprites.end())
        {
            ret = (*it).second;
        }      
        
        return ret;
    }
    
    // -------------==============-------------==============-------------==============-------------===
    Node* Skeleton::getNode(const std::string& name)
    {
        Node* ret = 0;
        
        BoneMap::iterator it = mBones.find(name);
        if(it != mBones.end())
        {
            ret = (*it).second;
        }      
        
        if(!ret)
        {
            SpriteMap::iterator itSpr = mSprites.find(name);
            if(itSpr != mSprites.end())
            {
                ret = (*itSpr).second;
            }     
        }
        
        return ret;
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void Skeleton::playSpriteAnimations()
    {
        // now tell all of the sprites they are in screen space
		SpriteMap::iterator it = mSprites.begin();
		SpriteMap::iterator itEnd = mSprites.end();
		while( it != itEnd )
		{
			SpriteAnimationPtr curAnim = it->second->getCurrentAnimation();
            if( curAnim )
            {
                curAnim->play();
            }
			++it;
		}
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void Skeleton::pauseSpriteAnimations()
    {
        // now tell all of the sprites they are in screen space
		SpriteMap::iterator it = mSprites.begin();
		SpriteMap::iterator itEnd = mSprites.end();
		while( it != itEnd )
		{
			SpriteAnimationPtr curAnim = it->second->getCurrentAnimation();
            if( curAnim )
            {
                curAnim->pause();
            }
			++it;
		}
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void Skeleton::loadAnimationsFromXml(const std::string& xmlPath,
                                         const AnimationManager::AnimationLoadBehavior loadBehavior)
    {
        
        switch ( loadBehavior )
        {
            case AnimationManager::AnimationLoadBehavior_Overwrite:
                
                mAnimationManager.clear();
            
                break;
                
            case AnimationManager::AnimationLoadBehavior_Append:
                
                break;
                
            default:
                
                // Undefined behavior
                assert(0);
                
        }
        
        PropertyList plist;
		
		MemberCallbackPtr<Skeleton>::type memberCallback( new MemberCallback<Skeleton>(this, &Skeleton::_animationsHaveLoaded) );
		CallbackPtr callback = static_pointer_cast<Callback>(memberCallback);
		
        FileManager::getInstancePtr()->readFile(xmlPath, callback, plist, FileManager::PP_NoAbstraction);
    }
    
    void Skeleton::_buildSkeleton( xmlNodePtr rootNode )
    {
        if(XML::attrExists(rootNode, "curveCollection"))
        {
            setCurveCollectionName(XML::parseString(rootNode, "curveCollection"));
            CurveManager::getManager().loadCurveCollection(getCurveCollectionName());
        }
        
        if(XML::attrExists(rootNode, "spriteCollection"))
        {
            setSpriteCollectionName(XML::parseString(rootNode, "spriteCollection"));
            SpriteManager::getInstancePtr()->loadSpriteCollection(getSpriteCollectionName());
        }
        
        _recursiveBuildSkeleton(rootNode, this, 0);
        
        if(XML::attrExists(rootNode, "animations"))
        {
            std::string aniPath = XML::parseString(rootNode, "animations");
            if( StringHelper::getExtension( aniPath ) == ".ani" )
            {
                // load the animation data
                mAnimationManager.loadAnimationsFromXml( aniPath,
                                                         AnimationManager::AnimationLoadBehavior_Overwrite
                                                       );
            }
            else
            {
                // load the animation data
                mAnimationManager.loadAnimationsFromBin( aniPath,
                                                         AnimationManager::AnimationLoadBehavior_Overwrite
                                                       );
            }
        }
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void Skeleton::_recursiveBuildSkeleton(xmlNodePtr parentXML, Skeleton* skel, Node* parent)
    {
        xmlNodePtr child = parentXML->children;
        while(child)
        {
            if (!xmlStrcmp(child->name, (const xmlChar*)"Bone"))
			{
                // found a bone child, attach it and recursively call on the bone
                std::string boneName = XML::parseString(child, "name");
                Bone* bone = new Bone(boneName);
                if(parent)
                {
                    skel->addChildBone(static_cast<Bone*>(parent), bone);
                }
                else
                {
                    skel->addRootBone(bone);
                }
                
                _recursiveBuildSkeleton(child, skel, bone);
                _setNodeProperties(child, bone);
            }
            else if (!xmlStrcmp(child->name, (const xmlChar*)"Sprite"))
			{
                if(parent)
                {                   
                    int parentGroupID = parent->getGroupID();
                    if(parentGroupID == NG_Bone)
                    {
                        // found a bone child, attach it and recursively call on the bone
                        std::string spriteName = XML::parseString(child, "name");
                        std::string spritePath = XML::parseString(child, "relativePath");
                        
                        int layer = XML::parseInt(child, "localLayer");
                        
                        Sprite* sprite = 
                            new Sprite( *(SpriteManager::getInstancePtr()->getSprite( spritePath ).get()) );
                        sprite->setName( spriteName );

                        
                        skel->addSpriteAsChildToBone(static_cast<Bone*>(parent), sprite, layer);
                     
                        _setNodeProperties(child, sprite);
                    }
                    else
                    {
                        // sprites have to parented to bones in the skeleton structure
                        assert(0);
                    }
                }
                else
                {
                    // sprites need a parent in the skeleton structure
                    assert(0);
                }                
            }
            
            child = child->next;
        }
        
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void Skeleton::_setNodeProperties(xmlNodePtr xmlNode, Node* node)
    {
        if(XML::attrExists(xmlNode, "localPos"))
        {
            node->setLocalPosition2D( XML::parseVector2(xmlNode, "localPos") );
        }
        
        if(XML::attrExists(xmlNode, "localScale"))
        {
            node->setLocalScale2D(XML::parseVector2(xmlNode, "localScale"));
        }
        
        if(XML::attrExists(xmlNode, "localAngleDeg"))
        {
            node->setLocalAngleZ(degToRad(XML::parseFloat(xmlNode, "localAngleDeg")));
        }          
        else if(XML::attrExists(xmlNode, "localAngleRad"))
        {
            node->setLocalAngleZ(XML::parseFloat(xmlNode, "localAngleRad"));
        }
        
        if( XML::attrExists( xmlNode, "localPivot" ) )
        {
            node->setLocalPivot( Vector3( XML::parseVector2( xmlNode, "localPivot") ) );
        }
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void Skeleton::setBindPose()
    {
        std::stack<Node*> nodeStack;
        if(this->mChild)
        {
            nodeStack.push(static_cast<Node*>(this->mChild));
        }
        
        while(!nodeStack.empty())
        {
            Node* node = nodeStack.top();
            nodeStack.pop();
            
            if(node->getGroupID() == NG_Bone)
            {
                Bone* bone = static_cast<Bone*>(node);
                
                BoneLocalData& boneBindPose = mBindPose[bone];
                
                boneBindPose.localPos = bone->getLocalPosition2D();
                boneBindPose.localScale = bone->getLocalScale2D();
                boneBindPose.localAngle = bone->getLocalAngleZ();
            }
            
            if(node->mSibling)
            {
                nodeStack.push(static_cast<Node*>(node->mSibling));
            }
            
            if(node->mChild)
            {
                nodeStack.push(static_cast<Node*>(node->mChild));
            }
        }
    }
    
    // -------------==============-------------==============-------------==============-------------===     
    void Skeleton::setSkeletonToBindPose()
    {
        std::stack<Node*> nodeStack;
        if(this->mChild)
        {
            nodeStack.push(static_cast<Node*>(this->mChild));
        }
        
        while(!nodeStack.empty())
        {
            Node* node = nodeStack.top();
            nodeStack.pop();
            
            if(node->getGroupID() == NG_Bone)
            {
                Bone* bone = static_cast<Bone*>(node);
                
                BoneLocalDataMap::iterator it = mBindPose.find(bone);
                if(it != mBindPose.end())
                {
                    BoneLocalData& boneBindPose = (*it).second;
                        
                    bone->setLocalPosition(boneBindPose.localPos);
                    bone->setLocalScale(boneBindPose.localScale);
                    bone->setLocalAngles(boneBindPose.localAngle);
                }
            }
            
            if(node->mSibling)
            {
                nodeStack.push(static_cast<Node*>(node->mSibling));
            }
            
            if(node->mChild)
            {
                nodeStack.push(static_cast<Node*>(node->mChild));
            }
        }
    }
    
    bool Skeleton::getBindPoseDataForBone(Bone* bone, BoneLocalData& boneLocalData)
    {
        bool ret = true;
        
        BoneLocalDataMap::iterator it = mBindPose.find(bone);
        if(it != mBindPose.end())
        {
            BoneLocalData& boneBindPose = (*it).second;
            
            bone->setLocalPosition(boneBindPose.localPos);
            bone->setLocalScale(boneBindPose.localScale);
            bone->setLocalAngles(boneBindPose.localAngle);
            
        }
        else
        {
            // No bind pose data for that skeleton
            assert(0);
            
            ret = true;
        }
        
        return ret;
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void Skeleton::setColor(const Color& color)
    {
        mColor = color;
        
        SpriteMap::iterator it = mSprites.begin();
        SpriteMap::iterator itEnd = mSprites.end();
        
        while (it != itEnd) 
        {
            (*it).second->setColor(mColor);
            ++it;
        }
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void Skeleton::scaleSpriteScale(const Vector2& scaler)
    {
        SpriteMap::iterator it = mSprites.begin();
        SpriteMap::iterator itEnd = mSprites.end();
        
        while (it != itEnd) 
        {
            Vector2 prevScale = (*it).second->getLocalScale2D();
            prevScale *= scaler;
            (*it).second->setLocalScale2D(prevScale);
            ++it;
        }
    }
	
	// -------------==============-------------==============-------------==============-------------===  
	void Skeleton::setScreenSpace(const bool isScreenSpace)
	{
		// set skeleton
		if(isScreenSpace && getLocalScale().Y > 0)
		{
			Vector2 size = getLocalScale2D();
			setLocalScale2D(Vector2(size.X, -size.Y));
		}
		mIsScreenSpace = isScreenSpace;            
		
		// set animation manager
		mAnimationManager.setScreenSpace(isScreenSpace);
		
		// now tell all of the sprites they are in screen space
		SpriteMap::iterator it = mSprites.begin();
		SpriteMap::iterator itEnd = mSprites.end();
		while( it != itEnd )
		{
			it->second->setScreenSpace( isScreenSpace );
			++it;
		}
	}
    
    // -------------==============-------------==============-------------==============-------------===
    void Skeleton::_skeletonHasLoaded(void* params) //const std::string& path, char* buffer, size_t length, const std::string& actualPath, const std::string& handlerPath, const PropertyList& plist)
    {
		FileManager::ReadFileCallbackParameters* rcp = (FileManager::ReadFileCallbackParameters*)params;
		
		SkeletonLoadedCallbackMap::iterator it;
        const Property* property = rcp->userData.getValueForKey("file_num");
        if(property)
		{
			it = mCallbackMap.find(rcp->path + property->asString());
		}
		else
		{
			it = mCallbackMap.find(rcp->path);
		}
		
		if(it != mCallbackMap.end())
		{
			Skeleton* skel = 0;
			SkeletonLoadedCallbackParameters skeletonParameters(skel);

			if (rcp->result == ResultCode_OK)
			{
				xmlDocPtr doc;
				xmlNodePtr root = XML::loadDocMemory(rcp->buffer, rcp->length, "Skeleton", doc);

				if(root)
				{
					// we loaded the xml doc
					skel = Skeleton::createFromXmlNode(root);
				
					// set position, angle, name
					skel->setName( rcp->userData["name"].asString() );
					skel->setLocalPosition2D( rcp->userData["pos"].asVector2() );
					skel->setLocalAngleZ( rcp->userData["angle"].asFloat() );
					skel->setLocalScale2D( rcp->userData["scale"].asVector2() );
				
					skeletonParameters.skel = skel;
					
					XML::cleanupDoc(doc);
				}
				
				// cleanup the file buffer.
				delete [] rcp->buffer;
			}
			else
			{
				skeletonParameters.result = ResultCode_ErrorFileNotFound;
			}
			
			
			// call the callback with the result.
			it->second->invoke(&skeletonParameters);
			
			
			mCallbackMap.erase(it);
        }
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void Skeleton::_animationsHaveLoaded(void* params)
    {
		FileManager::ReadFileCallbackParameters* rcp = (FileManager::ReadFileCallbackParameters*)params;
		
        if(rcp->result == ResultCode_OK)
        {
            XMLDocument xmlDoc( rcp->buffer, rcp->length, "Animations" );
            
            if( xmlDoc )
            {
                XMLDocument::Iterator docIt = xmlDoc.getRootIterator();
                Property curveProperty;
                if( docIt.getAttribute( "curveCollection", curveProperty ) )
                {
                    setCurveCollectionName( curveProperty.asString() );
                    CurveManager::getManager().loadCurveCollection(getCurveCollectionName());
                }
                
                XMLDocument::NamedIterator child =
                    xmlDoc.getRootIterator().getChildNamedIterator("Animation");
                
                while(child)
                {
                    Animation* ani = Animation::createAnimationFromXmlNode(
                                        XMLDocument::Iterator(child.getCurrentNode()), this );
                    mAnimationManager.addAnimation(ani);
                    
                    ++child;
                }
                
            }
            
            delete [] rcp->buffer;
        }
    }
    
    void Skeleton::_skeletonFileLoaded(void* params)
    {
        FileManager::ReadFileCallbackParameters* rcp =
            (FileManager::ReadFileCallbackParameters*)params;
        
        if( rcp->result == ResultCode_OK )
        {
            xmlDocPtr xmlDoc;
            xmlNodePtr rootNode = XML::loadDocMemory(rcp->buffer, rcp->length, "Skeleton", xmlDoc);
            
            if(rootNode)
            {
                _buildSkeleton( rootNode );

                // free up the xml memory
                XML::cleanupDoc(xmlDoc);
            }
            
            delete [] rcp->buffer;
        }
    }
    
    void Skeleton::drawSkeleton( SpriteBatch& spriteBatch )
    {
        BoneMap::iterator boneIt = mBones.begin();
        BoneMap::iterator boneItEnd = mBones.end();
        
        while (boneIt != boneItEnd)
        {
            spriteBatch.drawQuadColored(99, ((*boneIt).second)->getWorldPosition2D(), ((*boneIt).second)->getWorldAngleZ(), (mIsScreenSpace ? Vector2(5.0f,5.0f) : Vector2(.25f,.25f)), Color::Red);
            ++boneIt;
        }
		
		std::queue<PCSNode*> boneQueue;
        boneQueue.push(this);
		Vector2 parentPos = getWorldPosition2D();
        Vector2 otherPos;
        
        while(!boneQueue.empty())
        {
            PCSNode* node = boneQueue.front();
            boneQueue.pop();
			
			if( node->getGroupID() == NG_Bone )
			{
				Bone* bone = (Bone*)node;
				parentPos = bone->getWorldPosition2D();
            }
			
            PCSNode* child = node->getChild();
            while(child)
            {
				if( child->getGroupID() == NG_Bone )
				{
					Bone* bChild = (Bone*)child;
                    
					// now draw the connections
					otherPos = bChild->getWorldPosition2D();
					spriteBatch.drawLineColored(99, parentPos, otherPos, (mIsScreenSpace ? 2.5f : 0.15f), Color::Red);
                    
				}
				
                boneQueue.push(child);
                child = child->getSibling();
            }
            
        }		

    }
}
