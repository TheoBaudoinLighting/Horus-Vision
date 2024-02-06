#pragma once

#include <string>

#include "GLFW/glfw3.h"

#include "hrs_input.h"

constexpr float DegRad = 3.14159265358979323846f / 180.0f;
constexpr float RadDeg = 180.0f / 3.14159265358979323846f;
constexpr float Epsilon = 0.0001f;
constexpr float EpsilonSq = Epsilon * Epsilon;
constexpr float EpsilonSqInv = 1.0f / EpsilonSq;
constexpr float EpsilonInv = 1.0f / Epsilon;
constexpr float Pi = 3.14159265358979323846f;
constexpr float PiInv = 1.0f / Pi;
constexpr float Pi2 = 1.57079632679489661923f;
constexpr float Pi4 = 0.785398163397448309616f;
constexpr float Pi8 = 0.392699081698724154808f;
constexpr float Pi16 = 0.196349540849362077404f;

class HorusWindowConfig
{
public:

	HorusInput* InputHandler;

	virtual ~HorusWindowConfig() = default;

	HorusWindowConfig() : InputHandler(nullptr) {}

	virtual GLFWwindow* GetWindow() = 0;
	virtual void SetWindow(GLFWwindow* window) = 0;

	virtual void OnScroll(GLFWwindow* Window, double Xoffset, double Yoffset)
	{
		if (InputHandler != nullptr)
		{
			InputHandler->OnScroll(Window, Xoffset, Yoffset);
		}
	}

	virtual void OnKey(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (InputHandler != nullptr)
		{
			InputHandler->OnKey(window, key, scancode, action, mods);
		}
	}

	virtual void OnMouse(GLFWwindow* window, int button, int action, int mods)
	{
		if (InputHandler != nullptr)
		{
			InputHandler->OnMouse(window, button, action, mods);
		}
	}

	virtual void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
	{
		if (InputHandler != nullptr)
		{
			InputHandler->OnMouseMove(window, xpos, ypos);
		}
	}

	virtual void OnResize(GLFWwindow* window, int width, int height)
	{
		if (InputHandler != nullptr)
		{
			InputHandler->OnResize(window, width, height);
		}
	}

	virtual void OnClose()
	{
		if (InputHandler != nullptr)
		{
			InputHandler->OnClose();
		}
	}

	virtual void ScrollCallback(double xoffset, double yoffset) = 0;
	virtual void KeyCallback(int key, int scancode, int action, int mods) = 0;
	virtual void MouseButtonCallback(int button, int action, int mods) = 0;
	virtual void ResizeCallback(int width, int height) = 0;
	virtual void CloseCallback() = 0;

	int WindowWidth = 1280;
	int WindowHeight = 800;
	std::string WindowTitle;
};