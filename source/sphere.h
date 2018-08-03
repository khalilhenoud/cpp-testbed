/**
 * @file sphere.h
 * @brief Geometric sphere.
 */
#ifndef _SPHERE_H_
#define _SPHERE_H_

#include <limits>
#include "geom.h"
#include "point.h"
#include "gvector.h"
#include "line.h"
#include "segment.h"

namespace math {

    /// Geometric sphere.
    class Sphere
    {
    public:
        Sphere() {}
        Sphere(const Sphere &sphere) { *this = sphere; }
        Sphere(const Point3D &_center, float _radius): center(_center), radius(_radius) {}
        ~Sphere() {}

        /**
         * @brief Classifies one sphere to another and returns the result.
         * @param   sphere      The sphere to classify against.
         * @return TANGENT, INTERSECT, COINCIDE, C1INSIDEC2, C2INSIDEC1, DISJOINT.
         */
        Classify ClassifyToSphere(const Sphere &sphere) const
        {
            LineSegment3D o1o2(center, sphere.center);
            float r1 = radius;
            float r2 = sphere.radius;;
            float r1plusr2 = r1 + r2;
            float r1plusr2square = r1plusr2 * r1plusr2;
            float r1minusr2 = r1 - r2;
            float r1minusr2square = r1minusr2 * r1minusr2;
            float r2minusr1 = r2 - r1;
            float length = o1o2.Length();
            float length_sqr = length * length;

            if (EQUALTO(r1plusr2square, length_sqr, std::numeric_limits<float>::epsilon()) || EQUALTO(r1minusr2square, length_sqr, std::numeric_limits<float>::epsilon()))
                return TANGENT;
            if (r1plusr2square > length_sqr)
                return INTERSECT;
            if ((center == sphere.center) && EQUALTO(radius, sphere.radius, std::numeric_limits<float>::epsilon()))
                return COINCIDENT;
            if (length < r2minusr1)
                return C1INSIDEC2;
            if (length < r1minusr2)
                return C2INSIDEC1;
            if (r1plusr2square < length_sqr)
                return DISJOINT;

            return DISJOINT;
        }

        /**
         * @brief Classify a line to a sphere.
         * @param   sphere                      The sphere to classify against.
         * @param   [out] intersection_point0   The first point of intersection.
         * @param   [out] intersection_point1   The second point of intersection.
         * @return INTERSECT, TANGENT or NONINTERSECT.
         */
        Classify ClassifyToSphere(const Line3D &line, Point3D &intersection_point0, Point3D &intersection_point1) const
        {
            Vector3D PC(line.position, center);
            float A = line.direction.Length() * line.direction.Length();
            float B = 2 * (line.direction.DotProduct(PC));
            float C = PC.Length() * PC.Length() - radius * radius;
            float delta = B * B - 4 * A * C;

            /*two intersection points exists*/
            if (delta > 0.f) {
                float t1 = (-B + sqrtf(delta))/(2 * A);
                float t2 = (-B - sqrtf(delta))/(2 * A);

                intersection_point0.x = line.direction.x * t1 + line.position.x;
                intersection_point0.y = line.direction.y * t1 + line.position.y;
                intersection_point0.z = line.direction.z * t1 + line.position.z;
                intersection_point1.x = line.direction.x * t2 + line.position.x;
                intersection_point1.y = line.direction.y * t2 + line.position.y;
                intersection_point1.z = line.direction.z * t2 + line.position.z;

                return INTERSECT;
            } else if(ISZERO(delta, std::numeric_limits<float>::epsilon())) {
                float t = (-B/2*A);

                intersection_point0.x = line.direction.x * t + line.position.x;
                intersection_point0.y = line.direction.y * t + line.position.y;
                intersection_point0.z = line.direction.z * t + line.position.z;
                intersection_point1 = intersection_point0;

                return TANGENT;
            }

            return NOINTERSECT;
        }

        /**
         * @brief Classify a point to a sphere.
         * @param   pt     The point to classify against.
         * @return Possible values: OUTSIDE, INSIDE, ON.
         */
        Classify ClassifyToSphere(const Point3D &pt) const
        {
            Vector3D pc(pt, this->center);
            if (pc.Length() > this->radius)
                return OUTSIDE;
            else if (pc.Length() < this->radius)
                return INSIDE;

            return ON;
        }

    public:
        Point3D center;
        float radius;
    };
}

#endif
