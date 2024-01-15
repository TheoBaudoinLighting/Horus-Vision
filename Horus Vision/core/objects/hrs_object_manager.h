#pragma once


#include "hrs_scene.h" // nothing
#include "hrs_camera.h" // nothing
#include "hrs_importer.h" // nothing
#include "hrs_mesh.h" // nothing
#include "hrs_light.h" // nothing
#include "hrs_material.h" // nothing
#include "hrs_material_editor.h" // nothing
#include "hrs_radeon.h" // glfw3.h

#include <string>
#include <variant>
#include <queue>
#include <map>

class IDManager 
{
public:
	static IDManager& getInstance() 
	{
		static IDManager instance;
		return instance;
	}

	IDManager(const IDManager&) = delete;
	IDManager& operator=(const IDManager&) = delete;

	int getNewID() 
	{
		if (!availableIDs.empty()) 
		{
			int id = availableIDs.front();
			availableIDs.pop();
			return id;
		}
		return nextID++;
	}

	void releaseID(int id) 
	{
		availableIDs.push(id);
	}

private:
	IDManager() : nextID(0) {}

	int nextID;
	std::queue<int> availableIDs;
};



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

	int GetIdByName(const std::string& name)
	{
		auto it = objectNameToIdMap.find(name);
		if (it != objectNameToIdMap.end())
		{
			return it->second;
		}

		return -1;
	}

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

	void get_outliner_lights(std::vector<std::string>& lights)
	{
		for (auto& light : m_lights_)
		{
			int id = light.first;
			std::string name = m_light_names_[id];
			lights.push_back(name);
		}
	}

	// ----------------------------------------------
	// Camera object ----------------------------------

	int create_camera(int SceneID, const std::string name);

	void destroy_camera(int id);

	void destroy_all_cameras()
	{
		for (auto& camera : m_cameras_)
		{
			camera.second.Destroy();
		}

		m_cameras_.clear();
	}

	HorusCamera& get_camera(int id)
	{
		return m_cameras_[id];
	}

	void bind_camera(int id);
	void unbind_camera(int id);

	void UpdateCamera(int id);
	void GetMatrices(int id, glm::mat4& projection, glm::mat4& view, glm::mat4& model);

	void PrintCameraInfo(int id);
	void ResetCamera(int id);

	void SetViewport(int id, int x, int y, int width, int height);

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

	void SetPitch(int id, float pitch);
	void SetHeading(int id, float heading);

	void set_camera_lookat(int id, RadeonProRender::float3& pivot);

	void TumbleCamera(int id, float x, float y);
	void PanCamera(int id, float x, float y);
	void ZoomCamera(int id, float distance);

	void compute_view_projection_matrix(int id, float* view, float* projection, float ratio);

	// Info for inspector
	glm::vec3 GetCameraLookAt(int id);
	glm::vec3 GetCameraPosition(int id);
	glm::vec3 GetCameraRotation(int id);
	glm::vec3 GetCameraScale(int id);

	void CameraSetPos(int id,int button, int state, int x, int y);

	void set_camera_fov(int id, float fov);
	void set_camera_aspect_ratio(int id, float aspect_ratio);
	void set_camera_near_plane(int id, float near_plane);
	void set_camera_far_plane(int id, float far_plane);

	void set_enable_dof(int id, bool enable);
	void set_dof_aperture(int id, float aperture);
	void set_dof_focal_distance(int id, float focal_distance);


	// ----------------------------------------------
	// Mesh object ----------------------------------

	int create_mesh(const char* path,const std::string& name);
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

	int create_material(std::string name)
	{
		std::string material_name = name;

		int suffix = 0;

		while (objectNameToIdMap.find(material_name) != objectNameToIdMap.end())
		{
			material_name = name + "_" + std::to_string(suffix);
			suffix++;

			spdlog::info("Material with name {} already exists", name);
		}

		int id = IDManager::getInstance().getNewID();

		HorusMaterial new_material;
		new_material.init();
		m_materials_[id] = new_material;
		m_material_names_[id] = material_name;
		objectNameToIdMap[material_name] = id;

		spdlog::info("Creating {} material with id {}", material_name, id);

		return id;
	}

	void destroy_material(int id)
	{
		auto it = m_materials_.find(id);
		if (it != m_materials_.end())
		{
			it->second.destroy_material();
			m_materials_.erase(it);
			m_material_names_.erase(id);
		}
		else
		{
			spdlog::error("Material with id {} does not exist", id);
		}

		for (auto it = objectNameToIdMap.begin(); it != objectNameToIdMap.end(); ++it)
		{
			if (it->second == id)
			{
				objectNameToIdMap.erase(it);
				break;
			}
		}

		IDManager::getInstance().releaseID(id);
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

	void set_reflection_mode(int id, int mode);
	void set_coating_mode(int id, int mode);

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

	int create_light(const std::string& name, const std::string& light_type, const std::string& hdri_image = "");

	void destroy_light(int id);

	void destroy_all_lights();

	void set_light_position(int id, RadeonProRender::float3& position);
	void set_light_rotation(int id, RadeonProRender::float3& rotation);
	void set_light_scale(int id, RadeonProRender::float3& scale);

	void set_light_intensity(int id, RadeonProRender::float3& intensity);

	// ----------------------------------------------
	// Scene object ----------------------------------

	int create_scene(const std::string& name);
	void set_scene(int id);
	rpr_scene get_scene();
	int get_scene_id_by_name(const char* name);
	int GetActiveSceneId();
	std::string& get_scene_name_by_id(int id);

	void destroy_scene(int id);
	void destroy_all_scenes();

	int CreateDefaultScene();

	void show_dummy_dragon();
	void show_dummy_plane();
	void show_LookdevScene();

	// ----------------------------------------------

private:

	HorusObjectManager() : m_background_material_() {}

	std::map<std::string, int> objectNameToIdMap;
	std::string name;

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
