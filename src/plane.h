/**
 * @file plane.h
 * @brief Plane in 3D space.
 */
#ifndef _PLANE_H_
#define _PLANE_H_

#include <math.h>
#include <limits>
#include "geom.h"
#include "point.h"
#include "line.h"
#include "segment.h"

namespace math {

    /**
     * @brief Represents plane in 3D space. Defined by a normal and a position belonging to the
     * plane.
     */
    class Plane
    {
    public:
        Plane() {}
        Plane(const Plane &plane) { *this = plane; }
        Plane(const Point3D &_position, const Vector3D &_normal): position(_position), normal(_normal) {}
        ~Plane() {}

        /**
         * @brief Classify a plane to another. The intersection line is specified in @a line.
         * @param   plane           The plane to classify against.
         * @param   [out] line      The resulting line if an intersection occurs.
         * @return PARALLEL, COINCIDENT, INTERSECT.
         */
        Classify ClassifyToPlane(const Plane &plane, Line3D &line) const
        {
            Vector3D crossproduct = normal.CrossProduct(plane.normal);
            Vector3D ab(position, plane.position);

            // If the length of the cross product is zero then we have parallel or coincident case.
            if (ISZERO(crossproduct.Length(), std::numeric_limits<float>::epsilon())) {
                float dotproduct = ab.DotProduct(normal);

                // If the dot product is zero then the planes are coincident, otherwise parallel.
                if (ISZERO(dotproduct, std::numeric_limits<float>::epsilon()))
                    return COINCIDENT;

                return PARALLEL;
            }

            // The plane intersect at a line that we will calculate.
            Vector3D v = normal.CrossProduct(plane.normal);
            Vector3D u = normal.CrossProduct(v);
            float alpha = (plane.normal.DotProduct(ab))/(plane.normal.DotProduct(u));
            Point3D p(position.x + alpha * u.x, position.y + alpha * u.y, position.z + alpha * u.z);

            // Set the line position and direction and return INTERSECT.
            line.position = p;
            line.direction = v;
            return INTERSECT;
        }

        /// Returns distance to the plane.
        float DistanceToPlane(const Point3D &pt) const
        {
            Vector3D am(this->position, pt);
            float dotproduct = am.DotProduct(this->normal);
            return ((dotproduct < 0.f)? - dotproduct: dotproduct)/(this->normal.Length());
        }

        /**
         * @brief Classify a point to a plane.
         * @param   pt     The point to classify.
         * @return FRONT, BACK, ON.
         */
        Classify ClassifyToPlane(const Point3D &pt) const
        {
            Vector3D am(this->position, pt);
            float dotproduct = am.DotProduct(this->normal);
            if (dotproduct > std::numeric_limits<float>::epsilon())
                return FRONT;
            else if (dotproduct < -std::numeric_limits<float>::epsilon())
                return BACK;
            else if (ISZERO(dotproduct, std::numeric_limits<float>::epsilon()))
                return ON;

            /// Should never reach.
            return ON;
        }

        /**
         * @brief Calculates the projection point on the plane.
         * @param   pt     The point we want to find the projection of.
         * @return The projection point of @a pt on the plane.
         */
        Point3D ProjectOnPlane(const Point3D &pt) const
        {
            Vector3D direction(-this->normal.x, -this->normal.y, -this->normal.z);
            Line3D line(pt, direction);
            Point3D intersectionpoint;
            IntersectPlaneAt(line, intersectionpoint);
            return intersectionpoint;
        }

        /**
         * @brief Classify a line to a plane.
         * @param   plane   The plane to classify against.
         * @return PARALLEL, BELONG or INTERSECT.
         */
        Classify ClassifyToPlane(const Line3D &line) const
        {
            float dotproduct = normal.DotProduct(line.direction);
            /*if the dot product is zero, then it is either PARALLEL or BELONG*/
            if (ISZERO(dotproduct, std::numeric_limits<float>::epsilon())) {
                Vector3D ab(line.position, position);
                if (ISZERO(ab.DotProduct(normal), std::numeric_limits<float>::epsilon()))
                    return BELONG;

                return PARALLEL;
            }

            return INTERSECT;
        }

        /**
         * @brief Finds intersection with a plane if any.
         * @param   plane                       The plane to intersect with.
         * @param   [out] intersection_point    The point of intersection if any.
         * @return A boolean indicating whether an intersection occurred. If true than @a
         * intersection_point will be filled.
         * @remarks It seems this function is prone to imprecision errors.
         */
        bool IntersectPlaneAt(const Line3D &line, Point3D &intersection_point) const
        {
            if (ClassifyToPlane(line) != INTERSECT)
                return false;

            Vector3D N = normal;
            // Finding D in Ax + By + Cz + D = 0; i.e. the equation of the plane.
            float D = - N.x * position.x - N.y * position.y - N.z * position.z;

            Vector3D M(line.position.x, line.position.y, line.position.z);
            // Finding t, the intersection time.
            float t = - (N.DotProduct(M) + D)/(N.DotProduct(line.direction));

            intersection_point.x = line.position.x + line.direction.x * t;
            intersection_point.y = line.position.y + line.direction.y * t;
            intersection_point.z = line.position.z + line.direction.z * t;

            return true;
        }

        /**
         * @brief Checks whether the segment intersects the plane.
         * @param   plane                       The plane to check against.
         * @param   [out] intersection_point    The intersection point with the plane.
         * @return Boolean indication whether the intersection occurred or not.
         */
        bool IntersectPlaneAt(const LineSegment3D &segment, Point3D &intersection_point) const
        {
            Line3D line(segment.pointA, segment.pointB);

            // If the infinite line the segment belongs to doesn't intersect the plane return false.
            if (!IntersectPlaneAt(line, intersection_point))
                return false;

            float t = segment.GetIntersectionTime(intersection_point);
            return t <= 1.f && t >= 0.f;
        }

    public:
        /// A point in space belonging to the plane.
        Point3D position;
        /// The normal to the plane.
        Vector3D normal;
    };
}

#endif
