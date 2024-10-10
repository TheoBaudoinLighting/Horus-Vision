#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>

#include "hrs_vao.h"
#include "hrs_ebo.h"

#include "hrs_opengl_camera.h"

class HorusGrid
{
public:

	HorusGrid() = default;

	void Init(float Width, float Height, float Slices)
	{
		std::vector<VertexData> GridVertices;
		std::vector<GLuint> GridIndices;

		m_Vertices_ = GridVertices;
		m_Indices_ = GridIndices;

		GenerateGrid(Width, Height, Slices);

		m_Vao_.Bind();

		HorusVBO VBO(m_Vertices_);
		HorusEBO EBO(m_Indices_);

		m_Vao_.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(VertexData), (void*)0);

		m_Vao_.Unbind();
		VBO.Unbind();
		EBO.Unbind();
	}

	void Draw(GLuint Program, HorusOpenGLCamera& Camera)
	{
		glUseProgram(Program);
		m_Vao_.Bind();

		const glm::vec3 CameraPosition = Camera.GetPosition();

		// Camera
		glUniform3f(glGetUniformLocation(Program, "camPos"), CameraPosition.x, CameraPosition.y, CameraPosition.z);
		Camera.Matrix(Program, "CameraMatrix");

		// Draw grid
		glDrawElements(GL_LINES, m_Indices_.size(), GL_UNSIGNED_INT, 0);

		m_Vao_.Unbind();
	}

	void GenerateGrid(float Width, float Height, unsigned int Slices)
	{
		m_Vertices_.clear();
		m_Indices_.clear();

		const auto Size = 5;
		const auto Quadrants = 4;
		const auto VerticesPerLine = 2;

		for (unsigned int i = 0; i < Slices + 1; i++)
		{
			const auto X = (Width / Slices) * i - Width / 2;
			const auto Y = (Height / Slices) * i - Height / 2;

			// Horizontal lines
			m_Vertices_.push_back({ glm::vec3(-Width / 2, 0, Y), glm::vec3(0, 1, 0) });
			m_Vertices_.push_back({ glm::vec3(Width / 2, 0, Y), glm::vec3(0, 1, 0) });

			// Vertical lines
			m_Vertices_.push_back({ glm::vec3(X, 0, -Height / 2), glm::vec3(0, 1, 0) });
			m_Vertices_.push_back({ glm::vec3(X, 0, Height / 2), glm::vec3(0, 1, 0) });
		}

		for (auto i = 0; i < m_Vertices_.size(); i++)
		{
			m_Indices_.push_back(i);
		}
	}

private:

	std::vector<VertexData> m_Vertices_;
	std::vector<GLuint> m_Indices_;

	HorusVAO m_Vao_;
};