/**
 * @file segment.h
 * @brief Line segment in 3D.
 */
#ifndef _LINE_SEGMENT_3D_
#define _LINE_SEGMENT_3D_

#include <limits>
#include "geom.h"
#include "point.h"
#include "line.h"

namespace math {

    /// Line segment in 3D.
    class LineSegment3D
    {
    public:
        LineSegment3D() {}
        LineSegment3D(const LineSegment3D &linesegment) { *this = linesegment; }
        LineSegment3D(const Point3D &_pointA, const Point3D &_pointB): pointA(_pointA), pointB(_pointB) {}
        ~LineSegment3D() {}

        /**
         * @brief Classify one segment to another.
         * @param   linesegment The segment to classify against.
         * @return PARALLEL COINCIDENT COPLANAR or DISJOINT.
         */
        Classify ClassifyToLineSegment(const LineSegment3D &linesegment) const
        {
            Line3D line1(pointA, pointB);
            Line3D line2(linesegment.pointA, linesegment.pointB);
            return line1.ClassifyToLine(line2);
        }

        /// Length of the segment
        float Length(void)
        {
            return  Vector3D(pointA, pointB).Length();
        }

        /**
         * @brief Given a point on the segment, get the time t corresponding to the point.
         * @param   intersection_point  The point to find the parametric time t.
         * @return The time t used in the parametric equation to get @a intersection_point.
         * @remarks Undefined behavior if @a intersection_point is not on the line holding the
         * segment.
         */
        float GetIntersectionTime(const Point3D &intersection_point) const
        {
            Vector3D AB(pointA, pointB);
            return (intersection_point.x - pointA.x)/(AB.x);
        }

        /*returns the intersection point between the two linesegment if it exists*/
        /**
         * @brief Finds intersection point between 2 line segments if any.
         * @param   linesegment                 The segment to test againt.
         * @param   [out] intersection_point    The intersection point if any.
         * @return Boolean indicating if an intersection happened or not.
         */
        bool IntersectLineSegmentAt(const LineSegment3D &linesegment, Point3D &intersection_point) const
        {
            if (ClassifyToLineSegment(linesegment) != COPLANAR)
                return false;

            Line3D line1(pointA, pointB);
            Line3D line2(linesegment.pointA, linesegment.pointB);
            line1.IntersectLineAt(line2, intersection_point);

            float t0, t1;
            t0 = GetIntersectionTime(intersection_point);
            t1 = linesegment.GetIntersectionTime(intersection_point);

            // If t0 and t1 are between 0 and 1 then the point is valid.
            if ((t0 >= 0.f && t0 <= 1.f) && (t1 >= 0.f && t1 <= 1.f))
                return true;

            return false;
        }

        /**
         * @brief Finds the overlapping line segment when 2 segments are coincident.
         * @param   linesegment             The segment to test against.
         * @param   [out] overlapedsegment  The resulting overlapped segment.
         * @return Boolean indicating whether such overlapped segment exists or not.
         */
        bool GetOverlappedSegment(const LineSegment3D &linesegment, LineSegment3D &overlapedsegment) const
        {
            // We do not consider the segments if they are not coincident.
            if (ClassifyToLineSegment(linesegment) != COINCIDENT)
                return false;

            // For simplification.
            const LineSegment3D &AB = *this;
            const LineSegment3D &CD = linesegment;
            Vector3D vAB(AB.pointA, AB.pointB);
            Vector3D vCD(CD.pointA, CD.pointB);

            if (vAB.Length() > vCD.Length()) {
                float TC = AB.GetIntersectionTime(CD.pointA);
                float TD = AB.GetIntersectionTime(CD.pointB);

                // If both points belong to the segment.
                if ((TC >= 0.f && TC <= 1.f) && (TD >= 0.f && TD <= 1.f))
                    overlapedsegment = CD;
                else if ((TC >= 0.f && TC <= 1.f) && !(TD >= 0.f && TD <= 1.f)) {
                    // If only C belongs to AB.
                    float dotproduct = vAB.DotProduct(vCD);

                    // If AB and CD have the same direction, the overlapped segment is CB.
                    if (EQUALTO(dotproduct, 1.f, std::numeric_limits<float>::epsilon())) {
                        overlapedsegment.pointA = CD.pointA;
                        overlapedsegment.pointB = AB.pointB;
                    } else { // They have opposite direction, the overlapped segment is CA.
                        overlapedsegment.pointA = CD.pointA;
                        overlapedsegment.pointB = AB.pointA;
                    }
                } else if (!(TC >= 0.f && TC <= 1.f) && (TD >= 0.f && TD <= 1.f)) {
                    // If only D belongs to AB.
                    float dotproduct = vAB.DotProduct(vCD);

                    // If AB and CD have the same direction, the overlapped segment is AD.
                    if (EQUALTO(dotproduct, 1.f, std::numeric_limits<float>::epsilon())) {
                        overlapedsegment.pointA = AB.pointA;
                        overlapedsegment.pointB = CD.pointB;
                    } else { // They have opposite directions, the overlapped segment is BD.
                        overlapedsegment.pointA = AB.pointB;
                        overlapedsegment.pointB = CD.pointB;
                    }
                }
            } else {
                float TA = CD.GetIntersectionTime(AB.pointA);
                float TB = CD.GetIntersectionTime(AB.pointB);

                // If both points belong to the segment.
                if ((TA >= 0.f && TA <= 1.f) && (TB >= 0.f && TB <= 1.f))
                    overlapedsegment = AB;
                else if ((TA >= 0.f && TA <= 1.f) && !(TB >= 0.f && TB <= 1.f)) {
                    // If only A belongs to CD.
                    float dotproduct = vAB.DotProduct(vCD);

                    // if AB and CD have the same direction, the overlapped segment is AD.
                    if (EQUALTO(dotproduct, 1.f, std::numeric_limits<float>::epsilon())) {
                        overlapedsegment.pointA = AB.pointA;
                        overlapedsegment.pointB = CD.pointB;
                    } else { // They have opposite directions, the overlapped segment is AC.
                        overlapedsegment.pointA = AB.pointA;
                        overlapedsegment.pointB = CD.pointA;
                    }
                } else if (!(TA >= 0.f && TA <= 1.f) && (TB >= 0.f && TB <= 1.f)) {
                    // If only B belongs to CD.
                    float dotproduct = vAB.DotProduct(vCD);

                    // if AB and CD have the same direction, the overlapped segment is CB.
                    if (EQUALTO(dotproduct, 1.f, std::numeric_limits<float>::epsilon())) {
                        overlapedsegment.pointA = CD.pointA;
                        overlapedsegment.pointB = AB.pointB;
                    } else { // They have opposite directions, the overlapped segment is DB.
                        overlapedsegment.pointA = CD.pointB;
                        overlapedsegment.pointB = AB.pointB;
                    }
                }
            }

            return true;
        }

        /// Returns the angle in degree between the two segments.
        float AngleWithSegment(const LineSegment3D &linesegment) const
        {
            Line3D line1(pointA, pointB);
            Line3D line2(linesegment.pointA, linesegment.pointB);
            return line1.AngleWithLine(line2);
        }

        /// Returns the point at time t along the segment.
        Point3D GetPointAt(float time) const
        {
            Vector3D vec(this->pointA, this->pointB);
            return pointA + vec * time;
        }

    public:
        /// Points defining the segment.
        Point3D pointA, pointB;
    };
}

#endif
