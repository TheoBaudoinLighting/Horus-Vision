
#include <iostream>
using namespace std;

#include "hrs_engine.h"
#include "hrs_input.h"

void HorusInput::OnScroll(GLFWwindow* window, double xoffset, double yoffset)
{


	// Debug Scroll Input
	/*if (yoffset > 0)
	{
		cout << "Scroll Up" << endl;
	}
	else if (yoffset < 0)
	{
		cout << "Scroll Down" << endl;
	}*/
}

void HorusInput::OnKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Close Engine
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		HorusEngine::GetInstance().Close(); // Call directly close function
	}



	// Debug Key Input
	// Check Key Press
	/*if (action == GLFW_PRESS)
	{
		const char* keyName = glfwGetKeyName(key, scancode);
		if (keyName != nullptr)
		{
			bool isShiftPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
			bool isCapsLockActive = GetKeyState(VK_CAPITAL) & 1;

			if (isShiftPressed || isCapsLockActive)
			{
				std::string keyStr(keyName);
				std::transform(keyStr.begin(), keyStr.end(), keyStr.begin(), ::toupper);
				cout << keyStr << " pressed" << endl;
			}
			else
			{
				cout << keyName << " pressed" << endl;
			}
		}
	}

	// Check Key Release
	if (action == GLFW_RELEASE)
	{
		const char* keyName = glfwGetKeyName(key, scancode);
		if (keyName != nullptr)
		{
			bool isShiftPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
			bool isCapsLockActive = GetKeyState(VK_CAPITAL) & 1;

			if (isShiftPressed || isCapsLockActive)
			{
				std::string keyStr(keyName);
				std::transform(keyStr.begin(), keyStr.end(), keyStr.begin(), ::toupper);
				cout << keyStr << " released" << endl;
			}
			else
			{
				cout << keyName << " released" << endl;
			}
		}
	}

	// Check Key Repeat
	if (action == GLFW_REPEAT)
	{
		const char* keyName = glfwGetKeyName(key, scancode);
		if (keyName != nullptr)
		{
			bool isShiftPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
			bool isCapsLockActive = GetKeyState(VK_CAPITAL) & 1;

			if (isShiftPressed || isCapsLockActive)
			{
				std::string keyStr(keyName);
				std::transform(keyStr.begin(), keyStr.end(), keyStr.begin(), ::toupper);
				cout << keyStr << " repeated" << endl;
			}
			else
			{
				cout << keyName << " repeated" << endl;
			}
		}
	}*/

	// Debug Key Input
	// Special Keys
	/*
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		cout << "Space pressed" << endl;
	}
	if (mods & GLFW_MOD_SHIFT)
	{
		cout << "Shift key " << ((action == GLFW_PRESS) ? "pressed" : (action == GLFW_RELEASE ? "released" : "held")) << endl;
	}
	if (mods & GLFW_MOD_CONTROL)
	{
		cout << "Control key " << ((action == GLFW_PRESS) ? "pressed" : (action == GLFW_RELEASE ? "released" : "held")) << endl;
	}
	if (mods & GLFW_MOD_ALT)
	{
		cout << "Alt key " << ((action == GLFW_PRESS) ? "pressed" : (action == GLFW_RELEASE ? "released" : "held")) << endl;
	}
	if (mods & GLFW_MOD_SUPER)
	{
		cout << "Super key " << ((action == GLFW_PRESS) ? "pressed" : (action == GLFW_RELEASE ? "released" : "held")) << endl;
	}*/
}

void HorusInput::OnMouse(GLFWwindow* window, int button, int action, int mods)
{



	// Debug Mouse Input
	/*if (action == GLFW_PRESS) {
		cout << "Mouse button " << button << " pressed with mods " << mods << endl;
	}
	else if (action == GLFW_RELEASE) {
		cout << "Mouse button " << button << " released with mods " << mods << endl;
	}

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	cout << "Mouse position at (" << xpos << ", " << ypos << ")" << endl;*/
}

void HorusInput::OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{



	// Debug Mouse Movement
	/*cout << "Mouse moved to position (" << xpos << ", " << ypos << ")" << endl;*/
}

void HorusInput::OnResize(GLFWwindow* window, int width, int height)
{

	// Debug Window Resize
	/*cout << "Window resized to (" << width << ", " << height << ")" << endl;*/
}

void HorusInput::OnClose()
{
	HorusEngine::GetInstance().Close();
}
