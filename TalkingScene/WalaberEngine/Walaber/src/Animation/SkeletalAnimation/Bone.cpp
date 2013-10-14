//
//  Bone.cpp
//  WalaberEngine
//
//  Created by Jason Pecho on 5/4/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "Bone.h"

#include <Skeleton.h>

namespace Walaber
{
    void Bone::setSkeleton(Skeleton* skeleton)
    {            
        mSkeleton = skeleton;
    }
}