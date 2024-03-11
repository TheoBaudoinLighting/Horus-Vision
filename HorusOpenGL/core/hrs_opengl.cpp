#include "hrs_opengl.h" // glad.h
#include "hrs_config.h" // glfw3.h

#include <spdlog/spdlog.h>
#include <hrs_console.h>

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <oleidl.h>

#include <cstdio>
#include <cassert>
#include <chrono>

#include "hrs_object_manager.h"
#include "hrs_timer.h"

// Horus libraries
#include "hrs_dragdrop.h"
#include "hrs_imgui_core.h"
#include "hrs_om_camera.h"
#include "hrs_om_group_shapes.h"
#include "imgui_impl_glfw.h"

// Temporary
float RectangleVertices[] =
{
	// Coords    // texCoords
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f,

	 1.0f,  1.0f,  1.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f
};

void GLAPIENTRY MessageCallback(GLenum type, [[maybe_unused]] GLuint id, GLenum severity, [[maybe_unused]] GLsizei length, const GLchar* message, [[maybe_unused]] const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}

// Callbacks
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

// TODO : Move to a better place and make it more generic
static const char* NewExternalData = nullptr;
static size_t NewExternalDataLength = 0;
#define EXTERNAL_PAYLOAD_TYPE "IMAGE"

void DropCallback(GLFWwindow* window, int count, const char** paths)
{
	if (count > 0)
	{
		NewExternalData = strdup(paths[0]);
		NewExternalDataLength = strlen(NewExternalData) + 1;
	}
}

bool HorusOpenGL::Init(int width, int height, HorusWindowConfig* window)
{
	[[maybe_unused]] HorusOpenGLManager& OpenGLManager = HorusOpenGLManager::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();
	HorusTimerManager& Timer = HorusTimerManager::GetInstance();
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusOmCamera& CameraManager = HorusOmCamera::GetInstance();

	Timer.StartTimer("OpenGL");

	m_Logger_.Init();
	spdlog::info("Horus Engine initialization");
	Console.AddLog(" [info] Horus Engine initialization");

	__super::Init(width, height, window);
	m_ViewportWindowWidth_ = width;
	m_ViewportWindowHeight_ = height;

	if (!glfwInit())
	{
		spdlog::error("Failed to initialize GLFW");
		return false;
	}

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (glVertexAttrib2sv == NULL && glVertexAttrib2svARB != NULL) glVertexAttrib2sv = (PFNGLVERTEXATTRIB2SVPROC)glVertexAttrib2svARB;
	if (glVertexAttrib2sv == NULL && glVertexAttrib2svNV != NULL) glVertexAttrib2sv = (PFNGLVERTEXATTRIB2SVPROC)glVertexAttrib2svNV;

	glfwSetErrorCallback([]([[maybe_unused]] int Error, const char* Description) { spdlog::error("GLFW error: {}", Description); });

	m_HorusWindow_ = glfwCreateWindow(m_ViewportWindowWidth_, m_ViewportWindowHeight_, window->WindowTitle.c_str(), nullptr, nullptr);

	if (!m_HorusWindow_)
	{
		spdlog::error("Failed to create GLFW window");
		glfwTerminate();
		return false;
	}

	window->SetWindow(m_HorusWindow_);

	glfwSetWindowUserPointer(m_HorusWindow_, &window);
	glfwMakeContextCurrent(m_HorusWindow_);
	glfwMaximizeWindow(m_HorusWindow_);

	// Set callbacks
	glfwSetKeyCallback(m_HorusWindow_, KeyCallbacks);
	glfwSetScrollCallback(m_HorusWindow_, ScrollCallback);
	glfwSetMouseButtonCallback(m_HorusWindow_, MouseCallback);
	glfwSetCursorPosCallback(m_HorusWindow_, CursorPositionCallback);
	glfwSetWindowSizeCallback(m_HorusWindow_, WindowResizeCallback);
	glfwSetWindowCloseCallback(m_HorusWindow_, WindowCloseCallback);
	glfwSetDropCallback(m_HorusWindow_, DropCallback);

	// Enable V-Sync
	glfwSwapInterval(1);

	gladLoadGL();
	if (!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress)))
	{
		spdlog::error("Failed to initialize GLAD");
		return false;
	}

	glViewport(0, 0, m_ViewportWindowWidth_, m_ViewportWindowHeight_);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	m_DefaultShader_.CreateShader("core/shaders/shader");
	m_LightShader_.CreateShader("core/shaders/light");
	m_GridShader_.CreateShader("core/shaders/Grid");

	InitViewportRenderTextures(m_ViewportWindowWidth_, m_ViewportWindowHeight_);

	std::vector<VertexData> GridVertices;
	std::vector<GLuint> GridIndices;
	glm::vec4 GridColor = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
	glm::mat4 GridModel = glm::mat4(1.0f);
	GridModel = glm::translate(GridModel, glm::vec3(0.0f, 0.0f, 0.0f));

	ObjectManager.CreateGrid(30.f, 30.0f, 10.0f, "DefaultGrid");

	m_GridShader_.Use();
	glUniformMatrix4fv(glGetUniformLocation(m_GridShader_.GetProgram(), "Model"), 1, GL_FALSE, &GridModel[0][0]);
	glUniform4f(glGetUniformLocation(m_GridShader_.GetProgram(), "GridColor"), GridColor.x, GridColor.y, GridColor.z, GridColor.w);


	// Print OpenGL version and renderer
	spdlog::info("OpenGL version: {}.{}", GLVersion.major, GLVersion.minor);
	spdlog::info("OpenGL initialized successfully.");

	Console.AddLog(" [info] OpenGL initialized successfully.");
	Console.AddLog(" [info] OpenGL version: %d.%d", GLVersion.major, GLVersion.minor);

	auto TimerEnd = Timer.StopTimer("OpenGL");
	spdlog::info("OpenGL initialization took {} ms", TimerEnd);

	return true;
}
bool HorusOpenGL::InitViewportRenderTextures(int Width, int Height)
{
	m_ViewportWindowWidth_ = Width;
	m_ViewportWindowHeight_ = Height;

	// Texture specially for the Radeon View, to be able to render the image
	glGenTextures(1, &m_RadeonTextureBufferId_);
	glBindTexture(GL_TEXTURE_2D, m_RadeonTextureBufferId_);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_ViewportWindowWidth_, m_ViewportWindowHeight_, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST = no smoothing
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // GL_NEAREST = no smoothing
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // GL_CLAMP_TO_EDGE = no wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // GL_CLAMP_TO_EDGE = no wrapping
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Create and init framebuffer and texture for OpenGL viewport
	glGenFramebuffers(1, &m_FramebufferObject_);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferObject_);

	glGenTextures(1, &m_OpenGlTextureBufferId_);
	glBindTexture(GL_TEXTURE_2D, m_OpenGlTextureBufferId_);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 500, 500, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_OpenGlTextureBufferId_, 0);

	glGenRenderbuffers(1, &m_RenderbufferObject_);
	glBindRenderbuffer(GL_RENDERBUFFER, m_RenderbufferObject_);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 500, 500);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderbufferObject_);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		spdlog::error("Framebuffer is not complete!");
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	return true;
}

void HorusOpenGL::InitRender()
{
	// Handle external payloads
	const ImGuiPayload* Payload = ImGui::GetDragDropPayload();
	bool UserIsDraggingExternalData = Payload != nullptr && Payload->IsDataType(EXTERNAL_PAYLOAD_TYPE);

	if ((UserIsDraggingExternalData || NewExternalData != nullptr) && ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceExtern))
	{
		if (NewExternalData)
		{
			ImGui::SetDragDropPayload(EXTERNAL_PAYLOAD_TYPE, NewExternalData, NewExternalDataLength, ImGuiCond_Once);
			free((void*)NewExternalData);
			NewExternalData = nullptr;
			NewExternalDataLength = 0;
		}

		ImGui::Text("Dragging file '%s'", ImGui::GetDragDropPayload()->Data);
		ImGui::EndDragDropSource();
	}
}
void HorusOpenGL::Render()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusGroupShapeManager& GroupShapeManager = HorusGroupShapeManager::GetInstance();
	HorusOmCamera& CameraManager = HorusOmCamera::GetInstance();

	BindOpenGLViewportFrameBuffer();
	m_DefaultShader_.Use();

	const auto BackgroundColorNormalised = glm::vec4(92.f, 92.f, 92.f, 255.f) / 255.f;
	glClearColor(BackgroundColorNormalised.x, BackgroundColorNormalised.y, BackgroundColorNormalised.z, BackgroundColorNormalised.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw here
	// Primary viewport object 
	ObjectManager.DrawGrid(ObjectManager.GetActiveGridId(), m_GridShader_.GetProgram(), CameraManager.GetOpenGLCamera(CameraManager.GetActiveOpenGLCameraID()));

	// Secondary viewport object
	// Sub viewport object

	GroupShapeManager.DrawAllGroupShapeInOpenGl(m_DefaultShader_.GetProgram(), CameraManager.GetOpenGLCamera(CameraManager.GetActiveOpenGLCameraID()));

	glUseProgram(0);
	UnbindOpenGLViewportFrameBuffer();
}
void HorusOpenGL::PostRender()
{
	//ErrorManager();
	glfwSwapBuffers(m_HorusWindow_);
	glfwPollEvents();
}

void HorusOpenGL::QuitRender()
{
	glDeleteFramebuffers(1, &m_FramebufferObject_);
	glDeleteRenderbuffers(1, &m_RenderbufferObject_);
	glDeleteTextures(1, &m_OpenGlTextureBufferId_);
	glDeleteTextures(1, &m_RadeonTextureBufferId_);
	glDeleteProgram(m_DefaultShader_.GetProgram());
	glDeleteProgram(m_LightShader_.GetProgram());
	glDeleteProgram(m_GridShader_.GetProgram());

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

void HorusOpenGL::BindOpenGLViewportFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferObject_);
}
void HorusOpenGL::UnbindOpenGLViewportFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void HorusOpenGL::ResizeOpenGLViewportFrameBuffer(int Width, int Height)
{
	HorusOmCamera& CameraManager = HorusOmCamera::GetInstance();

	glBindTexture(GL_TEXTURE_2D, m_OpenGlTextureBufferId_);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_OpenGlTextureBufferId_, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, m_RenderbufferObject_);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Width, Height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderbufferObject_);
	glViewport(0, 0, Width, Height);

	glViewport(0, 0, Width, Height);

	if (Width > 0 && Height > 0)
	{
		float Aspect = (float)Width / (float)Height;
		CameraManager.SetCameraAspectRatio(CameraManager.GetActiveCameraID(), Aspect);
	}

	m_ViewportWindowWidth_ = Width;
	m_ViewportWindowHeight_ = Height;
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
