#pragma once

#include <string>

#include "common.h"
#include "RadeonProRender_v2.h"
#include "Math/float3.h"
#include "Math/float4.h"

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

	void set_position(const RadeonProRender::float3& position);
	void set_rotation(const RadeonProRender::float3& rotation);
	void set_scale(const RadeonProRender::float3& scale);

private:

	rpr_light m_light = nullptr;
};
