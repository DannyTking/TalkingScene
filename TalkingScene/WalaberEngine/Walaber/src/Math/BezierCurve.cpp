#include "BezierCurve.h"
#include "VectorTools.h"

#include <math.h>
#include <StringHelper.h>

namespace Walaber
{
    /*
    static const unsigned int NUM_STEPS=25;
    
    static const float subdiv_step  = 1.0f / (float)(NUM_STEPS);
    static const float subdiv_step2 = subdiv_step*subdiv_step;
    static const float subdiv_step3 = subdiv_step*subdiv_step*subdiv_step;
    
    static const float pre1 = 3.0 * subdiv_step;
    static const float pre2 = 3.0 * subdiv_step2;
    static const float pre4 = 6.0 * subdiv_step2;
    static const float pre5 = 6.0 * subdiv_step3;
    */
    
    // --------========--------========--------========--------========--------========--------========--------========--------========
    // --------========--------========--------========--------========--------========--------========--------========--------========
    float BezierCurve::_evaluateCurve(const float position, const float offset) const
    {	
        int k0, k1;
        float t = this->findSegment(position, k0, k1);
        float interp = BezierInterpY(this->mKeys[k0], this->mKeys[k1], t);
        return (offset + interp);
    }


    // --------========--------========--------========--------========--------========--------========--------========--------========
    // --------========--------========--------========--------========--------========--------========--------========--------========
    float BezierCurve::BezierInterpY(const CurveKey& k0, const CurveKey& k1, float t)
    {
        /* Using forward differences instead of brute force
         impl - http://antigrain.com/research/bezier_interpolation/index.html#PAGE_BEZIER_INTERPOLATION
         math - http://collaboration.cmc.ec.gc.ca/science/rpn/biblio/ddj/Website/articles/CUJ/1997/9711/bartley/bartley.htm
         @ TODO: make this adpative so it's not one number of steps for every curve segment
            source - http://antigrain.com/research/adaptive_bezier/
            paper - S. Lien, M. Shantz, and V. Pratt. "Adaptive Forward Differencing for Rendering Curves and Surfaces," Computer Graphics (SIGGRAPH), 1987
        */
        /*

        Vector2 p0 = k0.asVector2();
        Vector2 p1 = k0.getHandleRight();
        Vector2 p2 = k1.getHandleLeft();
        Vector2 p3 = k1.asVector2();
        
        float tmp1x = p0.Y - p1.Y * 2.0f + p2.Y;
        
        float tmp2x = (p1.Y - p2.Y) * 3.0f - p0.Y + p3.Y;
        
        float fx = p0.Y;
        
        float dfx = ( p1.Y - p0.Y ) * pre1 + tmp1x * pre2 + tmp2x * subdiv_step3;
        
        float ddfx = tmp1x*pre4 + tmp2x*pre5;
        
        float dddfx = tmp2x*pre5;
        
        int step = 0;
        
        // Suppose, we have some abstract object Polygon which
        // has method AddVertex(x, y), similar to LineTo in
        // many graphical APIs.
        // Note, that the loop has only operation add!
        float last = 0;
        float lastPoint;
        while( step <= NUM_STEPS )
        {
            float dist = (float)step * subdiv_step;
            
            if( t > last && t < dist )
            {
                float l = lerp( lastPoint, fx+dfx, (t-last) / (dist - last) );
                //printf("BezierLerp [%f][%f] -> t: %f    bezierY: %f \n", k0.getPosition(), k1.getPosition(), t, fx );

                return l;
            }
            else if( isEqualf( t, dist ) )
            {
                //printf("Bezier [%f][%f] -> t: %f    bezierY: %f \n", k0.getPosition(), k1.getPosition(), t, fx );

                return fx;
            }
            
            fx   += dfx;
            
            dfx  += ddfx;
            ddfx += dddfx;
            
            last = dist;
            lastPoint = fx;
            
            ++step;
        }
        
        //printf("Bezier [%f][%f] -> t: %f    bezierY: %f \n", k0.getPosition(), k1.getPosition(), t, fx );
        return fx;
         */
        
        static const int MAX_ITER = 26;
        
        int     i = 0;
        float   u;
        float   u1 = 0.0f;
        float   u2 = 1.0f;
        
        float bezierPt;
        
        float x = k0.asVector2().X + ( ( k1.asVector2().X - k0.asVector2().X ) * t );
        // slow iterative curve subdivision.
        do
        {
            u = 0.5 * (u1 + u2);
            bezierPt =  bezierInterp(k0.getPosition(), k0.getHandleRightPosition(),
                                     k1.getPosition(), k1.getHandleLeftPosition(),
                                     u);
            if (bezierPt < x)
            {
                u1 = u;
            }
            else
            {
                u2 = u;
            }
        }
        while( !isEqualf(bezierPt, x) && (++i < MAX_ITER));
        
        float bezierY = bezierInterp(k0.getValue(), k0.getHandleRightValue(),
                                     k1.getValue(), k1.getHandleLeftValue(),
                                     u);
        
        return bezierY;
	}


    // --------========--------========--------========--------========--------========--------========--------========--------========
    // --------========--------========--------========--------========--------========--------========--------========--------========
    void BezierCurve::loadFromXmlNode(xmlNodePtr root, float keyDifferenceToIgnore )
    {
        
        // look for the curve asset (we only take the first one)
        xmlNodePtr child = root->children;
        while (child)
        {            
            if (xmlStrcmp( child->name, (const xmlChar*)"PreLoop") == 0)
            {
                std::string content = XML::parseString(child, "value");
                
                this->setPreLoop( curveLoopTypeFromString(content) );
				
				Logger::printf("Walaber", Logger::SV_VERBOSE, "PreLoop -> got content! [%s] = preLoop[%d]\n", content.c_str(), (int)getPreLoop());
            }
            else if (xmlStrcmp( child->name, (const xmlChar*)"PostLoop") == 0)
            {
                std::string content = XML::parseString(child, "value");
                
                this->setPostLoop( curveLoopTypeFromString(content) );
				
                Logger::printf("Walaber", Logger::SV_VERBOSE, "PostLoop -> got content! [%s] = postLoop[%d]\n", content.c_str(), (int)getPostLoop());
            }
            if (xmlStrcmp( child->name, (const xmlChar*)"Keys") == 0)
            {
                xmlNodePtr key = child->children;
                
                float lastAddedCurveKeyValue=0;
                bool didNotAddLastCurveKey=false;
                CurveKey lastCurveKey;
                
                while(key)
                {
                    if(xmlStrcmp( key->name, (const xmlChar*)"Key") == 0)
                    {
                        float position = XML::parseFloat(key, "position");
                        float value = XML::parseFloat(key, "value");
						
                        Vector2 handle_left = XML::parseVector2(key, "handle_left");
                        Vector2 handle_right = XML::parseVector2(key, "handle_right");
                        
                        CurveKey ck(position, value);
                        ck.setHandleLeft( handle_left.X, handle_left.Y );
                        ck.setHandleRight( handle_right.X, handle_right.Y );
                        
                        bool shouldAddCurveKey=true;
                        
                        
                        if(absf(value-lastAddedCurveKeyValue)<keyDifferenceToIgnore &&
                           mKeys.size() )
                        {
                            shouldAddCurveKey=false;
                            didNotAddLastCurveKey=true;
                            lastCurveKey=ck;
                            
                            Logger::printf("Walaber", Logger::SV_VERBOSE, "Keys -> not adding curvekey! value: [%f] last value: [%f] key difference to ignore: [%f]\n", value, lastAddedCurveKeyValue, keyDifferenceToIgnore);
                        }
                        else if(didNotAddLastCurveKey)
                        {
                            Logger::printf("Walaber", Logger::SV_VERBOSE, "Keys -> got content! position: [%f] value: [%f]\n", lastCurveKey.getPosition(), lastCurveKey.getValue());
                            
                            addCurveKey(lastCurveKey);
                        }
                        
                        if(shouldAddCurveKey)
                        {
                            lastAddedCurveKeyValue=value;
                            
                            didNotAddLastCurveKey=false;
                            
                            //Logger::printf("Walaber", Logger::SV_DEBUG, "Keys -> got content! position: [%f] value: [%f]\n", position, value);
                            
                            addCurveKey(ck);
                        }
                    }
                    key = key->next;
                }
                
                if( didNotAddLastCurveKey )
                {
                    addCurveKey( lastCurveKey );
                }
            }
            
            child = child->next;
        }
    }
    
    // -------------==============-------------==============-------------==============-------------===
    // <Summary>
    // loads compressed keyframes. If keyframes are not compressed, they should not use this
    // function
    // </Summary>
    void BezierCurve::_loadCompressedKeys( char* buffer, unsigned int& bytesRead, unsigned int flags, const CompressionRecord& timeCR, const CompressionRecord& valueCR, const float keyDifferenceToIgnore, std::vector< int >& outSkippedKeyIndices )
    {
        assert( ( flags & CCF_1_Byte_Compression ) || ( flags & CCF_2_Byte_Compression ) &&
                !( flags & CCF_Skeletal_Animation_Collection ) );
        
        unsigned int numKeys = 0;
        memcpy(&numKeys, &buffer[bytesRead], sizeof(numKeys) );
        bytesRead += sizeof(numKeys);
        
        float lastAddedCurveKeyValue=0;
        bool didNotAddLastCurveKey=false;
        CurveKey lastCurveKey;
        std::vector<int>            mKeysIgnored;
        
        float timeInc;
        float valueInc;
        unsigned int sizeOfReadValue;
        // we don't unpack all of the key data, we generate some of it
        if (flags & CCF_1_Byte_Compression )
        {
            // we lose a value of representation because we need 0 to represent 0
            float limit = static_cast<float>(std::numeric_limits<unsigned char>::max()) - 1.0f;
            
            timeInc = (timeCR.mMaxValue - timeCR.mMinValue) / limit;
            valueInc = (valueCR.mMaxValue - valueCR.mMinValue) / limit;
            sizeOfReadValue = sizeof(unsigned char);
        }
        else if (flags & CCF_2_Byte_Compression )
        {
            // we lose a value of representation because we need 0 to represent 0
            float limit = static_cast<float>(std::numeric_limits<unsigned short>::max()) - 1.0f;
            
            timeInc = (timeCR.mMaxValue - timeCR.mMinValue) / limit;
            valueInc = (valueCR.mMaxValue - valueCR.mMinValue) / limit;
            sizeOfReadValue = sizeof(unsigned short);
        }
        else
        {
            timeInc = 1.0f;
            valueInc = 1.0f;
            sizeOfReadValue = sizeof( float );
        }
        
        if (flags & CCF_1_Byte_Compression )
        {
            for (unsigned int i=0; i < numKeys; ++i)
            {
                CurveKey ck;
                
                ck.fillBezierOneByteCompression( timeInc,
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
                
                ck.fillBezierTwoByteCompression( timeInc,
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

