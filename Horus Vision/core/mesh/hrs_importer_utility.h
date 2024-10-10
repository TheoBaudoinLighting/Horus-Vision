#pragma once

// Radeon ProRender
#include "RadeonProRender_v2.h"

// C++ standard library
#include <vector>

// assimp
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/DefaultLogger.hpp>

// Horus
#include "hrs_preprocess_meshdata.h"
#include "hrs_mesh.h"
#include "hrs_model_data.h"
#include "hrs_mesh_data.h"
#include "hrs_vertex_data.h"
#include "hrs_opengl_shape.h"

class HorusMeshImporterUtility
{
public:

	static HorusMeshImporterUtility& GetInstance()
	{
		static HorusMeshImporterUtility Instance;
		return Instance;
	}

	HorusMeshImporterUtility(HorusMeshImporterUtility const&) = delete;
	void operator=(HorusMeshImporterUtility const&) = delete;

	static ModelData GenerateModelData(PreProcessMeshData& Data);
	static MeshData GetMeshFromModelData(const ModelData& Model, const std::string& MeshName);

	static const aiScene* ReadWithAssimp(const std::string& Path);

	// TODO : Implement these functions later for an alternative to Assimp
	//static void ReadWithTinyObjLoader(const std::string& Path);

	static const aiNode* FindNode(const aiNode* Node, const aiMesh* Mesh, const aiScene* Scene);
	static aiMatrix4x4 ProcessGlobaltransform(const aiNode* Node);
	static bool ProcessVertices(const aiMesh* Mesh, const aiMatrix4x4& Transform, std::vector<VertexData>& Vertices, bool kHasNormals, bool HasTangentsAndBitangents, bool kHasTexCoords);
	static bool ProcessMaterialIDs(const aiMesh* Mesh, std::vector<unsigned int>& MaterialIDs);
	static bool ProcessIndices(const aiMesh* Mesh, std::vector<unsigned int>& Indices);
	static void ProcessFace(const aiMesh* Mesh, const aiFace& Face, std::vector<unsigned int>& Indices);

	// Check functions
	static bool HasNormals(const aiMesh* Mesh);
	static bool HasTexCoords(const aiMesh* Mesh);
	static bool HasTangentsAndBitangents(const aiMesh* Mesh);
	static bool HasFaces(const aiMesh* Mesh);
	static bool HasBones(const aiMesh* Mesh);

	static bool IsPathValid(const std::string& Path);
	static bool IsSceneValid(const aiScene* Scene);

	static bool IsSceneContainsMesh(const aiScene* Scene);
	static int GetNumberOfTextureCoords(const aiMesh* Mesh);
	static bool IsMeshValid(const aiMesh* Mesh);

	// Create functions
	static bool CreateOpenGlMesh(MeshData& Mesh);
	static bool CreateRadeonMesh(MeshData& Mesh);
	
private:

	HorusMeshImporterUtility() = default;
};

inline void ProcessFace(const aiMesh* Mesh, const aiFace& face, std::vector<unsigned int>& kIndices)
{
	for (unsigned int i = 0; i < face.mNumIndices; i++)
	{
		kIndices.push_back(face.mIndices[i]);
	}
}





