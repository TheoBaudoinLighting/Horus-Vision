#pragma once

#include <glad/glad.h>

#include "hrs_vbo.h"
#include "spdlog/spdlog.h"

class HorusVAO
{
public:

	GLuint ID;

	HorusVAO()
	{
		if (!glGenVertexArrays)
		{
			printf("Error: glGenVertexArrays is NULL\n");
			return;
		}
		glGenVertexArrays(1, &ID);
	}

	static void LinkAttrib(const HorusVBO& VBO,GLuint Layout, GLuint NumComponents, GLenum Type, GLsizei Stride, void* Offset)
	{
		VBO.Bind();
		glEnableVertexAttribArray(Layout);
		glVertexAttribPointer(Layout, NumComponents, Type, GL_FALSE, Stride, Offset);
		VBO.Unbind();
	}

	void GetId() { spdlog::info("VAO ID: {}", ID); }

	void Bind() const { glBindVertexArray(ID); }
	static void Unbind() { glBindVertexArray(0); }
	void Delete() const { glDeleteVertexArrays(1, &ID); }
};
