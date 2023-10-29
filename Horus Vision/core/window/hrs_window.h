#pragma once

#include "hrs_opengl.h"

#include "hrs_config.h"
#include "hrs_radeon.h"
#include "hrs_imgui.h"

#include <RadeonProRender_v2.h>
#include <glm/glm.hpp>

#include "hrs_engine.h"
#include "GLFW/glfw3.h"

class HorusWindow : public HorusWindowConfig
{
public:

	HorusWindow() : m_window_(nullptr), m_radeon_context_(HorusRadeon::get_instance()),
	                m_opengl_context_(HorusOpenGL::get_instance()), m_imgui_context_(HorusImGui::get_instance()),
	                m_engine_(HorusEngine::get_instance()),
	                m_IsRunning_(true),
	                m_IsFullscreen_(false),
	                m_IsVSync_(true), m_IsResizable_(true), m_IsDecorated_(true), m_IsFocused_(false),
	                m_IsMaximized_(false), m_IsMinimized_(false), m_IsHovered_(false), m_IsClosing_(false),
	                m_IsKeyPressed_(false), m_fps_(0), m_mouse_wheel_(0), m_KeyState_{}, m_mouse_pos_(glm::vec2(0.0f)),
	                m_mouse_motion_(glm::vec2(0.0f))
	{
	}

	~HorusWindow() = default;

	void init_contexts(int width, int height, HorusWindow* window);

	bool init_window(int width, int height, const std::string& title);
	void render();

	GLFWwindow* get_window() override { return m_window_; }

	void set_window(GLFWwindow* window) override { m_window_ = window; }

	static HorusWindow* from_native_window(GLFWwindow& window);

	void scroll_callback(double xoffset, double yoffset) override { m_mouse_wheel_ = yoffset; }
	void mouse_button_callback(int button, int action, int mods) override;
	void key_callback(int key, int scancode, int action, int mods) override;
	void resize_callback(int width, int height) override { m_window_width_ = width; m_window_height_ = height; }
	void close_callback() override {}
	void process_input();
	void close();

	bool is_running() const { return m_IsRunning_; }
	bool is_fullscreen() const { return m_IsFullscreen_; }
	bool is_vsync() const { return m_IsVSync_; }
	bool is_resizable() const { return m_IsResizable_; }
	bool is_decorated() const { return m_IsDecorated_; }
	bool is_focused() const { return m_IsFocused_; }
	bool is_maximized() const { return m_IsMaximized_; }
	bool is_minimized() const { return m_IsMinimized_; }
	bool is_hovered() const { return m_IsHovered_; }
	bool is_closing() const { return m_IsClosing_; }

	int get_fps() const { return m_fps_; }
	int get_height() const { return m_window_height_; }
	int get_width() const { return m_window_width_; }

private:

	GLFWwindow* m_window_;

	HorusRadeon& m_radeon_context_;
	HorusOpenGL& m_opengl_context_;
	HorusImGui& m_imgui_context_;

	HorusEngine& m_engine_;

	bool m_IsRunning_;
	bool m_IsFullscreen_;
	bool m_IsVSync_;
	bool m_IsResizable_;
	bool m_IsDecorated_;
	bool m_IsFocused_;
	bool m_IsMaximized_;
	bool m_IsMinimized_;
	bool m_IsHovered_;
	bool m_IsClosing_ = false;
	bool m_IsKeyPressed_;

	int m_fps_;
	int m_mouse_wheel_;

	char m_KeyState_[256];

	glm::vec2 m_mouse_pos_;
	glm::vec2 m_mouse_motion_;

	rpr_camera m_camera_{};
	rpr_context m_context_{};
	rpr_framebuffer m_framebuffer_{};
	rpr_scene m_scene_{};
};
