
#include <glad/glad.h> 
#include "hrs_shader_manager.h"

#include <vector>
#include <string>
#include <stdexcept>
#include <fstream>
#include <iostream>

#include "spdlog/spdlog.h"
#include <hrs_console.h>

static void LoadFile(std::string const& Name, std::vector<char>& Contents, bool Binary = false)
{
	std::ios_base::openmode OpenMode = std::ios::in;
	if (Binary) {
		OpenMode |= std::ios::binary;
	}

	std::ifstream File(Name, OpenMode);
	if (!File.is_open())
	{
		exit(-1);
	}

	Contents.assign(std::istreambuf_iterator<char>(File), std::istreambuf_iterator<char>());
}

static GLuint CompileShader(GLenum Type, std::vector<GLchar> const& Source)
{
	HorusConsole& Console = HorusConsole::GetInstance();

	GLuint Shader = glCreateShader(Type);

	GLint Len = static_cast<GLint>(Source.size());
	GLchar const* SourceArray = Source.data();

	glShaderSource(Shader, 1, &SourceArray, &Len);
	glCompileShader(Shader);

	GLint Result = GL_TRUE;
	glGetShaderiv(Shader, GL_COMPILE_STATUS, &Result);

	if (Result == GL_FALSE)
	{
		std::string Log(Len, '\0');
		glGetShaderInfoLog(Shader, Len, &Result, Log.data());
		glDeleteShader(Shader);
		throw std::runtime_error(Log);
	}

	spdlog::info("shader compiled with success");
	spdlog::info("shader type: {}", Shader);

	Console.AddLog(" [success] Shader compiled with success");
	Console.AddLog(" [success] Shader type: %d ", Shader);

	return Shader;
}

GLuint HorusShaderManager::CompileProgram(std::string const& ProgName)
{
	HorusConsole& Console = HorusConsole::GetInstance();

	std::string VsName = ProgName + ".vert";
	std::string FsName = ProgName + ".frag";

	auto LoadAndCompileShader = [](const std::string& Filename, GLenum ShaderType)
		{
			std::vector<GLchar> SourceCode;
			LoadFile(Filename, SourceCode);
			return CompileShader(ShaderType, SourceCode);
		};

	GLuint VertexShader = LoadAndCompileShader(VsName, GL_VERTEX_SHADER);
	GLuint FragShader = LoadAndCompileShader(FsName, GL_FRAGMENT_SHADER);

	GLuint Program = glCreateProgram();

	glAttachShader(Program, VertexShader);
	glAttachShader(Program, FragShader);

	glDeleteShader(VertexShader);
	glDeleteShader(FragShader);

	glLinkProgram(Program);

	GLint Result = GL_TRUE;
	glGetProgramiv(Program, GL_LINK_STATUS, &Result);

	if (Result == GL_FALSE)
	{
		GLint Length = 0;
		std::vector<char> Log;

		glGetProgramiv(Program, GL_INFO_LOG_LENGTH, &Length);

		Log.resize(Length);

		glGetProgramInfoLog(Program, Length, &Result, Log.data());

		glDeleteProgram(Program);

		throw std::runtime_error(std::string(Log.begin(), Log.end()));
	}

	spdlog::info("program compiled with success");
	spdlog::info("program name: {}", Program);

	Console.AddLog(" [success] Program compiled with success");
	Console.AddLog(" [success] Program name: %d ", Program);

	return Program;
}

GLuint HorusShaderManager::GetProgram(std::string const& ProgName)
{
	if (auto Iter = m_Programs_.find(ProgName); Iter != m_Programs_.end())
	{
		return Iter->second;
	}
	else
	{
		GLuint Program = CompileProgram(ProgName);
		m_Programs_[ProgName] = Program;
		return Program;
	}
}