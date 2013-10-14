//
//  Node.h
//  WalaberEngine
//
//  Created by Jason Pecho on 4/26/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef _NODE_H_
#define _NODE_H_

#include <PCSNode.h>
#include <Vector2.h>
#include <Vector3.h>
#include <Matrix3.h>
#include <cstdlib>

namespace Walaber
{
    class Node : public PCSNode
    {
    public:// enums, structs, and typedefs
        enum NodeGroup
        {
            NG_None = -1,
            NG_Sprite,
            NG_Skeleton,
            NG_Bone,
            NG_Camera,
            NG_SIZE
        };
        
    public:// interface
        /*!
         default ctor 
        */
        Node();
        
        /*!
         Name Ctor
        */
        Node(const std::string& name, const int groupId = -1);
        
        /*!
         Copy ctor
        */
        Node(const Node& node);
        
        /*!
         op =
        */
        Node& operator=(const Node& node);
        
        /*!
         Dtor
        */
        virtual ~Node();
        
        /*!
         local position setter
        */
        virtual void setLocalPosition2D(const Vector2& pos);
        
        /*!
         local position setter
        */
        virtual void setLocalPosition(const Vector3& pos);
        
        /*!
         local pivot setter
         */
        virtual void setLocalPivot(const Vector3& pivot);
        
        /*!
         local scale setter
        */
        virtual void setLocalScale2D(const Vector2& scale);
        
        /*!
         local scale setter
        */
        virtual void setLocalScale(const Vector3& scale);
        
        /*!
         local rotation setter
        */
        virtual void setLocalAngleZ(const float& radAngle);
        
        /*!
         local rotation setter
        */
        virtual void setLocalAngles(const Vector3& eulerAnglesRadians);
        
        /*!
         Set skew as a single value
         */
        virtual void setLocalSkew( const float skew );
        
        /*!
         adds offset to local position
        */
        virtual void applyPositionOffset2D(const Vector2& offset);
        
        /*!
         adds offset to local position
        */
        virtual void applyPositionOffset(const Vector3& offset);
        
        /*!
         add offset to local pivot
         */
        virtual void applyPivotOffset(const Vector3& offset);
        
        /*!
         adds offset to local scale
        */
        virtual void applyScaleOffset2D(const Vector2& offset);
        
        /*!
         adds offset to local scale
        */
        virtual void applyScaleOffset(const Vector3& offset);
        
        /*!
         adds offset to local angle
        */
        virtual void applyAngleOffsetZ(const float offset);
        
        /*!
         adds offset to local angle
        */
        virtual void applyAnglesOffset(const Vector3& eulerAngleRadiansOffset);
        
        /*!
         adds offset to local skew
         */
        virtual void applySkewOffset(const float skewOffset);
        
        /*!
         World dirty pos setter
        */        
        inline void setWorldPosDirty(const bool dirty)
        {
            mWorldPositionDirty = dirty;
            
            Node* child = static_cast<Node*>(mChild);
            while(child)
            {
                if( child->mWorldPositionDirty != dirty )
                {
                    child->setWorldPosDirty(dirty);
                }
                
                child = static_cast<Node*>(child->mSibling);
            }  
        }
        
        /*!
         World dirty scale setter
        */        
        inline void setWorldScaleDirty(const bool dirty)
        {
            mWorldScaleDirty = dirty;
            mLocalTransformDirty = mWorldTransformDirty = dirty;
            
            Node* child = static_cast<Node*>(mChild);
            while(child)
            {
                child->setWorldScaleDirty(dirty);
                child = static_cast<Node*>(child->mSibling);
            }  
        }
        
        /*!
         World dirty rot setter
        */        
        inline void setWorldAngleDirty(const bool dirty)
        {
            mWorldAngleDirty = dirty;
            mLocalTransformDirty = mWorldTransformDirty = dirty;
            
            Node* child = static_cast<Node*>(mChild);
            while(child)
            {
                child->setWorldAngleDirty(dirty);
                child = static_cast<Node*>(child->mSibling);
            }          
        }
        
        /*!
         local position getter
        */
        Vector2 getLocalPosition2D() const
        {
            return Vector2( mLocalPosition.X, mLocalPosition.Y );
        }
        
        /*!
         local position getter
        */
        Vector3 getLocalPosition() const
        {
            return mLocalPosition;
        }
        
        /*!
         local scale getter
        */
        Vector2 getLocalScale2D() const
        {
            return Vector2( mLocalScale.X, mLocalScale.Y );
        }
        
        /*!
         local scale getter
        */
        Vector3 getLocalScale() const
        {
            return mLocalScale;
        }
        
        /*!
         local rotation getter
        */
        float getLocalAngleZ() const
        {
            return mLocalAngles.Z;
        }
        
        /*!
         local rotation getter
        */
        Vector3 getLocalAngles() const
        {
            return mLocalAngles;
        }
        
        /*!
         world position getter
        */
        Vector2 getWorldPosition2D();
        
        /*!
         world position getter
        */
        Vector3 getWorldPosition();
        
        /*!
         world scale getter
        */
        Vector2 getWorldScale2D();
        
        /*!
         world scale getter
        */
        Vector3 getWorldScale();
        
        /*!
         world rotation getter
        */
        float getWorldAngleZ();
        
        /*!
         world rotation getter
        */
        Vector3 getWorldAngles();

        /*!
         Local transformation getter
        */
        const Matrix3& getLocalTransformation();
        
        /*!
         World transformation getter
        */
        const Matrix3& getWorldTransformation();
        
        /*!
         Get the skew of the node as a singlue value
        */
        float getLocalSkew() const
        {
            return mLocalSkew;
        }
        
        /*!
         given a local vector, return that vector in world space, taking our world scale, and 
         angle into account.
        */        
        Vector2 transformVector(const Vector2& vector);
        
        /*!
         given a local vector, return that vector in world space, taking our world scale, and 
         angle into account.
        */
        Vector3 transformVector(const Vector3& vector);
        
        /*!
         given a local point, return that point in world space, taking our world position, scale, 
         and angle into account.
        */        
        Vector2 transformPoint(const Vector2& point);
        
        /*!
         given a local point, return that point in world space, taking our world position, scale, 
         and angle into account.
        */
        Vector3 transformPoint(const Vector3& point);
        
        /*!
         given a world-space vector, return that vector in local space, taking our world scale, 
         and angle into account.
        */        
        Vector2 inverseTransformVector(const Vector2& vector);
        
        /*!
         given a world-space vector, return that vector in local space, taking our world scale, 
         and angle into account.
        */
        Vector3 inverseTransformVector(const Vector3& vector);
        
        /*!
         given a world point, return that point in local space, taking our world position, scale, 
         and angle into account.
        */        
        Vector2 inverseTransformPoint(const Vector2& point);
        
        /*!
         given a world point, return that point in local space, taking our world position, scale, 
         and angle into account.
        */
        Vector3 inverseTransformPoint(const Vector3& point);
        
        
    protected:// members
        Vector3     mLocalPosition;
        Vector3     mWorldPosition;
        
        Vector3     mLocalPivot;
        
        Vector3     mLocalScale;
        Vector3     mWorldScale;
        
        Vector3     mLocalAngles; // radians
        Vector3     mWorldAngles; // radians
        
        Matrix3     mLocalTransformation;
        Matrix3     mWorldTransformation;
        
        float       mLocalSkew;
        
        bool        mLocalTransformDirty;
        bool        mWorldTransformDirty;
        bool        mWorldPositionDirty;
        bool        mWorldScaleDirty;
        bool        mWorldAngleDirty;
        
        friend class Skeleton;
    };
}

#endif
