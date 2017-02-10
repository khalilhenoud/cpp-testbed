/**
 * @file binary_serializer.h
 * @brief Contains binary format serializer.
 */
#ifndef BINARY_SERIALIZER_H_INCLUDED
#define BINARY_SERIALIZER_H_INCLUDED

#include "serializer.h"

namespace core {

    /**
     * @brief Reads and serializes a scene to a binary format.
     * @todo To be implemented later on.
     */
    class BinarySerializer: public Serializer
    {
    public:
        BinarySerializer() {}
        ~BinarySerializer() {}

        /// Loads a scene from a binary format file.
        virtual Model *LoadSceneFromFile(std::string file_path);

        /// Write a scene to a file in binary format.
        virtual void WriteSceneToFile(const Model *scene) const;
    };
}

#endif // BINARY_SERIALIZER_H_INCLUDED
