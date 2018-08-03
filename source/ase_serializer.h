/**
 * @file ase_serializer.h
 * @brief Reads scene data from ASE file.
 */
#ifndef ASE_SERIALIZER_H_INCLUDED
#define ASE_SERIALIZER_H_INCLUDED

#include "serializer.h"

namespace core {

    /**
     * @brief Loads scene data from an ASE file.
     * @remarks Currently missing from loading a mesh from an ASE files, are multi-sets of UV
     * coordinates, tangents and binormals, and multi-sets of materials.
     * @remarks Hierarchies are not yet implemented in an ASE file, will need to look into that.
     */
    class ASESerializer: public Serializer
    {
    public:
        ASESerializer() {}
        ~ASESerializer() {}

        /**
         * @brief Given a file path, it will open the file and read the scene content.
         * @param file_path The file path relative to the project directory.
         * @return A Model or NULL on failure.
         */
        virtual Model *LoadSceneFromFile(std::string file_path);

    private:
        /**
         * @brief Given the ASE file content as a string, parses the content for the scene.
         * @param file Content of the ASE file.
         * @return A model hierarchy if successful, otherwise NULL.
         */
        Model *ReadSceneFromFileContent(std::string file);

        /**
         * @brief Reads the material list from the file content.
         * @param file Content of the ASE file.
         * @return A vector containing all the material in the file.
         */
        std::vector<Material> ReadSceneMaterialListFromFileContent(std::string file);

        /**
         * @brief Given a string @a content, the function will search for @a label inside @a content
         * @param content The string to search within.
         * @param label The string to search for.
         * @param offset Offset within @a content to start looking from.
         * @return A string content the data specific to label or an empty string if @a label is not
         * found
         */
        std::string ReadChunk(std::string content, std::string label, unsigned int offset = 0);

        /**
         * @brief Read a color given by @a label within @a content.
         * @param label The label containing the color.
         * @param content The string to search within.
         * @return Returns the color component found in content or white color on failure.
         */
        Color ReadColorComponent(std::string label, std::string content);

        /**
         * @brief Reads the texture map information from content given by label.
         * @param label The label indicating the texture map data block start.
         * @param content The chunk of text where the info is searched for.
         * @return The texture map with the correct data or a default map if label does not exist.
         */
        TextureMap ReadTextureMap(std::string label, std::string content);
    };
}

#endif // ASE_SERIALIZER_H_INCLUDED
