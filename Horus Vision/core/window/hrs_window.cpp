#include "hrs_window.h"

HorusEngine& m_engine_ = HorusEngine::get_instance();
HorusRadeon& m_radeon_context_ = HorusRadeon::get_instance();
HorusOpenGL& m_opengl_context_ = HorusOpenGL::get_instance();
HorusImGui& m_imgui_context_ = HorusImGui::get_instance();

void HorusWindow::init_contexts(int width, int height, HorusWindow* window)
{
	m_opengl_context_.init(width, height, window);
	m_imgui_context_.init(width, height, window);
	m_radeon_context_.init(width, height, window);

	spdlog::info("OpenGL, ImGui and Radeon contexts initialized.");
}

bool HorusWindow::init_window(int width, int height, const std::string& title)
{
	m_window_width_ = width;
	m_window_height_ = height;
	m_window_title_ = title;

	init_contexts(m_window_width_, m_window_height_, this);

	m_radeon_context_.init_graphics();
	m_radeon_context_.init_render();

	spdlog::info("Window initialized.");

	return m_IsRunning_;
}

void HorusWindow::render()
{
	m_IsClosing_ = m_engine_.get_is_closing();

	m_opengl_context_.init_render();
	m_imgui_context_.init_render();
	m_radeon_context_.init_render();

	m_radeon_context_.render_engine();

	m_engine_.ui_init();

	m_imgui_context_.post_render();
	m_opengl_context_.post_render();
	m_radeon_context_.post_render();

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
	if (glfwGetKey(m_window_, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		close();
	}

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
		ImVec2 img_size = m_engine_.get_image_size();

		float img_aspect_ratio = m_engine_.get_image_aspect_ratio();
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

			m_radeon_context_.resize_render(img_size.x, img_size.y);
		}
	}
}

void HorusWindow::close()
{
	m_radeon_context_.quit_render();
	m_imgui_context_.quit_render();
	m_opengl_context_.quit_render();

	m_IsRunning_ = false;
}
