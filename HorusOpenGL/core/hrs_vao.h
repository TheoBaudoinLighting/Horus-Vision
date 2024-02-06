#pragma once

#include <glad/glad.h>

#include "hrs_vbo.h"

class HorusVAO
{
public:

	GLuint ID;

	HorusVAO() : ID(0) {}

	void LinkAttrib(HorusVBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizei stride, void* offset)
	{
		VBO.Bind();
		glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
		glEnableVertexAttribArray(layout);
		VBO.Unbind();
	}

	void Init()
	{
		if (ID == 0)
		{
			glGenVertexArrays(1, &ID);
		}
	}

	void Bind()
	{
		glBindVertexArray(ID);
	}

	void Unbind()
	{
		glBindVertexArray(0);
	}

	void Delete()
	{
		glDeleteVertexArrays(1, &ID);
	}
};
