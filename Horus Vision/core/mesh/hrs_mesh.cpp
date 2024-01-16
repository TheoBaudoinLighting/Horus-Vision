#include "hrs_mesh.h"

#include <Math/mathutils.h>
#include <Math/matrix.h>

#include "hrs_importer.h"
#include "hrs_material.h"
#include "hrs_radeon.h"
#include "hrs_object_manager.h"
#include <hrs_console.h>

void HorusMesh::GetMeshInfo()
{
	rprShapeGetInfo(m_Shape_, RPR_SHAPE_TRANSFORM, sizeof(int), &m_Transform_, nullptr);
}

void HorusMesh::Init(const char* path)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusMeshImporter& MeshImporter = HorusMeshImporter::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::get_instance();
	HorusConsole& Console = HorusConsole::GetInstance();

	m_Shape_ = MeshImporter.LoadMesh(path);

	GetMeshInfo();

	rpr_scene g_scene = nullptr;
	g_scene = ObjectManager.GetScene();

	rprSceneAttachShape(g_scene, m_Shape_);
	gc.Add(m_Shape_);

	spdlog::info("Mesh {} loaded", path);
	Console.AddLog("Mesh %s loaded", path);
}

void HorusMesh::DestroyMesh()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	rpr_scene GScene = ObjectManager.GetScene();

	rprSceneDetachShape(GScene, m_Shape_);

	rprObjectDelete(m_Shape_);
}

void HorusMesh::AssignMaterial(rpr_material_node material)
{
	CHECK(rprShapeSetMaterial(m_Shape_, material));
}

void HorusMesh::SetShapePosition(glm::vec3 pos)
{
	GetMeshInfo();

	m_Position_ = pos;

	UpdateTransform();
}
void HorusMesh::SetShapeRotation(glm::vec3 rot)
{
	GetMeshInfo();

	m_Rotation_ = rot;

	UpdateTransform();
}
void HorusMesh::SetShapeScale(glm::vec3 scale)
{
	GetMeshInfo();

	m_Scale_ = scale;

	UpdateTransform();
}

void HorusMesh::SetObjectToScene(rpr_scene& scene)
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	rpr_scene GScene = ObjectManager.GetScene();

	rprSceneAttachShape(GScene, m_Shape_);
}

void HorusMesh::UpdateTransform()
{
	glm::mat4 Translate = glm::translate(glm::mat4(1.0f), m_Position_);
	glm::mat4 RotationX = glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation_.x), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 RotationY = glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation_.y), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 RotationZ = glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation_.z), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 Scale = glm::scale(glm::mat4(1.0f), m_Scale_);

	glm::mat4 Rotation = RotationX * RotationY * RotationZ;

	m_Transform_ = Translate * Rotation * Scale;

	rprShapeSetTransform(m_Shape_, RPR_TRUE, &m_Transform_[0][0]);
}



