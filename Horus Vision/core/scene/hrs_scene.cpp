
#include "hrs_scene.h" // nothing
#include "hrs_engine.h" // glad.h
#include "hrs_radeon.h" // glfw3.h
#include "hrs_object_manager.h" // glfw3.h
#include "hrs_importer_manager.h"

#include <mutex>

#include "hrs_timer.h"

std::mutex SceneMutex;

void HorusScene::Init()
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& Gc = HorusGarbageCollector::GetInstance();

	// create scene
	CHECK(rprContextCreateScene(Radeon.GetContext(), &m_Scene_));
	Gc.Add(m_Scene_);
}

void HorusScene::DestroyScene()
{
	//CHECK(rprObjectDelete(m_Scene_));
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

void HorusScene::CreateGrid()
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();

	rpr_curve Curve = nullptr;

	size_t NumControlPoints = 4;
	rpr_float control_points[] = { -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f };
	rpr_int control_points_stride = 3 * sizeof(rpr_float);

	size_t num_indices = 4;
	rpr_uint curveCount = 1;
	rpr_uint indices[] = { 0, 1, 2, 3 };
	rpr_float radius = 0.1f;
	rpr_float textureUV = 0.0f;
	rpr_int segment_per_curve = 4;
	rpr_uint creation_flag = 0;

	CHECK(rprContextCreateCurve(Radeon.GetContext(), &Curve, NumControlPoints, control_points, control_points_stride, num_indices, curveCount, indices, &radius, &textureUV, &segment_per_curve, creation_flag));


}

void HorusScene::ClearScene()
{
	CHECK(rprSceneClear(m_Scene_));
}

void HorusScene::UpdateScene() {}

void HorusScene::ShowDummyDragon()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	glm::vec4 Roughness = { 0.45f, 0.45f, 0.45f, 0.45f };

	int Dragon = ObjectManager.CreateGroupShape("resources/Meshes/dragon/dragon_clean_low_Scale.obj", "dragon_low_msh");
	int DragonMat = ObjectManager.CreateMaterial("M_Dragon");

	ObjectManager.SetBaseColor(DragonMat, "resources/Textures/dragon/DRAGON_CLEAN_diffuse.jpg");
	ObjectManager.SetNormalMap(DragonMat, "resources/Textures/dragon/DRAGON_CLEAN_normal.png");
	ObjectManager.SetReflectionRoughness(DragonMat, Roughness);
	ObjectManager.AssignMaterial(Dragon, DragonMat);

	//ObjectManager.create_material_editor_node(0, "M_Dragon");
	/*ObjectManager.assign_material_editor_node(0, 0); */

	ObjectManager.SetGroupShapeRotation(Dragon, { 0.f, 1.f, 0.0f });
}

void HorusScene::ShowDummyPlane()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	int Floor = ObjectManager.CreateGroupShape("resources/Meshes/florr_test.fbx", "plane_msh");
	int FloorMat = ObjectManager.CreateMaterial("M_Floor");

	ObjectManager.SetBaseColor(FloorMat, "resources/Textures/ufoieaklw_8K_Albedo.jpg");
	//g_object_manager.set_base_color(FloorMat, debug_color); // debug purpose
	ObjectManager.SetNormalMap(FloorMat, "resources/Textures/ufoieaklw_8K_Normal.jpg");
	ObjectManager.SetReflectionRoughness(FloorMat, "resources/Textures/ufoieaklw_8K_Roughness.jpg");
	ObjectManager.AssignMaterial(Floor, FloorMat);

	//ObjectManager.create_material_editor_node(1, "M_Floor");
	/*ObjectManager.assign_material_editor_node(1, 1); */// assign material to mesh

	ObjectManager.SetGroupShapeScale(Floor, { 10.0f, 10.0f, 10.0f });
}

void HorusScene::ShowLookdevScene()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusImporterManager& GltfImporter = HorusImporterManager::GetInstance();
	HorusRadeon& Radeon = HorusRadeon::GetInstance();

	{
		glm::vec4 Roughness = { 0.45f, 0.45f, 0.45f, 0.45f };

		std::array<float, 3> Color = { 1, 1, 1 };
		std::array<float, 3> Specular = { 1, 1, 1 };
		std::array<float, 3> TransmissionColor = { 0.996078f, 0.858824f, 0.639216f };
		std::array<float, 3> RefractionWeight = { 1.f, 1.f, 1.f };

		// Create Background
		int BackgoundMesh = ObjectManager.CreateGroupShape("resources/Lookdev/Basic_Assets/Background.fbx", "Background");
		int BackgroundMat = ObjectManager.CreateMaterial("M_Background");

		ObjectManager.SetReflectionMode(BackgroundMat, HorusMaterial::ReflectionTypePBR); // 0 = PBR
		ObjectManager.AssignMaterial(BackgoundMesh, BackgroundMat);
		ObjectManager.SetGroupShapeScale(BackgoundMesh, { 20.0f, 10.0f, 10.0f });

		{
			// Create statue (Arethusa)
			//int Aretheuse =  ObjectManager.CreateGroupShape("resources/Lookdev/Downloaded/aretheuse/source/ARETHEUSE/ARETHEUSE.obj", "aretheuse");
			//int AreMat = ObjectManager.CreateMaterial("M_aretheuse");

			//ObjectManager.SetBaseColor(AreMat, "resources/Lookdev/Downloaded/aretheuse/textures/ARETHEUSE.jpg");
			//ObjectManager.SetReflectionMode(AreMat, 0); // 0 = PBR
			//ObjectManager.SetReflectionRoughness(AreMat, Roughness);
			//ObjectManager.AssignMaterial(Aretheuse, AreMat);
			//

			//ObjectManager.SetGroupShapeScale(Aretheuse, { 0.25f, 0.25f, 0.25f });
			//ObjectManager.SetGroupShapeRotation(Aretheuse, { 0.f, -90.f, 0.0f });
		}

		// Create Gold Sphere
		int SphereMesh = ObjectManager.CreateGroupShape("resources/Lookdev/Basic_Assets/Sphere.fbx", "Sphere");
		int Goldmat = ObjectManager.CreateMaterial("M_Sphere");
		ObjectManager.SetBaseColor(Goldmat, "resources/Lookdev/Material/MetalGoldPaint002/MetalGoldPaint002_COL_8K_METALNESS.png");
		ObjectManager.SetNormalMap(Goldmat, "resources/Lookdev/Material/MetalGoldPaint002/MetalGoldPaint002_NRM_8K_METALNESS.png");
		ObjectManager.SetReflectionRoughness(Goldmat, "resources/Lookdev/Material/MetalGoldPaint002/MetalGoldPaint002_ROUGHNESS_8K_METALNESS.png");
		//ObjectManager.SetMetallic(Goldmat, "resources/Lookdev/Material/MetalGoldPaint002/MetalGoldPaint002_METALNESS_8K_METALNESS.png");
		ObjectManager.SetReflectionMode(Goldmat, HorusMaterial::ReflectionTypeMetalness); // 1 = metalness
		ObjectManager.AssignMaterial(SphereMesh, Goldmat);
		
		ObjectManager.SetGroupShapePosition(SphereMesh, { 5.0f, 2.f, 0.0f });
		ObjectManager.SetGroupShapeScale(SphereMesh, { 0.5f, 1.2f, 0.5f });

		{
			// Create Statue (Arria)
			int Arria = ObjectManager.CreateGroupShape("resources/Lookdev/Downloaded/Arria/source/ARRIA/ARRIA.obj", "Arria");

			ObjectManager.SetGroupShapeScale(Arria, { 0.25f, 0.25f, 0.25f });
		}
	}

	// Test Scene
	//auto ImportBarberChair = GltfImporter.ImportGltf("resources/Meshes/gltf_teapot/cube_floor.gltf", Radeon.GetContext(), Radeon.GetMatsys(), &ObjectManager.GetScene());
	//auto ImportBarberChair = GltfImporter.ImportGltf("resources/Scene/gltf/BarberShopChair_01_4k.gltf", Radeon.GetContext(), Radeon.GetMatsys(), &ObjectManager.GetScene());

	glm::vec4 GlassBaseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	glm::vec4 GlassRoughness = { 1.0f, 1.0f, 1.0f, 1.0f };
	glm::vec4 GlassTransmissionColor = { 0.3f, 0.5f, 0.8f, 1.0f };
	glm::vec4 GlassRefractionWeight = { 1.f, 1.f, 1.f, 1.0f };
	glm::vec4 GlassRefractionRoughness = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Create Glass Sphere
	int SphereMesh = ObjectManager.CreateGroupShape("resources/Lookdev/Basic_Assets/Sphere.fbx", "Sphere");
	int Glassmat = ObjectManager.CreateMaterial("M_Sphere");

	ObjectManager.SetBaseColor(Glassmat, GlassBaseColor);
	ObjectManager.SetReflectionRoughness(Glassmat, GlassRoughness);
	ObjectManager.SetReflectionMode(Glassmat, HorusMaterial::ReflectionTypePBR); // 0 = PBR
	ObjectManager.SetRefractionColor(Glassmat, GlassTransmissionColor);
	ObjectManager.SetRefractionWeight(Glassmat, GlassRefractionWeight);
	ObjectManager.SetRefractionRoughness(Glassmat, GlassRefractionRoughness);
	ObjectManager.SetTransparency(Glassmat, GlassRefractionRoughness);
	ObjectManager.AssignMaterial(SphereMesh, Glassmat);

	ObjectManager.SetGroupShapePosition(SphereMesh, { -3.0f, 2.f, 0.0f });

	// Light part

	//int DirectionnalLight = ObjectManager.CreateLight("Lgt_Dir01", "directional");

	//int SkyLight = ObjectManager.CreateLight("Lgt_Sky01", "sky");

	// Sphere Light (Point)
	int SphereLight01 = ObjectManager.CreateLight("Lgt_Sphere01", "sphere");
	ObjectManager.SetLightPosition(SphereLight01, glm::vec3(3.5f, 2.0f, 0.0f));
	ObjectManager.SetLightIntensity(SphereLight01, glm::vec3(2.0f, 2.0f, 2.0f));

	int SphereLight02 = ObjectManager.CreateLight("Lgt_Sphere02", "sphere");
	ObjectManager.SetLightPosition(SphereLight02, glm::vec3(8.0f, 1.0f, 0.0f));
	ObjectManager.SetLightIntensity(SphereLight02, glm::vec3(2.0f, 2.0f, 2.0f));

	// Disk Light (Spot)
	int Spotlight = ObjectManager.CreateLight("Lgt_Spot01", "disk");
	ObjectManager.SetLightPosition(Spotlight, glm::vec3(-5.0f, 1.0f, 0.0f));
	ObjectManager.SetLightRotation(Spotlight, glm::vec3(-45.0f, 0.0f, 0.0f));
	ObjectManager.SetLightIntensity(Spotlight, glm::vec3(2.0f, 2.0f, 2.0f));

	int DiskLight = ObjectManager.CreateLight("Lgt_Spot02", "disk");
	ObjectManager.SetLightPosition(DiskLight, glm::vec3(-3.0f, 1.0f, 0.0f));
	ObjectManager.SetLightIntensity(DiskLight, glm::vec3(2.0f, 2.0f, 2.0f));

	


	
}

void HorusScene::ShowJaguardXKSS()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	// Create Background
	int BackgoundMesh = ObjectManager.CreateGroupShape("resources/Lookdev/Basic_Assets/Background.fbx", "Background");
	int BackgroundMat = ObjectManager.CreateMaterial("M_Background");

	ObjectManager.SetReflectionMode(BackgroundMat, HorusMaterial::ReflectionTypePBR); // 0 = PBR
	ObjectManager.AssignMaterial(BackgoundMesh, BackgroundMat);
	ObjectManager.SetGroupShapeScale(BackgoundMesh, { 20.0f, 10.0f, 10.0f });


	// Create Car
	int JaguardXKSS = ObjectManager.CreateGroupShape("resources/Scene/Jaguard/jaguard_xkss.fbx", "Jaguard_XKSS");
}
