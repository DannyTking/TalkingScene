//
//  Matrix3.cpp
//  WalaberEngine
//
//  Created by Pecho, Jason on 2/13/13.
//
//

#include "Matrix3.h"

#include <StringHelper.h>

namespace Walaber
{
    Matrix3 Matrix3::Identity( Vector3::UnitX, Vector3::UnitY, Vector3::UnitZ );
    
    //----------------------------------------------------------------------------------------------
    
    Matrix3::Matrix3() :
    mColumn0( Vector3::UnitX ),
    mColumn1( Vector3::UnitY ),
    mColumn2( Vector3::UnitZ )
    {
    }
    
    //----------------------------------------------------------------------------------------------
    
    Matrix3::Matrix3( const Matrix3& matrix ) :
    mColumn0( matrix.mColumn0 ),
    mColumn1( matrix.mColumn1 ),
    mColumn2( matrix.mColumn2 )
    {
    }
    
    //----------------------------------------------------------------------------------------------
    
    Matrix3::Matrix3( const Vector3& column0, const Vector3& column1, const Vector3& column2 ) :
    mColumn0( column0 ),
    mColumn1( column1 ),
    mColumn2( column2 )
    {    
    }
    
    //----------------------------------------------------------------------------------------------
    
    Matrix3::Matrix3( const float t0, const float t1, const float t2,
                      const float t3, const float t4, const float t5,
                      const float t6, const float t7, const float t8 ) :
    mColumn0( t0, t1, t2 ),
    mColumn1( t3, t4, t5 ),
    mColumn2( t6, t7, t8 )
    {
    }
    
    //----------------------------------------------------------------------------------------------
    
    Matrix3::Matrix3( const float scalar ) :
    mColumn0( scalar ),
    mColumn1( scalar ),
    mColumn2( scalar )
    {
    }
    
    
    //----------------------------------------------------------------------------------------------
    
    Matrix3::~Matrix3()
    {
        
    }
    
    //----------------------------------------------------------------------------------------------
    
    Matrix3& Matrix3::operator=( const Matrix3& matrix )
    {
        mColumn0 = matrix.mColumn0;
        mColumn1 = matrix.mColumn1;
        mColumn2 = matrix.mColumn2;
        
        return *this;
    }
    
    //----------------------------------------------------------------------------------------------
    
    void Matrix3::setColumn0( const Vector3& column0 )
    {
        mColumn0 = column0;
    }
    
    //----------------------------------------------------------------------------------------------
    
    void Matrix3::setColumn1( const Vector3& column1 )
    {
        mColumn1 = column1;
    }
    
    //----------------------------------------------------------------------------------------------
    
    void Matrix3::setColumn2( const Vector3& column2 )
    {
        mColumn2 = column2;
    }
    
    //----------------------------------------------------------------------------------------------
    
    void Matrix3::set( const float t0, const float t1, const float t2,
                       const float t3, const float t4, const float t5,
                       const float t6, const float t7, const float t8 )
    {
        mColumn0.set( t0, t1, t2 );
        mColumn1.set( t3, t4, t5 );
        mColumn2.set( t6, t7, t8 );
    }
    
    //----------------------------------------------------------------------------------------------
    
    void Matrix3::setElement( const unsigned int index, const float value )
    {
        // make sure we don't overwrite the buffer
        assert( index <= 8 );
        
        switch( index )
        {
            case 0:
                mColumn0.X = value;
                break;
                
            case 1:
                mColumn0.Y = value;
                break;
                
            case 2:
                mColumn0.Z = value;
                break;
                
            case 3:
                mColumn1.X = value;
                break;
                
            case 4:
                mColumn1.Y = value;
                break;
                
            case 5:
                mColumn1.Z = value;
                break;
                
            case 6:
                mColumn2.X = value;
                break;
                
            case 7:
                mColumn2.Y = value;
                break;
                
            case 8:
                mColumn2.Z = value;
                break;
        }
    }
    
    //----------------------------------------------------------------------------------------------
    
    Vector3 Matrix3::getColumn0( ) const
    {
        return mColumn0;
    }
    
    //----------------------------------------------------------------------------------------------
    
    Vector3 Matrix3::getColumn1( ) const
    {
        return mColumn1;
    }
    
    //----------------------------------------------------------------------------------------------
    
    Vector3 Matrix3::getColumn2( ) const
    {
        return mColumn2;
    }
    
    //----------------------------------------------------------------------------------------------
    
    float Matrix3::getElement( const unsigned int index ) const
    {
        assert( index >= 0 && index <= 8 );

        float ret = 0.0f;
        
        switch( index )
        {
            case 0:
                ret = mColumn0.X;
                break;
                
            case 1:
                ret = mColumn0.Y;
                break;
                
            case 2:
                ret = mColumn0.Z;
                break;
                
            case 3:
                ret = mColumn1.X;
                break;
                
            case 4:
                ret = mColumn1.Y;
                break;
                
            case 5:
                ret = mColumn1.Z;
                break;
                
            case 6:
                ret = mColumn2.X;
                break;
                
            case 7:
                ret = mColumn2.Y;
                break;
                
            case 8:
                ret = mColumn2.Z;
                break;
        }
        
        return ret;
    }
    
    
    //----------------------------------------------------------------------------------------------
    
    const Matrix3 Matrix3::operator +( const Matrix3& matrix ) const
    {
        return Matrix3(
                        mColumn0 + matrix.mColumn0,
                        mColumn1 + matrix.mColumn1,
                        mColumn2 + matrix.mColumn2 );
    }
    
    //----------------------------------------------------------------------------------------------
    
    const Matrix3 Matrix3::operator -( ) const
    {
        return Matrix3(
                       -mColumn0,
                       -mColumn1,
                       -mColumn2 );
    }
    
    //----------------------------------------------------------------------------------------------
    
    const Matrix3 Matrix3::operator -( const Matrix3& matrix ) const
    {
        return Matrix3(
                       mColumn0 - matrix.mColumn0,
                       mColumn1 - matrix.mColumn1,
                       mColumn2 - matrix.mColumn2 );
    }
    
    //----------------------------------------------------------------------------------------------
    
    const Matrix3 Matrix3::operator *( const float scalar ) const
    {
        return Matrix3(
                       mColumn0 * scalar,
                       mColumn1 * scalar,
                       mColumn2 * scalar );
    }
    
    //----------------------------------------------------------------------------------------------
    
    const Vector3 Matrix3::operator *( const Vector3& vector ) const
    {
        return
        Vector3(
                ( mColumn0.X * vector.X ) + ( mColumn1.X * vector.Y ) + ( mColumn2.X * vector.Z ),
                ( mColumn0.Y * vector.X ) + ( mColumn1.Y * vector.Y ) + ( mColumn2.Y * vector.Z ),
                ( mColumn0.Z * vector.X ) + ( mColumn1.Z * vector.Y ) + ( mColumn2.Z * vector.Z ) );
    }
    
    //----------------------------------------------------------------------------------------------
    
    const Matrix3 Matrix3::operator *( const Matrix3& matrix ) const
    {
        return
        Matrix3(
                ( mColumn0.X * matrix.mColumn0.X ) +
                ( mColumn1.X * matrix.mColumn0.Y ) +
                ( mColumn2.X * matrix.mColumn0.Z ) ,    // m0
        
                ( mColumn0.Y * matrix.mColumn0.X ) +
                ( mColumn1.Y * matrix.mColumn0.Y ) +
                ( mColumn2.Y * matrix.mColumn0.Z ) ,    // m1
                
                ( mColumn0.Z * matrix.mColumn0.X ) +
                ( mColumn1.Z * matrix.mColumn0.Y ) +
                ( mColumn2.Z * matrix.mColumn0.Z ) ,    // m2
                
                ( mColumn0.X * matrix.mColumn1.X ) +
                ( mColumn1.X * matrix.mColumn1.Y ) +
                ( mColumn2.X * matrix.mColumn1.Z ) ,    // m3
                
                ( mColumn0.Y * matrix.mColumn1.X ) +
                ( mColumn1.Y * matrix.mColumn1.Y ) +
                ( mColumn2.Y * matrix.mColumn1.Z ) ,    // m4
    
                ( mColumn0.Z * matrix.mColumn1.X ) +
                ( mColumn1.Z * matrix.mColumn1.Y ) +
                ( mColumn2.Z * matrix.mColumn1.Z ) ,    // m5
                
                ( mColumn0.X * matrix.mColumn2.X ) +
                ( mColumn1.X * matrix.mColumn2.Y ) +
                ( mColumn2.X * matrix.mColumn2.Z ) ,    // m6
        
                ( mColumn0.Y * matrix.mColumn2.X ) +
                ( mColumn1.Y * matrix.mColumn2.Y ) +
                ( mColumn2.Y * matrix.mColumn2.Z ) ,    // m7
        
                ( mColumn0.Z * matrix.mColumn2.X ) +
                ( mColumn1.Z * matrix.mColumn2.Y ) +
                ( mColumn2.Z * matrix.mColumn2.Z )      // m8
               );        
    }
    
    //----------------------------------------------------------------------------------------------
    
    Matrix3& Matrix3::operator +=( const Matrix3& matrix )
    {
        mColumn0 += matrix.mColumn0;
        mColumn1 += matrix.mColumn1;
        mColumn2 += matrix.mColumn2;
        
        return *this;
    }
    
    //----------------------------------------------------------------------------------------------
    
    Matrix3& Matrix3::operator -=( const Matrix3& matrix )
    {
        mColumn0 -= matrix.mColumn0;
        mColumn1 -= matrix.mColumn1;
        mColumn2 -= matrix.mColumn2;
        
        return *this;
    }
    
    //----------------------------------------------------------------------------------------------
    
    Matrix3& Matrix3::operator *=( const float scalar )
    {
        mColumn0 *= scalar;
        mColumn1 *= scalar;
        mColumn2 *= scalar;
     
        return *this;
    }
    
    //----------------------------------------------------------------------------------------------
    
    Matrix3& Matrix3::operator *=( const Matrix3& matrix )
    {
        *this = *this * matrix;
        
        return *this;
    }
    
    //----------------------------------------------------------------------------------------------
    
    void Matrix3::transpose( )
    {
        float temp;
        
        // swamp m1 and m3
        temp = mColumn0.Y;
        mColumn0.Y = mColumn1.X;
        mColumn1.X = temp;
        
        // swamp m2 and m6
        temp = mColumn0.Z;
        mColumn0.Z = mColumn2.X;
        mColumn2.X = temp;
        
        // swamp m5 and m7
        temp = mColumn1.Z;
        mColumn1.Z = mColumn2.Y;
        mColumn2.Y = temp;
    }
    
    //----------------------------------------------------------------------------------------------
    
    Matrix3 Matrix3::getTranspose( ) const
    {
        return
        Matrix3(
                 mColumn0.X,
                 mColumn1.X,
                 mColumn2.X,
                 mColumn0.Y,
                 mColumn1.Y,
                 mColumn2.Y,
                 mColumn0.Z,
                 mColumn1.Z,
                 mColumn2.Z
               );
    }
    
    //----------------------------------------------------------------------------------------------
    
    void Matrix3::invert( )
    {
        // if the determinant of the matrix is 0, there is no inverse
        assert ( getDeterminant() );
        
        // get the inverse determinant
        float invDeterminant = 1.0f / getDeterminant();
        
        // getting the adjugate matrix can be done by getting the transpose matrix and
        // finding the cofactors ( determinant of square matrices for each entry ) or by
        // combining a few cross products
        Vector3 crossA = mColumn1.crossProduct( mColumn2 );
        Vector3 crossB = mColumn2.crossProduct( mColumn0 );
        Vector3 crossC = mColumn0.crossProduct( mColumn1 );
        
        // now we multiply the adjugate by the inverse determinant and assign accordingly
        mColumn0.X = crossA.X * invDeterminant;
        mColumn0.Y = crossB.X * invDeterminant;
        mColumn0.Z = crossC.X * invDeterminant;
        
        mColumn1.X = crossA.Y * invDeterminant;
        mColumn1.Y = crossB.Y * invDeterminant;
        mColumn1.Z = crossC.Y * invDeterminant;
    
        mColumn2.X = crossA.Z * invDeterminant;
        mColumn2.Y = crossB.Z * invDeterminant;
        mColumn2.Z = crossC.Z * invDeterminant;
        
    }
    
    //----------------------------------------------------------------------------------------------
    
    Matrix3 Matrix3::getInverse( ) const
    {
        // if the determinant of the matrix is 0, there is no inverse
        assert ( getDeterminant() );
        
        // get the inverse determinant
        float invDeterminant = 1.0f / getDeterminant();
        
        // getting the adjugate matrix can be done by getting the transpose matrix and
        // finding the cofactors ( determinant of square matrices for each entry ) or by
        // combining a few cross products
        Vector3 crossA = mColumn1.crossProduct( mColumn2 );
        Vector3 crossB = mColumn2.crossProduct( mColumn0 );
        Vector3 crossC = mColumn0.crossProduct( mColumn1 );
        
        // now we multiply the adjugate by the inverse determinant and assign accordingly
        return Matrix3(
                       crossA.X * invDeterminant,
                       crossB.X * invDeterminant,
                       crossC.X * invDeterminant,
        
                       crossA.Y * invDeterminant,
                       crossB.Y * invDeterminant,
                       crossC.Y * invDeterminant,
        
                       crossA.Z * invDeterminant,
                       crossB.Z * invDeterminant,
                       crossC.Z * invDeterminant
                     );
    }
    
    //----------------------------------------------------------------------------------------------
    
    void Matrix3::loadIdentity()
    {
        mColumn0 = Vector3::UnitX;
        mColumn1 = Vector3::UnitY;
        mColumn2 = Vector3::UnitZ;
    }
    
    //----------------------------------------------------------------------------------------------
    
    float Matrix3::getDeterminant( ) const
    {
        return ( mColumn0.X * ( ( mColumn1.Y * mColumn2.Z ) - ( mColumn1.Z * mColumn2.Y ) ) ) -
               ( mColumn1.X * ( ( mColumn0.Y * mColumn2.Z ) - ( mColumn0.Z * mColumn2.Y ) ) ) +
               ( mColumn2.X * ( ( mColumn0.Y * mColumn1.Z ) - ( mColumn0.Z * mColumn1.Y ) ) );
    }
    
    //----------------------------------------------------------------------------------------------
    
    Matrix3 Matrix3::rotationX( const float radiansX )
    {
        float cosX = cosf( radiansX );
        float sinX = sinf( radiansX );

        return
        Matrix3(
                1,
                0,
                0,
                0,
                cosX,
                sinX,
                0,
                -sinX,
                cosX
                );
        
    }
    
    //----------------------------------------------------------------------------------------------
    
    Matrix3 Matrix3::rotationY( const float radiansY )
    {
        float cosY = cosf( radiansY );
        float sinY = sinf( radiansY );
        
        return
        Matrix3(
                 cosY,
                 0,
                 -sinY,
                 0,
                 1,
                 0,
                 sinY,
                 0,
                 cosY
               );                 
    }
    
    //----------------------------------------------------------------------------------------------
    
    Matrix3 Matrix3::rotationZ( const float radiansZ )
    {
        float cosZ = cosf( radiansZ );
        float sinZ = sinf( radiansZ );
        
        return
        Matrix3(
                 cosZ,
                 sinZ,
                 0,
                 -sinZ,
                 cosZ,
                 0,
                 0,
                 0,
                 1
                );
    }
    
    //----------------------------------------------------------------------------------------------
    
    Matrix3 Matrix3::rotationZYX( const Vector3& radiansXYZ )
    {
        float cosX = cosf( radiansXYZ.X );
        float cosY = cosf( radiansXYZ.Y );
        float cosZ = cosf( radiansXYZ.Z );
        float sinX = sinf( radiansXYZ.X );
        float sinY = sinf( radiansXYZ.Y );
        float sinZ = sinf( radiansXYZ.Z );
        float cosZsinY = cosZ * sinY;
        float sinZsinY = sinZ * sinY;
        
        return
        Matrix3(
                 cosZ * cosY,
                 sinZ * cosY,
                 -sinY,
                 ( ( sinX * ( cosZsinY ) ) - ( sinZ * cosX ) ),
                 ( ( cosZ * cosX ) + ( sinX * ( sinZsinY ) ) ),
                 cosY * sinX,
                 ( ( sinX * sinZ ) + ( cosX * ( cosZsinY ) ) ),
                 ( -( sinX * cosZ ) + ( cosX * ( sinZsinY ) ) ),
                 cosY * cosX
                );
    }
    
    //----------------------------------------------------------------------------------------------
    
    Matrix3 Matrix3::scale( const Vector3& scaleVec )
    {
        return
        Matrix3(
                 scaleVec.X,
                 0,
                 0,
                 0,
                 scaleVec.Y,
                 0,
                 0,
                 0,
                 scaleVec.Z
               );
    }
    
    //----------------------------------------------------------------------------------------------
    
    Matrix3 Matrix3::skew( const float xSkewRadians, const float ySkewRadians,
                           const Vector2& origin )
    {
        float tanX = tanf(xSkewRadians);
        float tanY = tanf(ySkewRadians);
        
        return
        Matrix3(
                 1.0f,
                 tanY,
                 0,
                 tanX,
                 1.0f,
                 0,
                 -origin.Y * tanX,
                 -origin.X * tanY,
                 1.0f
               );
                
    }
    
    //----------------------------------------------------------------------------------------------
    
    Matrix3 Matrix3::skewGradient( const float skewGradientRadians )
    {
        return
        Matrix3(
                1.0f,                
                0,
                0,
                sinf( skewGradientRadians ),
                cosf( skewGradientRadians ),
                0,
                0,
                0,
                1.0f
                );
    }
    
    //----------------------------------------------------------------------------------------------
    // @TODO: implement this function!!
    std::string Matrix3::toString( )
    {
        std::string ret;
        
        return ret;
    }
}