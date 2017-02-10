/**
 * @file pipeline.h
 * @brief Holds transformations stack.
 */
#ifndef PIPELINE_H_INCLUDED
#define PIPELINE_H_INCLUDED

#include <cstdio>
#include <assert.h>
#include "matrix.h"

#define MODELVIEWSTACKSIZE 256
#define PROJECTIONSTACKSIZE 10

namespace core {

    /**
     * @brief Holds transformation stack, this insulates the client code from dealing with a
     * specific graphics API code.
     * @remarks Singleton class.
     * @todo Add support for texture stack.
     */
    class Pipeline
    {
    public:
        enum StackMode {
            MODELVIEW,
            PROJECTION
        };

        enum ProjectionType {
            ORTHOGRAPHIC,
            PERSPECTIVE
        };

        Pipeline(): modelview_index(0), projection_index(0), stack_mode(MODELVIEW), projection_type(PERSPECTIVE)
        {
            instance = this;
        }

        virtual ~Pipeline()
        {
            if (instance == this)
                instance = NULL;
        }

        /// Returns the current projection type.
        ProjectionType GetProjectionType(void) const
        {
            return projection_type;
        }

        /**
         * @brief Gets the viewport properties.
         * @param [out] x   The horizontal position of the viewport.
         * @param [out] y   The vertical position of the viewport.
         * @param [out] width   The width of the viewport.
         * @param [out] height  The height of the viewport.
         */
        void GetViewportInfo(float &x, float &y, float &width, float &height) const
        {
            x = viewport_x;
            y = viewport_y;
            width = viewport_width;
            height = viewport_height;
        }

        /**
         * @brief Sets the viewport dimensions and location on the screen.
         * @param	x	  	The starting x position on the screen.
         * @param	y	  	The starting y position on the screen (y axis is downward, a positive
         * 					will push the viewport down).
         * @param	width 	The width of the viewport (should match ratio of the frustum otherwise
         * 					distortion would occur).
         * @param	height	The height of the viewport (see above).
         */
        void SetViewport(float x, float y, float width, float height)
        {
            viewport_x = x;
            viewport_y = y;
            viewport_width = width;
            viewport_height = height;
        }

        /**
         * @brief Specify the frustum dimensions, this is used by the graphics api to create the
         * perspective projection matrix.
         * @param	right 	The right frustum clipping plane on the near plane.
         * @param	left  	The left frustum clipping plane on the near plane.
         * @param	bottom	The bottom frustum clipping plane on the near plane.
         * @param	top   	The top frustum clipping plane on the near plane.
         * @param	_near  	The near clipping plane distance (positive value).
         * @param	_far   	The far clipping plane distance (positive value).
         */
        void frustrum(float left, float right, float bottom, float top, float _near, float _far)
        {
            projection_type = PERSPECTIVE;
            frustum_left = left;
            frustum_right = right;
            frustum_bottom = bottom;
            frustum_top = top;
            frustum_near = _near;
            frustum_far = _far;
        }

        /**
         * @brief Specify the orthographic frustum dimensions, this is used by the graphics api to
         * create the orthographic projection matrix.
         * @param	right 	The right frustum clipping plane on the near plane.
         * @param	left  	The left frustum clipping plane on the near plane.
         * @param	bottom	The bottom frustum clipping plane on the near plane.
         * @param	top   	The top frustum clipping plane on the near plane.
         * @param	_near  	The near clipping plane distance (positive value).
         * @param	_far   	The far clipping plane distance (positive value).
         */
        void ortho(float left, float right, float bottom, float top, float _near, float _far)
        {
            projection_type = ORTHOGRAPHIC;
            frustum_left = left;
            frustum_right = right;
            frustum_bottom = bottom;
            frustum_top = top;
            frustum_near = _near;
            frustum_far = _far;
        }

        /// Returns the frustum data.
        void GetFrustumInfo(float &left, float &right, float &bottom, float &top, float &_near, float &_far) const
        {
            left = frustum_left;
            right = frustum_right;
            bottom = frustum_bottom;
            top = frustum_top;
            _near = frustum_near;
            _far = frustum_far;
        }

        /**
         * @brief Gets the matrix at the top of the current stack.
         * @param [out] output To be filled with the current stack top matrix.
         */
        void GetMatrix(math::Matrix4D &output) const
        {
            if (stack_mode == MODELVIEW)
                output = modelview_stack[modelview_index];
            else
                output = projection_stack[projection_index];
        }

        /**
         * @brief Sets which stack we are currently affecting.
         * @param mode The current stack mode.
         * @see StackMode
         */
        void SetMatrixMode(StackMode mode)
        {
            stack_mode = mode;
        }

        /**
         * @brief Duplicate the matrix at the top of the stack and push it on top. This effectively
         * makes the matrix on the top of the stack and the one just below it identical.
         */
        void PushMatrix(void)
        {
            if (stack_mode == MODELVIEW) {
                assert(modelview_index + 1 < MODELVIEWSTACKSIZE);
                modelview_stack[modelview_index + 1] = modelview_stack[modelview_index];
                ++modelview_index;
            } else {
                assert(projection_index + 1 < PROJECTIONSTACKSIZE);
                projection_stack[projection_index + 1] = projection_stack[projection_index];
                ++projection_index;
            }
        }

        /**
         * @brief Get the matrix on the top of the stack and remove it from the top.
         * @param [out] output The matrix that was at the top of the stack.
         */
        void PopMatrix(math::Matrix4D &output)
        {
            if (stack_mode == MODELVIEW) {
                assert(modelview_index > 0);
                output = modelview_stack[modelview_index--];
            } else {
                assert(projection_index > 0);
                output = projection_stack[projection_index--];
            }
        }

        /// @brief same as PopMatrix but discards the matrix.
        void PopMatrixEmpty(void)
        {
            if (stack_mode == MODELVIEW) {
                assert(modelview_index > 0);
                --modelview_index;
            } else {
                assert(projection_index > 0);
                --projection_index;
            }
        }

        /// @brief Loads the identity matrix at the top of the current stack.
        void LoadIdentity(void)
        {
            if (stack_mode == MODELVIEW)
                modelview_stack[modelview_index] = math::Matrix4D();
            else
                projection_stack[projection_index] = math::Matrix4D();
        }

        /// @brief Replaces the top matrix with the new matrix.
        void Replace(const math::Matrix4D &matrix)
        {
            if (stack_mode == MODELVIEW)
                modelview_stack[modelview_index] = matrix;
            else
                projection_stack[projection_index] = matrix;
        }

        /**
         * @brief Post multiply the current matrix at the top of the stack with the new @a matrix,
         * meaning the transformation that @a matrix represent will be applied after the
         * transformation represented by the matrix on top of the stack.
         * @param matrix The transformation to post apply to the top of the stack.
         */
        void PostMultiply(const math::Matrix4D &matrix)
        {
            if (stack_mode == MODELVIEW)
                modelview_stack[modelview_index] = matrix * modelview_stack[modelview_index];
            else
                projection_stack[projection_index] = matrix * projection_stack[projection_index];
        }

        void PostRotateX(float angle)
        {
            PostMultiply(math::Matrix4D::RotationX(angle));
        }

        void PostRotateY(float angle)
        {
            PostMultiply(math::Matrix4D::RotationY(angle));
        }

        void PostRotateZ(float angle)
        {
            PostMultiply(math::Matrix4D::RotationZ(angle));
        }

        void PostTranslate(float x, float y, float z)
        {
            PostMultiply(math::Matrix4D::Translation(x, y, z));
        }

        /**
         * @brief Pre-multiply the top of the stack matrix with @a matrix.
         * @param matrix The transformation to pre-apply to the top of the stack.
         * @see postMultiply.
         */
        void PreMultiply(const math::Matrix4D &matrix)
        {
            if (stack_mode == MODELVIEW)
                modelview_stack[modelview_index] = modelview_stack[modelview_index] * matrix;
            else
                projection_stack[projection_index] = projection_stack[projection_index] * matrix;
        }

        void PreRotateX(float angle)
        {
            PreMultiply(math::Matrix4D::RotationX(angle));
        }

        void PreRotateY(float angle)
        {
            PreMultiply(math::Matrix4D::RotationY(angle));
        }

        void PreRotateZ(float angle)
        {
            PreMultiply(math::Matrix4D::RotationZ(angle));
        }

        void PreTranslate(float x, float y, float z)
        {
            PreMultiply(math::Matrix4D::Translation(x, y, z));
        }

        /**
         * @brief Gets the current Pipeline reference.
         * @remarks No checks are done for the reference validity.
         */
        static Pipeline *GetCurrentPipeline(void)
        {
            return Pipeline::instance;
        }

        /// Set the current static instance.
        static Pipeline *SetCurrentPipeline(Pipeline *pipeline)
        {
            instance = pipeline;
        }

    private:
        StackMode stack_mode;

        math::Matrix4D modelview_stack[MODELVIEWSTACKSIZE];
        int modelview_index;

        math::Matrix4D projection_stack[PROJECTIONSTACKSIZE];
        int projection_index;

        ProjectionType projection_type;
        float frustum_left, frustum_right, frustum_top, frustum_bottom, frustum_near, frustum_far;
        float viewport_x, viewport_y, viewport_width, viewport_height;

        static Pipeline *instance;

        friend class Renderer;
    };
}

#endif // PIPELINE_H_INCLUDED
