
#include "hrs_window.h" // glad.h
#include "hrs_engine.h" // glfw.3

#include "imgui_notify.h"

#include "GLFW/glfw3.h"

void HorusWindow::InitContexts(int width, int height, HorusWindow* window)
{
	HorusOpenGL& OpenGL = HorusOpenGL::GetInstance();
	HorusImGui& ImGui = HorusImGui::GetInstance();
	HorusRadeon& Radeon = HorusRadeon::GetInstance();

	OpenGL.Init(width, height, window);
	ImGui.Init(width, height, window);
	Radeon.Init(width, height, window);

	spdlog::info("OpenGL, ImGui and Radeon contexts initialized.");
}

bool HorusWindow::InitWindow(int width, int height, const std::string& title)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusEngine& Engine = HorusEngine::GetInstance();

	m_WindowWidth_ = width;
	m_WindowHeight_ = height;
	m_WindowTitle_ = title;

	Engine.InitContexts(m_WindowWidth_, m_WindowHeight_, this);
	Radeon.InitGraphics();

	ImGui::InsertNotification({ ImGuiToastType_Info, 3000, "Window initialized sucessfully !" });
	spdlog::info("Window initialized.");

	return m_IsRunning_;
}

void HorusWindow::Render()
{
	HorusOpenGL& OpenGL = HorusOpenGL::GetInstance();
	HorusImGui& ImGui = HorusImGui::GetInstance();
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusEngine& Engine = HorusEngine::GetInstance();

	m_IsClosing_ = Engine.GetIsClosing();

	OpenGL.InitRender();
	ImGui.InitRender();

	Radeon.RenderEngine();
	// TODO Render UI Here

	ImGui.PostRender();
	OpenGL.PostRender();

	ProcessInput();

	if (m_IsClosing_ == true)
	{
		Close();
	}
}

void HorusWindow::mouse_button_callback(int button, int action, int mods)
{
}

void HorusWindow::key_callback(int key, int scancode, int action, int mods)
{
}

void HorusWindow::ProcessInput()
{
	HorusEngine& Engine = HorusEngine::GetInstance();
	HorusRadeon& Radeon = HorusRadeon::GetInstance();


	// close callback
	if (glfwWindowShouldClose(m_Window_))
	{
		Close();
	}

	if (glfwGetKey(m_Window_, GLFW_KEY_F11) == GLFW_PRESS)
	{
		if (glfwGetWindowAttrib(m_Window_, GLFW_DECORATED) == GLFW_TRUE)
		{
			glfwSetWindowAttrib(m_Window_, GLFW_DECORATED, GLFW_FALSE);
		}
		else
		{
			glfwSetWindowAttrib(m_Window_, GLFW_DECORATED, GLFW_TRUE);
		}
	}

	if (glfwGetKey(m_Window_, GLFW_KEY_F10) == GLFW_PRESS)
	{
		if (glfwGetWindowAttrib(m_Window_, GLFW_FLOATING) == GLFW_TRUE)
		{
			glfwSetWindowAttrib(m_Window_, GLFW_FLOATING, GLFW_FALSE);
		}
		else
		{
			glfwSetWindowAttrib(m_Window_, GLFW_FLOATING, GLFW_TRUE);
		}
	}

	if (glfwGetKey(m_Window_, GLFW_KEY_F9) == GLFW_PRESS)
	{
		if (glfwGetWindowAttrib(m_Window_, GLFW_RESIZABLE) == GLFW_TRUE)
		{
			glfwSetWindowAttrib(m_Window_, GLFW_RESIZABLE, GLFW_FALSE);
		}
		else
		{
			glfwSetWindowAttrib(m_Window_, GLFW_RESIZABLE, GLFW_TRUE);
		}
	}

	if (glfwGetKey(m_Window_, GLFW_KEY_F8) == GLFW_PRESS)
	{
		if (glfwGetWindowAttrib(m_Window_, GLFW_MAXIMIZED) == GLFW_TRUE)
		{
			glfwSetWindowAttrib(m_Window_, GLFW_MAXIMIZED, GLFW_FALSE);
		}
		else
		{
			glfwSetWindowAttrib(m_Window_, GLFW_MAXIMIZED, GLFW_TRUE);
		}
	}

	if (glfwGetKey(m_Window_, GLFW_KEY_F7) == GLFW_PRESS)
	{
		if (glfwGetWindowAttrib(m_Window_, GLFW_VISIBLE) == GLFW_TRUE)
		{
			glfwSetWindowAttrib(m_Window_, GLFW_VISIBLE, GLFW_FALSE);
		}
		else
		{
			glfwSetWindowAttrib(m_Window_, GLFW_VISIBLE, GLFW_TRUE);
		}
	}

	if (glfwGetWindowAttrib(m_Window_, GLFW_RESIZABLE) == GLFW_TRUE)
	{
		ImVec2 ImgSize = Engine.GetImageSize();

		float ImgAspectRatio = Engine.GetImageAspectRatio();
		float CurrentAspectRatio = ImgSize.x / ImgSize.y;

		if (const float Epsilon = 0.01f; abs(CurrentAspectRatio - ImgAspectRatio) > Epsilon)
		{
			if (CurrentAspectRatio > ImgAspectRatio)
			{
				ImgSize.x = ImgSize.y * ImgAspectRatio;
			}
			else
			{
				ImgSize.y = ImgSize.x / ImgAspectRatio;
			}

			Radeon.ResizeRender(ImgSize.x, ImgSize.y);
		}
	}
}

void HorusWindow::Close()
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusImGui& ImGui = HorusImGui::GetInstance();
	HorusOpenGL& OpenGL = HorusOpenGL::GetInstance();

	Radeon.QuitRender();
	ImGui.QuitRender();
	OpenGL.QuitRender();

	m_IsRunning_ = false;
}
