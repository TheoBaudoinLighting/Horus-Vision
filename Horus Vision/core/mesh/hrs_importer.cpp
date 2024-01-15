#include "hrs_importer.h"

#include <RadeonProRender_v2.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <tbb/parallel_for.h>

#include <iostream>

#include "hrs_radeon.h"
#include "glm/glm.hpp"
#include <hrs_console.h>

rpr_shape HorusMeshImporter::load_mesh(const std::string& pFile)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();
	HorusConsole& Console = HorusConsole::get_instance();

	spdlog::info("Loading mesh: {}", pFile);
	Console.AddLog(" [info] Loading mesh : %s", pFile.c_str());

	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(pFile, aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType | aiProcess_ValidateDataStructure);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		spdlog::error("Error with Assimp : {}", importer.GetErrorString());
		Console.AddLog(" [error] Error with Assimp : %s", importer.GetErrorString());
		std::exit(EXIT_FAILURE);
	}

	aiMesh* mesh = scene->mMeshes[0];

	std::vector<rpr_float> vertices;
	std::vector<rpr_float> normals;
	std::vector<rpr_float> texcoords;
	std::vector<rpr_int> vIndices;
	std::vector<rpr_int> nIndices;
	std::vector<rpr_int> tIndices;
	std::vector<rpr_int> faceVert;

	unsigned int totalElements = mesh->mNumVertices + mesh->mNumFaces;
	unsigned int processedElements = 0;

	int number_of_udims = 0;
	for (int i = 0; i < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++i)
	{
		if (mesh->HasTextureCoords(i))
		{
			++number_of_udims;
		}
		else
		{
			spdlog::info("Number of UDIMs: {}", number_of_udims);
			Console.AddLog(" [info] Number of UDIMs : %d", number_of_udims);
			break;
		}
	}

	std::vector<int> validUdims;
	for (int udim = 0; udim < number_of_udims; ++udim)
	{
		if (mesh->HasTextureCoords(udim))
		{
			validUdims.push_back(udim);
		}
	}

	spdlog::info("Valid UDIMs: {}", validUdims.size());
	Console.AddLog(" [info] Valid UDIMs : %d", validUdims.size());

#pragma omp parallel for
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{

		aiVector3D vertex = mesh->mVertices[i];
		vertices.push_back(vertex.x);
		vertices.push_back(vertex.y);
		vertices.push_back(vertex.z);

		if (mesh->HasNormals())
		{

			aiVector3D normal = mesh->mNormals[i];
			normals.push_back(normal.x);
			normals.push_back(normal.y);
			normals.push_back(normal.z);
		}

		if (mesh->HasTextureCoords(0))
		{

			aiVector3D texcoord = mesh->mTextureCoords[0][i];
			texcoords.push_back(texcoord.x);
			texcoords.push_back(texcoord.y);
		}
	}

	std::thread load_mesh_thread([&]()
		{

			for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
			{
				aiFace face = mesh->mFaces[i];

				faceVert.push_back(face.mNumIndices);


				for (unsigned int j = 0; j < face.mNumIndices; ++j)
				{
					vIndices.push_back(face.mIndices[j]);

					if (mesh->HasNormals())
					{
						nIndices.push_back(face.mIndices[j]);
					}

					if (mesh->HasTextureCoords(0))
					{
						tIndices.push_back(face.mIndices[j]);
					}

					if (mesh->HasTextureCoords(1))
					{
						tIndices.push_back(face.mIndices[j]);
					}

				}
			}
		});

	/*for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace& face = mesh->mFaces[i];

		unsigned int material_index = mesh->mMaterialIndex;


	}*/



	load_mesh_thread.join();

	rpr_context context = Radeon.get_context();

	bool isValidNorm = mesh->HasNormals();
	bool isValidTex = mesh->HasTextureCoords(0);

	rpr_shape t_shape;

	CHECK(rprContextCreateMesh(context, vertices.data()
		, vertices.size() / 3                                    // num_vertices
		, 3 * sizeof(rpr_float),                                  // vertex_stride
		isValidNorm ? normals.data() : nullptr,                  // normals
		isValidNorm ? normals.size() / 3 : 0,                    // num_normals
		isValidNorm ? 3 * sizeof(rpr_float) : 0,                 // normal_stride
		isValidTex ? texcoords.data() : nullptr,                 // texcoords
		isValidTex ? texcoords.size() / 2 : 0,                   // num_texcoords
		isValidTex ? 2 * sizeof(rpr_float) : 0,                  // texcoord_stride
		vIndices.data(),                                         // vertex_indices
		sizeof(rpr_int),                                         // vertex_index_stride
		isValidNorm ? nIndices.data() : nullptr,                 // normal_indices
		isValidNorm ? sizeof(rpr_int) : 0,                       // normal_index_stride
		isValidTex ? tIndices.data() : nullptr,                  // texcoord_indices
		isValidTex ? sizeof(rpr_int) : 0,                        // texcoord_index_stride
		faceVert.data(),                                         // num_face_vertices
		faceVert.size(),                                         // num_faces
		&t_shape));                                              // out_mesh

	spdlog::info("Mesh loaded: {}", pFile);
	Console.AddLog(" [success] Mesh loaded : %s", pFile.c_str());

	return t_shape;
}
