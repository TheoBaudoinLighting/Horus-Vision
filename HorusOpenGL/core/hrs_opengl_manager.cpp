#include "hrs_opengl_manager.h"
#include <hrs_console.h>
#include <ranges>

#include "spdlog/spdlog.h"

// New way
void HorusOpenGLManager::CreateBuffer(GLuint TotalVertices, bool HasEBO)
{
	glGenBuffers(1, &m_VertexVBO_);
	glGenBuffers(1, &m_ColorVBO_);
	glGenBuffers(1, &m_TextureVBO_);
	glGenBuffers(1, &m_NormalVBO_);
	glGenVertexArrays(1, &m_VAO_);

	if (HasEBO)
	{
		glGenBuffers(1, &m_EBO_);
		m_HasEBO_ = HasEBO;
	}

	m_VerticesCount_ = TotalVertices;
}

void HorusOpenGLManager::FillEBO(const GLuint* indices, GLsizeiptr size, FillType type)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, static_cast<GLenum>(type));
}
void HorusOpenGLManager::FillVBO(VBOTYPE Type, const void* Data, GLsizeiptr Size, FillType fillType)
{
	glBindVertexArray(m_VAO_);

	switch (Type)
	{
	case VBOTYPE::VERTEX:
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexVBO_);
		break;
	case VBOTYPE::COLOR:
		glBindBuffer(GL_ARRAY_BUFFER, m_ColorVBO_);
		break;
	case VBOTYPE::TEXTURE:
		glBindBuffer(GL_ARRAY_BUFFER, m_TextureVBO_);
		break;
	case VBOTYPE::NORMAL:
		glBindBuffer(GL_ARRAY_BUFFER, m_NormalVBO_);

		break;
	default:
		break;
	}

	glBufferData(GL_ARRAY_BUFFER, Size, Data, static_cast<GLenum>(fillType));
	glBindVertexArray(0);
}
void HorusOpenGLManager::AppendVBO(VBOTYPE type, const void* dta, GLsizeiptr size, GLuint offset)
{
	glBindVertexArray(m_VAO_);

	switch (type)
	{
	case VBOTYPE::VERTEX:
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexVBO_);
		break;
	case VBOTYPE::COLOR:
		glBindBuffer(GL_ARRAY_BUFFER, m_ColorVBO_);
		break;
	case VBOTYPE::TEXTURE:
		glBindBuffer(GL_ARRAY_BUFFER, m_TextureVBO_);
		break;
	case VBOTYPE::NORMAL:
		glBindBuffer(GL_ARRAY_BUFFER, m_NormalVBO_);
		break;
	default:
		break;
	}

	glBufferSubData(GL_ARRAY_BUFFER, offset, size, dta);
	glBindVertexArray(0);
}

void HorusOpenGLManager::LinkEBO()
{
	glBindVertexArray(m_VAO_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO_);
	glBindVertexArray(0);
}
//void HorusOpenGLManager::LinkVBO(const HorusShaderManager& Shader, const std::string& attribute, VBOTYPE type, ComponentType componentType, DataType dataType)
//{
//	auto ShaderManager = Shader.GetProgram();
//
//	auto ID = glGetAttribLocation(ShaderManager, attribute.c_str());
//
//	glBindVertexArray(m_VAO_);
//
//	if (type == VBOTYPE::VERTEX)
//	{
//		glBindBuffer(GL_ARRAY_BUFFER, m_VertexVBO_);
//	}
//	else if (type == VBOTYPE::COLOR)
//	{
//		glBindBuffer(GL_ARRAY_BUFFER, m_ColorVBO_);
//	}
//	else if (type == VBOTYPE::TEXTURE)
//	{
//		glBindBuffer(GL_ARRAY_BUFFER, m_TextureVBO_);
//	}
//	else if (type == VBOTYPE::NORMAL)
//	{
//		glBindBuffer(GL_ARRAY_BUFFER, m_NormalVBO_);
//	}
//
//	glVertexAttribPointer(ID, static_cast<GLint>(componentType), static_cast<GLenum>(dataType), GL_FALSE, 0, nullptr);
//	glEnableVertexAttribArray(ID);
//	glBindVertexArray(0);
//}
void HorusOpenGLManager::Render(DrawType type)
{
	glBindVertexArray(m_VAO_);

	if (m_HasEBO_)
	{
		glDrawElements(static_cast<GLenum>(type), m_VerticesCount_, GL_UNSIGNED_INT, nullptr);
	}
	else
	{
		glDrawArrays(static_cast<GLenum>(type), 0, m_VerticesCount_);
	}

	glBindVertexArray(0);
}
void HorusOpenGLManager::Clear()
{
	glDeleteBuffers(1, &m_VertexVBO_);
	glDeleteBuffers(1, &m_ColorVBO_);
	glDeleteBuffers(1, &m_TextureVBO_);
	glDeleteBuffers(1, &m_NormalVBO_);
	glDeleteVertexArrays(1, &m_VAO_);
	glDeleteBuffers(1, &m_EBO_);

	m_VerticesCount_ = 0;
	m_HasEBO_ = false;
}

// Old way
HorusVAO& HorusOpenGLManager::CreateVAO(int ID)
{
	if (m_VAOs_.contains(ID))
	{
		spdlog::error("VAO with ID {} already exists", ID);

		return m_VAOs_[ID];
	}

	HorusVAO VAO;
	m_VAOs_[ID] = VAO;

	spdlog::info("Created VAO with ID {}", ID);

	return m_VAOs_[ID];
}
HorusVBO& HorusOpenGLManager::CreateVBO(int ID, std::vector<VertexData>& Vertices)
{
	if (m_VBOs_.contains(ID))
	{
		spdlog::error("VBO with ID {} already exists", ID);

		return m_VBOs_[ID];
	}

	HorusVBO VBO(Vertices);
	m_VBOs_[ID] = VBO;

	spdlog::info("Created VBO with ID {}", ID);

	return m_VBOs_[ID];
}
HorusEBO& HorusOpenGLManager::CreateEBO(int ID, std::vector<unsigned int>& Indices)
{
	if (m_EBOs_.contains(ID))
	{
		spdlog::error("EBO with ID {} already exists", ID);

		return m_EBOs_[ID];
	}

	HorusEBO EBO(Indices);
	m_EBOs_[ID] = EBO;

	spdlog::info("Created EBO with ID {}", ID);

	return m_EBOs_[ID];
}

void HorusOpenGLManager::BindVAO(int ID)
{
	if (!m_VAOs_.contains(ID))
	{
		spdlog::error("VAO with ID {} does not exist", ID);
		return;
	}

	m_VAOs_[ID].Bind();
}
void HorusOpenGLManager::BindVBO(int ID)
{
	if (!m_VBOs_.contains(ID))
	{
		spdlog::error("VBO with ID {} does not exist", ID);
		return;
	}

	m_VBOs_[ID].Bind();
}
void HorusOpenGLManager::BindEBO(int ID)
{
	if (!m_EBOs_.contains(ID))
	{
		spdlog::error("EBO with ID {} does not exist", ID);
		return;
	}

	m_EBOs_[ID].Bind();
}

void HorusOpenGLManager::UnbindVAO(int ID)
{
	if (!m_VAOs_.contains(ID))
	{
		spdlog::error("VAO with ID {} does not exist", ID);
		return;
	}

	m_VAOs_[ID].Unbind();
}
void HorusOpenGLManager::UnbindVBO(int ID)
{
	if (!m_VBOs_.contains(ID))
	{
		spdlog::error("VBO with ID {} does not exist", ID);
		return;
	}

	m_VBOs_[ID].Unbind();
}
void HorusOpenGLManager::UnbindEBO(int ID)
{
	if (!m_EBOs_.contains(ID))
	{
		spdlog::error("EBO with ID {} does not exist", ID);
		return;
	}

	m_EBOs_[ID].Unbind();
}

void HorusOpenGLManager::DeleteVAO(int ID)
{
	if (auto It = m_VAOs_.find(ID); It != m_VAOs_.end())
	{
		It->second.Delete();
		m_VAOs_.erase(It);
	}

	spdlog::info(" [info] Deleted VAO with ID {}", ID);
}
void HorusOpenGLManager::DeleteVBO(int ID)
{
	if (auto It = m_VBOs_.find(ID); It != m_VBOs_.end())
	{
		It->second.Delete();
		m_VBOs_.erase(It);
	}

	spdlog::info("Deleted VBO with ID {}", ID);
}
void HorusOpenGLManager::DeleteEBO(int ID)
{
	if (auto It = m_EBOs_.find(ID); It != m_EBOs_.end())
	{
		It->second.Delete();
		m_EBOs_.erase(It);
	}

	spdlog::info("Deleted EBO with ID {}", ID);
}
void HorusOpenGLManager::DeleteAllVAOs()
{
	for (auto& Val : m_VAOs_ | std::views::values)
	{
		Val.Delete();
	}

	m_VAOs_.clear();

	spdlog::info("Deleted all VAOs");
}
void HorusOpenGLManager::DeleteAllVBOs()
{
	for (auto& Val : m_VBOs_ | std::views::values)
	{
		Val.Delete();
	}

	m_VBOs_.clear();

	spdlog::info("Deleted all VBOs");
}
void HorusOpenGLManager::DeleteAllEBOs()
{
	for (auto& Val : m_EBOs_ | std::views::values)
	{
		Val.Delete();
	}

	m_EBOs_.clear();

	spdlog::info("Deleted all EBOs");
}

void HorusOpenGLManager::SetVaoAttrib(int ID, int index, int size, int stride, int offset)
{
	if (!m_VAOs_.contains(ID))
	{
		spdlog::error("VAO with ID {} does not exist", ID);

		return;
	}

	spdlog::info("Set VAO attribute with ID {}", ID);

	m_VAOs_[ID].LinkAttrib(m_VBOs_[ID], index, size, GL_FLOAT, stride, reinterpret_cast<void*>(offset));
}
