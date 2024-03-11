#pragma once

#include "hrs_ebo.h"
#include "hrs_opengl_camera.h"

#include "hrs_vao.h"


// Show an camera object in the viewport
class HorusCameraObjectView
{
public:

	HorusCameraObjectView() = default;

	void Init(std::vector<VertexData>& Vertices, std::vector<GLuint>& Indices) {

		m_Vertices_ = Vertices;
		m_Indices_ = Indices;

		m_Vao_.Bind();

		HorusVBO Vbo;
		HorusEBO Ebo;

		m_Vao_.LinkAttrib(Vbo, 0, 3, GL_FLOAT, sizeof(VertexData), (void*)0);
		m_Vao_.LinkAttrib(Vbo, 1, 3, GL_FLOAT, sizeof(VertexData), (void*)(3 * sizeof(float)));
		m_Vao_.LinkAttrib(Vbo, 2, 2, GL_FLOAT, sizeof(VertexData), (void*)(6 * sizeof(float)));
		m_Vao_.LinkAttrib(Vbo, 3, 3, GL_FLOAT, sizeof(VertexData), (void*)(8 * sizeof(float)));

		m_Vao_.Unbind();
		Vbo.Unbind();
		Ebo.Unbind();
	}

	void Draw(GLuint Program, HorusOpenGLCamera& Camera, int CameraIDToShow, glm::mat4 ModelMatrix, glm::vec3 Translation, glm::quat Rotation, glm::vec3 Scale)
	{
		glUseProgram(Program);
		m_Vao_.Bind();

		const glm::vec3 CameraPosition = Camera.GetPosition();

		glUniform3f(glGetUniformLocation(Program, "camPos"), CameraPosition.x, CameraPosition.y, CameraPosition.z);
		Camera.Matrix(Program, "camMatrix");

		glm::mat4 Trans = glm::mat4(1.0f);
		glm::mat4 Rot = glm::mat4(1.0f);
		glm::mat4 Scal = glm::mat4(1.0f);

		Trans = glm::translate(Trans, Translation);
		Rot = glm::mat4_cast(Rotation);
		Scal = glm::scale(Scal, Scale);

		glUniformMatrix4fv(glGetUniformLocation(Program, "translation"), 1, GL_FALSE, glm::value_ptr(Trans));
		glUniformMatrix4fv(glGetUniformLocation(Program, "rotation"), 1, GL_FALSE, glm::value_ptr(Rot));
		glUniformMatrix4fv(glGetUniformLocation(Program, "scale"), 1, GL_FALSE, glm::value_ptr(Scal));
		glUniformMatrix4fv(glGetUniformLocation(Program, "model"), 1, GL_FALSE, glm::value_ptr(ModelMatrix));

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawElements(GL_TRIANGLES, m_Indices_.size(), GL_UNSIGNED_INT, 0);
	}

private:

	std::vector<VertexData> m_Vertices_;
	std::vector<GLuint> m_Indices_;
	HorusVAO m_Vao_;
};