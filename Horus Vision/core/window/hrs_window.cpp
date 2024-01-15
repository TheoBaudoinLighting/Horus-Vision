
#include "hrs_window.h" // glad.h
#include "hrs_engine.h" // glfw.3

#include "imgui_notify.h"

#include "GLFW/glfw3.h"

void HorusWindow::init_contexts(int width, int height, HorusWindow* window)
{
	HorusOpenGL& OpenGL = HorusOpenGL::get_instance();
	HorusImGui& ImGui = HorusImGui::get_instance();
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	OpenGL.init(width, height, window);
	ImGui.init(width, height, window);
	Radeon.init(width, height, window);

	spdlog::info("OpenGL, ImGui and Radeon contexts initialized.");
}

bool HorusWindow::init_window(int width, int height, const std::string& title)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();
	HorusEngine& Engine = HorusEngine::get_instance();

	m_window_width_ = width;
	m_window_height_ = height;
	m_window_title_ = title;

	Engine.InitContexts(m_window_width_, m_window_height_, this);
	Radeon.init_graphics();

	ImGui::InsertNotification({ ImGuiToastType_Info, 3000, "Window initialized sucessfully !" });
	spdlog::info("Window initialized.");

	return m_IsRunning_;
}

void HorusWindow::render()
{
	HorusOpenGL& OpenGL = HorusOpenGL::get_instance();
	HorusImGui& ImGui = HorusImGui::get_instance();
	HorusRadeon& Radeon = HorusRadeon::get_instance();
	HorusEngine& Engine = HorusEngine::get_instance();

	m_IsClosing_ = Engine.get_is_closing();

	OpenGL.init_render();
	ImGui.init_render();

	Radeon.render_engine();
	// TODO Render UI Here

	ImGui.post_render();
	OpenGL.post_render();

	process_input();

	if (m_IsClosing_ == true)
	{
		close();
	}
}

void HorusWindow::mouse_button_callback(int button, int action, int mods)
{
}

void HorusWindow::key_callback(int key, int scancode, int action, int mods)
{
}

void HorusWindow::process_input()
{
	HorusEngine& Engine = HorusEngine::get_instance();
	HorusRadeon& Radeon = HorusRadeon::get_instance();


	// close callback
	if (glfwWindowShouldClose(m_window_))
	{
		close();
	}

	if (glfwGetKey(m_window_, GLFW_KEY_F11) == GLFW_PRESS)
	{
		if (glfwGetWindowAttrib(m_window_, GLFW_DECORATED) == GLFW_TRUE)
		{
			glfwSetWindowAttrib(m_window_, GLFW_DECORATED, GLFW_FALSE);
		}
		else
		{
			glfwSetWindowAttrib(m_window_, GLFW_DECORATED, GLFW_TRUE);
		}
	}

	if (glfwGetKey(m_window_, GLFW_KEY_F10) == GLFW_PRESS)
	{
		if (glfwGetWindowAttrib(m_window_, GLFW_FLOATING) == GLFW_TRUE)
		{
			glfwSetWindowAttrib(m_window_, GLFW_FLOATING, GLFW_FALSE);
		}
		else
		{
			glfwSetWindowAttrib(m_window_, GLFW_FLOATING, GLFW_TRUE);
		}
	}

	if (glfwGetKey(m_window_, GLFW_KEY_F9) == GLFW_PRESS)
	{
		if (glfwGetWindowAttrib(m_window_, GLFW_RESIZABLE) == GLFW_TRUE)
		{
			glfwSetWindowAttrib(m_window_, GLFW_RESIZABLE, GLFW_FALSE);
		}
		else
		{
			glfwSetWindowAttrib(m_window_, GLFW_RESIZABLE, GLFW_TRUE);
		}
	}

	if (glfwGetKey(m_window_, GLFW_KEY_F8) == GLFW_PRESS)
	{
		if (glfwGetWindowAttrib(m_window_, GLFW_MAXIMIZED) == GLFW_TRUE)
		{
			glfwSetWindowAttrib(m_window_, GLFW_MAXIMIZED, GLFW_FALSE);
		}
		else
		{
			glfwSetWindowAttrib(m_window_, GLFW_MAXIMIZED, GLFW_TRUE);
		}
	}

	if (glfwGetKey(m_window_, GLFW_KEY_F7) == GLFW_PRESS)
	{
		if (glfwGetWindowAttrib(m_window_, GLFW_VISIBLE) == GLFW_TRUE)
		{
			glfwSetWindowAttrib(m_window_, GLFW_VISIBLE, GLFW_FALSE);
		}
		else
		{
			glfwSetWindowAttrib(m_window_, GLFW_VISIBLE, GLFW_TRUE);
		}
	}

	if (glfwGetWindowAttrib(m_window_, GLFW_RESIZABLE) == GLFW_TRUE)
	{
		ImVec2 img_size = Engine.get_image_size();

		float img_aspect_ratio = Engine.get_image_aspect_ratio();
		float current_aspect_ratio = img_size.x / img_size.y;

		const float epsilon = 0.01f;

		if (abs(current_aspect_ratio - img_aspect_ratio) > epsilon)
		{
			if (current_aspect_ratio > img_aspect_ratio)
			{
				img_size.x = img_size.y * img_aspect_ratio;
			}
			else
			{
				img_size.y = img_size.x / img_aspect_ratio;
			}

			Radeon.resize_render(img_size.x, img_size.y);
		}
	}
}

void HorusWindow::close()
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();
	HorusImGui& ImGui = HorusImGui::get_instance();
	HorusOpenGL& OpenGL = HorusOpenGL::get_instance();

	Radeon.quit_render();
	ImGui.quit_render();
	OpenGL.quit_render();

	m_IsRunning_ = false;
}
