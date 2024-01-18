#include "hrs_mesh.h"

#include "hrs_reset_buffers.h"
#include "hrs_importer.h"
#include "hrs_radeon.h"
#include "hrs_object_manager.h"
#include "hrs_console.h"

void ResetBuffer()
{
	HorusResetBuffers::GetInstance().CallResetBuffers();
}

void HorusMesh::GetMeshInfo()
{
	rprShapeGetInfo(m_Shape_, RPR_SHAPE_TRANSFORM, sizeof(int), &m_Transform_, nullptr);
}

void HorusMesh::Init(const char* Path)
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusMeshImporter& MeshImporter = HorusMeshImporter::GetInstance();
	HorusGarbageCollector& Gc = HorusGarbageCollector::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	m_Shape_ = MeshImporter.LoadMesh(Path);

	GetMeshInfo();

	rpr_scene GScene = ObjectManager.GetScene();
	rprSceneAttachShape(GScene, m_Shape_);
	Gc.Add(m_Shape_);

	spdlog::info("Mesh {} loaded", Path);
	Console.AddLog("Mesh %s loaded", Path);

	ResetBuffer();
}

void HorusMesh::DestroyMesh()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	

	rpr_scene GScene = ObjectManager.GetScene();

	rprSceneDetachShape(GScene, m_Shape_);

	rprObjectDelete(m_Shape_);

	ResetBuffer();
}

void HorusMesh::AssignMaterial(rpr_material_node Material)
{
	HorusConsole& Console = HorusConsole::GetInstance();

	CHECK(rprShapeSetMaterial(m_Shape_, Material))

	spdlog::info("Material correctly assigned to mesh");
	Console.AddLog("[success] Material correctly assigned to mesh");
	ResetBuffer();
}

void HorusMesh::SetShapePosition(glm::vec3 Pos)
{
	GetMeshInfo();

	m_Position_ = Pos;

	UpdateTransform();
}
void HorusMesh::SetShapeRotation(glm::vec3 Rot)
{
	GetMeshInfo();

	m_Rotation_ = Rot;

	UpdateTransform();
}
void HorusMesh::SetShapeScale(glm::vec3 Scale)
{
	GetMeshInfo();

	m_Scale_ = Scale;

	UpdateTransform();
}

void HorusMesh::SetObjectToScene()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	rpr_scene GScene = ObjectManager.GetScene();

	rprSceneAttachShape(GScene, m_Shape_);
}

void HorusMesh::UpdateTransform()
{
	glm::mat4 Translate = translate(glm::mat4(1.0f), m_Position_);
	glm::mat4 RotationX = rotate(glm::mat4(1.0f), glm::radians(m_Rotation_.x), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 RotationY = rotate(glm::mat4(1.0f), glm::radians(m_Rotation_.y), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 RotationZ = rotate(glm::mat4(1.0f), glm::radians(m_Rotation_.z), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 Scale = scale(glm::mat4(1.0f), m_Scale_);

	glm::mat4 Rotation = RotationX * RotationY * RotationZ;

	m_Transform_ = Translate * Rotation * Scale;

	rprShapeSetTransform(m_Shape_, RPR_FALSE, &m_Transform_[0][0]); // RPR_TRUE = transpose matrix in row
}



