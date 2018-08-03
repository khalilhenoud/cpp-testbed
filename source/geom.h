/**
 * @file geom.h
 * @brief Some common geometric functionality.
 */
#ifndef _GEOM_H
#define _GEOM_H

#include <limits>

#define PI			3.14159265f
#define EQUALTO(M, N, EPSILON)		(((M) <= ((N) + (EPSILON))) && ((M) >= ((N) - (EPSILON))))
#define ISZERO(M, EPSILON)			EQUALTO((M), 0, (EPSILON))

/**
 * @namespace math
 * @brief Contains all math functionality specific to the project.
 */
namespace math {

    /// Classification values for 2 geometric entities being compared.
    enum Classify {
        NEITHER,
        PARALLEL,
        COINCIDENT,
        PERPENDICULAR,
        DISJOINT,
        LEFT,
        RIGHT,
        BELONG,	///< To describe belonging to a line or ray or segment.
        ONLINE,	///< To describe belonging to the line holding the ray or the line segment.
        TANGENT,
        INTERSECT,
        C1INSIDEC2,
        C2INSIDEC1,
        OUTSIDE,
        INSIDE,
        ON,
        NOINTERSECT,
        INTERSECTONEPOINT,
        COPLANAR,
        FRONT,
        BACK
    };
}

#endif
