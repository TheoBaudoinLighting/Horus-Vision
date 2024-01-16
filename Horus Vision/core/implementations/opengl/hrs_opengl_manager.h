#pragma once

// External includes
#include <glad/glad.h>

// Project includes
#include "hrs_vao.h" // glad.h
#include "hrs_vbo.h" // glad.h
#include "hrs_ebo.h" // glad.h

// Basic includes
#include <map>

class HorusOpenGLManager
{
public:

	static HorusOpenGLManager& GetInstance()
	{
		static HorusOpenGLManager Instance; 
		return Instance;
	}

	HorusOpenGLManager(HorusOpenGLManager const&) = delete;
	void operator=(HorusOpenGLManager const&) = delete;

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

	HorusOpenGLManager() {}

	// Manager components
	std::map<int, HorusVAO> m_VAOs_;
	std::map<int, HorusVBO> m_VBOs_;
	std::map<int, HorusEBO> m_EBOs_;

};
