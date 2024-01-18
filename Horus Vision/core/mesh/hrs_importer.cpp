#include "hrs_importer.h"

#include <RadeonProRender_v2.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/DefaultLogger.hpp>

#include <iostream>

#include "hrs_radeon.h"
#include <hrs_console.h>
#include <mutex>

inline std::mutex LoadMeshMutex;

rpr_shape HorusMeshImporter::LoadMesh(const std::string& MeshPath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	spdlog::info("Loading mesh: {}", MeshPath);
	Console.AddLog(" [info] Loading mesh : %s", MeshPath.c_str());

	if (MeshPath.empty())
	{
		spdlog::error("Mesh path is empty");
		Console.AddLog(" [error] Mesh path is empty");
		return nullptr;
	}

	Assimp::Importer Importer;

	const aiScene* Scene = Importer.ReadFile(MeshPath, aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType | aiProcess_ValidateDataStructure);

	//const aiScene* Scene = Importer.ReadFile(MeshPath, aiProcessPreset_TargetRealtime_Quality);



	if (!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode)
	{
		spdlog::error("Error with Assimp : {}", Importer.GetErrorString());
		Console.AddLog(" [error] Error with Assimp : %s", Importer.GetErrorString());
		return nullptr;
	}

	aiMesh* Mesh = Scene->mMeshes[0];

	if (!Mesh)
	{
		spdlog::error("Error with Assimp : {}", Importer.GetErrorString());
		Console.AddLog(" [error] Error with Assimp : %s", Importer.GetErrorString());
		return nullptr;
	}

	std::vector<rpr_float> Vertices;
	std::vector<rpr_float> Normals;
	std::vector<rpr_float> Texcoords;
	std::vector<rpr_int> VIndices;
	std::vector<rpr_int> NIndices;
	std::vector<rpr_int> TIndices;
	std::vector<rpr_int> FaceVert;

	Vertices.reserve(Mesh->mNumVertices * 3);
	Normals.reserve(Mesh->mNumVertices * 3);
	Texcoords.reserve(Mesh->mNumVertices * 2);
	VIndices.reserve(Mesh->mNumFaces * 3);
	NIndices.reserve(Mesh->mNumFaces * 3);
	TIndices.reserve(Mesh->mNumFaces * 3);
	FaceVert.reserve(Mesh->mNumFaces);

	if (m_LoadMeshThread_.joinable())
	{
		m_LoadMeshThread_.join();
	}

	std::vector<std::vector<rpr_float>> UdimTexcoords;
	for (int i = 0; i < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++i)
	{
		if (!Mesh->HasTextureCoords(i)) {
			break;
		}
		UdimTexcoords.emplace_back();
		UdimTexcoords.back().reserve(Mesh->mNumVertices * 2);
	}

	spdlog::info("Mesh has {} Udims", UdimTexcoords.size());

	bool HasNormals = Mesh->HasNormals();
	bool HasTexCoords = Mesh->HasTextureCoords(0);

	for (unsigned int i = 0; i < Mesh->mNumVertices; ++i)
	{
		aiVector3D Vertex = Mesh->mVertices[i];
		Vertices.emplace_back(Vertex.x);
		Vertices.emplace_back(Vertex.y);
		Vertices.emplace_back(Vertex.z);

		if (HasNormals)
		{
			aiVector3D Normal = Mesh->mNormals[i];
			Normals.emplace_back(Normal.x);
			Normals.emplace_back(Normal.y);
			Normals.emplace_back(Normal.z);
		}

		if (HasTexCoords)
		{
			aiVector3D TexCoord = Mesh->mTextureCoords[0][i];
			Texcoords.emplace_back(TexCoord.x);
			Texcoords.emplace_back(TexCoord.y);
		}

	}

	m_LoadMeshThread_ = std::thread([&]() {
		for (unsigned int i = 0; i < Mesh->mNumFaces; ++i)
		{
			std::lock_guard Guard(LoadMeshMutex);

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

				for (size_t Udim = 0; Udim < UdimTexcoords.size(); ++Udim)
				{
					if (Mesh->mTextureCoords[Udim])
					{
						if (i < Mesh->mNumVertices)
						{
							aiVector3D TexCoord = Mesh->mTextureCoords[Udim][i];
							UdimTexcoords[Udim].emplace_back(TexCoord.x);
							UdimTexcoords[Udim].emplace_back(TexCoord.y);
						}
					}
					else
					{
						spdlog::error("Mesh has no texture coordinates");
					}
				}
			}
		}
		});


	if (m_LoadMeshThread_.joinable())
	{
		m_LoadMeshThread_.join();
	}

	rpr_context Context = Radeon.GetContext();

	bool IsValidNorm = Mesh->HasNormals();
	bool IsValidTex = Mesh->HasTextureCoords(0);

	rpr_shape TShape;

	CHECK(rprContextCreateMesh(Context, Vertices.data()
		, Vertices.size() / 3                                    // num_vertices
		, 3 * sizeof(rpr_float),                                 // vertex_stride
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
		&TShape)) // out_mesh

		spdlog::info("Mesh loaded: {}", MeshPath);
	Console.AddLog(" [success] Mesh loaded : %s", MeshPath.c_str());

	return TShape;
}
