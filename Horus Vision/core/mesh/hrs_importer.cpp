#include "hrs_importer.h"

#include <RadeonProRender_v2.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/DefaultLogger.hpp>

#include <iostream>

#include "hrs_radeon.h"
#include "glm/glm.hpp"
#include <hrs_console.h>

rpr_shape HorusMeshImporter::LoadMesh(const std::string& pFile)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	spdlog::info("Loading mesh: {}", pFile);
	Console.AddLog(" [info] Loading mesh : %s", pFile.c_str());

	Assimp::Importer Importer;

	/*const aiScene* scene = importer.ReadFile(pFile, aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType | aiProcess_ValidateDataStructure);*/

	const aiScene* Scene = Importer.ReadFile(pFile, aiProcessPreset_TargetRealtime_Quality);


	if (!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode)
	{
		spdlog::error("Error with Assimp : {}", Importer.GetErrorString());
		Console.AddLog(" [error] Error with Assimp : %s", Importer.GetErrorString());
		return nullptr;
	}

	aiMesh* Mesh = Scene->mMeshes[0];

	std::vector<rpr_float> Vertices;
	std::vector<rpr_float> Normals;
	std::vector<rpr_float> Texcoords;
	std::vector<rpr_int> VIndices;
	std::vector<rpr_int> NIndices;
	std::vector<rpr_int> TIndices;
	std::vector<rpr_int> FaceVert;

	int NumberOfUdims = 0;
	for (int i = 0; i < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++i)
	{
		if (Mesh->HasTextureCoords(i))
		{
			++NumberOfUdims;
		}
		else
		{
			spdlog::info("Number of UDIMs: {}", NumberOfUdims);
			Console.AddLog(" [info] Number of UDIMs : %d", NumberOfUdims);
			break;
		}
	}

	std::vector<int> ValidUdims;
	for (int Udim = 0; Udim < NumberOfUdims; ++Udim)
	{
		if (Mesh->HasTextureCoords(Udim))
		{
			ValidUdims.push_back(Udim);
		}
	}

	spdlog::info("Valid UDIMs: {}", ValidUdims.size());
	Console.AddLog(" [info] Valid UDIMs : %d", ValidUdims.size());

#pragma omp parallel for
	for (unsigned int i = 0; i < Mesh->mNumVertices; ++i)
	{

		aiVector3D Vertex = Mesh->mVertices[i];
		Vertices.push_back(Vertex.x);
		Vertices.push_back(Vertex.y);
		Vertices.push_back(Vertex.z);

		if (Mesh->HasNormals())
		{

			aiVector3D Normal = Mesh->mNormals[i];
			Normals.push_back(Normal.x);
			Normals.push_back(Normal.y);
			Normals.push_back(Normal.z);
		}

		if (Mesh->HasTextureCoords(0))
		{

			aiVector3D Texcoord = Mesh->mTextureCoords[0][i];
			Texcoords.push_back(Texcoord.x);
			Texcoords.push_back(Texcoord.y);
		}
	}

	std::thread LoadMeshThread([&]()
		{

			for (unsigned int i = 0; i < Mesh->mNumFaces; ++i)
			{
				aiFace Face = Mesh->mFaces[i];

				FaceVert.push_back(Face.mNumIndices);

				for (unsigned int j = 0; j < Face.mNumIndices; ++j)
				{
					VIndices.push_back(Face.mIndices[j]);

					if (Mesh->HasNormals())
					{
						NIndices.push_back(Face.mIndices[j]);
					}

					if (Mesh->HasTextureCoords(0))
					{
						TIndices.push_back(Face.mIndices[j]);
					}

					if (Mesh->HasTextureCoords(1))
					{
						TIndices.push_back(Face.mIndices[j]);
					}

				}
			}
		});

	/*for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace& face = mesh->mFaces[i];

		unsigned int material_index = mesh->mMaterialIndex;


	}*/

	LoadMeshThread.join();

	rpr_context Context = Radeon.GetContext();

	bool IsValidNorm = Mesh->HasNormals();
	bool IsValidTex = Mesh->HasTextureCoords(0);

	rpr_shape TShape;

	CHECK(rprContextCreateMesh(Context, Vertices.data()
		, Vertices.size() / 3                                    // num_vertices
		, 3 * sizeof(rpr_float),                                  // vertex_stride
		IsValidNorm ? Normals.data() : nullptr,                  // normals
		IsValidNorm ? Normals.size() / 3 : 0,                    // num_normals
		IsValidNorm ? 3 * sizeof(rpr_float) : 0,                 // normal_stride
		IsValidTex ? Texcoords.data() : nullptr,                 // texcoords
		IsValidTex ? Texcoords.size() / 2 : 0,                   // num_texcoords
		IsValidTex ? 2 * sizeof(rpr_float) : 0,                  // texcoord_stride
		VIndices.data(),                                         // vertex_indices
		sizeof(rpr_int),                                         // vertex_index_stride
		IsValidNorm ? NIndices.data() : nullptr,                 // normal_indices
		IsValidNorm ? sizeof(rpr_int) : 0,                       // normal_index_stride
		IsValidTex ? TIndices.data() : nullptr,                  // texcoord_indices
		IsValidTex ? sizeof(rpr_int) : 0,                        // texcoord_index_stride
		FaceVert.data(),                                         // num_face_vertices
		FaceVert.size(),                                         // num_faces
		&TShape));                                              // out_mesh

	spdlog::info("Mesh loaded: {}", pFile);
	Console.AddLog(" [success] Mesh loaded : %s", pFile.c_str());

	return TShape;
}
