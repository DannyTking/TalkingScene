#ifndef _HERMITE_CURVE_H_
#define _HERMITE_CURVE_H_

#include <Curve.h>

namespace Walaber
{
    class HermiteCurve : public Curve
    {
    public:	// enums, typedefs, and structs
        
        enum HermiteCurveTangent
        {
            HCT_Flat,
            HCT_Linear,
            HCT_Smooth
        };

    public: // statics
        
        static float Hermite(const CurveKey& k0, const CurveKey& k1, float t);
        
    public: // interface
     
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // default ctor
        // </Summary>
        HermiteCurve():
        Curve()
        {}
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Load a curve from a buffer that was loaded from a curve collection
        // </Summary>
        HermiteCurve(char* buf, unsigned int& bytesRead):
        Curve(buf, bytesRead)
        {}
		
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Load a curve from a buffer that was loaded from a curve collection with flags
        // </Summary>
		HermiteCurve(char* buffer, unsigned int& bytesRead, unsigned int flags, const CompressionRecord& timeCR, const CompressionRecord& valueCR, float keyDifferenceToIgnore = 0.0f ):
		Curve(buffer, bytesRead, flags, timeCR, valueCR, keyDifferenceToIgnore)
        {}
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // dtor
        // </Summary>
        ~HermiteCurve()
        {}
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // compute the in and out tangents for a key - both in and out are of same type
        // </Summary>
        void ComputeTangent(int keyIndex, HermiteCurveTangent tangentType);	
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // compute the in and out tangents for a key individually
        // </Summary>
        void ComputeTangent(int keyIndex, HermiteCurveTangent tangentInType, HermiteCurveTangent tangentOutType);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // compute the in and out tangents for all keys - both in and out are of same type
        // </Summary>
        void ComputeTangents(HermiteCurveTangent tangentType);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // compute the in and out tangents for all keys - specifying in and out separate
        // </Summary>
        void ComputeTangents(HermiteCurveTangent tangentInType, HermiteCurveTangent tangentOutType);
                                                                                    
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // implementation of base class load from XML node
        // </Summary>
        void loadFromXmlNode(xmlNodePtr root, float keyDifferenceToIgnore = 0.0f);
        
    private: // interface
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // actually evaluate the hermite curve at the position
        // </Summary>
        float _evaluateCurve(const float position, const float offset) const;
    };

}


#endif	//_CURVE_H
