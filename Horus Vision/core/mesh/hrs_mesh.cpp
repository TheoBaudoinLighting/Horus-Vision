#include "hrs_mesh.h"

#include <Math/mathutils.h>

#include "hrs_importer.h"
#include "hrs_material.h"
#include "hrs_radeon.h"
#include "hrs_object_manager.h"

//HorusRadeon& Radeon = HorusRadeon::get_instance();
//HorusObjectManager& ObjectManager = HorusObjectManager::get_instance();
//HorusMeshImporter& MeshImporter = HorusMeshImporter::get_instance();

void HorusMesh::init(const char* path, const char* name)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();
	HorusObjectManager& ObjectManager = HorusObjectManager::get_instance();
	HorusMeshImporter& MeshImporter = HorusMeshImporter::get_instance();

	m_shape_ = MeshImporter.load_mesh(path);

	rpr_scene g_scene = nullptr;
	g_scene = ObjectManager.get_scene();

	rprSceneAttachShape(g_scene, m_shape_);

	RPRGarbageCollector m_gc_ = Radeon.get_gc();

	m_gc_.GCAdd(m_shape_);
}

void HorusMesh::destroy_mesh()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::get_instance();

	rpr_scene g_scene = nullptr;
	g_scene = ObjectManager.get_scene();

	rprSceneDetachShape(g_scene, m_shape_);

	rprObjectDelete(m_shape_);

	spdlog::info("Mesh destroyed");
}

void HorusMesh::assign_material(rpr_material_node material)
{
	CHECK(rprShapeSetMaterial(m_shape_, material));
}

void HorusMesh::get_info()
{
	rprShapeGetInfo(m_shape_, RPR_SHAPE_TRANSFORM, sizeof(int), &m_transform_, nullptr);
	//rprShapeGetInfo(m_shape_, RPR_SHAPE_MATERIAL, sizeof(int), &m_material_, nullptr);
}

void HorusMesh::set_shape_position(RadeonProRender::float3 pos)
{
	get_info();

	m_position_ = pos;

	m_transform_.m03 = m_position_.x;
	m_transform_.m13 = m_position_.y;
	m_transform_.m23 = m_position_.z;

	update_transform();
}

void HorusMesh::set_shape_rot(RadeonProRender::float3 rotation_axis, float rotation_angle)
{
	get_info();

	m_rotation_axis_ = rotation_axis;
	m_rotation_angle_ = rotation_angle;

	m_transform_ = m_transform_ * RadeonProRender::rotation(m_rotation_axis_, m_rotation_angle_);

	update_transform();
}

void HorusMesh::set_shape_scale(RadeonProRender::float3 scale)
{
	get_info();

	m_scale_ = scale;

	m_transform_.m00 = m_scale_.x;
	m_transform_.m11 = m_scale_.y;
	m_transform_.m22 = m_scale_.z;

	update_transform();
}

void HorusMesh::set_object_to_scene(rpr_scene& scene)
{
	HorusObjectManager& ObjectManager = HorusObjectManager::get_instance();

	rpr_scene g_scene = nullptr;
	g_scene = ObjectManager.get_scene();

	rprSceneAttachShape(g_scene, m_shape_);
}

void HorusMesh::update_transform()
{
	m_transform_ = RadeonProRender::translation(m_position_) * RadeonProRender::rotation(m_rotation_axis_, m_rotation_angle_) * RadeonProRender::scale(m_scale_);

	CHECK(rprShapeSetTransform(m_shape_, false, &m_transform_.m00));
}



