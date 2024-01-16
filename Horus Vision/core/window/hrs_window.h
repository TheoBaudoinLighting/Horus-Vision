#pragma once

// Project includes
#include "hrs_opengl.h" // glad.h
#include "hrs_radeon.h" // glfw3.h
#include "hrs_config.h" // glfw3.h
#include "hrs_imgui.h" // glfw3.h

// External includes
#include <RadeonProRender_v2.h>
#include <glm/glm.hpp>

class HorusWindow : public HorusWindowConfig
{
public:

	HorusWindow() : m_Window_(nullptr),
	                m_IsRunning_(true),
	                m_IsFullscreen_(false),
	                m_IsVSync_(true), m_IsResizable_(true), m_IsDecorated_(true), m_IsFocused_(false),
	                m_IsMaximized_(false), m_IsMinimized_(false), m_IsHovered_(false),
	                m_IsKeyPressed_(false), m_Fps_(0), m_MouseWheel_(0), m_KeyState_{}, m_MousePos_(glm::vec2(0.0f)),
	                m_MouseMotion_(glm::vec2(0.0f))
	{
	}

	~HorusWindow() = default;

	void InitContexts(int width, int height, HorusWindow* window);

	bool InitWindow(int width, int height, const std::string& title);
	void Render();

	GLFWwindow* get_window() override { return m_Window_; }

	void set_window(GLFWwindow* window) override { m_Window_ = window; }

	static HorusWindow* from_native_window(GLFWwindow& window);

	void OnScroll(GLFWwindow* window, double xoffset, double yoffset) override {}
	void OnKey(GLFWwindow* window, int key, int scancode, int action, int mods) override {}
	void OnMouse(GLFWwindow* window, int button, int action, int mods) override {}
	void OnMouseMove(GLFWwindow* window, double xpos, double ypos) override {}
	void OnResize(GLFWwindow* window, int width, int height) override {}
	void OnClose() override {}

	void scroll_callback(double xoffset, double yoffset) override { m_MouseWheel_ = yoffset; }
	void mouse_button_callback(int button, int action, int mods) override;
	void key_callback(int key, int scancode, int action, int mods) override;
	void resize_callback(int width, int height) override { m_WindowWidth_ = width; m_WindowHeight_ = height; }
	void close_callback() override {}
	void ProcessInput();
	void Close();

	bool IsRunning() const { return m_IsRunning_; }
	bool IsFullscreen() const { return m_IsFullscreen_; }
	bool IsVsync() const { return m_IsVSync_; }
	bool IsResizable() const { return m_IsResizable_; }
	bool IsDecorated() const { return m_IsDecorated_; }
	bool IsFocused() const { return m_IsFocused_; }
	bool IsMaximized() const { return m_IsMaximized_; }
	bool IsMinimized() const { return m_IsMinimized_; }
	bool IsHovered() const { return m_IsHovered_; }
	bool IsClosing() const { return m_IsClosing_; }

	int GetFps() const { return m_Fps_; }
	int GetHeight() const { return m_WindowHeight_; }
	int GetWidth() const { return m_WindowWidth_; }

private:

	GLFWwindow* m_Window_;

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

	int m_Fps_;
	int m_MouseWheel_;

	char m_KeyState_[256];

	glm::vec2 m_MousePos_;
	glm::vec2 m_MouseMotion_;

	rpr_camera m_Camera_{};
	rpr_context m_Context_{};
	rpr_framebuffer m_Framebuffer_{};
	rpr_scene m_Scene_{};
};
