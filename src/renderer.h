/**
 * @file renderer.h
 * @brief Renderer interface class.
 */
#ifndef RENDERER_H_INCLUDED
#define RENDERER_H_INCLUDED

#include "model.h"
#include "mesh.h"
#include "pipeline.h"

namespace core {

    /// Interface class for rendering.
    class Renderer
    {
    public:
        Renderer() {}
        virtual ~Renderer() {}

        /// Initializes the renderer.
        virtual void Initialize(void) = 0;
        /// Cleanup to be done before application shutdown.
        virtual void Cleanup(void) = 0;
        /// Loads a list of texture maps.
        virtual void LoadTextureMaps(std::vector<std::string> paths) = 0;
        /// Draw a 'Model'.
        virtual void DrawModel(const Model &model) const = 0;
        /// Draw a 'Mesh'
        virtual void DrawMesh(const Mesh &mesh) const = 0;
        /// Called before rendering starts.
        virtual void PreUpdate() = 0;
        /// Called after rendering has finished.
        virtual void PostUpdate(int frametime) = 0;

        /// Sets the viewport properties based on the data in the current pipeline.
        virtual void UpdateViewportProperties(const Pipeline *pipeline) = 0;
        /// Sets the projection frustum and type based on the data in the current pipeline.
        virtual void UpdateProjectionProperties(const Pipeline *pipeline) = 0;
    };
}

#endif // RENDERER_H_INCLUDED
