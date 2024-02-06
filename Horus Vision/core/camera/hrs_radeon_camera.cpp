
#include "hrs_object_manager.h"
#include "hrs_radeon_camera.h"

#include <Math/mathutils.h>
#include <cmath>

#include "common.h"
#include "hrs_radeon.h"
#include "spdlog/spdlog.h"

using namespace std;
using namespace RadeonProRender;

// Debug	
void HorusRadeonCamera::VariableCheckers(std::string Name)
{
	cout << "Start info --------------------------" << '\n';
	cout << "Camera : " << Name << '\n';
	cout << "Camera type : " << m_CameraType_ << '\n';
	cout << "Camera transform : " << m_Transform_[0][0] << m_Transform_[0][1] << m_Transform_[0][2] << m_Transform_[0][3] << '\n';
	cout << "Camera look at : " << m_LookAt_.x << " " << m_LookAt_.y << " " << m_LookAt_.z << '\n';
	cout << "Camera up : " << m_Up_.x << " " << m_Up_.y << " " << m_Up_.z << '\n';
	cout << "Camera position : " << m_Position_.x << " " << m_Position_.y << " " << m_Position_.z << '\n';
	cout << "Camera fstop : " << m_FStop_ << '\n';
	cout << "Camera near : " << m_Near_ << '\n';
	cout << "Camera far : " << m_Far_ << '\n';
	cout << "Camera aspect : " << m_Aspect_ << '\n';
	cout << "Camera fov : " << m_Fov_ << '\n';
	cout << "Camera viewport x : " << m_ViewportX_ << '\n';
	cout << "Camera viewport y : " << m_ViewportY_ << '\n';
	cout << "Camera window width : " << m_WindowWidth_ << '\n';
	cout << "Camera window height : " << m_WindowHeight_ << '\n';
	cout << "Camera instance : " << m_Camera_ << '\n';
	cout << "End info --------------------------" << '\n';

	// Get RPR_CAMERA_FOCUS_DISTANCE
	rpr_float m_FocusDistance_;
	CHECK(rprCameraGetInfo(m_Camera_, RPR_CAMERA_FOCUS_DISTANCE, sizeof(m_FocusDistance_), &m_FocusDistance_, nullptr));
	cout << "Camera focus distance : " << m_FocusDistance_ << '\n';

}
void HorusRadeonCamera::GetCameraInfo()
{
	/*try
	{
		CHECK(rprCameraGetInfo(m_Camera_, RPR_CAMERA_LOOKAT, sizeof(m_LookAt_), &m_LookAt_, nullptr));
		CHECK(rprCameraGetInfo(m_Camera_, RPR_CAMERA_UP, sizeof(m_Up_), &m_Up_, nullptr));
		CHECK(rprCameraGetInfo(m_Camera_, RPR_CAMERA_POSITION, sizeof(m_Position_), &m_Position_, nullptr));
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << '\n';

	}*/
}

void HorusRadeonCamera::Init(std::string Name)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusGarbageCollector& GC = HorusGarbageCollector::GetInstance();

	m_CameraType_ = Free;

	// Init camera variables
	m_Up_ = glm::vec3(0.0f, 1.0f, 0.0f);
	m_LookAt_ = glm::vec3(0.0f, 0.0f, 0.0f);
	m_Position_ = glm::vec3(0.0f, 9.0f, 40.0f); // glm::vec3(0.0f, 9.0f, 40.0f);
	m_Transform_ = glm::mat4(1.0f);
	m_Aspect_ = static_cast<float>(m_WindowWidth_) / static_cast<float>(m_WindowHeight_);
	m_Fov_ = 45.0f;
	m_FStop_ = 100.f;
	m_Near_ = 0.1f;
	m_Far_ = 1000.0f;
	m_Scale_ = glm::vec3(1.0f, 1.0f, 1.0f);
	m_FocusPlane_ = 7.5;
	m_ApertureBlades_ = 5;

	CHECK(rprContextCreateCamera(Radeon.GetContext(), &m_Camera_));

	CHECK(rprCameraSetMode(m_Camera_, RPR_CAMERA_MODE_PERSPECTIVE));
	CHECK(rprCameraSetFocalLength(m_Camera_, m_Fov_));
	CHECK(rprCameraSetSensorSize(m_Camera_, 36.0f, 24.0f));
	CHECK(rprCameraSetFocusDistance(m_Camera_, m_FocusPlane_));
	CHECK(rprCameraSetFStop(m_Camera_, m_FStop_));
	CHECK(rprCameraSetExposure(m_Camera_, 1.0f));
	CHECK(rprCameraSetNearPlane(m_Camera_, m_Near_));
	CHECK(rprCameraSetFarPlane(m_Camera_, m_Far_));
	CHECK(rprCameraSetApertureBlades(m_Camera_, m_ApertureBlades_));

	CHECK(rprObjectSetName(m_Camera_, Name.c_str())); // Set camera name

	CHECK(rprCameraLookAt(m_Camera_, m_Position_.x, m_Position_.y, m_Position_.z, m_LookAt_.x, m_LookAt_.y, m_LookAt_.z, m_Up_.x, m_Up_.y, m_Up_.z));
	CHECK(rprSceneSetCamera(ObjectManager.GetScene(), m_Camera_));

	GC.Add(m_Camera_);
}
void HorusRadeonCamera::Reset()
{
	GetCameraInfo();

	m_Up_ = glm::vec3(0.0f, 1.0f, 0.0f);
	m_LookAt_ = glm::vec3(0.0f, 0.0f, 0.0f);
	m_Position_ = glm::vec3(0.0f, 9.0f, 40.0f);

	SetFov(45.0f);

	VariableCheckers("Reset");
	UpdateCamera();
}
void HorusRadeonCamera::Destroy()
{
	if (m_Camera_)
	{
		//CHECK(rprObjectDelete(m_Camera_))
		m_Camera_ = nullptr;
	}
}

void HorusRadeonCamera::Bind()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	CHECK(rprSceneSetCamera(ObjectManager.GetScene(), m_Camera_))
}
void HorusRadeonCamera::Unbind()
{
	CHECK(rprSceneSetCamera(nullptr, m_Camera_))
}

void HorusRadeonCamera::UpdateCamera()
{
	rprCameraLookAt(m_Camera_, m_Position_.x, m_Position_.y, m_Position_.z, m_LookAt_.x, m_LookAt_.y, m_LookAt_.z, m_Up_.x, m_Up_.y, m_Up_.z);
}
void HorusRadeonCamera::UpdateTransform()
{
	glm::mat4 Translate = translate(glm::mat4(1.0f), m_Position_);
	glm::mat4 RotationX = rotate(glm::mat4(1.0f), m_Rotation_.x, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 RotationY = rotate(glm::mat4(1.0f), m_Rotation_.y, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 RotationZ = rotate(glm::mat4(1.0f), m_Rotation_.z, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 Scale = scale(glm::mat4(1.0f), m_Scale_);

	glm::mat4 Rotation = RotationX * RotationY * RotationZ;

	m_Transform_ = Translate * Rotation * Scale;

	rprCameraSetTransform(m_Camera_, false, &m_Transform_[0][0]);

	UpdateCamera();
}

void HorusRadeonCamera::MoveCamera(CameraDirection Dir)
{
	GetCameraInfo();

	if (m_CameraType_ == Free)
	{
		switch (Dir)
		{
		case Forward:

			m_Direction_ = glm::normalize(m_LookAt_ - m_Position_);

			const glm::vec3 TempLookAtFront = glm::normalize(m_LookAt_ - m_Position_);

			m_Position_ += TempLookAtFront * m_Scale_;

			break;
		case Backward:

			m_Direction_ = glm::normalize(m_LookAt_ - m_Position_);

			const glm::vec3 TempLookAtBack = glm::normalize(m_LookAt_ - m_Position_);

			m_Position_ -= TempLookAtBack * m_Scale_;

			break;
		case Left:

			const glm::vec3 TempRightLeft = glm::normalize(glm::cross(m_LookAt_ - m_Position_, m_Up_));

			m_Position_ += TempRightLeft * m_Scale_;

			break;
		case Right:

			const glm::vec3 TempRightRight = glm::normalize(glm::cross(m_LookAt_ - m_Position_, m_Up_));

			m_Position_ -= TempRightRight * m_Scale_;

			break;
		case Up:

			const glm::vec3 TempUpUp = glm::normalize(glm::cross(glm::cross(m_LookAt_ - m_Position_, m_Up_), m_LookAt_ - m_Position_));
			m_Position_ += TempUpUp * m_Scale_;

			break;
		case Down:

			const glm::vec3 TempUpDown = glm::normalize(glm::cross(glm::cross(m_LookAt_ - m_Position_, m_Up_), m_LookAt_ - m_Position_));
			m_Position_ -= TempUpDown * m_Scale_;

			break;
		}
	}

	//VariableCheckers("Move Camera");
	UpdateCamera();
}
void HorusRadeonCamera::Move2D(int x, int y)
{

	//VariableCheckers("Move 2D");
	UpdateCamera();
}

void HorusRadeonCamera::ChangePitch(float Degrees)
{

	//VariableCheckers("Change Pitch");
	UpdateCamera();
}
void HorusRadeonCamera::ChangeHeading(float degrees)
{

	//VariableCheckers("Change Heading");
	UpdateCamera();
}

void HorusRadeonCamera::SetMode(CameraType CamMode)
{
	if (CamMode == CameraType::Free)
	{
		m_CameraType_ = CameraType::Free;
		rprCameraSetMode(m_Camera_, RPR_CAMERA_MODE_PERSPECTIVE);
	}
	else if (CamMode == CameraType::Orthographic)
	{
		m_CameraType_ = CameraType::Orthographic;
		rprCameraSetMode(m_Camera_, RPR_CAMERA_MODE_ORTHOGRAPHIC);
	}

	//VariableCheckers("Set Mode");
}
void HorusRadeonCamera::SetPosition(glm::vec3 pos)
{
	GetCameraInfo();

	m_Position_ = pos;

	UpdateCamera();
}
void HorusRadeonCamera::SetLookAt(glm::vec3 pos)
{
	GetCameraInfo();

	m_LookAt_ = pos;

	UpdateCamera();
}
void HorusRadeonCamera::SetFov(const float Fov)
{
	GetCameraInfo();

	m_Fov_ = Fov;
	rprCameraSetFocalLength(m_Camera_, m_Fov_);

	UpdateCamera();
}
void HorusRadeonCamera::SetViewport(int loc_x, int loc_y, int width, int height)
{
	m_ViewportX_ = loc_x;
	m_ViewportY_ = loc_y;
	m_WindowWidth_ = width;
	m_WindowHeight_ = height;

	m_Aspect_ = static_cast<float>(width) / static_cast<float>(height);
}
void HorusRadeonCamera::SetClipping(float NearClipDistance, float FarClipDistance)
{
	m_Near_ = NearClipDistance;
	m_Far_ = FarClipDistance;

	rprCameraSetNearPlane(m_Camera_, m_Near_);
	rprCameraSetFarPlane(m_Camera_, m_Far_);
	VariableCheckers("Set Clipping");
	UpdateCamera();
}

void HorusRadeonCamera::Tumbling(float x, float y, float sensitivity)
{
	GetCameraInfo();

	// New method with quaternions
	const float AngleX = -x * sensitivity;
	const float AngleY = y * sensitivity;

	glm::vec3 CameraDirection = glm::normalize(m_Position_ - m_LookAt_);
	const float Radius = glm::length(m_Position_ - m_LookAt_);

	glm::vec3 Axis = glm::cross(CameraDirection, m_Up_);

	glm::quat QuatPitch = glm::angleAxis(AngleY, Axis); // y = pitch
	glm::quat QuatHeading = glm::angleAxis(AngleX, m_Up_); // x = heading

	glm::quat QuatTemp = glm::normalize(glm::cross(QuatPitch, QuatHeading));

	CameraDirection = glm::rotate(QuatTemp, CameraDirection);

	m_Position_ = m_LookAt_ + CameraDirection * Radius;

	{
		// Previous method with Euler angles (deprecated)
	/*const float AngleX = x * 2.5f;
	const float AngleY = y * 2.5f;

	glm::vec3 CamToPivot = m_Position_ - m_LookAt_;

	const float Radius = glm::length(CamToPivot);
	float CurrentYaw = atan2f(CamToPivot.z, CamToPivot.x);
	float CurrentPitch = asinf(CamToPivot.y / Radius);

	CurrentYaw += AngleX;
	CurrentPitch += AngleY;

	constexpr float MinPitch = glm::radians(89.9f);
	CurrentPitch = glm::max(-MinPitch, glm::min(CurrentPitch, MinPitch));

	CamToPivot.x = Radius * cosf(CurrentPitch) * cosf(CurrentYaw);
	CamToPivot.y = Radius * sinf(CurrentPitch);
	CamToPivot.z = Radius * cosf(CurrentPitch) * sinf(CurrentYaw);

	m_Position_ = m_LookAt_ + CamToPivot;*/
	}

	UpdateCamera();
}
void HorusRadeonCamera::Zoom(float distance)
{
	GetCameraInfo();

	m_Direction_ = glm::normalize(m_LookAt_ - m_Position_);
	m_Right_ = glm::normalize(glm::cross(m_Direction_, m_Up_));

	const glm::vec3 TempTranslation = m_Direction_ * distance;

	m_Position_ = m_Position_ + TempTranslation;

	UpdateCamera();
}
void HorusRadeonCamera::Pan(float x, float y, float sensitivity)
{
	GetCameraInfo();

	const glm::vec3 Forward = glm::normalize(m_LookAt_ - m_Position_);
	const glm::vec3 Right = glm::normalize(glm::cross(Forward, m_Up_));
	const glm::vec3 Up = glm::normalize(m_Up_);

	const glm::vec3 Displacement = (Right * x + Up * (-y)) * sensitivity;

	m_Position_ += Displacement;
	m_LookAt_ += Displacement;

	UpdateCamera();
}

// Getters

CameraType HorusRadeonCamera::GetMode()
{
	VariableCheckers("Get Mode");
	return m_CameraType_;
}

void HorusRadeonCamera::GetViewport(int& LocX, int& LocY, int& Width, int& Height)
{
	LocX = m_ViewportX_;
	LocY = m_ViewportY_;
	Width = m_WindowWidth_;
	Height = m_WindowHeight_;

	VariableCheckers("Get Viewport");
}
void HorusRadeonCamera::GetMatrices(glm::mat4& P, glm::mat4& V, glm::mat4& M)
{
	/*P = m_ProjectionMatrix_;
	V = m_ViewMatrix_;
	M = m_ModelMatrix_;*/
}

glm::vec3 HorusRadeonCamera::GetLookAt()
{
	return m_LookAt_;
}
glm::vec3 HorusRadeonCamera::GetPosition()
{
	return m_Position_;
}
glm::vec3 HorusRadeonCamera::GetTranslation()
{
	return m_Position_;
}
glm::vec3 HorusRadeonCamera::GetRotation()
{
	glm::mat4 Rotation = glm::lookAt(m_Position_, m_LookAt_, m_Up_);

	glm::mat4 RotationMatrix = glm::mat4(glm::mat3(Rotation));

	glm::vec3 RotationEuler = glm::eulerAngles(glm::quat_cast(RotationMatrix));

	RotationEuler = glm::degrees(RotationEuler);

	return RotationEuler;
}
glm::vec3 HorusRadeonCamera::GetUp()
{
	return m_Up_;
}
glm::vec3 HorusRadeonCamera::GetRight()
{
	return m_Right_;
}
glm::vec3 HorusRadeonCamera::GetDirection()
{
	return m_Direction_;
}
glm::vec3 HorusRadeonCamera::GetPitchAxis()
{
	return m_PitchAxis_;
}
glm::vec3 HorusRadeonCamera::GetHeadingAxis()
{
	return m_HeadingAxis_;
}
glm::vec3 HorusRadeonCamera::GetCameraScale()
{
	return m_Scale_;
}

float HorusRadeonCamera::GetFov()
{
	return m_Fov_;
}
float HorusRadeonCamera::GetNear()
{
	return m_Near_;
}
float HorusRadeonCamera::GetFar()
{
	return m_Far_;
}
float HorusRadeonCamera::GetFStop()
{
	return m_FStop_;
}
float HorusRadeonCamera::GetAspect()
{
	return m_Aspect_;
}
float HorusRadeonCamera::GetAperture()
{
	return m_Aperture_;
}
float HorusRadeonCamera::GetFocusPlane()
{
	return m_FocusPlane_;
}
int HorusRadeonCamera::GetApertureBlades()
{
	return m_ApertureBlades_;
}

// Setters

void HorusRadeonCamera::SetCameraRotation(float pitch, float yaw, float roll)
{
	GetCameraInfo();

	m_Rotation_ = glm::vec3(pitch, yaw, roll);

	UpdateTransform();
}
void HorusRadeonCamera::SetFocusPlane(float FocusDistance)
{
	GetCameraInfo();

	m_FocusPlane_ = FocusDistance;
	rprCameraSetFocusDistance(m_Camera_, FocusDistance);

	UpdateCamera();
}
void HorusRadeonCamera::SetFStop(float FStop)
{
	GetCameraInfo();

	m_FStop_ = FStop;
	CHECK(rprCameraSetFStop(m_Camera_, m_FStop_));

	UpdateCamera();
}
void HorusRadeonCamera::SetApertureBlades(int ApertureBlades)
{
	GetCameraInfo();

	m_ApertureBlades_ = ApertureBlades;
	CHECK(rprCameraSetApertureBlades(m_Camera_, m_ApertureBlades_));

	UpdateCamera();
}
void HorusRadeonCamera::SetSensorSize(float Width, float Height)
{
	CHECK(rprCameraSetSensorSize(m_Camera_, Width, Height));

	UpdateCamera();
}
void HorusRadeonCamera::SetAspect(const float Aspect)
{
	GetCameraInfo();

	m_Aspect_ = Aspect;
	//CHECK(rprCameraSetSensorSize(m_Camera_, 36.0f, 24.0f * m_Aspect_));

	UpdateCamera();
}
void HorusRadeonCamera::SetCameraScale(glm::vec3 Scale)
{
	GetCameraInfo();

	m_Scale_ = Scale;

	UpdateCamera();
}

glm::mat4 ExtractRotation(const glm::vec3& Position, const glm::vec3& LookAt, const glm::vec3& Up)
{
	glm::mat4 Rotation = glm::lookAt(Position, LookAt, Up);


	return Rotation;
}


