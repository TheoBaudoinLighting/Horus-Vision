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

	// 
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
