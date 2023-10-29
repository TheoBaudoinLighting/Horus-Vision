#pragma once

#include "GLAD/glad.h"

#include "hrs_radeon.h"

#include <memory>

#include "hrs_material_editor.h"
#include "imgui.h"

class HorusEngine
{
public:

	static HorusEngine& get_instance();

	int context_info(rpr_context& context);

	void ui_init();
	void ui_viewer(bool* p_open);
	void ui_property_editor(bool* p_open);
	void ui_outliner(bool* p_open);
	void ui_material_editor(bool* p_open);

	bool get_is_closing() { return m_IsClosing_; }

	ImVec2 get_image_size() { return img_size; }
	float get_image_aspect_ratio() { return aspect_ratio_viewer; }

	void call_reset_buffer();

private:

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

	bool m_IsClosing_ = false;

	glm::vec2 size_{};

	ImVec2 img_size;
	float aspect_ratio_viewer;

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

	inline static bool show_winui_viewer = true;
	inline static bool show_winui_property_editor = true;
	inline static bool show_winui_outliner = false;
	inline static bool show_winui_material_editor = true;
	inline static bool show_winui_main_menu_bar = false;
	inline static bool show_winui_material_browser = true;

	std::string selectedMesh;
	std::string selectedMaterial;
	std::string selectedCamera;
	std::map<std::string, int> meshNameToID;

	int m_min_samples_ = 4;
	int m_max_samples_ = 128;
	float m_adaptive_threshold_ = 0.01f;
};
