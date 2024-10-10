#pragma once

#include <vector>
#include <glad/glad.h>

#include "hrs_vertex_data.h"

class HorusVBO
{
public:

	GLuint ID;

	explicit HorusVBO(const std::vector<VertexData>& Vertices)
	{
		glGenBuffers(1, &ID);
		glBindBuffer(GL_ARRAY_BUFFER, ID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * Vertices.size(), Vertices.data(), GL_STATIC_DRAW);
	}

	~HorusVBO()
	{
		glDeleteBuffers(1, &ID);
	}

	void Bind() const { glBindBuffer(GL_ARRAY_BUFFER, ID); }
	static void Unbind() { glBindBuffer(GL_ARRAY_BUFFER, 0); }
	void Delete() const { glDeleteBuffers(1, &ID); }
};
