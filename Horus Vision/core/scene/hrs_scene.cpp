#include "hrs_scene.h"

#include "hrs_engine.h"
#include "hrs_radeon.h"
#include "objects/hrs_object_manager.h"

//HorusEngine& Engine = HorusEngine::get_instance();
//HorusObjectManager& ObjectManager = HorusObjectManager::get_instance();
//HorusRadeon& Radeon = HorusRadeon::get_instance();

void HorusScene::init()
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	// create scene
	CHECK(rprContextCreateScene(Radeon.get_context(), &m_scene_));
	CHECK(rprContextSetScene(Radeon.get_context(), m_scene_));
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
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	CHECK(rprContextSetScene(Radeon.get_context(), m_scene_));
}

void HorusScene::clear_scene()
{
	CHECK(rprSceneClear(m_scene_));
}

void HorusScene::update_scene() {}

void HorusScene::show_dummy_dragon()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::get_instance();

	std::array<float, 3> roughness = { 0.45f, 0.45f, 0.45f };

	ObjectManager.create_mesh("resources/Meshes/dragon/dragon_clean_low_Scale.obj", "dragon_low_msh", 0);

	ObjectManager.create_material(0, "M_Dragon");
	ObjectManager.set_base_color(0, "resources/Textures/dragon/DRAGON_CLEAN_diffuse.jpg");
	ObjectManager.set_normal(0, "resources/Textures/dragon/DRAGON_CLEAN_normal.png");
	ObjectManager.set_roughness(0, roughness);
	ObjectManager.assign_material(0, 0);

	ObjectManager.create_material_editor_node(0, "M_Dragon");

	/*ObjectManager.assign_material_editor_node(0, 0); */


	ObjectManager.set_shape_rotation(0, { 0.f, 1.f, 0.0f }, 10.0f);
}

void HorusScene::show_dummy_plane()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::get_instance();

	ObjectManager.create_mesh("resources/Meshes/florr_test.fbx", "plane_msh", 1);

	ObjectManager.create_material(1, "M_Floor");
	ObjectManager.set_base_color(1, "resources/Textures/ufoieaklw_8K_Albedo.jpg");
	//g_object_manager.set_base_color(1, debug_color); // debug purpose
	ObjectManager.set_normal(1, "resources/Textures/ufoieaklw_8K_Normal.jpg");
	ObjectManager.set_roughness(1, "resources/Textures/ufoieaklw_8K_Roughness.jpg");
	ObjectManager.assign_material(1, 1);

	ObjectManager.create_material_editor_node(1, "M_Floor");

	/*ObjectManager.assign_material_editor_node(1, 1); */// assign material to mesh

	ObjectManager.set_shape_scale(1, { 10.0f, 10.0f, 10.0f });
}
