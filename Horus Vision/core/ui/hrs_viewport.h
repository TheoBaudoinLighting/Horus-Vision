#pragma once

// Project includes
#include "hrs_window.h" // glad.h
#include "hrs_reset_buffers.h" // nothing
#include "hrs_material_editor.h" // nothing

// Basic includes

// External includes
#include "imgui.h"

class HorusViewportInput
{
public:
	static HorusViewportInput& GetInstance()
	{
		static HorusViewportInput Instance;
		return Instance;
	}

	HorusViewportInput(const HorusViewportInput&) = delete;
	void operator=(const HorusViewportInput&) = delete;

	void ProcessInputOpenGl();
	void ProcessInputRadeon();

private:

	HorusViewportInput() = default;
};