#pragma once

#include <RadeonProRender_v2.h>

#include <glm/glm.hpp>

class HorusMesh
{
public:

	void GetMeshInfo();

	void Init(const char* path);
	void DestroyMesh();

	void AssignMaterial(rpr_material_node material);

	// Getters
	rpr_shape GetShape() const { return m_Shape_; }
	glm::mat4 GetTransform() const { return m_Transform_; }
	glm::vec3 GetShapePosition() const { return glm::vec3(m_Position_.x, m_Position_.y, m_Position_.z); }
	glm::vec3 GetShapeRotation() const { return glm::vec3(m_Rotation_.x, m_Rotation_.y, m_Rotation_.z); }
	glm::vec3 GetShapeScale() const { return glm::vec3(m_Scale_.x, m_Scale_.y, m_Scale_.z); }

	// Setters
	void SetShapePosition(glm::vec3 pos);
	void SetShapeRotation(glm::vec3 rot);
	void SetShapeScale(glm::vec3 scale);

	void SetObjectToScene(rpr_scene& scene);

private:

	void UpdateTransform();

	rpr_shape m_Shape_ = nullptr;

	glm::mat4 m_Transform_ = glm::mat4(1.0f);
	glm::vec3 m_Position_ = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 m_Rotation_ = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 m_Scale_ = glm::vec3(1.0f, 1.0f, 1.0f);

	float m_RotationAngle_ = 0.0f;



};
