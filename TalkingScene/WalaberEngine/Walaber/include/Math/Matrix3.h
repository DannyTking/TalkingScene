//
//  Matrix3.h
//  WalaberEngine
//
//  Created by Pecho, Jason on 2/13/13.
//  (C) Disney Mobile 2013
//

#ifndef _MATRIX_3_H_
#define _MATRIX_3_H_

#include <Vector3.h>

#include <string>

namespace Walaber
{
    /*!
     A class representing a 3x3 Matrix in column major order
    */
    class Matrix3
    {
    public:
        
        /*!
         Default Ctor - sets matrix to Identity
        */
        Matrix3();
        
        /*!
         Copy Ctor
         @param matrix the matrix to copy
        */
        Matrix3( const Matrix3& matrix );
        
        /*!
         Ctor - takes in each of the 3 columns and assigns them accordingly
         @param column0 the first column to copy
         @param column1 the second column to copy
         @param column3 the third column to copy
        */
        Matrix3( const Vector3& column0, const Vector3& column1, const Vector3& column2 );
        
        /*!
         Ctor - sets all entries in the matrix
         @param t0 mColumn0.X = t0
         @param t1 mColumn0.Y = t1
         @param t2 mColumn0.Z = t2
         @param t3 mColumn1.X = t3
         @param t4 mColumn1.Y = t4
         @param t5 mColumn1.Z = t5
         @param t6 mColumn2.X = t6
         @param t7 mColumn2.Y = t7
         @param t8 mColumn2.Z = t8
        */
        Matrix3( const float t0, const float t1, const float t2,
                 const float t3, const float t4, const float t5,
                 const float t6, const float t7, const float t8 );
        
        /*!
         Ctor - sets all entries in the matrix to scalar
         @param scalar the value that gets assigned into every entry of the matrix
        */
        explicit Matrix3( const float scalar );
        
        /*!
         Default Ctor - sets matrix to Identity
         */
        ~Matrix3();
        
        /*!
         Set an 3x3 matrix equal to another 3x3 matrix
         @param matrix the matrix to copy
        */
        Matrix3& operator=( const Matrix3& matrix );
        
        /*!
         Set column 0 of a 3x3 matrix
         @param column0 the column to copy into matrix column 0
        */
        void setColumn0( const Vector3& column0 );
        
        /*!
         Set column1 of a 3x3 matrix
         @param column1 the column to copy into matrix column 1
        */
        void setColumn1( const Vector3& column1 );
        
        /*!
         Set column 2 of a 3x3 matrix
         @param column2 the column to copy into matrix column 2
        */
        void setColumn2( const Vector3& column2 );
        
        /*!
         Set all of the elements of a 3x3 matrix
         @param t0 mData[0] = t0
         @param t1 mData[1] = t1
         @param t2 mData[2] = t2
         @param t3 mData[3] = t3
         @param t4 mData[4] = t4
         @param t5 mData[5] = t5
         @param t6 mData[6] = t6
         @param t7 mData[7] = t7
         @param t8 mData[8] = t8
         */
        void set( const float t0, const float t1, const float t2,
                  const float t3, const float t4, const float t5,
                  const float t6, const float t7, const float t8 );
        
        /*!
         Set an element of a 3x3 matrix
         @param index the index in which to assign the value [0-8]
         @param value the value to be set in the entry of matrix
        */
        void setElement( const unsigned int index, const float value );
        
        /*!
         Get column 0 of a 3x3 matrix
         @return column 0 of the matrix
        */
        Vector3 getColumn0( ) const;
        
        /*!
         Get column 1 of a 3x3 matrix
         @return column 1 of the matrix
        */
        Vector3 getColumn1( ) const;
        
        /*!
         Get column 2 of a 3x3 matrix
         @return column 2 of the matrix
        */
        Vector3 getColumn2( ) const;
        
        /*!
         Get an element from a matrix 
         @param index the index into the matrix [0-8]
                0  3  6
                1  4  7
                2  5  8
         @return value of element at index
        */
        float getElement( const unsigned int index ) const;
        
        /*!
         Add two 3x3 matrices
         @param matrix matrix to add to this matrix
         @return the sum of the two matrices
        */
        const Matrix3 operator +( const Matrix3& matrix ) const;
        
        /*!
         Negate a 3x3 matrix
         @return a negated version of this matrix
        */
        const Matrix3 operator -( ) const;
        
        /*!
         Subtract two 3x3 matrices
         @param matrix matrix to subtract from this matrix
         @return the difference of the two matrices
        */
        const Matrix3 operator -( const Matrix3& matrix ) const;
        
        /*!
         Multiple a 3x3 matrix by a scalar
         @param scalar number to scale the matrix by
         @return scaled version of the matrix
        */
        const Matrix3 operator *( const float scalar ) const;
        
        /*!
         Add two 3x3 matrices
         @param matrix matrix to add to this matrix
         @return the sum of the two matrices
        */
        const Vector3 operator *( const Vector3& vector ) const;
        
        /*!
         Multiply two 3x3 matrices
         @param matrix matrix to multiply this matrix by
         @return the product of the two matrices
        */
        const Matrix3 operator *( const Matrix3& matrix ) const;
        
        /*!
         Add two 3x3 matrices and store in this matrix
         @param matrix matrix to add to this matrix
         @return this matrix as a sum of the two
        */
        Matrix3& operator +=( const Matrix3& matrix );
        
        /*!
         Subtract two 3x3 matrices and store in this matrix
         @param matrix matrix to subtract from this matrix
         @return this matrix as a difference of the two
        */
        Matrix3& operator -=( const Matrix3& matrix );
        
        /*!
         Multiply a matrix by a scalar and store in this matrix
         @param scalar number to multiple this matrix by
         @return the scaled version of this matrix
        */
        Matrix3& operator *=( const float scalar );
        
        /*!
         Multiply two 3x3 matrices and store in this matrix
         @param matrix matrix to mutliply this matrix by
         @return this matrix as a product of the two matrices
        */
        Matrix3& operator *=( const Matrix3& matrix );
        
        /*!
         Transpose this 3x3 matrix
        */
        void transpose( );
        
        /*!
         Return a transposed copy of this 3x3 matrix
         @return a transposed copy of this matrix
        */
        Matrix3 getTranspose( ) const;
        
        /*!
         Invert this 3x3 matrix
        */
        void invert( );
        
        /*!
         Return an inverted copy of this 3x3 matrix
         @return an inverted copy of this matrix
        */
        Matrix3 getInverse( ) const;
        
        /*!
         Load the identity matrix into this matrix
        */
        void loadIdentity();
        
        /*!
         Get the determinant of this 3x3 matrix
         @return the determinant of the matrix
        */
        float getDeterminant( ) const;
        
        /*!
         Create and return a rotation matrix that contains an x rotation
         @param radiansX rotation around X axis in radians
         @return a rotation matrix that represents a rotation of radiansX around the X axis
        */
        static Matrix3 rotationX( const float radiansX );
        
        /*!
         Create and return a rotation matrix that contains an y rotation
         @param radiansY rotation around Y axis in radians
         @return a rotation matrix that represents a rotation of radiansY around the Y axis
        */
        static Matrix3 rotationY( const float radiansY );
        
        /*!
         Create and return a rotation matrix that contains an Z rotation
         @param radiansZ rotation around Z axis in radians
         @return a rotation matrix that represents a rotation of radiansZ around the Z axis
        */
        static Matrix3 rotationZ( const float radiansZ );
        
        /*!
         Create and return a rotation matrix that contains a rotation around the Z axis
         then the Y axis, then the X axis
         @param radiansXYZ rotation around X, Y, and Z axes are contained within the vector
         @return a rotation matrix that represents a rotation of radiansXYZ.X around the X axis,
                 radiansXYZ.Y around the Y axis, and radiansXYZ.Z around the Z axis
        */
        static Matrix3 rotationZYX( const Vector3& radiansXYZ );
        
        /*!
         Create and return a matrix that represents a scale on all of the axes (X,Y,Z)
         @param scaleVec represents a scale along the axis matching the member name 
         @return a matrix that represents a scale in the XYZ directions matching the values in 
                 scaleVec
        */
        static Matrix3 scale( const Vector3& scaleVec );
        
        /*!
         Create and return a matrix that represents a skew matrix
         @param xSkewRadians degrees in radians of the skew on the x axis
         @param ySkewRadians degrees in radians of the skew on the y axis
         @param origin the origin point of the skew
         @return a matrix that represents a skew
        */
        static Matrix3 skew( const float xSkewRadians,
                             const float ySkewRadians,
                             const Vector2& origin );
        
        /*!
         Create and return a matrix that represents a skew gradient matrix matrix where y axis 
         is projected on the x. This matrix alone will not create a real skew, it must be used in 
         combo with scale and rotations
         @param thetaRadians degrees in radians of the skew
         @return a matrix that represents a skew
         */
        static Matrix3 skewGradient( const float skewGradientRadians );
        
        /*!
         Returns a string representation of the Matrix
         @return text output containing the value of each entry in matrix
        */
        std::string toString( );
        
    private:
        
        Vector3         mColumn0;
        Vector3         mColumn1;
        Vector3         mColumn2;
        
        static Matrix3 Identity;
        
    }; // class Matrix 4x4
     
}// namespace Walaber



#endif
