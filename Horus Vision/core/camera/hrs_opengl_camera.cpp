#include "hrs_opengl_camera.h"

#include <iostream>
using namespace std;

#include "glad/glad.h"
#include "spdlog/spdlog.h"

void HorusOpenGLCamera::Init(int Width, int Height)
{
	m_WindowWidth_ = Width;
	m_WindowHeight_ = Height;

	m_Up_ = glm::vec3(0.0f, 1.0f, 0.0f);
	m_LookAt_ = glm::vec3(0.0f, 0.0f, 0.0f);
	m_Position_ = glm::vec3(28.0f, 21.0f, 28.0f); // glm::vec3(0.0f, 9.0f, 40.0f); -> maya is glm::vec3(28.0f, 21.0f, 28.0f);
	m_Transform_ = glm::mat4(1.0f);
	m_AspectRatio_ = float(m_WindowWidth_) / float(m_WindowHeight_);
	m_FOV_ = 45.0f;
	m_FStop_ = 100.0f;
	m_NearPlane_ = 0.1f;
	m_FarPlane_ = 1000.0f;
	m_Scale_ = glm::vec3(1.0f, 1.0f, 1.0f);
	m_FocusPlane_ = 7.5f;
	m_ApertureBlades_ = 5;

	UpdateMatrix(m_FOV_, m_NearPlane_, m_FarPlane_, m_WindowWidth_, m_WindowHeight_);

	spdlog::info("OpenGL Camera created");
}
void HorusOpenGLCamera::Reset()
{
	//m_CameraType_ = Free;
	m_Up_ = glm::vec3(0.0f, 1.0f, 0.0f);
	m_FOV_ = 45.0f;
	m_Position_ = glm::vec3(0.0f, 9.0f, 40.0f); // glm::vec3(0.0f, 9.0f, 40.0f);
	m_Transform_ = glm::mat4(1.0f);
	m_AspectRatio_ = float(m_WindowWidth_) / float(m_WindowHeight_);
	m_NearPlane_ = 0.1f;
	m_FarPlane_ = 1000.0f;
	m_Scale_ = glm::vec3(1.0f, 1.0f, 1.0f);

	UpdateMatrix(m_FOV_, m_NearPlane_, m_FarPlane_, m_WindowWidth_, m_WindowHeight_);
}
void HorusOpenGLCamera::Destroy()
{
}
void HorusOpenGLCamera::Matrix(GLuint& ProgramID, const char* Name)
{
	glUniformMatrix4fv(glGetUniformLocation(ProgramID, Name), 1, GL_FALSE, glm::value_ptr(m_Mvp_));
}
void HorusOpenGLCamera::UpdateMatrix(float Fov, float Near, float Far, int Width, int Height)
{
	m_WindowWidth_ = Width;
	m_WindowHeight_ = Height;

	m_FOV_ = Fov;
	m_NearPlane_ = Near;
	m_FarPlane_ = Far;
	m_AspectRatio_ = float(m_WindowWidth_) / float(m_WindowHeight_);

	m_ProjectionMatrix_ = glm::perspective(m_FOV_, m_AspectRatio_, m_NearPlane_, m_FarPlane_);
	m_ViewMatrix_ = glm::lookAt(m_Position_, m_LookAt_, m_Up_);
	m_ModelMatrix_ = glm::mat4(1.0f);

	m_Mvp_ = m_ProjectionMatrix_ * m_ViewMatrix_ * m_ModelMatrix_;
}
void HorusOpenGLCamera::VariableCheckers(std::string Name)
{
	cout << "Start info --------------------------" << '\n';
	cout << "Camera OpenGL transform : " << m_Transform_[0][0] << m_Transform_[0][1] << m_Transform_[0][2] << m_Transform_[0][3] << '\n';
	cout << "Camera OpenGL look at : " << m_LookAt_.x << " " << m_LookAt_.y << " " << m_LookAt_.z << '\n';
	cout << "Camera OpenGL up : " << m_Up_.x << " " << m_Up_.y << " " << m_Up_.z << '\n';
	cout << "Camera OpenGL position : " << m_Position_.x << " " << m_Position_.y << " " << m_Position_.z << '\n';
	cout << "Camera OpenGL fstop : " << m_FStop_ << '\n';
	cout << "Camera OpenGL near : " << m_NearPlane_ << '\n';
	cout << "Camera OpenGL far : " << m_FarPlane_ << '\n';
	cout << "Camera OpenGL aspect : " << m_AspectRatio_ << '\n';
	cout << "Camera OpenGL fov : " << m_FOV_ << '\n';
	cout << "Camera OpenGL viewport x : " << m_ViewportX_ << '\n';
	cout << "Camera OpenGL viewport y : " << m_ViewportY_ << '\n';
	cout << "Camera OpenGL window width : " << m_WindowWidth_ << '\n';
	cout << "Camera OpenGL window height : " << m_WindowHeight_ << '\n';
	cout << "Camera OpenGL focus distance : " << m_FocusPlane_ << '\n';
	cout << "Camera OpenGL instance : " << this << '\n';
	cout << "End info --------------------------" << '\n';
}
void HorusOpenGLCamera::PrintCameraInfo()
{
	VariableCheckers("Camera OpenGL Manual Check");
}
void HorusOpenGLCamera::UpdateCamera()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(m_ViewportX_, m_ViewportY_, m_WindowWidth_, m_WindowHeight_);

	m_ProjectionMatrix_ = glm::perspective(m_FOV_, m_AspectRatio_, m_NearPlane_, m_FarPlane_);

	m_ViewMatrix_ = glm::lookAt(m_Position_, m_LookAt_, m_Up_);
	m_ModelMatrix_ = glm::mat4(1.0f);
	m_Mvp_ = m_ProjectionMatrix_ * m_ViewMatrix_ * m_ModelMatrix_;

	UpdateMatrix(m_FOV_, m_NearPlane_, m_FarPlane_, m_WindowWidth_, m_WindowHeight_);
}

void HorusOpenGLCamera::SendToShader(const HorusShaderManager& Shader)
{
	m_ViewMatrix_ = glm::lookAt(m_Position_, m_LookAt_, m_Up_);
	Shader.SendData("proj", m_ProjectionMatrix_);
	Shader.SendData("view", m_ViewMatrix_);
	Shader.SendData("cameraPosition", m_Position_.x, m_Position_.y, m_Position_.z);
}

void HorusOpenGLCamera::GetMatrices(glm::mat4& view, glm::mat4& projection, glm::mat4& mvp)
{
	view = m_ViewMatrix_;
	projection = m_ProjectionMatrix_;
	mvp = m_Mvp_;
}

void HorusOpenGLCamera::Tumble(float Pitch, float Yaw, float sensitivity)
{
	const float AngleX = -Pitch * sensitivity;
	const float AngleY = Yaw * sensitivity;

	glm::vec3 CameraDirection = glm::normalize(m_Position_ - m_LookAt_);
	const float Radius = glm::length(m_Position_ - m_LookAt_);

	glm::vec3 Axis = glm::cross(CameraDirection, m_Up_);

	// (inversed, yeah I know :D)
	glm::quat QuatPitch = glm::angleAxis(AngleY, Axis);
	glm::quat QuatHeading = glm::angleAxis(AngleX, m_Up_);

	glm::quat QuatRotation = glm::normalize(glm::cross(QuatPitch, QuatHeading));

	CameraDirection = glm::rotate(QuatRotation, CameraDirection);

	m_Position_ = m_LookAt_ + CameraDirection * Radius;

	UpdateMatrix(m_FOV_, m_NearPlane_, m_FarPlane_, m_WindowWidth_, m_WindowHeight_);
}

void HorusOpenGLCamera::Pan(float X, float Y, float sensitivity)
{
	const glm::vec3 Forward = glm::normalize(m_LookAt_ - m_Position_);
	const glm::vec3 Right = glm::normalize(glm::cross(Forward, m_Up_));
	const glm::vec3 Up = glm::normalize(m_Up_);

	const glm::vec3 Displacement = (Right * X + Up * (-Y)) * sensitivity;

	m_Position_ += Displacement;
	m_LookAt_ += Displacement;

	UpdateMatrix(m_FOV_, m_NearPlane_, m_FarPlane_, m_WindowWidth_, m_WindowHeight_);
}

void HorusOpenGLCamera::Zoom(float Z)
{
	m_Direction_ = glm::normalize(m_LookAt_ - m_Position_);
	m_Right_ = glm::normalize(glm::cross(m_Direction_, m_Up_));

	const glm::vec3 TempTranslation = m_Direction_ * Z;

	if (glm::length(m_Position_ + TempTranslation) > 0.1f)
		m_Position_ = m_Position_ + TempTranslation;

	UpdateMatrix(m_FOV_, m_NearPlane_, m_FarPlane_, m_WindowWidth_, m_WindowHeight_);
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
glm::vec3 HorusOpenGLCamera::GetScale()
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
double HorusOpenGLCamera::GetFocusPlane()
{
	return 0.0;
}
double HorusOpenGLCamera::GetAspectRatio()
{
	return m_AspectRatio_;
}

int HorusOpenGLCamera::GetApertureBlades()
{
	return 0;
}

//void HorusOpenGLCamera::SetMode(CameraMode mode)
//{
//	if (mode == PERSPECTIVE)
//	{
//		m_ProjectionMatrix_ = glm::perspective(m_FOV_, m_AspectRatio_, m_NearPlane_, m_FarPlane_);
//	}
//	else if (mode == ORTHOGRAPHIC)
//	{
//		m_ProjectionMatrix_ = glm::ortho(-1.5f * float(m_AspectRatio_), 1.5f * float(m_AspectRatio_), -1.5f, 1.5f, -10.0f, 10.f);
//	}
//
//	m_ViewMatrix_ = glm::lookAt(m_Position_, m_LookAt_, m_Up_);
//	m_ModelMatrix_ = glm::mat4(1.0f);
//	m_Mvp_ = m_ProjectionMatrix_ * m_ViewMatrix_ * m_ModelMatrix_;
//
//}
void HorusOpenGLCamera::SetPosition(glm::vec3 position)
{
	m_Position_ = position;

	UpdateMatrix(m_FOV_, m_NearPlane_, m_FarPlane_, m_WindowWidth_, m_WindowHeight_);
}
void HorusOpenGLCamera::SetCameraRotation(glm::vec3 rotation)
{
	m_Rotation_ = rotation;

	UpdateMatrix(m_FOV_, m_NearPlane_, m_FarPlane_, m_WindowWidth_, m_WindowHeight_);
}
void HorusOpenGLCamera::SetCameraScale(glm::vec3 scale)
{
	m_Scale_ = scale;

	UpdateMatrix(m_FOV_, m_NearPlane_, m_FarPlane_, m_WindowWidth_, m_WindowHeight_);
}
void HorusOpenGLCamera::SetLookAt(glm::vec3 lookAt)
{
	m_LookAt_ = lookAt;

	UpdateMatrix(m_FOV_, m_NearPlane_, m_FarPlane_, m_WindowWidth_, m_WindowHeight_);
}
void HorusOpenGLCamera::SetFov(float fov)
{
	m_FOV_ = fov;

	UpdateMatrix(m_FOV_, m_NearPlane_, m_FarPlane_, m_WindowWidth_, m_WindowHeight_);
}
void HorusOpenGLCamera::SetViewport(int x, int y, int width, int height)
{
	m_ViewportX_ = x;
	m_ViewportY_ = y;
	m_WindowWidth_ = width;
	m_WindowHeight_ = height;
	m_AspectRatio_ = float(m_WindowWidth_) / float(m_WindowHeight_);

	UpdateMatrix(m_FOV_, m_NearPlane_, m_FarPlane_, m_WindowWidth_, m_WindowHeight_);
}
void HorusOpenGLCamera::SetNear(float nearPlane)
{
	m_NearPlane_ = nearPlane;

	UpdateMatrix(m_FOV_, m_NearPlane_, m_FarPlane_, m_WindowWidth_, m_WindowHeight_);
}
void HorusOpenGLCamera::SetFar(float farPlane)
{
	m_FarPlane_ = farPlane;

	UpdateMatrix(m_FOV_, m_NearPlane_, m_FarPlane_, m_WindowWidth_, m_WindowHeight_);
}

void HorusOpenGLCamera::SetAspectRatio(int Width, int Height)
{
	m_AspectRatio_ = float(Width) / float(Height);

	UpdateMatrix(m_FOV_, m_NearPlane_, m_FarPlane_, Width, Height);
}

void HorusOpenGLCamera::SetAspectRatio(float AspectRatio)
{
	m_AspectRatio_ = AspectRatio;

	UpdateMatrix(m_FOV_, m_NearPlane_, m_FarPlane_, m_WindowWidth_, m_WindowHeight_);
}

void HorusOpenGLCamera::SetViewMatrix(const glm::mat4& viewMatrix)
{
    m_ViewMatrix_ = viewMatrix;
    m_Position_ = glm::vec3(glm::inverse(viewMatrix)[3]); // Update position based on the view matrix
    UpdateMatrix(m_FOV_, m_NearPlane_, m_FarPlane_, m_WindowWidth_, m_WindowHeight_);
}


