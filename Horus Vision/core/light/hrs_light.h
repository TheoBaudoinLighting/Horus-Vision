#pragma once

#include <string>

#include "hrs_garbage_collector.h"
#include "RadeonProRender_v2.h"
#include "common.h"
#include "Math/mathutils.h"

struct HorusLightData
{
	RadeonProRender::float3 position = { 0.0f, 0.0f, 0.0f };
	RadeonProRender::float3 rotation = { 0.0f, 0.0f, 0.0f };
	RadeonProRender::float3 scale = { 1.0f, 1.0f, 1.0f };

	RadeonProRender::float3 intesity = { 1.f, 1.f, 1.f };
	RadeonProRender::float3 color = { 1.f, 1.f, 1.f };
	float temperature = 6500.f;

	std::string path = "";
};

class HorusLight
{
public:

	HorusLight() {}
	~HorusLight() {}

	void init(const std::string& light_type, const std::string& hdri_image = "");

	rpr_light create_light(const std::string& light_type, const std::string& hdri_image = "");

	void destroy_light();

	void get_info();

	void set_position(const RadeonProRender::float3& position);
	void set_rotation(RadeonProRender::float3 rotation_axis, float rotation_angle);
	void set_scale(const RadeonProRender::float3& scale);

	void update_light();

private:

	rpr_light m_light = nullptr;

	RadeonProRender::float3 m_translation_ = { 0.0f, 0.0f, 0.0f };
	RadeonProRender::float3 m_position_ = { 0.0f, 0.0f, 0.0f };
	RadeonProRender::float4 m_rotation_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	RadeonProRender::float3 m_scale_ = { 1.0f, 1.0f, 1.0f };
	RadeonProRender::float3 m_rotation_axis_ = { 0.0f, 0.0f, 0.0f };

	float m_rotation_angle_ = 0.0f;

	RadeonProRender::matrix m_transform_;
};
