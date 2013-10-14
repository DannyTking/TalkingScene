//
//  Skeleton.h
//  WalaberEngine
//
//  Created by Jason Pecho on 5/3/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef _SKELETON_H_
#define _SKELETON_H_

#include <string>
#include <map>
#include <vector>

#include <DrawableNode.h>
#include <SkeletonAnimationManager.h>
#include <XMLHelper.h>
#include <Callback.h>

namespace Walaber
{
    class Bone;
    class Sprite;
    class SpriteBatch;
    
    class Camera;
    
    class Skeleton : public DrawableNode
    {
    public: //typedefs, structs, and enums
        
        struct BoneLocalData
        {
            Vector3 localPos;
            Vector3 localScale;
            Vector3 localAngle;
            
            BoneLocalData():
            localPos(),
            localScale(),
            localAngle(0.0f)
            {}
            
            BoneLocalData(const Vector3& pLocalPos, const Vector3& pLocalScale, const Vector3 pLocalAngle):
            localPos(pLocalPos),
            localScale(pLocalScale),
            localAngle(pLocalAngle)
            {}
        };
        
        struct SkeletonLoadedCallbackParameters
        {
			ResultCode result;
            Skeleton* skel;
            
            SkeletonLoadedCallbackParameters():
			result(ResultCode_OK),
            skel(0)
            {}
            
            SkeletonLoadedCallbackParameters(Skeleton* skeleton):
			result(ResultCode_OK),
            skel(skeleton)
            {}
        };
        
        typedef std::map< Bone*, BoneLocalData >    BoneLocalDataMap;
        typedef std::map< std::string, Bone* >      BoneMap;
        typedef std::map< std::string, Sprite* >    SpriteMap;
        typedef std::map< Sprite*, int >            SpriteToRelativeLayerMap;
        
        typedef std::multimap< std::string, CallbackPtr >    SkeletonLoadedCallbackMap;
        
    public:// interface
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        //  Default Ctor
        // </Summary>
        Skeleton();
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Name and Group Ctor
        // </Summary>
        Skeleton(const std::string& name, const int groupID = NG_Skeleton); 
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // positional ctor
        // </Summary>
        Skeleton(const std::string& name, const Vector2& pos, const float angle,
                 const Vector2& scale);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Copy Ctor
        // </Summary>
        Skeleton(const Skeleton& skeleton);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // op =
        // </Summary>
        Skeleton& operator=(const Skeleton& skeleton);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Dtor
        // </Summary>
        ~Skeleton();
       
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // creates a skeleton given an xmlnode ptr to the root of the xml entry
        // </Summary>
        static Skeleton* createFromXmlNode(xmlNodePtr root);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // creates a skeleton given an xml file asynchronously and calls back to provide skeleton*
        // </Summary>
		static void loadSkeletonWithCallback(const std::string& xmlPath, CallbackPtr callback )
		{
			std::string name = "NONE";
			loadSkeletonWithCallback( xmlPath, callback, Vector2(0.0f, 0.0f), 0.0f,
                                     Vector2(1.0f, 1.0f), name );
		}
		
        static void loadSkeletonWithCallback(const std::string& xmlPath, CallbackPtr callback,
											 const Vector2& pos, const float angle,
                                             const Vector2& scale, const std::string& name);
        
        /*!
         Loads data from an xml file to initialize the skeleton
         @param xmlPath path to the xml file containing the skeleton data
         @todo: add callback functionality
        */
        void loadFromXMLFile( const std::string& xmlPath );
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // the the layer of the skeleton and all of its child sprites
        // </Summary>
        void setLayer(const int layer);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // update sprites
        // </Summary>
        void update(float deltaTime);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // draw sprites through the sprite batch
        // </Summary>
        void draw(SpriteBatch* sb);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // add sprite to bone as child at layer. Only one sprite per layer is allowed (based on flash limitations)
        // </Summary>
        void addSpriteAsChildToBone(Bone* bone, Sprite* sprite, const int spriteLayer);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // adds a root bone to the skeleton and set its local transformation as the bind pose for that bone
        // </Summary>
        void addRootBone(Bone* bone);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // add childBone as a child of parentBone and set its local transformation as the bind pose for that bone
        // </Summary>
        void addChildBone(Bone* parentBone, Bone* childBone);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // returns a bone by name
        // </Summary>
        Bone* getBone(const std::string& name);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // returns a sprite by name
        // </Summary>
        Sprite* getSprite(const std::string& name);  
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // get either bone ore sprite by name, of bone and sprite have same name, prefer bone
        // </Summary>
        Node* getNode(const std::string& name);  
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // get animation manager to control animations
        // </Summary>
        SkeletonAnimationManager* getAnimationManager()
        {
            return &mAnimationManager;
        }
        
        /*
         Loop over the sprites and tell them to play
         */
        void playSpriteAnimations();
        
        /*
         Loop over the sprites and tell them to pause
         */
        void pauseSpriteAnimations();
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // loadAnimation
        // </Summary>
        void loadAnimationsFromXml( const std::string& xmlPath,
                                    const AnimationManager::AnimationLoadBehavior loadBehavior );
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // goes through each bone and sets its local transformation as the bind pose
        // </Summary>        
        void setBindPose();
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // sets each bones local transformations to their bind pose transformations
        // </Summary>        
        void setSkeletonToBindPose();
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // returns the local data struct for a bone's bind pose transformation
        // </Summary>    
        bool getBindPoseDataForBone(Bone* bone, BoneLocalData& boneLocalData);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set the bledning color on all connected sprites
        // </Summary>    
        void setColor(const Color& color);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set the local scale of all sprites (sprite sizes are based in pixel space)
        // </Summary>    
        void scaleSpriteScale(const Vector2& scaler);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // calling this tells the skeleton and animation manager that it will be animated in screen space where down is positive Y and angles are reversed
        // </Summary>    
        void setScreenSpace(const bool isScreenSpace);
		
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // returns the name of the curve collection if the animation uses one
        // </Summary>  
		std::string getCurveCollectionName()
		{
			return mCurveCollection;
		}
		
		void setCurveCollectionName(const std::string& curveCollection)
		{
			mCurveCollection = curveCollection;
		}
		
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // returns the name of the curve collection if the animation uses one
        // </Summary>
		std::string getSpriteCollectionName()
		{
			return mSpriteCollection;
		}
		
		void setSpriteCollectionName(const std::string& spriteCollection)
		{
			mSpriteCollection = spriteCollection;
		}
        
		// CALLBACKS
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // read file call back for xml loading
        // </Summary>
        static void _skeletonHasLoaded(void* params); //const std::string& path, char* buffer, size_t length, const std::string& actualPath, const std::string& handlerPath, const PropertyList& plist);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // read file call back for xml animationloading, on a per skeleton basis (no static)
        // </Summary>
        void _animationsHaveLoaded(void* params); //const std::string& path, char* buffer, size_t length, const std::string& actualPath, const std::string& handlerPath, const PropertyList& plist);
        
        void _skeletonFileLoaded(void* params);
		
		void printBoneNames()
		{
			PCSNode::printTree(this);
		}
        
        /*!
         Used to draw the actual skeleton, bone to bone
         */
        void drawSkeleton( SpriteBatch& spriteBatch );
        
        void resetSkeleton();
        
    private: // interface
        
        void _buildSkeleton( xmlNodePtr rootNode );
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // recursively find children and build a skeleton
        // </Summary>
        static void _recursiveBuildSkeleton(xmlNodePtr parentXML, Skeleton* skel, Node* parent);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // looks for common node properties in xml and sets them if found
        // </Summary>
        static void _setNodeProperties(xmlNodePtr xmlNode, Node* node);
        
        
    private:// members
    
        BoneLocalDataMap                    mBindPose;
        BoneMap                             mBones;
        SpriteMap                           mSprites;    
        SpriteToRelativeLayerMap            mSpriteRelativeLayers;
        SkeletonAnimationManager            mAnimationManager;
        Color                               mColor;
		std::string							mCurveCollection;
        std::string                         mSpriteCollection;
        bool                                mIsScreenSpace;
        
        static SkeletonLoadedCallbackMap           mCallbackMap;
    };
    
    MAKE_SHARED_PTR(Skeleton);
}

#endif
