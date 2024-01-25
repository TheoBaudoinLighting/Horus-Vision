#include "hrs_reset_buffers.h"
#include "hrs_importer_manager.h"
#include "hrs_radeon.h"

#include "RadeonProRender.hpp"
#include "ProRenderGLTF.h" 
#include "common.h"
#include "hrs_object_manager.h"

const char* ExportGltfTimer = "ExportGltf";

bool HorusImporterManager::ImportGltf(const std::string& Path, rpr_context Context, rpr_material_system Matsys, rpr_scene* Scene)
{
	HorusGarbageCollector& GarbageCollector = HorusGarbageCollector::GetInstance();
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	const int SaveSceneId = ObjectManager.GetActiveSceneId();

	RprGltfImportCallbacks* Callbacks = nullptr;
	unsigned int Flags = 0;
	RPRS_context RPRSContext = nullptr;

	if (Path.empty())
	{
		spdlog::error("Imported GLTF scene path is empty");
		return false;
	}

	rpr_scene SceneTemp = nullptr;
	CHECK(rprImportFromGLTF(Path.c_str(), Context, Matsys, &SceneTemp, Callbacks, Flags, RPRSContext));

	//----------------- CAMERA -----------------//

	// Get the camera
	int CameraCount = 0;
	CHECK(rprGLTF_ListImported_Cameras(0, 0, &CameraCount));

	rpr_camera* Cameras = new rpr_camera[CameraCount];
	CHECK(rprGLTF_ListImported_Cameras(Cameras, CameraCount * sizeof(rpr_camera), NULL));

	spdlog::info("Camera count: {}", CameraCount);

	for (int i = 0; i < CameraCount; i++)
	{
		rpr_camera Camera = Cameras[i];

		ObjectManager.CameraExtractor(Camera);

		//ObjectManager.CreateRadeonCameraFromGLTF(SaveSceneId, "Camera", Camera);

		spdlog::info("Camera found");
	}

	//----------------- LIGHT -----------------//

	// Get the lights
	int LightCount = 0;
	CHECK(rprGLTF_ListImported_Lights(0, 0, &LightCount));

	rpr_light* Lights = new rpr_light[LightCount];
	CHECK(rprGLTF_ListImported_Lights(Lights, LightCount * sizeof(rpr_light), NULL));

	spdlog::info("Light count: {}", LightCount);

	for (int i = 0; i < LightCount; i++)
	{
		rpr_light Light = Lights[i];
		// TODO : Add light to the scene
		spdlog::info("Light found");
	}

	//----------------- SHAPE -----------------//

	// Get the shapes
	int ShapeCount = 0;
	CHECK(rprGLTF_ListImported_Shapes(0, 0, &ShapeCount));

	rpr_shape* Shapes = new rpr_shape[ShapeCount];
	CHECK(rprGLTF_ListImported_Shapes(Shapes, ShapeCount * sizeof(rpr_shape), NULL));

	spdlog::info("Shape count: {}", ShapeCount);

	for (int i = 0; i < ShapeCount; i++)
	{
		rpr_shape Shape = Shapes[i];
		// TODO : Add shape to the scene
		spdlog::info("Shape found");
	}

	//----------------- MATERIAL -----------------//
	int MaterialCount = 0;
	CHECK(rprGLTF_ListImported_MaterialNodes(0, 0, &MaterialCount));

	rpr_material_node* MaterialNodes = new rpr_material_node[MaterialCount];
	CHECK(rprGLTF_ListImported_MaterialNodes(MaterialNodes, MaterialCount * sizeof(rpr_material_node), NULL));

	spdlog::info("Material count: {}", MaterialCount);

	for (int i = 0; i < MaterialCount; i++)
	{
		rpr_material_node MaterialNode = MaterialNodes[i];
		// TODO : Add material to the scene
		spdlog::info("Material found");
	}

	//----------------- IMAGE -----------------//
	int ImageCount = 0;
	CHECK(rprGLTF_ListImported_Images(0, 0, &ImageCount));

	rpr_image* Images = new rpr_image[ImageCount];
	CHECK(rprGLTF_ListImported_Images(Images, ImageCount * sizeof(rpr_image), NULL));

	spdlog::info("Image count: {}", ImageCount);

	for (int i = 0; i < ImageCount; i++)
	{
		rpr_image Image = Images[i];
		// TODO : Add image to the scene
		spdlog::info("Image found");
	}

	//----------------- BUFFERS -----------------//
	int BufferCount = 0;
	CHECK(rprGLTF_ListImported_Buffer(0, 0, &BufferCount));

	rpr_buffer* Buffers = new rpr_buffer[BufferCount];
	CHECK(rprGLTF_ListImported_Buffer(Buffers, BufferCount * sizeof(rpr_buffer), NULL));

	spdlog::info("Buffer count: {}", BufferCount);

	for (int i = 0; i < BufferCount; i++)
	{
		rpr_buffer Buffer = Buffers[i];
		// TODO : Add buffer to the scene
		spdlog::info("Buffer found");
	}

	//----------------- CLEANUP -----------------//


	delete[] Cameras;
	delete[] Lights;
	delete[] Shapes;
	delete[] MaterialNodes;
	delete[] Images;
	delete[] Buffers;

	// Reassign the scene
	ObjectManager.SetScene(SaveSceneId);

	rprObjectDelete(SceneTemp);

	rprGLTF_ReleaseImportedData();
	rprGLTF_DeleteListImportedObjects();

	spdlog::info("Imported GLTF scene: {}", Path);

	HorusResetBuffers::GetInstance().CallResetBuffers();

	return true;
}
bool HorusImporterManager::ExportGltf(const std::string& Path, rpr_context Context, rpr_scene* Scene)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusTimerManager& TimerManager = HorusTimerManager::GetInstance();

	TimerManager.StartTimer(ExportGltfTimer);

	if (Path.empty() || !Context || !Scene)
	{
		spdlog::error("Invalid parameters for exporting GLTF scene");
		TimerManager.StopTimer(ExportGltfTimer);
		return false;
	}

	if (rpr_int Status = rprExportToGLTF(Path.c_str(), Context, nullptr, Scene, 1, 0, nullptr); Status != RPR_SUCCESS)
	{
		spdlog::error("Failed to export GLTF scene: {}", Path);
		TimerManager.StopTimer(ExportGltfTimer);
		return false;
	}

	if (auto TimeExport = TimerManager.StopTimer(ExportGltfTimer); TimeExport > 1000)
		spdlog::info("Exported GLTF scene: {} in {} seconds", Path, TimeExport / 1000.0f);
	else
		spdlog::info("Exported GLTF scene: {} in {} milliseconds", Path, TimeExport);

	return true;
}

