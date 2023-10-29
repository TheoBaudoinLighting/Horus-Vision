#pragma once
#include <map>
#include <RadeonProRender_v2.h>
#include <Math/quaternion.h>

class HorusMesh
{
public:

	void init(const char* path, const char* name);
	void destroy_mesh();

	void assign_material(rpr_material_node material);

	void get_info();

	RadeonProRender::matrix get_transform() const { return m_transform_; }

	void set_shape_position(RadeonProRender::float3 pos);
	void set_shape_rot(RadeonProRender::float3 rotation_axis, float rotation_angle);
	void set_shape_scale(RadeonProRender::float3 scale);

	void set_object_to_scene(rpr_scene& scene);

	rpr_shape get_shape() const { return m_shape_; }

private:

	void update_transform();

	rpr_shape m_shape_;

	RadeonProRender::float3 m_translation_ = { 0.0f, 0.0f, 0.0f };
	RadeonProRender::float3 m_position_ = { 0.0f, 0.0f, 0.0f };
	RadeonProRender::float4 m_rotation_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	RadeonProRender::float3 m_scale_ = { 1.0f, 1.0f, 1.0f };

	RadeonProRender::matrix m_transform_;

	std::map<int, std::string> m_mesh_names_;
	float m_rotation_angle_ = 0.0f;
	RadeonProRender::float3 m_rotation_axis_ = { 0.0f, 0.0f, 0.0f };
};
