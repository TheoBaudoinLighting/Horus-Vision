#pragma once

// Project includes
#include "hrs_opengl_manager.h" // glad.h
#include "hrs_shader_manager.h" // nothing
#include "hrs_logger.h" // nothing
#include "hrs_context.h" // glfw3.h

// Basic Includes
#include <future>

class HorusOpenGL : public HorusContext
{
public:

	static HorusOpenGL& GetInstance()
	{
		static HorusOpenGL Instance; // Instance unique
		return Instance;
	}

	HorusOpenGL(const HorusOpenGL&) = delete;
	void operator=(const HorusOpenGL&) = delete;

	bool Init(int width, int height, HorusWindowConfig* window) override;

	void InitBuffers(int Width, int Height);

	void Render();

	void InitRender() override;
	void PostRender() override;
	void QuitRender() override;

	std::tuple<GLuint, GLuint, GLuint> GetShaderVariables(GLuint program, const char* texture_name, const char* position_name, const char* texcoord_name);

	// Getters
	GLuint GetRadeonTextureBuffer() { return m_RadeonTextureBufferId_; }
	GLuint GetOpenGlTextureBuffer() { return m_OpenGlTextureBufferId_; }
	GLFWwindow* GetWindow() { return m_HorusWindow_; }

	void ErrorManager();

private:

	HorusOpenGL(): m_HorusWindow_(nullptr), m_WindowWidth_(0), m_WindowHeight_(0), m_ProgramId_(0),
	               m_VertexBufferId_(0), m_IndexBufferId_(0),
	               m_RadeonTextureBufferId_(0), m_OpenGlTextureBufferId_(0), m_FramebufferObject_(0),
	               m_TextureLocation_(0),
	               m_PositionLocation_(0),
	               m_TexcoordLocation_(0)
	{
	}

	HorusLogger m_Logger_;

	// Window components
	GLFWwindow* m_HorusWindow_;
	int m_WindowWidth_, m_WindowHeight_;

	// Opengl components
	GLuint m_ProgramId_;
	GLuint m_VertexBufferId_;
	GLuint m_IndexBufferId_;
	GLuint m_FramebufferObject_;

	// Buffers to render in ImGui and Radeon
	GLuint m_RadeonTextureBufferId_;
	GLuint m_OpenGlTextureBufferId_;

	// Shader
	HorusShaderManager m_DefaultShader_;
	HorusShaderManager m_LightShader_;

	// Shader variables
	GLuint m_TextureLocation_, m_PositionLocation_, m_TexcoordLocation_;

	GLuint m_QuadIndexData_[6] = { 0, 1, 3, 3, 1, 2 };
	GLfloat m_QuadVertexData_[20] = { -1, -1, 0.5, 0, 0, 1, -1, 0.5, 1, 0, 1, 1, 0.5, 1, 1, -1, 1, 0.5, 0, 1 };

	const char* m_GlslVersion_ = "#version 460";
};