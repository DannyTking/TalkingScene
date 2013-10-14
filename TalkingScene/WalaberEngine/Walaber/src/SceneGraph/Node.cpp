;//
//  Node.cpp
//  WalaberEngine
//
//  Created by Jason Pecho on 4/26/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "Node.h"

#include "VectorTools.h"

namespace Walaber
{
    // -------------==============-------------==============-------------==============-------------===
    Node::Node():
    PCSNode("defaultNode"),
    mLocalPosition(),
    mWorldPosition(),
    mLocalPivot(),
    mLocalScale(Vector3::One),
    mWorldScale(Vector3::One),
    mLocalAngles(),
    mWorldAngles(),
    mLocalTransformation(),
    mWorldTransformation(),
    mLocalSkew( 0.0f ),
    mLocalTransformDirty(false),
    mWorldTransformDirty(false),
    mWorldPositionDirty(false),
    mWorldScaleDirty(false),
    mWorldAngleDirty(false)
    {}
    
    // -------------==============-------------==============-------------==============-------------===
    Node::Node(const std::string& name, const int groupId ):
    PCSNode(name, groupId),
    mLocalPosition(),
    mWorldPosition(),
    mLocalPivot(),
    mLocalScale(Vector3::One),
    mWorldScale(Vector3::One),
    mLocalAngles(),
    mWorldAngles(),
    mLocalTransformation(),
    mWorldTransformation(),
    mLocalSkew( 0.0f ),
    mLocalTransformDirty(false),
    mWorldPositionDirty(false),
    mWorldScaleDirty(false),
    mWorldAngleDirty(false)
    {
        
    }
    
    // -------------==============-------------==============-------------==============-------------===
    Node::Node(const Node& node):
    PCSNode(node),
    mLocalPosition(node.mLocalPosition),
    mWorldPosition(node.mWorldPosition),
    mLocalPivot(node.mLocalPivot),
    mLocalScale(node.mLocalScale),
    mWorldScale(node.mWorldScale),
    mLocalAngles(node.mLocalAngles),
    mWorldAngles(node.mWorldAngles),
    mLocalTransformation(node.mLocalTransformation),
    mWorldTransformation(node.mWorldTransformation),
    mLocalSkew(node.mLocalSkew),
    mLocalTransformDirty(node.mLocalTransformDirty),
    mWorldTransformDirty(node.mWorldTransformDirty),
    mWorldPositionDirty(node.mWorldPositionDirty),
    mWorldScaleDirty(node.mWorldScaleDirty),
    mWorldAngleDirty(node.mWorldAngleDirty)
    {
        
    }
    
    // -------------==============-------------==============-------------==============-------------===
    Node& Node::operator=(const Node& node)
    {
        PCSNode::operator=(node);
        mLocalPosition = node.mLocalPosition;
        mWorldPosition = node.mWorldPosition;
        mLocalPivot = node.mLocalPivot;
        mLocalScale = node.mLocalScale;
        mWorldScale = node.mWorldScale;
        mLocalAngles = node.mLocalAngles;
        mWorldAngles = node.mWorldAngles;
        mLocalTransformation = node.mLocalTransformation;
        mWorldTransformation = node.mWorldTransformation;
        mLocalSkew = node.mLocalSkew;
        mLocalTransformDirty = node.mLocalTransformDirty;
        mWorldTransformDirty = node.mWorldTransformDirty;
        mWorldPositionDirty = node.mWorldPositionDirty;
        mWorldScaleDirty = node.mWorldScaleDirty;
        mWorldAngleDirty = node.mWorldAngleDirty;
        
        return *this;
    }
    
    // -------------==============-------------==============-------------==============-------------===
    Node::~Node()
    {}
    
    // -------------==============-------------==============-------------==============-------------===
    void Node::setLocalPosition2D(const Vector2& pos)
    {
        mLocalPosition.X = pos.X;
        mLocalPosition.Y = pos.Y;
        
        setWorldPosDirty(true);
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void Node::setLocalPosition(const Vector3& pos)
    {
        mLocalPosition = pos;
        
        setWorldPosDirty(true);
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void Node::setLocalPivot(const Vector3& pivot)
    {
        mLocalPivot = pivot;
    }    
    
    // -------------==============-------------==============-------------==============-------------===
    void Node::setLocalScale2D(const Vector2& scale)
    {
        mLocalScale.X = scale.X;
        mLocalScale.Y = scale.Y;
        
        setWorldScaleDirty(true);
        
        Node* child = static_cast<Node*>(mChild);
        while(child)
        {
            child->setWorldPosDirty(true);
            child = static_cast<Node*>(child->mSibling);         
        } 
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void Node::setLocalScale(const Vector3& scale)
    {
        mLocalScale = scale;
        
        setWorldScaleDirty(true);
        
        Node* child = static_cast<Node*>(mChild);
        while(child)
        {
            child->setWorldPosDirty(true);
            child = static_cast<Node*>(child->mSibling);
        }
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void Node::setLocalAngleZ(const float& radAngle)
    {       
        mLocalAngles.Z = radAngle;
        
        setWorldAngleDirty(true);
        
        Node* child = static_cast<Node*>(mChild);
        while(child)
        {
            child->setWorldPosDirty(true);
            child = static_cast<Node*>(child->mSibling);         
        } 
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void Node::setLocalAngles(const Vector3& eulerAnglesRadians)
    {
        mLocalAngles = eulerAnglesRadians;
        
        setWorldAngleDirty(true);
        
        Node* child = static_cast<Node*>(mChild);
        while(child)
        {
            child->setWorldPosDirty(true);
            child = static_cast<Node*>(child->mSibling);
        }
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void Node::setLocalSkew( const float skew )
    {
        mLocalSkew = skew;
        
        mLocalTransformDirty = mWorldTransformDirty = true;
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void Node::applyPositionOffset2D(const Vector2& offset)
    {
        mLocalPosition.X += offset.X;
        mLocalPosition.Y += offset.Y;
        
        setWorldPosDirty(true);
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void Node::applyPositionOffset(const Vector3& offset)
    {
        mLocalPosition += offset;
        
        setWorldPosDirty(true);
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void Node::applyPivotOffset(const Vector3& offset)
    {
        mLocalPivot += offset;
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void Node::applyScaleOffset2D(const Vector2& offset)
    {
        mLocalScale.X += offset.X;
        mLocalScale.Y += offset.Y;
        
        setWorldScaleDirty(true);
        
        Node* child = static_cast<Node*>(mChild);
        while(child)
        {
            child->setWorldPosDirty(true);
            child = static_cast<Node*>(child->mSibling);         
        } 
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void Node::applyScaleOffset(const Vector3& offset)
    {
        mLocalScale += offset;
        
        setWorldScaleDirty(true);
        
        Node* child = static_cast<Node*>(mChild);
        while(child)
        {
            child->setWorldPosDirty(true);
            child = static_cast<Node*>(child->mSibling);
        }
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void Node::applyAngleOffsetZ(const float offset)
    {
        mLocalAngles.Z += offset;
        
        setWorldAngleDirty(true);
        
        Node* child = static_cast<Node*>(mChild);
        while(child)
        {
            child->setWorldPosDirty(true);
            child = static_cast<Node*>(child->mSibling);         
        } 
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void Node::applyAnglesOffset(const Vector3& eulerAngleRadiansOffset)
    {
        mLocalAngles += eulerAngleRadiansOffset;
        
        setWorldAngleDirty(true);
        
        Node* child = static_cast<Node*>(mChild);
        while(child)
        {
            child->setWorldPosDirty(true);
            child = static_cast<Node*>(child->mSibling);
        }
    }
    
    void Node::applySkewOffset(const float skewOffset)
    {
        mLocalSkew += skewOffset;
        
        mLocalTransformDirty = mWorldTransformDirty = true;
    }
    
    // -------------==============-------------==============-------------==============-------------===
    Vector2 Node::getWorldPosition2D()
    {
        Vector3 worldPos = getWorldPosition();
        
        return Vector2( worldPos );
    }
    
    // -------------==============-------------==============-------------==============-------------===
    Vector3 Node::getWorldPosition()
    {
        if(mWorldPositionDirty)
        {
            if(mParent)
            {
                mWorldPosition = static_cast<Node*>(mParent)->transformPoint( mLocalPosition );
            }
            else
            {
                mWorldPosition = mLocalPosition;
            }
            
            mWorldPositionDirty = false;
        }
        
        return mWorldPosition;
    }
    
    // -------------==============-------------==============-------------==============-------------===
    Vector2 Node::getWorldScale2D()
    {
        Vector3 worldScale = getWorldScale();
        
        return Vector2 ( worldScale );
    }
    
    // -------------==============-------------==============-------------==============-------------===
    Vector3 Node::getWorldScale()
    {
        if(mWorldScaleDirty)
        {
            Vector3 parentScale = Vector3::One;
            if(mParent)
            {
                parentScale = static_cast<Node*>(mParent)->getWorldScale();
            }
            mWorldScale = parentScale * mLocalScale;
            
            mWorldScaleDirty = false;
        }
        
        return mWorldScale;
    }
    
    // -------------==============-------------==============-------------==============-------------===
    float Node::getWorldAngleZ()
    {        
        return getWorldAngles().Z;
    }

    // -------------==============-------------==============-------------==============-------------===
    Vector3 Node::getWorldAngles()
    {
        if(mWorldAngleDirty)
        {
            Vector3 parentAngles;
            if(mParent)
            {
                parentAngles = static_cast<Node*>(mParent)->getWorldAngles();
            }
            mWorldAngles = parentAngles + mLocalAngles;
            
            mWorldAngleDirty = false;
        }
        
        return mWorldAngles;
    }
    
    // -------------r============-------------==============-------------==============-------------===
    
    const Matrix3& Node::getLocalTransformation()
    {        
        if( mLocalTransformDirty )
        {
            mLocalTransformation = Matrix3::rotationZYX( mLocalAngles ) *
                Matrix3::skewGradient( mLocalSkew ) * Matrix3::scale( mLocalScale );
            
            mLocalTransformDirty = false;            
        }
        return mLocalTransformation;
    }
    
    // -------------==============-------------==============-------------==============-------------===
    
    const Matrix3& Node::getWorldTransformation()
    {
        if (mWorldTransformDirty)
        {
            if( mParent )
            {
                const Matrix3& parentMatrix = static_cast<Node*>(mParent)->getWorldTransformation();
                
                if (mWorldTransformDirty)
                {
                    mWorldTransformation = parentMatrix * getLocalTransformation();
                    mWorldTransformDirty = false;
                }
            }
            else
            {
                mWorldTransformation = getLocalTransformation();
                mWorldTransformDirty = false;
            }
        }
        
        return mWorldTransformation; 
    }
    
    // -------------==============-------------==============-------------==============-------------===
    Vector2 Node::transformVector(const Vector2& vector)
    {
        return Vector2( getWorldTransformation() * Vector3( vector ) );
    }
    
    // -------------==============-------------==============-------------==============-------------===
    Vector3 Node::transformVector(const Vector3& vector)
    {
        return ( getWorldTransformation() * vector );
    }
        
    // -------------==============-------------==============-------------==============-------------===
    Vector2 Node::transformPoint(const Vector2& point)
    {
        return Vector2( getWorldPosition() + getWorldTransformation() * ( Vector3( point ) ) );
    }
    
    // -------------==============-------------==============-------------==============-------------===
    Vector3 Node::transformPoint(const Vector3& point)
    {
        const Matrix3& localTransform = getLocalTransformation();
        
        Vector3 p = point - mLocalPivot;
        p = localTransform * p;
        p = p + mLocalPivot;
        
        if( mParent )
        {
            const Matrix3& parentTransform = static_cast<Node*>(mParent)->getWorldTransformation();
            
            p = getWorldPosition() + (parentTransform * p);
        }
        else
        {
            p = getWorldPosition() + p;
        }
        
        return p;
    }
    
    // -------------==============-------------==============-------------==============-------------===
    Vector2 Node::inverseTransformVector(const Vector2& vector)
    {
        Matrix3 inverse = getWorldTransformation();
        inverse.invert();

        return Vector2( inverse * Vector3( vector ) );
    }
        
    // -------------==============-------------==============-------------==============-------------===
    Vector2 Node::inverseTransformPoint(const Vector2& point)
    {
        Matrix3 inverse = getWorldTransformation();
        inverse.invert();
            
        return Vector2( inverse * ( Vector3( point ) - getWorldPosition() ) );
    }
     
}