#pragma once

// Project includes
#include "common.h" // nothing
#include "hrs_garbage_collector.h" // nothing
#include "hrs_context.h" // glfw3.h

// External includes
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

	void CallRenderMultiTiles(int Tile_Size, int MaxIterationPerTiles);
	void RenderMultiTiles(HorusFrameBufferMetadata& fb_metadata, rpr_scene, rpr_context context, rpr_uint MaxIterationRendering);

	float GetRenderProgress();

	glm::vec2 SetWindowSize(int width, int height);

	rpr_context GetContext() { return m_Context_; }
	rpr_material_system GetMatsys() { return m_Matsys_; }

	GLuint GetTextureBuffer() { return m_TextureBuffer_; } // Buffer to render in ImGui for Radeon

	rpr_framebuffer GetFrameBuffer() { return m_FrameBuffer_; }
	rpr_framebuffer GetFrameBufferResolved() { return m_FrameBuffer2_; }

	void SetSampleCount(int sample_count) { m_SampleCount_ = sample_count; }

	bool GetIsDirty() { return m_IsDirty_; }
	bool SetIsDirty(bool is_dirty) { m_IsDirty_ = is_dirty; return m_IsDirty_; }

	int GetSampleCount() { return m_SampleCount_; }
	int GetMinSamples() { return m_MinSamples_; }
	int GetMaxSamples() { return m_MaxSamples_; }
	int SetMinSamples(int min_samples) { m_MinSamples_ = min_samples; return m_MinSamples_; }
	int SetMaxSamples(int max_samples) { 	m_MaxSamples_ = max_samples; return m_MaxSamples_; }

private:

	HorusRadeon(): m_WindowConfig_(nullptr), m_IsDirty_(false), m_Program_(0)
	{
	}

	// Render stuff

	int m_WindowWidth_ = 0;
	int m_WindowHeight_ = 0;
	HorusWindowConfig* m_WindowConfig_;

	int m_MinSamples_ = 4;
	int m_MaxSamples_ = 32;
	int m_SampleCount_ = 0;
	int m_BatchSize_ = 0;

	int m_RenderTargetSizeX_ = m_WindowWidth_;
	int m_RenderTargetSizeY_ = m_WindowHeight_;
	const int m_MaxIterationTiledRendering_ = 128;

	float m_SensorY_ = 36.0f;
	float m_SensorX_ = m_SensorY_ * (float(m_RenderTargetSizeX_) / float(m_RenderTargetSizeY_));

	float m_PreviousLensShift_ = 0.0;

	bool m_ThreadRunning_ = false;
	std::thread m_RenderThread_; // single-threading
	std::vector<std::thread> m_RenderThreads_; // multi-threading

	bool m_IsDirty_;

	// Radeon stuff

	GLuint m_Program_;

	GLuint m_TextureBuffer_ = 0;
	GLuint m_VertexBufferId_ = 0;
	GLuint m_IndexBufferId_ = 0;

	GLuint m_UpBuffer_ = 0;

	rpr_framebuffer m_FrameBuffer_ = nullptr;
	rpr_framebuffer m_FrameBuffer2_ = nullptr;

	rpr_context m_Context_ = nullptr;
	rpr_material_system m_Matsys_ = nullptr;
	rpr_camera m_Camera_ = nullptr;

	std::shared_ptr<float> m_FbData_ = nullptr;
};