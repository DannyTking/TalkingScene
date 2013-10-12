//
//  LinearCurve.h
//  WalaberEngine
//
//  Created by Jason Pecho on 5/12/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef _LINEAR_CURVE_H_
#define _LINEAR_CURVE_H_

#include <Curve.h>

namespace Walaber
{
    class LinearCurve : public Curve
    {        
    public: // interface
     
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // default ctor
        // </Summary>
        LinearCurve():
        Curve()
        {}
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Load a curve from a buffer that was loaded from a curve collection
        // </Summary>
        LinearCurve(char* buf, unsigned int& bytesRead):
        Curve(buf, bytesRead)
        {}
        
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Load a curve from a buffer that was loaded from a curve collection with flags
        // </Summary>
		LinearCurve(char* buffer, unsigned int& bytesRead, unsigned int flags, const CompressionRecord& timeCR, const CompressionRecord& valueCR, float keyDifferenceToIgnore = 0.0f ):
		Curve(buffer, bytesRead, flags, timeCR, valueCR, keyDifferenceToIgnore)
        {}
		
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // inserts a single a key at 0 with the value value
        // </Summary>
		LinearCurve( float value )
        {
            addCurveKey( CurveKey(0.0f, value ) );
            computeTangents();
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // dtor
        // </Summary>
        virtual ~LinearCurve()
        {}
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // implementation of base class load from XML node
        // </Summary>
        virtual void loadFromXmlNode(xmlNodePtr root, float keyDifferenceToIgnore = 0.0f );
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // returns a pair that constains the min and max values contained in this curve
        // </Summary>
        virtual std::pair<float, float> getMinMaxNonTimeValue()
        {			
			return getMinMaxKeyValue();
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // go through all keys and compute linear tangents
        // </Summary>
        virtual void computeTangents();
        
    private: // interface
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // actually evaluate the hermite curve at the position
        // </Summary>
        virtual float _evaluateCurve(const float position, const float offset) const;
    };

}


#endif	//_CURVE_H
