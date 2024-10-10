#pragma once

#include <glad/glad.h>

#include <vector>

class HorusEBO
{
public:

	GLuint ID;

	HorusEBO(std::vector<GLuint> Indices)
	{
		glGenBuffers(1, &ID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(GLuint), Indices.data(), GL_STATIC_DRAW);
	}

	void Bind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID); }
	void Unbind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
	void Delete() { glDeleteBuffers(1, &ID); }
};




