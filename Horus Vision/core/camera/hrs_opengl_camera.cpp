#include "hrs_opengl_camera.h"

#include <coroutine>

#include "glad/glad.h"
#include "spdlog/spdlog.h"

void HorusOpenGLCamera::Init()
{
	m_CameraMode_ = PERSPECTIVE;
	m_Up_ = glm::vec3(0.0f, 1.0f, 0.0f);
	m_FOV_ = 45.0f;
	m_Position_ = glm::vec3(0.0f, 9.0f, 40.0f); // glm::vec3(0.0f, 9.0f, 40.0f);
	m_Transform_ = glm::vec3(0.0f, 0.0f, 0.0f);
	m_AspectRatio_ = float(m_WindowWidth_) / float(m_WindowHeight_);
	m_NearPlane_ = 0.1f;
	m_FarPlane_ = 10000.0f;
	m_Scale_ = glm::vec3(1.0f, 1.0f, 1.0f);

	spdlog::info("OpenGL Camera created");
}
void HorusOpenGLCamera::Reset()
{
	//m_CameraType_ = Free;
	m_Up_ = glm::vec3(0.0f, 1.0f, 0.0f);
	m_FOV_ = 45.0f;
	m_Position_ = glm::vec3(0.0f, 0.0f, 0.0f); // glm::vec3(0.0f, 9.0f, 40.0f);
	m_Transform_ = glm::vec3(0.0f, 0.0f, 0.0f);
	m_AspectRatio_ = float(m_WindowWidth_) / float(m_WindowHeight_);
	m_NearPlane_ = 0.1f;
	m_FarPlane_ = 10000.0f;
	m_Scale_ = glm::vec3(1.0f, 1.0f, 1.0f);
}
void HorusOpenGLCamera::Destroy()
{
}
void HorusOpenGLCamera::GetCameraInfo()
{
}
void HorusOpenGLCamera::UpdateCamera()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(m_ViewportX_, m_ViewportY_, m_WindowWidth_, m_WindowHeight_);

	if (m_CameraMode_ == PERSPECTIVE)
	{
		m_ProjectionMatrix_ = glm::perspective(m_FOV_, m_AspectRatio_, m_NearPlane_, m_FarPlane_);
	}
	else if (m_CameraMode_ == ORTHOGRAPHIC)
	{
		m_ProjectionMatrix_ = glm::ortho(0.0f, (float)m_WindowWidth_, 0.0f, (float)m_WindowHeight_, m_NearPlane_, m_FarPlane_);
	}

	m_ViewMatrix_ = glm::lookAt(m_Position_, m_LookAt_, m_Up_);
	m_ModelMatrix_ = glm::mat4(1.0f);
	m_Mvp_ = m_ProjectionMatrix_ * m_ViewMatrix_ * m_ModelMatrix_;
}

void HorusOpenGLCamera::SendToShader(const HorusShaderManager& Shader)
{
	m_ViewMatrix_ = glm::lookAt(m_Position_, m_LookAt_, m_Up_);
	Shader.SendData("proj", m_ProjectionMatrix_);
	Shader.SendData("view", m_ViewMatrix_);
	Shader.SendData("cameraPosition", m_Position_.x , m_Position_.y, m_Position_.z);
}

void HorusOpenGLCamera::GetMatricies(glm::mat4& view, glm::mat4& projection, glm::mat4& mvp)
{
	view = m_ViewMatrix_;
	projection = m_ProjectionMatrix_;
	mvp = m_Mvp_;
}


glm::vec3 HorusOpenGLCamera::GetLookAt()
{
	return m_LookAt_;
}
glm::vec3 HorusOpenGLCamera::GetPosition()
{
	return m_Position_;
}
glm::vec3 HorusOpenGLCamera::GetTranslation()
{
	return m_Position_;
}
glm::vec3 HorusOpenGLCamera::GetRotation()
{
	return m_Rotation_;
}
glm::vec3 HorusOpenGLCamera::GetUp()
{
	return m_Up_;
}
glm::vec3 HorusOpenGLCamera::GetRight()
{
	return m_Right_;
}
glm::vec3 HorusOpenGLCamera::GetDirection()
{
	return m_Direction_;
}
glm::vec3 HorusOpenGLCamera::GetPitchAxis()
{
	return m_PitchAxis_;
}
glm::vec3 HorusOpenGLCamera::GetHeadingAxis()
{
	return m_HeadingAxis_;
}
glm::vec3 HorusOpenGLCamera::GetCameraScale()
{
	return m_Scale_;
}

double HorusOpenGLCamera::GetFov()
{
	return m_FOV_;
}
double HorusOpenGLCamera::GetNear()
{
	return m_NearPlane_;
}
double HorusOpenGLCamera::GetFar()
{
	return m_FarPlane_;
}
double HorusOpenGLCamera::GetFStop()
{
	return 0.0;
}
double HorusOpenGLCamera::GetAspect()
{
	return m_AspectRatio_;
}

void HorusOpenGLCamera::SetMode(CameraMode mode)
{
	if (mode == PERSPECTIVE)
	{
		m_ProjectionMatrix_ = glm::perspective(m_FOV_, m_AspectRatio_, m_NearPlane_, m_FarPlane_);
	}
	else if (mode == ORTHOGRAPHIC)
	{
		m_ProjectionMatrix_ = glm::ortho(-1.5f * float(m_AspectRatio_), 1.5f * float(m_AspectRatio_), -1.5f, 1.5f, -10.0f, 10.f);
	}

	m_ViewMatrix_ = glm::lookAt(m_Position_, m_LookAt_, m_Up_);
	m_ModelMatrix_ = glm::mat4(1.0f);
	m_Mvp_ = m_ProjectionMatrix_ * m_ViewMatrix_ * m_ModelMatrix_;

}
void HorusOpenGLCamera::SetPosition(glm::vec3 position)
{
	m_Position_ = position;
}
void HorusOpenGLCamera::SetCameraRotation(glm::vec3 rotation)
{
	m_Rotation_ = rotation;
}
void HorusOpenGLCamera::SetCameraScale(glm::vec3 scale)
{
	m_Scale_ = scale;
}
void HorusOpenGLCamera::SetLookAt(glm::vec3 lookAt)
{
	m_LookAt_ = lookAt;
}
void HorusOpenGLCamera::SetFov(float fov)
{
	m_FOV_ = fov;
}
void HorusOpenGLCamera::SetViewport(int x, int y, int width, int height)
{
	m_ViewportX_ = x;
	m_ViewportY_ = y;
	m_WindowWidth_ = width;
	m_WindowHeight_ = height;
	m_AspectRatio_ = float(m_WindowWidth_) / float(m_WindowHeight_);
}
void HorusOpenGLCamera::SetClipping(float nearPlane, float farPlane)
{
	m_NearPlane_ = nearPlane;
	m_FarPlane_ = farPlane;
}
void HorusOpenGLCamera::SetAspectRatio(float aspectRatio)
{
	m_AspectRatio_ = aspectRatio;
}
