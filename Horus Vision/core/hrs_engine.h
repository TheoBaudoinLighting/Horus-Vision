#pragma once

//#include "GLAD/glad.h"

#include "hrs_radeon.h"

#include <memory>

#include "hrs_material_editor.h"
#include "imgui.h"

class HorusEngine
{
public:

	static HorusEngine& get_instance()
	{
		static HorusEngine instance;
		return instance;
	}

	HorusEngine(HorusEngine const&) = delete;
	void operator=(HorusEngine const&) = delete;

	int context_info(rpr_context& context);

	void ui_init();
	void ui_viewer(bool* p_open);
	void ui_end_rendering_overlay(bool* p_open);
	void ui_property_editor(bool* p_open);
	void ui_outliner(bool* p_open);
	void ui_material_editor(bool* p_open);
	void ui_viewer_rt(bool* p_open);

	void update_performance_data(float currentFPS, float currentSampling)
	{
		fps_data_[current_frame_ % data_points_count] = currentFPS;
		sampling_data_[current_frame_ % data_points_count] = currentSampling;
		current_frame_++;
	}

	bool get_is_closing() { return m_is_closing_; }

	ImVec2 get_image_size() { return img_size; }
	float get_image_aspect_ratio() { return aspect_ratio_viewer; }

	void call_reset_buffer();

private:

	HorusEngine() {}

	const float desired_width_ = 800.0f;
	char userInput[256] = "";
	int suffix = 001;

	int camera_number_ = 0;
	int light_number_ = 0;
	int mesh_number_ = 0;
	int material_number_ = 0;
	int image_number_ = 0;
	int post_effect_number_ = 0;
	int volume_number_ = 0;
	int curve_number_ = 0;
	int grid_number_ = 0;
	int buffer_number_ = 0;
	int framebuffer_number_ = 0;
	int scene_number_ = 0;
	int material_node_number_ = 0;
	int composite_number_ = 0;
	int lut_number_ = 0;

	rpr_uint numFrames;
	rpr_render_statistics render_statistics_;

	bool enable_adaptive_sampling_ = false;
	bool enable_preview_mode_ = false;
	bool enable_AA_ = true;
	bool show_object_contour_ = false;
	int samplerType = RPR_CONTEXT_SAMPLER_TYPE_SOBOL;

	char GPU00N[1024];
	char GPU01N[1024];

	std::vector<float> frame;
	std::vector<float> samples_mkr;

	bool m_is_closing_ = false;

	glm::vec2 size_{};

	ImVec2 img_size;
	float aspect_ratio_viewer;
	ImVec2 stored_image_position_;

	bool enable_render_region_ = false;
	bool previous_enable_adaptive_sampling_ = false;
	bool previous_enable_AA_ = false;
	bool previous_reset_mode_ = false;
	bool m_ResetBuffer_ = false;
	bool enable_backdrop_image_ = false;
	bool previous_enable_backdrop_image_ = false;
	bool enable_denoiser_ = false;
	bool enable_russian_roulette_ = false;
	bool previous_enable_russian_roulette_ = false;
	bool previous_enable_transparent_background = false;
	bool enable_transparent_background = false;
	long long m_chrono_time_;

	inline static float last_progress = -1.0f;
	inline static bool has_started = false;
	inline static bool is_options_changed = false;
	inline static std::chrono::high_resolution_clock::time_point start_time;
	inline static std::chrono::high_resolution_clock::time_point end_time;
	long long total_seconds;
	long long hours;
	long long minutes;
	long long seconds;
	long long milliseconds;

	static const int data_points_count = 100;


	float fps_data_[data_points_count] = {};
	float sampling_data_[data_points_count] = {};
	int current_frame_ = 0;

	inline static bool show_winui_viewer = true;
	inline static bool show_winui_property_editor = true;
	inline static bool show_winui_outliner = false;
	inline static bool show_winui_material_editor = false;
	inline static bool show_winui_main_menu_bar = false;
	inline static bool show_winui_material_browser = true;
	inline static bool show_winui_viewer_rt = true;

	std::string selectedMesh;
	std::string selectedMaterial;
	std::string selectedCamera;
	std::map<std::string, int> meshNameToID;

	int m_min_samples_ = 4;
	int m_max_samples_ = 128;
	float m_adaptive_threshold_ = 0.01f;
};
