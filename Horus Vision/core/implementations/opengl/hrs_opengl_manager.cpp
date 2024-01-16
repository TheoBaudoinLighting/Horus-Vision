#include "hrs_opengl_manager.h"
#include <hrs_console.h>
#include <ranges>

#include "spdlog/spdlog.h"

HorusVAO& HorusOpenGLManager::CreateVAO(int ID)
{
	if (m_VAOs_.contains(ID))
	{
		spdlog::error("VAO with ID {} already exists", ID);

		return m_VAOs_[ID];
	}

	HorusVAO VAO;
	VAO.Init();
	m_VAOs_[ID] = VAO;

	spdlog::info("Created VAO with ID {}", ID);

	return m_VAOs_[ID];
}

HorusVBO& HorusOpenGLManager::CreateVBO(int ID, GLfloat* vertices, GLsizeiptr size)
{
	if (m_VBOs_.contains(ID))
	{
		spdlog::error("VBO with ID {} already exists", ID);

		return m_VBOs_[ID];
	}

	HorusVBO VBO;
	VBO.Init(vertices, size);
	m_VBOs_[ID] = VBO;

	spdlog::info("Created VBO with ID {}", ID);

	return m_VBOs_[ID];
}

HorusEBO& HorusOpenGLManager::CreateEBO(int ID, GLuint* indices, GLuint size)
{
	if (m_EBOs_.contains(ID))
	{
		spdlog::error("EBO with ID {} already exists", ID);

		return m_EBOs_[ID];
	}

	HorusEBO EBO;
	EBO.Init(indices, size);
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
