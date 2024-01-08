#pragma once

#include <RadeonProRender_v2.h>
#include <Math/float3.h>
#include <Math/matrix.h>

enum class CameraType
{
	Perspective,
	Orthographic
};

enum class CameraMode
{
	Free,
	Target
};

enum class CameraCalculation
{
	Euler,
	Quaternion
};

class HorusCamera
{
public:

	void init();
	void destroy();

	void bind();
	void unbind();

	void get_info();

	// Move camera
	void move_forward();
	void move_backward();
	void move_left();
	void move_right();
	void move_up();
	void move_down();

	void set_lookat(const RadeonProRender::float3& pivot);

	void scrolling(float dy);
	void tumbling(float dx, float dy);
	void track(float dx, float dy);
	void dolly(float distance);

	void compute_view_projection_matrix(float* view, float* projection, float ratio);


	void set_focal_length(float focal_length);
	void set_aperture_blades(int num_blades);
	void set_exposure(float exposure);
	void set_focal_distance(float focal_distance);
	void set_sensor_size(float sensor_size);
	void set_lens_shift(float lens_shift_x, float lens_shift_y);
	void set_tilt(float tilt_x, float tilt_y);
	void set_focal_tilt(float focal_tilt);
	void set_FStop(float FStop);
	void set_near_clip(float near_clip);
	void set_far_clip(float far_clip);

	void set_camera_speed(float speed);

	HorusCamera get_camera();

private:

	void update_camera();

	CameraType m_type_;
	CameraMode m_mode_;
	CameraCalculation m_calculation_;

	RadeonProRender::float3 m_position_;
	RadeonProRender::float3 m_lookat_ = RadeonProRender::float3(0.0f, 0.0f, 0.0f); // define center of the scene
	RadeonProRender::float3 m_up_;
	RadeonProRender::float3 m_right_;
	RadeonProRender::matrix m_transform_;
	RadeonProRender::float3 m_pivot_;
	
	RadeonProRender::float3 m_lookat_norm_;
	RadeonProRender::float3 m_direction_;
	RadeonProRender::float3 m_forward_;
	RadeonProRender::float3 m_motion_;
	RadeonProRender::float3 m_world_up_ = RadeonProRender::float3(0.0f, 1.0f, 0.0f);
	RadeonProRender::matrix m_matrix_;

	float m_camera_speed_ = 1.5f;

	float m_fov_ = 0;

	rpr_camera m_camera_ = nullptr;
};












