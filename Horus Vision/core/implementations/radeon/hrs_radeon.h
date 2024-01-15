#pragma once

// Project includes
#include "common.h" // nothing
#include "hrs_garbage_collector.h" // nothing
#include "hrs_context.h" // glfw3.h

// External includes
#include "glm/glm.hpp"
#include "spdlog/spdlog.h"

class HorusRadeon : public HorusContext
{
public:

	static HorusRadeon& get_instance()
	{
		static HorusRadeon instance; // Instance unique
		return instance;
	}

	HorusRadeon(const HorusRadeon&) = delete;
	void operator=(const HorusRadeon&) = delete;

	void create_frame_buffers(int width, int height);
	bool init(int width, int height, HorusWindowConfig* window) override;

	void init_render() override { }
	void post_render() override { }
	void quit_render() override;

	bool init_graphics();
	void resize_render(int width, int height);

	void render_engine();
	void render_engine_tiled();

	float get_render_progress();

	glm::vec2 set_window_size(int width, int height);

	rpr_context get_context() { return m_context_; }
	rpr_material_system get_matsys() { return m_matsys_; }

	GLuint get_texture_buffer() { return m_texture_buffer_; } // Buffer to render in ImGui for Radeon

	rpr_framebuffer get_frame_buffer() { return m_frame_buffer_; }
	rpr_framebuffer get_frame_buffer_resolved() { return m_frame_buffer_2_; }

	void set_sample_count(int sample_count) { m_sample_count_ = sample_count; }

	bool get_is_dirty() { return m_is_dirty_; }
	bool set_is_dirty(bool is_dirty) { m_is_dirty_ = is_dirty; return m_is_dirty_; }

	int get_sample_count() { return m_sample_count_; }
	int get_min_samples() { return m_min_samples_; }
	int get_max_samples() { return m_max_samples_; }
	int set_min_samples(int min_samples) { m_min_samples_ = min_samples; return m_min_samples_; }
	int set_max_samples(int max_samples) { 	m_max_samples_ = max_samples; return m_max_samples_; }

private:

	HorusRadeon() {}
	
	// Render stuff

	int m_window_width_ = 0;
	int m_window_height_ = 0;
	HorusWindowConfig* m_window_config_;

	int m_min_samples_ = 4;
	int m_max_samples_ = 32;
	int m_sample_count_ = 0;
	int m_batch_size_ = 0;

	bool m_thread_running_ = false;
	std::thread m_render_thread_;

	bool m_is_dirty_;

	// Radeon stuff

	GLuint m_program_;

	GLuint m_texture_buffer_ = 0;
	GLuint m_vertex_buffer_id_ = 0;
	GLuint m_index_buffer_id_ = 0;

	GLuint m_up_buffer_ = 0;

	rpr_framebuffer m_frame_buffer_ = nullptr;
	rpr_framebuffer m_frame_buffer_2_ = nullptr;

	rpr_context m_context_ = nullptr;
	rpr_material_system m_matsys_ = nullptr;
	rpr_camera m_camera_ = nullptr;

	std::shared_ptr<float> m_fb_data_ = nullptr;
};