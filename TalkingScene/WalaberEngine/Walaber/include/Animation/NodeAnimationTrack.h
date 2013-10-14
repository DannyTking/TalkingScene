//
//  NodeAnimationTrack.h
//  WalaberEngine
//
//  Created by Jason Pecho on 5/5/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef _NODE_ANIMATION_TRACK_H_
#define _NODE_ANIMATION_TRACK_H_

#include <Curve.h>
#include <WalaberMath.h>
#include <AnimationSharedDefinitions.h>

namespace Walaber
{
    class Node;
    class Animation;
    
    class NodeAnimationTrack
    {
    public:// typedefs, enums, structs
        
        enum TrackBitFlag
        {
            TBF_Position = 1,
            TBF_Scale = 2,
            TBF_Angle = 4,
            TBF_SkewGradient = 8,
            TBF_Pivot = 16,
        };
        
    public:// interface
                
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Default Ctor
        // </Summary>
        NodeAnimationTrack();
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // init with node track is animating
        // </Summary>
        explicit NodeAnimationTrack(Node* node);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // copy ctor
        // </Summary>
        NodeAnimationTrack(const NodeAnimationTrack& nodeAnimationTrack);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // op=
        // </Summary>
        NodeAnimationTrack& operator=(const NodeAnimationTrack& nodeAnimationTrack);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Dtor
        // </Summary>
        ~NodeAnimationTrack();
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set curves for local pos
        // </Summary>
        void setAnimation(Animation* animation);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set curves for local pos
        // </Summary>
        void setCurvePosition( CurvePtr curve_localPosX, CurvePtr curve_localPosY,
                       CurvePtr curve_localPosZ )
        {
            // set bit flags
            mTrackBitFlags |= TBF_Position;
            
            mCurve_LocalPositionX = curve_localPosX;
            mCurve_LocalPositionY = curve_localPosY;
            mCurve_LocalPositionZ = curve_localPosZ;
            
            _setTrackLength();
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set curves for local pos
        // </Summary>
        void setCurvePositionX( CurvePtr curve_localPosX )
        {
            // set bit flags
            mTrackBitFlags |= TBF_Position;
            
            mCurve_LocalPositionX = curve_localPosX;
            
            _setTrackLength();
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set curves for local pos
        // </Summary>
        void setCurvePositionY( CurvePtr curve_localPosY )
        {
            // set bit flags
            mTrackBitFlags |= TBF_Position;
            
            mCurve_LocalPositionY = curve_localPosY;
            
            _setTrackLength();
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set curves for local pos
        // </Summary>
        void setCurvePositionZ( CurvePtr curve_localPosZ )
        {
            // set bit flags
            mTrackBitFlags |= TBF_Position;
            
            mCurve_LocalPositionZ = curve_localPosZ;
            
            _setTrackLength();
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set curves for local scale
        // </Summary>
        void setCurveScale( CurvePtr curve_localScaleX, CurvePtr curve_localScaleY,
                            CurvePtr curve_localScaleZ )
        {
            // set bit flags
            mTrackBitFlags |= TBF_Scale;
            
            mCurve_LocalScaleX = curve_localScaleX;
            mCurve_LocalScaleY = curve_localScaleY;
            mCurve_LocalScaleZ = curve_localScaleZ;
            
            _setTrackLength();
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set curves for local scale
        // </Summary>
        void setCurveScaleX( CurvePtr curve_localScaleX )
        {
            // set bit flags
            mTrackBitFlags |= TBF_Scale;
            
            mCurve_LocalScaleX = curve_localScaleX;
            
            _setTrackLength();
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set curves for local scale
        // </Summary>
        void setCurveScaleY( CurvePtr curve_localScaleY )
        {
            // set bit flags
            mTrackBitFlags |= TBF_Scale;
            
            mCurve_LocalScaleY = curve_localScaleY;
            
            _setTrackLength();
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set curves for local scale
        // </Summary>
        void setCurveScaleZ( CurvePtr curve_localScaleZ )
        {
            // set bit flags
            mTrackBitFlags |= TBF_Scale;
            
            mCurve_LocalScaleZ = curve_localScaleZ;
            
            _setTrackLength();
        }

        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set curve for local angle
        // </Summary>
        void setCurveAngle( CurvePtr curve_localAngleX, CurvePtr curve_localAngleY,
                            CurvePtr curve_localAngleZ )
        {
            // set bit flags
            mTrackBitFlags |= TBF_Angle;
            
            mCurve_LocalAngleX = curve_localAngleX;
            mCurve_LocalAngleY = curve_localAngleY;
            mCurve_LocalAngleZ = curve_localAngleZ;
            
            _setTrackLength();
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set curve for local angle
        // </Summary>
        void setCurveAngleX( CurvePtr curve_localAngleX )
        {
            // set bit flags
            mTrackBitFlags |= TBF_Angle;
            
            mCurve_LocalAngleX = curve_localAngleX;
            
            _setTrackLength();
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set curve for local angle
        // </Summary>
        void setCurveAngleY( CurvePtr curve_localAngleY )
        {
            // set bit flags
            mTrackBitFlags |= TBF_Angle;
            
            mCurve_LocalAngleY = curve_localAngleY;
            
            _setTrackLength();
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set curve for local angle
        // </Summary>
        void setCurveAngleZ( CurvePtr curve_localAngleZ )
        {
            // set bit flags
            mTrackBitFlags |= TBF_Angle;
            
            mCurve_LocalAngleZ = curve_localAngleZ;
            
            _setTrackLength();
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set curve for local angle
        // </Summary>
        void setCurveSkewGradient( CurvePtr curve_skewGradient )
        {
            // set bit flags
            mTrackBitFlags |= TBF_SkewGradient;
            
            mCurve_SkewGradient = curve_skewGradient;
            
            _setTrackLength();
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set curve for local angle
        // </Summary>
        void setCurvePivot( CurvePtr curve_localPivotX, CurvePtr curve_localPivotY,
                            CurvePtr curve_localPivotZ )
        {
            // set bit flags
            mTrackBitFlags |= TBF_Pivot;
            
            mCurve_LocalPivotX = curve_localPivotX;
            mCurve_LocalPivotY = curve_localPivotY;
            mCurve_LocalPivotZ = curve_localPivotZ;
            
            _setTrackLength();
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set curve for local angle
        // </Summary>
        void setCurvePivotX( CurvePtr curve_localPivotX )
        {
            // set bit flags
            mTrackBitFlags |= TBF_Pivot;
            
            mCurve_LocalPivotX = curve_localPivotX;
            
            _setTrackLength();
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set curve for local angle
        // </Summary>
        void setCurvePivotY( CurvePtr curve_localPivotY )
        {
            // set bit flags
            mTrackBitFlags |= TBF_Pivot;
            
            mCurve_LocalPivotY = curve_localPivotY;
            
            _setTrackLength();
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set curve for local angle
        // </Summary>
        void setCurvePivotZ( CurvePtr curve_localPivotZ )
        {
            // set bit flags
            mTrackBitFlags |= TBF_Pivot;
            
            mCurve_LocalPivotZ = curve_localPivotZ;
            
            _setTrackLength();
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // laod curves for local pos
        // </Summary>
        void loadCurvesPosition( const std::string& curve_localPosX,
                                 const std::string& curve_localPosY,
                                 const std::string& curve_localPosZ );
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // load curves for local scale
        // </Summary>
        void loadCurvesScale( const std::string& curve_localScaleX,
                              const std::string& curve_localScaleY,
                              const std::string& curve_localScaleZ );
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // laod curve for local angle
        // </Summary>
        void loadCurvesAngle( const std::string& curve_localAngleX,
                              const std::string& curve_localAngleY,
                              const std::string& curve_localAngleZ );
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // load curve for skew gradient
        // </Summary>
        void loadCurveSkewGradient( const std::string& curve_skewGradient);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // laod curves for local pos
        // </Summary>
        void loadCurvesPivot( const std::string& curve_localPivotX,
                              const std::string& curve_localPivotY,
                              const std::string& curve_localPivotZ );
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // get the length of the longest curve
        // </Summary>
        float getTrackLength()const
        {
            return mTrackLength;
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // apply track to node
        // </Summary>
        void apply(const AnimationBlendMode blendMode, const float time, const float weight=1.0f,
                   const float scale=1.0f, const bool isScreenSpace = false);
        
    public: //callbacks
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // called when a curve is finished loading
        // </Summary>
        void _curveFinishedLoading(void* data);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // called when a curve is finished loading
        // </Summary>
        void _setTrackLength()
        {
            std::vector< float > lengths(13);
            lengths[0] = mCurve_LocalPositionX ? mCurve_LocalPositionX->getLastTime() : 0.0f;
            lengths[1] = mCurve_LocalPositionY ? mCurve_LocalPositionY->getLastTime() : 0.0f;
            lengths[2] = mCurve_LocalPositionZ ? mCurve_LocalPositionZ->getLastTime() : 0.0f;
            lengths[3] = mCurve_LocalScaleX ? mCurve_LocalScaleX->getLastTime() : 0.0f;
            lengths[4] = mCurve_LocalScaleY ? mCurve_LocalScaleY->getLastTime() : 0.0f;
            lengths[5] = mCurve_LocalScaleZ ? mCurve_LocalScaleZ->getLastTime() : 0.0f;
            lengths[6] = mCurve_LocalAngleX ? mCurve_LocalAngleX->getLastTime() : 0.0f;
            lengths[7] = mCurve_LocalAngleY ? mCurve_LocalAngleY->getLastTime() : 0.0f;
            lengths[8] = mCurve_LocalAngleZ ? mCurve_LocalAngleZ->getLastTime() : 0.0f;
            lengths[9] = mCurve_SkewGradient ? mCurve_SkewGradient->getLastTime() : 0.0f;
            lengths[10] = mCurve_LocalPivotX ? mCurve_LocalPivotX->getLastTime() : 0.0f;
            lengths[11] = mCurve_LocalPivotY ? mCurve_LocalPivotY->getLastTime() : 0.0f;
            lengths[12] = mCurve_LocalPivotZ ? mCurve_LocalPivotZ->getLastTime() : 0.0f;

            
            mTrackLength = maxsetf( lengths );
        }
        
    private:// members        
        
        int                     mTrackBitFlags;
        float                   mTrackLength;
        CurvePtr                mCurve_LocalPositionX;
        CurvePtr                mCurve_LocalPositionY;
        CurvePtr                mCurve_LocalPositionZ;
        CurvePtr                mCurve_LocalScaleX;
        CurvePtr                mCurve_LocalScaleY;
        CurvePtr                mCurve_LocalScaleZ;
        CurvePtr                mCurve_LocalAngleX;
        CurvePtr                mCurve_LocalAngleY;
        CurvePtr                mCurve_LocalAngleZ;
        CurvePtr                mCurve_SkewGradient;
        CurvePtr                mCurve_LocalPivotX;
        CurvePtr                mCurve_LocalPivotY;
        CurvePtr                mCurve_LocalPivotZ;
        Animation*              mAnimation;
        Node*                   mNode;
    };
}
#endif 