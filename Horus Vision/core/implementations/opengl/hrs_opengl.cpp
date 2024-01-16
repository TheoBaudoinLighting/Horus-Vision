#include "hrs_opengl.h" // glad.h
#include "hrs_config.h" // glfw3.h

#include <spdlog/spdlog.h>
#include <hrs_console.h>

#include <stdio.h>
#include <cassert>
#include <chrono>

void GLAPIENTRY MessageCallback(GLenum type, [[maybe_unused]] GLuint id, GLenum severity, [[maybe_unused]] GLsizei length, const GLchar* message, [[maybe_unused]] const void* userParam) 
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}

void KeyCallbacks(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	HorusInput::GetInstance().OnKey(window, key, scancode, action, mods);
}
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	HorusInput::GetInstance().OnScroll(window, xoffset, yoffset);
}
void MouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	HorusInput::GetInstance().OnMouse(window, button, action, mods);
}
void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
	HorusInput::GetInstance().OnMouseMove(window, xpos, ypos);
}
void WindowResizeCallback(GLFWwindow* window, int width, int height)
{
	HorusInput::GetInstance().OnResize(window, width, height);
}
void WindowCloseCallback(GLFWwindow* window)
{
	HorusInput::GetInstance().OnClose();
}

bool HorusOpenGL::Init(int width, int height, HorusWindowConfig* window)
{
	HorusShaderManager& ShaderManager = HorusShaderManager::GetInstance();
	[[maybe_unused]] HorusOpenGLManager& OpenGLManager = HorusOpenGLManager::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	m_logger_.Init();

	spdlog::info("Horus Engine initialization");
	Console.AddLog(" [info] Horus Engine initialization");

	__super::Init(width, height, window); // Call base class init

	m_WindowWidth_ = width;
	m_WindowHeight_ = height;


	if (!glfwInit())
	{
		spdlog::error("Failed to initialize GLFW");
		return false;
	}

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL

	glfwSetErrorCallback([]([[maybe_unused]] int Error, const char* Description) { spdlog::error("GLFW error: {}", Description); });

	m_HorusWindow_ = glfwCreateWindow(m_WindowWidth_, m_WindowHeight_, window->m_WindowTitle_.c_str(), nullptr, nullptr);

	if (!m_HorusWindow_)
	{
		spdlog::error("Failed to create GLFW window");
		glfwTerminate();
		return false;
	}

	window->set_window(m_HorusWindow_);

	glfwSetWindowUserPointer(m_HorusWindow_, &window);
	glfwMakeContextCurrent(m_HorusWindow_);

	// Set callbacks
	glfwSetKeyCallback(m_HorusWindow_, KeyCallbacks);
	glfwSetScrollCallback(m_HorusWindow_, ScrollCallback);
	glfwSetMouseButtonCallback(m_HorusWindow_, MouseCallback);
	glfwSetCursorPosCallback(m_HorusWindow_, CursorPositionCallback);
	glfwSetWindowSizeCallback(m_HorusWindow_, WindowResizeCallback);
	glfwSetWindowCloseCallback(m_HorusWindow_, WindowCloseCallback);

	glfwSwapInterval(1);

	gladLoadGL();

	if (!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress)))
	{
		spdlog::error("Failed to initialize GLAD");
		return false;
	}

	glViewport(0, 0, m_WindowWidth_, m_WindowHeight_);

	m_ProgramId_ = ShaderManager.GetProgram("core/shaders/shader");
	GetShaderVariables(m_ProgramId_, "g_Texture", "inPosition", "inTexcoord");

	InitBuffers(m_WindowWidth_, m_WindowHeight_);

	spdlog::info("OpenGL version: {}.{}", GLVersion.major, GLVersion.minor);
	spdlog::info("OpenGL initialized successfully.");

	Console.AddLog(" [info] OpenGL initialized successfully.");
	Console.AddLog(" [info] OpenGL version: %d.%d", GLVersion.major, GLVersion.minor);

	glEnable(GL_DEPTH_TEST);

	/*glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);*/

	return true;
}

void HorusOpenGL::InitBuffers(int Width, int Height)
{
	HorusOpenGLManager& OpenGLManager = HorusOpenGLManager::GetInstance();

	m_WindowWidth_ = Width;
	m_WindowHeight_ = Height;

	//glEnable(GL_DEPTH_TEST);

	OpenGLManager.CreateVAO(0);
	OpenGLManager.BindVAO(0);

	OpenGLManager.CreateVBO(0, m_QuadVertexData_, sizeof(m_QuadVertexData_));
	OpenGLManager.CreateEBO(0, m_QuadIndexData_, sizeof(m_QuadIndexData_));

	OpenGLManager.SetVaoAttrib(0, 0, 3, 5 * sizeof(GLfloat), 0);
	OpenGLManager.SetVaoAttrib(0, 1, 2, 5 * sizeof(GLfloat), 3 * sizeof(GLfloat));
	OpenGLManager.SetVaoAttrib(0, 2, 2, 5 * sizeof(GLfloat), 3 * sizeof(GLfloat));

	OpenGLManager.UnbindVAO(0);
	OpenGLManager.UnbindVBO(0);
	OpenGLManager.UnbindEBO(0);

	// TODO : Move this to a opengl manager
	glGenTextures(1, &m_TextureBufferID_);
	glBindTexture(GL_TEXTURE_2D, m_TextureBufferID_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST = no smoothing
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // GL_NEAREST = no smoothing
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // GL_CLAMP_TO_EDGE = no wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // GL_CLAMP_TO_EDGE = no wrapping
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_WindowWidth_, m_WindowHeight_, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void HorusOpenGL::InitRender()
{
	HorusOpenGLManager& OpenGLManager = HorusOpenGLManager::GetInstance();

	glClearColor(0.298f, 0.384f, 0.419f, 1.0f); // green - blue pastel
	//glClearColor(0.678f, 0.847f, 0.902f, 1.0f); // sky blue
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	OpenGLManager.BindVAO(0);
	OpenGLManager.BindVBO(0);

	auto [texture_location, position_location, texcoord_location] = GetShaderVariables(m_ProgramId_, "g_Texture", "inPosition", "inTexcoord");

	glUseProgram(m_ProgramId_);

	glUniform1i(m_TextureLocation_, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_TextureBufferID_);

	glVertexAttribPointer(m_PositionLocation_, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
	glVertexAttribPointer(m_TexcoordLocation_, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	glEnableVertexAttribArray(m_PositionLocation_);
	glEnableVertexAttribArray(m_TexcoordLocation_);

	glDrawElements(GL_TRIANGLES, sizeof(m_QuadIndexData_) / sizeof(int), GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(m_PositionLocation_);
	glDisableVertexAttribArray(m_TexcoordLocation_);

	OpenGLManager.UnbindVAO(0);
	OpenGLManager.UnbindVBO(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}

void HorusOpenGL::PostRender()
{
	//ErrorManager();
	glfwSwapBuffers(m_HorusWindow_);
	glfwPollEvents();
}

void HorusOpenGL::QuitRender()
{
	HorusOpenGLManager& OpenGLManager = HorusOpenGLManager::GetInstance();

	spdlog::info("Unloading OpenGL..");

	OpenGLManager.DeleteAllVAOs();
	OpenGLManager.DeleteAllVBOs();
	OpenGLManager.DeleteAllEBOs();

	glfwDestroyWindow(m_HorusWindow_);
	glfwTerminate();

	spdlog::info("OpenGL unloaded successfully.");
}

std::tuple<GLuint, GLuint, GLuint> HorusOpenGL::GetShaderVariables(GLuint program, const char* texture_name, const char* position_name, const char* texcoord_name)
{
	m_TextureLocation_ = glGetUniformLocation(program, texture_name);
	m_PositionLocation_ = glGetAttribLocation(program, position_name);
	m_TexcoordLocation_ = glGetAttribLocation(program, texcoord_name);

	GLuint TextureLocation = m_TextureLocation_;
	GLuint PositionLocation = m_PositionLocation_;
	GLuint TexcoordLocation = m_TexcoordLocation_;

	return std::make_tuple(TextureLocation, PositionLocation, TexcoordLocation);
}

void HorusOpenGL::ErrorManager()
{
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		HorusConsole& Console = HorusConsole::GetInstance();

		spdlog::error("OpenGL error : {} at file {} line {}", error, __FILE__, __LINE__);
		Console.AddLog(" [error] OpenGL error : {} at file {} line {}", error, __FILE__, __LINE__);

		if (error == GL_INVALID_ENUM)
		{
			spdlog::error("GL_INVALID_ENUM");
			Console.AddLog(" [error] GL_INVALID_ENUM");
		}
		else if (error == GL_INVALID_VALUE)
		{
			spdlog::error("GL_INVALID_VALUE");
			Console.AddLog(" [error] GL_INVALID_VALUE");
		}
		else if (error == GL_INVALID_OPERATION)
		{
			spdlog::error("GL_INVALID_OPERATION");
			Console.AddLog(" [error] GL_INVALID_OPERATION");
		}
		else if (error == GL_INVALID_FRAMEBUFFER_OPERATION)
		{
			spdlog::error("GL_INVALID_FRAMEBUFFER_OPERATION");
			Console.AddLog(" [error] GL_INVALID_FRAMEBUFFER_OPERATION");
		}
		else if (error == GL_OUT_OF_MEMORY)
		{
			spdlog::error("GL_OUT_OF_MEMORY");
			Console.AddLog(" [error] GL_OUT_OF_MEMORY");
		}
		else if (error == GL_STACK_UNDERFLOW)
		{
			spdlog::error("GL_STACK_UNDERFLOW");
			Console.AddLog(" [error] GL_STACK_UNDERFLOW");
		}
		else if (error == GL_STACK_OVERFLOW)
		{
			spdlog::error("GL_STACK_OVERFLOW");
			Console.AddLog(" [error] GL_STACK_OVERFLOW");
		}
	}
}
