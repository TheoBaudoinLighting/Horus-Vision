#pragma once

#include <RadeonProRender.hpp>
#include <vector>

#include <glm/glm.hpp>

class HorusGroupShape
{
public:

	void GetGroupShapeInfo();

	void Init(const char* Path);
	void DestroyGroupShape();

	// Getters Group Shape
	[[nodiscard]] std::vector<std::pair<rpr_shape, std::string>> GetShapeAndName();
	[[nodiscard]] std::vector<rpr_shape> GetShape();
	[[nodiscard]] std::vector<std::string> GetShapeName();
	[[nodiscard]] glm::mat4 GetGroupTransform() const { return m_Transform_; }
	[[nodiscard]] glm::vec3 GetGroupShapePosition() const { return { m_Position_.x, m_Position_.y, m_Position_.z }; }
	[[nodiscard]] glm::vec3 GetGroupShapeRotation() const { return { m_Rotation_.x, m_Rotation_.y, m_Rotation_.z }; }
	[[nodiscard]] glm::vec3 GetGroupShapeScale() const { return { m_Scale_.x, m_Scale_.y, m_Scale_.z }; }

	// Setters Group Shape
	void SetGroupShapePosition(glm::vec3 Pos);
	void SetGroupShapeRotation(glm::vec3 Rot);
	void SetGroupShapeScale(glm::vec3 Scale);
	void SetGroupResetTransform();
	void SetGroupShapeToScene();

	// Getters Shape
	[[nodiscard]] rpr_shape GetShape(const std::string& shapeName);
	[[nodiscard]] glm::vec3 GetShapePosition(const std::string& shapeName);
	[[nodiscard]] glm::vec3 GetShapeRotation(const std::string& shapeName);
	[[nodiscard]] glm::vec3 GetShapeScale(const std::string& shapeName);

	// Setters Shape
	void SetShapeName(const std::string& newName);
	void SetShapePosition(const std::string& shapeName, glm::vec3 Pos);
	void SetShapeRotation(const std::string& shapeName, glm::vec3 Rot);
	void SetShapeScale(const std::string& shapeName, glm::vec3 Scale);
	void ResetShapeTransform(const std::string& shapeName);

private:

	// Group Shape
	void UpdateGroupShapeTransform();

	std::vector<std::tuple<rpr_shape, std::string>> m_Shape_ = {};
	std::vector<rpr_material_node> m_Materials_ = {};

	glm::mat4 m_Transform_ = glm::mat4(1.0f);
	glm::vec3 m_Position_ = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 m_Rotation_ = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 m_Scale_ = glm::vec3(1.0f, 1.0f, 1.0f);

	float m_RotationAngle_ = 0.0f;

	// Shape



};
