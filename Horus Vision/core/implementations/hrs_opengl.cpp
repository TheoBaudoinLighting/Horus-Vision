#include "hrs_opengl.h"
#include "hrs_config.h"

#include <spdlog/spdlog.h>

#include "GLFW/glfw3.h"

bool HorusOpenGL::init(int width, int height, HorusWindowConfig* window)
{
	m_logger_.init();

	__super::init(width, height, window); // Call base class init

	if (!glfwInit())
	{
		spdlog::error("Failed to initialize GLFW");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

	glfwSetErrorCallback([](int error, const char* description) { spdlog::error("GLFW error: {}", description); });

	GLFWwindow* Horus_Window = glfwCreateWindow(window->m_window_width_, window->m_window_height_, window->m_window_title_.c_str(), nullptr, nullptr);

	if (!Horus_Window)
	{
		spdlog::error("Failed to create GLFW window");
		glfwTerminate();
		return false;
	}

	window->set_window(Horus_Window);

	glfwSetWindowUserPointer(Horus_Window, &window);
	glfwMakeContextCurrent(Horus_Window);
	glfwSwapInterval(1);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		spdlog::error("Failed to initialize GLAD");
		return false;
	}

	spdlog::info("OpenGL version: {}.{}", GLVersion.major, GLVersion.minor);
	spdlog::info("OpenGL initialized successfully.");

	return true;
}

void HorusOpenGL::init_render()
{
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void HorusOpenGL::post_render()
{
	glfwSwapBuffers(m_window_->get_window());
	glfwPollEvents();
}

void HorusOpenGL::quit_render()
{
	spdlog::info("Unloading OpenGL..");
	glfwDestroyWindow(m_window_->get_window());
	glfwTerminate();
	spdlog::info("OpenGL unloaded successfully.");

	return exit(0);
}