/**
 * @file mesh.h
 * @brief Contains geometric data including mesh, materials and textures.
 */
#ifndef MESH_H_INCLUDED
#define MESH_H_INCLUDED

#include <string>
#include <vector>

/**
 * @namespace core
 * @brief Holds all classes critical to the project.
 */
namespace core {

    #define MAX_UV_LAYERS 8

    /**
     * @brief Describes a texture.
     * @remarks Should support SRGB, it seems to be the new standard (Targa TGA files).
     * @remarks A usage counter in the renderer should be kept; a texture is distinct by its path.
     */
    class TextureMap
    {
    public:
        TextureMap(void): u_offset(0.f), v_offset(0.f), u_scale(0.f), v_scale(0.f), angle(0.f) {}

    public:
        /// Name of the texture map.
        std::string name;
        /// The path to the referenced texture file.
        std::string path;
        /// i.e. Diffuse, specular, etc... Or format.
        std::string type;
        /// Modify the texture matrix.
        float u_offset, v_offset;
        float u_scale, v_scale;
        float angle;
    };

    /// Color component.
    class Color
    {
    public:
        Color(void): r(0.f), g(0.f), b(0.f), a(1.f) {}
        ~Color(void) {}

    public:
        float r, g, b, a;
    };

    /**
     * @brief Holds the material applied to a mesh.
     * @remarks Should be extended to support a PBR model.
     */
    class Material
    {
    public:
        Material(void): shininess(0.0f), opacity(1.0f) {}

    public:
        std::string name;
        Color ambient;
        Color diffuse;
        Color specular;
        float shininess;
        float opacity;
        /// List of texture maps specified in the material.
        std::vector <TextureMap> textures;
    };

    /**
     * @brief The core mesh class, the smallest entity that can be rendered.
     * @remarks A vertex is duplicated when it has different normals specified or different UVs
     * (all layers), or different color values. That is why when you import a model in UE4 the
     * vertex number is different than in the CAD program.
     * @remarks Only triangles are supported currently.
     */
    class Mesh
    {
    public:
        /// Default constructor.
        Mesh(): vertices(NULL), normals(NULL), colors(NULL), is_using_colors(false), vertex_number(0), uv_layer_count(0),
                index_array(NULL), index_array_size(0) {}

        virtual ~Mesh()
        {
            if (vertices)
                delete [] vertices;
            vertices = NULL;

            if (normals)
                delete [] normals;
            normals = NULL;

            if (colors)
                delete [] colors;
            colors = NULL;

            // Freeing data associated with the uv layers.
            for (unsigned int i = 0; i < uv_layer_count; ++i) {
                delete [] tangents[i];
                delete [] binormals[i];
                delete [] uv_coordinates[i];
            }

            delete [] index_array;
            index_array = NULL;
        }

        /**
         * @brief Returns all textures used in the current model.
         * @return A vector containing all the textures paths.
         */
        std::vector<std::string> GetTexturesList(void)
        {
            std::vector<std::string> paths;
            for (unsigned int i = 0; i < materials.size(); ++i) {
                for (unsigned int j = 0; j < materials[i].textures.size(); ++j)
                    paths.push_back(materials[i].textures[i].path);
            }

            return paths;
        }

    public:
        std::string name;

        /**
         * @brief 'vertices', 'colors' and 'normals' count in specified in 'vertex_number'.
         * @remarks 'colors' can be used as control parameters for material blending or other shader
         * parameters.
         */
        float *vertices;
        float *normals;
        float *colors;
        bool is_using_colors;
        unsigned int vertex_number;

        /**
         * @brief Tangents, bi-normals and UV coordinates are grouped in arrays of arrays of floats.
         * @remarks 'uv_layer_counts' Holds the actual uv layer count, maximum is 'MAX_UV_LAYERS',
         * the length of each individual array is controlled by 'vertex_number'.
         */
        float *tangents[MAX_UV_LAYERS];
        float *binormals[MAX_UV_LAYERS];
        float *uv_coordinates[MAX_UV_LAYERS];
        unsigned int uv_layer_count;

        /// The indices that make up the polygons in the mesh.
        unsigned short *index_array;
        unsigned int index_array_size;

        /// Material vector.
        std::vector<Material> materials;
    };
}

#endif // MESH_H_INCLUDED
