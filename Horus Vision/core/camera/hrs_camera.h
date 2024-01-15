#pragma once

//  Quaternion camera code adapted from: https://hamelot.io/visualization/moderngl-camera/

#include <RadeonProRender_v2.h>
#include <Math/float3.h>
#include <Math/matrix.h>

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

	HorusCamera() {}
	~HorusCamera() {}

	HorusCamera(const HorusCamera&) = delete;
	HorusCamera& operator=(const HorusCamera&) = delete;

	void Init();
	void Reset();
	void Destroy();

	void VariableCheckers(std::string name);

	void Bind();
	void Unbind();

	void GetCameraInfo();
	void UpdateCamera();

	void MoveCamera(CameraDirection dir);
	void ChangePitch(float degrees);
	void ChangeHeading(float degrees);

	void Move2D(int x, int y);

	void SetMode(CameraType cam_mode);
	void SetPosition(glm::vec3 pos);
	void SetLookAt(glm::vec3 pos);
	void SetFOV(double fov);
	void SetViewport(int loc_x, int loc_y, int width, int height);
	void SetClipping(double near_clip_distance, double far_clip_distance);

	void Tumbling(float x, float y);
	void Zoom(float distance);
	void Pan(float x, float y);

	void SetPos(int button, int state, int x, int y);

	// Getters
	CameraType GetMode();
	void GetViewport(int& loc_x, int& loc_y, int& width, int& height);
	void GetMatrices(glm::mat4& P, glm::mat4& V, glm::mat4& M);

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












