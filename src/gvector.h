/**
 * @file gvector.h
 * @brief 3d geometric vector class.
 */
#ifndef _VECTOR_3D_
#define _VECTOR_3D_

#include <math.h>
#include <limits>
#include "point.h"

namespace math {

    /**
     * @brief 3D vector class.
     */
    class Vector3D
    {
    public:
        Vector3D(): x(0), y(1), z(0), w(0) {}
        Vector3D(const Vector3D &vec) { *this = vec; }
        Vector3D(float _x, float _y, float _z, float _w = 0.f): x(_x), y(_y), z(_z), w(_w) {}
        Vector3D(const Point3D &A, const Point3D &B): x(B.x - A.x), y(B.y - A.y), z(B.z - A.z), w(B.w - A.w) {}
        ~Vector3D() {}

        /// Multiply a vector by a scalar.
        Vector3D operator *(float scale) const
        {
            return Vector3D(x * scale, y * scale, z * scale, w * scale);
        }

        Vector3D operator *=(float scale)
        {
            x *= scale;
            y *= scale;
            z *= scale;
            w *= scale;
            return *this;
        }

        /// Subtracts 2 vectors.
        Vector3D operator -(const Vector3D &vec) const
        {
             return Vector3D(x - vec.x, y - vec.y, z - vec.z, w - vec.w);
        }

        /// Overloading unary - operator.
        Vector3D operator -(void) const
        {
            return Vector3D(-x, -y, -z, w);
        }

        Vector3D operator -=(const Vector3D &vec)
        {
            x -= vec.x;
            y -= vec.y;
            z -= vec.z;
            w -= vec.w;
            return *this;
        }

        /// Add 2 vectors.
        Vector3D operator +(const Vector3D &vec) const
        {
            return Vector3D(x + vec.x, y + vec.y, z + vec.z, w + vec.w);
        }

        Vector3D operator +=(const Vector3D &vec)
        {
            x += vec.x;
            y += vec.y;
            z += vec.z;
            w += vec.w;
            return *this;
        }

        /// Returns the length of the vector.
        float Length(void) const
        {
            return sqrtf(x * x + y * y + z * z);
        }

        /// Returns the dot product.
        float DotProduct(const Vector3D &vec) const
        {
            return x * vec.x + y * vec.y + z * vec.z;
        }

        /// Returns the resultant vector.
        Vector3D CrossProduct(const Vector3D &vec) const
        {
            return Vector3D(y * vec.z - vec.y * z, vec.x * z - x * vec.z, x * vec.y - vec.x * y);
        }

        /// Compares equality within an epsilon difference.
        bool operator ==(const Vector3D &vec) const
        {
            if ((x >= (vec.x - std::numeric_limits<float>::epsilon()) && x <= (vec.x + std::numeric_limits<float>::epsilon())) &&
                (y >= (vec.y - std::numeric_limits<float>::epsilon()) && y <= (vec.y + std::numeric_limits<float>::epsilon())) &&
                (z >= (vec.z - std::numeric_limits<float>::epsilon()) && z <= (vec.z + std::numeric_limits<float>::epsilon())) &&
                (w >= (vec.w - std::numeric_limits<float>::epsilon()) && w <= (vec.w + std::numeric_limits<float>::epsilon())))
                    return true;

            return false;
        }

        /// Normalizes the current vector.
        void Normalize(void)
        {
            float length = Length();
            x /= length;
            y /= length;
            z /= length;
        }

    public:
        float x, y, z, w;
    };

    /// Multiply a scalar with a vector.
	static Vector3D operator *(float scale, const Vector3D &vec)
	{
        return Vector3D(vec.x * scale, vec.y * scale, vec.z * scale, vec.w * scale);
    }

    /// Add a point and a vector.
	static Point3D operator +(const Point3D &pt, const Vector3D &vec)
	{
        return Point3D(pt.x + vec.x, pt.y + vec.y, pt.z + vec.z, pt.w + vec.w);
    }

    /// Returns the vector resulting from the difference between 2 points.
	static Vector3D operator -(const Point3D &pt1, const Point3D &pt2)
	{
        return Vector3D(pt1.x - pt2.x, pt1.y - pt2.y, pt1.z - pt2.z, pt1.w - pt2.w);
    }
}

#endif // _VECTOR_3D_

