#include "hrs_object_manager.h"

#include <Math/matrix.h>

#include "spdlog/spdlog.h"

//HorusRadeon& radeon_ImpM = HorusRadeon::get_instance();
//HorusMaterialEditorBrowser& material_editor_browser_ImpM = HorusMaterialEditorBrowser::get_instance();
//HorusMaterialEditorCreateMenu& material_editor_create_menu_ImpM = HorusMaterialEditorCreateMenu::get_instance();

// Camera management ------------------------------------------------------------------------------

void HorusObjectManager::set_background_image(const std::string& path)
{
	m_background_material_.set_background_image(path);
}

void HorusObjectManager::unset_background_image()
{
	m_background_material_.unset_background_image();
}

void HorusObjectManager::bind_camera(int id)
{
	m_cameras_[id].bind();
}

void HorusObjectManager::unbind_camera(int id)
{
	m_cameras_[id].unbind();
}

int HorusObjectManager::get_active_camera_id()
{
	return m_active_camera_id;
}

void HorusObjectManager::set_active_camera(int id)
{
	if (m_cameras_.find(id) == m_cameras_.end())
	{
		spdlog::error("no camera with this id exists. ");
		throw std::runtime_error("no camera with this id exists. ");
	}

	m_active_camera_id = id;
}

void HorusObjectManager::get_camera_id_by_name(const char* name)
{

}

void HorusObjectManager::get_camera_name_by_id(int id)
{


}

void HorusObjectManager::move_camera_forward(int id)
{
	HorusCamera camera = get_camera(id);
	camera.move_forward();
}

void HorusObjectManager::move_camera_backward(int id)
{
	HorusCamera camera = get_camera(id);
	camera.move_backward();
}

void HorusObjectManager::move_camera_left(int id)
{
	HorusCamera camera = get_camera(id);
	camera.move_left();
}

void HorusObjectManager::move_camera_right(int id)
{
	HorusCamera camera = get_camera(id);
	camera.move_right();
}

void HorusObjectManager::move_camera_up(int id)
{
	HorusCamera camera = get_camera(id);
	camera.move_up();
}

void HorusObjectManager::move_camera_down(int id)
{
	HorusCamera camera = get_camera(id);
	camera.move_down();
}

void HorusObjectManager::scroll_camera(int id, float delta)
{
	HorusCamera camera = get_camera(id);
	camera.scrolling(delta);
}

void HorusObjectManager::tumble_camera(int id, float x, float y)
{
	HorusCamera camera = get_camera(id);
	camera.tumbling(x, y);
}

void HorusObjectManager::track_camera(int id, float x, float y)
{
	HorusCamera camera = get_camera(id);
	camera.track(x, y);
}

void HorusObjectManager::dolly_camera(int id, float distance)
{
	HorusCamera camera = get_camera(id);
	camera.dolly(distance);
}

void HorusObjectManager::compute_view_projection_matrix(int id, float* view, float* projection, float ratio)
{
	HorusCamera camera = get_camera(id);
	camera.compute_view_projection_matrix(view, projection, ratio);
}

void HorusObjectManager::set_camera_lookat(int id, RadeonProRender::float3& pivot)
{
	HorusCamera camera = get_camera(id);
	camera.set_lookat(pivot);
}

// -----------------------------------------------------------------------------------------------
// Mesh management -------------------------------------------------------------------------------

bool HorusObjectManager::create_mesh(const char* path, const char* name, int id)
{
	if (m_meshes_.find(id) != m_meshes_.end())
	{
		return false;
	}

	HorusMesh new_mesh;
	new_mesh.init(path, name);
	m_meshes_[id] = new_mesh;
	m_mesh_names_[id] = name;


	return true;
}

void HorusObjectManager::destroy_mesh(int id)
{
	auto it = m_meshes_.find(id);

	if (it != m_meshes_.end())
	{
		m_meshes_[id].destroy_mesh();
	}
}

HorusMesh& HorusObjectManager::get_mesh(int id)
{
	return m_meshes_[id];
}

rpr_shape HorusObjectManager::get_mesh_shape(int id)
{
	if (m_meshes_.find(id) == m_meshes_.end())
	{
		spdlog::error("no mesh with this id exists. ");
		throw std::runtime_error("no mesh with this id exists. ");
	}

	return m_meshes_[id].get_shape();
}

void HorusObjectManager::get_mesh_count(int* count)
{
	*count = m_meshes_.size();
}

void HorusObjectManager::get_mesh_name(int id)
{

}

void HorusObjectManager::get_mesh_id(const char* name)
{

}

void HorusObjectManager::get_mesh_id_by_index(int index, int* id)
{

}

void HorusObjectManager::get_mesh_index_by_id(int id, int* index)
{
}

void HorusObjectManager::get_mesh_index_by_name(const char* name)
{
}

RadeonProRender::matrix HorusObjectManager::get_mesh_transform(int id)
{
	if (m_meshes_.find(id) == m_meshes_.end())
	{
		spdlog::error("no mesh with this id exists. ");
		throw std::runtime_error("no mesh with this id exists. ");
	}

	m_meshes_[id].get_transform();
}

void HorusObjectManager::destroy_all_meshes()
{
	for (auto& mesh : m_meshes_)
	{
		mesh.second.destroy_mesh();
	}
}

void HorusObjectManager::set_mesh_name(int id, const char* name)
{
}

void HorusObjectManager::set_mesh_id(int id, int new_id)
{
}

void HorusObjectManager::set_mesh_index(int id, int index)
{
}

void HorusObjectManager::set_shape_position(int id, RadeonProRender::float3 pos)
{
	if (m_meshes_.find(id) == m_meshes_.end())
	{
		spdlog::error("no mesh with this id exists. ");
		throw std::runtime_error("no mesh with this id exists. ");
	}

	m_meshes_[id].set_shape_position(pos);
}

void HorusObjectManager::set_shape_rotation(int id, RadeonProRender::float3 rotate_axis, float rotation_angle)
{
	if (m_meshes_.find(id) == m_meshes_.end())
	{
		spdlog::error("no mesh with this id exists. ");
		throw std::runtime_error("no mesh with this id exists. ");
	}

	m_meshes_[id].set_shape_rot(rotate_axis, rotation_angle);
}

void HorusObjectManager::set_shape_scale(int id, RadeonProRender::float3 scale)
{
	if (m_meshes_.find(id) == m_meshes_.end())
	{
		spdlog::error("no mesh with this id exists. ");
		throw std::runtime_error("no mesh with this id exists. ");
	}

	m_meshes_[id].set_shape_scale(scale);
}

// -----------------------------------------------------------------------------------------------
// Material management ---------------------------------------------------------------------------

void HorusObjectManager::destroy_all_material()
{
	for (auto& material : m_materials_)
	{
		material.second.destroy_material();
	}
}

void HorusObjectManager::assign_material(int mesh_id, int mat_id)
{
	if (m_meshes_.find(mesh_id) == m_meshes_.end())
	{
		spdlog::error("no mesh with this id exists. ");
		throw std::runtime_error("no mesh with this id exists. ");
	}

	if (m_materials_.find(mat_id) == m_materials_.end())
	{
		spdlog::error("no material with this id exists. ");
		throw std::runtime_error("no material with this id exists. ");
	}


	m_meshes_[mesh_id].assign_material(m_materials_[mat_id].get_material());
}

void HorusObjectManager::set_base_color(int id, const std::string& texturePath)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material with this id exists. ");
		throw std::runtime_error("no material with this id exists. ");
	}
	m_materials_[id].set_base_color(texturePath);
}

void HorusObjectManager::set_base_color(int id, const std::array<float, 3>& color)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material with this id exists. ");
		throw std::runtime_error("no material with this id exists. ");
	}
	m_materials_[id].set_base_color(color);
}

void HorusObjectManager::set_metallic(int id, const std::string& texturePath)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material with this id exists. ");
		throw std::runtime_error("no material with this id exists. ");
	}

	m_materials_[id].set_metallic(texturePath);
}

void HorusObjectManager::set_metallic(int id, const std::array<float, 3>& color)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material with this id exists. ");
		throw std::runtime_error("no material with this id exists. ");
	}

	m_materials_[id].set_metallic(color);
}

void HorusObjectManager::set_roughness(int id, const std::string& texturePath)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material with this id exists. ");
		throw std::runtime_error("no material with this id exists. ");
	}

	m_materials_[id].set_roughness(texturePath);
}

void HorusObjectManager::set_roughness(int id, const std::array<float, 3>& color)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material with this id exists. ");
		throw std::runtime_error("no material with this id exists. ");
	}

	m_materials_[id].set_roughness(color);
}

void HorusObjectManager::set_normal(int id, const std::string& texturePath)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material with this id exists. ");
		throw std::runtime_error("no material with this id exists. ");
	}

	m_materials_[id].set_normal(texturePath);
}

void HorusObjectManager::set_normal(int id, const std::array<float, 3>& color)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material with this id exists. ");
		throw std::runtime_error("no material with this id exists. ");
	}

	m_materials_[id].set_normal(color);
}

void HorusObjectManager::set_opacity(int id, const std::string& texturePath)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material with this id exists. ");
		throw std::runtime_error("no material with this id exists. ");
	}

	m_materials_[id].set_opacity(texturePath);
}

void HorusObjectManager::set_opacity(int id, const std::array<float, 3>& color)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material with this id exists. ");
		throw std::runtime_error("no material with this id exists. ");
	}

	m_materials_[id].set_opacity(color);
}

void HorusObjectManager::set_emissive(int id, const std::string& texturePath)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material with this id exists. ");
		throw std::runtime_error("no material with this id exists. ");
	}

	m_materials_[id].set_emissive(texturePath);
}

void HorusObjectManager::set_emissive(int id, const std::array<float, 3>& color)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material with this id exists. ");
		throw std::runtime_error("no material with this id exists. ");
	}

	m_materials_[id].set_emissive(color);
}

void HorusObjectManager::set_reflection_color(int id, const std::array<float, 3>& color)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material with this id exists. ");
		throw std::runtime_error("no material with this id exists. ");
	}

	m_materials_[id].set_reflection_color(color);
}

void HorusObjectManager::set_reflection_color(int id, const std::string& texturePath)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material with this id exists. ");
		throw std::runtime_error("no material with this id exists.");
	}

	m_materials_[id].set_reflection_color(texturePath);
}

void HorusObjectManager::set_reflection_weight(int id, const std::array<float, 3>& color)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material with this id exists.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_materials_[id].set_reflection_weight(color);
}

void HorusObjectManager::set_reflection_weight(int id, const std::string& texturePath)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material with this id exists.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_materials_[id].set_reflection_weight(texturePath);
}

void HorusObjectManager::set_reflection_roughness(int id, const std::array<float, 3>& color)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material with this id exists.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_materials_[id].set_reflection_roughness(color);

}

void HorusObjectManager::set_reflection_roughness(int id, const std::string& texturePath)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material with this id exists.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_materials_[id].set_reflection_roughness(texturePath);

}

void HorusObjectManager::set_refraction_color(int id, const std::array<float, 3>& color)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material with this id exists.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_materials_[id].set_refraction_color(color);
}

void HorusObjectManager::set_refraction_color(int id, const std::string& texturePath)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material with this id exists.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_materials_[id].set_refraction_color(texturePath);

}

void HorusObjectManager::set_coating_color(int id, const std::array<float, 3>& color)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material with this id exists.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_materials_[id].set_coating_color(color);
}

void HorusObjectManager::set_coating_color(int id, const std::string& texturePath)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material with this id exists.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_materials_[id].set_coating_color(texturePath);
}

void HorusObjectManager::set_sheen(int id, const std::array<float, 3>& color)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material with this id exists.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_materials_[id].set_sheen(color);
}

void HorusObjectManager::set_sheen(int id, const std::string& texturePath)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material with this id exists.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_materials_[id].set_sheen(texturePath);
}

void HorusObjectManager::set_sss_scatter_color(int id, const std::array<float, 3>& color)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material with this id exists.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_materials_[id].set_sss_scatter_color(color);
}

void HorusObjectManager::set_sss_scatter_color(int id, const std::string& texturePath)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material with this id exist.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_materials_[id].set_sss_scatter_color(texturePath);
}

void HorusObjectManager::set_backscatter_color(int id, const std::array<float, 3>& color)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material with this id exist.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_materials_[id].set_backscatter_color(color);
}

void HorusObjectManager::set_backscatter_color(int id, const std::string& texturePath)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material whith this id exist.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_materials_[id].set_backscatter_color(texturePath);
}

void HorusObjectManager::set_ior(int id, float ior)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material whith this id exist.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_materials_[id].set_ior(ior);
}

void HorusObjectManager::set_refraction_weight(int id, const std::array<float, 3>& weight)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material whith this id exist.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_materials_[id].set_refraction_weight(weight);
}

void HorusObjectManager::set_refraction_weight(int id, const std::string& texturePath)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material whith this id exist.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_materials_[id].set_refraction_weight(texturePath);
}

void HorusObjectManager::set_refraction_roughness(int id, const std::array<float, 3>& roughness)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material whith this id exist.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_materials_[id].set_refraction_roughness(roughness);
}

void HorusObjectManager::set_refraction_roughness(int id, const std::string& texturePath)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material whith this id exist.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_materials_[id].set_refraction_roughness(texturePath);
}

void HorusObjectManager::set_coating_weight(int id, const std::array<float, 3>& weight)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material whith this id exist.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_materials_[id].set_coating_weight(weight);
}

void HorusObjectManager::set_coating_weight(int id, const std::string& texturePath)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material whith this id exist.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_materials_[id].set_coating_weight(texturePath);
}

void HorusObjectManager::set_coating_roughness(int id, const std::array<float, 3>& roughness)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material whith this id exist.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_materials_[id].set_coating_roughness(roughness);
}

void HorusObjectManager::set_coating_roughness(int id, const std::string& texturePath)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material whith this id exist.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_materials_[id].set_coating_roughness(texturePath);
}

void HorusObjectManager::set_sheen_weight(int id, const std::array<float, 3>& weight)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material whith this id exist.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_materials_[id].set_sheen_weight(weight);
}

void HorusObjectManager::set_sheen_weight(int id, const std::string& texturePath)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material whith this id exist.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_materials_[id].set_sheen_weight(texturePath);
}

void HorusObjectManager::set_backscatter_weight(int id, const std::array<float, 3>& weight)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("no material whith this id exist.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_materials_[id].set_backscatter_weight(weight);
}

void HorusObjectManager::set_backscatter_weight(int id, const std::string& texturePath)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("");
		throw std::runtime_error("no material with this id exists.");
	}

	m_materials_[id].set_backscatter_weight(texturePath);
}

void HorusObjectManager::set_diffuse_weight(int id, const std::array<float, 3>& color)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("");
		throw std::runtime_error("no material with this id exists.");
	}

	m_materials_[id].set_diffuse_weight(color);
}

void HorusObjectManager::set_diffuse_weight(int id, const std::string& texturePath)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("");
		throw std::runtime_error("no material with this id exists.");
	}

	m_materials_[id].set_diffuse_weight(texturePath);
}

void HorusObjectManager::set_emission_weight(int id, const std::array<float, 3>& color)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("");
		throw std::runtime_error("no material with this id exists.");
	}

	m_materials_[id].set_emission_weight(color);
}

void HorusObjectManager::set_emission_weight(int id, const std::string& texturePath)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("");
		throw std::runtime_error("no material with this id exists.");
	}

	m_materials_[id].set_emission_weight(texturePath);
}

void HorusObjectManager::set_transparency(int id, const std::array<float, 3>& color)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("");
		throw std::runtime_error("no material with this id exists.");
	}

	m_materials_[id].set_transparency(color);
}

void HorusObjectManager::set_transparency(int id, const std::string& texturePath)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("");
		throw std::runtime_error("no material with this id exists.");
	}

	m_materials_[id].set_transparency(texturePath);
}

void HorusObjectManager::set_sss_scatter_distance(int id, const std::array<float, 3>& color)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("");
		throw std::runtime_error("no material with this id exists.");
	}

	m_materials_[id].set_sss_scatter_distance(color);
}

void HorusObjectManager::set_sss_scatter_distance(int id, const std::string& texturePath)
{
	if (m_materials_.find(id) == m_materials_.end())
	{
		spdlog::error("");
		throw std::runtime_error("no material with this id exists.");
	}

	m_materials_[id].set_sss_scatter_distance(texturePath);
}

// -----------------------------------------------------------------------------------------------
// Material editor management --------------------------------------------------------------------

void HorusObjectManager::assign_material_editor_node(int mesh_id, int mat_id)
{
	if (m_meshes_.find(mesh_id) == m_meshes_.end())
	{
		spdlog::error("no mesh with this id exists. ");
		throw std::runtime_error("no mesh with this id exists. ");
	}

	rpr_material_node newmat = nullptr;

	set_material_editor_mesh_to_set_material(mesh_id);	

	newmat = m_material_editors_[mat_id].get_final_material_for_export();

	m_meshes_[mesh_id].assign_material(newmat);

	spdlog::info("material editor node assigned to mesh with id: {}", mesh_id);

}

void HorusObjectManager::set_material_from_editor_node(int id, rpr_material_node mat)
{
	m_meshes_[id].assign_material(mat);
}

void HorusObjectManager::show_material_editor(int id)
{
	if (m_material_editors_.find(id) == m_material_editors_.end())
	{
		spdlog::error("no material editor with this id exists. ");
		throw std::runtime_error("no material editor with this id exists. ");
	}

	m_material_editors_[id].update();
}

void HorusObjectManager::open_material_editor(int id)
{
}

void HorusObjectManager::close_material_editor(int id)
{
}

void HorusObjectManager::open_material_editor_browser()
{
    HorusMaterialEditorBrowser& MaterialEditor = HorusMaterialEditorBrowser::get_instance();

	MaterialEditor.update();
}

void HorusObjectManager::close_material_editor_browser()
{
	HorusMaterialEditorBrowser& MaterialEditor = HorusMaterialEditorBrowser::get_instance();

	MaterialEditor.quit();
}

int HorusObjectManager::set_material_editor_to_show(int id)
{
	m_material_editor_to_show_ = id;

	return m_material_editor_to_show_;
}

int HorusObjectManager::get_material_editor_to_show()
{
	return m_material_editor_to_show_;
}

void HorusObjectManager::open_material_editor_create_menu()
{
	//material_editor_create_menu_ImpM.update();
}

void HorusObjectManager::close_material_editor_create_menu()
{
	//material_editor_create_menu_ImpM.quit();
}



void HorusObjectManager::quit_material_editor(int id)
{
	m_material_editors_[id].quit();
}

void HorusObjectManager::destroy_all_material_editors()
{
	for (auto& material_editor : m_material_editors_)
	{
		material_editor.second.quit();
	}
}


// -----------------------------------------------------------------------------------------------
// Light management ------------------------------------------------------------------------------

HorusLight& HorusObjectManager::create_light(int id, const std::string& name, const std::string& light_type, const std::string& hdri_image)
{
	if (m_lights_.find(id) != m_lights_.end())
	{
		spdlog::error("light with this id already exists. ");

		return m_lights_[id];
	}

	HorusLight new_light;
	new_light.init(light_type, hdri_image);
	m_lights_[id] = new_light;
	m_light_names_[id] = name;

	return m_lights_[id];
}

void HorusObjectManager::destroy_light(int id)
{
	auto it = m_lights_.find(id);

	if (it != m_lights_.end())
	{
		m_lights_[id].destroy_light();
	}
}

void HorusObjectManager::destroy_all_lights()
{
	for (auto& light : m_lights_)
	{
		light.second.destroy_light();
	}
}

void HorusObjectManager::set_light_position(int id, RadeonProRender::float3& position) {}

void HorusObjectManager::set_light_rotation(int id, RadeonProRender::float3& rotation) {}

void HorusObjectManager::set_light_scale(int id, RadeonProRender::float3& scale) {}

void HorusObjectManager::set_light_intensity(int id, RadeonProRender::float3& intensity) {}

void HorusObjectManager::create_scene(int id, const std::string& name)
{
	if (m_scenes_.find(id) != m_scenes_.end())
	{
		spdlog::error("scene with this id already exists. ");
		throw std::runtime_error("scene with this id already exists. ");
	}

	HorusScene new_scene;
	new_scene.init();
	m_scenes_[id] = new_scene;
	m_active_scene_id_ = id;
	m_scene_names_[id] = name;
}

rpr_scene HorusObjectManager::get_scene()
{
	return  m_scenes_[m_active_scene_id_].get_scene();
}

void HorusObjectManager::destroy_scene(int id)
{
	auto it = m_scenes_.find(id);

	if (it != m_scenes_.end())
	{
		m_scenes_[id].destroy_scene();
	}
}

void HorusObjectManager::destroy_all_scenes()
{
	for (auto& scene : m_scenes_)
	{
		scene.second.destroy_scene();
	}
}

void HorusObjectManager::show_dummy_dragon()
{
	m_scenes_[m_active_scene_id_].show_dummy_dragon();
}

void HorusObjectManager::show_dummy_plane()
{
	m_scenes_[m_active_scene_id_].show_dummy_plane();
}

// -----------------------------------------------------------------------------------------------