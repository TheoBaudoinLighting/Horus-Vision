
#include <glad/glad.h> 
#include "hrs_shader_manager.h"

#include <vector>
#include <string>
#include <stdexcept>
#include <fstream>
#include <iostream>

#include "spdlog/spdlog.h"
#include <hrs_console.h>

// Previous version of shader manager (deprecated, but keeped for reference)
static void LoadFile(std::string const& Name, std::vector<char>& Contents, bool Binary = false)
{
	std::ios_base::openmode OpenMode = std::ios::in;
	if (Binary) {
		OpenMode |= std::ios::binary;
	}

	std::ifstream File(Name, OpenMode);
	if (!File.is_open())
	{
		spdlog::error("failed to open file: {}", Name);
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
		glGetShaderiv(Shader, GL_INFO_LOG_LENGTH, &Len);
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
//GLuint HorusShaderManager::GetProgram(std::string const& ProgName)
//{
//	if (auto Iter = m_Programs_.find(ProgName); Iter != m_Programs_.end())
//	{
//		return Iter->second;
//	}
//	else
//	{
//		GLuint Program = CompileProgram(ProgName);
//		m_Programs_[ProgName] = Program;
//		return Program;
//	}
//}

// New version of shader manager
HorusShaderManager::HorusShaderManager()
{
	m_CurrentProgram_ = 0;
}
bool HorusShaderManager::Initialize()
{
	m_VertexShaderId_ = glCreateShader(GL_VERTEX_SHADER);

	if (m_VertexShaderId_ == 0)
	{
		spdlog::error("failed to create vertex shader");
		return false;
	}

	m_FragmentShaderId_ = glCreateShader(GL_FRAGMENT_SHADER);

	if (m_FragmentShaderId_ == 0)
	{
		spdlog::error("failed to create fragment shader");
		return false;
	}

	spdlog::info("Shaders initialized with success");

	return true;
}
void HorusShaderManager::Uninitialize()
{
	glDeleteShader(m_VertexShaderId_);
	glDeleteShader(m_FragmentShaderId_);
}

GLuint HorusShaderManager::CreateShader(std::string const& ShaderFilepath)
{
	HorusConsole& Console = HorusConsole::GetInstance();

	const std::string VertexShaderFilepath = ShaderFilepath + ".vert";
	const std::string FragmentShaderFilepath = ShaderFilepath + ".frag";

	auto LoadAndCompileShader = [](const std::string& Filename, GLenum ShaderType)
		{
			std::vector<GLchar> SourceCode;
			LoadFile(Filename, SourceCode);
			return CompileShader(ShaderType, SourceCode);
		};

	GLuint VertexShader = LoadAndCompileShader(VertexShaderFilepath, GL_VERTEX_SHADER);
	GLuint FragShader = LoadAndCompileShader(FragmentShaderFilepath, GL_FRAGMENT_SHADER);

	m_CurrentProgram_ = glCreateProgram();

	glAttachShader(m_CurrentProgram_, VertexShader);
	glAttachShader(m_CurrentProgram_, FragShader);
	glLinkProgram(m_CurrentProgram_);
	glDetachShader(m_CurrentProgram_, VertexShader);
	glDetachShader(m_CurrentProgram_, FragShader);

	GLint Result = GL_TRUE;
	glGetProgramiv(m_CurrentProgram_, GL_LINK_STATUS, &Result);

	if (Result == GL_FALSE)
	{
		GLint Length = 0;
		std::vector<char> Log;

		glGetProgramiv(m_CurrentProgram_, GL_INFO_LOG_LENGTH, &Length);

		Log.resize(Length);

		glGetProgramInfoLog(m_CurrentProgram_, Length, &Result, Log.data());

		glDeleteProgram(m_CurrentProgram_);

		try
		{
			throw std::runtime_error(std::string(Log.begin(), Log.end()));
		}
		catch (const std::exception& e)
		{
			spdlog::error("failed to link shader program: {}", e.what());
		}
	}

	spdlog::info("program compiled with success");
	spdlog::info("program name: {}", m_CurrentProgram_);

	Console.AddLog(" [success] Program compiled with success");
	Console.AddLog(" [success] Program name: %d ", m_CurrentProgram_);

	return m_CurrentProgram_;
}

bool HorusShaderManager::SendData(const std::string& UniformName, GLint Data) const
{
	auto ID = glGetUniformLocation(m_CurrentProgram_, UniformName.c_str());

	if (ID == -1)
	{
		spdlog::error("Shader variable {} not found or not used.", UniformName);
		return false;
	}

	glUniform1i(ID, Data);
	return  true;
}
bool HorusShaderManager::SendData(const std::string& UniformName, GLuint Data) const
{
	auto ID = glGetUniformLocation(m_CurrentProgram_, UniformName.c_str());

	if (ID == -1)
	{
		spdlog::error("Shader variable {} not found or not used.", UniformName);
		return false;
	}

	glUniform1ui(ID, Data);
	return  true;
}
bool HorusShaderManager::SendData(const std::string& UniformName, GLfloat Data) const
{
	auto ID = glGetUniformLocation(m_CurrentProgram_, UniformName.c_str());

	if (ID == -1)
	{
		spdlog::error("Shader variable {} not found or not used.", UniformName);
		return false;
	}

	glUniform1f(ID, Data);
	return  true;
}

bool HorusShaderManager::SendData(const std::string& UniformName, GLfloat Data1, GLfloat Data2) const
{
	auto ID = glGetUniformLocation(m_CurrentProgram_, UniformName.c_str());

	if (ID == -1)
	{
		spdlog::error("Shader variable {} not found or not used.", UniformName);
		return false;
	}

	glUniform2f(ID, Data1, Data2);
	return  true;
}
bool HorusShaderManager::SendData(const std::string& UniformName, GLfloat Data1, GLfloat Data2, GLfloat Data3) const
{
	auto ID = glGetUniformLocation(m_CurrentProgram_, UniformName.c_str());

	if (ID == -1)
	{
		spdlog::error("Shader variable {} not found or not used.", UniformName);
		return false;
	}

	glUniform3f(ID, Data1, Data2, Data3);
	return  true;
}
bool HorusShaderManager::SendData(const std::string& UniformName, GLfloat Data1, GLfloat Data2, GLfloat Data3, GLfloat Data4) const
{
	auto ID = glGetUniformLocation(m_CurrentProgram_, UniformName.c_str());

	if (ID == -1)
	{
		spdlog::error("Shader variable {} not found or not used.", UniformName);
		return false;
	}

	glUniform4f(ID, Data1, Data2, Data3, Data4);
	return  true;
}

bool HorusShaderManager::SendData(const std::string& UniformName, const glm::mat4& Data) const
{
	auto ID = glGetUniformLocation(m_CurrentProgram_, UniformName.c_str());

	if (ID == -1)
	{
		spdlog::error("Shader variable {} not found or not used.", UniformName);
		return false;
	}

	glUniformMatrix4fv(ID, 1, GL_FALSE, &Data[0][0]);
	return  true;
}
bool HorusShaderManager::SendData(const std::string& UniformName, const glm::mat3& Data) const
{
	auto ID = glGetUniformLocation(m_CurrentProgram_, UniformName.c_str());

	if (ID == -1)
	{
		spdlog::error("Shader variable {} not found or not used.", UniformName);
		return false;
	}

	glUniformMatrix3fv(ID, 1, GL_FALSE, &Data[0][0]);
	return  true;
}

void HorusShaderManager::Use()
{
	glUseProgram(m_CurrentProgram_);
}
void HorusShaderManager::Destroy()
{
	glDeleteProgram(m_CurrentProgram_);
}

bool HorusShaderManager::LinkProgram()
{
	glAttachShader(m_CurrentProgram_, m_VertexShaderId_);
	glAttachShader(m_CurrentProgram_, m_FragmentShaderId_);
	glLinkProgram(m_CurrentProgram_);
	glDetachShader(m_CurrentProgram_, m_VertexShaderId_);
	glDetachShader(m_CurrentProgram_, m_FragmentShaderId_);

	GLint Error = 0;
	glGetProgramiv(m_CurrentProgram_, GL_LINK_STATUS, &Error);

	if (Error != GL_TRUE)
	{
		GLchar ErrorMessage[1024];
		auto BufferSize = 1000;
		glGetProgramInfoLog(m_CurrentProgram_, BufferSize, &BufferSize, ErrorMessage);

		try
		{
			throw std::runtime_error(ErrorMessage);
		}
		catch (const std::exception& e)
		{
			spdlog::error("failed to link shader program: {}", e.what());
		}

		return false;
	}
	else
	{
		spdlog::info("Shader program linked with success");
		return true;
	}
}
bool HorusShaderManager::CompileShaders(const std::string& ShaderFilepath, ShaderType Type)
{
	HorusConsole& Console = HorusConsole::GetInstance();
	GLuint Shader = glCreateShader(Type == ShaderType::Vertex ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);

	GLint Len = static_cast<GLint>(ShaderFilepath.size());
	GLchar const* SourceArray = ShaderFilepath.data();

	glShaderSource(Shader, 1, &SourceArray, &Len);
	glCompileShader(Shader);

	GLint Result = GL_TRUE;
	glGetShaderiv(Shader, GL_COMPILE_STATUS, &Result);

	if (Result == GL_FALSE)
	{
		std::string Log(Len, '\0');
		glGetShaderiv(Shader, GL_INFO_LOG_LENGTH, &Len);
		glGetShaderInfoLog(Shader, Len, &Result, Log.data());
		glDeleteShader(Shader);

		try
		{
			throw std::runtime_error(Log);
		}
		catch (const std::exception& e)
		{
			spdlog::error("failed to compile shader: {}", e.what());
		}

		//throw std::runtime_error(Log);
	}

	spdlog::info("shader compiled with success");
	spdlog::info("shader type: {}", Shader);
	Console.AddLog(" [success] Shader compiled with success");
	Console.AddLog(" [success] Shader type: %d ", Shader);

	return true;
}