#pragma once

// Project includes
#include "hrs_window.h" // glad.h
#include "hrs_reset_buffers.h" // nothing
#include "hrs_material_editor.h" // nothing

// External includes
#include "imgui.h"

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

	const ImVec4 m_GreenColor_ = ImVec4(0.6f, 1.0f, 0.6f, 1.0f);
	const ImVec4 m_PinkColor_ = ImVec4(1.0f, 0.6f, 0.7f, 1.0f);

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