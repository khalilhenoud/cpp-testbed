#include <iostream>
#include <fstream>
#include <windows.h>
#include <cstdlib>
#include <Shlwapi.h>
#include "ase_serializer.h"
#include "gvector.h"

using namespace math;

#ifndef _MSC_VER
    #define _itoa_s(A, B, C) _itoa(A, B, C)
#endif

// Temporary class used to parse the ASE file content.
class Intermediate_Face
{
public:
    /**
     * @brief Given a list of faces with the normals, uvs and indices, this function calculates the
     * tangents and binormals.
     * @param vertices The vertices used by the faces.
     * @param vertices_number The number of vertices in the list.
     * @param [in, out] faces The list of faces to calculate the normal and binormals.
     * @param faces_number The number of faces in the list.
     * @remarks This function does not take into consideration the smoothing groups, we need to
     * modify it to take that into consideration, also material ID per face should be taken into
     * consideration.
     */
    static void CalculateNormalsTangentsAndBinormals(Point3D *vertices, int vertices_number, Intermediate_Face *faces, int faces_number);

public:
	// Indices.
	unsigned int v0, v1, v2;
	Vector3D face_normal;
	Vector3D face_tangent;
	Vector3D face_binormal;
	// Normals, Tangents, Bi-normals per vertex.
	Vector3D v0_normal, v1_normal, v2_normal;
	Vector3D v0_tangent, v1_tangent, v2_tangent;
	Vector3D v0_binormal, v1_binormal, v2_binormal;
	// UV coordinates per vertex.
	Vector3D v0_texture, v1_texture, v2_texture;
};

void Intermediate_Face::CalculateNormalsTangentsAndBinormals(Point3D *vertices, int vertices_number, Intermediate_Face *faces, int faces_number)
{
	std::vector<Vector3D> verticesnormals;
	std::vector<Vector3D> verticestangents;
	std::vector<Vector3D> verticesbinormals;

	// Calculating the tangent and binormal per face.
	for (int i = 0; i < faces_number; ++i) {
		Intermediate_Face &face = faces[i];
		Vector3D v0v1 = vertices[face.v1] - vertices[face.v0];
		Vector3D v0v2 = vertices[face.v2] - vertices[face.v0];
		Vector3D c0c1 = face.v1_texture - face.v0_texture;
		Vector3D c0c2 = face.v2_texture - face.v0_texture;
		float M = c0c1.x * c0c2.y - c0c2.x * c0c1.y;
		Vector3D T = (v0v1 * c0c2.y) - (v0v2 * c0c1.y);
		Vector3D B = (v0v2 * c0c1.x) - (v0v1 * c0c2.x);
		T = T * (1.f/M);
		T.Normalize();
		B = B * (1.f/M);
		B.Normalize();
		// See "http://stackoverflow.com/questions/5255806/how-to-calculate-tangent-and-binormal".
		face.face_tangent	= T;
		face.face_binormal	= B;
	}

	// Averaging the normal, tangent and bi-normal for each vertex.
	std::vector<Vector3D> sharednormals;
    std::vector<Vector3D> sharedtangents;
    std::vector<Vector3D> sharedbinormals;
	for (int i = 0; i < vertices_number; ++i) {
		sharednormals.clear();
		sharedtangents.clear();
		sharedbinormals.clear();

		for (int faceindex = 0; faceindex < faces_number; ++faceindex) {
			// If any of the vertices is the same as the current vertex.
			if ((vertices[faces[faceindex].v0] == vertices[i]) ||
                (vertices[faces[faceindex].v1] == vertices[i]) ||
                (vertices[faces[faceindex].v2] == vertices[i])) {

                // Check if the normal was already added.
				bool found = false;
				for (unsigned int sn = 0; sn != sharednormals.size(); ++sn) {
					if (sharednormals[sn] == faces[faceindex].face_normal) {
						found = true;
						break;
					}
				}

				// Add the normal in case it wasn't added.
				if (!found)
                    sharednormals.push_back(faces[faceindex].face_normal);

                // Do the same for the tangents.
				found = false;
				for (unsigned int sn = 0; sn != sharedtangents.size(); ++sn) {
					if (sharedtangents[sn] == faces[faceindex].face_tangent) {
						found = true;
						break;
					}
				}

				if (!found)
                    sharedtangents.push_back(faces[faceindex].face_tangent);

				// Do the same for the binormals.
				found = false;
				for (unsigned int sn = 0; sn != sharedbinormals.size(); ++sn) {
					if (sharedbinormals[sn] == faces[faceindex].face_binormal) {
						found = true;
						break;
					}
				}

				if (!found)
                    sharedbinormals.push_back(faces[faceindex].face_binormal);
			}
		}

		// Averaging the normals.
		Vector3D averagenormal(0, 0, 0);
		for (unsigned int sn = 0; sn != sharednormals.size(); ++sn) {
			averagenormal.x += sharednormals[sn].x;
			averagenormal.y += sharednormals[sn].y;
			averagenormal.z += sharednormals[sn].z;
		}

		averagenormal.x /= sharednormals.size();
		averagenormal.y /= sharednormals.size();
		averagenormal.z /= sharednormals.size();
		verticesnormals.push_back(averagenormal);

		// Averaging the tangents.
		Vector3D averagetangent(0, 0, 0);
		for (unsigned int sn = 0; sn != sharedtangents.size(); ++sn) {
			averagetangent.x += sharedtangents[sn].x;
			averagetangent.y += sharedtangents[sn].y;
			averagetangent.z += sharedtangents[sn].z;
		}

		averagetangent.x /= sharedtangents.size();
		averagetangent.y /= sharedtangents.size();
		averagetangent.z /= sharedtangents.size();
		verticestangents.push_back(averagetangent);

		// Averaging the bi-normals.
		Vector3D averagebinormal(0, 0, 0);
		for (unsigned int sn = 0; sn != sharedbinormals.size(); ++sn) {
			averagebinormal.x += sharedbinormals[sn].x;
			averagebinormal.y += sharedbinormals[sn].y;
			averagebinormal.z += sharedbinormals[sn].z;
		}

		averagebinormal.x /= sharedbinormals.size();
		averagebinormal.y /= sharedbinormals.size();
		averagebinormal.z /= sharedbinormals.size();
		verticesbinormals.push_back(averagebinormal);
	}

	// Setting the normal, tangent and binormal per face.
	for (int i = 0; i < vertices_number; ++i) {
		for (int faceindex = 0; faceindex < faces_number; ++faceindex) {
			// If any of the vertices is the same as the current one.
			if (vertices[faces[faceindex].v0] == vertices[i]) {
				faces[faceindex].v0_normal = verticesnormals[i];
				faces[faceindex].v0_tangent = verticestangents[i];
				faces[faceindex].v0_binormal = verticesbinormals[i];
			}

			if (vertices[faces[faceindex].v1] == vertices[i]) {
				faces[faceindex].v1_normal = verticesnormals[i];
				faces[faceindex].v1_tangent = verticestangents[i];
				faces[faceindex].v1_binormal = verticesbinormals[i];
			}

			if (vertices[faces[faceindex].v2] == vertices[i]) {
				faces[faceindex].v2_normal = verticesnormals[i];
				faces[faceindex].v2_tangent = verticestangents[i];
				faces[faceindex].v2_binormal = verticesbinormals[i];
			}
		}
	}
}

class Intermediate_Mesh
{
public:
    // Constructors.
	Intermediate_Mesh(void): vertices(NULL), vertices_number(0), faces(NULL), faces_number(0) {}
	Intermediate_Mesh(
                   std::string _name,
                   unsigned int _vertices_number, Point3D *_vertices,
                   unsigned int _faces_number, Intermediate_Face *_faces,
                   core::Material _material):
                       name(_name),
                       vertices_number(_vertices_number), vertices(_vertices),
                       faces_number(_faces_number), faces(_faces),
                       material(_material) {}

    ~Intermediate_Mesh(void)
    {
        if (vertices_number && vertices)
            delete [] vertices;
        vertices = NULL;

        if (faces_number && faces)
            delete [] faces;
        faces = NULL;
    }

    /**
     * @brief Converts an intermediate mesh to a final mesh independent of file format.
     * @return A mesh.
     */
    core::Mesh *ConvertToMesh()
    {
        core::Mesh *targetmodel = new core::Mesh();
        targetmodel->name = this->name;
        targetmodel->vertex_number = this->vertices_number;
        targetmodel->vertices = new float[this->vertices_number * 4];
        targetmodel->normals = new float[this->vertices_number * 3];
        // Setting the tangents, binormals and uvs.
        targetmodel->uv_layer_count = 1;
        targetmodel->tangents[0] = new float[this->vertices_number * 3];
        targetmodel->binormals[0] = new float[this->vertices_number * 3];
        targetmodel->uv_coordinates[0] = new float[this->vertices_number * 3];
        targetmodel->materials.push_back(this->material);

        // Deep copy of the vertex array.
        for (unsigned int i = 0; i < this->vertices_number; ++i) {
            targetmodel->vertices[i * 4 + 0] = this->vertices[i].x;
            targetmodel->vertices[i * 4 + 1] = this->vertices[i].y;
            targetmodel->vertices[i * 4 + 2] = this->vertices[i].z;
            targetmodel->vertices[i * 4 + 3] = this->vertices[i].w;
        }

        // Filling the normals, tangents, binormals and UVCoordinates.
        for (unsigned int vertexindex = 0; vertexindex < this->vertices_number; ++vertexindex) {
            for (unsigned int faceindex = 0; faceindex < this->faces_number; ++faceindex) {
                Intermediate_Face face = this->faces[faceindex];

                if (face.v0 == vertexindex || face.v1 == vertexindex || face.v2 == vertexindex) {
                    Vector3D normal;
                    Vector3D tangent;
                    Vector3D binormal;
                    Vector3D uvcoordinates;

                    if (face.v0 == vertexindex) {
                        normal = face.v0_normal;
                        tangent = face.v0_tangent;
                        binormal = face.v0_binormal;
                        uvcoordinates = face.v0_texture;
                    } else if (face.v1 == vertexindex) {
                        normal = face.v1_normal;
                        tangent = face.v1_tangent;
                        binormal = face.v1_binormal;
                        uvcoordinates = face.v1_texture;
                    } else if(face.v2 == vertexindex) {
                        normal = face.v2_normal;
                        tangent = face.v2_tangent;
                        binormal = face.v2_binormal;
                        uvcoordinates = face.v2_texture;
                    }

                    // Filling the normal information.
                    targetmodel->normals[vertexindex * 3 + 0] = normal.x;
                    targetmodel->normals[vertexindex * 3 + 1] = normal.y;
                    targetmodel->normals[vertexindex * 3 + 2] = normal.z;
                    // Filling the tangent information.
                    targetmodel->tangents[0][vertexindex * 3 + 0] = tangent.x;
                    targetmodel->tangents[0][vertexindex * 3 + 1] = tangent.y;
                    targetmodel->tangents[0][vertexindex * 3 + 2] = tangent.z;
                    // Filling the bi-normal information.
                    targetmodel->binormals[0][vertexindex * 3 + 0] = binormal.x;
                    targetmodel->binormals[0][vertexindex * 3 + 1] = binormal.y;
                    targetmodel->binormals[0][vertexindex * 3 + 2] = binormal.z;
                    // Filling the texture information.
                    targetmodel->uv_coordinates[0][vertexindex * 3 + 0] = uvcoordinates.x;
                    targetmodel->uv_coordinates[0][vertexindex * 3 + 1] = uvcoordinates.y;
                    targetmodel->uv_coordinates[0][vertexindex * 3 + 2] = uvcoordinates.z;
                    break;
                }
            }
        }

        // Filling the index array.
        targetmodel->index_array_size = this->faces_number * 3;
        targetmodel->index_array = new unsigned short[this->faces_number * 3];
        for (unsigned int i = 0; i < this->faces_number; ++i) {
            targetmodel->index_array[i * 3 + 0] = this->faces[i].v0;
            targetmodel->index_array[i * 3 + 1] = this->faces[i].v1;
            targetmodel->index_array[i * 3 + 2] = this->faces[i].v2;
        }

        return targetmodel;
    }

public:
	std::string name;
	Point3D *vertices;
	unsigned int vertices_number;
	Intermediate_Face *faces;
	unsigned int faces_number;
	core::Material material;
};

core::Model *core::ASESerializer::LoadSceneFromFile(std::string file_path)
{
    char buffer[1000];
	memset(buffer, 0, 1000);

	// Get the current directory string and add to it the file path.
	GetCurrentDirectory(1000, (LPSTR)buffer);
	std::string directory = buffer;
	directory += "\\" + file_path;

	// Check if the file exists.
	if (!PathFileExists(directory.c_str()))
		return NULL;

	// Open and read the file content.
	std::ifstream file;
	file.open(directory.c_str(), std::ios_base::in);
	if (!file.is_open())
		return NULL;

    // Find the file content length.
	file.seekg(0, std::ios_base::end);
	unsigned int filesize = 0;
	filesize = (unsigned int)file.tellg();

	// Go back tot he beginning and read the content.
	file.seekg(0, std::ios_base::beg);
	char *array = new char[filesize + 1];
	memset(array, 0, filesize + 1);
	file.read(array, filesize);

	return ReadSceneFromFileContent(std::string(array));
}

std::string core::ASESerializer::ReadChunk(std::string content, std::string label, unsigned int offset)
{
	std::basic_string<char>::size_type index = 0;
	index = content.find(label, offset);

	// If it isn't found return empty string.
	if (index == std::basic_string<char>::npos)
        return std::string();

    // Find the first opening bracket after 'label'.
	int count = 1;
	index = content.find_first_of('{', index);

	// If not found also return empty string.
	if (index == std::basic_string<char>::npos)
        return std::string();

    // Now count until you find the closing bracket matching the first opened one.
	char ch;
	std::basic_string<char>::size_type current_index = index;
	while (count) {
        ++current_index;
		ch = content[current_index];

		if (ch == '{')
            ++count;
		if (ch == '}')
            --count;
	}

	return content.substr(index + 1, current_index - index - 1);
}

core::Color core::ASESerializer::ReadColorComponent(std::string label, std::string content)
{
	core::Color white;
	white.r = white.g = white.b = 1.f;

	core::Color color;
	color.a = 1.f;

	std::basic_string<char>::size_type index = 0;
	index = content.find(label, 0);

	// If the label wasn't found return white.
	if (index == std::basic_string<char>::npos)
        return white;

	index += strlen(label.c_str()) + 1;

	// Read the red component.
	color.r = (float)atof(content.substr(index, content.find_first_of("\t", index) - index).c_str());
	index = content.find_first_of("\t", index) + 1;

	// Read the green component.
	color.g = (float)atof(content.substr(index, content.find_first_of("\t", index) - index).c_str());
	index = content.find_first_of("\t", index) + 1;

	// Read the blue component.
	color.b = (float)atof(content.substr(index, content.find_first_of("\n", index) - index).c_str());
	return color;
}

core::TextureMap core::ASESerializer::ReadTextureMap(std::string label, std::string content)
{
    core::TextureMap dummy_texture;
	core::TextureMap map;

	std::basic_string<char>::size_type index = 0;
	index = content.find(label, 0);

	// If the label does not exist.
	if (index == std::basic_string<char>::npos)
        return dummy_texture;

    // Read the chunk indicated by label, if it doesn't exist return dummy map.
	std::string chunk = ReadChunk(content, label);
	if (chunk.size() == 0)
        return dummy_texture;

	// Read the name of the texture map.
	index = chunk.find("*MAP_NAME", 0);
	index += strlen("*MAP_NAME");
	index = chunk.find("\"", index) + 1;
	map.name = chunk.substr(index, chunk.find("\"", index) - index);

	// Reading the texture type.
	index = chunk.find("*MAP_CLASS", 0);
	index += strlen("*MAP_CLASS");
	index = chunk.find("\"", index) + 1;
	map.type = chunk.substr(index, chunk.find("\"", index) - index);

	// Read the texture path.
	index = chunk.find("*BITMAP", 0);
	index += strlen("*BITMAP");
	index = chunk.find("\"", index) + 1;
	map.path = chunk.substr(index, chunk.find("\"", index) - index);

	// Read the U offset.
	index = chunk.find("*UVW_U_OFFSET", 0);
	index += strlen("*UVW_U_OFFSET") + 1;
	map.u_offset = (float)atof(chunk.substr(index, chunk.find("\n", index) - index).c_str());

	// Read the V offset.
	index = chunk.find("*UVW_V_OFFSET", 0);
	index += strlen("*UVW_V_OFFSET") + 1;
	map.v_offset = (float)atof(chunk.substr(index, chunk.find("\n", index) - index).c_str());

	// Read the U scale.
	index = chunk.find("*UVW_U_TILING", 0);
	index += strlen("*UVW_U_TILING") + 1;
	map.u_scale = (float)atof(chunk.substr(index, chunk.find("\n", index) - index).c_str());

	// Read the V scale.
	index = chunk.find("*UVW_V_TILING", 0);
	index += strlen("*UVW_V_TILING") + 1;
	map.v_scale = (float)atof(chunk.substr(index, chunk.find("\n", index) - index).c_str());

	// Read the angle.
	index = chunk.find("*UVW_ANGLE", 0);
	index += strlen("*UVW_ANGLE") + 1;
	map.angle = (float)atof(chunk.substr(index, chunk.find("\n", index) - index).c_str());

	return map;
}

std::vector<core::Material> core::ASESerializer::ReadSceneMaterialListFromFileContent(std::string file)
{
    std::vector<core::Material> materiallist;
    std::string text;

    // Read the material chunk into text.
	text = ReadChunk(file, "*MATERIAL_LIST");

	// Finding the number of materials in the file.
	std::basic_string<char>::size_type index = 0;
	index = text.find("*MATERIAL_COUNT", 0);
	index += strlen("*MATERIAL_COUNT");
	// Reading the material count.
	int materialnumber = atoi(text.substr(index, text.find_first_of("\n", index) - index).c_str());

	// Reading the materials.
	for (int i = 0; i < materialnumber; ++i) {
		std::string str;
		char number[100] = {0};
		_itoa_s(i, number, 10);

		std::string label = "*MATERIAL ";
		label += number;
        core::Material material;

		// Reading the chunk of the corresponding material into str.
		str = ReadChunk(text, label);
		// Get the material name.
		index = str.find("*MATERIAL_NAME", 0);
		index += strlen("*MATERIAL_NAME");
		index = str.find_first_of("\"", index);

		// Get the name, ambient diffuse and specular color.
		material.name = str.substr(index + 1, str.find_first_of("\"", index + 1) - index - 1);
		material.ambient = ReadColorComponent("*MATERIAL_AMBIENT", str);
		material.diffuse = ReadColorComponent("*MATERIAL_DIFFUSE", str);
		material.specular = ReadColorComponent("*MATERIAL_SPECULAR", str);

		// Reading the shininess and transparency.
		index = str.find("*MATERIAL_SHINESTRENGTH", 0);
		index += strlen("*MATERIAL_SHINESTRENGTH") + 1;
		material.shininess = (float)atof(str.substr(index, str.find_first_of("\n", index) - index).c_str());
		index = str.find("*MATERIAL_TRANSPARENCY", 0);
		index += strlen("*MATERIAL_TRANSPARENCY") + 1;
		material.opacity = 1.f - (float)atof(str.substr(index, str.find_first_of("\n", index) - index).c_str());

		// Diffuse, opacity and bump map.
		material.textures.push_back(ReadTextureMap("*MAP_DIFFUSE", str));
		material.textures.push_back(ReadTextureMap("*MAP_OPACITY", str));
		material.textures.push_back(ReadTextureMap("*MAP_BUMP", str));

		// Add the material to the list.
		materiallist.push_back(material);
	}

	return materiallist;
}

core::Model *core::ASESerializer::ReadSceneFromFileContent(std::string file)
{
    core::Model *scene = new core::Model();
    std::vector<core::Material> materiallist = ReadSceneMaterialListFromFileContent(file);
	std::string text, subtext;

	// Reading the models.
	std::basic_string<char>::size_type geomobject_index = 0;
	geomobject_index = file.find("*GEOMOBJECT", 0);

	// Iterate till there are no more models in the file.
	while (geomobject_index != std::basic_string<char>::npos) {
        // Will hold all intermediary content.
		int vertices_number = 0;
		Point3D *vertices = NULL;
		int faces_number = 0;
		Intermediate_Face *faces = NULL;
		Intermediate_Mesh *mesh = NULL;
		core::Model *model = NULL;

		// Chunk containing the current model.
		text = ReadChunk(file, "*GEOMOBJECT", (unsigned int)geomobject_index);
		geomobject_index += strlen("*GEOMOBJECT") + 1;

        // Creating the mesh and the model to hold it.
        model = new core::Model();
		mesh = new Intermediate_Mesh();

		// Get the name of the model (assign the same name to mesh appended with "_mesh").
		int index = text.find("*NODE_NAME", 0);
		index += strlen("*NODE_NAME");
		index = text.find("\"", index);
		index += 1;
		model->name = text.substr(index, text.find_first_of("\"", index) - index);
		mesh->name = model->name + "_mesh";

		// Read the vertices number, the face number.
		index = text.find("*MESH_NUMVERTEX", 0);
		index += strlen("*MESH_NUMVERTEX");
		vertices_number = atoi(text.substr(index, text.find("\n", index)).c_str());
		vertices = new Point3D[vertices_number];

		index = text.find("*MESH_NUMFACES", 0);
		index += strlen("*MESH_NUMFACES");
		faces_number = atoi(text.substr(index, text.find("\n", index)).c_str());
		faces = new Intermediate_Face[faces_number];

		// Reading the vertices.
		subtext = ReadChunk(text, "*MESH_VERTEX_LIST");
		std::basic_string<char>::size_type subindex = 0;

		for (int i = 0; i < vertices_number; ++i) {
			subindex = subtext.find("*MESH_VERTEX", subindex);
			subindex += strlen("*MESH_VERTEX");
			char array[30] = {0};
			_itoa_s(i, array, 10);
			subindex = subtext.find(array, subindex);
			subindex = subtext.find_first_of("\t", subindex) + 1;
			vertices[i].x = (float)atof(subtext.substr(subindex, subtext.find_first_of("\t",subindex) - subindex).c_str());
			subindex = subtext.find_first_of("\t", subindex) + 1;
			vertices[i].y = (float)atof(subtext.substr(subindex, subtext.find_first_of("\t",subindex) - subindex).c_str());
			subindex = subtext.find_first_of("\t", subindex) + 1;
			vertices[i].z = (float)atof(subtext.substr(subindex, subtext.find_first_of("\t\n ",subindex) - subindex).c_str());
		}

		// Read the faces.
		subtext = ReadChunk(text, "*MESH_FACE_LIST");
		subindex = 0;

		for (int i = 0; i < faces_number; ++i) {
			subindex = subtext.find("*MESH_FACE", subindex);
			subindex += strlen("*MESH_FACE");
            char array[30] = {0};
			_itoa_s(i, array, 10);
			std::string tag = array;
			tag += ":";
			subindex = subtext.find(tag, subindex);
			subindex += tag.size();

			// Reading the first tag.
			tag = "A:";
			subindex = subtext.find(tag, subindex);
			subindex += tag.size();
			subindex = subtext.find_first_not_of(" \t", subindex);
			faces[i].v0 = atoi(subtext.substr(subindex, subtext.find_first_of(" \t", subindex) - subindex).c_str());
			subindex = subtext.find_first_of(" \t", subindex);
			// Reading the second tag.
			tag = "B:";
			subindex = subtext.find(tag, subindex);
			subindex += tag.size();
			subindex = subtext.find_first_not_of(" \t", subindex);
			faces[i].v1 = atoi(subtext.substr(subindex, subtext.find_first_of(" \t", subindex) - subindex).c_str());
			subindex = subtext.find_first_of(" \t", subindex);
			// Reading the third tag.
			tag = "C:";
			subindex = subtext.find(tag, subindex);
			subindex += tag.size();
			subindex = subtext.find_first_not_of(" \t", subindex);
			faces[i].v2 = atoi(subtext.substr(subindex, subtext.find_first_of(" \t", subindex) - subindex).c_str());
			subindex = subtext.find_first_of(" \t", subindex);

			subindex = subtext.find_first_of("\n", subindex);
		}

		// Reading the number of texture vertices.
		index = text.find("*MESH_NUMTVERTEX", index);
		index += strlen("*MESH_NUMTVERTEX") + 1;
		int number_tex_vertices = atoi(text.substr(index, text.find_first_of("\n", index) - index).c_str());
		Vector3D *tvertices = new Vector3D[number_tex_vertices];
		subtext = ReadChunk(text, "*MESH_TVERTLIST");
		subindex = 0;

		for (int i = 0; i < number_tex_vertices; ++i) {
			subindex = subtext.find("*MESH_TVERT", subindex);
			subindex += strlen("*MESH_TVERT");
			char array[30] = {0};
			_itoa_s(i, array, 10);
			subindex = subtext.find(array, subindex);
			subindex += strlen(array);
			subindex = subtext.find_first_of("\t", subindex) + 1;
			tvertices[i].x = (float)atof(subtext.substr(subindex, subtext.find_first_of("\t", subindex) - subindex).c_str());
			subindex = subtext.find_first_of("\t", subindex) + 1;
			tvertices[i].y = (float)atof(subtext.substr(subindex, subtext.find_first_of("\t", subindex) - subindex).c_str());
			subindex = subtext.find_first_of("\n", subindex);
		}

		// Reading the texture coordinates into the faces.
		subtext = ReadChunk(text, "*MESH_TFACELIST");
		subindex = 0;

		for (int i = 0; i < faces_number; ++i) {
			subindex = subtext.find("*MESH_TFACE", subindex);
			subindex += strlen("*MESH_TFACE");
			char array[30] = {0};
			_itoa_s(i, array, 10);
			subindex = subtext.find(array, subindex);
			subindex += strlen(array);
			subindex = subtext.find_first_of("\t", subindex) + 1;
			int m = atoi(subtext.substr(subindex, subtext.find_first_of("\t", subindex) - subindex).c_str());
			Vector3D v0_texture = tvertices[m];
			subindex = subtext.find_first_of("\t", subindex) + 1;
			m = atoi(subtext.substr(subindex, subtext.find_first_of("\t", subindex) - subindex).c_str());
			Vector3D v1_texture = tvertices[m];
			subindex = subtext.find_first_of("\t", subindex) + 1;
			m = atoi(subtext.substr(subindex, subtext.find_first_of("\n", subindex) - subindex).c_str());
			Vector3D v2_texture = tvertices[m];
			subindex = subtext.find_first_of("\n", subindex);
			faces[i].v0_texture = v0_texture;
			faces[i].v1_texture = v1_texture;
			faces[i].v2_texture = v2_texture;
		}

		// Reading the normal of each face.
		subtext = ReadChunk(text, "*MESH_NORMALS");
		subindex = 0;

		for (int i = 0; i < faces_number; ++i) {
			Vector3D m;
			subindex = subtext.find("*MESH_FACENORMAL", subindex);
			subindex += strlen("*MESH_FACENORMAL");
			char array[30] = {0};
			_itoa_s(i, array, 10);
			subindex = subtext.find(array, subindex);
			subindex += strlen(array);
			subindex = subtext.find_first_of("\t", subindex) + 1;
			m.x = (float)atof(subtext.substr(subindex, subtext.find_first_of("\t", subindex) - subindex).c_str());
			subindex = subtext.find_first_of("\t", subindex) + 1;
			m.y = (float)atof(subtext.substr(subindex, subtext.find_first_of("\t", subindex) - subindex).c_str());
			subindex = subtext.find_first_of("\t", subindex) + 1;
			m.z = (float)atof(subtext.substr(subindex, subtext.find_first_of("\n", subindex) - subindex).c_str());
			subindex = subtext.find_first_of("\n", subindex);
			faces[i].face_normal = m;
		}

		// Calculate the correct normals, tangents and binormals.
		Intermediate_Face::CalculateNormalsTangentsAndBinormals(vertices, vertices_number, faces, faces_number);

		mesh->vertices = vertices;
		mesh->vertices_number = vertices_number;
		mesh->faces = faces;
		mesh->faces_number = faces_number;

		// Reading the material index or the wireframe color.
		index = text.find("*MATERIAL_REF", 0);
		if (index != std::basic_string<char>::npos) {
			index += strlen("*MATERIAL_REF");
			int materialindex = atoi(text.substr(index, text.find_first_of("\n", index) - index).c_str());
			mesh->material = materiallist[materialindex];
		} else {
			index = text.find("*WIREFRAME_COLOR", 0);
			index += strlen("*WIREFRAME_COLOR") + 1;
			Color cl;
			cl.r = (float)atof(text.substr(index, text.find_first_of(" \t", index) - index).c_str());
			index = text.find_first_of(" \t", index) + 1;
			cl.g = (float)atof(text.substr(index, text.find_first_of(" \t", index) - index).c_str());
			index = text.find_first_of(" \t", index) + 1;
			cl.b = (float)atof(text.substr(index, text.find_first_of(" \n", index) - index).c_str());
			core::Material defaultmat;
			defaultmat.diffuse = cl;
			defaultmat.ambient = cl;
			mesh->material = defaultmat;
		}

		core::Mesh *core_mesh = mesh->ConvertToMesh();
		delete mesh;
		mesh = NULL;

		model->meshes.push_back(core_mesh);
		scene->sub_models.push_back(model);

		// Find the next object in the list.
		geomobject_index = file.find("*GEOMOBJECT", geomobject_index);
	}

	return scene;
}
