#ifndef _BEZIER_CURVE_H_
#define _BEZIER_CURVE_H_

#include <Curve.h>

namespace Walaber
{
    class BezierCurve : public Curve
    {
        /*
         pass in a time between 0 and 1 as a distance between the times of curve key 0 and 1
         This function will convert that value into the curve domain and return the y value
         @param k0 keyframe A in the bezier in which to interpolate between
         @param k1 keyframe B in the bezier in which to interpolate between
         @param t normalized distance between the positions of each of the curve keys
        */
        static inline float BezierInterpY(const CurveKey& k0, const CurveKey& k1, float t);

    public: // interface
     
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // default ctor
        // </Summary>
        BezierCurve():
        Curve()
        {}
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Load a curve from a buffer that was loaded from a curve collection
        // </Summary>
        BezierCurve(char* buf, unsigned int& bytesRead):
        Curve(buf, bytesRead)
        {}
		
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Load a curve from a buffer that was loaded from a curve collection with flags
        // </Summary>
		BezierCurve(char* buffer, unsigned int& bytesRead, unsigned int flags, const CompressionRecord& timeCR, const CompressionRecord& valueCR, float keyDifferenceToIgnore = 0.0f ):
		Curve( )
        {
            std::vector< int > skippedKeys;
            
			_loadCompressedKeys( buffer, bytesRead, flags, timeCR, valueCR, keyDifferenceToIgnore,
                                skippedKeys );
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // dtor
        // </Summary>
        ~BezierCurve()
        {}
                                                                                    
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // implementation of base class load from XML node
        // </Summary>
        void loadFromXmlNode(xmlNodePtr root, float keyDifferenceToIgnore = 0.0f);
		
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // returns a pair that constains the min and max values contained in this curve
        // </Summary>
        virtual std::pair<float, float> getMinMaxNonTimeValue()
        {
			float min = 0;
			float max = 0;
			
			/* std curve data
			 float position;
			 float internalValue;
			 float handleLeftPosition;
			 float handleLeftValue;
			 float handleRightPosition;
			 float handleRightValue;
			 */
			
			min = max = mKeys[0].data.bez.internalValue;
			unsigned int size = mKeys.size();
			for(unsigned int i = 1; i < size; ++i)
			{				
				min = (Walaber::minf(min, mKeys[i].data.bez.internalValue));
				max = (Walaber::maxf(max, mKeys[i].data.bez.internalValue));
				
				min = (Walaber::minf(min, mKeys[i].data.bez.handleLeftPosition));
				max = (Walaber::maxf(max, mKeys[i].data.bez.handleLeftPosition));
				
				min = (Walaber::minf(min, mKeys[i].data.bez.handleLeftValue));
				max = (Walaber::maxf(max, mKeys[i].data.bez.handleLeftValue));
				
				min = (Walaber::minf(min, mKeys[i].data.bez.handleRightPosition));
				max = (Walaber::maxf(max, mKeys[i].data.bez.handleRightPosition));
				
				min = (Walaber::minf(min, mKeys[i].data.bez.handleRightValue));
				max = (Walaber::maxf(max, mKeys[i].data.bez.handleRightValue));
				
				Logger::printf("Walaber", Logger::SV_DEBUG, "key: [%i] pos: [%f] val: [%f] tanIn: [%f] tanOut: [%f]\n", i, mKeys[i].data.bez.position, mKeys[i].data.bez.internalValue, mKeys[i].data.bez.handleLeftPosition, mKeys[i].data.bez.handleLeftValue, mKeys[i].data.bez.handleRightPosition, mKeys[i].data.bez.handleRightPosition );
			}
			
			return std::make_pair( min, max );
        }	
		
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Writes the data of a curve object into the buffer - returns the number of bytes written
        // </Summary>
        virtual unsigned int writeToBuffer(char* buf, const unsigned int flags, const CompressionRecord& timeCR, const CompressionRecord& valueCR)
        {
            unsigned int bytesWritten = 0;
            float timeInc = 0;
			float valueInc = 0;
			
            // cannot use this flag with bezier curves
            assert( ! (flags & CCF_Skeletal_Animation_Collection ) );
            
			if( flags & CCF_1_Byte_Compression )
			{
                // we lose a value of representation because we need 0 to represent 0
                float limit = static_cast<float>(std::numeric_limits<unsigned char>::max()) - 1.0f;

				timeInc = (timeCR.mMaxValue - timeCR.mMinValue) / limit;
				valueInc = (valueCR.mMaxValue - valueCR.mMinValue) / limit;
			}
			if( flags & CCF_2_Byte_Compression )
			{
                // we lose a value of representation because we need 0 to represent 0
                float limit = static_cast<float>(std::numeric_limits<unsigned short>::max()) - 1.0f;

				timeInc = (timeCR.mMaxValue - timeCR.mMinValue) / limit;
				valueInc = (valueCR.mMaxValue - valueCR.mMinValue) / limit;
			}
            
            if( !flags )
			{
				bytesWritten = Curve::writeToBuffer(buf);
            }
			else if ( flags & CCF_1_Byte_Compression )
			{
				unsigned int numKeys = mKeys.size();
				memcpy(&buf[bytesWritten], &numKeys, sizeof(numKeys));		
				bytesWritten += sizeof(numKeys);
				
				for( unsigned int i=0; i < numKeys; ++i)
				{		
					// crunch the time
                    unsigned char val = crunchFloat<unsigned char>( mKeys[i].data.bez.position,
                                                      timeCR.mMinValue, timeInc );
                    
                    memcpy( &buf[bytesWritten], &val, sizeof(val) );
                    bytesWritten += sizeof(val);
                    
                    // crunch the value
                    val = crunchFloat<unsigned char>( mKeys[i].data.bez.internalValue,
                                                      valueCR.mMinValue, valueInc );
                    
                    memcpy( &buf[bytesWritten], &val, sizeof(val) );
                    bytesWritten += sizeof(val);
					
					// crunch the left handle position
					val = crunchFloat<unsigned char>( mKeys[i].data.bez.handleLeftPosition,
                                                      valueCR.mMinValue, valueInc );
                    
                    memcpy( &buf[bytesWritten], &val, sizeof(val) );
                    bytesWritten += sizeof(val);
                    
					// crunch the left handle value
					val = crunchFloat<unsigned char>( mKeys[i].data.bez.handleLeftValue,
                                                      valueCR.mMinValue, valueInc );
                    
                    memcpy( &buf[bytesWritten], &val, sizeof(val) );
                    bytesWritten += sizeof(val);
                    
                    // crunch the right handle position
					val = crunchFloat<unsigned char>( mKeys[i].data.bez.handleRightPosition,
                                                      valueCR.mMinValue, valueInc );
                    
                    memcpy( &buf[bytesWritten], &val, sizeof(val) );
                    bytesWritten += sizeof(val);
                    
					// crunch the right handle value
					val = crunchFloat<unsigned char>( mKeys[i].data.bez.handleRightValue,
                                                      valueCR.mMinValue, valueInc );
                    
                    memcpy( &buf[bytesWritten], &val, sizeof(val) );
                    bytesWritten += sizeof(val);
				}
			}
			else if ( flags & CCF_2_Byte_Compression )
			{
				unsigned int numKeys = mKeys.size();
				memcpy(&buf[bytesWritten], &numKeys, sizeof(numKeys));		
				bytesWritten += sizeof(numKeys);
				
				for( unsigned int i=0; i < numKeys; ++i)
				{		
					// crunch the time
                    unsigned short val = crunchFloat<unsigned short>( mKeys[i].data.bez.position,
                                                      timeCR.mMinValue, timeInc );
                    
                    memcpy( &buf[bytesWritten], &val, sizeof(val) );
                    bytesWritten += sizeof(val);
                    
                    // crunch the value
                    val = crunchFloat<unsigned short>( mKeys[i].data.bez.internalValue,
                                                      valueCR.mMinValue, valueInc );
                    
                    memcpy( &buf[bytesWritten], &val, sizeof(val) );
                    bytesWritten += sizeof(val);
					
					// crunch the left handle position
					val = crunchFloat<unsigned short>( mKeys[i].data.bez.handleLeftPosition,
                                                      valueCR.mMinValue, valueInc );
                    
                    memcpy( &buf[bytesWritten], &val, sizeof(val) );
                    bytesWritten += sizeof(val);
                    
					// crunch the left handle value
					val = crunchFloat<unsigned short>( mKeys[i].data.bez.handleLeftValue,
                                                      valueCR.mMinValue, valueInc );
                    
                    memcpy( &buf[bytesWritten], &val, sizeof(val) );
                    bytesWritten += sizeof(val);
                    
                    // crunch the right handle position
					val = crunchFloat<unsigned short>( mKeys[i].data.bez.handleRightPosition,
                                                      valueCR.mMinValue, valueInc );
                    
                    memcpy( &buf[bytesWritten], &val, sizeof(val) );
                    bytesWritten += sizeof(val);
                    
					// crunch the right handle value
					val = crunchFloat<unsigned short>( mKeys[i].data.bez.handleRightValue,
                                                      valueCR.mMinValue, valueInc );
                    
                    memcpy( &buf[bytesWritten], &val, sizeof(val) );
                    bytesWritten += sizeof(val);
				}
			}
            
            return bytesWritten;
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        //returns the size in bytes of the curve object
        // </Summary>
        virtual size_t getWriteSize(unsigned int flags)
        {
			size_t size  = 0;
			if( !flags )
			{
				size = mKeys.getWriteSize();
				size += sizeof(mPreLoop);
				size += sizeof(mPostLoop);
				size += sizeof(mTimeRange);
				size += sizeof(mInvTimeRange);
            }
			else if ( flags & CCF_Skeletal_Animation_Collection )
			{
                // The skeletal animation
                printf("You Cannot Use the SKELETAL_ANIMATION_COLLECTION Flag with Bezier curves."
                       "This flag is deprecated!!");
				assert( 0 );
			}
			else if ( flags & CCF_1_Byte_Compression )
			{
				// if there is a compression specified, it means that we need to save space, so don't write out the padding! This is a super manual function, so this feels kind of dirty
				/*
				 float position;
                 float internalValue;
                 float handleLeftPosition;
                 float handleLeftValue;
                 float handleRightPosition;
                 float handleRightValue;
                 */
				
				unsigned int numKeys = mKeys.size();
				size += sizeof(numKeys);
				
				// we're converting all of the floats to chars, so divide size of the struct by a
                // float
				size += ( sizeof( CurveKey::CurveDataBezier ) / sizeof( float ) ) * numKeys;
			}
			else if ( flags & CCF_2_Byte_Compression )
			{
				// if there is a compression specified, it means that we need to save space, so don't write out the padding! This is a super manual function, so this feels kind of dirty
				/*
				 float position;
                 float internalValue;
                 float handleLeftPosition;
                 float handleLeftValue;
                 float handleRightPosition;
                 float handleRightValue;
				 */
				
				unsigned int numKeys = mKeys.size();
				size += sizeof(numKeys);
				
				// we're converting all of the floats to chars, so divide size of the struct by a
                // float
				size += ( sizeof( CurveKey::CurveDataBezier ) / sizeof( unsigned short ) ) * numKeys;
			}
			
			return size;
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // print out values
        // </Summary>
        virtual void print( Logger::Severity severity = Logger::SV_VERBOSE )
        {
            _printBezierKeys( severity );
            _printData( severity );
        }
        
        
    private: // interface
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // actually evaluate the hermite curve at the position
        // </Summary>
        float _evaluateCurve(const float position, const float offset) const;
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // loads compressed keyframes. If keyframes are not compressed, they should not use this
        // function
        // </Summary>
        virtual void _loadCompressedKeys( char* buffer, unsigned int& bytesRead, unsigned int flags,
                                         const CompressionRecord& timeCR,
                                         const CompressionRecord& valueCR,
                                         const float keyDifferenceToIgnore,
                                         std::vector< int >& outSkippedKeyIndices );
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // print out key frame data
        // </Summary>
        void _printBezierKeys( Logger::Severity severity)
        {
            for ( unsigned int i=0; i < mKeys.size(); ++i)
            {
                Logger::printf("Walaber", severity,
                               "Key: [ %i ]\n", i );
                mKeys[i].printBezier( severity );
            }
        }
    };

}


#endif	//_BEZIER_CURVE_H_
