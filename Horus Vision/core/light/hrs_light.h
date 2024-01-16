#pragma once

#include <string>

#include "RadeonProRender_v2.h"
#include "common.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct HorusLightData
{
	glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
	glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
	glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

	glm::vec3 Intesity = { 1.f, 1.f, 1.f };
	glm::vec3 Color = { 1.f, 1.f, 1.f };
	float Temperature = 6500.f;

	std::string Path = "";
};

class HorusLight
{
public:

	HorusLight(): m_Transform_()
	{
	}

	~HorusLight() {}

	void Init(const std::string& light_type, const std::string& hdri_image = "");

	rpr_light CreateLight(const std::string& light_type, const std::string& hdri_image = "");

	void DestroyLight();

	void GetInfo();

	void SetPosition(const glm::vec3& position);
	void SetRotation(const glm::vec3& rotation_axis, float rotation_angle);
	void SetScale(const glm::vec3& scale);

	void UpdateLight();

private:

	rpr_light m_Light_ = nullptr;

	glm::vec3 m_Translation_ = { 0.0f, 0.0f, 0.0f };
	glm::vec3 m_Position_ = { 0.0f, 0.0f, 0.0f };
	glm::vec3 m_Rotation_ = { 0.0f, 0.0f, 0.0f };
	glm::vec3 m_Scale_ = { 1.0f, 1.0f, 1.0f };
	glm::vec3 m_RotationAxis_ = { 0.0f, 0.0f, 0.0f };

	float m_RotationAngle_ = 0.0f;

	//RadeonProRender::matrix m_transform_;
	glm::mat4 m_Transform_;
};
