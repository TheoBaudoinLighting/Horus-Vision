#include "hrs_camera.h"

#include <RadeonProRender_v2.h>
#include <Math/mathutils.h>

#include "common.h"
#include "objects/hrs_object_manager.h"
#include "spdlog/spdlog.h"

using namespace std;
using namespace RadeonProRender;

// Debug	
void HorusCamera::VariableCheckers(std::string name)
{
	cout << "Start info --------------------------" << endl;
	cout << "Camera : " << name << endl;
	cout << "Camera type : " << m_CameraType_ << endl;
	cout << "Camera transform : " << m_Transform_.x << " " << m_Transform_.y << " " << m_Transform_.z << endl;
	cout << "Camera look at : " << m_LookAt_.x << " " << m_LookAt_.y << " " << m_LookAt_.z << endl;
	cout << "Camera up : " << m_Up_.x << " " << m_Up_.y << " " << m_Up_.z << endl;
	cout << "Camera position : " << m_Position_.x << " " << m_Position_.y << " " << m_Position_.z << endl;
	cout << "Camera fstop : " << m_FStop_ << endl;
	cout << "Camera near : " << m_Near_ << endl;
	cout << "Camera far : " << m_Far_ << endl;
	cout << "Camera aspect : " << m_Aspect_ << endl;
	cout << "Camera fov : " << m_Fov_ << endl;
	cout << "Camera viewport x : " << m_Viewport_X_ << endl;
	cout << "Camera viewport y : " << m_Viewport_Y_ << endl;
	cout << "Camera window width : " << _Window_Width_ << endl;
	cout << "Camera window height : " << _Window_Height_ << endl;
	cout << "Camera instance : " << m_camera_ << endl;
	cout << "End info --------------------------" << endl;
}

void HorusCamera::GetCameraInfo()
{
	//rprCameraGetInfo(m_camera_, RPR_CAMERA_MODE, sizeof(m_CameraType_), &m_CameraType_, nullptr);
	//rprCameraGetInfo(m_camera_, RPR_CAMERA_TRANSFORM, sizeof(m_Transform_), &m_Transform_, nullptr);
	rprCameraGetInfo(m_camera_, RPR_CAMERA_LOOKAT, sizeof(m_LookAt_), &m_LookAt_, nullptr);
	rprCameraGetInfo(m_camera_, RPR_CAMERA_UP, sizeof(m_Up_), &m_Up_, nullptr);
	rprCameraGetInfo(m_camera_, RPR_CAMERA_POSITION, sizeof(m_Position_), &m_Position_, nullptr);
	//rprCameraGetInfo(m_camera_, RPR_CAMERA_FSTOP, sizeof(m_FStop_), &m_FStop_, nullptr);
	//rprCameraGetInfo(m_camera_, RPR_CAMERA_NEAR_PLANE, sizeof(m_Near_), &m_Near_, nullptr);
	//rprCameraGetInfo(m_camera_, RPR_CAMERA_FAR_PLANE, sizeof(m_Far_), &m_Far_, nullptr);
}

void HorusCamera::Init()
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();
	HorusObjectManager& ObjectManager = HorusObjectManager::get_instance();

	m_CameraType_ = CameraType::Free;

	spdlog::info("Camera init");
	std::cout << "Updating camera in instance: " << this << std::endl;

	CHECK(rprContextCreateCamera(Radeon.get_context(), &m_camera_));

	GetCameraInfo();

	rprCameraSetMode(m_camera_, RPR_CAMERA_MODE_PERSPECTIVE);
	//rprCameraSetFStop(m_camera_, 1.0f);
	//rprCameraSetSensorSize(m_camera_, 36.0f, 24.0f);
	//rprCameraSetFocalLength(m_camera_, 45.0f);
	//rprCameraSetFocusDistance(m_camera_, 1.0f);
	//rprCameraSetApertureBlades(m_camera_, 7);
	//rprCameraSetLensShift(m_camera_, 0.0f, 0.0f);
	//rprCameraSetPostScale(m_camera_, 1.0f);

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
	CHECK(rprSceneSetCamera(ObjectManager.get_scene(), m_camera_));

	//VariableCheckers("End Init");
}

void HorusCamera::Reset()
{
	GetCameraInfo();

	m_Up_ = glm::vec3(0.0f, 1.0f, 0.0f);
	m_LookAt_ = glm::vec3(0.0f, 0.0f, 0.0f);
	m_Position_ = glm::vec3(0.0f, 9.0f, 40.0f);

	VariableCheckers("Reset");
	UpdateCamera();
}

void HorusCamera::Destroy()
{
	if (m_camera_)
	{
		CHECK(rprObjectDelete(m_camera_));
		m_camera_ = nullptr;
	}
}

void HorusCamera::Bind()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::get_instance();

	CHECK(rprSceneSetCamera(ObjectManager.get_scene(), m_camera_));
}

void HorusCamera::Unbind()
{
	CHECK(rprSceneSetCamera(nullptr, m_camera_));
}

void HorusCamera::UpdateCamera()
{
	//VariableCheckers("Start Update Camera");
	//std::cout << "Updating camera in instance: " << this << std::endl;

	/*glMatrixMode(GL_PROJECTION);
	glLoadIdentity();*/
	//glViewport(m_Viewport_X_, m_Viewport_Y_, _Window_Width_, _Window_Height_); // TODO : activate this for lighting

	if (m_CameraType_ == Orthographic)
	{
		//spdlog::info("Orthographic camera");
		//rprCameraSetMode(m_camera_, RPR_CAMERA_MODE_ORTHOGRAPHIC);

		//m_ProjectionMatrix_ = glm::ortho(-1.5f * float(m_Aspect_), 1.5f * float(m_Aspect_), -1.5f, 1.5f, -10.f, 10.f);
	}
	else if (m_CameraType_ == Free)
	{
		//spdlog::info("Free camera");

		//rprCameraSetMode(m_camera_, RPR_CAMERA_MODE_PERSPECTIVE);

		/*m_ProjectionMatrix_ = glm::perspective(m_Fov_, m_Aspect_, m_Near_, m_Far_);

		glm::vec3 axis = glm::cross(m_Direction_, m_Up_);

		glm::quat pith_quat = glm::angleAxis(m_CameraPitch_, axis);
		glm::quat heading_quat = glm::angleAxis(m_CameraHeading_, m_Up_);

		glm::quat joined = glm::cross(pith_quat, heading_quat);
		joined = glm::normalize(joined);

		m_Direction_ = glm::rotate(joined, m_Direction_);

		m_Position_ += m_PositionDelta_;
		m_LookAt_ = m_Position_ + m_Direction_ * 1.0f;

		m_CameraHeading_ *= 0.5f;
		m_CameraPitch_ *= 0.5f;
		m_PositionDelta_ = m_PositionDelta_ * 0.8f;*/
	}

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

			glm::vec3 TempLookAtBack =  glm::normalize(m_LookAt_ - m_Position_);

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
	
	//VariableCheckers("Set Look At");
	UpdateCamera();
}

void HorusCamera::SetFOV(double fov)
{
	GetCameraInfo();

	m_Fov_ = fov;

	rprCameraSetFocalLength(m_camera_, m_Fov_);
	//VariableCheckers("Set FOV");
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

	float angle_x = x * 2.5f;
	float angle_y = y * 2.5f;

	glm::vec3 CamToPivot = m_Position_ - m_LookAt_;

	float Radius = glm::length(CamToPivot);
	float CurrentYaw = atan2f(CamToPivot.z, CamToPivot.x);
	float CurrentPitch = asinf(CamToPivot.y / Radius);

	CurrentYaw += angle_x;
	CurrentPitch += angle_y;

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

	glm::vec3 forward = glm::normalize(m_LookAt_ - m_Position_);
	glm::vec3 right = glm::normalize(glm::cross(forward, m_Up_));
	glm::vec3 up = glm::normalize(m_Up_);

	float panSensitivity = 0.01f;

	glm::vec3 displacement = (right * x + up * (-y)) * panSensitivity;

	m_Position_ += displacement;
	m_LookAt_ += displacement;

	UpdateCamera();
}

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
