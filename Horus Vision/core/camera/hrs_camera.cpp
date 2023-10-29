#include "hrs_camera.h"

#include <RadeonProRender_v2.h>
#include <Math/mathutils.h>

#include "common.h"
#include "objects/hrs_object_manager.h"
#include "spdlog/spdlog.h"

using namespace std;
using namespace RadeonProRender;

HorusRadeon& radeon_ImpCam = HorusRadeon::get_instance();
HorusObjectManager& g_object_impco = HorusObjectManager::get_instance();

// Basic funtionality

bool are_orthogonal(const RadeonProRender::float3& a, const RadeonProRender::float3& b)
{
	float dotProduct = a.x * b.x + a.y * b.y + a.z * b.z;
	return abs(dotProduct) < 1e-6;  // tolérance numérique
}

float degrees(float radians)
{
	return radians * RAD_DEG;
}

float radians(float degrees)
{
	return degrees * DEG_RAD;
}

float length(const RadeonProRender::float3& v)
{
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

float distance(const RadeonProRender::float3& v1, const RadeonProRender::float3& v2)
{
	RadeonProRender::float3 v = v1;
	return length(v - v2);
}

void frustrum(float left, float right, float bottom, float top, float znear, float zfar, float* m16)
{
	float temp, temp2, temp3, temp4;

	temp = 2.0f * znear;
	temp2 = right - left;
	temp3 = top - bottom;
	temp4 = zfar - znear;

	m16[0] = temp / temp2;
	m16[1] = 0.0f;
	m16[2] = 0.0f;
	m16[3] = 0.0f;
	m16[4] = 0.0f;
	m16[5] = temp / temp3;
	m16[6] = 0.0f;
	m16[7] = 0.0f;
	m16[8] = (right + left) / temp2;
	m16[9] = (top + bottom) / temp3;
	m16[10] = (-zfar - znear) / temp4;
	m16[11] = -1.0f;
	m16[12] = 0.0f;
	m16[13] = 0.0f;
	m16[14] = (-temp * zfar) / temp4;
	m16[15] = 0.0f;
}

void perspective(float fovy, float aspect, float znear, float zfar, float* m16)
{
	float ymax, xmax;
	ymax = znear * tanf(fovy * PI / 180.0);
	xmax = ymax * aspect;
	frustrum(-xmax, xmax, -ymax, ymax, znear, zfar, m16);
}

void cross(const float* v1, const float* v2, float* result)
{
	result[0] = v1[1] * v2[2] - v1[2] * v2[1];
	result[1] = v1[2] * v2[0] - v1[0] * v2[2];
	result[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

float dot(const float* v1, const float* v2)
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

void normalize(const float* v, float* r)
{
	float invLen = 1.0f / sqrtf(dot(v, v)) + FLT_EPSILON;
	r[0] = v[0] * invLen;
	r[1] = v[1] * invLen;
	r[2] = v[2] * invLen;
}

void clamp(float* v, float min, float max)
{
	if (v[0] < min) v[0] = min;
	if (v[0] > max) v[0] = max;
	if (v[1] < min) v[1] = min;
	if (v[1] > max) v[1] = max;
	if (v[2] < min) v[2] = min;
	if (v[2] > max) v[2] = max;
}

void lookatF(const float* eye, const float* at, const float* up, float* m16)
{
	float x[3], y[3], z[3], tmp[3];

	tmp[0] = eye[0] - at[0];
	tmp[1] = eye[1] - at[1];
	tmp[2] = eye[2] - at[2];

	normalize(tmp, z);
	normalize(up, y);

	cross(y, z, tmp);
	normalize(tmp, x);

	cross(z, x, tmp);
	normalize(tmp, y);


	m16[0] = x[0];
	m16[1] = y[0];
	m16[2] = z[0];
	m16[3] = 0.0f;
	m16[4] = x[1];
	m16[5] = y[1];
	m16[6] = z[1];
	m16[7] = 0.0f;
	m16[8] = x[2];
	m16[9] = y[2];
	m16[10] = z[2];
	m16[11] = 0.0f;
	m16[12] = -dot(x, eye);
	m16[13] = -dot(y, eye);
	m16[14] = -dot(z, eye);
	m16[15] = 1.0f;
}

void HorusCamera::init()
{
	this->m_fov_ = radians(m_fov_);

	m_position_ = float3(0.0f, 5.0f, 20.0f);
	m_lookat_ = float3(0.0f, 0.0f, 0.0f);
	m_up_ = float3(0.0f, 1.0f, 0.0f);
	m_forward_ = normalize(m_lookat_ - m_position_);
	m_pivot_ = m_lookat_;

	CHECK(rprContextCreateCamera(radeon_ImpCam.get_context(), &m_camera_));

	CHECK(rprCameraLookAt(m_camera_, m_position_.x, m_position_.y, m_position_.z, m_lookat_.x, m_lookat_.y, m_lookat_.z, m_up_.x, m_up_.y, m_up_.z));

	//CHECK(rprCameraSetFocalLength(m_camera_, 75.0f));
	CHECK(rprSceneSetCamera(g_object_impco.get_scene(), m_camera_));
}

void HorusCamera::destroy()
{
	if (m_camera_)
	{
		CHECK(rprObjectDelete(m_camera_));
		m_camera_ = nullptr;
	}
}

void HorusCamera::bind()
{
	CHECK(rprSceneSetCamera(g_object_impco.get_scene(), m_camera_));
}

void HorusCamera::unbind()
{
	CHECK(rprSceneSetCamera(nullptr, m_camera_));
}

void HorusCamera::set_lookat(const float3& pivot)
{
	get_info();

	m_lookat_ = pivot;

	update_camera();
}

void HorusCamera::get_info()
{
	rprCameraGetInfo(m_camera_, RPR_CAMERA_POSITION, sizeof(m_position_), &m_position_, nullptr);
	rprCameraGetInfo(m_camera_, RPR_CAMERA_LOOKAT, sizeof(m_lookat_), &m_lookat_, nullptr);
	rprCameraGetInfo(m_camera_, RPR_CAMERA_UP, sizeof(m_up_), &m_up_, nullptr);
}

void HorusCamera::move_forward()
{
	get_info();

	m_forward_ = normalize(m_lookat_ - m_position_);

	float3 lookat = m_lookat_ - m_position_;
	lookat.normalize();

	m_position_ = m_position_ + lookat * m_camera_speed_;

	update_camera();
}

void HorusCamera::move_backward()
{
	get_info();

	m_forward_ = normalize(m_lookat_ - m_position_);

	float3 lookat = m_lookat_ - m_position_;
	lookat.normalize();

	m_position_ = m_position_ - lookat * m_camera_speed_;

	update_camera();
}

void HorusCamera::move_left()
{
	get_info();

	float3 right = RadeonProRender::cross(m_lookat_ - m_position_, m_up_);
	right.normalize();

	m_position_ = m_position_ - right * m_camera_speed_;
	update_camera();
}

void HorusCamera::move_right()
{
	get_info();

	float3 right = RadeonProRender::cross(m_lookat_ - m_position_, m_up_);
	right.normalize();

	m_position_ = m_position_ + right * m_camera_speed_;
	update_camera();
}

void HorusCamera::move_up()
{
	get_info();

	float3 up = RadeonProRender::cross(RadeonProRender::cross(m_lookat_ - m_position_, m_up_), m_lookat_ - m_position_);
	up.normalize();

	m_position_ = m_position_ + up * m_camera_speed_;
	update_camera();
}

void HorusCamera::move_down()
{
	get_info();

	float3 up = RadeonProRender::cross(RadeonProRender::cross(m_lookat_ - m_position_, m_up_), m_lookat_ - m_position_);
	up.normalize();

	m_position_ = m_position_ - up * m_camera_speed_;
	update_camera();
}

void HorusCamera::scrolling(float dy)
{
	get_info();

	if (dy > 0)
	{
		m_forward_ = normalize(m_lookat_ - m_position_);

		float3 lookat = m_lookat_ - m_position_;
		lookat.normalize();

		m_position_ = m_position_ + lookat * m_camera_speed_;
	}
	else
	{
		m_forward_ = normalize(m_lookat_ - m_position_);

		float3 lookat = m_lookat_ - m_position_;
		lookat.normalize();

		m_position_ = m_position_ - lookat * m_camera_speed_;
	}

	update_camera();
}

void HorusCamera::tumbling(float dx, float dy)
{
	get_info();

	float angle_x = dx * 1.0f;
	float angle_y = dy * 1.0f;

	
	RadeonProRender::float3 camera_to_pivot = m_position_ - m_lookat_;

	float radius = length(camera_to_pivot);
	float current_yaw = atan2f(camera_to_pivot.z, camera_to_pivot.x);
	float current_pitch = asinf(camera_to_pivot.y / radius);

	current_yaw += angle_x;
	current_pitch += angle_y;


	const float min_pitch = radians(90.0f);
	current_pitch = std::max(-min_pitch, std::min(min_pitch, current_pitch));

	camera_to_pivot.x = radius * cosf(current_pitch) * cosf(current_yaw);
	camera_to_pivot.y = radius * sinf(current_pitch);
	camera_to_pivot.z = radius * cosf(current_pitch) * sinf(current_yaw);

	m_position_ = m_lookat_ + camera_to_pivot;

	update_camera();
}

void HorusCamera::track(float dx, float dy)
{
	get_info();

	RadeonProRender::float3 forward = m_lookat_ - m_position_;  
	RadeonProRender::float3 right = cross(m_up_, forward);

	right = normalize(right);
	RadeonProRender::float3 up = normalize(m_up_);

	m_position_ += right * dx + up * dy;
	m_lookat_ += right * dx + up * dy;

	update_camera();
}

void HorusCamera::dolly(float distance)
{
	get_info();

	m_forward_ = normalize(m_lookat_ - m_position_);
	m_right_ = normalize(cross(m_forward_, m_up_));

	RadeonProRender::float3 translation = m_forward_ * distance;

	m_position_ = m_position_ + translation;
	m_lookat_ = m_lookat_ + translation;

	update_camera();
}

void HorusCamera::compute_view_projection_matrix(float* view, float* projection, float ratio)
{
	get_info();

	lookatF(&m_position_.x, &m_lookat_.x, &m_up_.x, view);

	const float aspect = ratio;

	perspective(degrees(m_fov_), aspect, 0.1f, 1000.0f, projection);

}

void HorusCamera::set_focal_length(float focal_length)
{
	CHECK(rprCameraSetFocalLength(m_camera_, focal_length));
}

void HorusCamera::set_aperture_blades(int num_blades)
{
	CHECK(rprCameraSetApertureBlades(m_camera_, num_blades));
}

void HorusCamera::set_exposure(float exposure)
{
	CHECK(rprCameraSetExposure(m_camera_, exposure));
}

void HorusCamera::set_focal_distance(float focal_distance)
{
	CHECK(rprCameraSetFocusDistance(m_camera_, focal_distance));
}

void HorusCamera::set_sensor_size(float sensor_size)
{
	CHECK(rprCameraSetSensorSize(m_camera_, sensor_size, sensor_size));
}

void HorusCamera::set_lens_shift(float lens_shift_x, float lens_shift_y)
{
	CHECK(rprCameraSetLensShift(m_camera_, lens_shift_x, lens_shift_y));
}

void HorusCamera::set_tilt(float tilt_x, float tilt_y)
{
	CHECK(rprCameraSetTiltCorrection(m_camera_, tilt_x, tilt_y));
}

void HorusCamera::set_focal_tilt(float focal_tilt)
{
	CHECK(rprCameraSetFocalTilt(m_camera_, focal_tilt));
}

void HorusCamera::set_FStop(float FStop)
{
	CHECK(rprCameraSetFStop(m_camera_, FStop));
}

void HorusCamera::set_near_clip(float near_clip)
{
	CHECK(rprCameraSetNearPlane(m_camera_, near_clip));
}

void HorusCamera::set_far_clip(float far_clip)
{
	CHECK(rprCameraSetFarPlane(m_camera_, far_clip));
}

void HorusCamera::set_camera_speed(float speed)
{
		m_camera_speed_ = speed;
}

HorusCamera HorusCamera::get_camera()
{
	return *this;
}

void HorusCamera::update_camera()
{
	rprCameraLookAt(m_camera_, m_position_.x, m_position_.y, m_position_.z, m_lookat_.x, m_lookat_.y, m_lookat_.z, m_up_.x, m_up_.y, m_up_.z);
}
