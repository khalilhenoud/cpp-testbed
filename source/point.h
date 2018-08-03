/**
 * @file point.h
 * @brief Geometric 3d point class.
 */
#ifndef _POINT_3D_
#define _POINT_3D_

#include <math.h>
#include <limits>

namespace math {

    /**
     * @brief Geometric 3d point class.
     */
    class Point3D
    {
    public:
        Point3D(): x(0), y(0), z(0), w(1) {}
        Point3D(float _x, float _y, float _z, float _w = 1.f): x(_x), y(_y), z(_z), w(_w) {}
        Point3D(const Point3D &point) { *this = point; }
        ~Point3D() {}

        /// Distance between 2 points.
        float Distance(const Point3D &point) const
        {
            return sqrtf(x * point.x + y * point.y + z * point.z);
        }

        /// Equality operator.
        bool operator ==(const Point3D &point) const
        {
            if((x >= (point.x - std::numeric_limits<float>::epsilon()) && x <= (point.x + std::numeric_limits<float>::epsilon())) &&
               (y >= (point.y - std::numeric_limits<float>::epsilon()) && y <= (point.y + std::numeric_limits<float>::epsilon())) &&
               (z >= (point.z - std::numeric_limits<float>::epsilon()) && z <= (point.z + std::numeric_limits<float>::epsilon())) &&
               (w >= (point.w - std::numeric_limits<float>::epsilon()) && w <= (point.w + std::numeric_limits<float>::epsilon())))
                return true;

            return false;
        }

        /// Overloading unary - operator.
        Point3D operator -(void) const
        {
            return Point3D(-x, -y, -z, w);
        }

    public:
        float x, y, z, w;
    };

    typedef Point3D Vertex;
}

#endif
