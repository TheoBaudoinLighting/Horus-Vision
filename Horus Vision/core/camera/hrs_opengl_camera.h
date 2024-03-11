#pragma once

// This file is created after Radeon Camera so, here i will use just the variables provided by Radeon Camera
#include "hrs_shader_manager.h"

#include "glm/glm.hpp"
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class HorusOpenGLCamera
{
public:

	HorusOpenGLCamera() = default;
	~HorusOpenGLCamera() {}

	void Init(int Width, int Height);
	void Reset();
	void Destroy();
	void Matrix(GLuint& ProgramID, const char* Name);
	void UpdateMatrix(float Fov, float Near, float Far, int Width, int Height);

	void VariableCheckers(std::string Name);
	void PrintCameraInfo();
	void UpdateCamera();
	void SendToShader(const HorusShaderManager& Shader);
	void GetMatrices(glm::mat4& view, glm::mat4& projection, glm::mat4& mvp);

	// Movement
	void Tumble(float Pitch, float Yaw, float sensitivity);
	void Pan(float X, float Y, float sensitivity);
	void Zoom(float Z);

	// Getters
	glm::vec3 GetLookAt();
	glm::vec3 GetPosition();
	glm::vec3 GetTranslation();
	glm::vec3 GetRotation();
	glm::vec3 GetUp();
	glm::vec3 GetRight();
	glm::vec3 GetDirection();
	glm::vec3 GetPitchAxis();
	glm::vec3 GetHeadingAxis();
	glm::vec3 GetScale();

	double GetFov();
	double GetNear();
	double GetFar();
	double GetFStop();
	double GetFocusPlane();
	double GetAspectRatio();
	int GetApertureBlades();

	glm::mat4 GetViewMatrix() { return m_ViewMatrix_; }
	glm::mat4 GetProjectionMatrix() { return m_ProjectionMatrix_; }

	// Setters
	void SetPosition(glm::vec3 position);
	void SetCameraRotation(glm::vec3 rotation);
	void SetCameraScale(glm::vec3 scale);
	void SetLookAt(glm::vec3 lookAt);
	void SetFov(float fov);
	void SetViewport(int x, int y, int width, int height);
	void SetNear(float nearPlane);
	void SetFar(float farPlane);
	void SetAspectRatio(int Width, int Height);
	void SetAspectRatio(float AspectRatio);

private:

	int m_ViewportX_ = 500;
	int m_ViewportY_ = 500;

	int m_WindowWidth_ = 500;
	int m_WindowHeight_ = 500;

	float m_AspectRatio_ = 1.0f;
	float m_FOV_ = 45.0f;
	float m_FStop_ = 1.4f;
	float m_NearPlane_ = 0.1f;
	float m_FarPlane_ = 1000.0f;
	float m_FocusPlane_ = 10.0f;
	int m_ApertureBlades_ = 5;

	glm::mat4 m_Transform_;
	glm::vec3 m_LookAt_;
	glm::vec3 m_Up_;
	glm::vec3 m_Position_;
	glm::vec3 m_Rotation_;
	glm::vec3 m_Scale_;
	glm::vec3 m_Direction_;
	glm::vec3 m_Right_;
	glm::vec3 m_PitchAxis_;
	glm::vec3 m_HeadingAxis_;

	glm::mat4 m_ModelMatrix_;
	glm::mat4 m_ViewMatrix_;
	glm::mat4 m_ProjectionMatrix_;
	glm::mat4 m_Mvp_;
};