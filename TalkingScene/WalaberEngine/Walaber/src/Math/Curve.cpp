//
//  Curve.cpp
//  WalaberEngine
//
//  Created by Jason Pecho on 5/12/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#include "Curve.h"
#include <StringHelper.h>

// for factory
#include <HermiteCurve.h>
#include <LinearCurve.h>
#include <TweenedLinearCurve.h>
#include <BezierCurve.h>

namespace Walaber
{
    // --------========--------========--------========--------========--------========--------========-
    CurveKey::CurveContinuity CurveKey::curveContinuityFromString( std::string& s )
    {
        if (s == "Step")
            return CC_Step;
        
        return CC_Smooth;
    }
    
    // -------------==============-------------==============-------------==============-------------===
    Curve* Curve::loadFromXml(const std::string& filename)
    {
        Curve* ret = 0;
        
        // check for a curve from an XNA framework XML file.
        xmlDocPtr doc;
        xmlNodePtr root_elem = XML::loadDoc( filename, doc );
        
        if(root_elem)
        {
            ret = Curve::loadFromXml(root_elem);
            XML::cleanupDoc(doc);
        }
        
        return ret;
    }
    
    // -------------==============-------------==============-------------==============-------------===
    Curve* Curve::loadFromXml(xmlNodePtr root, float keyDifferenceToIgnore)
    {
        Curve* ret = 0;
        
        if(root)
        {
            if(!xmlStrcmp(root->name, (const xmlChar*)"XnaContent"))
            {
                // curve type is from XNA so create a  curve and load it!
                ret = new HermiteCurve();
                ret->loadFromXmlNode(root);
            }
            else if(!xmlStrcmp(root->name, (const xmlChar*)"Curve"))
            {
                std::string lowerType = StringHelper::toLower(XML::parseString(root, "type"));
                
                if(lowerType == "linear")
                {
                    ret = new LinearCurve();
                }
                else if(lowerType == "tweenedlinear")
                {
                    ret = new TweenedLinearCurve();
                }
                else if (lowerType == "bezier")
				{
					ret = new BezierCurve();
				}
				else
                {
                    // curve type does not exist - fall back to linear
                    assert(0);
                    ret = new LinearCurve();
                }  
                
                ret->loadFromXmlNode(root, keyDifferenceToIgnore);
            }
        }
        
        return ret;
    }
    
    // --------========--------========--------========--------========--------========--------========
    Curve::CurveLoopType Curve::curveLoopTypeFromString( std::string& s )
    {
        if (s == "Constant")
            return CLT_Constant;
        else if (s == "Cycle")
            return CLT_Cycle;
        else if (s == "CycleOffset")
            return CLT_CycleOffset;
        else if (s == "Oscillate")
            return CLT_Oscillate;
        else if (s == "Linear")
            return CLT_Linear;
        
        return CLT_Constant;
    }   
    
    // -------------==============-------------==============-------------==============-------------===
    float Curve::evaluate(float position) const
    {
        if (this->mKeys.size() == 0)
        {
            return 0.0f;
        }
        if (this->mKeys.size() == 1)
        {
            return this->mKeys[0].getValue();
        }
        
        CurveKey key = this->mKeys[0];
        CurveKey key2 = this->mKeys[this->mKeys.size() - 1];
        
        float t = position;
        float num6 = 0.0f;
        if (t < key.getPosition())
        {
            if (this->mPreLoop == CLT_Constant)
            {
                return key.getValue();
            }
            if (this->mPreLoop == CLT_Linear)
            {
                return (key.getValue() - (key.getTangentIn() * (key.getPosition() - t)));
            }           
            
            float num5 = this->calcCycle(t);
            float num3 = t - (key.getPosition() + (num5 * this->mTimeRange));
            if (this->mPreLoop == CLT_Cycle)
            {
                t = key.getPosition() + num3;
            }
            else if (this->mPreLoop == CLT_CycleOffset)
            {
                t = key.getPosition() + num3;
                num6 = (key2.getValue() - key.getValue()) * num5;
            }
            else
            {
                t = ((((int) num5) & 1) != 0) ? (key2.getPosition() - num3) : (key.getPosition() + num3);
            }
        }
        else if (key2.getPosition() < t)
        {
            if (this->mPostLoop == CLT_Constant)
            {
                return key2.getValue();
            }
            if (this->mPostLoop == CLT_Linear)
            {
                return (key2.getValue() - (key2.getTangentOut() * (key2.getPosition() - t)));
            }
            
            float num4 = this->calcCycle(t);
            float num2 = t - (key.getPosition() + (num4 * this->mTimeRange));
            if (this->mPostLoop == CLT_Cycle)
            {
                t = key.getPosition() + num2;
            }
            else if (this->mPostLoop == CLT_CycleOffset)
            {
                t = key.getPosition() + num2;
                num6 = (key2.getValue() - key.getValue()) * num4;
            }
            else
            {
                t = ((((int) num4) & 1) != 0) ? (key2.getPosition() - num2) : (key.getPosition() + num2);
            }
        }
        
        return _evaluateCurve(t, num6);
    }
    
    // -------------==============-------------==============-------------==============-------------===
    float Curve::calcCycle(float t) const
    {
        float num = (t - this->mKeys[0].getPosition()) * this->mInvTimeRange;
        if (num < 0.0f)
        {
            num--;
        }
        int num2 = (int) num;
        return (float) num2;
    }
    
    // --------========--------========--------========--------========--------========--------========-
    float Curve::findSegment(float t, int& k0, int& k1) const
    {	
        CurveKey* kk0;
        CurveKey* kk1;
        
        float num2 = t;
        k0 = 0;
        kk0 = (CurveKey*)&this->mKeys[k0];
        
        for (unsigned int i = 1; i < this->mKeys.size(); i++)
        {
            k1 = i;
            kk1 = (CurveKey*)&this->mKeys[k1];
            
            if (kk1->getPosition() >= t)
            {
                double position = kk0->getPosition();
                double num6 = kk1->getPosition();
                double num5 = t;
                double num3 = num6 - position;
                num2 = 0.0f;
                if (num3 > 1E-10)
                {
                    num2 = (float) ((num5 - position) / num3);
                }
                return num2;
            }
            k0 = k1;
            kk0 = kk1;
        }
        
        return num2;
    }
    
    // --------========--------========--------========--------========--------========--------========-
    void Curve::addCurveKey( const CurveKey& ck )
    {
        if (this->mKeys.size() > 0)
        {
            assert(ck.getPosition() > this->mKeys[this->mKeys.size() -1].getPosition());
            if (ck.getPosition() <= this->mKeys[this->mKeys.size() -1].getPosition())
            {
                Logger::printf("Walaber", Logger::SV_ERROR, "ERROR!  keys must be added in order! last is [%f] and new is [%f]\n",
															this->mKeys[this->mKeys.size()-1].getPosition(), ck.getPosition());
                
                return;
            }
        }
        
        mKeys.push_back( ck );
        
        if (this->mKeys.size() > 0)
        {
            this->mTimeRange = this->mKeys[this->mKeys.size()-1].getPosition() - this->mKeys[0].getPosition();
            this->mInvTimeRange = 1.0f / mTimeRange;
        }
        else
        {
            this->mTimeRange = 0.0f;
            this->mInvTimeRange = 0.0f;
        }
        
        //Logger::printf("Walaber", Logger::SV_DEBUG, "Curve::addCurveKey - added Key p[%f] v[%f] tanIn[%f] tanOut[%f] continuity[%d]\n",
		//											ck.getPosition(), ck.getValue(), ck.getTangentIn(), ck.getTangentOut(), (int)ck.getContinuity() );
    }
    
    // -------------==============-------------==============-------------==============-------------===
    // <Summary>
    // loads compressed keyframes. If keyframes are not compressed, they should not use this
    // function
    // </Summary>
    void Curve::_loadCompressedKeys( char* buffer, unsigned int& bytesRead, unsigned int flags, const CompressionRecord& timeCR, const CompressionRecord& valueCR, const float keyDifferenceToIgnore, std::vector< int >& outSkippedKeyIndices )
    {
        assert( ( flags & CCF_1_Byte_Compression ) || ( flags & CCF_2_Byte_Compression ) ||
               ( flags & CCF_Skeletal_Animation_Collection ) );
        
        unsigned int numKeys = 0;
        memcpy(&numKeys, &buffer[bytesRead], sizeof(numKeys) );
        bytesRead += sizeof(numKeys);
        
        float lastAddedCurveKeyValue=0;
        bool didNotAddLastCurveKey=false;
        CurveKey lastCurveKey;
        std::vector<int>            mKeysIgnored;
        
        float timeInc;
        float valueInc;
        //unsigned int sizeOfReadValue;
        // we don't unpack all of the key data, we generate some of it
        if (flags & CCF_1_Byte_Compression )
        {
            // we lose a value of representation because we need 0 to represent 0
            float limit = static_cast<float>(std::numeric_limits<unsigned char>::max()) - 1.0f;
            
            timeInc = (timeCR.mMaxValue - timeCR.mMinValue) / limit;
            valueInc = (valueCR.mMaxValue - valueCR.mMinValue) / limit;
            //sizeOfReadValue = sizeof(unsigned char);
        }
        else if (flags & CCF_2_Byte_Compression )
        {
            // we lose a value of representation because we need 0 to represent 0
            float limit = static_cast<float>(std::numeric_limits<unsigned short>::max()) - 1.0f;
            
            timeInc = (timeCR.mMaxValue - timeCR.mMinValue) / limit;
            valueInc = (valueCR.mMaxValue - valueCR.mMinValue) / limit;
            //sizeOfReadValue = sizeof(unsigned short);
        }
        else
        {
            timeInc = 1.0f;
            valueInc = 1.0f;
            //sizeOfReadValue = sizeof( float );
        }
        
        if( flags & CCF_Skeletal_Animation_Collection )
        {
            if (flags & CCF_1_Byte_Compression )
            {
                for (unsigned int i=0; i < numKeys; ++i)
                {
                    CurveKey ck;
                    
                    ck.fillPositionValueOneByteCompression( timeInc,
                                                           timeCR.mMinValue,
                                                           valueInc,
                                                           valueCR.mMinValue,
                                                           buffer,
                                                           bytesRead );
                    
                    if( i != (numKeys-1) )
                    {
                        if( ! addCurveKeyOptional(ck,
                                                  keyDifferenceToIgnore,
                                                  lastCurveKey,
                                                  lastAddedCurveKeyValue,
                                                  didNotAddLastCurveKey,
                                                  outSkippedKeyIndices ) )
                        {
                            // if we didnt add this key, we need to add it to the skipped list
                            outSkippedKeyIndices.push_back( i );
                        }
                    }
                    else
                    {
                        addCurveKey( ck );
                    }
                }
            }
            else if (flags & CCF_2_Byte_Compression )
            {
                for (unsigned int i=0; i < numKeys; ++i)
                {
                    CurveKey ck;
                    
                    ck.fillPositionValueTwoByteCompression( timeInc,
                                                           timeCR.mMinValue,
                                                           valueInc,
                                                           valueCR.mMinValue,
                                                           buffer,
                                                           bytesRead );
                    if( i != (numKeys-1) )
                    {
                        if( ! addCurveKeyOptional(ck,
                                                  keyDifferenceToIgnore,
                                                  lastCurveKey,
                                                  lastAddedCurveKeyValue,
                                                  didNotAddLastCurveKey,
                                                  outSkippedKeyIndices ) )
                        {
                            // if we didnt add this key, we need to add it to the skipped list
                            outSkippedKeyIndices.push_back( i );
                        }
                    }
                    else
                    {
                        addCurveKey( ck );
                    }
                }
            }
            else
            {
                for (unsigned int i=0; i < numKeys; ++i)
                {
                    CurveKey ck;
                    
                    ck.fillPositionValue( timeInc,
                                         timeCR.mMinValue,
                                         valueInc,
                                         valueCR.mMinValue,
                                         buffer,
                                         bytesRead );
                    if( i != (numKeys-1) )
                    {
                        if( ! addCurveKeyOptional(ck,
                                                  keyDifferenceToIgnore,
                                                  lastCurveKey,
                                                  lastAddedCurveKeyValue,
                                                  didNotAddLastCurveKey,
                                                  outSkippedKeyIndices ) )
                        {
                            // if we didnt add this key, we need to add it to the skipped list
                            outSkippedKeyIndices.push_back( i );
                        }
                    }
                    else
                    {
                        addCurveKey( ck );
                    }
                    
                }
            }
        }
        else if (flags & CCF_1_Byte_Compression )
        {
            for (unsigned int i=0; i < numKeys; ++i)
            {
                CurveKey ck;
                
                ck.fillStandardOneByteCompression( timeInc,
                                                  timeCR.mMinValue,
                                                  valueInc,
                                                  valueCR.mMinValue,
                                                  buffer,
                                                  bytesRead );
                
                if( i != (numKeys-1) )
                {
                    if( ! addCurveKeyOptional(ck,
                                              keyDifferenceToIgnore,
                                              lastCurveKey,
                                              lastAddedCurveKeyValue,
                                              didNotAddLastCurveKey,
                                              outSkippedKeyIndices ) )
                    {
                        // if we didnt add this key, we need to add it to the skipped list
                        outSkippedKeyIndices.push_back( i );
                    }
                }
                else
                {
                    addCurveKey( ck );
                }
            }
        }
        else if (flags & CCF_2_Byte_Compression )
        {
            for (unsigned int i=0; i < numKeys; ++i)
            {
                CurveKey ck;
                
                ck.fillStandardTwoByteCompression( timeInc,
                                                  timeCR.mMinValue,
                                                  valueInc,
                                                  valueCR.mMinValue,
                                                  buffer,
                                                  bytesRead );
                
                if( i != (numKeys-1) )
                {
                    if( ! addCurveKeyOptional(ck,
                                              keyDifferenceToIgnore,
                                              lastCurveKey,
                                              lastAddedCurveKeyValue,
                                              didNotAddLastCurveKey,
                                              outSkippedKeyIndices ) )
                    {
                        // if we didnt add this key, we need to add it to the skipped list
                        outSkippedKeyIndices.push_back( i );
                    }
                }
                else
                {
                    addCurveKey( ck );
                }
            }
        }
    }
}
