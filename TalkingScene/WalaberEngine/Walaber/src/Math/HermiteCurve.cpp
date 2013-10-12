#include "HermiteCurve.h"

#include <math.h>
#include <StringHelper.h>

namespace Walaber
{
    // --------========--------========--------========--------========--------========--------========--------========--------========
    // --------========--------========--------========--------========--------========--------========--------========--------========
    void HermiteCurve::ComputeTangent(int keyIndex, HermiteCurveTangent tangentType)
    {
        this->ComputeTangent(keyIndex, tangentType, tangentType);
    }

    // --------========--------========--------========--------========--------========--------========--------========--------========
    // --------========--------========--------========--------========--------========--------========--------========--------========
    void HermiteCurve::ComputeTangent(int keyIndex, HermiteCurveTangent tangentInType, HermiteCurveTangent tangentOutType)
    {
        float num2;
        float num4;
        float num7;
        float num8;
        if (((int)this->mKeys.size() <= keyIndex) || (keyIndex < 0))
        {
            return;
        }
        
        CurveKey& key = this->mKeys[keyIndex];
        float position = num8 = num4 = key.getPosition();
        float num = num7 = num2 = key.getValue();
        
        if (keyIndex > 0)
        {
            position = this->mKeys[keyIndex - 1].getPosition();
            num = this->mKeys[keyIndex - 1].getValue();
        }
        if ((keyIndex + 1) < (int)this->mKeys.size())
        {
            num4 = this->mKeys[keyIndex + 1].getPosition();
            num2 = this->mKeys[keyIndex + 1].getValue();
        }
        if (tangentInType == HCT_Smooth)
        {
            float num10 = num4 - position;
            float num6 = num2 - num;
            if (abs(num6) < 1.192093E-07f)
            {
                key.setTangentIn( 0.0f );
            }
            else
            {
                key.setTangentIn( (num6 * abs((float) (position - num8))) / num10 );
            }
        }
        else if (tangentInType == HCT_Linear)
        {
            key.setTangentIn( num7 - num );
        }
        else
        {
            key.setTangentIn( 0.0f );
        }
        
        if (tangentOutType == HCT_Smooth)
        {
            float num9 = num4 - position;
            float num5 = num2 - num;
            if (abs(num5) < 1.192093E-07f)
            {
                key.setTangentOut( 0.0f );
            }
            else
            {
                key.setTangentOut( (num5 * abs((float) (num4 - num8))) / num9 );
            }
        }
        else if (tangentOutType == HCT_Linear)
        {
            key.setTangentOut( num2 - num7 );
        }
        else
        {
            key.setTangentOut( 0.0f );
        }
    }

    // --------========--------========--------========--------========--------========--------========--------========--------========
    // --------========--------========--------========--------========--------========--------========--------========--------========
    void HermiteCurve::ComputeTangents(HermiteCurveTangent tangentType)
    {
        this->ComputeTangents(tangentType, tangentType);
    }

    // --------========--------========--------========--------========--------========--------========--------========--------========
    // --------========--------========--------========--------========--------========--------========--------========--------========
    void HermiteCurve::ComputeTangents(HermiteCurveTangent tangentInType, HermiteCurveTangent tangentOutType)
    {
        for (unsigned int i = 0; i < this->mKeys.size(); i++)
        {
            this->ComputeTangent(i, tangentInType, tangentOutType);
        }
    }

    // --------========--------========--------========--------========--------========--------========--------========--------========
    // --------========--------========--------========--------========--------========--------========--------========--------========
    float HermiteCurve::_evaluateCurve(const float position, const float offset) const
    {	
        int k0, k1;
        float t = this->findSegment(position, k0, k1);
        return (offset + Hermite(this->mKeys[k0], this->mKeys[k1], t));
    }


    // --------========--------========--------========--------========--------========--------========--------========--------========
    // --------========--------========--------========--------========--------========--------========--------========--------========
    float HermiteCurve::Hermite(const CurveKey& k0, const CurveKey& k1, float t)
    {	
        if (k0.getContinuity() == CurveKey::CC_Step)
        {
            if (t >= 1.0f)
            {
                return k1.getValue();
            }
            return k0.getValue();
        }
        float num = t * t;
        float num2 = num * t;
        float internalValue = k0.getValue();
        float num5 = k1.getValue();
        float tangentOut = k0.getTangentOut();
        float tangentIn = k1.getTangentIn();
        return ((((internalValue * (((2.0f * num2) - (3.0f * num)) + 1.0f)) + (num5 * ((-2.0f * num2) + (3.0f * num)))) + (tangentOut * ((num2 - (2.0f * num)) + t))) + (tangentIn * (num2 - num)));
    }


    // --------========--------========--------========--------========--------========--------========--------========--------========
    // --------========--------========--------========--------========--------========--------========--------========--------========
    void HermiteCurve::loadFromXmlNode(xmlNodePtr root, float keyDifferenceToIgnore )
    {
        
        // look for the curve asset (we only take the first one)
        xmlNodePtr asset = root->children;
        while (asset)
        {
            if (xmlStrcmp( asset->name, (const xmlChar*)"Asset") == 0)
            {
                if (!XML::attrExists(asset, "Type"))
                {
                    Logger::printf("Walaber", Logger::SV_ERROR, "ERROR!  XML parsing error, node does not have 'Type' attribute!\n");
                    assert(0);
                }
                
                if (XML::parseString(asset, "Type") != "Framework:Curve")
                {
                    Logger::printf("Walaber", Logger::SV_ERROR, "ERROR!  XML is not Curve asset type!  It is %s.\n", XML::parseString(asset, "Type").c_str());
                    assert(0);
                }
                
                // OK, we have a valid Curve XML file.  parse up the children now.
                xmlNodePtr kid = asset->children;
                while (kid)
                {
                    if (xmlStrcmp( kid->name, (const xmlChar*)"PreLoop") == 0)
                    {
                        xmlChar* node = xmlNodeGetContent(kid);
                        std::string content = std::string((const char*)node);
                        xmlFree(node);
                        this->setPreLoop( curveLoopTypeFromString(content) );
   
                        Logger::printf("Walaber", Logger::SV_VERBOSE, "PreLoop -> got content! [%s] = preLoop[%d]\n", content.c_str(), (int)getPreLoop());
                    }
                    else if (xmlStrcmp( kid->name, (const xmlChar*)"PostLoop") == 0)
                    {
                        xmlChar* node = xmlNodeGetContent(kid);
                        std::string content = std::string((const char*)node);
                        xmlFree(node);
                        this->setPostLoop( curveLoopTypeFromString(content) );
    
                        Logger::printf("Walaber", Logger::SV_VERBOSE, "PostLoop -> got content! [%s] = postLoop[%d]\n", content.c_str(), (int)getPostLoop());
                    }
                    if (xmlStrcmp( kid->name, (const xmlChar*)"Keys") == 0)
                    {
                        xmlChar* node = xmlNodeGetContent(kid);
                        std::string content = std::string((const char*)node);
                        xmlFree(node);
                        
                        Logger::printf("Walaber", Logger::SV_VERBOSE, "Keys -> got content! [%s]\n", content.c_str());
                        
                        // parse this out...
                        std::vector<std::string> items = StringHelper::split(content, ' ');
                        
                        float lastAddedCurveKeyValue=0;
                        bool didNotAddLastCurveKey=false;
                        CurveKey lastCurveKey;
                        
                        unsigned int numItems = items.size();
                        for (unsigned int i = 0; i < numItems; i+=5)
                        {
                            float pos, val, tanIn, tanOut;
                            sscanf( items[i + 0].c_str(), "%f", &pos );
                            sscanf( items[i + 1].c_str(), "%f", &val );
                            sscanf( items[i + 2].c_str(), "%f", &tanIn );
                            sscanf( items[i + 3].c_str(), "%f", &tanOut );
                            
                            CurveKey ck( pos, val, tanIn, tanOut, CurveKey::curveContinuityFromString( items[i + 4] ) );
                            
                            bool shouldAddCurveKey=true;
                            
                            if( absf(val-lastAddedCurveKeyValue)<keyDifferenceToIgnore &&
                               mKeys.size() )
                            {
                                shouldAddCurveKey=false;
                                didNotAddLastCurveKey=true;
                                lastCurveKey=ck;
                                
                                Logger::printf("Walaber", Logger::SV_VERBOSE, "Keys -> not adding curvekey! value: [%f] last value: [%f] key difference to ignore: [%f]\n", val, lastAddedCurveKeyValue, keyDifferenceToIgnore);
                            }
                            else if(didNotAddLastCurveKey)
                            {
                                Logger::printf("Walaber", Logger::SV_VERBOSE, "Keys -> got content! position: [%f] value: [%f]\n", lastCurveKey.getPosition(), lastCurveKey.getValue());
                                
                                this->addCurveKey(lastCurveKey);
                            }
                            
                            if(shouldAddCurveKey || i == numItems-5)
                            {
                                lastAddedCurveKeyValue=val;
                                
                                didNotAddLastCurveKey=false;
                                
                                Logger::printf("Walaber", Logger::SV_VERBOSE, "Keys -> got content! position: [%f] value: [%f]\n", pos, val);
                                
                                this->addCurveKey( ck );
                            }
                        }
                    }
                    
                    kid = kid->next;
                }
                break;
            }
            
            asset = asset->next;
        }
    }
	
}

