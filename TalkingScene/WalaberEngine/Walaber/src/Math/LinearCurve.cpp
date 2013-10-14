//
//  CurveBase.h
//  WalaberEngine
//
//  Created by Jason Pecho on 5/12/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#include "LinearCurve.h"

#include <WalaberMath.h>

namespace Walaber
{
    // --------========--------========--------========--------========--------========--------========
    void LinearCurve::loadFromXmlNode(xmlNodePtr root, float keyDifferenceToIgnore )
    {        
        // look for the curve asset (we only take the first one)
        xmlNodePtr child = root->children;
        while (child)
        {            
            if (xmlStrcmp( child->name, (const xmlChar*)"PreLoop") == 0)
            {
                std::string content = XML::parseString(child, "value");
                
                this->setPreLoop( curveLoopTypeFromString(content) );

                Logger::printf("Walaber", Logger::SV_DEBUG, "PreLoop -> got content! [%s] = preLoop[%d]\n", content.c_str(), (int)getPreLoop());
            }
            else if (xmlStrcmp( child->name, (const xmlChar*)"PostLoop") == 0)
            {
                std::string content = XML::parseString(child, "value");
                
                this->setPostLoop( curveLoopTypeFromString(content) );

                Logger::printf("Walaber", Logger::SV_DEBUG, "PostLoop -> got content! [%s] = postLoop[%d]\n", content.c_str(), (int)getPostLoop());
            }
            if (xmlStrcmp( child->name, (const xmlChar*)"Keys") == 0)
            {
                xmlNodePtr key = child->children;
                
                float lastAddedCurveKeyValue = 0.0f ;
                bool didNotAddLastCurveKey=false;
                CurveKey lastCurveKey;
                
                while(key)
                {
                    if(xmlStrcmp( key->name, (const xmlChar*)"Key") == 0)
                    {
                        float position = XML::parseFloat(key, "position");
                        float value = XML::parseFloat(key, "value");
                        
                        CurveKey ck(position, value, 0.0f, 0.0f);
                        
                        bool shouldAddCurveKey=true;
                        
                        
                        if(absf(value-lastAddedCurveKeyValue) < keyDifferenceToIgnore &&
                           mKeys.size())
                        {
                            shouldAddCurveKey=false;
                            didNotAddLastCurveKey=true;
                            lastCurveKey=ck;
                            
                            Logger::printf("Walaber", Logger::SV_DEBUG, "Keys -> not adding curvekey! value: [%f] last value: [%f] key difference to ignore: [%f]\n", value, lastAddedCurveKeyValue, keyDifferenceToIgnore);
                        }
                        else if(didNotAddLastCurveKey)
                        {
                            Logger::printf("Walaber", Logger::SV_DEBUG, "Keys -> got content! position: [%f] value: [%f]\n", lastCurveKey.getPosition(), lastCurveKey.getValue());
                            
                            addCurveKey(lastCurveKey);
                        }
                        
                        if(shouldAddCurveKey)
                        {
                            lastAddedCurveKeyValue=value;
                            
                            didNotAddLastCurveKey=false;
                            
                            Logger::printf("Walaber", Logger::SV_DEBUG, "Keys -> got content! position: [%f] value: [%f]\n", position, value);

                            addCurveKey(ck);
                        }
                    }
                    key = key->next;
                }
                
                if( didNotAddLastCurveKey )
                {
                    addCurveKey( lastCurveKey );
                }
                computeTangents();
            }
            
            child = child->next;
        }
    }
    
    // -------------==============-------------==============-------------==============-------------===
    void LinearCurve::computeTangents()
    {
        if(!getIsConstant())
        {           
            unsigned int size = mKeys.size();
            for(int i = 1; i < size; ++i)
            {
                mKeys[i-1].setTangentOut((mKeys[i].getValue() - mKeys[i-1].getValue())/(mKeys[i].getPosition() - mKeys[i-1].getPosition()));
                mKeys[i].setTangentIn(-mKeys[i-1].getTangentOut());
            }
            
             // do the first key
            mKeys[0].setTangentIn(-mKeys[0].getTangentOut());
            mKeys[size-1].setTangentOut(-mKeys[size-1].getTangentIn());
        }
    }
    
    // --------========--------========--------========--------========--------========--------========
    float LinearCurve::_evaluateCurve(const float position, const float offset) const
    {	
        int k0, k1;
        float t = this->findSegment(position, k0, k1);
        
        return (offset + lerp(this->mKeys[k0].getValue(), this->mKeys[k1].getValue(), t));
    }
	
}

