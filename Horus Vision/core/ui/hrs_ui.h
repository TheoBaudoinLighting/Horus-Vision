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

	HorusUI() = default;

	// Init
	inline static bool m_ShowOpenGLViewport_ = false;
	inline static bool m_ShowRadeonViewport_ = true;
	inline static bool m_ShowOutliner_ = true;
	inline static bool m_ShowConsole_ = false;
	inline static bool m_ShowScene_ = false;
	inline static bool m_ShowInspector_ = true;

	bool m_LoadLogoTexture_ = true;
	bool m_LoadUILogoTexture_ = true;

	int m_LogoWidth_ = 0;
	int m_LogoHeight_ = 0;
	GLuint m_LogoTexture_ = 0;

	int m_UILogoWidth_ = 0;
	int m_UILogoHeight_ = 0;
	GLuint m_UILogoTexture_ = 0;

	// Main Menu Bar
	bool m_IsRunning_ = true;
	bool m_IsClosing_ = false;
	bool m_OptionsChanged_ = false;

	// Windows




};

