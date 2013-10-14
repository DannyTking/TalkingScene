//
//  TweenedLinearCurve.cpp
//  WalaberEngine
//
//  Created by Jason Pecho on 5/13/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "TweenedLinearCurve.h"

namespace Walaber
{
    // --------========--------========--------========--------========--------========--------========
    void TweenedLinearCurve::loadFromXmlNode(xmlNodePtr root, float keyDifferenceToIgnore )
    {        
        // look for the curve asset (we only take the first one)
        xmlNodePtr child = root->children;
        while (child)
        {            
            if (xmlStrcmp( child->name, (const xmlChar*)"PreLoop") == 0)
            {
                std::string content = XML::parseString(child, "value");
                
                this->setPreLoop( curveLoopTypeFromString(content) );
#if _DEBUG
                printf("PreLoop -> got content! [%s] = preLoop[%d]\n", content.c_str(), (int)getPreLoop());
#endif
            }
            else if (xmlStrcmp( child->name, (const xmlChar*)"PostLoop") == 0)
            {
                std::string content = XML::parseString(child, "value");
                
                this->setPostLoop( curveLoopTypeFromString(content) );
#ifdef _DEBUG
                printf("PostLoop -> got content! [%s] = postLoop[%d]\n", content.c_str(), (int)getPostLoop());
#endif
            }
            if (xmlStrcmp( child->name, (const xmlChar*)"Keys") == 0)
            {
                xmlNodePtr key = child->children;
                
                float lastAddedCurveKeyValue=0;
                bool didNotAddLastCurveKey=false;
                CurveKey lastCurveKey;
                EasingFunction lastTweeningFunction;
                
                while(key)
                {
                    if(xmlStrcmp( key->name, (const xmlChar*)"Key") == 0)
                    {
                        float position = XML::parseFloat(key, "position");
                        float value = XML::parseFloat(key, "value");
                        
                        CurveKey ck(position, value, 0.0f, 0.0f);
                        
                        EasingFunction tweeningFunction;
                        
                        if(XML::attrExists(key, "tween"))
                        {
                            std::string tween = XML::parseString(key, "tween");
                            float tweenValue = XML::parseFloat(key, "tweenValue");
                            
                            tweeningFunction = Tweens::getEasingFunctionForString(tween,tweenValue);
                        }
                        else
                        {
                            tweeningFunction = &Tweens::linearTween;
                        }
                        
                        bool shouldAddCurveKey=true;                        

                        if(absf(value-lastAddedCurveKeyValue)<keyDifferenceToIgnore &&
                           mKeys.size() )
                        {
                            shouldAddCurveKey=false;
                            didNotAddLastCurveKey=true;
                            lastCurveKey=ck;
                            lastTweeningFunction=tweeningFunction;
                            
                            Logger::printf("Walaber", Logger::SV_DEBUG, "Keys -> not adding curvekey! value: [%f] last value: [%f] key difference to ignore: [%f]\n", value, lastAddedCurveKeyValue, keyDifferenceToIgnore);
                        }
                        else if(didNotAddLastCurveKey)
                        {
                            Logger::printf("Walaber", Logger::SV_DEBUG, "Keys -> got content! position: [%f] value: [%f]\n", lastCurveKey.getPosition(), lastCurveKey.getValue());
                            
                            addCurveKey(lastCurveKey);
                            
                            mTweeningFunctions.push_back(lastTweeningFunction);
                        }
                        
                        if(shouldAddCurveKey)
                        {
                            lastAddedCurveKeyValue=value;
                            
                            didNotAddLastCurveKey=false;
                            
#ifdef _DEBUG
                            printf("Keys -> got content! position: [%f] value: [%f]\n", position, value);
#endif
                            
                            addCurveKey(ck);
                            
                            mTweeningFunctions.push_back(tweeningFunction);
                        }
                        
                    }
                    key = key->next;
                }
                
                if( didNotAddLastCurveKey )
                {
                    addCurveKey( lastCurveKey );
                    mTweeningFunctions.push_back(lastTweeningFunction);
                }
                
                _computeDurations();
                computeTangents();
            }
            
            child = child->next;
        }
    }
    
    // --------========--------========--------========--------========--------========--------========
    float TweenedLinearCurve::_evaluateCurve(const float position, const float offset) const
    {	
        int k0, k1;
        this->findSegment(position, k0, k1);
        float t = position - mKeys[k0].getPosition();
        
        return mTweeningFunctions[k0](t, mKeys[k0].getValue(), mKeyDurations[k0].valueChange, mKeyDurations[k0].duration);
    }
    
    // -------------==============-------------==============-------------==============-------------===
    // <Summary>
    // computes the durations and changes between keys
    // </Summary>
    void TweenedLinearCurve::_computeDurations()
    {
        mKeyDurations.resize(mKeys.size());
        if(!getIsConstant())
        {           
            unsigned int size = mKeys.size();
            for(int i = 1; i < size; ++i)
            {
                mKeyDurations[i-1].valueChange = mKeys[i].getValue() - mKeys[i-1].getValue();
                mKeyDurations[i-1].duration = mKeys[i].getPosition() - mKeys[i-1].getPosition();
            }            
        }
            // do the first key
        mKeyDurations[mKeys.size()-1].valueChange = 0.0f;
        mKeyDurations[mKeys.size()-1].duration = 1.0f;
    }
}