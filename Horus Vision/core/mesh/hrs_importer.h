#pragma once

#include <RadeonProRender.hpp>
#include <thread>
#include <vector>
#include <assimp/scene.h>

#include "hrs_importer_utility.h"
#include "hrs_mesh.h"
#include "hrs_opengl_shape.h"
#include "Math/mathutils.h"

class HorusMeshImporter
{
public:

	// Singleton HorusMeshImporter

	static HorusMeshImporter& GetInstance()
	{
		static HorusMeshImporter Instance;
		return Instance;
	}

	HorusMeshImporter(HorusMeshImporter const&) = delete;
	void operator=(HorusMeshImporter const&) = delete;

	//-------------------------------------------------------------

	// Utility functions
	std::vector<VertexData> AssembleOpenGLVertices(std::vector<glm::vec3>& Vertices, std::vector<glm::vec3>& Normals, std::vector<glm::vec3>& TexCoords);


	// Import mesh with assimp
	std::vector<std::tuple<rpr_shape, std::string>> LoadMesh(const std::string& MeshPath);
	std::vector<std::tuple<HorusOpenGlShape, std::string>> LoadOpenGlMesh(const std::string& MeshPath, std::vector<VertexData>& OpenGLVertices, std::vector<GLuint>& OpenGLIndices);
	HorusOpenGlShape LoadSingleOpenGlMesh(const std::string& MeshPath, HorusOpenGlShape& Shape);
	
	void LoadAndConvertOBJToText(const std::string& MeshPath);

	//rpr_shape LoadMeshTOL(const std::string& MeshPath);

private:

	HorusMeshImporter(): m_NumFaces_(0), m_Shape_(nullptr), m_ImportedShape_(nullptr)
	{
	}

	std::string m_Name_;

	std::vector<float> m_Vertices_;
	std::vector<float> m_Normals_;
	std::vector<rpr_int> m_Indices_;  
	std::vector<float> m_Texcoords_;  
	std::vector<rpr_int> m_VertexIndices_;
	std::vector<rpr_int> m_NormalIndices_;
	std::vector<rpr_int> m_TexcoordIndices_;  
	std::vector<rpr_int> m_NumFaceVertices_;

	std::vector<RadeonProRender::float4> m_VerticesUVX_ ;
	std::vector<RadeonProRender::float4> m_NormalsUVX_ ;

	size_t m_NumFaces_; 
	std::vector<std::vector<rpr_float>> m_AllTexCoords_;
	std::vector<rpr_float> m_UdimTexCoords_;

	std::thread m_LoadMeshThread_;

	rpr_shape m_Shape_;
	rpr_shape m_ImportedShape_;
};
