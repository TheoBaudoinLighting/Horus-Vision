#pragma once
#pragma warning(disable : 4996)
#pragma warning(disable : 4244)
#pragma warning(disable : 4305)

#include <RadeonProRender.hpp>
#include <Math/mathutils.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum CameraType
{
	Free,
	Orthographic
};

enum CameraDirection
{
	Forward,
	Backward,
	Left,
	Right,
	Up,
	Down
};

class HorusRadeonCamera
{
public:

	HorusRadeonCamera() : m_CameraType_(), m_Fov_(0),
		m_Near_(0),
		m_Far_(0),
		m_Transform_(),
		m_LookAt_(), m_Up_(),
		m_Position_(),
		m_Scale_(),
		m_Direction_(),
		m_Right_(),
		m_PitchAxis_(),
		m_HeadingAxis_()
	{
	}

	~HorusRadeonCamera() {}

	HorusRadeonCamera(const HorusRadeonCamera&) = delete;
	HorusRadeonCamera& operator=(const HorusRadeonCamera&) = delete;

	void Init(std::string Name);
	void Reset();
	void Destroy();

	void VariableCheckers(std::string Name);

	void Bind();
	void Unbind();

	void GetCameraInfo();
	void UpdateCamera();
	void UpdateTransform();

	void MoveCamera(CameraDirection Dir);
	void ChangePitch(float Degrees);
	void ChangeHeading(float degrees);

	void Move2D(int x, int y);

	void Tumbling(float x, float y, float sensitivity);
	void Zoom(float distance);
	void Pan(float x, float y, float sensitivity);

	// Getters
	CameraType GetMode();
	void GetViewport(int& LocX, int& LocY, int& Width, int& Height);
	void GetMatrices(glm::mat4& P, glm::mat4& V, glm::mat4& M);

	// For Inspector
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

	float GetFov();
	float GetNear();
	float GetFar();
	float GetFStop();
	float GetAspect();
	float GetAperture();
	float GetFocusPlane();

	int GetApertureBlades();

	std::string& GetName() { return m_Name_; }

	// Setters
	void SetMode(CameraType CamMode);
	void SetPosition(glm::vec3 pos);
	void SetLookAt(glm::vec3 pos);
	void SetFov(const float Fov);
	void SetViewport(int loc_x, int loc_y, int width, int height);
	void SetClipping(float NearClipDistance, float FarClipDistance);
	void SetAspect(float Aspect);
	void SetCameraScale(glm::vec3 Scale);
	void SetCameraRotation(float pitch, float yaw, float roll);
	void SetFocusPlane(float FocusDistance);
	void SetFStop(float FStop);
	void SetApertureBlades(int ApertureBlades);
	void SetSensorSize(float Width, float Height);

private:

	std::string m_Name_ = "Camera";

	CameraType m_CameraType_ = Free;

	int m_ViewportX_ = 0;
	int m_ViewportY_ = 0;

	int m_WindowWidth_ = 0;
	int m_WindowHeight_ = 0;

	float m_Aspect_ = 0;
	float m_Fov_ = 45;
	float m_Near_ = 0.01;
	float m_Far_ = 1000;
	float m_FStop_ = 100;
	float m_Aperture_ = 0;
	float m_FocusPlane_ = 7.5;

	int m_ApertureBlades_ = 5;

	glm::quat m_QuatRotation_ = glm::quat(1, 0, 0, 0);
	glm::mat4 m_Transform_ = glm::mat4(1.0f);
	glm::vec3 m_LookAt_ = glm::vec3(0, 0, 0);
	glm::vec3 m_Up_ = glm::vec3(0, 1, 0);
	glm::vec3 m_Position_ = glm::vec3(0, 0, 0);
	glm::vec3 m_Rotation_ = glm::vec3(0, 0, 0);
	glm::vec3 m_Scale_ = glm::vec3(1, 1, 1);
	glm::vec3 m_Direction_ = glm::vec3(0, 0, 0);
	glm::vec3 m_Right_ = glm::vec3(0, 0, 0);
	glm::vec3 m_PitchAxis_ = glm::vec3(0, 0, 0);
	glm::vec3 m_HeadingAxis_ = glm::vec3(0, 0, 0);
	glm::quat m_CameraOrientation_ = glm::quat(1, 0, 0, 0);

	rpr_camera m_Camera_ = nullptr;
};












