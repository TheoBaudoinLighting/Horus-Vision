#pragma once

#define NOMINMAX
#include <glad/glad.h>

#include <string>
#include <map>

class HorusShaderManager
{
public:

	HorusShaderManager() {}

	~HorusShaderManager()
	{
		for (auto shader = programs_.cbegin(); shader != programs_.cend(); ++shader)
		{
			glDeleteProgram(shader->second);
		}
	}

	GLuint get_program(std::string const& prog_name);

private:

	GLuint compile_program(std::string const& prog_name);

	HorusShaderManager(HorusShaderManager const&);
	HorusShaderManager& operator=(HorusShaderManager const&);

	std::map<std::string, GLuint> programs_;
};

