#pragma once

#include <RadeonProRender_v2.h>
#include <vector>
#include <assimp/scene.h>

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

	// Import mesh with assimp
	rpr_shape LoadMesh(const std::string& pFile);

private:

	HorusMeshImporter(): m_NumFaces_(0), m_Shape_(nullptr), m_ImportedShape_(nullptr)
	{
	}

	std::vector<aiMesh*> m_Meshes_;
	std::vector<float> m_Vertices_;
	std::vector<float> m_Normals_;
	std::vector<rpr_int> m_Indices_;  
	std::vector<float> m_Texcoords_;  
	std::vector<rpr_int> m_VertexIndices_;
	std::vector<rpr_int> m_NormalIndices_;
	std::vector<rpr_int> m_TexcoordIndices_;  
	std::vector<rpr_int> m_NumFaceVertices_; 
	size_t m_NumFaces_; 

	std::vector<std::vector<rpr_float>> m_AllTexCoords_;
	std::vector<rpr_float> m_UdimTexCoords_;

	rpr_shape m_Shape_;
	rpr_shape m_ImportedShape_;
};
