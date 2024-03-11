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
#include "hrs_vertex_data.h"
#include "hrs_mesh_data.h"
#include "hrs_model_data.h"
#include "hrs_om_id_manager.h"
#include "hrs_opengl_shape.h"

inline std::mutex gLoadMeshMutex;
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

std::vector<VertexData> HorusMeshImporter::AssembleOpenGLVertices(std::vector<glm::vec3>& Vertices, std::vector<glm::vec3>& Normals, std::vector<glm::vec3>& TexCoords)
{
	std::vector<VertexData> OpenGLVertices;

	for (int i = 0; i < Vertices.size(); ++i)
	{
		OpenGLVertices.push_back({ Vertices[i], Normals[i], TexCoords[i] });
	}

	return OpenGLVertices;
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
		return {};
	}

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
		return {};
	}

	std::vector<std::tuple<rpr_shape, std::string>> ShapesAndNames;
	for (unsigned int m = 0; m < Scene->mNumMeshes; ++m)
	{
		aiMesh* Mesh = Scene->mMeshes[m];

		const aiNode* pNode = FindNodeForMesh(Scene->mRootNode, Mesh, Scene);
		if (!pNode) {
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

			if (HasNormals) {
				aiVector3D Normal = Transform * Mesh->mNormals[i];
				Normals.emplace_back(Normal.x);
				Normals.emplace_back(Normal.y);
				Normals.emplace_back(Normal.z);
			}

			if (HasTexCoords) {
				aiVector3D TexCoord = Mesh->mTextureCoords[0][i];
				Texcoords.emplace_back(TexCoord.x);
				Texcoords.emplace_back(TexCoord.y);
			}

		}

		for (unsigned int I = 0; I < Mesh->mNumFaces; ++I)
		{
			std::lock_guard Guard(gLoadMeshMutex);

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
			if (HorusIdManager::GetInstance().ContainsName(MaterialName)) {

				//if (ObjectManager.MaterialExists(MaterialName)) {
				Material = ObjectManager.GetMaterialIdByName(MaterialName);
			}
			else {
				Material = ObjectManager.CreateMaterial(MaterialName);
			}

			ObjectManager.AssignMaterialDirectly(TShape, Material);
		}
	}

	spdlog::info("Mesh Radeon loaded: {}", MeshPath);
	Console.AddLog(" [success] Mesh Radeon loaded : %s", MeshPath.c_str());

	return ShapesAndNames;
}

std::vector<std::tuple<HorusOpenGlShape, std::string>> HorusMeshImporter::LoadOpenGlMesh(const std::string& MeshPath, std::vector<VertexData>& OpenGLVertices, std::vector<GLuint>& OpenGLIndices)
{
	HorusConsole& Console = HorusConsole::GetInstance();

	spdlog::info("Loading mesh: {}", MeshPath);
	Console.AddLog(" [info] Loading mesh : %s", MeshPath.c_str());

	if (MeshPath.empty())
	{
		spdlog::error("Mesh path is empty");
		Console.AddLog(" [error] Mesh path is empty");
		return {};
	}

	std::vector<std::tuple<HorusOpenGlShape, std::string>> OpenGlShapesAndNames;

	Assimp::Importer Importer;

	const aiScene* Scene = Importer.ReadFile(MeshPath, aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_GenNormals | aiProcess_GenSmoothNormals
		| aiProcess_ValidateDataStructure);

	if (!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode)
	{
		spdlog::error("Error with Assimp : {}", Importer.GetErrorString());
		Console.AddLog(" [error] Error with Assimp : %s", Importer.GetErrorString());
		return {};
	}

	for (unsigned int m = 0; m < Scene->mNumMeshes; ++m)
	{
		aiMesh* Mesh = Scene->mMeshes[m];

		const aiNode* pNode = FindNodeForMesh(Scene->mRootNode, Mesh, Scene);
		if (!pNode)
		{
			spdlog::warn("No corresponding node found for mesh {}", m);
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

		std::vector<GLuint> Vertices;
		std::vector<GLuint> Normals;
		std::vector<GLuint> Texcoords;
		std::vector<GLuint> VIndices;
		std::vector<GLuint> NIndices;
		std::vector<GLuint> TIndices;
		std::vector<GLuint> FaceVert;

		Vertices.reserve(Mesh->mNumVertices * 3);
		Normals.reserve(Mesh->mNumVertices * 3);
		Texcoords.reserve(Mesh->mNumVertices * 2);
		VIndices.reserve(Mesh->mNumFaces * 3);
		NIndices.reserve(Mesh->mNumFaces * 3);
		TIndices.reserve(Mesh->mNumFaces * 3);
		FaceVert.reserve(Mesh->mNumFaces);

		std::vector<std::vector<GLuint>> UdimTexcoords;
		for (int i = 0; i < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++i)
		{
			if (!Mesh->HasTextureCoords(i)) {
				break;
			}
			UdimTexcoords.emplace_back();
			UdimTexcoords.back().reserve(Mesh->mNumVertices * 2);
		}

		spdlog::info("Mesh has {} texture coordinate", UdimTexcoords.size());
		Console.AddLog(" [info] Mesh has %d texture coordinate", UdimTexcoords.size());

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

		for (unsigned int I = 0; I < Mesh->mNumFaces; ++I)
		{
			std::lock_guard Guard(gLoadMeshMutex);

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

		bool IsValidNorm = Mesh->HasNormals();
		bool IsValidTex = Mesh->HasTextureCoords(0);

		std::vector<VertexData> VerticesData;
		std::vector<GLuint> IndicesData;

		if (VerticesData.empty() && IndicesData.empty())
		{
			VerticesData.reserve(Mesh->mNumVertices);
			IndicesData.reserve(Mesh->mNumFaces * 3);
		}

		if (VerticesData.empty() && IndicesData.empty())
		{
			for (unsigned int i = 0; i < Mesh->mNumVertices; ++i)
			{
				VertexData Vertex;
				Vertex.Position = { Vertices[i * 3], Vertices[i * 3 + 1], Vertices[i * 3 + 2] };
				Vertex.Normal = { Normals[i * 3], Normals[i * 3 + 1], Normals[i * 3 + 2] };
				Vertex.TexCoords = { Texcoords[i * 2], Texcoords[i * 2 + 1] };
				VerticesData.push_back(Vertex);
			}

			for (unsigned int i = 0; i < Mesh->mNumFaces; ++i)
			{
				aiFace Face = Mesh->mFaces[i];
				for (unsigned int j = 0; j < Face.mNumIndices; ++j)
				{
					IndicesData.push_back(Face.mIndices[j]);
				}
			}
		}

		HorusOpenGlShape OpenGLShape;

		OpenGLShape.InitOpenGlMesh(VerticesData, IndicesData);
		OpenGlShapesAndNames.emplace_back(OpenGLShape, ShapeName);
	}

	spdlog::info("Mesh OpenGL loaded: {}", MeshPath);
	Console.AddLog(" [success] Mesh OpenGL loaded : %s", MeshPath.c_str());

	return OpenGlShapesAndNames;
}

HorusOpenGlShape HorusMeshImporter::LoadSingleOpenGlMesh(const std::string& MeshPath, HorusOpenGlShape& Shape)
{
	HorusConsole& kConsole = HorusConsole::GetInstance();

	spdlog::info("Loading mesh: {}", MeshPath);
	kConsole.AddLog(" [info] Loading mesh : %s", MeshPath.c_str());

	if (MeshPath.empty())
	{
		spdlog::error("Mesh path is empty");
		kConsole.AddLog(" [error] Mesh path is empty");
		return HorusOpenGlShape();
	}

	Assimp::Importer kImporter;
	const aiScene* kScene = kImporter.ReadFile(MeshPath, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_FlipUVs);

	// Check if the file is valid
	if (!kScene || kScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !kScene->mRootNode)
	{
		spdlog::error("Error with Assimp : {}", kImporter.GetErrorString());
		kConsole.AddLog(" [error] Error with Assimp : %s", kImporter.GetErrorString());
		return HorusOpenGlShape();
	}

	// Check if there are meshes in the file
	if (kScene->mNumMeshes <= 0) {
		spdlog::error("No meshes found in file");
		kConsole.AddLog(" [error] No meshes found in file");
		return HorusOpenGlShape();
	}

	aiMesh* kMesh = kScene->mMeshes[0];

	if (!kMesh->HasNormals() || !kMesh->HasTextureCoords(0))
	{
		spdlog::error("Mesh lacks necessary data (normals or texture coordinates).");
		kConsole.AddLog(" [error] Mesh lacks necessary data (normals or texture coordinates).");
		return HorusOpenGlShape();
	}

	std::vector<VertexData> kVertices;
	std::vector<unsigned int> kIndices;

	bool kHasNormals = kMesh->HasNormals();
	bool kHasTexCoords = kMesh->HasTextureCoords(0);

	const aiNode* kPNode = FindNodeForMesh(kScene->mRootNode, kMesh, kScene);
	if (!kPNode) {
		spdlog::error("No corresponding node found for mesh");
		kConsole.AddLog(" [error] No corresponding node found for mesh");
		return HorusOpenGlShape();
	}

	aiMatrix4x4 kTransform = CalculateGlobalTransform(kPNode);

	for (unsigned int i = 0; i < kMesh->mNumVertices; ++i) 
	{
		aiVector3D kPos = kTransform * kMesh->mVertices[i];
		VertexData kVertex;
		kVertex.Position = { kPos.x, kPos.y, kPos.z };

		if (kHasNormals) {
			aiVector3D kNormal = kTransform * kMesh->mNormals[i];
			kVertex.Normal = { kNormal.x, kNormal.y, kNormal.z };
		}

		if (kHasTexCoords) {
			aiVector3D kTexCoord = kMesh->mTextureCoords[0][i];
			kVertex.TexCoords = { kTexCoord.x, kTexCoord.y };
		}

		kVertices.push_back(kVertex);
	}

	for (unsigned int i = 0; i < kMesh->mNumFaces; ++i) {
		aiFace kFace = kMesh->mFaces[i];
		for (unsigned int j = 0; j < kFace.mNumIndices; ++j) {
			kIndices.push_back(kFace.mIndices[j]);
		}
	}

	if (kVertices.empty() || kIndices.empty()) {
		spdlog::error("Mesh data is empty");
		kConsole.AddLog(" [error] Mesh data is empty");
		return HorusOpenGlShape();
	}

	if (Shape.InitOpenGlMesh(kVertices, kIndices))
	{
		spdlog::info("Mesh OpenGL loaded: {}", MeshPath);
		kConsole.AddLog(" [success] Mesh OpenGL loaded : %s", MeshPath.c_str());

		spdlog::info("Mesh Vertex count: {}", kVertices.size());
		spdlog::info("Mesh Index count: {}", kIndices.size());

		return Shape;
	}

	spdlog::error("Failed to load mesh");
	kConsole.AddLog(" [error] Failed to load mesh");
	return HorusOpenGlShape();
}


void HorusMeshImporter::LoadAndConvertOBJToText(const std::string& MeshPath) {
	std::vector<float> vertices;
	std::vector<float> texCoords;
	std::vector<unsigned int> indices;
	std::string line;

	std::ifstream myfile(MeshPath);
	if (!myfile.is_open()) {
		std::cerr << "Unable to open file" << std::endl;
		return;
	}

	while (getline(myfile, line)) {
		std::istringstream iss(line);
		std::string prefix;
		iss >> prefix;
		if (prefix == "v") {
			float x, y, z;
			iss >> x >> y >> z;
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);
		}
		else if (prefix == "vt") {
			float u, v;
			iss >> u >> v;
			texCoords.push_back(u);
			texCoords.push_back(v);
		}
		else if (prefix == "f") {
			std::string vertex1, vertex2, vertex3;
			iss >> vertex1 >> vertex2 >> vertex3;
			// Extrait les indices de vertex pour chaque face (simplifié, assume une face triangulaire)
			std::istringstream(vertex1) >> vertex1; // Réutilisation de vertex1 pour extraire juste l'indice du sommet
			std::istringstream(vertex2) >> vertex2;
			std::istringstream(vertex3) >> vertex3;
			indices.push_back(std::stoul(vertex1.substr(0, vertex1.find('/'))) - 1);
			indices.push_back(std::stoul(vertex2.substr(0, vertex2.find('/'))) - 1);
			indices.push_back(std::stoul(vertex3.substr(0, vertex3.find('/'))) - 1);
		}
	}
	myfile.close();

	// Format and write to file
	std::ofstream outputFile("mesh_data.txt");
	outputFile << std::fixed << std::setprecision(6);
	outputFile << "float meshVertices[] = {" << std::endl;
	for (size_t i = 0, j = 0; i < vertices.size(); i += 3, j += 2) {
		outputFile << " " << vertices[i] << "f, " << vertices[i + 1] << "f, " << vertices[i + 2] << "f, ";
		if (j < texCoords.size()) {
			outputFile << texCoords[j] << "f, " << texCoords[j + 1] << "f," << std::endl;
		}
		else {
			outputFile << "0.0f, 0.0f," << std::endl; // Fallback pour texCoords manquants
		}
	}
	outputFile << "};" << std::endl;

	outputFile << "unsigned int meshIndices[] = {" << std::endl;
	for (size_t i = 0; i < indices.size(); ++i) {
		outputFile << " " << indices[i] << "u";
		if (i < indices.size() - 1) outputFile << ", ";
		outputFile << std::endl;
	}
	outputFile << "};" << std::endl;

	outputFile.close();
	std::cout << "Mesh data including vertices, texcoords, and indices has been written to mesh_data.txt in the desired format." << std::endl;
}
