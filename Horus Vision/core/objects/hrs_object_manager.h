#pragma once

#include "hrs_radeon.h"
#include "hrs_scene.h"
#include "hrs_camera.h"
#include "hrs_material.h"
#include "hrs_importer.h"
#include "hrs_mesh.h"
#include "hrs_light.h"

#include <string>
#include <variant>

#include "hrs_material_editor.h"

class HorusObjectManager
{
public:

	static HorusObjectManager& get_instance()
	{
		static HorusObjectManager instance;
		return instance;
	}

	HorusObjectManager(HorusObjectManager const&) = delete;
	void operator=(HorusObjectManager const&) = delete;

	// Other -----------------------------------------

	void set_background_image(const std::string& path);

	void unset_background_image();

	// ----------------------------------------------
	// Outliner -------------------------------------

	void get_outliner_meshes(std::vector<std::string>& meshes)
	{
		for (auto& mesh : m_meshes_)
		{
			int id = mesh.first;
			std::string name = m_mesh_names_[id];
			meshes.push_back(name);
		}
	}

	void get_outliner_materials(std::vector<std::string>& materials)
	{
		for (auto& material : m_materials_)
		{
			int id = material.first;
			std::string name = m_material_names_[id];
			materials.push_back(name);
		}
	}

	void get_outliner_cameras(std::vector<std::string>& cameras)
	{
		for (auto& camera : m_cameras_)
		{
			int id = camera.first;
			std::string name = m_camera_names_[id];
			cameras.push_back(name);
		}
	}

	// ----------------------------------------------
	// Camera object ----------------------------------

	bool create_camera_with_id(int id, const std::string name)
	{
		if (m_cameras_.find(id) != m_cameras_.end())
		{
			return false;
		}

		HorusCamera new_camera;
		new_camera.init();
		m_cameras_[id] = new_camera;
		m_camera_names_[id] = name;

		return true;
	}

	void destroy_camera(int id)
	{
		auto it = m_cameras_.find(id);

		if (it != m_cameras_.end())
		{
			it->second.destroy();
			m_cameras_.erase(it);
		}
	}

	void destroy_all_cameras()
	{
		for (auto& camera : m_cameras_)
		{
			camera.second.destroy();
		}

		m_cameras_.clear();
	}

	HorusCamera& get_camera(int id)
	{
		return m_cameras_[id];
	}

	void bind_camera(int id);
	void unbind_camera(int id);

	int get_active_camera_id();
	void set_active_camera(int id);

	void get_camera_id_by_name(const char* name);
	void get_camera_name_by_id(int id);

	void move_camera_forward(int id);
	void move_camera_backward(int id);
	void move_camera_left(int id);
	void move_camera_right(int id);
	void move_camera_up(int id);
	void move_camera_down(int id);
	void scroll_camera(int id, float delta);

	void set_camera_lookat(int id, RadeonProRender::float3& pivot);

	void tumble_camera(int id, float x, float y);
	void track_camera(int id, float x, float y);
	void dolly_camera(int id, float distance);

	void compute_view_projection_matrix(int id, float* view, float* projection, float ratio);

	// ----------------------------------------------
	// Mesh object ----------------------------------

	bool create_mesh(const char* path, const char* name, int id);
	void destroy_mesh(int id);

	HorusMesh& get_mesh(int id /*, HorusMesh* mesh*/);
	rpr_shape get_mesh_shape(int id);
	void get_mesh_count(int* count);
	void get_mesh_name(int id);
	void get_mesh_id(const char* name);
	void get_mesh_id_by_index(int index, int* id);
	void get_mesh_index_by_id(int id, int* index);
	void get_mesh_index_by_name(const char* name);

	RadeonProRender::matrix get_mesh_transform(int id);

	void destroy_all_meshes();

	void set_mesh_name(int id, const char* name);
	void set_mesh_id(int id, int new_id);
	void set_mesh_index(int id, int index);

	void set_shape_position(int id, RadeonProRender::float3 pos);
	void set_shape_rotation(int id, RadeonProRender::float3 rotate_axis, float rotation_angle);
	void set_shape_scale(int id, RadeonProRender::float3 scale);

	// ----------------------------------------------
	// Material object ------------------------------

	HorusMaterial& create_material(int id, std::string name)
	{
		if (m_materials_.find(id) != m_materials_.end())
		{
			spdlog::error("Material with id {} already exists", id);

			return m_materials_[id];
		}

		spdlog::info("Creating material with id {}", id);

		HorusMaterial new_material;
		new_material.init();
		m_materials_[id] = new_material;
		m_material_names_[id] = name;

		m_active_camera_id = id;

		return m_materials_[id];
	}

	

	void destroy_material(int id)
	{
		auto it = m_materials_.find(id);

		if (it != m_materials_.end())
		{
			it->second.destroy_material();
			m_materials_.erase(it);
		}
	}

	void destroy_all_material();

	void assign_material(int mesh_id, int mat_id);

	void set_base_color(int id, const std::string& texturePath);
	void set_base_color(int id, const std::array<float, 3>& color);

	void set_metallic(int id, const std::string& texturePath);
	void set_metallic(int id, const std::array<float, 3>& color);

	void set_roughness(int id, const std::string& texturePath);
	void set_roughness(int id, const std::array<float, 3>& color);

	void set_normal(int id, const std::string& texturePath);
	void set_normal(int id, const std::array<float, 3>& color);

	void set_opacity(int id, const std::string& texturePath);
	void set_opacity(int id, const std::array<float, 3>& color);

	void set_emissive(int id, const std::string& texturePath);
	void set_emissive(int id, const std::array<float, 3>& color);

	void set_reflection_color(int id, const std::array<float, 3>& color);
	void set_reflection_color(int id, const std::string& texturePath);

	void set_reflection_weight(int id, const std::array<float, 3>& color);
	void set_reflection_weight(int id, const std::string& texturePath);

	void set_reflection_roughness(int id, const std::array<float, 3>& color);
	void set_reflection_roughness(int id, const std::string& texturePath);

	void set_refraction_color(int id, const std::array<float, 3>& color);
	void set_refraction_color(int id, const std::string& texturePath);

	void set_coating_color(int id, const std::array<float, 3>& color);
	void set_coating_color(int id, const std::string& texturePath);

	void set_sheen(int id, const std::array<float, 3>& color);
	void set_sheen(int id, const std::string& texturePath);

	void set_sss_scatter_color(int id, const std::array<float, 3>& color);
	void set_sss_scatter_color(int id, const std::string& texturePath);

	void set_backscatter_color(int id, const std::array<float, 3>& color);
	void set_backscatter_color(int id, const std::string& texturePath);

	void set_ior(int id, float ior);

	void set_refraction_weight(int id, const std::array<float, 3>& weight);
	void set_refraction_weight(int id, const std::string& texturePath);

	void set_refraction_roughness(int id, const std::array<float, 3>& roughness);
	void set_refraction_roughness(int id, const std::string& texturePath);

	void set_coating_weight(int id, const std::array<float, 3>& weight);
	void set_coating_weight(int id, const std::string& texturePath);

	void set_coating_roughness(int id, const std::array<float, 3>& roughness);
	void set_coating_roughness(int id, const std::string& texturePath);

	void set_sheen_weight(int id, const std::array<float, 3>& weight);
	void set_sheen_weight(int id, const std::string& texturePath);

	void set_backscatter_weight(int id, const std::array<float, 3>& weight);
	void set_backscatter_weight(int id, const std::string& texturePath);

	void set_diffuse_weight(int id, const std::array<float, 3>& color);
	void set_diffuse_weight(int id, const std::string& texturePath);

	void set_emission_weight(int id, const std::array<float, 3>& color);
	void set_emission_weight(int id, const std::string& texturePath);

	void set_transparency(int id, const std::array<float, 3>& color);
	void set_transparency(int id, const std::string& texturePath);

	void set_sss_scatter_distance(int id, const std::array<float, 3>& color);
	void set_sss_scatter_distance(int id, const std::string& texturePath);

	// ----------------------------------------------
	// Material Editor object --------------------------

	HorusMaterialEditor& create_material_editor_node(int id, std::string name)
	{
		if (m_material_editors_.find(id) != m_material_editors_.end())
		{
			spdlog::error("Material Editor with id {} already exists", id);

			return m_material_editors_[id];
		}

		spdlog::info("Creating material graph with id {}", id);

		HorusMaterialEditor new_material_editor;
		new_material_editor.init();

		m_material_editors_[id] = new_material_editor;
		m_material_editor_names_[id] = name;
		m_material_editor_mesh_to_set_material_[id] = id;

		return m_material_editors_[id];
	}

	void assign_material_editor_node(int mesh_id, int mat_id);

	void set_material_from_editor_node(int id, rpr_material_node mat);

	int set_material_editor_mesh_to_set_material(int id)
	{
		m_material_editor_mesh_to_set_material_[m_material_editor_to_show_] = id;

		return id;
	}

	int get_mesh_id_to_set_material()
	{
		return m_material_editor_mesh_to_set_material_[m_material_editor_to_show_];
	}

	void show_material_editor(int id);

	void open_material_editor(int id);

	void close_material_editor(int id);

	void open_material_editor_browser();

	void close_material_editor_browser();

	void get_material_editor_materials(std::vector<std::string>& materials)
	{
		for (auto& material : m_material_editors_)
		{
			int id = material.first;
			std::string name = m_material_editor_names_[id];
			materials.push_back(name);
		}
	}

	int get_material_editor_id_by_name(const char* name)
	{
		for (auto& material : m_material_editors_)
		{
			int id = material.first; 
			std::string material_name = m_material_editor_names_[id];

			if (material_name == name)
			{
				return id;
			}
		}

		return -1;
	}

	int set_material_editor_to_show(int id);

	int get_material_editor_to_show();

	void open_material_editor_create_menu();

	void close_material_editor_create_menu();

	void quit_material_editor(int id);

	void destroy_all_material_editors();

	// ----------------------------------------------
	// Light object ---------------------------------

	HorusLight& create_light(int id, const std::string& name, const std::string& light_type, const std::string& hdri_image = "");

	void destroy_light(int id);

	void destroy_all_lights();

	void set_light_position(int id, RadeonProRender::float3& position);
	void set_light_rotation(int id, RadeonProRender::float3& rotation);
	void set_light_scale(int id, RadeonProRender::float3& scale);

	void set_light_intensity(int id, RadeonProRender::float3& intensity);

	// ----------------------------------------------
	// Scene object ----------------------------------

	void create_scene(int id, const std::string& name);
	rpr_scene get_scene();

	void destroy_scene(int id);
	void destroy_all_scenes();

	void show_dummy_dragon();
	void show_dummy_plane();

	// ----------------------------------------------

private:

	HorusObjectManager() : m_background_material_() {}

	// Camera object ----------------

	std::map<int, HorusCamera> m_cameras_;
	int m_active_camera_id = 0;
	int m_next_camera_id_ = 0;
	std::map<int, std::string> m_camera_names_;

	// ------------------------------
	// Mesh object ------------------

	std::map<int, HorusMesh> m_meshes_;
	int m_mesh_index_ = 0;
	int m_mesh_count_ = 0;
	std::map<int, std::string> m_mesh_names_;

	// ------------------------------
	// Material object --------------

	std::map<int, HorusMaterial> m_materials_;
	std::unordered_map<int, HorusMaterialParameters> temp_mat_params_;
	int m_material_index_ = 0;
	int m_material_count_ = 0;
	std::map<int, std::string> m_material_names_;

	// ------------------------------
	// Material Editor object -------

	std::map<int, HorusMaterialEditor> m_material_editors_;
	int m_material_editor_index_ = 0;
	int m_material_editor_count_ = 0;
	std::map<int, std::string> m_material_editor_names_;
	std::map<int, int> m_material_editor_mesh_to_set_material_;

	int m_material_editor_to_show_ = 0;

	// ------------------------------
	// background image

	HorusMaterial m_background_material_;

	// ------------------------------
	// Light object -----------------

	std::map<int, HorusLight> m_lights_;
	int m_light_index_ = 0;
	int m_light_count_ = 0;
	std::map<int, std::string> m_light_names_;
	// ------------------------------
	// Scene object -----------------

	std::map<int, HorusScene> m_scenes_;
	int m_scene_index_ = 0;
	int m_scene_count_ = 0;
	int m_active_scene_id_ = 0;
	std::map<int, std::string> m_scene_names_;

	// ------------------------------
};
