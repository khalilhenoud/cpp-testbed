/**
 * @file camera.h
 * @brief Holds the camera class.
 */
#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include "matrix.h"

namespace core {

    /**
     * @brief Camera class, used to create a view matrix.
     */
    class Camera
    {
    public:
        Camera(): position(math::Point3D(0, 0, 0)), lookatdirection(math::Vector3D(0, 0, -1)), upvector(math::Vector3D(0, 1, 0)) {}
        ~Camera() {}

        /**
         * @brief Function used to set the camera properties.
         * @param _position The position of the camera in world coordinates.
         * @param _lookatposition The location the camera is looking to in world coordinates.
         * @param _upvector The up axis along which the camera is oriented.
         */
        void LookAt(const math::Point3D &_position, const math::Point3D &_lookatposition, const math::Vector3D &_upvector)
        {
            lookatdirection = _lookatposition - _position;
            lookatdirection.Normalize();
            upvector = _upvector;
            upvector.Normalize();
            position = _position;
        }

        /**
         * @brief Returns the viewing transformation matrix.
         * @return A viewing matrix.
         */
        math::Matrix4D GetViewTransformation(void) const
        {
            math::Matrix4D translation = math::Matrix4D::Translation(-position.x, -position.y, -position.z);
            math::Matrix4D crossproduct = math::Matrix4D::CrossProduct(upvector);
            math::Vector3D tmp = -lookatdirection;
            math::Vector3D result = crossproduct * tmp;
            math::Matrix4D camerarotation;
            camerarotation.m00 = result.x;
            camerarotation.m01 = result.y;
            camerarotation.m02 = result.z;
            camerarotation.m10 = upvector.x;
            camerarotation.m11 = upvector.y;
            camerarotation.m12 = upvector.z;
            camerarotation.m20 = -lookatdirection.x;
            camerarotation.m21 = -lookatdirection.y;
            camerarotation.m22 = -lookatdirection.z;

            return camerarotation * translation;
        }

    public:
        math::Point3D position;
        math::Vector3D lookatdirection;
        math::Vector3D upvector;
    };
}

#endif // CAMERA_H_INCLUDED
