#pragma once

// This file is created after Radeon Camera so, here i will use just the variables provided by Radeon Camera
#include "hrs_shader_manager.h"

#include "glm/glm.hpp"
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


enum CameraMovement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

enum CameraMode
{
	ORTHOGRAPHIC,
	PERSPECTIVE
};

class HorusOpenGLCamera
{
public:

	/*static HorusOpenGLCamera& GetInstance()
	{
		static HorusOpenGLCamera Instance;
		return Instance;
	}

	HorusOpenGLCamera(HorusOpenGLCamera const&) = delete;
	void operator=(HorusOpenGLCamera const&) = delete;*/

	HorusOpenGLCamera() = default;
	~HorusOpenGLCamera() {}

	void Init();
	void Reset();
	void Destroy();

	void GetCameraInfo();
	void UpdateCamera();
	void SendToShader(const HorusShaderManager& Shader);
	void GetMatricies(glm::mat4& view, glm::mat4& projection, glm::mat4& mvp);

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
	glm::vec3 GetCameraScale();

	double GetFov();
	double GetNear();
	double GetFar();
	double GetFStop();
	double GetAspect();

	// Setters
	void SetMode(CameraMode mode);
	void SetPosition(glm::vec3 position);
	void SetCameraRotation(glm::vec3 rotation);
	void SetCameraScale(glm::vec3 scale);
	void SetLookAt(glm::vec3 lookAt);
	void SetFov(float fov);
	void SetViewport(int x, int y, int width, int height);
	void SetClipping(float nearPlane, float farPlane);
	void SetAspectRatio(float aspectRatio);

private:

	

	CameraMode m_CameraMode_;

	int m_ViewportX_;
	int m_ViewportY_;

	int m_WindowWidth_;
	int m_WindowHeight_;

	float m_AspectRatio_;
	float m_FOV_;
	float m_NearPlane_;
	float m_FarPlane_;
	
	glm::vec3 m_Transform_;
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