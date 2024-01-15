#pragma once

// External includes
#include "imgui.h"
#include "imgui_internal.h"
#include "imnodes.h"
#include "ImGuizmo.h"
//#include "L2DFileDialog.h"

// Basic includes
#include <filesystem>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <regex>
#include <chrono>

class HorusUI
{
public:

	static HorusUI& get_instance()
	{
		static HorusUI instance;
		return instance;
	}

	HorusUI(HorusUI const&) = delete;
	void operator=(HorusUI const&) = delete;

	void Init();

	// Main Menu Bar
	void MainMenuBar();

	void RenderUI();

	// Reset Buffers
	void ResetBuffers();

	// Getters
	bool GetOptionsChanged() { return m_OptionsChanged_; }

	// Setters
	void SetOptionsChanged(bool value) { m_OptionsChanged_ = value; }

private:

	HorusUI() {}

	// Init
	inline static bool m_ShowOpenGLViewport_ = false;
	inline static bool m_ShowRadeonViewport_ = true;
	inline static bool m_ShowOutliner_ = true;
	inline static bool m_ShowConsole_ = false;
	inline static bool m_ShowScene_ = false;
	inline static bool m_ShowInspector_ = true;

	// Main Menu Bar
	bool m_IsRunning_ = true;
	bool m_IsClosing_ = false;
	bool m_OptionsChanged_ = false;

	// Windows




};

