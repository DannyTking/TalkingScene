//
//  Bone.h
//  WalaberEngine
//
//  Created by Jason Pecho on 5/4/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef _BONE_H_
#define _BONE_H_

#include <string>

#include <Node.h>

namespace Walaber
{
    class Skeleton;
    
    class Bone : public Node
    {
    public:// interface
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        //  Default Ctor
        // </Summary>
        Bone():
        Node("Bone", NG_Bone)
        {}
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        //  Name Ctor
        // </Summary>
        Bone(const std::string& name):
        Node(name, NG_Bone)
        {}
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        //  copy Ctor
        // </Summary>
        Bone(const Bone& bone):
        Node(bone)
        {}
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        //  op =
        // </Summary>
        Bone& operator=(const Bone& bone)
        {
            Node::operator=(bone);
            
            return *this;
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        //  Default Ctor
        // </Summary>
        ~Bone()
        {}
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        //  Set the skeleton this bone is a part of
        // </Summary>
        void setSkeleton(Skeleton* skeleton);
        
    private:// members
        // we can add constrainst to rotation etc later if we'd like
        Skeleton*           mSkeleton;
    };
}

#endif