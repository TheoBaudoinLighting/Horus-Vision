
#include "hrs_engine.h"
#include "hrs_object_manager.h"
#include "hrs_importer.h"
#include "hrs_scene.h"
#include "hrs_radeon.h"
#include "hrs_timer.h"

#include <cassert>
#include <iostream>
#include <thread>
#include <memory>
#include <array>
#include <chrono>
#include <future>
#include <array>
#include <vector>
#include <future>
#include <functional> // Pour std::ref
#include <atomic>


#include <Math/mathutils.h>
#include "hrs_radeon_posteffect.h"
#include "hrs_ocio.h"

#include <tbb/task_group.h>

#include "spdlog/spdlog.h"
#include "ImGuizmo.h"
#include <hrs_console.h>
#include <hrs_viewport.h>

#include "RadeonImageFilters.h" // TODO : Implement RadeonImageFilters

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <shared_mutex>

#include "hrs_statistics.h"
#include "hrs_utils.h"
#include "picojson.h"
#include "stbi/stb_image_write.h"

class RenderProgressCallback
{
public:

	struct Update
	{
		Update() : HasUpdate(0), Done(0), Aborted(0), RenderReady(false), CamUpdated(0), Progress(0.0f) {}

		volatile int HasUpdate;
		volatile int Done;
		volatile int Aborted;
		bool RenderReady;
		int CamUpdated;
		float Progress;

		void Clear()
		{
			HasUpdate = Done = Aborted = CamUpdated = 0;
		}
	};

	static void NotifyUpdate(float x, void* userData)
	{
		auto update = static_cast<Update*>(userData);
		update->HasUpdate = 1;
		update->Progress = x;
	}
};

RenderProgressCallback::Update RenderProgressCallback;
std::atomic<bool> StopFlag{ false };

constexpr auto InvalidTime = std::chrono::time_point<std::chrono::high_resolution_clock>::max();
int BenchmarkNumberOfRenderIteration = 0;
auto BenchmarkStart = InvalidTime;

template <typename T> T Clamp(T x, T a, T b)
{
	return x < a ? a : (x > b ? b : x);
}

std::mutex RenderMutex; // single-threading

void StopAllThreads()
{
	StopFlag = true;
}

void RenderJob(const rpr_context Context, RenderProgressCallback::Update* Update)
{
	RenderMutex.lock();
	rprContextRender(Context);
	Update->Done = 1;
	RenderMutex.unlock();
}

void HorusRadeon::ResizeFrameBufferData(int Width, int Height)
{
	if (Width <= 0 || Height <= 0)
	{
		spdlog::error("Invalid dimensions for ResizeFrameBufferData: width = {}, height = {}", Width, Height);
		return;
	}

	size_t NewSize = static_cast<size_t>(Width) * Height * 4 * sizeof(float);
	m_FbData_ = std::shared_ptr<float[]>(new float[Width * Height * 4], std::default_delete<float[]>());
	m_CurrentFramebufferSize_ = NewSize;
	spdlog::info("Framebuffer data resized to: width = {}, height = {}", Width, Height);
}

void HorusRadeon::AsyncFramebufferUpdate(const rpr_context Context, rpr_framebuffer Framebuffer)
{
	if (rpr_int Status = rprContextResolveFrameBuffer(Context, Framebuffer, m_FrameBufferDest_, false); Status != RPR_SUCCESS)
	{
		spdlog::error("RPR Error: {}", Status);

		return;
	}

	size_t FramebufferSize = 0;
	CHECK(rprFrameBufferGetInfo(m_FrameBufferDest_, RPR_FRAMEBUFFER_DATA, 0, nullptr, &FramebufferSize));

	// Check if the framebuffer size is the same as the allocated memory size
	if (FramebufferSize != m_CurrentFramebufferSize_)
	{
		spdlog::error("Mismatch in framebuffer size and allocated memory size.");
		m_FbData_ = std::shared_ptr<float[]>(new float[FramebufferSize / sizeof(float)], std::default_delete<float[]>());
		m_CurrentFramebufferSize_ = FramebufferSize;
		//return;
	}

	if (m_FbData_ == nullptr)
	{
		spdlog::error("m_FbData_ is null.");
		return;
	}

	CHECK(rprFrameBufferGetInfo(m_FrameBufferDest_, RPR_FRAMEBUFFER_DATA, FramebufferSize, m_FbData_.get(), nullptr));
}

glm::vec2 HorusRadeon::SetWindowSize(int Width, int Height)
{
	m_WindowWidth_ = Width;
	m_WindowHeight_ = Height;

	return { m_WindowWidth_, m_WindowHeight_ };
}

void HorusRadeon::CreateFrameBuffers(int width, int height)
{
	//HorusOpenGLManager& OpenGLManager = HorusOpenGLManager::GetInstance();

	m_WindowWidth_ = width;
	m_WindowHeight_ = height;

	glDeleteTextures(1, &m_RadeonTextureBuffer_);
	/*OpenGLManager.DeleteAllVAOs();
	OpenGLManager.DeleteAllVBOs();
	OpenGLManager.DeleteAllEBOs();*/

	if (m_IsAdaptativeSampling_)
	{
		CHECK(rprObjectDelete(m_FrameBufferAdaptive_));
	}

	CHECK(rprObjectDelete(m_FrameBufferA_));
	CHECK(rprObjectDelete(m_FrameBufferDest_));

	rpr_framebuffer_format Fmt = { 4, RPR_COMPONENT_TYPE_FLOAT32 };
	rpr_framebuffer_desc Desc = { static_cast<unsigned int>(m_WindowWidth_), static_cast<unsigned int>(m_WindowHeight_) };

	if (m_IsAdaptativeSampling_)
	{
		CHECK(rprContextCreateFrameBuffer(m_ContextA_, Fmt, &Desc, &m_FrameBufferAdaptive_));
	}

	CHECK(rprContextCreateFrameBuffer(m_ContextA_, Fmt, &Desc, &m_FrameBufferA_));
	CHECK(rprContextCreateFrameBuffer(m_ContextA_, Fmt, &Desc, &m_FrameBufferDest_));

	spdlog::debug("Radeon framebuffers successfully created..");
}
void HorusRadeon::ResizeRender(int width, int height)
{
	std::lock_guard Lock(RenderMutex);
	HorusOpenGL& OpenGL = HorusOpenGL::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	m_WindowWidth_ = width;
	m_WindowHeight_ = height;

	//m_IsFramebufferAreOperational_ = false;

	if (m_RenderThread_.joinable())
	{
		m_RenderThread_.join(); // Check if the render thread is joinable and join it before resizing
	}

	if (rprContextAbortRender(m_ContextA_) == RPR_SUCCESS)
	{
		spdlog::info("Render successfully interrupted for resizing.");
	}
	else
	{
		spdlog::error("Failed to interrupt the render for resizing.");
		return;
	}

	if (m_ClassicRenderFuture_.valid())
	{
		if (auto Status = m_ClassicRenderFuture_.wait_for(std::chrono::seconds(0)); Status == std::future_status::ready)
		{
			spdlog::info("Render task successfully finished for resizing.");
		}
		else
		{
			spdlog::warn("The render task is still running for resizing.");
		}
	}
	else
	{
		spdlog::info("No render task to wait for resizing.");
	}

	// Create framebuffers for the new size
	CreateFrameBuffers(m_WindowWidth_, m_WindowHeight_);

	// Set AOVs for the context
	if (m_IsAdaptativeSampling_)
	{
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_COLOR, m_FrameBufferA_));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_VARIANCE, m_FrameBufferAdaptive_));
	}
	else
	{
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_COLOR, m_FrameBufferA_));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_VARIANCE, nullptr));
	}

	// Check if m_WindowWidth_ and m_WindowHeight_ are not equal to 0 or negative
	if (m_WindowWidth_ == 0 || m_WindowHeight_ == 0)
	{
		CHECK(RPR_ERROR_INTERNAL_ERROR);
	}
	else if (m_WindowWidth_ < 0 || m_WindowHeight_ < 0)
	{
		CHECK(RPR_ERROR_INTERNAL_ERROR);
	}

	// Resize the framebuffer data to the new size of the window
	ResizeFrameBufferData(m_WindowWidth_, m_WindowHeight_);

	// Set the new size of the window to the OpenGL
	OpenGL.InitBuffers(m_WindowWidth_, m_WindowHeight_);

	// Set the new size of the window to the ImGui
	Init(m_WindowWidth_, m_WindowHeight_, m_WindowConfig_);

	// Set the new size of the window to the viewport
	CHECK(rprContextResolveFrameBuffer(m_ContextA_, m_FrameBufferA_, m_FrameBufferDest_, false));

	size_t fb_size = 0;
	CHECK(rprFrameBufferGetInfo(m_FrameBufferDest_, RPR_FRAMEBUFFER_DATA, 0, nullptr, &fb_size));

	SetWindowSize(m_WindowWidth_, m_WindowHeight_);
	ObjectManager.SetViewport(ObjectManager.GetActiveRadeonCameraId(), 0, 0, m_WindowWidth_, m_WindowHeight_);
	ObjectManager.SetSensorSize(ObjectManager.GetActiveRadeonCameraId(), 36.0f, 24.0f);
	m_SampleCount_ = 1;

	//m_IsFramebufferAreOperational_ = true;

	spdlog::info("Radeon successfully resized in : Width : {}, Height : {}", m_WindowWidth_, m_WindowHeight_);
	Console.AddLog(" [info] Radeon successfully resized in : Width : %d, Height : %d ", m_WindowWidth_, m_WindowHeight_);
}

bool HorusRadeon::Init(int width, int height, HorusWindowConfig* window)
{
	HorusOpenGL& OpenGL = HorusOpenGL::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	m_WindowWidth_ = width;
	m_WindowHeight_ = height;
	m_WindowConfig_ = window;
	m_IsDirty_ = true;

	m_RadeonTextureBuffer_ = OpenGL.GetRadeonTextureBuffer();

	spdlog::info("Radeon initialized.");
	Console.AddLog(" [info] Radeon initialized.");

	SetWindowSize(m_WindowWidth_, m_WindowHeight_);

	return true;
}
bool HorusRadeon::InitGraphics()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusEngine& Engine = HorusEngine::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();
	HorusTimerManager& TimerManager = HorusTimerManager::GetInstance();
	HorusOCIO& OCIO = HorusOCIO::GetInstance();
	HorusGarbageCollector& Gc = HorusGarbageCollector::GetInstance();

	std::lock_guard Lock(RenderMutex);

	spdlog::info("Init Radeon graphics..");
	Console.AddLog(" [info] Init Radeon graphics..");

	spdlog::info("RPR_API_VERSION : {}", RPR_API_VERSION);

	m_SampleCount_ = 1;

	const rpr_int PluginId = rprRegisterPlugin(RPR_PLUGIN_FILE_NAME);
	CHECK_NE(PluginId, -1)

		const rpr_int Plugins[] = { PluginId };
	size_t NumPlugins = std::size(Plugins);
	spdlog::info("Number of plugins : {}", NumPlugins);
	Console.AddLog(" [info] Number of plugins : %d ", NumPlugins);

	// Enable Tracing
	/*CHECK(rprContextSetParameterByKeyString(0, RPR_CONTEXT_TRACING_PATH, "logs"));
	CHECK(rprContextSetParameterByKey1u(0, RPR_CONTEXT_TRACING_ENABLED, 1));*/

	// TODO : Get Number of GPU in system

	rpr_int Status = rprCreateContext(RPR_API_VERSION, Plugins, NumPlugins,
		RPR_CREATION_FLAGS_ENABLE_GL_INTEROP | RPR_CREATION_FLAGS_ENABLE_GPU0 | RPR_CREATION_FLAGS_ENABLE_GPU1,
		g_contextProperties, nullptr, &m_ContextA_);

	// TODO : find a way for detect the number of GPU and CPU available
	// TODO : Add a way to select the GPU to use after the creation of the context
	constexpr int NumberOfGpus = 2;
	constexpr int NumberOfCpus = 0;

	CHECK(Status);

	CHECK(rprContextSetActivePlugin(m_ContextA_, Plugins[0]));
	CHECK(rprContextCreateMaterialSystem(m_ContextA_, 0, &m_Matsys_));

	spdlog::info("Rpr context successfully set active plugin..");
	spdlog::info("Rpr context successfully created..");

	Console.AddLog(" [info] Rpr context successfully set active plugin");
	Console.AddLog(" [info] Rpr context successfully created");

	ObjectManager.CreateDefaultScene();

	CHECK(rprContextSetParameterByKey1f(m_ContextA_, RPR_CONTEXT_DISPLAY_GAMMA, 2.2f));
	CHECK(rprContextSetParameterByKey1f(m_ContextA_, RPR_CONTEXT_RADIANCE_CLAMP, 2.f));

	// Set Classic Render (Default) -> Don't call the function because at the startup the engine need to render the first frame forced
	const rpr_framebuffer_desc Desc = { static_cast<unsigned int>(m_WindowWidth_), static_cast<unsigned int>(m_WindowHeight_) };
	const rpr_framebuffer_format Fmt = { 4, RPR_COMPONENT_TYPE_FLOAT32 };
	CHECK(rprContextCreateFrameBuffer(m_ContextA_, Fmt, &Desc, &m_FrameBufferA_));
	CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_COLOR, m_FrameBufferA_));

	// Set Adaptive Render 
	CHECK(rprContextCreateFrameBuffer(m_ContextA_, Fmt, &Desc, &m_FrameBufferAdaptive_));
	CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_VARIANCE, m_FrameBufferAdaptive_));

	// Create framebuffer for final render
	CHECK(rprContextCreateFrameBuffer(m_ContextA_, Fmt, &Desc, &m_FrameBufferDest_));

	CHECK(rprContextSetParameterByKeyPtr(m_ContextA_, RPR_CONTEXT_RENDER_UPDATE_CALLBACK_FUNC, (void*)RenderProgressCallback::NotifyUpdate));
	CHECK(rprContextSetParameterByKeyPtr(m_ContextA_, RPR_CONTEXT_RENDER_UPDATE_CALLBACK_DATA, &RenderProgressCallback));

	CHECK(rprContextSetParameterByKey1u(m_ContextA_, RPR_CONTEXT_ITERATIONS, 1));
	CHECK(rprContextRender(m_ContextA_));

	CHECK(rprContextSetParameterByKey1u(m_ContextA_, RPR_CONTEXT_ITERATIONS, m_MaxIterations_));
	rprContextSetParameterByKey1u(m_ContextA_, RPR_CONTEXT_MAX_RECURSION, 4);

	size_t FramebufferSize = 0;
	CHECK(rprFrameBufferGetInfo(m_FrameBufferDest_, RPR_FRAMEBUFFER_DATA, 0, nullptr, &FramebufferSize));
	m_CurrentFramebufferSize_ = FramebufferSize;

	m_FbData_ = std::shared_ptr<float[]>(new float[m_WindowWidth_ * m_WindowHeight_ * 4], std::default_delete<float[]>());

	const char OcioConfig[] = "resources/aces_1.0.3/config.ocio";
	CHECK(rprContextSetParameterByKeyString(m_ContextA_, RPR_CONTEXT_OCIO_CONFIG_PATH, OcioConfig));
	CHECK(rprContextSetParameterByKeyString(m_ContextA_, RPR_CONTEXT_OCIO_RENDERING_COLOR_SPACE, "Utility - Raw"));


	spdlog::info("Radeon graphics successfully initialized..");
	Console.AddLog(" [info] Radeon graphics successfully initialized");

	m_IsFramebufferAreOperational_ = true;

	HorusStatistics::GetInstance().InitStatistics(NumberOfGpus, NumberOfCpus);
	Engine.SetEngineIsReady(true);

	if (auto EndEngineTimer = TimerManager.StopTimer("EngineInit"); EndEngineTimer > 1000)
	{
		long long Seconds = EndEngineTimer / 1000.0;
		spdlog::info("Engine initialized in : {} seconds", Seconds);
	}
	else
	{
		spdlog::info("Engine initialized in : {} ms", EndEngineTimer);
	}

	return true;
}
void HorusRadeon::QuitRender()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusGarbageCollector& Gc = HorusGarbageCollector::GetInstance();

	spdlog::info("Unload Radeon..");
	spdlog::info("Release memory..");

	StopAllThreads();

	CHECK(rprContextAbortRender(m_ContextA_));

	ObjectManager.DestroyAllLights();
	ObjectManager.DestroyAllMaterial();
	ObjectManager.DestroyAllGroupShape();

	spdlog::info("All Radeon objects deleted..");

	glDeleteTextures(1, &m_RadeonTextureBuffer_);

	CHECK(rprObjectDelete(m_Matsys_))
		m_Matsys_ = nullptr;
	CHECK(rprObjectDelete(m_FrameBufferA_))
		m_FrameBufferA_ = nullptr;
	/*
	CHECK(rprObjectDelete(m_FrameBufferC_))
		m_FrameBufferC_ = nullptr;*/
	CHECK(rprObjectDelete(m_FrameBufferAdaptive_))
		m_FrameBufferAdaptive_ = nullptr;
	CHECK(rprObjectDelete(m_FrameBufferDest_))
		m_FrameBufferDest_ = nullptr;

	ObjectManager.DestroyAllCameras();

	Gc.Clean();

	CHECK(rprContextClearMemory(m_ContextA_));
	CheckNoLeak(m_ContextA_);
	CHECK(rprObjectDelete(m_ContextA_))
		m_ContextA_ = nullptr;

	spdlog::info("Radeon successfully unloaded..");
}

void HorusRadeon::SetPreviewMode(bool Enable)
{
	m_IsPreviewEnabled_ = Enable;
	rprContextSetParameterByKey1u(m_ContextA_, RPR_CONTEXT_PREVIEW, Enable);
	spdlog::debug("Preview mode is: {}", Enable);
}
void HorusRadeon::SetLockPreviewMode(bool Enable)
{
	if (Enable)
	{
		m_MaxSamplesTemp_ = m_MaxSamples_;
		m_MaxSamples_ = 8;
	}
	else
	{
		m_MaxSamples_ = m_MaxSamplesTemp_;
	}

	m_IsLockPreviewEnabled_ = Enable;
	spdlog::debug("Lock preview mode is: {}", Enable);
}

void HorusRadeon::RenderEngine()
{
	RenderClassic();

	/*if (m_IsAdaptativeSampling_)
	{
		RenderAdaptive();
	}
	else
	{

	}*/
}
void HorusRadeon::RenderClassic()
{
	if (!m_LockingRender_)
	{
		if (!m_IsDirty_ && m_MaxSamples_ != -1 && m_SampleCount_ >= m_MaxSamples_) {
			return;
		}

		constexpr int MinSamplesToSwitch = 3;

		// Benchmark
		{
			const auto TimeUpdateStarts = std::chrono::high_resolution_clock::now();

			if (BenchmarkStart == InvalidTime)
				BenchmarkStart = TimeUpdateStarts;
			if (BenchmarkNumberOfRenderIteration >= 100 && m_IsDirty_)
			{
				double ElapsedTimeMs = std::chrono::duration<double, std::milli>(TimeUpdateStarts - BenchmarkStart).count();
				double RenderPerSecond = static_cast<double>(BenchmarkNumberOfRenderIteration) * 1000.0 / ElapsedTimeMs;
				spdlog::info("Rendering -> {} iterations per second.", RenderPerSecond);
				BenchmarkNumberOfRenderIteration = 0;
				BenchmarkStart = TimeUpdateStarts;
			}
		}

		RenderProgressCallback.Clear();

		if (m_IsDirty_ /*&& m_IsFramebufferAreOperational_*/)
		{
			if (m_SampleCount_ <= MinSamplesToSwitch)
			{
				m_ClassicRenderFuture_ = std::async(std::launch::async, &RenderJob, m_ContextA_, &RenderProgressCallback); // Simple Future
			}
			/*if (m_SampleCount_ <= MinSamplesToSwitch)
			{
				m_ClassicRenderFuture_ = std::async(std::launch::async, RenderJob, m_ContextA_, &RenderProgressCallback);
			}*/

			else if (m_SampleCount_ >= MinSamplesToSwitch + 1)
			{
				m_RenderThread_ = std::jthread(RenderJob, m_ContextA_, &RenderProgressCallback);
				m_RenderThread_.detach();
			}

			if (m_SampleCount_ <= MinSamplesToSwitch && !m_IsLockPreviewEnabled_)
			{
				if (!m_IsPreviewEnabled_)
				{
					SetPreviewMode(true);
				}
			}
			else if (m_SampleCount_ >= MinSamplesToSwitch + 1 && !m_IsLockPreviewEnabled_)
			{
				if (m_IsPreviewEnabled_)
				{
					SetPreviewMode(false);
				}
			}
			else if (m_IsLockPreviewEnabled_)
			{
				if (!m_IsPreviewEnabled_)
				{
					SetPreviewMode(true);
				}
			}

			m_ThreadRunning_ = true;
		}

		while (!RenderProgressCallback.Done)
		{
			if (!m_IsDirty_ && m_MaxSamples_ != -1 && m_SampleCount_ >= m_MaxSamples_) {
				break;
			}

			if (RenderProgressCallback.HasUpdate)
			{
				RenderMutex.lock();
				m_SampleCount_++;

				m_ClassicRenderInfoFuture_ = std::async(std::launch::async, &HorusRadeon::AsyncFramebufferUpdate, this, m_ContextA_, m_FrameBufferA_);

				glBindTexture(GL_TEXTURE_2D, m_RadeonTextureBuffer_);
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_WindowWidth_, m_WindowHeight_, GL_RGBA, GL_FLOAT, static_cast<GLvoid*>(m_FbData_.get()));
				glBindTexture(GL_TEXTURE_2D, 0);

				RenderProgressCallback.HasUpdate = false;

				RenderMutex.unlock();
				break;
			}
		}

		if (m_ThreadRunning_ /*&& m_IsFramebufferAreOperational_*/)
		{
			if (m_SampleCount_ <= MinSamplesToSwitch || m_SampleCount_ >= m_MaxSamples_ && m_ClassicRenderFuture_.valid() && m_ClassicRenderInfoFuture_.valid())
			{
				m_ClassicRenderFuture_.wait(); // Simple Future Wait
				m_ClassicRenderInfoFuture_.wait();
			}

			m_ThreadRunning_ = false;
		}
		else
		{
			m_ThreadRunning_ = false;
		}


		GetClassicRenderProgress();
		BenchmarkNumberOfRenderIteration += m_BatchSize_;
	}
}
void HorusRadeon::RenderAdaptive()
{
	if (!m_LockingRender_)
	{
		if (!m_IsDirty_ && (m_IsAdaptativeSampling_ && m_ActivePixelRemains_ == 0))
		{
			return;
		}

		// Benchmark
		{
			const auto TimeUpdateStarts = std::chrono::high_resolution_clock::now();

			if (BenchmarkStart == InvalidTime)
				BenchmarkStart = TimeUpdateStarts;
			if (BenchmarkNumberOfRenderIteration >= 100 && m_IsDirty_)
			{
				double ElapsedTimeMs = std::chrono::duration<double, std::milli>(TimeUpdateStarts - BenchmarkStart).count();
				double RenderPerSecond = static_cast<double>(BenchmarkNumberOfRenderIteration) * 1000.0 / ElapsedTimeMs;
				spdlog::info("Adaptive rendering -> {} iterations per second.", RenderPerSecond);
				BenchmarkNumberOfRenderIteration = 0;
				BenchmarkStart = TimeUpdateStarts;
			}
		}

		RenderProgressCallback.Clear();

		if (m_IsDirty_)
		{
			//m_AdaptiveRenderFuture_ = std::thread(&HorusRadeon::RenderJob, this, m_ContextA_,std::ref( m_RenderProgressCallback_));
			//m_AdaptiveRenderFuture_.detach();
			m_ThreadRunning_ = true;
		}

		while (!RenderProgressCallback.Done)
		{
			if (!m_IsDirty_ && (m_IsAdaptativeSampling_ && m_ActivePixelRemains_ == 0))
			{
				break;
			}

			if (RenderProgressCallback.HasUpdate)
			{
				RenderMutex.lock();

				//m_AdaptiveRenderInfoFuture_ = std::thread(&HorusRadeon::AsyncFramebufferUpdate, this, m_ContextA_, m_FrameBufferAdaptive_);
				//m_AdaptiveRenderInfoFuture_.detach();

				glBindTexture(GL_TEXTURE_2D, m_RadeonTextureBuffer_);
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_WindowWidth_, m_WindowHeight_, GL_RGBA, GL_FLOAT, static_cast<const GLvoid*>(m_FbData_.get()));
				glBindTexture(GL_TEXTURE_2D, 0);

				RenderProgressCallback.HasUpdate = false;

				RenderMutex.unlock();
				break;
			}
		}

		if (m_ThreadRunning_)
		{
			//m_AdaptiveRenderFuture_.wait();
			//m_AdaptiveRenderInfoFuture_.wait();
			m_ThreadRunning_ = false;
		}

		GetAdaptiveRenderProgress();

		BenchmarkNumberOfRenderIteration += m_BatchSize_;
	}
}

void HorusRadeon::SetClassicRender()
{
	HorusUI& Ui = HorusUI::GetInstance();
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusTimerManager::GetInstance().ResetTimer("RenderTimer");

	m_IsAdaptativeSampling_ = false;

	// Clean Adaptive Render
	SetActivePixelRemains(GetWindowSize().x / GetWindowSize().y);
	CHECK(rprFrameBufferClear(GetAdaptiveRenderFrameBuffer()));

	ObjectManager.SetSensorSize(ObjectManager.GetActiveRadeonCameraId(), 36.0f, 24.0f);


	//CHECK(rprContextSetParameterByKey1u(m_ContextA_, RPR_CONTEXT_SAMPLER_TYPE, RPR_CONTEXT_SAMPLER_TYPE_SOBOL));

	ResizeRender(GetWindowSize().x, GetWindowSize().y);

	CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_VARIANCE, nullptr));

	CHECK(rprContextSetParameterByKey1u(m_ContextA_, RPR_CONTEXT_ITERATIONS, m_MaxIterations_));

	CHECK(rprContextSetParameterByKey1u(m_ContextA_, RPR_CONTEXT_ADAPTIVE_SAMPLING_TILE_SIZE, 0));
	CHECK(rprContextSetParameterByKey1u(m_ContextA_, RPR_CONTEXT_ADAPTIVE_SAMPLING_MIN_SPP, 0));
	CHECK(rprContextSetParameterByKey1f(m_ContextA_, RPR_CONTEXT_ADAPTIVE_SAMPLING_THRESHOLD, 0));

	Ui.SetOptionsChanged(true);
	SetIsDirty(true);
	SetSampleCount(1);
	CHECK(rprFrameBufferClear(GetClassicRenderFrameBuffer()));

	spdlog::debug("Set Classic Render : {} x {} : Set", m_WindowWidth_, m_WindowHeight_);
}
void HorusRadeon::SetAdaptiveRender()
{
	HorusUI& Ui = HorusUI::GetInstance();
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusTimerManager::GetInstance().ResetTimer("RenderTimer");

	m_IsAdaptativeSampling_ = true;

	// Clean Classic Render
	SetSampleCount(1);
	CHECK(rprFrameBufferClear(GetClassicRenderFrameBuffer()));

	// Change sampler
	//CHECK(rprContextSetParameterByKey1f(m_ContextA_, RPR_CONTEXT_SAMPLER_TYPE, RPR_CONTEXT_SAMPLER_TYPE_CMJ));

	// Set Aspect Ratio
	ObjectManager.SetSensorSize(ObjectManager.GetActiveRadeonCameraId(), 36.0f, 24.0f);

	ResizeRender(GetWindowSize().x, GetWindowSize().y);

	CHECK(rprContextSetParameterByKey1u(m_ContextA_, RPR_CONTEXT_ADAPTIVE_SAMPLING_TILE_SIZE, m_MinAdaptiveTileSize_)); // Recommanded value : 4 ~ 16
	CHECK(rprContextSetParameterByKey1u(m_ContextA_, RPR_CONTEXT_ADAPTIVE_SAMPLING_MIN_SPP, m_MinSamplesAdaptive_));
	CHECK(rprContextSetParameterByKey1f(m_ContextA_, RPR_CONTEXT_ADAPTIVE_SAMPLING_THRESHOLD, m_AdaptiveThreshold_)); // Recommanded value : 0.005 ~ 0.1 -> Max 1
	CHECK(rprContextSetParameterByKey1u(m_ContextA_, RPR_CONTEXT_ITERATIONS, m_MaxIterationsPerSamples_));

	Ui.SetOptionsChanged(true);
	SetIsDirty(true);
	SetActivePixelRemains(m_WindowWidth_ * m_WindowHeight_);
	CHECK(rprFrameBufferClear(GetAdaptiveRenderFrameBuffer()));

	spdlog::debug("Set Adaptive Render : {} x {} : Set", m_WindowWidth_, m_WindowHeight_);

}

void HorusRadeon::CallRenderMultiTiles(int Tile_Size, int MaxIterationPerTiles)
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();
	HorusViewportRadeon& RadeonViewport = HorusViewportRadeon::GetInstance();

	HorusFrameBufferMetadata FbMetadata;
	FbMetadata.RenderTargetSizeX = m_WindowWidth_;
	FbMetadata.RenderTargetSizeY = m_WindowHeight_;

	FbMetadata.TileSizeX = Tile_Size;
	FbMetadata.TileSizeY = Tile_Size;

	/*std::jthread RenderThread(&RenderMultiTiles, FbMetadata, ObjectManager.GetScene(), m_ContextA_, MaxIterationPerTiles);
	RenderThread.detach();*/
	RenderMultiTiles(FbMetadata, ObjectManager.GetScene(), m_ContextA_, MaxIterationPerTiles);

	if (!stbi_write_png("FinalRender.png", m_WindowWidth_, m_WindowHeight_, 3, FbMetadata.FbData.data(), m_WindowWidth_ * 3))
	{
		spdlog::error("Failed to save the final render image.");
		Console.AddLog(" [error] Failed to save the final render image.");
	}
	else
	{
		spdlog::info("Final Render : {} x {} : Saved", m_WindowWidth_, m_WindowHeight_);
		Console.AddLog(" [success] Final Render : %d x %d : Saved", m_WindowWidth_, m_WindowHeight_);
	}

	RadeonViewport.SetViewportLocked(false);
	RadeonViewport.SetIsFullRender(false);

	ResizeRender(m_WindowWidth_, m_WindowHeight_);
}
void HorusRadeon::RenderMultiTiles(HorusFrameBufferMetadata& FbMetadata, rpr_scene, const rpr_context Context, const rpr_uint MaxIterationRendering)
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	rprCameraGetInfo(m_Camera_, RPR_CAMERA_LENS_SHIFT, sizeof(m_PreviousLensShift_), &m_PreviousLensShift_, nullptr);

	CHECK_GT(FbMetadata.RenderTargetSizeX, FbMetadata.TileSizeX);
	CHECK_GT(FbMetadata.RenderTargetSizeY, FbMetadata.TileSizeY);

	FbMetadata.FbData.resize(static_cast<std::vector<rpr_uchar, std::allocator<rpr_uchar>>::size_type>(FbMetadata.RenderTargetSizeX) * FbMetadata.RenderTargetSizeY * 3);

	float TileSizeXf = FbMetadata.RenderTargetSizeX / static_cast<float>(FbMetadata.TileSizeX);
	float TileSizeYf = FbMetadata.RenderTargetSizeY / static_cast<float>(FbMetadata.TileSizeY);

	const int TileSizeX = static_cast<int>(ceil(TileSizeXf));
	const int TileSizeY = static_cast<int>(ceil(TileSizeYf));

	spdlog::info("RenderMultiTiles : ");
	spdlog::info("Virtual resolution : {} x {}", FbMetadata.RenderTargetSizeX, FbMetadata.RenderTargetSizeY);
	spdlog::info("Tile resolution : {} x {}", FbMetadata.TileSizeX, FbMetadata.TileSizeY);
	spdlog::info("Tiled resolution : {} x {}", TileSizeXf, TileSizeYf);
	spdlog::info("Tiled offset : {} x {}", -TileSizeXf / 2.0f + 0.5f, -TileSizeYf / 2.0f + 0.5f);

	const rpr_framebuffer_desc Desc = { static_cast<rpr_uint>(FbMetadata.TileSizeX), static_cast<rpr_uint>(FbMetadata.TileSizeY) };
	const rpr_framebuffer_format Fmt = { 4, RPR_COMPONENT_TYPE_FLOAT32 };

	rpr_framebuffer FrameBuffer = nullptr; rpr_int Status = rprContextCreateFrameBuffer(Context, Fmt, &Desc, &FrameBuffer); CHECK(Status)
		rpr_framebuffer FrameBufferResolved = nullptr; Status = rprContextCreateFrameBuffer(Context, Fmt, &Desc, &FrameBufferResolved); CHECK(Status)
		Status = rprContextSetAOV(Context, RPR_AOV_COLOR, FrameBuffer); CHECK(Status)

		size_t FrameBufferDatasize = 0;
	Status = rprFrameBufferGetInfo(FrameBuffer, RPR_FRAMEBUFFER_DATA, 0, nullptr, &FrameBufferDatasize); CHECK(Status)
		CHECK_EQ(FrameBufferDatasize, FbMetadata.TileSizeX * FbMetadata.TileSizeY * 4 * sizeof(float))
		auto FrameBufferData = static_cast<float*>(malloc(FrameBufferDatasize));

	rpr_camera camera = nullptr;
	Status = rprSceneGetCamera(ObjectManager.GetScene(), &camera); CHECK(Status)
		//status = rprCameraSetSensorSize(camera, fb_metadata.RenderTargetSizeX, fb_metadata.RenderTargetSizeY); CHECK(status);

		float ShiftY = -(TileSizeYf / 2.0f) + 0.5f;
	for (int YTiles = 0; YTiles < TileSizeY; YTiles++)
	{
		const float DeltaY = 1.0f;
		float ShiftX = -(TileSizeXf / 2.0f) + 0.5f;

		for (int XTiles = 0; XTiles < TileSizeX; XTiles++)
		{
			const float DeltaX = 1.0f;
			Status = rprCameraSetLensShift(camera, ShiftX, ShiftY); CHECK(Status);
			Status = rprFrameBufferClear(FrameBuffer); CHECK(Status);

			for (rpr_uint i = 0; i < MaxIterationRendering; i++)
			{
				Status = rprContextSetParameterByKey1u(Context, RPR_CONTEXT_FRAMECOUNT, i); CHECK(Status);
				Status = rprContextRender(Context); CHECK(Status);
			}

			Status = rprContextResolveFrameBuffer(Context, FrameBuffer, FrameBufferResolved, false); CHECK(Status);
			Status = rprFrameBufferGetInfo(FrameBufferResolved, RPR_FRAMEBUFFER_DATA, FrameBufferDatasize, FrameBufferData, nullptr); CHECK(Status);

			const int OffsetInRenderTargetX = XTiles * FbMetadata.TileSizeX;
			const int OffsetInRenderTargetY = YTiles * FbMetadata.TileSizeY;

			for (int J = 0; J < FbMetadata.TileSizeY; J++)
			{
				for (int i = 0; i < FbMetadata.TileSizeX; i++)
				{
					const int DstX = OffsetInRenderTargetX + i;
					const int DstY = OffsetInRenderTargetY + J;

					if (DstX >= FbMetadata.RenderTargetSizeX || DstY >= FbMetadata.RenderTargetSizeY)
						continue;

					const int Idx = DstX + (FbMetadata.RenderTargetSizeY - DstY - 1) * FbMetadata.RenderTargetSizeX;
					const int SIdx = i + (FbMetadata.TileSizeY - J - 1) * FbMetadata.TileSizeX;

					[[maybe_unused]] float INVW = 1.0f / FrameBufferData[4 * SIdx + 3];

					FbMetadata.FbData[3 * Idx + 0] = Clamp(static_cast<int>(FrameBufferData[4 * SIdx] * 255.f), 0, 255);
					FbMetadata.FbData[3 * Idx + 1] = Clamp(static_cast<int>(FrameBufferData[4 * SIdx + 1] * 255.f), 0, 255);
					FbMetadata.FbData[3 * Idx + 2] = Clamp(static_cast<int>(FrameBufferData[4 * SIdx + 2] * 255.f), 0, 255);
				}
			}
			ShiftX += DeltaX;
		}
		ShiftY += DeltaY;
	}

	Status = rprCameraSetLensShift(camera, m_PreviousLensShift_, m_PreviousLensShift_); CHECK(Status)

		free(FrameBufferData); FrameBufferData = nullptr;
	if (FrameBuffer) {
		Status = rprObjectDelete(FrameBuffer); CHECK(Status)
			FrameBuffer = nullptr;
	}
	if (FrameBufferResolved) {
		Status = rprObjectDelete(FrameBufferResolved);  CHECK(Status)
			FrameBufferResolved = nullptr;
	}
}

void HorusRadeon::SetVisualizationRenderMode(int Mode)
{
	switch (Mode)
	{
	case 0:
		CHECK(rprContextSetParameterByKey1u(m_ContextA_, RPR_CONTEXT_RENDER_MODE, RPR_RENDER_MODE_DIFFUSE));
		break;
	case 1:
		CHECK(rprContextSetParameterByKey1u(m_ContextA_, RPR_CONTEXT_RENDER_MODE, RPR_RENDER_MODE_GLOBAL_ILLUMINATION));
		break;
	case 2:
		CHECK(rprContextSetParameterByKey1u(m_ContextA_, RPR_CONTEXT_RENDER_MODE, RPR_RENDER_MODE_DIRECT_ILLUMINATION));
		break;
	case 3:
		CHECK(rprContextSetParameterByKey1u(m_ContextA_, RPR_CONTEXT_RENDER_MODE, RPR_RENDER_MODE_DIRECT_ILLUMINATION_NO_SHADOW));
		break;
	case 4:
		CHECK(rprContextSetParameterByKey1u(m_ContextA_, RPR_CONTEXT_RENDER_MODE, RPR_RENDER_MODE_WIREFRAME));
		break;
	case 5:
		CHECK(rprContextSetParameterByKey1u(m_ContextA_, RPR_CONTEXT_RENDER_MODE, RPR_RENDER_MODE_MATERIAL_INDEX));
		break;
	case 6:
		CHECK(rprContextSetParameterByKey1u(m_ContextA_, RPR_CONTEXT_RENDER_MODE, RPR_RENDER_MODE_POSITION));
		break;
	case 7:
		CHECK(rprContextSetParameterByKey1u(m_ContextA_, RPR_CONTEXT_RENDER_MODE, RPR_RENDER_MODE_NORMAL));
		break;
	case 8:
		CHECK(rprContextSetParameterByKey1u(m_ContextA_, RPR_CONTEXT_RENDER_MODE, RPR_RENDER_MODE_TEXCOORD));
		break;
	case 9:
		CHECK(rprContextSetParameterByKey1u(m_ContextA_, RPR_CONTEXT_RENDER_MODE, RPR_RENDER_MODE_AMBIENT_OCCLUSION));
		break;
	}

	m_SelectedRenderVizMode_ = Mode;
}
void HorusRadeon::SetShowAOVsMode(int AOV)
{
	{
		// Don't make Color AOV null because it's needed for the render
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_COLOR, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_OPACITY, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_WORLD_COORDINATE, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_UV, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_MATERIAL_ID, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_GEOMETRIC_NORMAL, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_SHADING_NORMAL, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_DEPTH, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_OBJECT_ID, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_OBJECT_GROUP_ID, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_SHADOW_CATCHER, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_BACKGROUND, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_EMISSION, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_VELOCITY, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_DIRECT_ILLUMINATION, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_INDIRECT_ILLUMINATION, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_AO, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_DIRECT_DIFFUSE, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_DIRECT_REFLECT, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_INDIRECT_DIFFUSE, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_INDIRECT_REFLECT, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_REFRACT, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_VOLUME, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_DIFFUSE_ALBEDO, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_VARIANCE, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_VIEW_SHADING_NORMAL, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_REFLECTION_CATCHER, nullptr));
		//rprContextSetAOV(m_ContextA_, RPR_AOV_COLOR_RIGHT, nullptr); // Not Needed for now
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_LIGHT_GROUP0, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_LIGHT_GROUP1, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_LIGHT_GROUP2, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_LIGHT_GROUP3, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_LIGHT_GROUP4, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_LIGHT_GROUP5, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_LIGHT_GROUP6, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_LIGHT_GROUP7, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_LIGHT_GROUP8, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_LIGHT_GROUP9, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_LIGHT_GROUP10, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_LIGHT_GROUP11, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_LIGHT_GROUP12, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_LIGHT_GROUP13, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_LIGHT_GROUP14, nullptr));
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_LIGHT_GROUP15, nullptr));
	}

	bool NoAoVs = false;

	switch (AOV)
	{
	case 0:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_COLOR, m_FrameBufferA_));
		NoAoVs = true;
		break;
	case 1:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_OPACITY, m_FrameBufferA_));
		break;
	case 2:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_WORLD_COORDINATE, m_FrameBufferA_));
		break;
	case 3:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_UV, m_FrameBufferA_));
		break;
	case 4:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_MATERIAL_ID, m_FrameBufferA_));
		break;
	case 5:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_GEOMETRIC_NORMAL, m_FrameBufferA_));
		break;
	case 6:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_SHADING_NORMAL, m_FrameBufferA_));
		break;
	case 7:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_DEPTH, m_FrameBufferA_));
		break;
	case 8:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_OBJECT_ID, m_FrameBufferA_));
		break;
	case 9:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_OBJECT_GROUP_ID, m_FrameBufferA_));
		break;
	case 10:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_SHADOW_CATCHER, m_FrameBufferA_));
		break;
	case 11:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_BACKGROUND, m_FrameBufferA_));
		break;
	case 12:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_EMISSION, m_FrameBufferA_));
		break;
	case 13:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_VELOCITY, m_FrameBufferA_));
		break;
	case 14:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_DIRECT_ILLUMINATION, m_FrameBufferA_));
		break;
	case 15:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_INDIRECT_ILLUMINATION, m_FrameBufferA_));
		break;
	case 16:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_AO, m_FrameBufferA_));
		break;
	case 17:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_DIRECT_DIFFUSE, m_FrameBufferA_));
		break;
	case 18:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_DIRECT_REFLECT, m_FrameBufferA_));
		break;
	case 19:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_INDIRECT_DIFFUSE, m_FrameBufferA_));
		break;
	case 20:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_INDIRECT_REFLECT, m_FrameBufferA_));
		break;
	case 21:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_REFRACT, m_FrameBufferA_));
		break;
	case 22:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_VOLUME, m_FrameBufferA_));
		break;
	case 23:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_DIFFUSE_ALBEDO, m_FrameBufferA_));
		break;
	case 24:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_VARIANCE, m_FrameBufferA_));
		break;
	case 25:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_VIEW_SHADING_NORMAL, m_FrameBufferA_));
		break;
	case 26:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_REFLECTION_CATCHER, m_FrameBufferA_));
		break;
	case 27:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_LIGHT_GROUP0, m_FrameBufferA_));
		break;
	case 28:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_LIGHT_GROUP1, m_FrameBufferA_));
		break;
	case 29:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_LIGHT_GROUP2, m_FrameBufferA_));
		break;
	case 30:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_LIGHT_GROUP3, m_FrameBufferA_));
		break;
	case 31:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_LIGHT_GROUP4, m_FrameBufferA_));
		break;
	case 32:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_LIGHT_GROUP5, m_FrameBufferA_));
		break;
	case 33:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_LIGHT_GROUP6, m_FrameBufferA_));
		break;
	case 34:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_LIGHT_GROUP7, m_FrameBufferA_));
		break;
	case 35:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_LIGHT_GROUP8, m_FrameBufferA_));
		break;
	case 36:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_LIGHT_GROUP9, m_FrameBufferA_));
		break;
	case 37:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_LIGHT_GROUP10, m_FrameBufferA_));
		break;
	case 38:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_LIGHT_GROUP11, m_FrameBufferA_));
		break;
	case 39:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_LIGHT_GROUP12, m_FrameBufferA_));
		break;
	case 40:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_LIGHT_GROUP13, m_FrameBufferA_));
		break;
	case 41:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_LIGHT_GROUP14, m_FrameBufferA_));
		break;
	case 42:
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_LIGHT_GROUP15, m_FrameBufferA_));
		break;
	}

	// for show AOVs before color AOV
	if (!NoAoVs)
	{
		CHECK(rprContextSetAOV(m_ContextA_, RPR_AOV_COLOR, m_FrameBufferA_));
	}
}

float HorusRadeon::GetClassicRenderProgress()
{
	int MaxSpp = m_MaxSamples_;
	int Progress = MaxSpp <= 0 ? 0 : m_SampleCount_ * 100.0f / MaxSpp;

	if (Progress >= 100)
	{
		m_IsDirty_ = false;
	}

	return Progress;
}
float HorusRadeon::GetAdaptiveRenderProgress()
{
	spdlog::info("GetAdaptiveRenderProgress : {}", m_ActivePixelRemains_);
	CHECK(rprContextGetInfo(m_ContextA_, RPR_CONTEXT_ACTIVE_PIXEL_COUNT, sizeof(m_ActivePixelRemains_), &m_ActivePixelRemains_, nullptr));

	int TotalPixel = m_WindowWidth_ * m_WindowHeight_;
	float Progress = TotalPixel <= 0 ? 0 : (TotalPixel - m_ActivePixelRemains_) * 100.0f / TotalPixel;

	return Progress;
}
