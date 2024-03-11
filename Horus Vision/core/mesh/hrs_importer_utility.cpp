
// C++ Includes
#include "spdlog/spdlog.h"

// Horus Includes
#include "hrs_preprocess_meshdata.h"
#include "hrs_importer_utility.h"
#include "hrs_om_id_manager.h"
#include "hrs_console.h"
#include "hrs_radeon.h"
#include "hrs_mesh.h"

// Assimp Includes
#include <assimp/DefaultLogger.hpp>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

// Radeon Includes
#include "hrs_object_manager.h"
#include "hrs_reset_buffers.h"
#include "RadeonProRender_v2.h"

aiNode* FindNodesForMesh(aiNode* pNode, aiMesh* pMesh, const aiScene* pScene)
{
	for (unsigned int i = 0; i < pNode->mNumMeshes; ++i)
	{
		if (pScene->mMeshes[pNode->mMeshes[i]] == pMesh)
			return pNode;
	}

	for (unsigned int i = 0; i < pNode->mNumChildren; ++i)
	{
		aiNode* found = FindNodesForMesh(pNode->mChildren[i], pMesh, pScene);
		if (found) return found;
	}

	return nullptr;
}

ModelData HorusMeshImporterUtility::GenerateModelData(PreProcessMeshData& Data)
{
	HorusConsole& kConsole = HorusConsole::GetInstance();
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusIdManager& kIdManager = HorusIdManager::GetInstance();

	std::string kPath = Data.Path;

	spdlog::info("Loading model: {}", kPath);
	kConsole.AddLog(" [info] Loading model : %s", kPath.c_str());

	// Check if the path is valid
	if (!IsPathValid(kPath))
	{
		spdlog::error("Path is not valid: {}", kPath);
		kConsole.AddLog(" [error] Path is not valid: %s", kPath.c_str());
		spdlog::error("Returning empty model data");
		kConsole.AddLog("[error] Returning empty model data");
		return {};
	}

	// Read the model with Assimp
	Assimp::Importer kImporter;

	spdlog::info("Reading model with Assimp : {}", kPath);

	const aiScene* kScene = kImporter.ReadFile(kPath, aiProcess_CalcTangentSpace |
		aiProcess_Triangulate | // If remove this, it will be quads and indices parsing will be different ( so remove the indices * 3 in kMesh filling data)
		aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_GenNormals | aiProcess_GenSmoothNormals
		| aiProcess_ValidateDataStructure);

	/*const aiScene* kScene = kImporter.ReadFile(kPath, 0);*/

	if (!kScene || kScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !kScene->mRootNode)
	{
		spdlog::error("Error with Assimp : {}", kImporter.GetErrorString());
		kConsole.AddLog(" [error] Error with Assimp : %s", kImporter.GetErrorString());
		return {};
	}

	// Check if the scene contains any meshes, if not return empty model data
	if (!IsSceneContainsMesh(kScene))
	{
		spdlog::warn("Scene does not contain any meshes");
		kConsole.AddLog(" [warn] Scene does not contain any meshes");
		spdlog::error("Returning empty model data");
		kConsole.AddLog("[error] Returning empty model data");
		return {};
	}

	// Create the model data
	ModelData kModelData;

	// Process the scene, per mesh
	for (unsigned int i = 0; i < kScene->mNumMeshes; i++)
	{
		// Get the mesh
		aiMesh* kMesh = kScene->mMeshes[i];

		// Check if the mesh is valid and contains vertices or some data
		if (!IsMeshValid(kMesh))
		{
			spdlog::warn("Mesh is not valid");
			kConsole.AddLog(" [warn] Mesh is not valid");
			continue;
		}

		MeshData kMeshData;

		aiNode* kNode = FindNodesForMesh(kScene->mRootNode, kMesh, kScene);
		if (!kNode)
		{
			spdlog::error("No corresponding node found for mesh {}", i);
			kConsole.AddLog(" [error] Error mesh finding node");
			spdlog::error("Mesh will be skipped");
			kConsole.AddLog(" [error] Mesh will be skipped");
			continue;
		}

		std::string kShapeName = kNode->mName.C_Str();
		aiMatrix4x4 kTransform = ProcessGlobaltransform(kNode);
		auto kTexCoodsNumber = GetNumberOfTextureCoords(kMesh);

		if (kShapeName.empty()) {
			kShapeName = "Mesh" + std::to_string(i);
		}

		kMeshData.MeshName = kShapeName;
		kMeshData.bHasNormals = kMesh->HasNormals();
		kMeshData.bHasTangents = kMesh->HasTangentsAndBitangents();
		kMeshData.bHasBitangents = kMesh->HasTangentsAndBitangents();
		kMeshData.bHasTextureCoords = kMesh->HasTextureCoords(kTexCoodsNumber);
		kMeshData.bHasMaterialIDs = kMesh->mMaterialIndex >= 0;

		if (!ProcessVertices(kMesh, kTransform, kMeshData.Vertices, kMeshData.bHasNormals, kMeshData.bHasBitangents, kMeshData.bHasTextureCoords)) {}
		if (!ProcessIndices(kMesh, kMeshData.Indices)) {}

		// Create an Unique ID for the mesh
		int kMeshID = kIdManager.GetNewId();

		kMeshData.MeshID = kMeshID;
		kMeshData.NumTextureCoords = kTexCoodsNumber;
		kMeshData.BoundingBoxMin = { kMesh->mAABB.mMin.x, kMesh->mAABB.mMin.y, kMesh->mAABB.mMin.z };
		kMeshData.BoundingBoxMax = { kMesh->mAABB.mMax.x, kMesh->mAABB.mMax.y, kMesh->mAABB.mMax.z };
		kMeshData.NumVertices = kMesh->mNumVertices;
		kMeshData.NumIndices = kMeshData.Indices.size();
		kMeshData.NumFaces = kMesh->mNumFaces;

		spdlog::info("*************************************");
		spdlog::info("Mesh ID: {}", kMeshData.MeshID);
		spdlog::info("Mesh Name: {}", kMeshData.MeshName);
		spdlog::info("Material IDs: {}", kMeshData.MaterialIDs.size());
		spdlog::info("Texture Coords: {}", kMeshData.NumTextureCoords);
		spdlog::info("Bounding Box Min: ({}, {}, {})", kMeshData.BoundingBoxMin.x, kMeshData.BoundingBoxMin.y, kMeshData.BoundingBoxMin.z);
		spdlog::info("Bounding Box Max: ({}, {}, {})", kMeshData.BoundingBoxMax.x, kMeshData.BoundingBoxMax.y, kMeshData.BoundingBoxMax.z);
		spdlog::info("Has Normals: {}", kMeshData.bHasNormals);
		spdlog::info("Has Tangents: {}", kMeshData.bHasTangents);
		spdlog::info("Has Bitangents: {}", kMeshData.bHasBitangents);
		spdlog::info("Has Texture Coords: {}", kMeshData.bHasTextureCoords);
		spdlog::info("Has Material IDs: {}", kMeshData.bHasMaterialIDs);
		spdlog::info("Num Vertices: {}", kMeshData.NumVertices);
		spdlog::info("Num Indices: {}", kMeshData.NumIndices);
		spdlog::info("Num Faces: {}", kMeshData.NumFaces);
		spdlog::info("*************************************");

		if (!CreateOpenGlMesh(kMeshData))
		{
			spdlog::error("Error creating OpenGL mesh, set to blank");
			kConsole.AddLog(" [error] Error creating OpenGL mesh, set to blank");
			kMeshData.OpenGlShape = {};
		}

		if (!CreateRadeonMesh(kMeshData))
		{
			spdlog::error("Error creating Radeon mesh, set to nullptr");
			kConsole.AddLog(" [error] Error creating Radeon mesh, set to nullptr");
			kMeshData.RadeonShape = nullptr;
		}

		if (kMesh->mMaterialIndex >= 0)
		{
			aiMaterial* kMaterial = kScene->mMaterials[kMesh->mMaterialIndex];
			aiString kMaterialName;
			kMaterial->Get(AI_MATKEY_NAME, kMaterialName);

			std::string kMaterialNameStr = kMaterialName.C_Str();

			int kMaterialID = -1;
			if (HorusIdManager::GetInstance().ContainsName(kMaterialNameStr))
			{
				kMaterialID = ObjectManager.GetMaterialIdByName(kMaterialNameStr);
			}
			else
			{
				kMaterialID = ObjectManager.CreateMaterial(kMaterialNameStr);
			}

			ObjectManager.AssignMaterialDirectly(kMeshData.RadeonShape, kMaterialID);
		}

		kModelData.Meshes[kMeshData.MeshID] = std::move(kMeshData);
	}

	if (kModelData.Meshes.empty())
	{
		spdlog::warn("Model does not contain any meshes");
		kConsole.AddLog(" [warn] Model does not contain any meshes");
		spdlog::error("Returning empty model data");
		kConsole.AddLog("[error] Returning empty model data");
		return {};
	}

	int kModelID = kIdManager.GetNewId();
	kModelData.ModelID = kModelID;
	kModelData.ModelName = kScene->mRootNode->mName.C_Str();

	return kModelData;
}
MeshData HorusMeshImporterUtility::GetMeshFromModelData(const ModelData& Model, const std::string& MeshName)
{

	return MeshData();
}
const aiScene* HorusMeshImporterUtility::ReadWithAssimp(const std::string& Path) // passed by reference
{
	HorusConsole& kConsole = HorusConsole::GetInstance();
	Assimp::Importer kImporter;

	const aiScene* kScene = kImporter.ReadFile(Path, aiProcess_CalcTangentSpace |
		aiProcess_Triangulate | // If remove this, it will be quads and indices parsing will be different ( so remove the indices * 3 in kMesh filling data)
		aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_GenNormals | aiProcess_GenSmoothNormals
		| aiProcess_ValidateDataStructure);

	if (!kScene || kScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !kScene->mRootNode)
	{
		spdlog::error("Error with Assimp : {}", kImporter.GetErrorString());
		kConsole.AddLog(" [error] Error with Assimp : %s", kImporter.GetErrorString());
		return nullptr;
	}

	return kScene;
}
const aiNode* HorusMeshImporterUtility::FindNode(const aiNode* Node, const aiMesh* Mesh, const aiScene* Scene)
{
	for (unsigned int i = 0; i < Node->mNumMeshes; ++i)
	{
		if (Scene->mMeshes[Node->mMeshes[i]] == Mesh)
			return Node;
	}

	for (unsigned int i = 0; i < Node->mNumChildren; ++i)
	{
		const aiNode* found = FindNode(Node->mChildren[i], Mesh, Scene);
		if (found) return found;
	}

	return nullptr;
}
aiMatrix4x4 HorusMeshImporterUtility::ProcessGlobaltransform(const aiNode* Node)
{
	if (!Node)
	{
		return aiMatrix4x4();
	}

	return Node->mTransformation;
}
bool HorusMeshImporterUtility::ProcessVertices(const aiMesh* Mesh, const aiMatrix4x4& Transform, std::vector<VertexData>& Vertices, bool kHasNormals, bool HasTangentsAndBitangents, bool kHasTexCoords)
{
	HorusConsole& kConsole = HorusConsole::GetInstance();

	if (!Mesh || Mesh->mNumVertices <= 0)
	{
		spdlog::warn("Mesh is not valid or does not contain any vertices");
		kConsole.AddLog(" [warn] Mesh is not valid or does not contain any vertices");
		return false;
	}

	for (unsigned int i = 0; i < Mesh->mNumVertices; ++i)
	{
		VertexData kVertex;
		aiVector3D kPos = Transform * Mesh->mVertices[i];

		kVertex.Position = glm::vec3(Mesh->mVertices[i].x, Mesh->mVertices[i].y, Mesh->mVertices[i].z);

		if (kHasNormals)
		{
			kVertex.Normal = glm::vec3(Mesh->mNormals[i].x, Mesh->mNormals[i].y, Mesh->mNormals[i].z);
		}

		// Color is set to white
		kVertex.Color = glm::vec3(1.0f);

		if (HasTangentsAndBitangents)
		{
			kVertex.Tangent = { Mesh->mTangents[i].x, Mesh->mTangents[i].y, Mesh->mTangents[i].z };
			kVertex.Bitangent = { Mesh->mBitangents[i].x, Mesh->mBitangents[i].y, Mesh->mBitangents[i].z };
		}

		if (kHasTexCoords) {
			kVertex.TexCoords = { Mesh->mTextureCoords[0][i].x, Mesh->mTextureCoords[0][i].y };
		}
		else
		{
			kVertex.TexCoords = { 0.0f, 0.0f };
		}

		Vertices.push_back(kVertex);
	}

	if (Vertices.empty() || Vertices.size() <= 0)
	{
		spdlog::warn("Vertices are empty");
		kConsole.AddLog(" [warn] Vertices are empty");
		return false;
	}

	if (kHasNormals && !Mesh->HasNormals())
	{
		spdlog::warn("Normals are empty");
		kConsole.AddLog(" [warn] Normals are empty");
		return false;
	}

	if (kHasTexCoords && !Mesh->HasTextureCoords(0))
	{
		spdlog::warn("Texture coordinates are empty");
		kConsole.AddLog(" [warn] Texture coordinates are empty");
		return false;
	}

	return true;
}
bool HorusMeshImporterUtility::ProcessMaterialIDs(const aiMesh* Mesh, std::vector<unsigned int>& MaterialIDs)
{
	HorusConsole& kConsole = HorusConsole::GetInstance();

	if (Mesh->mMaterialIndex < 0)
	{
		spdlog::warn("Material index is less than 0");
		kConsole.AddLog(" [warn] Material index is less than 0");
		return false;
	}

	for (unsigned int i = 0; i < Mesh->mNumFaces; i++)
	{
		MaterialIDs.push_back(Mesh->mMaterialIndex);
	}

	if (MaterialIDs.empty() || MaterialIDs.size() <= 0)
	{
		spdlog::warn("Material IDs are empty");
		kConsole.AddLog(" [warn] Material IDs are empty");
		return false;
	}

	return true;
}
bool HorusMeshImporterUtility::ProcessIndices(const aiMesh* Mesh, std::vector<unsigned int>& Indices)
{
	HorusConsole& kConsole = HorusConsole::GetInstance();

	for (unsigned int i = 0; i < Mesh->mNumFaces; i++) {
		const aiFace& face = Mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			Indices.push_back(face.mIndices[j]);
		}
	}

	if (Indices.empty() || Indices.size() <= 0)
	{
		spdlog::warn("Indices are empty");
		kConsole.AddLog(" [warn] Indices are empty");
		return false;
	}

	return true;
}
void HorusMeshImporterUtility::ProcessFace(const aiMesh* Mesh, const aiFace& Face, std::vector<unsigned int>& Indices)
{
	for (unsigned int i = 0; i < Face.mNumIndices; i++)
	{
		Indices.push_back(Face.mIndices[i]);
	}
}

bool HorusMeshImporterUtility::HasNormals(const aiMesh* Mesh)
{
	if (!Mesh->HasNormals())
	{
		spdlog::warn("Mesh does not contain any normals");
		HorusConsole::GetInstance().AddLog(" [warn] Mesh does not contain any normals");
		return false;
	}

	return true;
}
bool HorusMeshImporterUtility::HasTexCoords(const aiMesh* Mesh)
{
	if (!Mesh->HasTextureCoords(0))
	{
		spdlog::warn("Mesh does not contain any texture coordinates");
		HorusConsole::GetInstance().AddLog(" [warn] Mesh does not contain any texture coordinates");
		return false;
	}

	return true;
}
bool HorusMeshImporterUtility::HasTangentsAndBitangents(const aiMesh* Mesh)
{
	if (!Mesh->HasTangentsAndBitangents())
	{
		spdlog::warn("Mesh does not contain any tangents and bitangents");
		HorusConsole::GetInstance().AddLog(" [warn] Mesh does not contain any tangents and bitangents");
		return false;
	}

	return true;
}
bool HorusMeshImporterUtility::HasFaces(const aiMesh* Mesh)
{
	if (!Mesh->HasFaces())
	{
		spdlog::warn("Mesh does not contain any faces");
		HorusConsole::GetInstance().AddLog(" [warn] Mesh does not contain any faces");
		return false;
	}

	return true;
}
bool HorusMeshImporterUtility::HasBones(const aiMesh* Mesh)
{
	if (!Mesh->HasBones())
	{
		spdlog::warn("Mesh does not contain any bones");
		HorusConsole::GetInstance().AddLog(" [warn] Mesh does not contain any bones");
		return false;
	}

	return true;
}

bool HorusMeshImporterUtility::IsPathValid(const std::string& Path)
{
	if (Path.empty() || Path == "")
	{
		return false;
	}

	return true;
}
bool HorusMeshImporterUtility::IsSceneValid(const aiScene* Scene)
{
	HorusConsole& kConsole = HorusConsole::GetInstance();

	if (!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode)
	{
		spdlog::error("Scene is not valid");
		kConsole.AddLog(" [error] Scene is not valid");
		return false;
	}

	return true;
}
bool HorusMeshImporterUtility::IsSceneContainsMesh(const aiScene* Scene)
{
	HorusConsole& kConsole = HorusConsole::GetInstance();

	if (Scene->mNumMeshes < 0)
	{
		spdlog::warn("Scene does not contain any meshes");
		kConsole.AddLog(" [warn] Scene does not contain any meshes");
		return false;
	}

	return true;
}
bool HorusMeshImporterUtility::IsMeshValid(const aiMesh* Mesh)
{
	HorusConsole& kConsole = HorusConsole::GetInstance();

	if (!Mesh || Mesh->mNumVertices <= 0)
	{
		spdlog::warn("Mesh is not valid or does not contain any vertices");
		kConsole.AddLog(" [warn] Mesh is not valid or does not contain any vertices");
		return false;
	}

	if (Mesh->mNumFaces <= 0)
	{
		spdlog::warn("Mesh does not contain any faces");
		kConsole.AddLog(" [warn] Mesh does not contain any faces");
		return false;
	}

	if (!Mesh->HasNormals())
	{
		spdlog::warn("Mesh does not contain any normals");
		kConsole.AddLog(" [warn] Mesh does not contain any normals");
	}

	if (!Mesh->HasTextureCoords(0))
	{
		spdlog::warn("Mesh does not contain any texture coordinates");
		kConsole.AddLog(" [warn] Mesh does not contain any texture coordinates");
	}

	if (!Mesh->HasTangentsAndBitangents())
	{
		spdlog::warn("Mesh does not contain any tangents and bitangents");
		kConsole.AddLog(" [warn] Mesh does not contain any tangents and bitangents");
	}

	if (!Mesh->HasFaces())
	{
		spdlog::warn("Mesh does not contain any faces");
		kConsole.AddLog(" [warn] Mesh does not contain any faces");
	}

	return true;
}

bool HorusMeshImporterUtility::CreateOpenGlMesh(MeshData& Mesh)
{
	if (Mesh.Vertices.empty()) {
		spdlog::error("Vertices are empty for mesh: {}", Mesh.MeshName);
		HorusConsole::GetInstance().AddLog(" [error] Vertices are empty");
		return false;
	}

	HorusOpenGlShape kShape;
	kShape.SetShapeName(Mesh.MeshName);
	kShape.InitOpenGlMesh(Mesh.Vertices, Mesh.Indices);

	Mesh.OpenGlShape = kShape;
	return true;
}

std::mutex gLoadRadeonMutex;
bool HorusMeshImporterUtility::CreateRadeonMesh(MeshData& Mesh)
{
	std::lock_guard<std::mutex> kLock(gLoadRadeonMutex);
	HorusRadeon& kRadeon = HorusRadeon::GetInstance();
	HorusObjectManager& kObjectManager = HorusObjectManager::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	std::vector<rpr_float> kVertices, kNormals, kTexCoords;
	std::vector<rpr_int> kVIndices, kNIndices, kTIndices, kFaceVert;

	kVertices.reserve(Mesh.NumVertices * 3);
	kNormals.reserve(Mesh.NumVertices * 3);
	kTexCoords.reserve(Mesh.NumVertices * 2);
	kVIndices.reserve(Mesh.NumFaces * 3);
	kNIndices.reserve(Mesh.NumFaces * 3);
	kTIndices.reserve(Mesh.NumFaces * 3);
	kFaceVert.reserve(Mesh.NumFaces);

	bool kHasNormals = Mesh.bHasNormals;
	bool kHasTexCoords = Mesh.bHasTextureCoords;

	for (unsigned int i = 0; i < Mesh.NumVertices; i++)
	{
		kVertices.push_back(Mesh.Vertices[i].Position.x);
		kVertices.push_back(Mesh.Vertices[i].Position.y);
		kVertices.push_back(Mesh.Vertices[i].Position.z);

		if (kHasNormals)
		{
			kNormals.push_back(Mesh.Vertices[i].Normal.x);
			kNormals.push_back(Mesh.Vertices[i].Normal.y);
			kNormals.push_back(Mesh.Vertices[i].Normal.z);
		}

		if (kHasTexCoords)
		{
			kTexCoords.push_back(Mesh.Vertices[i].TexCoords.x);
			kTexCoords.push_back(Mesh.Vertices[i].TexCoords.y);
		}
	}

	for (unsigned int i = 0; i < Mesh.NumFaces; i++)
	{
		kFaceVert.push_back(3);

		kVIndices.push_back(Mesh.Indices[i * 3]);
		kVIndices.push_back(Mesh.Indices[i * 3 + 1]);
		kVIndices.push_back(Mesh.Indices[i * 3 + 2]);

		if (kHasNormals)
		{
			kNIndices.push_back(Mesh.Indices[i * 3]);
			kNIndices.push_back(Mesh.Indices[i * 3 + 1]);
			kNIndices.push_back(Mesh.Indices[i * 3 + 2]);
		}

		if (kHasTexCoords)
		{
			kTIndices.push_back(Mesh.Indices[i * 3]);
			kTIndices.push_back(Mesh.Indices[i * 3 + 1]);
			kTIndices.push_back(Mesh.Indices[i * 3 + 2]);
		}
	}

	rpr_context kContext = kRadeon.GetContext();
	rpr_shape kShape = nullptr;

	bool IsValidNorm = Mesh.bHasNormals;
	bool IsValidTex = Mesh.bHasTextureCoords;

	CHECK(rprContextCreateMesh(kContext, kVertices.data()
		, kVertices.size() / 3									 // num_vertices
		, 3 * sizeof(rpr_float),                                 // vertex_stride
		IsValidNorm ? kNormals.data() : nullptr,                 // normals
		IsValidNorm ? kNormals.size() / 3 : 0,                   // num_normals
		IsValidNorm ? 3 * sizeof(rpr_float) : 0,                 // normal_stride
		IsValidTex ? kTexCoords.data() : nullptr,                // texcoords
		IsValidTex ? kTexCoords.size() / 2 : 0,                  // num_texcoords
		IsValidTex ? 2 * sizeof(rpr_float) : 0,                  // texcoord_stride
		kVIndices.data(),                                        // vertex_indices
		sizeof(rpr_int),                                         // vertex_index_stride
		IsValidNorm ? kVIndices.data() : nullptr,                // normal_indices
		IsValidNorm ? sizeof(rpr_int) : 0,                       // normal_index_stride
		IsValidTex ? kTIndices.data() : nullptr,                 // texcoord_indices
		IsValidTex ? sizeof(rpr_int) : 0,                        // texcoord_index_stride
		kFaceVert.data(),                                        // num_face_vertices
		kFaceVert.size(),                                        // num_faces
		&kShape)); // out_mesh

	gc.Add(kShape);

	// Set the shape to MeshData
	Mesh.RadeonShape = kShape;
	// Attach the shape to the scene
	CHECK(rprSceneAttachShape(kObjectManager.GetScene(), kShape));
	spdlog::info("Radeon mesh created");
	HorusConsole::GetInstance().AddLog(" [info] Radeon mesh created");

	return true;
}

int HorusMeshImporterUtility::GetNumberOfTextureCoords(const aiMesh* Mesh)
{
	HorusConsole& kConsole = HorusConsole::GetInstance();

	int kTextureCoords = -1;

	for (int i = 0; i < AI_MAX_NUMBER_OF_TEXTURECOORDS; i++)
	{
		if (!Mesh->HasTextureCoords(i)) {
			break;
		}

		kTextureCoords = i + 1;
	}

	if (kTextureCoords <= -1)
	{
		spdlog::warn("Mesh does not contain any texture coordinates");
		kConsole.AddLog(" [warn] Mesh does not contain any texture coordinates");
	}

	return kTextureCoords;
}

