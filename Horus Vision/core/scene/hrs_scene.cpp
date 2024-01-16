
#include "hrs_scene.h" // nothing
#include "hrs_engine.h" // glad.h
#include "hrs_radeon.h" // glfw3.h
#include "hrs_object_manager.h" // glfw3.h

void HorusScene::Init()
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();

	// create scene
	CHECK(rprContextCreateScene(Radeon.GetContext(), &m_Scene_));
}

void HorusScene::DestroyScene()
{
	CHECK(rprObjectDelete(m_Scene_));
}

void HorusScene::CreateDefaultScene()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	// Create Default Engine Scene
	ObjectManager.CreateDefaultScene();
}

void HorusScene::SetScene(rpr_scene Scene)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();

	m_Scene_ = Scene;

	CHECK(rprContextSetScene(Radeon.GetContext(), Scene));
}

void HorusScene::LoadScene(const char* Path) {}

void HorusScene::UnloadScene()
{
	CHECK(rprSceneClear(m_Scene_));
}

void HorusScene::ExportScene(const char* path) {}

void HorusScene::ImportScene(const char* path) {}

void HorusScene::ShowScene()
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();

	CHECK(rprContextSetScene(Radeon.GetContext(), m_Scene_));
}

void HorusScene::ClearScene()
{
	CHECK(rprSceneClear(m_Scene_));
}

void HorusScene::UpdateScene() {}

void HorusScene::ShowDummyDragon()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	std::array<float, 3> Roughness = { 0.45f, 0.45f, 0.45f };

	int Dragon = ObjectManager.CreateMesh("resources/Meshes/dragon/dragon_clean_low_Scale.obj", "dragon_low_msh");
	int DragonMat = ObjectManager.CreateMaterial("M_Dragon");

	ObjectManager.SetBaseColor(DragonMat, "resources/Textures/dragon/DRAGON_CLEAN_diffuse.jpg");
	ObjectManager.SetNormal(DragonMat, "resources/Textures/dragon/DRAGON_CLEAN_normal.png");
	ObjectManager.SetRoughness(DragonMat, Roughness);
	ObjectManager.AssignMaterial(Dragon, DragonMat);

	//ObjectManager.create_material_editor_node(0, "M_Dragon");
	/*ObjectManager.assign_material_editor_node(0, 0); */

	ObjectManager.SetShapeRotation(Dragon, { 0.f, 1.f, 0.0f });
}

void HorusScene::ShowDummyPlane()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	int Floor = ObjectManager.CreateMesh("resources/Meshes/florr_test.fbx", "plane_msh");
	int FloorMat = ObjectManager.CreateMaterial("M_Floor");

	ObjectManager.SetBaseColor(FloorMat, "resources/Textures/ufoieaklw_8K_Albedo.jpg");
	//g_object_manager.set_base_color(FloorMat, debug_color); // debug purpose
	ObjectManager.SetNormal(FloorMat, "resources/Textures/ufoieaklw_8K_Normal.jpg");
	ObjectManager.SetRoughness(FloorMat, "resources/Textures/ufoieaklw_8K_Roughness.jpg");
	ObjectManager.AssignMaterial(Floor, FloorMat);

	//ObjectManager.create_material_editor_node(1, "M_Floor");
	/*ObjectManager.assign_material_editor_node(1, 1); */// assign material to mesh

	ObjectManager.SetShapeScale(Floor, { 10.0f, 10.0f, 10.0f });
}

void HorusScene::ShowLookdevScene()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();


	// Create Background
	int BackgoundMesh = ObjectManager.CreateMesh("resources/Lookdev/Basic_Assets/Background.fbx", "Background");
	int BackgroundMat = ObjectManager.CreateMaterial("M_Background");

	ObjectManager.SetReflectionMode(BackgroundMat, 0); // 0 = PBR
	ObjectManager.AssignMaterial(BackgoundMesh, BackgroundMat);
	//ObjectManager.create_material_editor_node(0, "M_Background");

	ObjectManager.SetShapeScale(BackgoundMesh, { 20.0f, 10.0f, 10.0f });

	// Create statue
	/*ObjectManager.create_mesh("resources/Lookdev/Downloaded/aretheuse/source/ARETHEUSE/ARETHEUSE.obj", "aretheuse", 1);

	ObjectManager.create_material(1, "M_aretheuse");
	ObjectManager.set_base_color(1, "resources/Lookdev/Downloaded/aretheuse/textures/ARETHEUSE.jpg");
	ObjectManager.assign_material(1, 1);
	ObjectManager.create_material_editor_node(1, "M_aretheuse");

	ObjectManager.set_shape_scale(1, { 0.2f, 0.2f, 0.2f });
	ObjectManager.set_shape_rotation(1, { 0.f, 1.f, 0.0f }, 90.0f);*/

	// Create Gold Sphere
	int Spheremesh = ObjectManager.CreateMesh("resources/Lookdev/Basic_Assets/Sphere.fbx", "Sphere");
	int Goldmat = ObjectManager.CreateMaterial("M_Sphere");
	ObjectManager.SetBaseColor(Goldmat, "resources/Lookdev/Material/MetalGoldPaint002/MetalGoldPaint002_COL_8K_METALNESS.png");
	ObjectManager.SetNormal(Goldmat, "resources/Lookdev/Material/MetalGoldPaint002/MetalGoldPaint002_NRM_8K_METALNESS.png");
	ObjectManager.SetRoughness(Goldmat, "resources/Lookdev/Material/MetalGoldPaint002/MetalGoldPaint002_ROUGHNESS_8K_METALNESS.png");
	ObjectManager.SetMetallic(Goldmat, "resources/Lookdev/Material/MetalGoldPaint002/MetalGoldPaint002_METALNESS_8K_METALNESS.png");
	ObjectManager.SetReflectionMode(Goldmat, 1); // 1 = metalness
	ObjectManager.AssignMaterial(Spheremesh, Goldmat);
	//ObjectManager.create_material_editor_node(1, "M_Sphere");

	ObjectManager.SetShapePosition(Spheremesh, { 5.0f, 2.f, 0.0f });

	// Create Glass Sphere

	std::array<float, 3> color = { 1, 1, 1 };
	std::array<float, 3> roughness = { 1, 1, 1 };
	std::array<float, 3> specular = { 1, 1, 1 };
	std::array<float, 3> transmission_color = { 0.996078f, 0.858824f, 0.639216f };
	std::array<float, 3> refraction_weight = { 1.f, 1.f, 1.f };

	//int floor = ObjectManager.create_mesh("resources/Meshes/florr_test.fbx", "plane_msh");

	//int floormata = ObjectManager.create_material("M_Floor");
	//ObjectManager.set_base_color(floormata, "resources/Textures/ufoieaklw_8K_Albedo.jpg");
	////g_object_manager.set_base_color(floormata, debug_color); // debug purpose
	//ObjectManager.set_normal(floormata, "resources/Textures/ufoieaklw_8K_Normal.jpg");
	//ObjectManager.set_roughness(floormata, "resources/Textures/ufoieaklw_8K_Roughness.jpg");
	//ObjectManager.assign_material(floor, floormata);

	//ObjectManager.create_material_editor_node(1, "M_Floor");

	/*ObjectManager.assign_material_editor_node(1, 1); */// assign material to mesh

	//ObjectManager.set_shape_scale(floor, { 100.0f, 100.0f, 100.0f });

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
