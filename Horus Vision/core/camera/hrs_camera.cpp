#include "hrs_camera.h"

#include <RadeonProRender_v2.h>
#include <Math/mathutils.h>

#include "common.h"
#include "hrs_radeon.h"
#include "objects/hrs_object_manager.h"
#include "spdlog/spdlog.h"

using namespace std;
using namespace RadeonProRender;

// Debug	
void HorusCamera::VariableCheckers(std::string Name)
{
	cout << "Start info --------------------------" << '\n';
	cout << "Camera : " << Name << '\n';
	cout << "Camera type : " << m_CameraType_ << '\n';
	cout << "Camera transform : " << m_Transform_.x << " " << m_Transform_.y << " " << m_Transform_.z << '\n';
	cout << "Camera look at : " << m_LookAt_.x << " " << m_LookAt_.y << " " << m_LookAt_.z << '\n';
	cout << "Camera up : " << m_Up_.x << " " << m_Up_.y << " " << m_Up_.z << '\n';
	cout << "Camera position : " << m_Position_.x << " " << m_Position_.y << " " << m_Position_.z << '\n';
	cout << "Camera fstop : " << m_FStop_ << '\n';
	cout << "Camera near : " << m_Near_ << '\n';
	cout << "Camera far : " << m_Far_ << '\n';
	cout << "Camera aspect : " << m_Aspect_ << '\n';
	cout << "Camera fov : " << m_Fov_ << '\n';
	cout << "Camera viewport x : " << m_Viewport_X_ << '\n';
	cout << "Camera viewport y : " << m_Viewport_Y_ << '\n';
	cout << "Camera window width : " << _Window_Width_ << '\n';
	cout << "Camera window height : " << _Window_Height_ << '\n';
	cout << "Camera instance : " << m_camera_ << '\n';
	cout << "End info --------------------------" << '\n';
}

void HorusCamera::GetCameraInfo()
{
	rprCameraGetInfo(m_camera_, RPR_CAMERA_LOOKAT, sizeof(m_LookAt_), &m_LookAt_, nullptr);
	rprCameraGetInfo(m_camera_, RPR_CAMERA_UP, sizeof(m_Up_), &m_Up_, nullptr);
	rprCameraGetInfo(m_camera_, RPR_CAMERA_POSITION, sizeof(m_Position_), &m_Position_, nullptr);
}

void HorusCamera::Init()
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	m_CameraType_ = CameraType::Free;

	spdlog::info("Camera init");
	std::cout << "Updating camera in instance: " << this << '\n';

	CHECK(rprContextCreateCamera(Radeon.GetContext(), &m_camera_))

	GetCameraInfo();

	rprCameraSetMode(m_camera_, RPR_CAMERA_MODE_PERSPECTIVE);

	m_Up_ = glm::vec3(0.0f, 1.0f, 0.0f);
	m_LookAt_ = glm::vec3(0.0f, 0.0f, 0.0f);
	m_Position_ = glm::vec3(0.0f, 9.0f, 40.0f);
	m_Transform_ = glm::vec3(0.0f, 0.0f, 0.0f);
	m_Aspect_ = double(_Window_Width_) / double(_Window_Height_);
	m_Fov_ = 45.0f;
	m_FStop_ = 1.0f;
	m_Near_ = 0.1f;
	m_Far_ = 10000.0f;
	m_CameraScale_ = glm::vec3(1.0f, 1.0f, 1.0f);

	CHECK(rprCameraLookAt(m_camera_, m_Position_.x, m_Position_.y, m_Position_.z, m_LookAt_.x, m_LookAt_.y, m_LookAt_.z, m_Up_.x, m_Up_.y, m_Up_.z));

	CHECK(rprCameraSetFocalLength(m_camera_, 45.0f));
	CHECK(rprSceneSetCamera(ObjectManager.GetScene(), m_camera_));
}

void HorusCamera::Reset()
{
	GetCameraInfo();

	m_Up_ = glm::vec3(0.0f, 1.0f, 0.0f);
	m_LookAt_ = glm::vec3(0.0f, 0.0f, 0.0f);
	m_Position_ = glm::vec3(0.0f, 9.0f, 40.0f);
	SetFOV(45.0f);

	VariableCheckers("Reset");
	UpdateCamera();
}

void HorusCamera::Destroy()
{
	if (m_camera_)
	{
		CHECK(rprObjectDelete(m_camera_))
			m_camera_ = nullptr;
	}
}

void HorusCamera::Bind()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	CHECK(rprSceneSetCamera(ObjectManager.GetScene(), m_camera_))
}

void HorusCamera::Unbind()
{
	CHECK(rprSceneSetCamera(nullptr, m_camera_))
}

void HorusCamera::UpdateCamera()
{
	//VariableCheckers("Start Update Camera");
	//std::cout << "Updating camera in instance: " << this << std::endl;

	/*glMatrixMode(GL_PROJECTION);
	glLoadIdentity();*/
	//glViewport(m_Viewport_X_, m_Viewport_Y_, _Window_Width_, _Window_Height_); // TODO : activate this for lighting

	//if (m_CameraType_ == Orthographic)
	//{
	//	//spdlog::info("Orthographic camera");
	//	//rprCameraSetMode(m_camera_, RPR_CAMERA_MODE_ORTHOGRAPHIC);

	//	//m_ProjectionMatrix_ = glm::ortho(-1.5f * float(m_Aspect_), 1.5f * float(m_Aspect_), -1.5f, 1.5f, -10.f, 10.f);
	//}
	//else if (m_CameraType_ == Free)
	//{
	//	//spdlog::info("Free camera");

	//	//rprCameraSetMode(m_camera_, RPR_CAMERA_MODE_PERSPECTIVE);

	//	/*m_ProjectionMatrix_ = glm::perspective(m_Fov_, m_Aspect_, m_Near_, m_Far_);

	//	glm::vec3 axis = glm::cross(m_Direction_, m_Up_);

	//	glm::quat pith_quat = glm::angleAxis(m_CameraPitch_, axis);
	//	glm::quat heading_quat = glm::angleAxis(m_CameraHeading_, m_Up_);

	//	glm::quat joined = glm::cross(pith_quat, heading_quat);
	//	joined = glm::normalize(joined);

	//	m_Direction_ = glm::rotate(joined, m_Direction_);

	//	m_Position_ += m_PositionDelta_;
	//	m_LookAt_ = m_Position_ + m_Direction_ * 1.0f;

	//	m_CameraHeading_ *= 0.5f;
	//	m_CameraPitch_ *= 0.5f;
	//	m_PositionDelta_ = m_PositionDelta_ * 0.8f;*/
	//}

	rprCameraLookAt(m_camera_, m_Position_.x, m_Position_.y, m_Position_.z, m_LookAt_.x, m_LookAt_.y, m_LookAt_.z, m_Up_.x, m_Up_.y, m_Up_.z);

	/*m_ViewMatrix_ = glm::lookAt(m_Position_, m_LookAt_, m_Up_);
	m_ModelMatrix_ = glm::mat4(1.0f);
	m_MVP_ = m_ProjectionMatrix_ * m_ViewMatrix_ * m_ModelMatrix_;*/

	//VariableCheckers("End Update Camera");
}

void HorusCamera::MoveCamera(CameraDirection dir)
{
	GetCameraInfo();

	if (m_CameraType_ == Free)
	{
		switch (dir)
		{
		case Forward:

			m_Direction_ = glm::normalize(m_LookAt_ - m_Position_);

			glm::vec3 TempLookAtFront = glm::normalize(m_LookAt_ - m_Position_);

			m_Position_ += TempLookAtFront * m_CameraScale_;

			break;
		case Backward:

			m_Direction_ = glm::normalize(m_LookAt_ - m_Position_);

			glm::vec3 TempLookAtBack = glm::normalize(m_LookAt_ - m_Position_);

			m_Position_ -= TempLookAtBack * m_CameraScale_;

			break;
		case Left:

			glm::vec3 TempRightLeft = glm::normalize(glm::cross(m_LookAt_ - m_Position_, m_Up_));

			m_Position_ += TempRightLeft * m_CameraScale_;

			break;
		case Right:

			glm::vec3 TempRightRight = glm::normalize(glm::cross(m_LookAt_ - m_Position_, m_Up_));

			m_Position_ -= TempRightRight * m_CameraScale_;

			break;
		case Up:

			glm::vec3 TempUpUp = glm::normalize(glm::cross(glm::cross(m_LookAt_ - m_Position_, m_Up_), m_LookAt_ - m_Position_));
			m_Position_ += TempUpUp * m_CameraScale_;

			break;
		case Down:

			glm::vec3 TempUpDown = glm::normalize(glm::cross(glm::cross(m_LookAt_ - m_Position_, m_Up_), m_LookAt_ - m_Position_));
			m_Position_ -= TempUpDown * m_CameraScale_;

			break;
		default:
			break;
		}
	}

	//VariableCheckers("Move Camera");
	UpdateCamera();
}

void HorusCamera::ChangePitch(float degrees)
{


	//VariableCheckers("Change Pitch");
	UpdateCamera();
}

void HorusCamera::ChangeHeading(float degrees)
{



	//VariableCheckers("Change Heading");
	UpdateCamera();
}

void HorusCamera::Move2D(int x, int y)
{




	//VariableCheckers("Move 2D");
	UpdateCamera();
}

void HorusCamera::SetMode(CameraType cam_mode)
{
	if (cam_mode == CameraType::Free)
	{
		m_CameraType_ = CameraType::Free;
		rprCameraSetMode(m_camera_, RPR_CAMERA_MODE_PERSPECTIVE);
	}
	else if (cam_mode == CameraType::Orthographic)
	{
		m_CameraType_ = CameraType::Orthographic;
		rprCameraSetMode(m_camera_, RPR_CAMERA_MODE_ORTHOGRAPHIC);
	}


	//VariableCheckers("Set Mode");
}

void HorusCamera::SetPosition(glm::vec3 pos)
{
	GetCameraInfo();

	m_Position_ = pos;

	//VariableCheckers("Set Position");
	UpdateCamera();
}

void HorusCamera::SetLookAt(glm::vec3 pos)
{
	GetCameraInfo();

	m_LookAt_ = pos;

	UpdateCamera();
}

void HorusCamera::SetFOV(double fov)
{
	GetCameraInfo();

	m_Fov_ = fov;
	rprCameraSetFocalLength(m_camera_, m_Fov_);

	UpdateCamera();
}

void HorusCamera::SetViewport(int loc_x, int loc_y, int width, int height)
{
	m_Viewport_X_ = loc_x;
	m_Viewport_Y_ = loc_y;
	_Window_Width_ = width;
	_Window_Height_ = height;

	m_Aspect_ = double(width) / double(height);
	//VariableCheckers("Set Viewport");
	//UpdateCamera();
}

void HorusCamera::SetClipping(double near_clip_distance, double far_clip_distance)
{
	m_Near_ = near_clip_distance;
	m_Far_ = far_clip_distance;

	rprCameraSetNearPlane(m_camera_, m_Near_);
	rprCameraSetFarPlane(m_camera_, m_Far_);
	VariableCheckers("Set Clipping");
	UpdateCamera();
}

void HorusCamera::Tumbling(float x, float y)
{
	GetCameraInfo();

	float AngleX = x * 2.5f;
	float AngleY = y * 2.5f;

	glm::vec3 CamToPivot = m_Position_ - m_LookAt_;

	float Radius = glm::length(CamToPivot);
	float CurrentYaw = atan2f(CamToPivot.z, CamToPivot.x);
	float CurrentPitch = asinf(CamToPivot.y / Radius);

	CurrentYaw += AngleX;
	CurrentPitch += AngleY;

	constexpr float MinPitch = glm::radians(89.f); // Important to avoid gimbal lock
	CurrentPitch = glm::max(-MinPitch, glm::min(CurrentPitch, MinPitch));

	CamToPivot.x = Radius * cosf(CurrentPitch) * cosf(CurrentYaw);
	CamToPivot.y = Radius * sinf(CurrentPitch);
	CamToPivot.z = Radius * cosf(CurrentPitch) * sinf(CurrentYaw);

	m_Position_ = m_LookAt_ + CamToPivot;

	//VariableCheckers("Tumbling");
	UpdateCamera();
}

void HorusCamera::Zoom(float distance)
{
	GetCameraInfo();

	m_Direction_ = glm::normalize(m_LookAt_ - m_Position_);
	m_Right_ = glm::normalize(glm::cross(m_Direction_, m_Up_));

	glm::vec3 TempTranslation = m_Direction_ * distance;

	m_Position_ = m_Position_ + TempTranslation;

	UpdateCamera();
}

void HorusCamera::Pan(float x, float y)
{
	GetCameraInfo();

	glm::vec3 Forward = glm::normalize(m_LookAt_ - m_Position_);
	glm::vec3 Right = glm::normalize(glm::cross(Forward, m_Up_));
	glm::vec3 Up = glm::normalize(m_Up_);

	float PanSensitivity = 0.01f;

	glm::vec3 Displacement = (Right * x + Up * (-y)) * PanSensitivity;

	m_Position_ += Displacement;
	m_LookAt_ += Displacement;

	UpdateCamera();
}

// Getters

CameraType HorusCamera::GetMode()
{
	VariableCheckers("Get Mode");
	return m_CameraType_;
}

void HorusCamera::GetViewport(int& loc_x, int& loc_y, int& width, int& height)
{
	loc_x = m_Viewport_X_;
	loc_y = m_Viewport_Y_;
	width = _Window_Width_;
	height = _Window_Height_;

	VariableCheckers("Get Viewport");
}

void HorusCamera::GetMatrices(glm::mat4& P, glm::mat4& V, glm::mat4& M)
{
	/*P = m_ProjectionMatrix_;
	V = m_ViewMatrix_;
	M = m_ModelMatrix_;*/
}

glm::vec3 HorusCamera::GetLookAt()
{
	return m_LookAt_;
}
glm::vec3 HorusCamera::GetPosition()
{
	return m_Position_;
}
glm::vec3 HorusCamera::GetTranslation()
{
	return m_Position_;
}
glm::vec3 HorusCamera::GetRotation()
{
	glm::vec3 direction = glm::normalize(m_Direction_);
	glm::vec3 up = glm::normalize(m_Up_);
	glm::vec3 right = glm::normalize(glm::cross(direction, up));

	glm::mat3 rotationMatrix = glm::mat3(right, up, -direction);

	glm::vec3 rotation = glm::eulerAngles(glm::quat_cast(rotationMatrix));

	return rotation;
}
glm::vec3 HorusCamera::GetUp()
{
	return m_Up_;
}
glm::vec3 HorusCamera::GetRight()
{
	return m_Right_;
}
glm::vec3 HorusCamera::GetDirection()
{
	return m_Direction_;
}
glm::vec3 HorusCamera::GetPitchAxis()
{
	return m_PitchAxis_;
}
glm::vec3 HorusCamera::GetHeadingAxis()
{
	return m_HeadingAxis_;
}
glm::vec3 HorusCamera::GetCameraScale()
{
	return m_CameraScale_;
}

double HorusCamera::GetFOV()
{
	return m_Fov_;
}
double HorusCamera::GetNear()
{
	return m_Near_;
}
double HorusCamera::GetFar()
{
	return m_Far_;
}
double HorusCamera::GetFStop()
{
	return m_FStop_;
}
double HorusCamera::GetAspect()
{
	return m_Aspect_;
}

// Setters

void HorusCamera::SetPos(int button, int state, int x, int y)
{
	/*if (button == ImGuiMouseButton_Left && state == GLFW_PRESS)
	{
		m_MoveCamera_ = true;
	}
	else if (button == ImGuiMouseButton_Left && state == GLFW_RELEASE)
	{

		m_MoveCamera_ = false;
	}

	spdlog::info("SetPos : {} {}", x, y);
	m_MousePosition_ = glm::vec3(x, y, 0.0f);
	spdlog::info("Mouse position : {} {} {}", m_MousePosition_.x, m_MousePosition_.y, m_MousePosition_.z);*/
	VariableCheckers("Set Pos");
	UpdateCamera();
}
void HorusCamera::SetCameraRotation(float pitch, float yaw, float roll)
{
	GetCameraInfo();

	pitch = glm::radians(pitch);
	yaw = glm::radians(yaw);
	roll = glm::radians(roll);

	glm::quat QuaternionRotation = glm::quat(glm::vec3(pitch, yaw, roll));

	glm::mat3 RotationMatrix = glm::mat3_cast(QuaternionRotation);

	m_Direction_ = RotationMatrix * glm::vec3(0.0f, 0.0f, -1.0f);

	m_Up_ = RotationMatrix * glm::vec3(0.0f, 1.0f, 0.0f);

	UpdateCamera();
}
void HorusCamera::SetAspect(double aspect)
{
	GetCameraInfo();

	m_Aspect_ = aspect;

	UpdateCamera();
}
void HorusCamera::SetCameraScale(glm::vec3 scale)
{
	GetCameraInfo();

	m_CameraScale_ = scale;

	UpdateCamera();
}




