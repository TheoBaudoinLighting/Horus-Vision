
#include "hrs_window.h" // glad.h
#include "hrs_engine.h" // glfw.3

#include "imgui_notify.h"

#include "GLFW/glfw3.h"

void HorusWindow::InitContexts(int width, int height, HorusWindow* window)
{
	HorusOpenGL& OpenGl = HorusOpenGL::GetInstance();
	HorusImGuiCore& ImGuiCore = HorusImGuiCore::GetInstance();
	HorusRadeon& Radeon = HorusRadeon::GetInstance();

	OpenGl.Init(width, height, window);
	ImGuiCore.Init(width, height, window);
	Radeon.Init(width, height, window);

	spdlog::info("OpenGL, ImGui and Radeon contexts initialized.");
}
bool HorusWindow::InitWindow(int width, int height, const std::string& title)
{
	HorusEngine& Engine = HorusEngine::GetInstance();

	WindowWidth = width;
	WindowHeight = height;
	WindowTitle = title;

	//Engine.InitContexts(WindowWidth, WindowHeight, this);

	ImGui::InsertNotification({ ImGuiToastType_Info, 3000, "Window initialized sucessfully !" });
	spdlog::info("Window initialized.");

	return m_IsRunning_;
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

			Radeon.ResizeRenderWindow(ImgSize.x, ImgSize.y);
		}
	}
}
void HorusWindow::Close()
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusImGuiCore& ImGuiCore = HorusImGuiCore::GetInstance();
	HorusOpenGL& OpenGL = HorusOpenGL::GetInstance();

	Radeon.QuitRender();
	ImGuiCore.QuitRender();
	OpenGL.QuitRender();

	m_IsRunning_ = false;
}
