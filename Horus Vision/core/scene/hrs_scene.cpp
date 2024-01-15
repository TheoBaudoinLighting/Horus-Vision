
#include "hrs_scene.h" // nothing
#include "hrs_engine.h" // glad.h
#include "hrs_radeon.h" // glfw3.h
#include "hrs_object_manager.h" // glfw3.h

void HorusScene::init()
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	// create scene
	CHECK(rprContextCreateScene(Radeon.get_context(), &m_scene_));
}

void HorusScene::destroy_scene()
{
	CHECK(rprObjectDelete(m_scene_));
}

void HorusScene::CreateDefaultScene()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::get_instance();

	// Create Default Engine Scene
	ObjectManager.CreateDefaultScene();
}

void HorusScene::SetScene(rpr_scene scene)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	m_scene_ = scene;

	CHECK(rprContextSetScene(Radeon.get_context(), scene));
}

void HorusScene::load_scene(const char* path) {}

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

	ObjectManager.create_mesh("resources/Meshes/dragon/dragon_clean_low_Scale.obj", "dragon_low_msh");

	ObjectManager.create_material("M_Dragon");
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

	ObjectManager.create_mesh("resources/Meshes/florr_test.fbx", "plane_msh");

	ObjectManager.create_material("M_Floor");
	ObjectManager.set_base_color(1, "resources/Textures/ufoieaklw_8K_Albedo.jpg");
	//g_object_manager.set_base_color(1, debug_color); // debug purpose
	ObjectManager.set_normal(1, "resources/Textures/ufoieaklw_8K_Normal.jpg");
	ObjectManager.set_roughness(1, "resources/Textures/ufoieaklw_8K_Roughness.jpg");
	ObjectManager.assign_material(1, 1);

	ObjectManager.create_material_editor_node(1, "M_Floor");

	/*ObjectManager.assign_material_editor_node(1, 1); */// assign material to mesh

	ObjectManager.set_shape_scale(1, { 10.0f, 10.0f, 10.0f });
}

void HorusScene::show_LookdevScene()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::get_instance();


	// Create Background
	int BackgoundMesh = ObjectManager.create_mesh("resources/Lookdev/Basic_Assets/Background.fbx", "Background");
	int BackgroundMat = ObjectManager.create_material("M_Background");

	ObjectManager.set_reflection_mode(BackgroundMat, 0); // 0 = PBR
	ObjectManager.assign_material(BackgoundMesh, BackgroundMat);
	//ObjectManager.create_material_editor_node(0, "M_Background");

	ObjectManager.set_shape_scale(BackgoundMesh, { 20.0f, 10.0f, 10.0f });


	// Create statue
	/*ObjectManager.create_mesh("resources/Lookdev/Downloaded/aretheuse/source/ARETHEUSE/ARETHEUSE.obj", "aretheuse", 1);

	ObjectManager.create_material(1, "M_aretheuse");
	ObjectManager.set_base_color(1, "resources/Lookdev/Downloaded/aretheuse/textures/ARETHEUSE.jpg");
	ObjectManager.assign_material(1, 1);
	ObjectManager.create_material_editor_node(1, "M_aretheuse");

	ObjectManager.set_shape_scale(1, { 0.2f, 0.2f, 0.2f });
	ObjectManager.set_shape_rotation(1, { 0.f, 1.f, 0.0f }, 90.0f);*/


	// Create Gold Sphere
	int spheremesh = ObjectManager.create_mesh("resources/Lookdev/Basic_Assets/Sphere.fbx", "Sphere");
	int goldmat = ObjectManager.create_material("M_Sphere");
	ObjectManager.set_base_color(goldmat, "resources/Lookdev/Material/MetalGoldPaint002/MetalGoldPaint002_COL_8K_METALNESS.png");
	ObjectManager.set_normal(goldmat, "resources/Lookdev/Material/MetalGoldPaint002/MetalGoldPaint002_NRM_8K_METALNESS.png");
	ObjectManager.set_roughness(goldmat, "resources/Lookdev/Material/MetalGoldPaint002/MetalGoldPaint002_ROUGHNESS_8K_METALNESS.png");
	ObjectManager.set_metallic(goldmat, "resources/Lookdev/Material/MetalGoldPaint002/MetalGoldPaint002_METALNESS_8K_METALNESS.png");
	ObjectManager.set_reflection_mode(goldmat, 1); // 1 = metalness
	ObjectManager.assign_material(spheremesh, goldmat);
	//ObjectManager.create_material_editor_node(1, "M_Sphere");

	ObjectManager.set_shape_position(spheremesh, { 5.0f, 2.f, 0.0f });

	// Create Glass Sphere

	std::array<float, 3> color = { 1, 1, 1 };
	std::array<float, 3> roughness = { 1, 1, 1 };
	std::array<float, 3> specular = { 1, 1, 1 };
	std::array<float, 3> transmission_color = { 0.996078f, 0.858824f, 0.639216f };
	std::array<float, 3> refraction_weight = { 1.f, 1.f, 1.f };

	int floor = ObjectManager.create_mesh("resources/Meshes/florr_test.fbx", "plane_msh");

	int floormata = ObjectManager.create_material("M_Floor");
	ObjectManager.set_base_color(floormata, "resources/Textures/ufoieaklw_8K_Albedo.jpg");
	//g_object_manager.set_base_color(floormata, debug_color); // debug purpose
	ObjectManager.set_normal(floormata, "resources/Textures/ufoieaklw_8K_Normal.jpg");
	ObjectManager.set_roughness(floormata, "resources/Textures/ufoieaklw_8K_Roughness.jpg");
	ObjectManager.assign_material(floor, floormata);

	//ObjectManager.create_material_editor_node(1, "M_Floor");

	/*ObjectManager.assign_material_editor_node(1, 1); */// assign material to mesh

	ObjectManager.set_shape_scale(floor, { 100.0f, 100.0f, 100.0f });




	//ObjectManager.create_mesh("resources/Lookdev/Basic_Assets/Sphere.fbx", "SphereGlass");
	/*ObjectManager.create_material(3, "M_SphereGlass");
	ObjectManager.set_base_color(3, color);
	ObjectManager.set_roughness(3, roughness);
	ObjectManager.set_refraction_color(3, transmission_color);
	ObjectManager.set_refraction_weight(3, refraction_weight);

	ObjectManager.set_reflection_mode(3, 0);
	ObjectManager.assign_material(3, 3);
	ObjectManager.create_material_editor_node(3, "M_SphereGlass");

	ObjectManager.set_shape_position(3, { 0.0f, 2.f, 0.0f });*/




}
