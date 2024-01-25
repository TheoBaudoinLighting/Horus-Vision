#include "hrs_grid.h"

#include "hrs_opengl_manager.h"
#include "hrs_shader_manager.h"

void HorusGrid::Grid()
{
	HorusOpenGLManager& OpenGLManager = HorusOpenGLManager::GetInstance();

	const auto Size = 10;
	const auto Quadrants = 4;
	const auto VerticesPerLines = 2;

	const auto BytePerLinesVertex = VerticesPerLines * static_cast<GLuint>(HorusOpenGLManager::ComponentType::XYZ) * sizeof(GLint);
	const auto BytePerLinesColor = VerticesPerLines * static_cast<GLuint>(HorusOpenGLManager::ComponentType::RGBA) * sizeof(GLfloat);
	const auto ByteVertexVBO = (Size + 1) * BytePerLinesVertex * Quadrants;
	const auto ByteColorVBO = (Size + 1) * BytePerLinesColor * Quadrants;

	OpenGLManager.CreateBuffer((Size + 1) * Quadrants * VerticesPerLines, false);
	OpenGLManager.FillVBO(HorusOpenGLManager::VBOTYPE::VERTEX, (GLint*)nullptr, ByteVertexVBO, HorusOpenGLManager::FillType::DYNAMIC);
	OpenGLManager.FillVBO(HorusOpenGLManager::VBOTYPE::COLOR, (GLfloat*)nullptr, ByteColorVBO, HorusOpenGLManager::FillType::DYNAMIC);

	auto OffsetColor = 0U;
	auto OffsetVertex = 0U;

	// Negative Vertical Lines
	for (auto i = 0; i <= Size; ++i)
	{
		GLint Vertex[] = { -Size + i, 0, Size, -Size + i, 0, -Size };

		GLfloat Color[] = { 0.5f, 0.5f, 0.5f, 1.0f,
							0.5f, 0.5f, 0.5f, 1.0f };

		OpenGLManager.AppendVBO(HorusOpenGLManager::VBOTYPE::VERTEX, Vertex, sizeof(Vertex), OffsetVertex);
		OpenGLManager.AppendVBO(HorusOpenGLManager::VBOTYPE::COLOR, Color, sizeof(Color), OffsetColor);

		OffsetVertex += BytePerLinesVertex;
		OffsetColor += BytePerLinesColor;
	}

	// Positive Vertical Lines
	for (auto i = 1; i <= Size; ++i)
	{
		GLint Vertex[] = { 0 + i, 0, Size, 0 + i, 0, -Size };

		GLfloat Color[] = { 0.5f, 0.5f, 0.5f, 1.0f,
							0.5f, 0.5f, 0.5f, 1.0f };

		OpenGLManager.AppendVBO(HorusOpenGLManager::VBOTYPE::VERTEX, Vertex, sizeof(Vertex), OffsetVertex);
		OpenGLManager.AppendVBO(HorusOpenGLManager::VBOTYPE::COLOR, Color, sizeof(Color), OffsetColor);

		OffsetVertex += BytePerLinesVertex;
		OffsetColor += BytePerLinesColor;
	}

	// Negative Horizontal Lines
	for (auto i = 0; i <= Size; ++i)
	{
		GLint Vertex[] = { -Size, 0, -Size + i, Size, 0, -Size + i };

		GLfloat Color[] = { 0.5f, 0.5f, 0.5f, 1.0f,
							0.5f, 0.5f, 0.5f, 1.0f };

		OpenGLManager.AppendVBO(HorusOpenGLManager::VBOTYPE::VERTEX, Vertex, sizeof(Vertex), OffsetVertex);
		OpenGLManager.AppendVBO(HorusOpenGLManager::VBOTYPE::COLOR, Color, sizeof(Color), OffsetColor);

		OffsetVertex += BytePerLinesVertex;
		OffsetColor += BytePerLinesColor;
	}

	// Positive Horizontal Lines
	for (auto i = 1; i <= Size; ++i)
	{
		GLint Vertex[] = { -Size, 0, 0 + i, Size, 0, 0 + i };

		GLfloat Color[] = { 0.5f, 0.5f, 0.5f, 1.0f,
							0.5f, 0.5f, 0.5f, 1.0f };

		OpenGLManager.AppendVBO(HorusOpenGLManager::VBOTYPE::VERTEX, Vertex, sizeof(Vertex), OffsetVertex);
		OpenGLManager.AppendVBO(HorusOpenGLManager::VBOTYPE::COLOR, Color, sizeof(Color), OffsetColor);

		OffsetVertex += BytePerLinesVertex;
		OffsetColor += BytePerLinesColor;
	}

	m_Position_ = glm::vec3(0.0f, 0.0f, 0.0f);
	m_Rotation_ = glm::vec3(0.0f, 0.0f, 0.0f);
}

void HorusGrid::Update()
{
}

void HorusGrid::Render(const HorusShaderManager& Shader)
{
	HorusOpenGLManager& OpenGLManager = HorusOpenGLManager::GetInstance();

	//Shader.SendData("isTextured", false);
	Shader.SendData("model", m_Model_);

	OpenGLManager.LinkVBO(Shader, "vertexIn", HorusOpenGLManager::VBOTYPE::VERTEX, HorusOpenGLManager::ComponentType::XYZ, HorusOpenGLManager::DataType::INT);
	OpenGLManager.LinkVBO(Shader, "colorIn", HorusOpenGLManager::VBOTYPE::COLOR, HorusOpenGLManager::ComponentType::RGBA, HorusOpenGLManager::DataType::FLOAT);

	OpenGLManager.Render(HorusOpenGLManager::DrawType::LINES);
}
