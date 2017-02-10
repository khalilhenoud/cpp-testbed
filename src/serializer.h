/**
 * @file serializer.h
 * @brief Contains interface class for serializing/de-serializing models.
 */
#ifndef SERIALIZER_H_INCLUDED
#define SERIALIZER_H_INCLUDED

#include "model.h"
#include "mesh.h"
#include <string>

namespace core {

    /**
     * @brief Serializer class, responsible for reading and writing entities to and from data files.
     * @todo To be extended to be able to parse specific types of entities (i.e. materials, meshes)
     * from files and to target a specific entity in the file.
     */
    class Serializer
    {
    public:
        Serializer() {}
        virtual ~Serializer() {}

        /// Loads a scene from a file (pure virtual).
        virtual Model *LoadSceneFromFile(std::string file_path) = 0;

        /// Write a scene to a file.
        virtual void WriteSceneToFile(const Model *scene) const {}
    };
}

#endif // SERIALIZER_H_INCLUDED
