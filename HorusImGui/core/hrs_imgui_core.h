#pragma once
#pragma warning( disable : 4005)
#pragma warning( disable : 4996)

// C++ includes
#include <filesystem>

// ImGui includes
#include "imgui.h"

// Project includes
#include "hrs_context.h" // glfw3.h

class HorusImGuiCore : public HorusContext
{
public:

	static HorusImGuiCore& GetInstance()
	{
		static HorusImGuiCore Instance;
		return Instance;
	}

	HorusImGuiCore(HorusImGuiCore const&) = delete;
	void operator=(HorusImGuiCore const&) = delete;

	void PrintMessage();

	bool Initialize(const int Width, const int Height, HorusWindowConfig* WindowConfig);

	void InitRender() override;
	void PostRender() override;
	void QuitRender() override;

	ImFont* GetIconFont() { return m_IconFont_; }

private:

	HorusImGuiCore() {}

	std::string m_CurrentPath_ = std::filesystem::current_path().string();

	std::string m_FontSemiBoldPath_ = "fonts/Montserrat/Montserrat-SemiBold.ttf";
	std::string m_FontBoldPath_ = "fonts/Montserrat/Montserrat-Bold.ttf";

	ImFont* m_IconFont_ = nullptr;

	const char* m_GlslVersion_ = "#version 460";

};
