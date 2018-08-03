/**
 * @file matrix.h
 * @brief Matrix functionalities.
 */
#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <math.h>
#include <limits>
#include "point.h"
#include "gvector.h"
#include "geom.h"

namespace math {

    /**
     * @brief Matrix class functionality, 3 by 3.
     * @remarks Should be changed to use an array of floats.
     */
    class Matrix3D
    {
    public:
        Matrix3D()
        {
            // Initialize to identity matrix.
            m00 = m01 = m02 = m10 = m11 = m12 = m20 = m21 = m22 = 0.f;
            m00 = m11 = m22 = 1.f;
        }

        /**
         * @brief Returns the determinant.
         * @remarks The Matrix is assumed to be invertible (no checks are made).
         */
        float determinant(void) const
        {
            return m00 * m11 * m22 - m00 * m12 * m21 - m01 * m10 * m22 + m01 * m12 * m20 + m02 * m10 * m21 - m02 * m11 * m20;
        }

        /// Multiplication of 2 matrices, returns a new matrix.
        Matrix3D operator *(const Matrix3D &m) const
        {
            Matrix3D matrix;
            matrix.m00 = m00*m.m00 + m01*m.m10 + m02*m.m20;
            matrix.m01 = m00*m.m01 + m01*m.m11 + m02*m.m21;
            matrix.m02 = m00*m.m02 + m01*m.m12 + m02*m.m22;
            matrix.m10 = m10*m.m00 + m11*m.m10 + m12*m.m20;
            matrix.m11 = m10*m.m01 + m11*m.m11 + m12*m.m21;
            matrix.m12 = m10*m.m02 + m11*m.m12 + m12*m.m22;
            matrix.m20 = m20*m.m00 + m21*m.m10 + m22*m.m20;
            matrix.m21 = m20*m.m01 + m21*m.m11 + m22*m.m21;
            matrix.m22 = m20*m.m02 + m21*m.m12 + m22*m.m22;
            return matrix;
        }

        /// Multiply a matrix with a scalar.
        Matrix3D operator *(float scale) const
        {
            Matrix3D toreturn;
            toreturn.m00 = m00 * scale; toreturn.m01 = m01 * scale; toreturn.m02 = m02 * scale;
            toreturn.m10 = m10 * scale; toreturn.m11 = m11 * scale; toreturn.m12 = m12 * scale;
            toreturn.m20 = m20 * scale; toreturn.m21 = m21 * scale; toreturn.m22 = m22 * scale;
            return toreturn;
        }

        /**
         * @brief Multiplies a matrix with a vector.
         * @param m The vector in case.
         * @return The new transformed vector.
         */
        Vector3D operator *(const Vector3D &m) const
        {
            return Vector3D(m00 * m.x + m01 * m.y + m02 * m.z, m10 * m.x + m11 * m.y + m12 * m.z, m20 * m.x + m21 * m.y + m22 * m.z);
        }

        /**
         * @brief Multiplies a matrix with a point.
         * @param m The point to consider.
         * @return the New transformed point.
         */
        Point3D operator *(const Point3D &m) const
        {
            return Point3D(m00 * m.x + m01 * m.y + m02 * m.z, m10 * m.x + m11 * m.y + m12 * m.z, m20 * m.x + m21 * m.y + m22 * m.z);
        }

        /// Matrix addition.
        Matrix3D operator +(const Matrix3D &m) const
        {
            Matrix3D result;
            result.m00 = m00 + m.m00;	result.m01 = m01 + m.m01;	result.m02 = m02 + m.m02;
            result.m10 = m10 + m.m10;	result.m11 = m11 + m.m11;	result.m12 = m12 + m.m12;
            result.m20 = m20 + m.m20;	result.m21 = m21 + m.m21;	result.m22 = m22 + m.m22;
            return result;
        }

        /**
         * @brief Creates an arbitrary rotation matrix around axis @a W by @a alpha degrees.
         * @param   W       The Axis to rotate against.
         * @param   alpha   The angle in degrees.
         * @return A matrix representing the arbitrary rotation.
         */
        static Matrix3D AxisAngle(Vector3D W, float alpha)
        {
            W.Normalize();
            Matrix3D S, Ssquare, I;
            // Creating the matrix S.
            S.m00 = S.m11 = S.m22 = 0.f;
            S.m01 = -W.z;
            S.m02 = W.y;
            S.m10 = W.z;
            S.m12 = -W.x;
            S.m20 = -W.y;
            S.m21 = W.x;
            S.m22 = 0;
            // Create Ssquare.
            Ssquare = S * S;
            float angle = alpha/180.f * PI;
            // Calculating the rotation matrix.
            return I + S * sinf(angle) + Ssquare * (1.f - cosf(angle));
        }

        /**
         * @brief Given a rotation matrix, extract the axis and angle of rotation (in degrees).
         * @param   [out] W     The axis of rotation.
         * @param   [out] angle The rotation angle.
         * @remarks The matrix must be rotation matrix, otherwise the behavior is erroneous.
         */
        void ToAxisAngle(Vector3D &W, float &angle) const
        {
            float trace = 0.f;
            // The trace of a matrix is the sum of the diagonal terms.
            trace = m00 + m11 + m22;
            angle = acosf((trace - 1.f)/2.f);
            angle = angle / PI * 180.f;
            if (EQUALTO(angle, 0.f, std::numeric_limits<float>::epsilon()))
                W = Vector3D(0, 0, 1);
            else if (EQUALTO(angle, 180.f, std::numeric_limits<float>::epsilon())) {
                // In case it is equal to 180, we use a special procedure to get the axis.
                float w0, w1, w2;
                if ((m00 >= m11) && (m00 >= m22)) {
                    w0 = sqrtf(m00 - m11 - m22 + 1.f)/2.f;
                    w1 = m01/(2 * w0);
                    w2 = m02/(2 * w0);
                } else if ((m11 >= m00) && (m11 >= m22)) {
                    w1 = sqrtf(m11 - m00 - m22 + 1.f)/2.f;
                    w0 = m01/(2 * w1);
                    w2 = m12/(2 * w1);
                } else if((m22 >= m00) && (m22 >= m11)) {
                    w2 = sqrtf(m22 - m00 - m11 + 1.f)/2.f;
                    w0 = m02/(2 * w2);
                    w1 = m12/(2 * w2);
                }

                W.x = w0;
                W.y = w1;
                W.z = w2;
            } else { // Use the standard procedure to find the rotation axis.
                W.x = m21 - m12;
                W.y = m02 - m20;
                W.z = m10 - m01;
                W.Normalize();
            }
        }

    public:
        float m00, m01, m02,
              m10, m11, m12,
              m20, m21, m22;
    };

    /// Multiply a scalar and a matrix, returns a new matrix.
    static Matrix3D operator *(float scale, const Matrix3D &m)
    {
        Matrix3D toreturn;
        toreturn.m00 = m.m00 * scale; toreturn.m01 = m.m01 * scale; toreturn.m02 = m.m02 * scale;
        toreturn.m10 = m.m10 * scale; toreturn.m11 = m.m11 * scale; toreturn.m12 = m.m12 * scale;
        toreturn.m20 = m.m20 * scale; toreturn.m21 = m.m21 * scale; toreturn.m22 = m.m22 * scale;
        return toreturn;
    }

    /**
     * @brief Matrix class functionality, 4 by 4.
     * @remarks Should be changed to use an array of floats.
     */
    class Matrix4D
    {
    public:
        Matrix4D()
        {
            m01 = m02 = m03 = m10 = m12 = m13 = m20 = m21 = m23 = m30 = m31 = m32 = 0.f;
            m00 = m11 = m22 = m33 = 1;
        }

        ~Matrix4D() {}

        /// Find the determinant of the matrix.
        float determinant(void) const
        {
            Matrix3D D0, D1, D2, D3;
            float det0, det1, det2, det3;

            D0.m00 = m11; D0.m01 = m12; D0.m02 = m13;
            D0.m10 = m21; D0.m11 = m22; D0.m12 = m23;
            D0.m20 = m31; D0.m21 = m32; D0.m22 = m33;

            D1.m00 = m10; D1.m01 = m12; D1.m02 = m13;
            D1.m10 = m20; D1.m11 = m22; D1.m12 = m23;
            D1.m20 = m30; D1.m21 = m32; D1.m22 = m33;

            D2.m00 = m10; D2.m01 = m11; D2.m02 = m13;
            D2.m10 = m20; D2.m11 = m21; D2.m12 = m23;
            D2.m20 = m30; D2.m21 = m31; D2.m22 = m33;

            D3.m00 = m10; D3.m01 = m11; D3.m02 = m12;
            D3.m10 = m20; D3.m11 = m21; D3.m12 = m22;
            D3.m20 = m30; D3.m21 = m31; D3.m22 = m32;

            det0 = D0.determinant();
            det1 = D1.determinant();
            det2 = D2.determinant();
            det3 = D3.determinant();

            return m00 * det0 - m01 * det1 + m02 * det2 - m03 * det3;
        }

        /// Returns an matrix representing the inverse of the current one.
        Matrix4D Inverse(void) const
        {
            float det1, det2, det3, det4, det5, det6, det7, det8, det9, det10, det11, det12, det13, det14, det15, det16;
            Matrix3D m;

            m.m00 = m11;	 m.m01 = m12;	 m.m02 = m13;
            m.m10 = m21;	 m.m11 = m22;	 m.m12 = m23;
            m.m20 = m31;	 m.m21 = m32;	 m.m22 = m33;
            det1 = m.determinant();

            m.m00 = m10;	 m.m01 = m12;	 m.m02 = m13;
            m.m10 = m20;	 m.m11 = m22;	 m.m12 = m23;
            m.m20 = m30;	 m.m21 = m32;	 m.m22 = m33;
            det2 = m.determinant();

            m.m00 = m10;	 m.m01 = m11;	 m.m02 = m13;
            m.m10 = m20;	 m.m11 = m21;	 m.m12 = m23;
            m.m20 = m30;	 m.m21 = m31;	 m.m22 = m33;
            det3 = m.determinant();

            m.m00 = m10;	 m.m01 = m11;	 m.m02 = m12;
            m.m10 = m20;	 m.m11 = m21;	 m.m12 = m22;
            m.m20 = m30;	 m.m21 = m31;	 m.m22 = m32;
            det4 = m.determinant();

            m.m00 = m01;	 m.m01 = m02;	 m.m02 = m03;
            m.m10 = m21;	 m.m11 = m22;	 m.m12 = m23;
            m.m20 = m31;	 m.m21 = m32;	 m.m22 = m33;
            det5 = m.determinant();

            m.m00 = m00;	 m.m01 = m02;	 m.m02 = m03;
            m.m10 = m20;	 m.m11 = m22;	 m.m12 = m23;
            m.m20 = m30;	 m.m21 = m32;	 m.m22 = m33;
            det6 = m.determinant();

            m.m00 = m00;	 m.m01 = m01;	 m.m02 = m03;
            m.m10 = m20;	 m.m11 = m21;	 m.m12 = m23;
            m.m20 = m30;	 m.m21 = m31;	 m.m22 = m33;
            det7 = m.determinant();

            m.m00 = m00;	 m.m01 = m01;	 m.m02 = m02;
            m.m10 = m20;	 m.m11 = m21;	 m.m12 = m22;
            m.m20 = m30;	 m.m21 = m31;	 m.m22 = m32;
            det8 = m.determinant();

            m.m00 = m01;	 m.m01 = m02;	 m.m02 = m03;
            m.m10 = m11;	 m.m11 = m12;	 m.m12 = m13;
            m.m20 = m31;	 m.m21 = m32;	 m.m22 = m33;
            det9 = m.determinant();

            m.m00 = m00;	 m.m01 = m02;	 m.m02 = m03;
            m.m10 = m10;	 m.m11 = m12;	 m.m12 = m13;
            m.m20 = m30;	 m.m21 = m32;	 m.m22 = m33;
            det10 = m.determinant();

            m.m00 = m00;	 m.m01 = m01;	 m.m02 = m03;
            m.m10 = m10;	 m.m11 = m11;	 m.m12 = m13;
            m.m20 = m30;	 m.m21 = m31;	 m.m22 = m33;
            det11 = m.determinant();

            m.m00 = m00;	 m.m01 = m01;	 m.m02 = m02;
            m.m10 = m10;	 m.m11 = m11;	 m.m12 = m12;
            m.m20 = m30;	 m.m21 = m31;	 m.m22 = m32;
            det12 = m.determinant();

            m.m00 = m01;	 m.m01 = m02;	 m.m02 = m03;
            m.m10 = m11;	 m.m11 = m12;	 m.m12 = m13;
            m.m20 = m21;	 m.m21 = m22;	 m.m22 = m23;
            det13 = m.determinant();

            m.m00 = m00;	 m.m01 = m02;	 m.m02 = m03;
            m.m10 = m10;	 m.m11 = m12;	 m.m12 = m13;
            m.m20 = m20;	 m.m21 = m22;	 m.m22 = m23;
            det14 = m.determinant();

            m.m00 = m00;	 m.m01 = m01;	 m.m02 = m03;
            m.m10 = m10;	 m.m11 = m11;	 m.m12 = m13;
            m.m20 = m20;	 m.m21 = m21;	 m.m22 = m23;
            det15 = m.determinant();

            m.m00 = m00;	 m.m01 = m01;	 m.m02 = m02;
            m.m10 = m10;	 m.m11 = m11;	 m.m12 = m12;
            m.m20 = m20;	 m.m21 = m21;	 m.m22 = m22;
            det16 = m.determinant();

            Matrix4D matrix;
            matrix.m00 = det1;			matrix.m01 = -det2;			matrix.m02 = det3;			matrix.m03 = -det4;
            matrix.m10 = -det5;			matrix.m11 = det6;			matrix.m12 = -det7;			matrix.m13 = det8;
            matrix.m20 = det9;			matrix.m21 = -det10;		matrix.m22 = det11;			matrix.m23 = -det12;
            matrix.m30 = -det13;		matrix.m31 = det14;			matrix.m32 = -det15;		matrix.m33 = det16;

            matrix = matrix.Transpose();
            matrix = matrix * (1/determinant());
            return matrix;
        }

        /// Returns the transpose of the matrix.
        Matrix4D Transpose(void) const
        {
            Matrix4D t;
            t.m00 = m00;	t.m01 = m10;	t.m02 = m20;	t.m03 = m30;
            t.m10 = m01;	t.m11 = m11;	t.m12 = m21;	t.m13 = m31;
            t.m20 = m02;	t.m21 = m12;	t.m22 = m22;	t.m23 = m32;
            t.m30 = m03;	t.m31 = m13;	t.m32 = m23;	t.m33 = m33;
            return t;
        }

        /// Inverse the current matrix.
        void SetInverse(void)
        {
            (*this) = this->Inverse();
        }

        /// Transpose the current matrix.
        void SetTranspose(void)
        {
            (*this) = this->Transpose();
        }

        /**
         * @brief Creates a rotation matrix along X.
         * @param angle The angle in radian.
         */
        static Matrix4D RotationX(float angle)
        {
            Matrix4D toreturn;
            toreturn.m11 = toreturn.m22 = cosf(angle);
            toreturn.m21 = sinf(angle);
            toreturn.m12 = -toreturn.m21;
            return toreturn;
        }

        /// Creates a rotation matrix along Y.
        static Matrix4D RotationY(float angle)
        {
            Matrix4D toreturn;
            toreturn.m00 = toreturn.m22 = cosf(angle);
            toreturn.m02 = sinf(angle);
            toreturn.m20 = -toreturn.m02;
            return toreturn;
        }

        /// Creates a rotation matrix along Z.
        static Matrix4D RotationZ(float angle)
        {
            Matrix4D toreturn;
            toreturn.m00 = toreturn.m11 = cosf(angle);
            toreturn.m10 = sinf(angle);
            toreturn.m01 = -toreturn.m10;
            return toreturn;
        }

        /// Creates a translation matrix.
        static Matrix4D Translation(float tx, float ty, float tz)
        {
            Matrix4D toreturn;
            toreturn.m03 = tx;
            toreturn.m13 = ty;
            toreturn.m23 = tz;
            return toreturn;
        }

        /// Creates a scale matrix.
        static Matrix4D Scale(float sx, float sy, float sz)
        {
            Matrix4D toreturn;
            toreturn.m00 = sx;
            toreturn.m11 = sy;
            toreturn.m22 = sz;
            return toreturn;
        }

        /**
         * @brief Calculate the matrix that when multiplied by another vector 'v' will give the
         * equivalent @a vec cross 'v' resultant vector.
         * @param vec The vector that is the first operand of the cross product operation. It cannot
         * be constant as it needs to be normalized.
         * @return A matrix that when multiplied by another vector 'v' will give the equivalent @a
         * vec cross 'v' resultant vector.
         */
        static Matrix4D CrossProduct(Vector3D vec)
        {
            Matrix4D toreturn;
            vec.Normalize();

            toreturn.m00 = toreturn.m30 = toreturn.m11 = toreturn.m31 = toreturn.m22 = toreturn.m32 = 0;
            toreturn.m10 = vec.z;
            toreturn.m20 = -vec.y;
            toreturn.m01 = -vec.z;
            toreturn.m21 = vec.x;
            toreturn.m02 = vec.y;
            toreturn.m12 = -vec.x;

            return toreturn;
        }

        /**
         * @brief Copies the element of the matrix to the 16 float array in a column major order.
         * Same as OpenGL expects it.
         * @param [out] m Buffer to hold the values.
         */
        void ToArrayColumnMajor(float *m) const
        {
            m[0] = m00;
            m[1] = m10;
            m[2] = m20;
            m[3] = m30;
            m[4] = m01;
            m[5] = m11;
            m[6] = m21;
            m[7] = m31;
            m[8] = m02;
            m[9] = m12;
            m[10] = m22;
            m[11] = m32;
            m[12] = m03;
            m[13] = m13;
            m[14] = m23;
            m[15] = m33;
        }

        /// Multiply a matrix with a scalar.
        Matrix4D operator *(float scale) const
        {
            Matrix4D toreturn;
            toreturn.m00 = m00 * scale; toreturn.m01 = m01 * scale; toreturn.m02 = m02 * scale; toreturn.m03 = m03 * scale;
            toreturn.m10 = m10 * scale; toreturn.m11 = m11 * scale; toreturn.m12 = m12 * scale; toreturn.m13 = m13 * scale;
            toreturn.m20 = m20 * scale; toreturn.m21 = m21 * scale; toreturn.m22 = m22 * scale; toreturn.m23 = m23 * scale;
            toreturn.m30 = m30 * scale; toreturn.m31 = m31 * scale; toreturn.m32 = m32 * scale; toreturn.m33 = m33 * scale;
            return toreturn;
        }

        /// Multiply a matrix with a vector.
        Vector3D operator *(const Vector3D &m) const
        {
            return Vector3D(m00 * m.x + m01 * m.y + m02 * m.z + m03 * m.w,
                            m10 * m.x + m11 * m.y + m12 * m.z + m13 * m.w,
                            m20 * m.x + m21 * m.y + m22 * m.z + m23 * m.w,
                            m30 * m.x + m31 * m.y + m32 * m.z + m33 * m.w);
        }

        /// Multiply a matrix with a point.
        Point3D operator *(const Point3D &m) const
        {
            return Point3D(m00 * m.x + m01 * m.y + m02 * m.z + m03 * m.w,
                           m10 * m.x + m11 * m.y + m12 * m.z + m13 * m.w,
                           m20 * m.x + m21 * m.y + m22 * m.z + m23 * m.w,
                           m30 * m.x + m31 * m.y + m32 * m.z + m33 * m.w);
        }

        /// Multiply 2 matrices.
        Matrix4D operator *(const Matrix4D &m) const
        {
            Matrix4D mt;
            mt.m00 = m00*m.m00 + m01*m.m10 + m02*m.m20 + m03*m.m30;
            mt.m01 = m00*m.m01 + m01*m.m11 + m02*m.m21 + m03*m.m31;
            mt.m02 = m00*m.m02 + m01*m.m12 + m02*m.m22 + m03*m.m32;
            mt.m03 = m00*m.m03 + m01*m.m13 + m02*m.m23 + m03*m.m33;
            mt.m10 = m10*m.m00 + m11*m.m10 + m12*m.m20 + m13*m.m30;
            mt.m11 = m10*m.m01 + m11*m.m11 + m12*m.m21 + m13*m.m31;
            mt.m12 = m10*m.m02 + m11*m.m12 + m12*m.m22 + m13*m.m32;
            mt.m13 = m10*m.m03 + m11*m.m13 + m12*m.m23 + m13*m.m33;
            mt.m20 = m20*m.m00 + m21*m.m10 + m22*m.m20 + m23*m.m30;
            mt.m21 = m20*m.m01 + m21*m.m11 + m22*m.m21 + m23*m.m31;
            mt.m22 = m20*m.m02 + m21*m.m12 + m22*m.m22 + m23*m.m32;
            mt.m23 = m20*m.m03 + m21*m.m13 + m22*m.m23 + m23*m.m33;
            mt.m30 = m30*m.m00 + m31*m.m10 + m32*m.m20 + m33*m.m30;
            mt.m31 = m30*m.m01 + m31*m.m11 + m32*m.m21 + m33*m.m31;
            mt.m32 = m30*m.m02 + m31*m.m12 + m32*m.m22 + m33*m.m32;
            mt.m33 = m30*m.m03 + m31*m.m13 + m32*m.m23 + m33*m.m33;
            return mt;
        }

        /**
         * @brief Creates an arbitrary rotation matrix around axis @a W by @a alpha degrees.
         * @param   W       The Axis to rotate against.
         * @param   alpha   The angle in degrees.
         * @return A matrix representing the arbitrary rotation.
         */
        static Matrix4D AxisAngle(Vector3D W, float alpha)
        {
            Matrix3D aa = Matrix3D::AxisAngle(W, alpha);
            Matrix4D matrix;
            matrix.m00 = aa.m00;	matrix.m01 = aa.m01;	matrix.m02 = aa.m02;	matrix.m03 = 0.f;
            matrix.m10 = aa.m10;	matrix.m11 = aa.m11;	matrix.m12 = aa.m12;	matrix.m13 = 0.f;
            matrix.m20 = aa.m20;	matrix.m21 = aa.m21;	matrix.m22 = aa.m22;	matrix.m23 = 0.f;
            matrix.m30 = 0.f;		matrix.m31 = 0.f;		matrix.m32 = 0.f;		matrix.m33 = 1.f;
            return matrix;
        }

        /**
         * @brief Given a rotation matrix, extract the axis and angle of rotation (in degrees).
         * @param   [out] W     The axis of rotation.
         * @param   [out] angle The rotation angle.
         * @remarks The matrix must be rotation matrix, otherwise the behavior is erroneous.
         */
        void ToAxisAngle(Vector3D &W, float &angle)
        {
            float trace = 0.f;
            // The trace of a matrix is the sum of the diagonal terms.
            trace = m00 + m11 + m22;
            angle = acosf((trace - 1.f)/2.f);
            angle = angle / PI * 180.f;
            if (EQUALTO(angle, 0.f, std::numeric_limits<float>::epsilon()))
                W = Vector3D(0, 0, 1);
            else if (EQUALTO(angle, 180.f, std::numeric_limits<float>::epsilon())) {
                // In case it is equal to 180, we use a special procedure to get the axis.
                float w0, w1, w2;
                if ((m00 >= m11) && (m00 >= m22)) {
                    w0 = sqrtf(m00 - m11 - m22 + 1.f)/2.f;
                    w1 = m01/(2 * w0);
                    w2 = m02/(2 * w0);
                } else if ((m11 >= m00) && (m11 >= m22)) {
                    w1 = sqrtf(m11 - m00 - m22 + 1.f)/2.f;
                    w0 = m01/(2 * w1);
                    w2 = m12/(2 * w1);
                } else if((m22 >= m00) && (m22 >= m11)) {
                    w2 = sqrtf(m22 - m00 - m11 + 1.f)/2.f;
                    w0 = m02/(2 * w2);
                    w1 = m12/(2 * w2);
                }

                W.x = w0;
                W.y = w1;
                W.z = w2;
            } else { // Use the standard procedure to find the rotation axis.
                W.x = m21 - m12;
                W.y = m02 - m20;
                W.z = m10 - m01;
                W.Normalize();
            }
        }

    public:
        float m00, m01, m02, m03,
              m10, m11, m12, m13,
              m20, m21, m22, m23,
              m30, m31, m32, m33;
    };

    /// Multiply a scalar and a matrix, returns a new matrix.
    static Matrix4D operator *(float scale, const Matrix4D &m)
    {
        Matrix4D toreturn;
        toreturn.m00 = m.m00 * scale; toreturn.m01 = m.m01 * scale; toreturn.m02 = m.m02 * scale; toreturn.m03 = m.m03 * scale;
        toreturn.m10 = m.m10 * scale; toreturn.m11 = m.m11 * scale; toreturn.m12 = m.m12 * scale; toreturn.m13 = m.m13 * scale;
        toreturn.m20 = m.m20 * scale; toreturn.m21 = m.m21 * scale; toreturn.m22 = m.m22 * scale; toreturn.m23 = m.m23 * scale;
        toreturn.m30 = m.m30 * scale; toreturn.m31 = m.m31 * scale; toreturn.m32 = m.m32 * scale; toreturn.m33 = m.m33 * scale;
        return toreturn;
    }
}

#endif
