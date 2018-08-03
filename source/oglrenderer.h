/**
 * @file oglrenderer.h
 * @brief OpenGL basic renderer.
 */
#ifndef OGL_RENDERER_H_INCLUDED
#define OGL_RENDERER_H_INCLUDED

#include <map>
#include "renderer.h"

namespace core {

    /**
     * @brief An extremely basic OpenGL renderer intended as a starting point for something bigger.
     */
    class OGLRenderer: public Renderer
    {
    public:
        OGLRenderer() {}
        virtual ~OGLRenderer() {}

        /// Initializes the renderer.
        virtual void Initialize(void);
        /// Cleanup to be done before application shutdown.
        virtual void Cleanup(void);

        /**
         * @brief Loads a series of textures.
         * @param paths An array containing the paths to all the textures.
         */
        virtual void LoadTextureMaps(std::vector<std::string> paths);

        /// Draw a 'Model'.
        virtual void DrawModel(const Model &model) const;

        /**
         * @brief Draws a mesh.
         * @param mesh The mesh to be drawn.
         * @remarks Only support rudimentary drawing of meshes (1 texture, 1 material, etc...).
         */
        virtual void DrawMesh(const Mesh &mesh) const;

        /// Called before rendering starts.
        virtual void PreUpdate();
        /// Called after rendering has finished.
        virtual void PostUpdate(int frametime);

        /// Sets the viewport properties based on the data in the current pipeline.
        virtual void UpdateViewportProperties(const Pipeline *pipeline);

        /// Sets the projection frustum and type based on the data in the current pipeline.
        virtual void UpdateProjectionProperties(const Pipeline *pipeline);

        /// Draws a 3D grid to help with navigation.
        void DrawGrid(void) const;

    private:
        /**
         * @brief Loads a texture map given the file path into VRAM.
         * @param path The path to the image file.
         * @return Boolean indicating if loading was successful or not.
         * @todo Should have more granular control of whether we want to load to disk or directly
         * to VRAM.
         */
        virtual bool LoadTextureMap(std::string path);

    private:
        /// Holds the textures id list.
        std::map<std::string, unsigned int> textures;
    };
}

#endif // OGL_RENDERER_H_INCLUDED
