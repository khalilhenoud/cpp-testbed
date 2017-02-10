/**
 * @file line.h
 * @brief Geometric 3D line.
 */
 #ifndef _LINE_3D_H
 #define _LINE_3D_H

 #include <math.h>
 #include <limits>
 #include "geom.h"
 #include "point.h"
 #include "gvector.h"

 namespace math {

    /// Geometric 3D line.
    class Line3D
    {
    public:
        Line3D() {}
        Line3D(const Line3D &line) { *this = line; }
        Line3D(const Point3D &_position, const Vector3D &_direction): position(_position), direction(_direction) {}
        Line3D(const Point3D &pointA, const Point3D &pointB): direction(Vector3D(pointA, pointB)), position(pointA) {}
        ~Line3D() {}

        /**
         * @brief Classify 1 line to another.
         * @param   line      The line to classify against.
         * @return PARALLEL COINCIDENT COPLANAR or DISJOINT.
         */
        Classify ClassifyToLine(const Line3D &line) const
        {
            Vector3D cross = direction.CrossProduct(line.direction);

            // Parallel or coincident.
            if (ISZERO(cross.Length(), std::numeric_limits<float>::epsilon())) {
                // Vector formed by joining the points belonging to the lines.
                Vector3D ab(position, line.position);
                // Getting the cross product.
                Vector3D cross2 = ab.CrossProduct(direction);

                // If it is 0 in length, this indicates the lines are coincident.
                if (ISZERO(cross2.Length(), std::numeric_limits<float>::epsilon()))
                    return COINCIDENT;

                return PARALLEL;
            }

            // Check if the lines belong to the same plane.
            if (AreCoplanar(line))
                return COPLANAR;

            return DISJOINT;
        }

        /// Returns angle between 2 lines in degrees.
        float AngleWithLine(const Line3D &line) const
        {
            Vector3D direction1 = direction;
            Vector3D direction2 = line.direction;
            direction1.Normalize();
            direction2.Normalize();
            float dotproduct = direction1.DotProduct(direction2);
            // Getting the angle in radian and change it to degrees.
            return acosf(dotproduct) / PI * 180.f;
        }

        /**
         * @brief Returns a boolean indicating whether the lines are coplanar.
         * @param   line      The other line to consider.
         * @return A boolean indicating whether they are coplanar or not.
         */
        bool AreCoplanar(const Line3D &line) const
        {
            Vector3D ab(position, line.position);
            Vector3D N = direction.CrossProduct(line.direction);

            // If they are parallel or coincident return true.
            if (ISZERO(N.Length(), std::numeric_limits<float>::epsilon()))
                return true;

            // Get the dotproduct.
            float dotproduct = N.DotProduct(ab);
            if (ISZERO(dotproduct, std::numeric_limits<float>::epsilon()))
                return true;

            return false;
        }

        /**
         * @brief Finds the intersection of 2 lines if any. @a intersection_point holds the result
         * of that intersection.
         * @param     line                        The other line to consider.
         * @param     [out] intersection_point     Holds the point of intersection if any.
         * @return Boolean indicating if there was an intersection at which point you can find the
         * resulting point in @a intersection_point.
         */
        bool IntersectLineAt(const Line3D &line, Point3D &intersection_point) const
        {
            // They have to be coplanar to be considered.
            if (ClassifyToLine(line) != COPLANAR)
                return false;

            float a1, b1, c1, xA, yA, zA, a2, b2, c2, xB, yB, zB;
            a1 = direction.x;
            xA = position.x;
            b1 = direction.y;
            yA = position.y;
            c1 = direction.z;
            zA = position.z;
            a2 = line.direction.x;
            xB = line.position.x;
            b2 = line.direction.y;
            yB = line.position.y;
            c2 = line.direction.z;
            zB = line.position.z;
            float condition1, condition2, condition3;
            condition1 = a1 * b2 - a2 * b1;
            condition2 = b1 * c2 - b2 * c1;
            condition3 = a1 * c2 - a2 * c1;

            float t, tprime;
            if (!ISZERO(condition1, std::numeric_limits<float>::epsilon())) {
                t = (a2 * (yB - yA) + b2 * (xA - xB))/condition1;
                tprime = (a1 * (yB - yA) + b1 * (xA - xB))/condition1;
            } else if (!ISZERO(condition2, std::numeric_limits<float>::epsilon())) {
                t = (-(c2 * (yA - yB) + b2 * (zB - zA)))/condition2;
                tprime = (-(c1 * (yA - yB) + b1 * (zB - zA)))/condition2;
            } else {
                t = (a2 * (zA - zB) - c2 * (xA - xB))/condition3;
                tprime = (a1 * (zA - zB) - c1 * (xA - xB))/condition3;
            }

            // Parametric intersection point.
            intersection_point.x = a1 * t + xA;
            intersection_point.y = b1 * t + yA;
            intersection_point.z = c1 * t + zA;

            return true;
        }

        /// Returns a boolean indicating whether the point is on the line or not.
        bool IsOnLine(const Point3D &pt) const
        {
            Vector3D pa(pt, this->position);
            // If the magnitude of the cross product is zero then the point belongs to the line.
            if (ISZERO(pa.CrossProduct(this->direction).Length(), std::numeric_limits<float>::epsilon()))
                return true;

            return false;
        }

        /// Distance from point to line.
        float DistanceToLine(const Point3D &pt) const
        {
            Point3D x1 = this->position;
            Point3D x2 = this->position + this->direction;
            Point3D x0 = pt;
            Vector3D v0 = x2 - x1;
            Vector3D v1 = x1 - x0;
            Vector3D v2 = x2 - x1;
            Vector3D v3 = v0.CrossProduct(v1);
            return v3.Length()/v2.Length();
        }

        /// Returns the projection point of pt along the line.
        Point3D ProjectOnLine(const Point3D &pt) const
        {
            Vector3D v = this->direction;
            Vector3D PA(pt, this->position);
            float t0 = - (v.DotProduct(PA))/(v.Length() * v.Length());
            return Point3D(this->position.x + this->direction.x * t0, this->position.y + this->direction.y * t0, this->position.z + this->direction.z * t0);
        }

    public:
        /// Direction along the line.
        Vector3D direction;
        /// Point belonging to the line.
        Point3D position;
    };
 }

#endif
