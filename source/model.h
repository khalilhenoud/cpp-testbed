/**
 * @file model.h
 * @brief Holds the model class.
 */
#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED

#include "mesh.h"
#include <string>
#include <vector>

namespace core {

    /**
     * @brief Holds the model class, which for example, could parent and render a mesh object or
     * another model object.
     * @remarks Transformation data should be added to this class, including translation, rotation
     * and scale transformations.
     */
    class Model
    {
    public:
        Model(): release_meshes_on_destroy(true), release_models_on_destroy(true) {}

        /**
         * @brief Destroys the meshes and sub models associated with the model, if the appropriate
         * release flags indicate as such, otherwise just remove the references.
         */
        virtual ~Model()
        {
            unsigned int i;
            if (release_meshes_on_destroy) {
                for (i = 0; i < meshes.size(); ++i)
                    delete meshes[i];
            }
            meshes.clear();

            if (release_models_on_destroy) {
                for (i = 0; i < sub_models.size(); ++i)
                    delete sub_models[i];
            }
            sub_models.clear();
        }

        /**
         * @brief Returns all textures used in the current model.
         * @return A vector containing all the textures paths.
         */
        std::vector<std::string> GetTexturesList(void)
        {
            std::vector<std::string> paths;
            std::vector<std::string> tmp;

            for (unsigned int i = 0; i < meshes.size(); ++i) {
                tmp = meshes[i]->GetTexturesList();
                paths.insert(paths.end(), tmp.begin(), tmp.end());
            }

            for (unsigned int i = 0; i < sub_models.size(); ++i) {
                tmp = sub_models[i]->GetTexturesList();
                paths.insert(paths.end(), tmp.begin(), tmp.end());
            }

            return paths;
        }

    public:
        std::string name;

        std::vector<Mesh *> meshes;
        bool release_meshes_on_destroy;

        std::vector<Model *> sub_models;
        bool release_models_on_destroy;
    };
}

#endif // MODEL_H_INCLUDED
