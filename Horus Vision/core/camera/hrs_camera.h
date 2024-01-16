#pragma once

#include <RadeonProRender_v2.h>
#include <Math/mathutils.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
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

class HorusCamera
{
public:

	HorusCamera(): m_CameraType_(), m_Viewport_X_(0), m_Viewport_Y_(0), _Window_Width_(0), _Window_Height_(0),
	               m_Aspect_(0), m_Fov_(0),
	               m_Near_(0),
	               m_Far_(0), m_FStop_(0),
	               m_Transform_(),
	               m_LookAt_(), m_Up_(),
	               m_Position_(),
	               m_CameraScale_(),
	               m_Direction_(),
	               m_Right_(),
	               m_PitchAxis_(),
	               m_HeadingAxis_()
	{
	}

	~HorusCamera() {}

	HorusCamera(const HorusCamera&) = delete;
	HorusCamera& operator=(const HorusCamera&) = delete;

	void Init();
	void Reset();
	void Destroy();

	void VariableCheckers(std::string Name);

	void Bind();
	void Unbind();

	void GetCameraInfo();
	void UpdateCamera();

	void MoveCamera(CameraDirection dir);
	void ChangePitch(float degrees);
	void ChangeHeading(float degrees);

	void Move2D(int x, int y);

	void Tumbling(float x, float y);
	void Zoom(float distance);
	void Pan(float x, float y);

	void SetPos(int button, int state, int x, int y);

	// Getters
	CameraType GetMode();
	void GetViewport(int& loc_x, int& loc_y, int& width, int& height);
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

	double GetFOV();
	double GetNear();
	double GetFar();
	double GetFStop();
	double GetAspect();

	std::string& GetName() { return name; }
	
	// Setters
	void SetMode(CameraType cam_mode);
	void SetPosition(glm::vec3 pos);
	void SetLookAt(glm::vec3 pos);
	void SetFOV(double fov);
	void SetViewport(int loc_x, int loc_y, int width, int height);
	void SetClipping(double near_clip_distance, double far_clip_distance);
	void SetAspect(double aspect);
	void SetCameraScale(glm::vec3 scale);
	void SetCameraRotation(float pitch, float yaw, float roll);

private:

	std::string name;

	CameraType m_CameraType_;

	int m_Viewport_X_;
	int m_Viewport_Y_;

	int _Window_Width_;
	int _Window_Height_;

	double m_Aspect_;
	double m_Fov_;
	double m_Near_;
	double m_Far_;
	double m_FStop_;

	glm::vec3 m_Transform_;
	glm::vec3 m_LookAt_;
	glm::vec3 m_Up_;
	glm::vec3 m_Position_;
	glm::vec3 m_CameraScale_;
	glm::vec3 m_Direction_;
	glm::vec3 m_Right_;
	glm::vec3 m_PitchAxis_;
	glm::vec3 m_HeadingAxis_;

	rpr_camera m_camera_ = nullptr;
};












