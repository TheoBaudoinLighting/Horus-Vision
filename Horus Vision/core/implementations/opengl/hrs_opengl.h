#pragma once

// Project includes
#include "hrs_opengl_manager.h" // glad.h
#include "hrs_shader_manager.h" // nothing
#include "hrs_context.h" // glfw3.h
#include "hrs_logger.h" // nothing

// Basic Includes
#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include <iostream>
#include <thread>
#include <memory>
#include <array>
#include <chrono>
#include <future>
#include <array>

class HorusOpenGL : public HorusContext
{
public:

	static HorusOpenGL& get_instance()
	{
		static HorusOpenGL instance; // Instance unique
		return instance;
	}

	HorusOpenGL(const HorusOpenGL&) = delete;
	void operator=(const HorusOpenGL&) = delete;

	bool init(int width, int height, HorusWindowConfig* window) override;

	void InitBuffers(int width, int height);

	void init_render() override;
	void post_render() override;
	void quit_render() override;

	std::tuple<GLuint, GLuint, GLuint> GetShaderVariables(GLuint program, const char* texture_name, const char* position_name, const char* texcoord_name);

	GLuint GetTextureBuffer() { return m_TextureBufferID_; }
	GLFWwindow* GetWindow() { return m_HorusWindow_; }

	void ErrorManager();

private:

	HorusOpenGL() {}
	
	HorusLogger m_logger_;

	// Window components
	GLFWwindow* m_HorusWindow_;
	int m_WindowWidth_, m_WindowHeight_;

	// Opengl components
	GLuint m_ProgramID_;
	GLuint m_VertexBufferID_;
	GLuint m_IndexBufferID_;

	// Buffers to render in ImGui and Radeon
	GLuint m_TextureBufferID_;

	// Shader variables
	GLuint m_TextureLocation_, m_PositionLocation_, m_TexcoordLocation_;

	GLuint quadIndexData[6] = { 0, 1, 3, 3, 1, 2 };
	GLfloat quadVertexData[20] = { -1, -1, 0.5, 0, 0, 1, -1, 0.5, 1, 0, 1, 1, 0.5, 1, 1, -1, 1, 0.5, 0, 1 };


	const char* glsl_version_ = "#version 460";
};