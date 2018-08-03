/**
 * @file quaternion.h
 * @brief Quaternion class.
 */
#ifndef _QUAT_H_
#define _QUAT_H_

#include <math.h>
#include <limits>
#include "matrix.h"

namespace math {

    /// Quaternion class.
    class Quat
    {
    public:
        /// Initialize to identity quaternion.
        Quat(): s(1.f), x(0.f), y(0.f), z(0.f) {}
        Quat(float _s, float _x, float _y, float _z): s(_s), x(_x), y(_y), z(_z) {}
        Quat(const Quat &quat) { *this = quat; }
        ~Quat() {}

        /// Adds two quaternions together and returns the result.
        Quat operator +(const Quat &quat) const
        {
            return Quat(s + quat.s, x + quat.x, y + quat.y, z + quat.z);
        }

        /// Multiply a quaternion by another and returns the result.
        Quat operator *(const Quat &quat) const
        {
            Quat result;
            Vector3D v0;
            Vector3D v1(x, y, z);
            Vector3D v2(quat.x, quat.y, quat.z);
            // Setting the resultant quaternion.
            result.s = s * quat.s - v1.DotProduct(v2);
            v0 = s * v2 + quat.s * v1 + v1.CrossProduct(v2);
            result.x = v0.x;
            result.y = v0.y;
            result.z = v0.z;
            return result;
        }

        /// Dot product of 2 quaternions.
        float DotProduct(const Quat &quat) const
        {
            Vector3D v1(x, y, z);
            Vector3D v2(quat.x, quat.y, quat.z);
            return s * quat.s + v1.DotProduct(v2);
        }

        /// Magnitude of the quaternion.
        float Length(void)
        {
            return sqrtf(s * s + x * x + y * y + z * z);
        }

        /// Returns inverse of quaternion.
        Quat Inverse(void) const
        {
            float l = s * s + x * x + y * y + z * z;
            return Quat(s / l, -x / l, -y / l, -z / l);
        }

        /// Sets the current quaternion to its inverse.
        void SetInverse(void)
        {
            *this = this->Inverse();
        }

        /**
         * @brief Calculates and returns the conjugate quaternion.
         * @return The conjugate quaternion.
         * @remarks The quaternion must necessarily be unitary, no normalization is done. This
         * function should be reserved for performance gains usage.
         */
        Quat Conjugate(void) const
        {
            return Quat(s, -x, -y, -z);
        }

        /// Set the current quaternion to its inverse.
        void SetConjugate(void)
        {
            *this = this->Conjugate();
        }

        /// Converts an axis angle pair into a quaternion, angle in degrees.
        static Quat ToQuaternion(const Vector3D &W, float angle)
        {
            Quat quat;
            // Converting angle to gradient.
            angle = angle / 180.f * PI;
            angle /= 2.f;
            quat.s = cosf(angle);
            float f = sinf(angle);
            quat.x = f * W.x;
            quat.y = f * W.y;
            quat.z = f * W.z;
            return quat;
        }

        /**
         * @brief Converts a UNIT rotation quaternion into its axis angle equivalent. @a angle in
         * degrees.
         * @param   [out] W     The axis of rotation.
         * @param   [out] angle The angle in degrees.
         */
        void ToAxisAngle(Vector3D &W, float &angle) const
        {
            // In case there is no rotation.
            float ns = (s < 0.f)? -s: s;
            if (EQUALTO(ns, 1.f, std::numeric_limits<float>::epsilon())) {
                angle = 0.f;
                W = Vector3D(0.f, 0.f, 1.f);
            } else { // Standard calculation method.
                angle = 2 * acosf(s);
                angle = angle / PI * 180.f;
                float f = sqrtf(1 - s * s);
                W.x = x / f;
                W.y = y / f;
                W.z = z / f;
            }
        }

        /// Converts a matrix 3D into a quaternion.
        static Quat ToQuaternion(const Matrix3D &matrix)
        {
            Quat quat;
            // Trace of a matrix (sum of diagonals).
            float trace = matrix.m00 + matrix.m11 + matrix.m22;

            if (trace > 0.f) {
                quat.s = sqrtf(trace + 1.f)/2.f;
                quat.x = (matrix.m12 - matrix.m21)/(4 * quat.s);
                quat.y = (matrix.m20 - matrix.m02)/(4 * quat.s);
                quat.z = (matrix.m01 - matrix.m10)/(4 * quat.s);
            } else {
                if ((matrix.m00 >= matrix.m11) && (matrix.m00 >= matrix.m22)) {
                    quat.x = sqrtf(matrix.m00 - matrix.m11 - matrix.m22 + 1.f)/2.f;
                    quat.s = (matrix.m12 - matrix.m21)/(4 * quat.x);
                    quat.y = (matrix.m01 + matrix.m10)/(4 * quat.x);
                    quat.z = (matrix.m02 + matrix.m20)/(4 * quat.x);
                } else if ((matrix.m11 >= matrix.m00) && (matrix.m11 >= matrix.m22)) {
                    quat.y = sqrtf(matrix.m11 - matrix.m00 - matrix.m22 + 1.f)/2.f;
                    quat.s = (matrix.m20 - matrix.m02)/(4 * quat.y);
                    quat.x = (matrix.m01 + matrix.m10)/(4 * quat.y);
                    quat.z = (matrix.m12 + matrix.m21)/(4 * quat.y);
                } else if((matrix.m22 >= matrix.m00) && (matrix.m22 >= matrix.m11)) {
                    quat.z = sqrtf(matrix.m22 - matrix.m00 - matrix.m11 + 1.f)/2.f;
                    quat.s = (matrix.m01 - matrix.m10)/(4 * quat.z);
                    quat.x = (matrix.m02 + matrix.m20)/(4 * quat.z);
                    quat.y = (matrix.m12 + matrix.m21)/(4 * quat.z);
                }
            }

            return quat;
        }

        /// Converts a matrix 4D into a quaternion.
        static Quat ToQuaternion(const Matrix4D &matrix)
        {
            Quat quat;
            // Trace of a matrix (sum of diagonals).
            float trace = matrix.m00 + matrix.m11 + matrix.m22;

            if (trace > 0.f) {
                quat.s = sqrtf(trace + 1.f)/2.f;
                quat.x = (matrix.m12 - matrix.m21)/(4 * quat.s);
                quat.y = (matrix.m20 - matrix.m02)/(4 * quat.s);
                quat.z = (matrix.m01 - matrix.m10)/(4 * quat.s);
            } else {
                if ((matrix.m00 >= matrix.m11) && (matrix.m00 >= matrix.m22)) {
                    quat.x = sqrtf(matrix.m00 - matrix.m11 - matrix.m22 + 1.f)/2.f;
                    quat.s = (matrix.m12 - matrix.m21)/(4 * quat.x);
                    quat.y = (matrix.m01 + matrix.m10)/(4 * quat.x);
                    quat.z = (matrix.m02 + matrix.m20)/(4 * quat.x);
                } else if ((matrix.m11 >= matrix.m00) && (matrix.m11 >= matrix.m22)) {
                    quat.y = sqrtf(matrix.m11 - matrix.m00 - matrix.m22 + 1.f)/2.f;
                    quat.s = (matrix.m20 - matrix.m02)/(4 * quat.y);
                    quat.x = (matrix.m01 + matrix.m10)/(4 * quat.y);
                    quat.z = (matrix.m12 + matrix.m21)/(4 * quat.y);
                } else if((matrix.m22 >= matrix.m00) && (matrix.m22 >= matrix.m11)) {
                    quat.z = sqrtf(matrix.m22 - matrix.m00 - matrix.m11 + 1.f)/2.f;
                    quat.s = (matrix.m01 - matrix.m10)/(4 * quat.z);
                    quat.x = (matrix.m02 + matrix.m20)/(4 * quat.z);
                    quat.y = (matrix.m12 + matrix.m21)/(4 * quat.z);
                }
            }

            return quat;
        }

        /// Transforms a quaternion to a 3D matrix.
        Matrix3D ToMatrix3D() const
        {
            Matrix3D matrix;
            float w, x, y, z;
            w = this->s;
            x = this->x;
            y = this->y;
            z = this->z;
            matrix.m00 = 1 - 2*y*y - 2*z*z;
            matrix.m01 = 2*x*y - 2*w*z;
            matrix.m02 = 2*x*z + 2*w*y;
            matrix.m10 = 2*x*y + 2*w*z;
            matrix.m11 = 1 - 2*x*x - 2*z*z;
            matrix.m12 = 2*y*z - 2*w*x;
            matrix.m20 = 2*x*z - 2*w*y;
            matrix.m21 = 2*y*z + 2*w*x;
            matrix.m22 = 1 - 2*x*x - 2*y*y;
            return matrix;
        }

        /// Transforms a quaternion to a 4D matrix.
        Matrix4D ToMatrix4D() const
        {
            Matrix4D matrix;
            float w, x, y, z;
            w = this->s;
            x = this->x;
            y = this->y;
            z = this->z;
            matrix.m00 = 1 - 2*y*y - 2*z*z;
            matrix.m01 = 2*x*y - 2*w*z;
            matrix.m02 = 2*x*z + 2*w*y;
            matrix.m10 = 2*x*y + 2*w*z;
            matrix.m11 = 1 - 2*x*x - 2*z*z;
            matrix.m12 = 2*y*z - 2*w*x;
            matrix.m20 = 2*x*z - 2*w*y;
            matrix.m21 = 2*y*z + 2*w*x;
            matrix.m22 = 1 - 2*x*x - 2*y*y;
            return matrix;
        }

    public:
        float s, x, y, z;
    };

    /// Multiplication of a scalar with a quat.
    static Quat operator *(float c, const Quat &quat)
    {
        Quat result;
        result.s = c * quat.s;
        result.x = c * quat.x;
        result.y = c * quat.y;
        result.z = c * quat.z;
        return result;
    }
}

#endif
