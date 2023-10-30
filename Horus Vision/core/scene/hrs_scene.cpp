#include "hrs_scene.h"

#include "hrs_engine.h"
#include "hrs_radeon.h"
#include "objects/hrs_object_manager.h"

HorusEngine& g_engine_imps = HorusEngine::get_instance();
HorusObjectManager& g_object_imps = HorusObjectManager::get_instance();
HorusRadeon& g_radeon_imps = HorusRadeon::get_instance();

void HorusScene::init()
{
	// create scene
	CHECK(rprContextCreateScene(g_radeon_imps.get_context(), &m_scene_));
	CHECK(rprContextSetScene(g_radeon_imps.get_context(), m_scene_));
}

void HorusScene::destroy_scene()
{
	CHECK(rprObjectDelete(m_scene_));
}

void HorusScene::load_scene(const char* path){}

void HorusScene::unload_scene()
{
	CHECK(rprSceneClear(m_scene_));
}

void HorusScene::export_scene(const char* path) {}

void HorusScene::import_scene(const char* path) {}

void HorusScene::show_scene()
{
	CHECK(rprContextSetScene(g_radeon_imps.get_context(), m_scene_));
}

void HorusScene::clear_scene()
{
	CHECK(rprSceneClear(m_scene_));
}

void HorusScene::update_scene() {}

void HorusScene::show_dummy_dragon()
{
	std::array<float, 3> roughness = { 0.45f, 0.45f, 0.45f };

	g_object_imps.create_mesh("resources/Meshes/dragon/dragon_clean_low_Scale.obj", "dragon_low_msh", 0);

	/*g_object_imps.create_material(0, "M_Dragon");
	g_object_imps.set_base_color(0, "resources/Textures/dragon/DRAGON_CLEAN_diffuse.jpg");
	g_object_imps.set_normal(0, "resources/Textures/dragon/DRAGON_CLEAN_normal.png");
	g_object_imps.set_roughness(0, roughness);
	g_object_imps.assign_material(0, 0);*/

	g_object_imps.create_material_editor_node(0, "M_Dragon");

	g_object_imps.assign_material_editor_node(0, 0); 


	g_object_imps.set_shape_rotation(0, { 0.f, 1.f, 0.0f }, 10.0f);
}

void HorusScene::show_dummy_plane()
{
	g_object_imps.create_mesh("resources/Meshes/florr_test.fbx", "plane_msh", 1);

	//g_object_imps.create_material(1, "M_Floor");
	//g_object_imps.set_base_color(1, "resources/Textures/ufoieaklw_8K_Albedo.jpg");
	////g_object_manager.set_base_color(1, debug_color); // debug purpose
	//g_object_imps.set_normal(1, "resources/Textures/ufoieaklw_8K_Normal.jpg");
	//g_object_imps.set_roughness(1, "resources/Textures/ufoieaklw_8K_Roughness.jpg");
	//g_object_imps.assign_material(1, 1);

	g_object_imps.create_material_editor_node(1, "M_Floor");

	g_object_imps.assign_material_editor_node(1, 1); // assign material to mesh

	g_object_imps.set_shape_scale(1, { 10.0f, 10.0f, 10.0f });
}
