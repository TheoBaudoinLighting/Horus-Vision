#pragma once

// Project includes
#include "hrs_window.h" // glad.h
#include "hrs_reset_buffers.h" // nothing
#include "hrs_material_editor.h" // nothing

// Basic includes

// External includes
#include "imgui.h"

class HorusViewportInput
{
public:
	static HorusViewportInput& GetInstance()
	{
		static HorusViewportInput Instance;
		return Instance;
	}

	HorusViewportInput(const HorusViewportInput&) = delete;
	void operator=(const HorusViewportInput&) = delete;

	void ProcessInput();

private:
	HorusViewportInput() = default;
};

class HorusViewportOpenGL
{
public:
	static HorusViewportOpenGL& GetInstance()
	{
		static HorusViewportOpenGL Instance;
		return Instance;
	}

	HorusViewportOpenGL(const HorusViewportOpenGL&) = delete;
	void operator=(const HorusViewportOpenGL&) = delete;

	void ViewportOpenGL(bool* p_open);

	void ResetBuffers();

private:
	HorusViewportOpenGL() = default;

	ImVec2 m_ImageSize_;
	ImVec2 m_StoreImagePosition_;

	int m_MinSamples_ = 4;
	int m_MaxSamples_ = 32;

	float m_AdaptiveThreshold_ = 0.01f;
	float m_LastProgress_ = -1.0f;
	float m_AspectRationRender_;

	bool m_EnableRenderRegion_ = false;
	bool m_HasStartedRender_ = false;
	bool m_OptionsChanged_ = false;
	bool m_ResetBuffer_ = false;
	bool m_IsFirstLaunch_ = true;


	inline static std::chrono::high_resolution_clock::time_point m_StartTime_;
	inline static std::chrono::high_resolution_clock::time_point m_EndTime_;
	long long m_TotalSeconds_;
	long long m_Hours_;
	long long m_Minutes_;
	long long m_Seconds_;
	long long m_Milliseconds_;
	long long m_ChronoTime_;
	long long m_Duration_ = 0;

	char m_Gpu00N_[1024];
	char m_Gpu01N_[1024];

	rpr_render_statistics m_RenderStatistics_;
};

class HorusViewportRadeon
{
public:
	static HorusViewportRadeon& GetInstance()
	{
		static HorusViewportRadeon Instance;
		return Instance;
	}

	HorusViewportRadeon(const HorusViewportRadeon&) = delete;
	void operator=(const HorusViewportRadeon&) = delete;

	void ViewportRadeon(bool* p_open);
	void OverlayRadeon(bool* p_open);

	void ResetBuffers();

	void ShowHelpMarker(const char* desc);

	// Getters
	bool GetViewportLocked() { return m_IsViewportLocked_; }
	bool GetIsFullRender() { return m_IsFullRender_; }

	// Setters
	void SetViewportLocked(bool value) { m_IsViewportLocked_ = value; }
	void SetIsFullRender(bool value) { m_IsFullRender_ = value; }

private:
	HorusViewportRadeon() = default;

	// Viewport variables
	const ImVec4 m_GreenColor_ = ImVec4(0.6f, 1.0f, 0.6f, 1.0f);
	const ImVec4 m_PinkColor_ = ImVec4(1.0f, 0.6f, 0.7f, 1.0f);

	ImVec2 m_Pos_ = ImGui::GetCursorScreenPos();
	ImVec2 m_ViewerSize_;
	ImVec2 m_LastSize_ = ImVec2(0, 0);
	float m_AvailableWidth_;
	float m_AvailableHeight_;
	ImVec2 m_ViewerWindowPos_;
	ImVec2 m_ViewerWindowSize_;
	ImVec2 m_UvMin_ = ImVec2(0.0f, 0.0f);
	ImVec2 m_UvMax_ = ImVec2(1.0f, 1.0f);
	int m_LastCustomX_ = m_CustomX_;
	int m_LastCustomY_ = m_CustomY_;
	int m_CustomX_ = 800;
	int m_CustomY_ = 600;
	bool m_FirstLaunch_ = true;

	bool m_IsZooming_ = false;
	bool m_UseTextColorForTint_ = false;
	bool m_IsResizable_ = false;

	// Mouse variables
	bool m_IsDragging_ = false;
	ImVec2 m_LastMousePos_ = ImVec2(0, 0);
	float m_OffsetX_;
	float m_OffsetY_;

	float m_WidthPerItems_ = 100;

	ImGuiIO& m_Io_ = ImGui::GetIO();
	bool m_IsLeftAltPressed_ = m_Io_.KeyAlt;

	ImGuiWindowFlags m_WindowFlags_ = 0;

	int m_ElementsNumber_ = 8;

	const char* m_SizePresetItem_[8] = { "800x600", "1024x768", "1280x720", "1920x1080", "2560x1440", "3840x2160", "7680x4320", "Custom" };
	int m_SizePresetItemCurrent_ = 0;

	// Radeon variables

	ImVec2 m_ImageSize_;
	ImVec2 m_StoreImagePosition_;

	int m_MinSamples_ = 4;
	int m_MaxSamples_ = 32;

	int m_TileSize_ = 128;
	int m_MaxIterationPerTile_ = 32;

	float m_AdaptiveThreshold_ = 0.01f;
	float m_LastProgress_ = -1.0f;
	float m_AspectRationRender_;

	bool m_EnableRenderRegion_ = false;
	bool m_HasStartedRender_ = false;
	bool m_OptionsChanged_ = false;
	bool m_ResetBuffer_ = false;
	bool m_IsFirstLaunch_ = true;
	bool m_IsViewportLocked_ = false;
	bool m_IsFullRender_ = false;
	bool m_IsFullRenderLaunched_ = false;
	bool m_CallToRenderFullImage_ = false;
	bool m_RenderIsLocked_ = false;

	inline static std::chrono::high_resolution_clock::time_point m_StartTime_;
	inline static std::chrono::high_resolution_clock::time_point m_EndTime_;
	long long m_TotalSeconds_;
	long long m_Hours_;
	long long m_Minutes_;
	long long m_Seconds_;
	long long m_Milliseconds_;
	long long m_ChronoTime_;
	long long m_Duration_ = 0;

	char m_Gpu00N_[1024];
	char m_Gpu01N_[1024];

	rpr_render_statistics m_RenderStatistics_;
};
