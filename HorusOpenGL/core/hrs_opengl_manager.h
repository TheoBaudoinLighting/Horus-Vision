#pragma once

// External includes
#include <glad/glad.h>
#include "hrs_shader_manager.h"

// Project includes
#include "hrs_vao.h" // glad.h
#include "hrs_vbo.h" // glad.h
#include "hrs_ebo.h" // glad.h

// Basic includes
#include <map>
#include <string>

class HorusOpenGLManager
{
public:

	enum class VBOTYPE
	{
		VERTEX,
		TEXTURE,
		NORMAL,
		COLOR,
		INDICES
	};

	enum class ComponentType
	{
		XY = 2,
		XYZ = 3,
		RGB = 3,
		RGBA = 4,
		UV = 2
	};

	enum class FillType
	{
		STATIC = GL_STATIC_DRAW,
		DYNAMIC = GL_DYNAMIC_DRAW
	};

	enum class DataType
	{
		INT = GL_INT,
		FLOAT = GL_FLOAT,
		UINT = GL_UNSIGNED_INT
	};

	enum class DrawType
	{
		POINTS = GL_POINTS,
		LINES = GL_LINES,
		LINE_LOOP = GL_LINE_LOOP,
		LINE_STRIP = GL_LINE_STRIP,
		TRIANGLES = GL_TRIANGLES,
		TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
		TRIANGLE_FAN = GL_TRIANGLE_FAN
	};

	static HorusOpenGLManager& GetInstance()
	{
		static HorusOpenGLManager Instance;
		return Instance;
	}

	HorusOpenGLManager(HorusOpenGLManager const&) = delete;
	void operator=(HorusOpenGLManager const&) = delete;

	void CreateBuffer(GLuint TotalVertices, bool HasEBO = false);

	void FillEBO(const GLuint* indices, GLsizeiptr size, FillType type = FillType::STATIC);
	void FillVBO(VBOTYPE Type, const void* Data, GLsizeiptr Size, FillType fillType = FillType::STATIC);
	void AppendVBO(VBOTYPE type, const void* dta, GLsizeiptr size, GLuint offset = 0);
	void LinkEBO();
	void LinkVBO(const HorusShaderManager& Shader, const std::string& attribute, VBOTYPE type, ComponentType componentType, DataType dataType);
	void Render(DrawType type);
	void Clear();

	HorusVAO& CreateVAO(int ID);
	HorusVBO& CreateVBO(int ID, GLfloat* vertices, GLsizeiptr size);
	HorusEBO& CreateEBO(int ID, GLuint* indices, GLuint size);

	void BindVAO(int ID);
	void BindVBO(int ID);
	void BindEBO(int ID);

	void UnbindVAO(int ID);
	void UnbindVBO(int ID);
	void UnbindEBO(int ID);

	void DeleteVAO(int ID);
	void DeleteVBO(int ID);
	void DeleteEBO(int ID);

	void DeleteAllVAOs();
	void DeleteAllVBOs();
	void DeleteAllEBOs();

	void SetVaoAttrib(int ID, int index, int size, int stride, int offset);

private:

	HorusOpenGLManager(): m_HasEBO_(false), m_VAO_(0), m_EBO_(0), m_VertexVBO_(0), m_ColorVBO_(0), m_TextureVBO_(0),
	                      m_NormalVBO_(0),
	                      m_VerticesCount_(0)
	{
	}

	// Manager components
	std::map<int, HorusVAO> m_VAOs_;
	std::map<int, HorusVBO> m_VBOs_;
	std::map<int, HorusEBO> m_EBOs_;

	bool m_HasEBO_;

	GLuint m_VAO_;
	GLuint m_EBO_;

	GLuint m_VertexVBO_;
	GLuint m_ColorVBO_;
	GLuint m_TextureVBO_;
	GLuint m_NormalVBO_;

	GLuint m_VerticesCount_;

};
