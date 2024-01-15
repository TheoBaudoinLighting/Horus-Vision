#pragma once

#define NOMINMAX

#include <string>
#include <map>

class HorusShaderManager
{
public:

	static HorusShaderManager& get_instance()
	{
		static HorusShaderManager instance; // Instance unique
		return instance;
	}

	HorusShaderManager(HorusShaderManager const&) = delete;
	void operator=(HorusShaderManager const&) = delete;

	HorusShaderManager() {}

	~HorusShaderManager()
	{
		/*for (auto shader = programs_.cbegin(); shader != programs_.cend(); ++shader)
		{
			glDeleteProgram(shader->second);
		}*/
	}

	GLuint get_program(std::string const& prog_name);

private:

	GLuint compile_program(std::string const& prog_name);

	std::map<std::string, GLuint> programs_;
};

