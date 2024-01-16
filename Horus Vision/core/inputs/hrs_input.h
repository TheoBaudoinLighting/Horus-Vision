#pragma once

#include "hrs_garbage_collector.h"

#include "GLFW/glfw3.h"

class HorusInput
{
public:

	static HorusInput& GetInstance()
	{
		static HorusInput Instance;
		return Instance;
	}

	HorusInput(HorusInput const&) = delete;
	void operator=(HorusInput const&) = delete;

	void OnScroll(GLFWwindow* window, double xoffset, double yoffset);
	void OnKey(GLFWwindow* window, int key, int scancode, int action, int mods);
	void OnMouse(GLFWwindow* window, int button, int action, int mods);
	void OnMouseMove(GLFWwindow* window, double xpos, double ypos);
	void OnResize(GLFWwindow* window, int width, int height);
	void OnClose();

private:

	HorusInput() {}
};
