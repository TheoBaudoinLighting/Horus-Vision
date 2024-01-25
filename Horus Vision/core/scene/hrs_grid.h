#pragma once

#include "hrs_shader_manager.h"
#include "glad/glad.h"
#include "glm/glm.hpp"


class HorusGrid
{
public:

	static HorusGrid& GetInstance()
	{
		static HorusGrid Instance;
		return Instance;
	}

	HorusGrid(HorusGrid const&) = delete;
	void operator=(HorusGrid const&) = delete;

	void Grid();
	void Update();
	void Render(const HorusShaderManager& Shader);

private:

	HorusGrid() = default;

	glm::mat4 m_Model_;
	glm::vec3 m_Position_;
	glm::vec3 m_Rotation_;
};
