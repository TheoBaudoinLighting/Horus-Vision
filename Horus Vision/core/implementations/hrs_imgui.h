#pragma once

#include <filesystem>

#include "hrs_context.h"

class HorusImGui : public HorusContext
{
public:

	static HorusImGui& get_instance()
	{
		static HorusImGui instance; // Instance unique
		return instance;
	}

	bool init(int width, int height, HorusWindowConfig* window) override;

	void init_render() override;
	void post_render() override;
	void quit_render() override;

private:

	HorusImGui() = default;
	HorusImGui(const HorusImGui&) = delete;
	void operator=(const HorusImGui&) = delete;

	std::string m_current_path_ = std::filesystem::current_path().string();
	std::string m_font_path_ = "fonts/Montserrat-Bold.ttf";

	const char* m_glsl_version_ = "#version 460";
};