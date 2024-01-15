#pragma once

#include <filesystem>

#include "hrs_context.h" // glfw3.h

class HorusImGui : public HorusContext
{
public:

	static HorusImGui& get_instance()
	{
		static HorusImGui instance; // Instance unique
		return instance;
	}

	HorusImGui(const HorusImGui&) = delete;
	void operator=(const HorusImGui&) = delete;

	bool init(int width, int height, HorusWindowConfig* window) override;

	void init_render() override;
	void post_render() override;
	void quit_render() override;

private:

	HorusImGui() {}

	std::string m_current_path_ = std::filesystem::current_path().string();

	//OverpassMono - VariableFont_wght.ttf

	//std::string m_font_path_ = "fonts/Montserrat-Bold.ttf";
	std::string m_font_path_ = "fonts/UbuntuMono-Bold.ttf";

	const char* m_glsl_version_ = "#version 460";
};