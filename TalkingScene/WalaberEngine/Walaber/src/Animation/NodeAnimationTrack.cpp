//
//  NodeAnimationTrack.cpp
//  WalaberEngine
//
//  Created by Jason Pecho on 5/5/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "NodeAnimationTrack.h"

#include <Node.h>
#include <CurveManager.h>
#include <Animation.h>
#include <MemberCallback.h>

namespace Walaber
{
    // -------------==============-------------==============-------------==============-------------===
    NodeAnimationTrack::NodeAnimationTrack():
    mTrackBitFlags(0),
    mTrackLength(0.0f),
    mCurve_LocalPositionX(),
    mCurve_LocalPositionY(),
    mCurve_LocalPositionZ(),
    mCurve_LocalScaleX(),
    mCurve_LocalScaleY(),
    mCurve_LocalScaleZ(),
    mCurve_LocalAngleX(),
    mCurve_LocalAngleY(),
    mCurve_LocalAngleZ(),
    mCurve_SkewGradient(),
    mCurve_LocalPivotX(),
    mCurve_LocalPivotY(),
    mCurve_LocalPivotZ(),
    mAnimation(0),
    mNode(0)
    {       
    }
    
    // -------------==============-------------==============-------------==============-------------===
    NodeAnimationTrack::NodeAnimationTrack(Node* node):
    mTrackBitFlags(0),
    mTrackLength(0.0f),
    mCurve_LocalPositionX(),
    mCurve_LocalPositionY(),
    mCurve_LocalPositionZ(),
    mCurve_LocalScaleX(),
    mCurve_LocalScaleY(),
    mCurve_LocalScaleZ(),
    mCurve_LocalAngleX(),
    mCurve_LocalAngleY(),
    mCurve_LocalAngleZ(),
    mCurve_SkewGradient(),
    mCurve_LocalPivotX(),
    mCurve_LocalPivotY(),
    mCurve_LocalPivotZ(),
    mAnimation(0),
    mNode(node)
    {        
    }
    
    // -------------==============-------------==============-------------==============-------------===
    NodeAnimationTrack::NodeAnimationTrack(const NodeAnimationTrack& nodeAnimationTrack):
    mTrackBitFlags(nodeAnimationTrack.mTrackBitFlags),
    mTrackLength(nodeAnimationTrack.mTrackLength),
    mCurve_LocalPositionX(nodeAnimationTrack.mCurve_LocalPositionX),
    mCurve_LocalPositionY(nodeAnimationTrack.mCurve_LocalPositionY),
    mCurve_LocalPositionZ(nodeAnimationTrack.mCurve_LocalPositionZ),
    mCurve_LocalScaleX(nodeAnimationTrack.mCurve_LocalScaleX),
    mCurve_LocalScaleY(nodeAnimationTrack.mCurve_LocalScaleY),
    mCurve_LocalScaleZ(nodeAnimationTrack.mCurve_LocalScaleZ),
    mCurve_LocalAngleX(nodeAnimationTrack.mCurve_LocalAngleX),
    mCurve_LocalAngleY(nodeAnimationTrack.mCurve_LocalAngleY),
    mCurve_LocalAngleZ(nodeAnimationTrack.mCurve_LocalAngleZ),
    mCurve_SkewGradient(nodeAnimationTrack.mCurve_SkewGradient),
    mCurve_LocalPivotX(nodeAnimationTrack.mCurve_LocalPivotX),
    mCurve_LocalPivotY(nodeAnimationTrack.mCurve_LocalPivotY),
    mCurve_LocalPivotZ(nodeAnimationTrack.mCurve_LocalPivotZ),
    mAnimation(nodeAnimationTrack.mAnimation),
    mNode(nodeAnimationTrack.mNode)
    {
    }
    
    // -------------==============-------------==============-------------==============-------------===
    NodeAnimationTrack& NodeAnimationTrack::operator=(const NodeAnimationTrack& nodeAnimationTrack)
    {
        mTrackBitFlags = nodeAnimationTrack.mTrackBitFlags;
        mTrackLength = nodeAnimationTrack.mTrackLength;
        
        mCurve_LocalPositionX = nodeAnimationTrack.mCurve_LocalPositionX;
        mCurve_LocalPositionY = nodeAnimationTrack.mCurve_LocalPositionY;
        mCurve_LocalPositionZ = nodeAnimationTrack.mCurve_LocalPositionZ;
        mCurve_LocalScaleX = nodeAnimationTrack.mCurve_LocalScaleX;
        mCurve_LocalScaleY = nodeAnimationTrack.mCurve_LocalScaleY;
        mCurve_LocalScaleZ = nodeAnimationTrack.mCurve_LocalScaleZ;
        mCurve_LocalAngleX = nodeAnimationTrack.mCurve_LocalAngleX;
        mCurve_LocalAngleY = nodeAnimationTrack.mCurve_LocalAngleY;
        mCurve_LocalAngleZ = nodeAnimationTrack.mCurve_LocalAngleZ;
        mCurve_SkewGradient = nodeAnimationTrack.mCurve_SkewGradient;
        mCurve_LocalPivotX = nodeAnimationTrack.mCurve_LocalPivotX;
        mCurve_LocalPivotY = nodeAnimationTrack.mCurve_LocalPivotY;
        mCurve_LocalPivotZ = nodeAnimationTrack.mCurve_LocalPivotZ;
        
        mAnimation = nodeAnimationTrack.mAnimation;
        mNode = nodeAnimationTrack.mNode;
        
        return *this;
    }
    
    // -------------==============-------------==============-------------==============-------------===
    NodeAnimationTrack::~NodeAnimationTrack()
    {        
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void NodeAnimationTrack::setAnimation(Animation* animation)
    {
        mAnimation = animation;
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void NodeAnimationTrack::loadCurvesPosition( const std::string& curve_localPosX,
                                                 const std::string& curve_localPosY,
                                                 const std::string& curve_localPosZ )
    {
        // set bit flags
        mTrackBitFlags |= TBF_Position;
        
        MemberCallbackPtr<NodeAnimationTrack>::type memcallback(
                        new MemberCallback<NodeAnimationTrack>(this,
                                &NodeAnimationTrack::_curveFinishedLoading));
        CallbackPtr callback = static_pointer_cast<Callback>(memcallback);
        
        if( !curve_localPosX.empty() )
        {
            CurveManager::getManager().getCurve(&mCurve_LocalPositionX, curve_localPosX, callback);
        }
        else
        {
            mCurve_LocalPositionX = CurveManager::getEmptyCurve();
        }
        
        if( !curve_localPosY.empty() )
        {
            CurveManager::getManager().getCurve(&mCurve_LocalPositionY, curve_localPosY, callback);
        }
        else
        {
            mCurve_LocalPositionY = CurveManager::getEmptyCurve();
        }
        
        if( !curve_localPosZ.empty() )
        {
            CurveManager::getManager().getCurve(&mCurve_LocalPositionZ, curve_localPosZ, callback);
        }
        else
        {
            mCurve_LocalPositionZ = CurveManager::getEmptyCurve();
        }
    }

    // -------------==============-------------==============-------------==============-------------===
    void NodeAnimationTrack::loadCurvesScale( const std::string& curve_localScaleX,
                                              const std::string& curve_localScaleY,
                                              const std::string& curve_localScaleZ )
    {
        // set bit flags
        mTrackBitFlags |= TBF_Scale;
    
        MemberCallbackPtr<NodeAnimationTrack>::type memcallback(
                        new MemberCallback<NodeAnimationTrack>(this,
                                &NodeAnimationTrack::_curveFinishedLoading));
        CallbackPtr callback = static_pointer_cast<Callback>(memcallback);
       
        if( !curve_localScaleX.empty() )
        {
            CurveManager::getManager().getCurve(&mCurve_LocalScaleX, curve_localScaleX, callback);
        }
        else
        {
            mCurve_LocalScaleX = CurveManager::getOneCurve();
        }
        
        if( !curve_localScaleY.empty() )
        {
            CurveManager::getManager().getCurve(&mCurve_LocalScaleY, curve_localScaleY, callback);
        }
        else
        {
            mCurve_LocalScaleY = CurveManager::getOneCurve();
        }
        
        if( !curve_localScaleZ.empty() )
        {
            CurveManager::getManager().getCurve(&mCurve_LocalScaleZ, curve_localScaleZ, callback);
        }
        else
        {
            mCurve_LocalScaleZ = CurveManager::getOneCurve();
        }
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void NodeAnimationTrack::loadCurvesAngle( const std::string& curve_localAngleX,
                                              const std::string& curve_localAngleY,
                                              const std::string& curve_localAngleZ )
    {
        // set bit flags
        mTrackBitFlags |= TBF_Angle;
        
        MemberCallbackPtr<NodeAnimationTrack>::type memcallback(
                        new MemberCallback<NodeAnimationTrack>(this,
                                &NodeAnimationTrack::_curveFinishedLoading));
        CallbackPtr callback = static_pointer_cast<Callback>(memcallback);
        
        if( !curve_localAngleX.empty() )
        {
            CurveManager::getManager().getCurve(&mCurve_LocalAngleX, curve_localAngleX, callback);
        }
        else
        {
            mCurve_LocalAngleX = CurveManager::getEmptyCurve();
        }
        
        if( !curve_localAngleY.empty() )
        {
            CurveManager::getManager().getCurve(&mCurve_LocalAngleY, curve_localAngleY, callback);
        }
        else
        {
            mCurve_LocalAngleY = CurveManager::getEmptyCurve();
        }
        
        if( !curve_localAngleZ.empty() )
        {
            CurveManager::getManager().getCurve(&mCurve_LocalAngleZ, curve_localAngleZ, callback);
        }
        else
        {
            mCurve_LocalAngleZ = CurveManager::getEmptyCurve();
        }
    }
    
    // -------------==============-------------==============-------------==============-------------===
    // <Summary>
    // load curve for skew gradient
    // </Summary>
    void NodeAnimationTrack::loadCurveSkewGradient( const std::string& curve_skewGradient)
    {
        // set bit flags
        mTrackBitFlags |= TBF_SkewGradient;
        
        MemberCallbackPtr<NodeAnimationTrack>::type memcallback(
                                                                new MemberCallback<NodeAnimationTrack>(this,
                                                                                                       &NodeAnimationTrack::_curveFinishedLoading));
        CallbackPtr callback = static_pointer_cast<Callback>(memcallback);
        
        if( !curve_skewGradient.empty() )
        {
            CurveManager::getManager().getCurve(&mCurve_SkewGradient, curve_skewGradient, callback);
        }
        else
        {
            mCurve_SkewGradient = CurveManager::getEmptyCurve();
        }

    }
    
    void NodeAnimationTrack::loadCurvesPivot( const std::string& curve_localPivotX,
                                              const std::string& curve_localPivotY,
                                              const std::string& curve_localPivotZ )
    {
        // set bit flags
        mTrackBitFlags |= TBF_Pivot;
        
        MemberCallbackPtr<NodeAnimationTrack>::type memcallback(
                                                                new MemberCallback<NodeAnimationTrack>(this,
                                                                                                       &NodeAnimationTrack::_curveFinishedLoading));
        CallbackPtr callback = static_pointer_cast<Callback>(memcallback);
        
        if( !curve_localPivotX.empty() )
        {
            CurveManager::getManager().getCurve(&mCurve_LocalPivotX, curve_localPivotX, callback);
        }
        else
        {
            mCurve_LocalPivotX = CurveManager::getEmptyCurve();
        }
        
        if( !curve_localPivotY.empty() )
        {
            CurveManager::getManager().getCurve(&mCurve_LocalPivotY, curve_localPivotY, callback);
        }
        else
        {
            mCurve_LocalPivotY = CurveManager::getEmptyCurve();
        }

        if( !curve_localPivotZ.empty() )
        {
            CurveManager::getManager().getCurve(&mCurve_LocalPivotZ, curve_localPivotZ, callback);
        }
        else
        {
            mCurve_LocalPivotZ = CurveManager::getEmptyCurve();
        }
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void NodeAnimationTrack::apply(const AnimationBlendMode blendMode, const float time, const float weight, const float scale, const bool isScreenSpace)
    {
        switch(blendMode)
        {
            case ABM_Default:
            {
                if(mTrackBitFlags & TBF_Position)
                {
                    mNode->setLocalPosition(
                                    Vector3(
                                            mCurve_LocalPositionX->evaluate(time)*(weight*scale),
                                            mCurve_LocalPositionY->evaluate(time)*(weight*scale),
                                            mCurve_LocalPositionZ->evaluate(time)*(weight*scale)
                                            )
                                            );
                }
                
                if(mTrackBitFlags & TBF_Scale)
                {
                    mNode->setLocalScale(
                                Vector3(
                                        mCurve_LocalScaleX->evaluate(time)*(weight*scale),
                                        mCurve_LocalScaleY->evaluate(time)*(weight*scale),
                                        mCurve_LocalScaleZ->evaluate(time)*(weight*scale)
                                        )
                                        );
                }
                
                if(mTrackBitFlags & TBF_Angle)
                {
                    float curveNormValueX = mCurve_LocalAngleX->evaluate(time);
                    float curveNormValueY = mCurve_LocalAngleY->evaluate(time);
                    float curveNormValueZ = mCurve_LocalAngleZ->evaluate(time);
                    curveNormValueX = normalizeAngle_PI(curveNormValueX);
                    curveNormValueY = normalizeAngle_PI(curveNormValueY);
                    curveNormValueZ = normalizeAngle_PI(curveNormValueZ);
                    mNode->setLocalAngles( Vector3 (
                                         curveNormValueX * (weight*scale),
                                         curveNormValueY * (weight*scale),
                                         curveNormValueZ * (weight*scale)
                                                   ) );
                }
                
                if(mTrackBitFlags & TBF_SkewGradient)
                {
                    float skewGradient = mCurve_SkewGradient->evaluate(time);
                    skewGradient = normalizeAngle_PI(skewGradient);
                    mNode->setLocalSkew( skewGradient * (weight*scale) );
                }
                
                if(mTrackBitFlags & TBF_Pivot)
                {
                    mNode->setLocalPivot(
                                         Vector3(
                                                mCurve_LocalPivotX->evaluate(time)*(weight*scale),
                                                mCurve_LocalPivotY->evaluate(time)*(weight*scale),
                                                mCurve_LocalPivotZ->evaluate(time)*(weight*scale)
                                                )
                                         );
                }
            }
                break;
                
            case ABM_Additive:
            {
                if(mTrackBitFlags & TBF_Position)
                {
                    mNode->applyPositionOffset(
                                    Vector3(
                                            mCurve_LocalPositionX->evaluate(time)*(weight*scale),
                                            mCurve_LocalPositionY->evaluate(time)*(weight*scale),
                                            mCurve_LocalPositionZ->evaluate(time)*(weight*scale)
                                            )
                                      );
                }
                
                if(mTrackBitFlags & TBF_Scale)
                {
                    mNode->applyScaleOffset(
                                    Vector3(
                                             mCurve_LocalScaleX->evaluate(time)*(weight*scale),
                                             mCurve_LocalScaleY->evaluate(time)*(weight*scale),
                                             mCurve_LocalScaleZ->evaluate(time)*(weight*scale)
                                           )
                                       );
                }
                
                if(mTrackBitFlags & TBF_Angle)
                {
                    float curveNormValueX = mCurve_LocalAngleX->evaluate(time);
                    float curveNormValueY = mCurve_LocalAngleY->evaluate(time);
                    float curveNormValueZ = mCurve_LocalAngleZ->evaluate(time);
                    curveNormValueX = normalizeAngle_PI(curveNormValueX);
                    curveNormValueY = normalizeAngle_PI(curveNormValueY);
                    curveNormValueZ = normalizeAngle_PI(curveNormValueZ);
                    mNode->applyAnglesOffset(
                                        Vector3 (
                                                  curveNormValueX * (weight*scale),
                                                  curveNormValueY * (weight*scale),
                                                  curveNormValueZ * (weight*scale)
                                                  ) );
                }
                
                if(mTrackBitFlags & TBF_SkewGradient)
                {
                    float skewGradient = mCurve_SkewGradient->evaluate(time);
                    skewGradient = normalizeAngle_PI(skewGradient);
                    mNode->setLocalSkew( skewGradient * (weight*scale) );
                }
                
                if(mTrackBitFlags & TBF_Pivot)
                {
                    mNode->applyPivotOffset(
                                           Vector3(
                                                  mCurve_LocalPivotX->evaluate(time)*(weight*scale),
                                                  mCurve_LocalPivotY->evaluate(time)*(weight*scale),
                                                  mCurve_LocalPivotZ->evaluate(time)*(weight*scale)
                                                  )
                                           );
                }
            }
                break;
                
            default:
                // illegal blend mode
                assert(0);
                break;
        }
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void NodeAnimationTrack::_curveFinishedLoading(void* data)
    {
        CurveManager::CurveLoadedCallbackParameters* params = (CurveManager::CurveLoadedCallbackParameters*)data;
        
        if( params->result == ResultCode_OK )
        {
            _setTrackLength();
        
            if(mAnimation)
            {
                mAnimation->notifyTrackLength(mTrackLength);
            }
        }
    }
}