/**
 * @file bbox.h
 * @brief Bounding box struct used in occlusion queries.
 */
#ifndef BBOX_H_INCLUDED
#define BBOX_H_INCLUDED

#include "point.h"

namespace math {

    /**
     * @brief Bounding box struct used in occlusion queries.
     * @remarks Should be moved to the renderer (or added to the math namespace).
     */
    struct BoundingBox
    {
        Point3D center;
        float maximumdistanceX;
        float maximumdistanceY;
        float maximumdistanceZ;
    };
}

#endif // BBOX_H_INCLUDED
