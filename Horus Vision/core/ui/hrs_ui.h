#pragma once
#pragma warning( disable : 4005)
#pragma warning( disable : 4996)

#include "glad/glad.h"

class HorusUI
{
public:
	static HorusUI& GetInstance()
	{
		static HorusUI Instance;
		return Instance;
	}

	HorusUI(const HorusUI&) = delete;
	void operator=(const HorusUI&) = delete;

	void Init();

	// Main Menu Bar
	void MainMenuBar();

	void RenderUI();

	// Reset Buffers
	void ResetBuffers();

	// Getters
	bool GetOptionsChanged() const { return m_OptionsChanged_; }

	// Setters
	void SetOptionsChanged(bool Value) { m_OptionsChanged_ = Value; }

private:
	HorusUI() = default;

	// Init
	inline static bool mShowOpenGlViewport_ = true;
	inline static bool mShowRadeonViewport_ = true;
	inline static bool mShowOutliner_ = true;
	inline static bool mShowConsole_ = true;
	inline static bool mShowInspector_ = true;
	inline static bool mShowStatistics_ = true;
	inline static bool mShowHelp_ = false;
	inline static bool mShowAbout_ = false;

	// Sub Menu
	inline static bool mShowImportMesh_ = false;


	bool m_LoadLogoTexture_ = true;
	bool m_LoadUiLogoTexture_ = true;

	int m_LogoWidth_ = 0;
	int m_LogoHeight_ = 0;
	GLuint m_LogoTexture_ = 0;

	int m_UiLogoWidth_ = 0;
	int m_UiLogoHeight_ = 0;
	GLuint m_UILogoTexture_ = 0;

	// Main Menu Bar
	bool m_IsRunning_ = true;
	bool m_IsClosing_ = false;
	bool m_OptionsChanged_ = false;
};
