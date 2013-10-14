//
//  Curve.h
//  WalaberEngine
//
//  Created by Jason Pecho on 5/12/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef _CURVE_H_
#define _CURVE_H_

#include <DynamicArray.h>
#include <string>
#include <limits>
#include <macros.h>
#include <XMLHelper.h>
#include <Logger.h>
#include <Vector2.h>

#define	CURVE_COLLECTION_FILE_HEADER_MAGIC				0xC081EC55
#define CURVE_COLLECTION_FILE_HEADER_MAGIC_V1			0xC081EC54
#define CURVE_COLLECTION_VERSION						3	

namespace Walaber
{
    // crunches a float value to a unsigned char or unsigned short
    template< class T >
    T crunchFloat( float value, float minVal, float increment );
    
    // decompresses a crunched value from a buffer and increments bytes read
    template< class T >
    float decompressValueFromBuffer( const char* const buffer, float minVal, float increment,
                                    unsigned int& bytesRead );
	
    struct CurveCollectionFileHeader
    {
        const int			mMagic;
		unsigned int		mVersion;
        unsigned int		mNumCurves;
		unsigned int		mFlags;
        unsigned int        mNumGroups;
		
        
        CurveCollectionFileHeader():
        mMagic(CURVE_COLLECTION_FILE_HEADER_MAGIC),
		mVersion(CURVE_COLLECTION_VERSION),
		mNumCurves(0),
		mFlags(0),
        mNumGroups( 0 )
        {}
    };
    
    struct CurveCollectionFileHeader_V2
    {
        const int			mMagic;
		unsigned int		mVersion;
        unsigned int		mNumCurves;
		unsigned int		mFlags;		
        
        CurveCollectionFileHeader_V2():
        mMagic(CURVE_COLLECTION_FILE_HEADER_MAGIC),
		mVersion( 2 ),
		mNumCurves(0),
		mFlags(0)
        {}
    };
    
	struct CurveCollectionFileHeader_V1
    {
        const int			mMagic;
        unsigned int		mNumCurves;
		
        
        CurveCollectionFileHeader_V1():
        mMagic(CURVE_COLLECTION_FILE_HEADER_MAGIC_V1),
		mNumCurves(0)
        {}
    };
	
	// A compression record allows us to know how we can compress a float based on the min value and the range
	struct CompressionRecord
	{
		float	mMinValue;
		float	mMaxValue;
		
		CompressionRecord():
		mMinValue(0.0f),
		mMaxValue(0.0f)
		{}
	};
	
	enum CurveCollectionFlags
	{
		CCF_1_Byte_Compression				= 1,
		CCF_2_Byte_Compression				= 2,
		CCF_Skeletal_Animation_Collection    = 4,
	};
    
    enum CurveType
    {
        CT_Hermite,
        CT_Bezier,
        CT_Linear,
        CT_TweenedLinear,
    };
	
	enum AnimationCurveType
	{
		AnimationCurveType_None,
		AnimationCurveType_PosX,
		AnimationCurveType_PosY,
		AnimationCurveType_AngleZ,
        
        // for backwards compatibility sake, we should not change the previous values
        AnimationCurveType_PosZ,
        
        AnimationCurveType_AngleX,
        AnimationCurveType_AngleY,
        
        AnimationCurveType_ScaleX,
        AnimationCurveType_ScaleY,
        AnimationCurveType_ScaleZ,
        
        AnimationCurveType_SkewGradient,
	};
	
	enum AnimationCurveCompressionTracks  
	{
		AnimationCurveCompressionTrack_POS_X,
		AnimationCurveCompressionTrack_POS_Y,
		AnimationCurveCompressionTrack_ANGLE_Z,
		AnimationCurveCompressionTrack_TIME,
        
        // for backwards compatibility sake, we should not change the previous values
        AnimationCurveCompressionTrack_POS_Z,
        
        AnimationCurveCompressionTrack_ANGLE_X,
        AnimationCurveCompressionTrack_ANGLE_Y,
        
        AnimationCurveCompressionTrack_SCALE_X,
        AnimationCurveCompressionTrack_SCALE_Y,
        AnimationCurveCompressionTrack_SCALE_Z,
        
        AnimationCurveCompressionTrack_SKEW_GRADIENT,
		
		AnimationCurveCompressionTrack_SIZE
	};
	
    // --------========--------========--------========--------========--------========--------========---
    class CurveKey
    {
    public: // enums typedefs and structs
        
        enum CurveContinuity
        {
            CC_Smooth,
            CC_Step,
            CC_DWORD = 0xFFFFFFFF,
        };
		
		// --------========--------========--------========--------========--------========--------========---
		struct CurveDataStandard
		{
			float position;
			float internalValue;
			float tangentIn;
			float tangentOut;
			CurveContinuity continuity;
			int padding;
		};
		
		// --------========--------========--------========--------========--------========--------========---
		struct CurveDataBezier
		{
			float position;
			float internalValue;
			float handleLeftPosition;
			float handleLeftValue;
			float handleRightPosition;
			float handleRightValue;
		};

        
    public: // statics
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // op < only compares position
        // </Summary>
        static CurveContinuity curveContinuityFromString( std::string& s );
    
    public: // interface
        
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // pos and val ctor
        // </Summary>
        CurveKey()
        {
            this->data.std.position = 0.0f;
            this->data.std.internalValue = 0.0f;
            this->data.std.tangentIn = this->data.std.tangentOut = 0.0f;
            this->data.std.continuity = CC_Smooth;
        }
		
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // pos and val ctor
        // </Summary>
        CurveKey(float position, float value)
        {
            this->data.std.position = position;
            this->data.std.internalValue = value;
            this->data.std.tangentIn = this->data.std.tangentOut = 0.0f;
            this->data.std.continuity = CC_Smooth;
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // all but continuity ctor
        // </Summary>
        CurveKey(float position, float value, float tangentIn, float tangentOut)
        {
            this->data.std.position = position;
            this->data.std.internalValue = value;
            this->data.std.tangentIn = tangentIn;
            this->data.std.tangentOut = tangentOut;
            this->data.std.continuity = CC_Smooth;
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // everything
        // </Summary>
        CurveKey(float position, float value, float tangentIn, float tangentOut, CurveContinuity continuity)
        {
            this->data.std.position = position;
            this->data.std.internalValue = value;
            this->data.std.tangentIn = tangentIn;
            this->data.std.tangentOut = tangentOut;
            this->data.std.continuity = continuity;
        }
		
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // everything (bezier)
        // </Summary>
        CurveKey(float position, float value, const Vector2& handle_left, const Vector2& handle_right )
        {
            this->data.bez.position = position;
            this->data.bez.internalValue = value;
            this->data.bez.handleLeftPosition = handle_left.X;
			this->data.bez.handleLeftValue = handle_left.Y;
			this->data.bez.handleRightPosition = handle_right.X;
			this->data.bez.handleRightValue = handle_right.Y;	
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // op = 
        // </Summary>
        inline CurveKey& operator = (const CurveKey& ck)
        {
            data.std.position = ck.data.std.position;
            data.std.internalValue = ck.data.std.internalValue;            
            data.std.tangentIn = ck.data.std.tangentIn;
            data.std.tangentOut = ck.data.std.tangentOut;			
            data.std.continuity = ck.data.std.continuity;
            data.std.padding = ck.data.std.padding;
            
            return *this;
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // boolean op ==
        // </Summary>
        inline bool operator == (const CurveKey& ck )
        {
            return ((this->data.std.position == ck.data.std.position) && (this->data.std.internalValue == ck.data.std.internalValue) && (this->data.std.tangentIn == ck.data.std.tangentIn) && (this->data.std.tangentOut == ck.data.std.tangentOut) && (this->data.std.continuity == ck.data.std.continuity));
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // op > only compares position
        // </Summary>
        inline bool operator > (const CurveKey& rhs)
        {
            return (this->data.std.position > rhs.data.std.position);
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // op < only compares position
        // </Summary>
        inline bool operator < (const CurveKey& rhs)
        {
            return (this->data.std.position < rhs.data.std.position);
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // return continuity
        // </Summary>
        CurveContinuity getContinuity() const { return this->data.std.continuity; }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set continuity
        // </Summary>
        void setContinuity( CurveContinuity c ) { this->data.std.continuity = c; }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // get position on timeline
        // </Summary>
        float getPosition() const { return this->data.std.position; }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set position on timeline
        // </Summary>
        void setPosition( float p ) { this->data.std.position = p; }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // get the in tangent
        // </Summary>
        float getTangentIn() const { return this->data.std.tangentIn; }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set the in tangent
        // </Summary>
        void setTangentIn( float ti ) { this->data.std.tangentIn = ti; }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // get the out tangent
        // </Summary>
        float getTangentOut() const { return this->data.std.tangentOut; }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set the out tangent
        // </Summary>
        void setTangentOut( float to ) { this->data.std.tangentOut = to; }
        
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set the left handle
        // </Summary>
		void setHandleLeft( const float pos, const float val ) { this->data.bez.handleLeftPosition = pos; this->data.bez.handleLeftValue = val; }
		
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // get the left handle position
        // </Summary>
        Vector2 getHandleLeft() const { return Vector2( data.bez.handleLeftPosition, data.bez.handleLeftValue ); }
        
        float getHandleLeftPosition() const
        {
            return this->data.bez.handleLeftPosition;
        }
        
        float getHandleLeftValue() const
        {
            return this->data.bez.handleLeftValue;
        }
        
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set the right handle
        // </Summary>
		void setHandleRight( const float pos, const float val ) { this->data.bez.handleRightPosition = pos; this->data.bez.handleRightValue = val; }
		
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // get the right handle position
        // </Summary>
        Vector2 getHandleRight() const { return Vector2( data.bez.handleRightPosition, data.bez.handleRightValue ); }
        
        float getHandleRightPosition() const
        {
            return this->data.bez.handleRightPosition;
        }
        
        float getHandleRightValue() const
        {
            return this->data.bez.handleRightValue;
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set the value at this key
        // </Summary>
        float getValue() const { return this->data.std.internalValue; }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // get the value at this key
        // </Summary>
        void setValue( float v ) { this->data.std.internalValue = v; }
        
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // get position and value as vector2
        // </Summary>
		Vector2 asVector2() const { return Vector2( data.std.position, data.std.internalValue ); }
        
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // fills a standard curve with values decompressed from a unsigned char to a float
        // </Summary>
        void fillStandardOneByteCompression(const float timeInc, const float timeMin,
                                            const float valueInc, const float valueMin,
                                            char* buffer, unsigned int& bytesRead )        
        
        {
            data.std.position = decompressValueFromBuffer< unsigned char >( buffer, timeMin,
                                                                            timeInc, bytesRead );
            
            data.std.internalValue = decompressValueFromBuffer< unsigned char >( buffer, valueMin,
                                                                                 valueInc, bytesRead );
            
            data.std.tangentIn = decompressValueFromBuffer< unsigned char >( buffer, valueMin,
                                                                             valueInc, bytesRead );
            
            data.std.tangentOut = decompressValueFromBuffer< unsigned char >( buffer, valueMin,
                                                                              valueInc, bytesRead );
            
            data.std.continuity = (CurveKey::CurveContinuity)buffer[bytesRead];
            bytesRead += sizeof(unsigned char);
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // fills a standard curve with values decompressed from a unsigned short to a float
        // </Summary>
        void fillStandardTwoByteCompression(const float timeInc, const float timeMin,
                                            const float valueInc, const float valueMin,
                                            char* buffer, unsigned int& bytesRead )        
        
        {
            data.std.position = decompressValueFromBuffer< unsigned short >( buffer, timeMin,
                                                                           timeInc, bytesRead );
            
            data.std.internalValue = decompressValueFromBuffer< unsigned short >( buffer, valueMin,
                                                                                valueInc, bytesRead );
            
            data.std.tangentIn = decompressValueFromBuffer< unsigned short >( buffer, valueMin,
                                                                            valueInc, bytesRead );
            
            data.std.tangentOut = decompressValueFromBuffer< unsigned short >( buffer, valueMin,
                                                                             valueInc, bytesRead );
            
            data.std.continuity = (CurveKey::CurveContinuity)buffer[bytesRead];
            bytesRead += sizeof(unsigned char);
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // fills a standard curve with values decompressed from a unsigned char to a float
        // </Summary>
        void fillBezierOneByteCompression(const float timeInc, const float timeMin,
                                            const float valueInc, const float valueMin,
                                            char* buffer, unsigned int& bytesRead )
        
        {
            data.bez.position = decompressValueFromBuffer< unsigned char >( buffer, timeMin,
                                                                           timeInc, bytesRead );
            
            data.bez.internalValue = decompressValueFromBuffer< unsigned char >( buffer, valueMin,
                                                                                valueInc, bytesRead );
            
            data.bez.handleLeftPosition = decompressValueFromBuffer< unsigned char >( buffer, valueMin,
                                                                            valueInc, bytesRead );
            
            data.bez.handleLeftValue = decompressValueFromBuffer< unsigned char >( buffer, valueMin,
                                                                             valueInc, bytesRead );
            
            data.bez.handleRightPosition = decompressValueFromBuffer< unsigned char >( buffer, valueMin,
                                                                                     valueInc, bytesRead );
            
            data.bez.handleRightValue = decompressValueFromBuffer< unsigned char >( buffer, valueMin,
                                                                                  valueInc, bytesRead );
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // fills a standard curve with values decompressed from a unsigned short to a float
        // </Summary>
        void fillBezierTwoByteCompression(const float timeInc, const float timeMin,
                                            const float valueInc, const float valueMin,
                                            char* buffer, unsigned int& bytesRead )
        
        {
            data.bez.position = decompressValueFromBuffer< unsigned short >( buffer, timeMin,
                                                                           timeInc, bytesRead );
            
            data.bez.internalValue = decompressValueFromBuffer< unsigned short >( buffer, valueMin,
                                                                                valueInc, bytesRead );
            
            data.bez.handleLeftPosition = decompressValueFromBuffer< unsigned short >( buffer, valueMin,
                                                                                     valueInc, bytesRead );
            
            data.bez.handleLeftValue = decompressValueFromBuffer< unsigned short >( buffer, valueMin,
                                                                                  valueInc, bytesRead );
            
            data.bez.handleRightPosition = decompressValueFromBuffer< unsigned short >( buffer, valueMin,
                                                                                      valueInc, bytesRead );
            
            data.bez.handleRightValue = decompressValueFromBuffer< unsigned short >( buffer, valueMin,
                                                                                   valueInc, bytesRead );
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // fills a curve's position and value with values decompressed from a unsigned char to a
        // float
        // </Summary>
        void fillPositionValueOneByteCompression(const float timeInc, const float timeMin,
                                                 const float valueInc, const float valueMin,
                                                 char* buffer, unsigned int& bytesRead )
        
        {
            data.std.position = decompressValueFromBuffer< unsigned char >( buffer, timeMin,
                                                                           timeInc, bytesRead );
            
            data.std.internalValue = decompressValueFromBuffer< unsigned char >( buffer, valueMin,
                                                                                valueInc, bytesRead );
    
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // fills a curve's position and value with values decompressed from a unsigned short to a
        // float
        // </Summary>
        void fillPositionValueTwoByteCompression(const float timeInc, const float timeMin,
                                                 const float valueInc, const float valueMin,
                                                 char* buffer, unsigned int& bytesRead )
        
        {
            data.std.position = decompressValueFromBuffer< unsigned short >( buffer, timeMin,
                                                                           timeInc, bytesRead );
            
            data.std.internalValue = decompressValueFromBuffer< unsigned short >( buffer, valueMin,
                                                                                valueInc, bytesRead );
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // fills a curve's position and value with float values 
        // </Summary>
        void fillPositionValue(const float timeInc, const float timeMin,
                          const float valueInc, const float valueMin,
                          char* buffer, unsigned int& bytesRead )
        
        {
            memcpy(&data.std.position, &buffer[bytesRead], sizeof(float));
            bytesRead += sizeof(float);
            
            memcpy(&data.std.internalValue, &buffer[bytesRead], sizeof(float));
            bytesRead += sizeof(float);
        }
        
        void printStandard( Logger::Severity severity = Logger::SV_VERBOSE )
        {
            Logger::printf("Walaber", severity, "position: [ %f ]\n", data.std.position );
            Logger::printf("Walaber", severity, "internalValue: [ %f ]\n", data.std.internalValue );
            Logger::printf("Walaber", severity, "tangentIn: [ %f ]\n", data.std.tangentIn );
            Logger::printf("Walaber", severity, "tangentOut: [ %f ]\n", data.std.tangentOut );
            Logger::printf("Walaber", severity, "continuity: [ %i ]\n", (int)data.std.continuity );
        }
        
        void printBezier( Logger::Severity severity = Logger::SV_VERBOSE )
        {
            Logger::printf("Walaber", severity, "position: [ %f ]\n", data.bez.position );
            Logger::printf("Walaber", severity, "internalValue: [ %f ]\n", data.bez.internalValue );
            Logger::printf("Walaber", severity, "handleLeftPosition: [ %f ]\n",
                           data.bez.handleLeftPosition );
            Logger::printf("Walaber", severity, "handleLeftValue: [ %f ]\n",
                           data.bez.handleLeftPosition );
            Logger::printf("Walaber", severity, "handleRightPosition: [ %f ]\n",
                           data.bez.handleRightPosition );
            Logger::printf("Walaber", severity, "handleRightValue: [ %f ]\n",
                           data.bez.handleRightValue );
        }
        
    private:
        // --------========--------========--------========--------========--------========--------=======
        
		union
		{
			CurveDataStandard std;
			CurveDataBezier bez;
		} data;
		
		friend class Curve;
		friend class BezierCurve;
    };
    

    
    // Abstract Interface Base class for curves!!
    class Curve
    {
    public: // enums, structs, and typedefs
        
        enum CurveLoopType
        {
            CLT_Constant,
            CLT_Cycle,
            CLT_CycleOffset,
            CLT_Oscillate,
            CLT_Linear,
        };
        
        typedef DynamicArray<CurveKey> CurveKeyList;
        
    public: //statics
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Load a curve from an xml file and return the curve ptr
        // </Summary>
        static Curve* loadFromXml(const std::string& filename);

        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Load a curve from an xml file and return the curve ptr
        // </Summary>
        static Curve* loadFromXml(xmlNodePtr root, float keyDifferenceToIgnore = 0.0f);
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Load a curve from an xml file and return the curve ptr
        // </Summary>
        static CurveLoopType curveLoopTypeFromString( std::string& s );
        
    public:
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Default ctor
        // </Summary>
        Curve():
        mKeys(),      
        mPreLoop(CLT_Constant),
        mPostLoop(CLT_Constant),
        mTimeRange(0.0f),
        mInvTimeRange(0.0f)
        {};
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Load a curve from a buffer that was loaded from a curve collection
        // </Summary>
        Curve(char* buffer, unsigned int& bytesRead):
        mKeys(buffer, bytesRead),      
        mPreLoop(CLT_Constant),
        mPostLoop(CLT_Constant),
        mTimeRange(0.0f),
        mInvTimeRange(0.0f)
        {
            memcpy(&mPreLoop, &buffer[bytesRead], sizeof(CurveLoopType));
            bytesRead += sizeof(CurveLoopType);
            memcpy(&mPostLoop, &buffer[bytesRead], sizeof(CurveLoopType));
            bytesRead += sizeof(CurveLoopType);
            memcpy(&mTimeRange, &buffer[bytesRead], sizeof(float));
            bytesRead += sizeof(float);
            memcpy(&mInvTimeRange, &buffer[bytesRead], sizeof(float));
            bytesRead += sizeof(float);
            
            // TODO: Remove keys using a key difference to ignore value
			
        };
		
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Load a curve from a buffer 
        // </Summary>
        Curve(char* buffer, unsigned int& bytesRead, unsigned int flags, const CompressionRecord& timeCR, const CompressionRecord& valueCR, float keyDifferenceToIgnore = 0.0f ):
        mKeys(),      
        mPreLoop(CLT_Constant),
        mPostLoop(CLT_Constant),
        mTimeRange(0.0f),
        mInvTimeRange(0.0f)
        {
            std::vector< int > skippedKeys;
            
			_loadCompressedKeys( buffer, bytesRead, flags, timeCR, valueCR, keyDifferenceToIgnore,
                                 skippedKeys );
            
			this->computeTangents();
        };
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // copy ctor
        // </Summary>
        Curve(const Curve& curve):
        mKeys(curve.mKeys),      
        mPreLoop(curve.mPreLoop),
        mPostLoop(curve.mPostLoop),
        mTimeRange(curve.mTimeRange),
        mInvTimeRange(curve.mInvTimeRange)
        {};
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // op=
        // </Summary>
        Curve& operator=(const Curve& curve)
        {
            mKeys = curve.mKeys;     
            mPreLoop = curve.mPreLoop;
            mPostLoop = curve.mPostLoop;
            mTimeRange = curve.mTimeRange;
            mInvTimeRange = curve.mInvTimeRange;
            
            return *this;
        };
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // dtor
        // </Summary>
        virtual ~Curve()
        {};
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // load the curve given an xml node ptr
        // </Summary>
        virtual void loadFromXmlNode(xmlNodePtr root, float keyDifferenceToIgnore = 0.0f) = 0;
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // goes through logic common to all curves (pre and post loop logic) then calls virtual evaluateCurve 
        // if necessary
        // </Summary>
        float evaluate(const float position) const; 
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // get the time of the first key
        // </Summary>
        float getFirstTime() { if (mKeys.size() == 0) { return 0.0f; } else { return mKeys[0].getPosition(); } }
        
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // get the time of the last key
        // </Summary>
        float getLastTime() { if (mKeys.size() == 0) { return 0.0f; } else { return mKeys[mKeys.size()-1].getPosition(); } }        
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // get the duration
        // </Summary>
        float getDuration() { return mTimeRange; }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // count how many cycles before or after the loop the time step is at
        // </Summary>
        float calcCycle(float t) const;
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // find the segment in the curve that the time falls into and return a normalized time into that section
        // </Summary>
        float findSegment(float t, int& k0, int& k1) const;
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // add a curve key to the curve
        // </Summary>
        void addCurveKey( const CurveKey& ck );
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // add a curve key to the curve
        // </Summary>
        bool addCurveKeyOptional( const CurveKey& ck,
                                  float keyDifferenceToIgnore,
                                  CurveKey& inOutLastCurveKey,
                                  float& inOutLastAddedCurveKeyValue,
                                  bool& inOutDidNotAddLastCurveKey,
                                  std::vector< int >& inOutSkippedIndices )
        {
            bool shouldAddCurveKey=true;
            //float position = ck.getPosition();
            float value = ck.getValue();

            if( absf( value - inOutLastAddedCurveKeyValue ) < keyDifferenceToIgnore &&
                mKeys.size() )
            {
                shouldAddCurveKey=false;
                inOutDidNotAddLastCurveKey=true;
                inOutLastCurveKey=ck;
                
                Logger::printf("Walaber", Logger::SV_DEBUG, "Keys -> not adding curvekey! value: [%f] last value: [%f] key difference to ignore: [%f]\n", value, inOutLastAddedCurveKeyValue , keyDifferenceToIgnore);
            }
            else if( inOutDidNotAddLastCurveKey )
            {
                //Logger::printf("Walaber", Logger::SV_DEBUG, "Keys -> got content! position: [%f] value: [%f]\n", inOutLastCurveKey.getPosition(), inOutLastCurveKey.getValue());
                
                inOutSkippedIndices.pop_back();
                
                addCurveKey( inOutLastCurveKey );
            }
            
            if(shouldAddCurveKey)
            {
                inOutLastAddedCurveKeyValue = value;
                
                inOutDidNotAddLastCurveKey=false;
                
                //Logger::printf("Walaber", Logger::SV_DEBUG, "Keys -> got content! position: [%f] value: [%f]\n", position, value);
                
                addCurveKey( ck );
            }
            
            return shouldAddCurveKey;
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // check if its only one key
        // </Summary>
        bool getIsConstant() const { return (this->mKeys.size() <= 1); }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // get a copy of the curve keys
        // </Summary>
        CurveKeyList getKeys() const { return mKeys; }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // get the post loop behavior
        // </Summary>
        CurveLoopType getPostLoop() const { return mPostLoop; }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set the post loop behavior
        // </Summary>
        void setPostLoop( CurveLoopType cl ) { mPostLoop = cl; }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // get the pre loop behavior
        // </Summary>
        CurveLoopType getPreLoop() const { return mPreLoop; }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // set the pre loop behavior
        // </Summary>
        void setPreLoop( CurveLoopType cl ) { mPreLoop = cl; }
        
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
				unsigned int numKeys = mKeys.size();
				size += sizeof(numKeys);
				
				// we will only write the pos and val - everything else can be computed at load time
				
				if ( flags & CCF_1_Byte_Compression )
				{					
					size += sizeof(unsigned char) * 2 * numKeys;
				}
				else if ( flags & CCF_2_Byte_Compression )
				{
					size += sizeof(unsigned short) * 2 * numKeys;
				}
				else
				{
					size += sizeof(float) * 2 * numKeys;
				}				
			}
			else if ( flags & CCF_1_Byte_Compression )
			{
				// if there is a compression specified, it means that we need to save space, so don't write out the padding! This is a super manual function, so this feels kind of dirty
				/*
				 float position;
				 float internalValue;
				 float tangentIn;
				 float tangentOut;
				 CurveContinuity continuity;
				 int padding;
				*/
				
				unsigned int numKeys = mKeys.size();
				size += sizeof(numKeys);
				
				// position internalValue tangentIn continuity gets written for each key
				size += ( 1 + 1 + 1 + 1 + 1 ) * numKeys;
			}
			else if ( flags & CCF_2_Byte_Compression )
			{
				// if there is a compression specified, it means that we need to save space, so don't write out the padding! This is a super manual function, so this feels kind of dirty
				/*
				 float position;
				 float internalValue;
				 float tangentIn;
				 float tangentOut;
				 CurveContinuity continuity;
				 int padding;
				 */
				
				unsigned int numKeys = mKeys.size();
				size += sizeof(numKeys);
				
				// position internalValue tangentIn continuity gets written for each key
				size += ( 2 + 2 + 2 + 2 + 1 ) * numKeys;
				
			}
			
			return size;
        }
        
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
			float tangentIn;
			float tangentOut;
			CurveContinuity continuity;
			int padding;
			*/
			
			min = max = mKeys[0].data.std.internalValue;
			unsigned int size = mKeys.size();
			for(unsigned int i = 1; i < size; ++i)
			{				
				min = (Walaber::minf(min, mKeys[i].data.std.internalValue));
				max = (Walaber::maxf(max, mKeys[i].data.std.internalValue));
				
				min = (Walaber::minf(min, mKeys[i].data.std.tangentIn));
				max = (Walaber::maxf(max, mKeys[i].data.std.tangentIn));
				
				min = (Walaber::minf(min, mKeys[i].data.std.tangentOut));
				max = (Walaber::maxf(max, mKeys[i].data.std.tangentOut));
				
				//Logger::printf("Walaber", Logger::SV_DEBUG, "key: [%i] pos: [%f] val: [%f] tanIn: [%f] tanOut: [%f]\n", i, mKeys[i].data.std.position, mKeys[i].data.std.internalValue, mKeys[i].data.std.tangentIn, mKeys[i].data.std.tangentOut );
			}
			
			return std::make_pair( min, max );
        }		
		
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // returns the min and max of the key values
        // </Summary>
        virtual std::pair<float, float> getMinMaxKeyValue()
        {
			float min = 0;
			float max = 0;
						
			min = max = mKeys[0].data.std.internalValue;
			unsigned int size = mKeys.size();
			for(unsigned int i = 1; i < size; ++i)
			{				
				min = (Walaber::minf(min, mKeys[i].data.std.internalValue));
				max = (Walaber::maxf(max, mKeys[i].data.std.internalValue));
								
				//Logger::printf("Walaber", Logger::SV_DEBUG, "key: [%i] pos: [%f] val: [%f] \n", i, mKeys[i].data.std.position, mKeys[i].data.std.internalValue );
			}
			
			return std::make_pair( min, max );
        }	
		
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // returns a pair that constains the min and max values contained in this curve
        // </Summary>
        virtual std::pair<float, float> getMinMaxKeyPosition()
        {
			float min = 0;
			float max = 0;
			
			min = max = mKeys[0].data.std.position;
			unsigned int size = mKeys.size();
			for(unsigned int i = 1; i < size; ++i)
			{
				min = (Walaber::minf(min, mKeys[i].data.std.position));
				max = (Walaber::maxf(max, mKeys[i].data.std.position));
								
				//Logger::printf("Walaber", Logger::SV_DEBUG, "key: [%i] pos: [%f] val: [%f] tanIn: [%f] tanOut: [%f]\n", i, mKeys[i].data.std.position, mKeys[i].data.std.internalValue, mKeys[i].data.std.tangentIn, mKeys[i].data.std.tangentOut );
			}
			
			return std::make_pair( min, max );
        }	
		
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Writes the data of a curve object into the buffer - returns the number of bytes written
        // </Summary>
        virtual unsigned int writeToBuffer(char* buf)
        {
            unsigned int bytesWritten = 0;
            
            // write key data
            bytesWritten += mKeys.writeToBuffer(&buf[bytesWritten]);
            
            // write rest of members
            memcpy(&buf[bytesWritten], &mPreLoop, sizeof(mPreLoop));
            bytesWritten += sizeof(mPreLoop);
            
            memcpy(&buf[bytesWritten], &mPostLoop, sizeof(mPostLoop));
            bytesWritten += sizeof(mPostLoop);
            
            memcpy(&buf[bytesWritten], &mTimeRange, sizeof(mTimeRange));
            bytesWritten += sizeof(mTimeRange);
            
            memcpy(&buf[bytesWritten], &mInvTimeRange, sizeof(mInvTimeRange));
            bytesWritten += sizeof(mInvTimeRange);
            
            return bytesWritten;
        }
		
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // Writes the data of a curve object into the buffer based on the flags passed in - returns the number of bytes written
        // </Summary>
        virtual unsigned int writeToBuffer(char* buf, const unsigned int flags, const CompressionRecord& timeCR, const CompressionRecord& valueCR)
        {
            unsigned int bytesWritten = 0;
            float timeInc = 0;
			float valueInc = 0;
			
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
				bytesWritten = writeToBuffer(buf);
            }
			else if ( flags & CCF_Skeletal_Animation_Collection )
			{
				unsigned int numKeys = mKeys.size();
				memcpy(&buf[bytesWritten], &numKeys, sizeof(numKeys));		
				bytesWritten += sizeof(numKeys);
				
				// we will only write the pos and val - everything else can be computed at load time
				for( unsigned int i=0; i < numKeys; ++i)
				{							
					if ( flags & CCF_1_Byte_Compression )
					{			
						// crunch the time
						unsigned char val = crunchFloat<unsigned char>( mKeys[i].data.std.position,
                                                                        timeCR.mMinValue, timeInc );
						
						memcpy( &buf[bytesWritten], &val, sizeof(val) );
						bytesWritten += sizeof(val);
						
						// crunch the value						
						val = crunchFloat<unsigned char>( mKeys[i].data.std.internalValue,
                                                          valueCR.mMinValue, valueInc );
						
						memcpy( &buf[bytesWritten], &val, sizeof(val) );
						bytesWritten += sizeof(val);
					}
					else if ( flags & CCF_2_Byte_Compression )
					{
						// crunch the time
						unsigned short val = crunchFloat<unsigned short>( mKeys[i].data.std.position,
                                                                       timeCR.mMinValue, timeInc );
						
						memcpy( &buf[bytesWritten], &val, sizeof(val) );
						bytesWritten += sizeof(val);
						
						// crunch the value
						val = crunchFloat<unsigned short>( mKeys[i].data.std.internalValue,
                                                         valueCR.mMinValue, valueInc );
						
						memcpy( &buf[bytesWritten], &val, sizeof(val) );
						bytesWritten += sizeof(val);
					}
					else
					{
						// crunch the time
						float val = mKeys[i].data.std.position;
						memcpy( &buf[bytesWritten], &val, sizeof(val) );
						bytesWritten += sizeof(val);
						
						// crunch the value
						val = mKeys[i].data.std.internalValue;
						memcpy( &buf[bytesWritten], &val, sizeof(val) );
						bytesWritten += sizeof(val);
					}	
				}
				
			}
			else if ( flags & CCF_1_Byte_Compression )
			{
				unsigned int numKeys = mKeys.size();
				memcpy(&buf[bytesWritten], &numKeys, sizeof(numKeys));		
				bytesWritten += sizeof(numKeys);
				
				for( unsigned int i=0; i < numKeys; ++i)
				{		
                    // crunch the time
                    unsigned char val = crunchFloat<unsigned char>( mKeys[i].data.std.position,
                                                                   timeCR.mMinValue, timeInc );
                    
                    memcpy( &buf[bytesWritten], &val, sizeof(val) );
                    bytesWritten += sizeof(val);
                    
                    // crunch the value
                    val = crunchFloat<unsigned char>( mKeys[i].data.std.internalValue,
                                                     valueCR.mMinValue, valueInc );
                    
                    memcpy( &buf[bytesWritten], &val, sizeof(val) );
                    bytesWritten += sizeof(val);
					
					// crunch the tan In
					val = crunchFloat<unsigned char>( mKeys[i].data.std.tangentIn,
                                                     valueCR.mMinValue, valueInc );
                    
                    memcpy( &buf[bytesWritten], &val, sizeof(val) );
                    bytesWritten += sizeof(val);

					// crunch the tan out
					val = crunchFloat<unsigned char>( mKeys[i].data.std.tangentOut,
                                                     valueCR.mMinValue, valueInc );
                    
                    memcpy( &buf[bytesWritten], &val, sizeof(val) );
                    bytesWritten += sizeof(val);
					
					// crunch the continuity
					val = (unsigned char)mKeys[i].data.std.continuity;
					memcpy( &buf[bytesWritten], &val, sizeof(val) );
					bytesWritten += sizeof(val);
				}
			}
			else if ( flags & CCF_2_Byte_Compression )
			{
				unsigned int numKeys = mKeys.size();
				memcpy(&buf[bytesWritten], &numKeys, sizeof(numKeys));		
				bytesWritten += sizeof(numKeys);
				
				// we will only write the pos and val - everything else can be computed at load time
				for( unsigned int i=0; i < numKeys; ++i)
				{		
                    // crunch the time
                    unsigned short val = crunchFloat<unsigned short>( mKeys[i].data.std.position,
                                                                   timeCR.mMinValue, timeInc );
                    
                    memcpy( &buf[bytesWritten], &val, sizeof(val) );
                    bytesWritten += sizeof(val);
                    
                    // crunch the value
                    val = crunchFloat<unsigned short>( mKeys[i].data.std.internalValue,
                                                     valueCR.mMinValue, valueInc );
                    
                    memcpy( &buf[bytesWritten], &val, sizeof(val) );
                    bytesWritten += sizeof(val);
					
                    // crunch the tan In
                    val = crunchFloat<unsigned short>( mKeys[i].data.std.tangentIn,
                                                      valueCR.mMinValue, valueInc );
                    
                    memcpy( &buf[bytesWritten], &val, sizeof(val) );
                    bytesWritten += sizeof(val);
                    
                    // crunch the tan Out
                    val = crunchFloat<unsigned short>( mKeys[i].data.std.tangentOut,
                                                      valueCR.mMinValue, valueInc );
                    
                    memcpy( &buf[bytesWritten], &val, sizeof(val) );
                    bytesWritten += sizeof(val);
                    
					// crunch the continuity
					unsigned char charVal = (unsigned char)mKeys[i].data.std.continuity;
					memcpy( &buf[bytesWritten], &charVal, sizeof(charVal) );
					bytesWritten += sizeof(charVal);
				}
			}
            
            return bytesWritten;
        }
		
		// -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // default does nothing since the base curve is not truly a curve
        // </Summary>
        virtual void computeTangents(){};
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // print out values
        // </Summary>
        virtual void print( Logger::Severity severity = Logger::SV_VERBOSE )
        {
            _printStandardKeys( severity );
            _printData( severity );            
        }
        
    protected: // interface
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // calls the appropriate evaluate curve function for type and actually does curve math
        // </Summary>
        virtual float _evaluateCurve(const float position, const float offset) const = 0;

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
        void _printStandardKeys( Logger::Severity severity)
        {
            for ( unsigned int i=0; i < mKeys.size(); ++i)
            {
                Logger::printf("Walaber", severity,
                               "Key: [ %i ]\n", i );
                mKeys[i].printStandard( severity );
            }
        }
        
        // -------------==============-------------==============-------------==============-------------===
        // <Summary>
        // print out the rest of the data the curve has 
        // </Summary>
        void _printData( Logger::Severity severity)
        {
            Logger::printf("Walaber", severity,
                           "pre-loop:           [ %i ]\n", (int)mPreLoop );
            Logger::printf("Walaber", severity,
                           "post-loop:          [ %i ]\n", (int)mPostLoop );
            Logger::printf("Walaber", severity,
                           "time range:         [ %f ]\n", mTimeRange );
            Logger::printf("Walaber", severity,
                           "inv time range:     [ %f ]\n", mInvTimeRange );
        }
       
    protected: // members
        
        CurveKeyList                mKeys;        
        CurveLoopType               mPreLoop;
        CurveLoopType               mPostLoop;
        float                       mTimeRange;
        float                       mInvTimeRange;
    };  
    
    MAKE_SHARED_PTR(Curve);
    
    template< class T >
    T crunchFloat( float value, float minVal, float increment )
    {
        T val;
        // crunch the value
        if( isEqualf( value, 0.0f ) )
        {
            val = 0;
        }
        else
        {
            // our  compressed value is between 1 and max, so we need to add 1 to the final iVal
            float fVal = ( value - minVal ) / increment;
            float rounder = fVal >= 0 ? .5f : -.5f;
            int iVal = static_cast<int>( floor( fVal + rounder ) + 1 );
            val = (T)clampi(iVal,1, std::numeric_limits<T>::max() );
        }
        
        return val;
    }
    
    template< class T >
    float decompressValueFromBuffer( const char* const buffer, float minVal, float increment,
                                     unsigned int& bytesRead )
    {
        float ret = 0.0f;
        
        T* value = (T*)(&buffer[bytesRead]);
        if( *value )
        {
            // we need to subtract 1 to get it back into range
            T finalVal = (*value) - 1;
            ret =((float)(finalVal) * increment) + minVal;
        }
        
        bytesRead += sizeof(T);
        
        return ret;
    }
}

#endif
