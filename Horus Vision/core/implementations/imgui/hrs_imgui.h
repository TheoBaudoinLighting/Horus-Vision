#pragma once
#pragma warning( disable : 4005)
#pragma warning( disable : 4996)

#include <filesystem>

#include "imgui.h"

#include "hrs_context.h" // glfw3.h

class HorusImGui : public HorusContext
{
public:

	static HorusImGui& GetInstance()
	{
		static HorusImGui Instance; // Instance unique
		return Instance;
	}

	HorusImGui(const HorusImGui&) = delete;
	void operator=(const HorusImGui&) = delete;

	bool Init(int Width, int Height, HorusWindowConfig* Window) override;

	void InitRender() override;
	void PostRender() override;
	void QuitRender() override;

	ImFont* GetIconFont() { return m_IconFont_; }

private:

	HorusImGui() {}

	std::string m_CurrentPath_ = std::filesystem::current_path().string();

	// Fonts
	/*ImFont* m_medium_;
	ImFont* m_bold_;
	ImFont* m_tab_icon_;
	ImFont* m_logo_;
	ImFont* m_tab_title_;
	ImFont* m_tab_title_icon_;
	ImFont* m_subtab_title_;
	ImFont* m_combo_arrow_;*/

	std::string m_FontSemiBoldPath_ = "fonts/Montserrat/Montserrat-SemiBold.ttf";
	std::string m_FontBoldPath_ = "fonts/Montserrat/Montserrat-Bold.ttf";

	ImFont* m_IconFont_ = nullptr;

	const char* m_GlslVersion_ = "#version 460";
};