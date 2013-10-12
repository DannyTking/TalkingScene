//
//  CurveLoading.h
//  WalaberEngine
//
//  Created by Pecho, Jason on 5/4/13.
//
//

#ifndef _CURVE_LOADING_H_
#define _CURVE_LOADING_H_

#include <Curve.h>
#include <HermiteCurve.h>
#include <BezierCurve.h>
#include <LinearCurve.h>
#include <TweenedLinearCurve.h>

namespace Walaber
{
    inline unsigned int writeStringToBuffer( const std::string& string, char* buf )
    {
        unsigned int bytesWritten = 0;
        unsigned int stringSize = string.size()+1; // +1 to include null terminator
        if(stringSize != 0)
        {
            stringSize = (((stringSize+4)-1)/4)*4; // round to the nearest multiple of 4
            
        }
        memcpy(buf, &stringSize, sizeof(unsigned int));
        bytesWritten += sizeof(unsigned int);
        
        unsigned int stringLength = string.size();
        memcpy(&buf[bytesWritten], string.c_str(), stringLength+1);
        bytesWritten += stringLength+1;
        
        unsigned int filler = stringSize - (stringLength + 1);
        memset(&buf[bytesWritten], 0, filler);
        bytesWritten += filler;
        
        return bytesWritten;
    }
    
    inline unsigned int getStringWriteSize( const std::string& string )
    {
        // size of object
        unsigned int writeSize = 0;
        writeSize += sizeof(int); // to store size of string
        unsigned int stringSize = string.size()+1;
        if(stringSize != 0)
        {
            stringSize = (((stringSize+4)-1)/4)*4; // round to the nearest multiple of 4
            
        }
        writeSize += stringSize;
        
        return writeSize;
    }
    
    inline std::string readStringFromBuffer( char * buffer, unsigned int& bytesRead )
    {
        bytesRead = 0;
        unsigned int stringSize;
        memcpy(&stringSize, buffer, sizeof(unsigned int));
        bytesRead += sizeof(unsigned int);
        
        char * readString = &buffer[bytesRead];
        bytesRead += stringSize;
        return std::string( readString );
    }
    
    inline Curve* createCurveForTypeFromBuffer( CurveType type, char* buf, unsigned int& bytesRead )
    {
        Curve* curve = 0;
        
        switch(type)
        {
            case CT_Hermite:
            {
                curve = new HermiteCurve( buf, bytesRead );
            }
                break;
                
            case CT_Bezier:
            {
                curve = new BezierCurve( buf, bytesRead );
            }
                break;
                
            case CT_Linear:
            {
                curve = new LinearCurve( buf, bytesRead );
            }
                break;
                
            case CT_TweenedLinear:
            {
                curve = new TweenedLinearCurve(buf, bytesRead );
            }
                break;
                
            default:
                // type came back FUBAR
                assert(0);
        }
        
        return curve;
    }

    inline Curve* createCurveForTypeFromBuffer( CurveType type, unsigned int flags, char* buf, const CompressionRecord& timeCR, const CompressionRecord& valueCR, unsigned int& bytesRead, float keyDifferenceToIgnore )
    {
        Curve* curve = 0;
        
        switch(type)
        {
            case CT_Hermite:
            {
                curve = new HermiteCurve( buf, bytesRead, flags, timeCR, valueCR, keyDifferenceToIgnore );
            }
                break;
                
            case CT_Bezier:
            {
                curve = new BezierCurve( buf, bytesRead, flags, timeCR, valueCR, keyDifferenceToIgnore );
            }
                break;
                
            case CT_Linear:
            {
                curve = new LinearCurve( buf, bytesRead, flags, timeCR, valueCR, keyDifferenceToIgnore );
            }
                break;
                
            case CT_TweenedLinear:
            {
                curve = new TweenedLinearCurve(buf, bytesRead, flags, timeCR, valueCR, keyDifferenceToIgnore );
            }
                break;
                
            default:
                // type came back FUBAR
                assert(0);
        }
        
        return curve;
    }
    
    inline Curve* createCurveForTypeFromBuffer( CurveType type, unsigned int flags, char* buf, const CompressionRecord& timeCR, const CompressionRecord& valueCR, unsigned int& bytesRead )
    {
        return createCurveForTypeFromBuffer( type, flags, buf, timeCR, valueCR, bytesRead, 0.0f );
    }
}
#endif // _CURVE_LOADING_H_
