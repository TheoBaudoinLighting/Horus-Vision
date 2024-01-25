#pragma once
#pragma warning(disable : 4996)
#pragma warning(disable : 4244)
#pragma warning(disable : 4267)
#pragma warning(disable : 4005)

// Project includes
#include "hrs_context.h" // glfw3.h

#include <future>
#include <ProRenderGLTF.h>

// External includes
#include "common.h"
#include "glm/glm.hpp"
#include "spdlog/spdlog.h"

class HorusRadeon : public HorusContext
{
public:

	struct HorusFrameBufferMetadata
	{
		int RenderTargetSizeX, RenderTargetSizeY;
		int TileSizeX, TileSizeY;
		std::vector<rpr_uchar> FbData;
	};

	static HorusRadeon& GetInstance()
	{
		static HorusRadeon Instance; // Instance unique
		return Instance;
	}

	HorusRadeon(const HorusRadeon&) = delete;
	void operator=(const HorusRadeon&) = delete;

	void CreateFrameBuffers(int width, int height);
	bool Init(int width, int height, HorusWindowConfig* window) override;

	void InitRender() override { }
	void PostRender() override { }
	void QuitRender() override;

	bool InitGraphics();
	void ResizeRender(int width, int height);

	void RenderEngine();
	void RenderClassic();
	void RenderAdaptive();
	void SetClassicRender();
	void SetAdaptiveRender();
	void CallRenderMultiTiles(int Tile_Size, int MaxIterationPerTiles);
	void RenderMultiTiles(HorusFrameBufferMetadata& FbMetadata, rpr_scene, rpr_context Context, rpr_uint MaxIterationRendering);

	float GetClassicRenderProgress();
	float GetAdaptiveRenderProgress();

	void AsyncFramebufferUpdate(rpr_context Context, rpr_framebuffer FramebufferA) const;
	glm::vec2 SetWindowSize(int width, int height);
	glm::vec2 GetWindowSize() const { return glm::vec2(m_WindowWidth_, m_WindowHeight_); }

	rpr_context GetContext() { return m_ContextA_; }
	rpr_material_system GetMatsys() { return m_Matsys_; }

	GLuint GetTextureBuffer() { return m_RadeonTextureBuffer_; } // Buffer to render in ImGui for Radeon

	bool GetIsAdaptiveRender() { return m_IsAdaptativeSampling_; }

	rpr_framebuffer GetClassicRenderFrameBuffer() { return m_FrameBufferA_; }
	rpr_framebuffer GetAdaptiveRenderFrameBuffer() { return m_FrameBufferAdaptive_; }
	rpr_framebuffer GetFrameBufferResolved() { return m_FrameBufferB_; }

	void SetSampleCount(int SampleCount) { m_SampleCount_ = SampleCount; }
	void SetActivePixelRemains(unsigned int ActivePixelRemains) { m_ActivePixelRemains_ = ActivePixelRemains; }
	void SetAdaptiveSamplingTileSize(int TileSize) { m_MinAdaptiveTileSize_ = TileSize; CHECK(rprContextSetParameterByKey1u(m_ContextA_, RPR_CONTEXT_ADAPTIVE_SAMPLING_TILE_SIZE, m_MinAdaptiveTileSize_)); }
	void SetAdaptiveSamplingThreshold(float Threshold) { m_AdaptiveThreshold_ = Threshold; CHECK(rprContextSetParameterByKey1f(m_ContextA_, RPR_CONTEXT_ADAPTIVE_SAMPLING_THRESHOLD, m_AdaptiveThreshold_)); }

	bool GetIsDirty() { return m_IsDirty_; }
	bool SetIsDirty(bool IsDirty) { m_IsDirty_ = IsDirty; return m_IsDirty_; }

	bool GetLockingRender() { return m_LockingRender_; }
	void SetLockingRender(bool LockingRender) { m_LockingRender_ = LockingRender; }

	int GetSampleCount() { return m_SampleCount_; }
	int GetMinSamples() { return m_MinSamples_; }
	int GetMaxSamples() { return m_MaxSamples_; }
	int SetMinSamples(int MinSamples) { m_MinSamples_ = MinSamples; return m_MinSamples_; }
	int SetMaxSamples(int MaxSamples) { m_MaxSamples_ = MaxSamples; return m_MaxSamples_; }

private:

	HorusRadeon() : m_WindowConfig_(nullptr), m_IsDirty_(false), m_Program_(0)
	{
	}

	// Render stuff

	int m_WindowWidth_ = 0;
	int m_WindowHeight_ = 0;
	HorusWindowConfig* m_WindowConfig_;

	// Classic render
	int m_MinSamples_ = 4;
	int m_MaxSamples_ = 32;
	int m_SampleCount_ = 0;
	int m_BatchSize_ = 1;
	int m_MaxIterations_ = 1; // 1 iteration = 1 sample

	// Adaptive render
	int m_MinAdaptiveTileSize_ = 8;
	int m_MinSamplesAdaptive_ = 16;
	int m_MaxIterationsPerSamples_ = 128;
	float m_AdaptiveThreshold_ = 0.015f;
	unsigned int m_ActivePixelRemains_ = 0;

	int m_RenderTargetSizeX_ = m_WindowWidth_;
	int m_RenderTargetSizeY_ = m_WindowHeight_;
	const int m_MaxIterationTiledRendering_ = 128;

	float m_SensorY_ = 36.0f;
	float m_SensorX_ = m_SensorY_ * (static_cast<float>(m_RenderTargetSizeX_) / static_cast<float>(m_RenderTargetSizeY_));

	float m_PreviousLensShift_ = 0.0;

	bool m_ThreadRunning_ = false;
	std::future<void> m_ClassicRenderFuture_;
	std::future<void> m_AdaptiveRenderFuture_;
	std::future<void> m_ClassicRenderInfoFuture_;
	std::future<void> m_AdaptiveRenderInfoFuture_;
	//std::thread m_RenderThread_; // single-threading
	std::jthread m_RenderThread_; // single-threading
	std::vector<std::thread> m_RenderThreads_; // multi-threading

	bool m_IsDirty_;
	bool m_IsAdaptativeSampling_ = false;
	bool m_LockingRender_ = false;

	// Radeon stuff

	GLuint m_Program_;

	GLuint m_RadeonTextureBuffer_ = 0;
	GLuint m_VertexBufferId_ = 0;
	GLuint m_IndexBufferId_ = 0;

	GLuint m_UpBuffer_ = 0;

	// Simple framebuffers
	rpr_framebuffer m_FrameBufferA_ = nullptr;
	rpr_framebuffer m_FrameBufferB_ = nullptr;
	rpr_framebuffer m_FrameBufferC_ = nullptr;

	// Adaptive Sampling framebuffers
	rpr_framebuffer m_FrameBufferAdaptive_ = nullptr;

	// Framebuffer dest
	rpr_framebuffer m_FrameBufferDest_ = nullptr;


	rpr_context m_ContextA_ = nullptr;

	rpr_material_system m_Matsys_ = nullptr;
	rpr_camera m_Camera_ = nullptr;

	std::shared_ptr<float> m_FbData_ = nullptr;
};