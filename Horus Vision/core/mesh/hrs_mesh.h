#pragma once

// C++ Standard Library
#include <iostream>
#include <vector>

// Radeon ProRender
#include <RadeonProRender.hpp>

// GLM
#include <glm/glm.hpp>

// Horus
#include "hrs_opengl_shape.h"
#include "hrs_opengl_camera.h"
#include "hrs_opengl_texture.h"
#include "hrs_vao.h"
#include "hrs_vbo.h"
#include "hrs_ebo.h"

// Horus Shape = HorusOpenGlShape + HorusRadeonShape / 2 in 1
class HorusShape
{
public:

	void Init(const char* Path);
	void InitOpenGlShape(const char* Path);
	void DestroyGroupShape();

	// Getters Group Shape
	std::vector<std::pair<rpr_shape, std::string>> GetShapeAndName();
	std::vector<std::pair<HorusOpenGlShape, std::string>> GetOpenGlShapeAndName();
	std::vector<rpr_shape> GetShape();
	std::vector<std::string> GetShapeName();
	glm::mat4 GetGroupTransform() const { return m_Transform_; }
	glm::vec3 GetGroupShapePosition() const { return { m_Position_.x, m_Position_.y, m_Position_.z }; }
	glm::vec3 GetGroupShapeRotation() const { return { m_Rotation_.x, m_Rotation_.y, m_Rotation_.z }; }
	glm::vec3 GetGroupShapeScale() const { return { m_Scale_.x, m_Scale_.y, m_Scale_.z }; }

	// Setters Group Shape
	void SetGroupShapePosition(glm::vec3 Pos);
	void SetGroupShapeRotation(glm::vec3 Rot);
	void SetGroupShapeScale(glm::vec3 Scale);
	void SetGroupResetTransform();
	void SetGroupShapeToScene();

	// Getters Shape
	[[nodiscard]] rpr_shape GetShape(const std::string& shapeName);
	[[nodiscard]] HorusOpenGlShape GetOpenGlShape(const std::string& shapeName);

private:

	// Group Shape
	void UpdateGroupShapeTransform();

	std::vector<std::tuple<rpr_shape, std::string>> mRadeonShape_ = {};
	std::vector<std::tuple<HorusOpenGlShape, std::string>> mOpenGlShape_ = {};
	std::map<int, rpr_shape> mRadeonShapes_;
	std::map<int, HorusOpenGlShape> mOpenGlShapes_;
	std::vector<rpr_material_node> m_Materials_ = {};

	glm::mat4 m_Transform_ = glm::mat4(1.0f);
	glm::vec3 m_Position_ = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 m_Rotation_ = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 m_Scale_ = glm::vec3(1.0f, 1.0f, 1.0f);

	float m_RotationAngle_ = 0.0f;
};
