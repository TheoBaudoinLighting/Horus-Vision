#pragma once

// Project includes
#include "hrs_window.h" // glad.h
#include "hrs_reset_buffers.h" // nothing
#include "hrs_material_editor.h" // nothing
#include "hrs_radeon.h" // glfw3.h

// Basic includes
#include <string>
#include <memory>

// External includes
#include "imgui.h"

class HorusViewportInput
{
public:
	static HorusViewportInput& get_instance()
	{
		static HorusViewportInput instance;
		return instance;
	}

	HorusViewportInput(HorusViewportInput const&) = delete;
	void operator=(HorusViewportInput const&) = delete;

	void ProcessInput();

private:

	HorusViewportInput() {}

};

class HorusViewportOpenGL
{
public:

	static HorusViewportOpenGL& get_instance()
	{
		static HorusViewportOpenGL instance;
		return instance;
	}

	HorusViewportOpenGL(HorusViewportOpenGL const&) = delete;
	void operator=(HorusViewportOpenGL const&) = delete;

	void ViewportOpenGL(bool* p_open);

	void ResetBuffers();


private:

	HorusViewportOpenGL() {}

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
	bool m_IsFirstLaunch = true;

	inline static std::chrono::high_resolution_clock::time_point m_StartTime_;
	inline static std::chrono::high_resolution_clock::time_point m_EndTime_;
	long long total_seconds;
	long long hours;
	long long minutes;
	long long seconds;
	long long milliseconds;
	long long m_chrono_time_;
	long long duration = 0;

	char GPU00N[1024];
	char GPU01N[1024];

	rpr_render_statistics render_statistics_;


};

class HorusViewportRadeon
{
public:

	static HorusViewportRadeon& get_instance()
	{
		static HorusViewportRadeon instance;
		return instance;
	}

	HorusViewportRadeon(HorusViewportRadeon const&) = delete;
	void operator=(HorusViewportRadeon const&) = delete;

	void ViewportRadeon(bool* p_open);
	void OverlayRadeon(bool* p_open);

	void ResetBuffers();


private:

	HorusViewportRadeon() {}

	// 
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
	bool m_IsFirstLaunch = true;

	inline static std::chrono::high_resolution_clock::time_point m_StartTime_;
	inline static std::chrono::high_resolution_clock::time_point m_EndTime_;
	long long total_seconds;
	long long hours;
	long long minutes;
	long long seconds;
	long long milliseconds;
	long long m_chrono_time_;
	long long duration = 0;

	char GPU00N[1024];
	char GPU01N[1024];

	rpr_render_statistics render_statistics_;




};

