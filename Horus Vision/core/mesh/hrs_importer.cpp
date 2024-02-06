#include "hrs_object_manager.h"
#include "hrs_importer.h"

#include <RadeonProRender.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/DefaultLogger.hpp>

#include <iostream>

#include "hrs_radeon.h"
#include <hrs_console.h>
#include <mutex>
#include <tuple>

#include "common.h"

inline std::mutex LoadMeshMutex;
inline std::mutex GlobalTransformMutex;

aiMatrix4x4 CalculateGlobalTransform(const aiNode* pNode)
{
	if (pNode->mParent != nullptr)
	{
		return CalculateGlobalTransform(pNode->mParent) * pNode->mTransformation;
	}
	else
	{
		return pNode->mTransformation;
	}
}

const aiNode* FindNodeForMesh(const aiNode* pNode, const aiMesh* pMesh, const aiScene* pScene)
{
	for (unsigned int i = 0; i < pNode->mNumMeshes; ++i)
	{
		if (pScene->mMeshes[pNode->mMeshes[i]] == pMesh)
			return pNode;
	}

	for (unsigned int i = 0; i < pNode->mNumChildren; ++i)
	{
		const aiNode* found = FindNodeForMesh(pNode->mChildren[i], pMesh, pScene);
		if (found) return found;
	}

	return nullptr;
}

std::vector<std::tuple<rpr_shape, std::string>> HorusMeshImporter::LoadMesh(const std::string& MeshPath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	spdlog::info("Loading mesh: {}", MeshPath);
	Console.AddLog(" [info] Loading mesh : %s", MeshPath.c_str());

	if (MeshPath.empty())
	{
		spdlog::error("Mesh path is empty");
		Console.AddLog(" [error] Mesh path is empty");
		return std::vector<std::tuple<rpr_shape, std::string>>();
	}

	std::vector<std::tuple<rpr_shape, std::string>> ShapesAndNames;

	Assimp::Importer Importer;

	const aiScene* Scene = Importer.ReadFile(MeshPath, aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_GenNormals | aiProcess_GenSmoothNormals 
		 | aiProcess_ValidateDataStructure);

	/*const aiScene* Scene = Importer.ReadFile(MeshPath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);*/

	if (!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode)
	{
		spdlog::error("Error with Assimp : {}", Importer.GetErrorString());
		Console.AddLog(" [error] Error with Assimp : %s", Importer.GetErrorString());
		return std::vector<std::tuple<rpr_shape, std::string>>();
	}

	for (unsigned int m = 0; m < Scene->mNumMeshes; ++m) 
	{
		aiMesh* Mesh = Scene->mMeshes[m];

		const aiNode* pNode = FindNodeForMesh(Scene->mRootNode, Mesh, Scene);
		if (!pNode)
		{
			spdlog::error("No corresponding node found for mesh {}", m);
			
			continue;
		}

		std::string ShapeName = pNode->mName.C_Str();

		aiMatrix4x4 Transform = CalculateGlobalTransform(pNode);

		if (!Mesh) 
		{
			spdlog::error("Error with Assimp: Mesh {} is invalid", m);
			Console.AddLog(" [error] Error with Assimp: Mesh %s is invalid", m);
			continue; 
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
		Console.AddLog(" [info] Mesh has %d Udims", UdimTexcoords.size());

		bool HasNormals = Mesh->HasNormals();
		bool HasTexCoords = Mesh->HasTextureCoords(0);

		for (unsigned int i = 0; i < Mesh->mNumVertices; ++i)
		{
			aiVector3D Vertex = Transform * Mesh->mVertices[i];
			Vertices.emplace_back(Vertex.x);
			Vertices.emplace_back(Vertex.y);
			Vertices.emplace_back(Vertex.z);

			if (HasNormals)
			{
				aiVector3D Normal = Transform * Mesh->mNormals[i];
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
			for (unsigned int I = 0; I < Mesh->mNumFaces; ++I)
			{
				std::lock_guard Guard(LoadMeshMutex);

				aiFace Face = Mesh->mFaces[I];
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
							if (I < Mesh->mNumVertices)
							{
								aiVector3D TexCoord = Mesh->mTextureCoords[Udim][I];
								UdimTexcoords[Udim].emplace_back(TexCoord.x);
								UdimTexcoords[Udim].emplace_back(TexCoord.y);
							}
						}
						else
						{
							spdlog::error("Mesh has no texture coordinates");
							Console.AddLog(" [error] Mesh has no texture coordinates");
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

		// TODO : Replace by rprContextCreateMeshEx2 
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

		ShapesAndNames.emplace_back(TShape, ShapeName);

		// create material and assign it to the shape
		if (Mesh->mMaterialIndex >= 0) 
		{
			aiMaterial* mat = Scene->mMaterials[Mesh->mMaterialIndex];
			aiString matName;
			mat->Get(AI_MATKEY_NAME, matName);

			std::string MaterialName = matName.C_Str();

			int Material;
			if (ObjectManager.MaterialExists(MaterialName)) {
				Material = ObjectManager.GetMaterialIdByName(MaterialName);
			}
			else {
				Material = ObjectManager.CreateMaterial(MaterialName);
			}

			ObjectManager.AssignMaterialDirectly(TShape, Material);
		}
	}

	spdlog::info("Mesh loaded: {}", MeshPath);
	Console.AddLog(" [info] Mesh loaded : %s", MeshPath.c_str());

	return ShapesAndNames;
}

