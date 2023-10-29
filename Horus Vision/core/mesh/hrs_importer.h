#pragma once

#include <RadeonProRender_v2.h>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


class HorusMeshImporter
{
public:

	// Singleton HorusMeshImporter

	static HorusMeshImporter& get_instance()
	{
		static HorusMeshImporter instance;
		return instance;
	}

	HorusMeshImporter(HorusMeshImporter const&) = delete;
	void operator=(HorusMeshImporter const&) = delete;

	//-------------------------------------------------------------

	// Import mesh with assimp
	rpr_shape load_mesh(const std::string& pFile);

private:

	HorusMeshImporter()	{}

	void process_mesh(aiMesh* mesh);

	std::vector<aiMesh*> m_meshes_;
	std::vector<float> m_vertices_;
	std::vector<float> m_normals_;
	std::vector<rpr_int> m_indices_;  
	std::vector<float> m_texcoords_;  
	std::vector<rpr_int> m_vertex_indices_;
	std::vector<rpr_int> m_normal_indices_;
	std::vector<rpr_int> m_texcoord_indices_;  
	std::vector<rpr_int> m_num_face_vertices_; 
	size_t m_num_faces; 

	std::vector<std::vector<rpr_float>> all_tex_coords_;
	std::vector<rpr_float> udim_tex_coords_;

	rpr_shape m_shape_;
	rpr_shape m_imported_shape_;
};
