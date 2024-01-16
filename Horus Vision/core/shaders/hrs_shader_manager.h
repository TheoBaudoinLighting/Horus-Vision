#pragma once

#define NOMINMAX

#include <string>
#include <map>

class HorusShaderManager
{
public:

	static HorusShaderManager& GetInstance()
	{
		static HorusShaderManager Instance; // Instance unique
		return Instance;
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

	GLuint GetProgram(std::string const& ProgName);

private:

	GLuint CompileProgram(std::string const& ProgName);

	std::map<std::string, GLuint> m_Programs_;
};

