//
//  AnimationBinary.h
//  WalaberEngine
//
//  Created by Pecho, Jason on 5/14/13.
//
//

#ifndef _ANIMATION_BINARY_H_
#define _ANIMATION_BINARY_H_

namespace Walaber
{
    namespace AnimationBinary
    {
        const static int MAGIC          =       0xC081DA11;
        const static int VERSION        =       1;
        
        struct AnimationBinaryFileHeader
        {
            const int                   mMagic;
            const unsigned short        mVersion;
            unsigned short              mFlags;
            unsigned int                mNumAnimations;
            
            AnimationBinaryFileHeader():
            mMagic( MAGIC ),
            mVersion( VERSION ),
            mFlags( 0 ),
            mNumAnimations( 0 )
            {}
        };
        
//        enum NodeAnimationFlags
//        {
//            NodeAnimationFlags_PosX             =   ( 1  <<   0 ),
//            NodeAnimationFlags_PosY             =   ( 1  <<   1 ),
//            NodeAnimationFlags_PosZ             =   ( 1  <<   2 ),
//            NodeAnimationFlags_ScaleX           =   ( 1  <<   3 ),
//            NodeAnimationFlags_ScaleY           =   ( 1  <<   4 ),
//            NodeAnimationFlags_ScaleZ           =   ( 1  <<   5 ),
//            NodeAnimationFlags_Angle            =   ( 1  <<   6 ),
//            NodeAnimationFlags_AngleX           =   ( 1  <<   7 ),
//            NodeAnimationFlags_AngleY           =   ( 1  <<   8 ),
//            NodeAnimationFlags_AngleZ           =   ( 1  <<   9 ),
//            NodeAnimationFlags_PivotX           =   ( 1  <<  10 ),
//            NodeAnimationFlags_PivotY           =   ( 1  <<  11 ),
//            NodeAnimationFlags_PivotZ           =   ( 1  <<  12 ),
//            NodeAnimationFlags_SkewGradient     =   ( 1  <<  13 ),
//        };
//        
        enum NodeAnimationTracks
        {
            NodeAnimationTracks_INVALID = -1,
            NodeAnimationTracks_PosX = 0,
            NodeAnimationTracks_PosY,
            NodeAnimationTracks_PosZ,
            NodeAnimationTracks_ScaleX,
            NodeAnimationTracks_ScaleY,
            NodeAnimationTracks_ScaleZ,
            NodeAnimationTracks_Angle, // this is actually Z, since we don't export all the angles
            NodeAnimationTracks_AngleX,
            NodeAnimationTracks_AngleY,
            NodeAnimationTracks_AngleZ,
            NodeAnimationTracks_PivotX,
            NodeAnimationTracks_PivotY,
            NodeAnimationTracks_PivotZ,
            NodeAnimationTracks_SkewGradient,
            NodeAnimationTracks_SIZE
        };
        
        const std::string NodeAnimationTracksAttributes[] = {
            "posXRelCurvePath",
            "posYRelCurvePath",
            "posZRelCurvePath",
            "scaleXRelCurvePath",
            "scaleYRelCurvePath",
            "scaleZRelCurvePath",
            "angleRelCurvePath",
            "angleXRelCurvePath",
            "angleYRelCurvePath",
            "angleZRelCurvePath",
            "pivotXRelCurvePath",
            "pivotYRelCurvePath",
            "pivotZRelCurvePath",
            "skewRelCurvePath",
        };
        
        const std::string NodeAnimationCurveName[] = {
            "_posX.xml",
            "_posY.xml",
            "_posZ.xml",
            "_scaleX.xml",
            "_scaleY.xml",
            "_scaleZ.xml",
            "_angle.xml",
            "_angleX.xml",
            "_angleY.xml",
            "_angleZ.xml",
            "_pivotX.xml",
            "_pivotY.xml",
            "_pivotZ.xml",
            "_skew.xml"
        };
    }
}

#endif // defined ( _ANIMATION_BINARY_H_ )
