#pragma once

#define NOMINMAX
#include <glad/glad.h>
#include <string>
#include <map>
#include <glm/glm.hpp>

class HorusShaderManager
{
public:

	enum class ShaderType
	{
		Vertex,
		Fragment
	};

	HorusShaderManager();
	~HorusShaderManager() = default;

	// Singleton not used anymore
	//static HorusShaderManager& GetInstance()
	//{
	//	static HorusShaderManager Instance; // Instance unique
	//	return Instance;
	//}

	//HorusShaderManager(HorusShaderManager const&) = delete;
	//void operator=(HorusShaderManager const&) = delete;

	/*GLuint GetProgram(std::string const& ProgName);*/

	static bool Initialize();
	static void Uninitialize();


	GLuint GetCurrentProgram() const { return m_CurrentProgram_; }

	GLuint CreateShader(const std::string& ShaderFilepath);

	bool SendData(const std::string& UniformName, GLint Data) const;
	bool SendData(const std::string& UniformName, GLuint Data) const;
	bool SendData(const std::string& UniformName, GLfloat Data) const;

	bool SendData(const std::string& UniformName, GLfloat Data1, GLfloat Data2) const;
	bool SendData(const std::string& UniformName, GLfloat Data1, GLfloat Data2, GLfloat Data3) const;
	bool SendData(const std::string& UniformName, GLfloat Data1, GLfloat Data2, GLfloat Data3, GLfloat Data4) const;

	bool SendData(const std::string& UniformName, const glm::mat4& Data) const;
	bool SendData(const std::string& UniformName, const glm::mat3& Data) const;

	void Use();
	void Destroy();

private:

	GLuint CompileProgram(std::string const& ProgName);

	bool LinkProgram();
	bool CompileShaders(const std::string& ShaderFilepath, ShaderType Type);

	std::map<std::string, GLuint> m_Programs_;

	GLuint m_CurrentProgram_ = 0;
	inline static GLuint m_VertexShaderId_ = 0;
	inline static GLuint m_FragmentShaderId_ = 0;

};

