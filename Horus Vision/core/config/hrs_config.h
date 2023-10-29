#pragma once

#include <string>

#include "GLFW/glfw3.h"

constexpr float DEG_RAD = 3.14159265358979323846f / 180.0f;
constexpr float RAD_DEG = 180.0f / 3.14159265358979323846f;
constexpr float EPSILON = 0.0001f;
const float EPSILON_SQ = EPSILON * EPSILON;
const float EPSILON_SQ_INV = 1.0f / EPSILON_SQ;
const float EPSILON_INV = 1.0f / EPSILON;
constexpr float PI = 3.14159265358979323846f;
const float PI_INV = 1.0f / PI;
constexpr float PI_2 = 1.57079632679489661923f;
constexpr float PI_4 = 0.785398163397448309616f;
constexpr float PI_8 = 0.392699081698724154808f;
constexpr float PI_16 = 0.196349540849362077404f;

class HorusWindowConfig
{
public:
	virtual GLFWwindow* get_window() = 0;
	virtual void set_window(GLFWwindow* window) = 0;

	virtual void scroll_callback(double xoffset, double yoffset) = 0;
	virtual void key_callback(int key, int scancode, int action, int mods) = 0;
	virtual void mouse_button_callback(int button, int action, int mods) = 0;
	virtual void resize_callback(int width, int height) = 0;
	virtual void close_callback() = 0;

	int m_window_width_ = 1280;
	int m_window_height_ = 800;
	std::string m_window_title_;
};
