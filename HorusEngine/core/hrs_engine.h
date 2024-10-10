#pragma once
#pragma warning (disable: 4127)  
#pragma warning (disable: 4996)  
#pragma warning (disable: 26451) 
#pragma warning(disable : 4244)
#pragma warning(disable : 4267)
#pragma warning(disable : 4005)

// External includes
#include "imgui.h"

// Project includes
#include "hrs_window.h" // glad.h
#include "hrs_material_editor.h" // nothing
#include "hrs_radeon.h" // glfw3.h

// Basic includes
#include <string>
#include <memory>

class HorusEngine
{
public:

	static HorusEngine& GetInstance()
	{
		static HorusEngine Instance;
		return Instance;
	}

	HorusEngine(HorusEngine const&) = delete;
	void operator=(HorusEngine const&) = delete;

	int ContextInfo(rpr_context& Context);

	void Init(int Width, int Height, const std::string& Title, std::string SavePath);
	void InitContexts(int Width, int Height, HorusWindow* Window);
	void PreRender();
	void Render();
	void PostRender();
	bool IsRunning() const { return m_IsRunning_; }
	void Close();

	void UpdatePerformanceData(float CurrentFps, float CurrentSampling)
	{
		m_FpsData_[m_CurrentFrame_ % DataPointsCount] = CurrentFps;
		m_SamplingData_[m_CurrentFrame_ % DataPointsCount] = CurrentSampling;
		m_CurrentFrame_++;
	}

	bool GetIsClosing() { return m_IsClosing_; }
	bool GetIsFirstLaunch() { return m_IsFirstLaunch_; }
	bool GetEngineIsReady() { return m_EngineIsReady_; }
	bool GetIsOptionsChanged() { return m_IsOptionsChanged_; }

	void SetIsClosing(bool closing) { m_IsClosing_ = closing; }
	void SetEngineIsReady(bool ready) { m_EngineIsReady_ = ready; }

	ImVec2 GetImageSize() { return m_ImgSize_; }
	float GetImageAspectRatio() { return m_AspectRatioViewer_; }

	static void CallResetBuffer();

private:

	HorusEngine(): m_Status_(), m_NumFrames_(0), m_RenderStatistics_(), m_Gpu00N_{}, m_Gpu01N_{},
	               m_AspectRatioViewer_(0), m_ChronoTime_(0),
	               m_TotalSeconds_(0), m_Hours_(0),
	               m_Minutes_(0),
	               m_Seconds_(0),
	               m_Milliseconds_(0)
	{
	}

	std::unique_ptr<HorusWindow> m_Window_;
	std::string m_SaveFilename_;

	bool m_EngineIsReady_ = false;
	bool m_IsRunning_ = true;
	bool m_IsClosing_ = false;
	bool m_IsFirstLaunch_ = true;
	bool m_IsSecondLaunch_ = true;
	bool m_LaunchLoadData_ = true;

	//std::future<bool> m_LoadingThread_ = async(std::launch::async, []() { return true; });
	std::jthread m_LoadingThread_;
	std::future_status m_Status_;

	const float m_DesiredWidth_ = 800.0f;
	char m_UserInput_[256] = "";
	int m_Suffix_ = 001;

	int m_CameraNumber_ = 0;
	int m_LightNumber_ = 0;
	int m_MeshNumber_ = 0;
	int m_MaterialNumber_ = 0;
	int m_ImageNumber_ = 0;
	int m_PostEffectNumber_ = 0;
	int m_VolumeNumber_ = 0;
	int m_CurveNumber_ = 0;
	int m_GridNumber_ = 0;
	int m_BufferNumber_ = 0;
	int m_FramebufferNumber_ = 0;
	int m_SceneNumber_ = 0;
	int m_MaterialNodeNumber_ = 0;
	int m_CompositeNumber_ = 0;
	int m_LutNumber_ = 0;

	rpr_uint m_NumFrames_;
	rpr_render_statistics m_RenderStatistics_;

	bool m_EnableAdaptiveSampling_ = false;
	bool m_EnablePreviewMode_ = false;
	bool m_EnableAa_ = true;
	bool m_ShowObjectContour_ = false;
	int m_SamplerType_ = RPR_CONTEXT_SAMPLER_TYPE_SOBOL;

	char m_Gpu00N_[1024];
	char m_Gpu01N_[1024];

	std::vector<float> m_Frame_;
	std::vector<float> m_SamplesMkr_;

	glm::vec2 m_Size_{};

	ImVec2 m_ImgSize_;
	float m_AspectRatioViewer_;
	ImVec2 m_StoredImagePosition_;

	bool m_EnableRenderRegion_ = false;
	bool m_PreviousEnableAdaptiveSampling_ = false;
	bool m_PreviousEnableAa_ = false;
	bool m_PreviousResetMode_ = false;
	bool m_ResetBuffer_ = false;
	bool m_EnableBackdropImage_ = false;
	bool m_PreviousEnableBackdropImage_ = false;
	bool m_EnableDenoiser_ = false;
	bool m_EnableRussianRoulette_ = false;
	bool m_PreviousEnableRussianRoulette_ = false;
	bool m_PreviousEnableTransparentBackground_ = false;
	bool m_EnableTransparentBackground_ = false;
	long long m_ChronoTime_;

	inline static float m_LastProgress_ = -1.0f;
	inline static bool m_HasStarted_ = false;
	inline static bool m_IsOptionsChanged_ = false;
	inline static std::chrono::high_resolution_clock::time_point m_StartTime_;
	inline static std::chrono::high_resolution_clock::time_point m_EndTime_;
	long long m_TotalSeconds_;
	long long m_Hours_;
	long long m_Minutes_;
	long long m_Seconds_;
	long long m_Milliseconds_;

	static const int DataPointsCount = 100;

	float m_FpsData_[DataPointsCount] = {};
	float m_SamplingData_[DataPointsCount] = {};
	int m_CurrentFrame_ = 0;

	inline static bool m_ShowWinuiViewer_ = false;
	inline static bool m_ShowWinuiRender_ = true;
	inline static bool m_ShowWinuiPropertyEditor_ = true;
	inline static bool m_ShowWinuiOutliner_ = false;
	inline static bool m_ShowWinuiMaterialEditor_ = false;
	inline static bool m_ShowWinuiMainMenuBar_ = false;
	inline static bool m_ShowWinuiMaterialBrowser_ = true;
	inline static bool m_ShowWinuiViewerRt_ = true;
	inline static bool m_ShowWinuiConsole_ = true;

	std::string m_SelectedMesh_;
	std::string m_SelectedMaterial_;
	std::string m_SelectedCamera_;
	std::map<std::string, int> m_MeshNameToId_;

	int m_MinSamples_ = 4;
	int m_MaxSamples_ = 128;
	float m_AdaptiveThreshold_ = 0.01f;
};
