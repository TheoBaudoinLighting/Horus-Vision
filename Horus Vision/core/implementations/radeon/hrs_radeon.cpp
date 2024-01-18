
#include "hrs_engine.h"
#include "hrs_object_manager.h"
#include "hrs_importer.h"
#include "hrs_scene.h"
#include "hrs_radeon.h"

#include <cassert>
#include <iostream>
#include <thread>
#include <memory>
#include <array>
#include <chrono>
#include <future>
#include <array>
#include <vector>

#include <Math/mathutils.h>

#include <tbb/task_group.h>

#include "spdlog/spdlog.h"
#include "ImGuizmo.h"
#include <hrs_console.h>
#include <hrs_viewport.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stbi/stb_image_write.h"

std::mutex RenderMutex;

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

const auto InvalidTime = std::chrono::time_point<std::chrono::high_resolution_clock>::max();
int BenchmarkNumberOfRenderIteration = 0;
auto BenchmarkStart = InvalidTime;

template <typename T> T Clamp(T x, T a, T b)
{
	return x < a ? a : (x > b ? b : x);
}

void RenderJob(rpr_context Ctxt, RenderProgressCallback::Update* Update)
{
	RenderMutex.lock();
	CHECK(rprContextRender(Ctxt))
	//rprContextRender(ctxt);
	Update->Done = 1;
	RenderMutex.unlock();
}

glm::vec2 HorusRadeon::SetWindowSize(int width, int height)
{
	m_WindowWidth_ = width;
	m_WindowHeight_ = height;

	return {m_WindowWidth_, m_WindowHeight_};
}

void HorusRadeon::CreateFrameBuffers(int width, int height)
{
	HorusOpenGLManager& OpenGLManager = HorusOpenGLManager::GetInstance();

	m_WindowWidth_ = width;
	m_WindowHeight_ = height;

	glDeleteTextures(1, &m_TextureBuffer_);
	OpenGLManager.DeleteAllVAOs();
	OpenGLManager.DeleteAllVBOs();
	OpenGLManager.DeleteAllEBOs();

	CHECK(rprObjectDelete(m_FrameBuffer_))
	CHECK(rprObjectDelete(m_FrameBuffer2_))

	rpr_framebuffer_format Fmt = { 4, RPR_COMPONENT_TYPE_FLOAT32 };
	rpr_framebuffer_desc Desc = { static_cast<unsigned int>(m_WindowWidth_), static_cast<unsigned int>(m_WindowHeight_) };

	CHECK(rprContextCreateFrameBuffer(m_Context_, Fmt, &Desc, &m_FrameBuffer_))
	CHECK(rprContextCreateFrameBuffer(m_Context_, Fmt, &Desc, &m_FrameBuffer2_))
}

bool HorusRadeon::Init(int width, int height, HorusWindowConfig* window)
{
	HorusOpenGL& OpenGL = HorusOpenGL::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	m_WindowWidth_ = width;
	m_WindowHeight_ = height;
	m_WindowConfig_ = window;
	m_IsDirty_ = true;

	m_TextureBuffer_ = OpenGL.GetTextureBuffer();

	spdlog::info("Radeon initialized.");
	Console.AddLog(" [info] Radeon initialized.");

	SetWindowSize(m_WindowWidth_, m_WindowHeight_);

	return true;
}

void HorusRadeon::QuitRender()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusGarbageCollector& Gc = HorusGarbageCollector::GetInstance();

	spdlog::info("Unload Radeon..");
	spdlog::info("Release memory..");

	ObjectManager.DestroyAllMeshes();
	ObjectManager.DestroyAllMaterial();
	ObjectManager.DestroyAllMaterialEditors();
	ObjectManager.DestroyAllLights();

	spdlog::info("All Radeon objects deleted..");

	glDeleteTextures(1, &m_TextureBuffer_);

	CHECK(rprObjectDelete(m_Matsys_))
	m_Matsys_ = nullptr;
	CHECK(rprObjectDelete(m_FrameBuffer_))
	m_FrameBuffer_ = nullptr;
	CHECK(rprObjectDelete(m_FrameBuffer2_))
	m_FrameBuffer2_ = nullptr;

	ObjectManager.DestroyAllScenes();

	Gc.Clean();

	ObjectManager.DestroyAllCameras();

	rprContextClearMemory(m_Context_);
	CheckNoLeak(m_Context_);
	CHECK(rprObjectDelete(m_Context_))
	m_Context_ = nullptr;

	spdlog::info("Radeon successfully unloaded..");
}

bool HorusRadeon::InitGraphics()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusEngine& Engine = HorusEngine::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	spdlog::info("Init Radeon graphics..");
	Console.AddLog(" [info] Init Radeon graphics..");

	spdlog::info("RPR_API_VERSION : {}", RPR_API_VERSION);

	rpr_int PluginId = rprRegisterPlugin(RPR_PLUGIN_FILE_NAME);
	CHECK_NE(PluginId, -1)

	rpr_int Plugins[] = { PluginId };
	size_t NumPlugins = std::size(Plugins);
	spdlog::info("Number of plugins : {}", NumPlugins);
	Console.AddLog(" [info] Number of plugins : %d ", NumPlugins);

	rpr_int Status = rprCreateContext(RPR_API_VERSION, Plugins, NumPlugins,
	                                  RPR_CREATION_FLAGS_ENABLE_GL_INTEROP | RPR_CREATION_FLAGS_ENABLE_GPU0 |
	                                  RPR_CREATION_FLAGS_ENABLE_GPU1 | RPR_CREATION_FLAGS_ENABLE_CPU,
	                                  g_contextProperties, nullptr, &m_Context_);

	CHECK(Status)

	CHECK(rprContextSetActivePlugin(m_Context_, Plugins[0]))
	CHECK(rprContextCreateMaterialSystem(m_Context_, 0, &m_Matsys_))

	spdlog::info("Rpr context successfully set active plugin..");
	spdlog::info("Rpr context successfully created..");

	Console.AddLog(" [info] Rpr context successfully set active plugin");
	Console.AddLog(" [info] Rpr context successfully created");

	ObjectManager.CreateDefaultScene(); // Create Default Engine Scene (Plane, Dragon, Light, Camera)

	m_SampleCount_ = 1;

	CHECK(rprContextSetParameterByKey1f(m_Context_, RPR_CONTEXT_DISPLAY_GAMMA, 2.2f))

	rpr_framebuffer_desc Desc = { static_cast<unsigned int>(m_WindowWidth_), static_cast<unsigned int>(m_WindowHeight_) };
	rpr_framebuffer_format Fmt = { 4, RPR_COMPONENT_TYPE_FLOAT32 };

	CHECK(rprContextCreateFrameBuffer(m_Context_, Fmt, &Desc, &m_FrameBuffer_))
	CHECK(rprContextCreateFrameBuffer(m_Context_, Fmt, &Desc, &m_FrameBuffer2_))

	CHECK(rprContextSetAOV(m_Context_, RPR_AOV_COLOR, m_FrameBuffer_))

	CHECK(rprContextSetParameterByKeyPtr(m_Context_, RPR_CONTEXT_RENDER_UPDATE_CALLBACK_FUNC, (void*)RenderProgressCallback::NotifyUpdate))
	CHECK(rprContextSetParameterByKeyPtr(m_Context_, RPR_CONTEXT_RENDER_UPDATE_CALLBACK_DATA, &RenderProgressCallback))

	CHECK(rprContextSetParameterByKey1u(m_Context_, RPR_CONTEXT_ITERATIONS, 1))

	std::thread RenderThread([&]() {
		CHECK(rprContextRender(m_Context_))
	});
	RenderThread.join();

	CHECK(rprContextSetParameterByKey1u(m_Context_, RPR_CONTEXT_ITERATIONS, m_BatchSize_))

	m_FbData_ = std::shared_ptr<float>(new float[m_WindowWidth_ * m_WindowHeight_ * 4], std::default_delete<float[]>());

	spdlog::info("Radeon graphics successfully initialized..");
	Console.AddLog(" [info] Radeon graphics successfully initialized");

	Engine.SetEngineIsReady(true);

	return true;
}

void HorusRadeon::ResizeRender(int width, int height)
{
	HorusOpenGL& OpenGL = HorusOpenGL::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	m_WindowWidth_ = width;
	m_WindowHeight_ = height;

	glViewport(0, 0, m_WindowWidth_, m_WindowHeight_);

	m_FbData_ = nullptr;

	CreateFrameBuffers(m_WindowWidth_, m_WindowHeight_);

	CHECK(rprContextSetAOV(m_Context_, RPR_AOV_COLOR, m_FrameBuffer_))

	m_FbData_ = std::shared_ptr<float>(new float[m_WindowWidth_ * m_WindowHeight_ * 4], std::default_delete<float[]>());


	spdlog::info("Before init buffers 01 and 02");
	OpenGL.InitBuffers(m_WindowWidth_, m_WindowHeight_);

	spdlog::info("after init buffers 01");
	Init(m_WindowWidth_, m_WindowHeight_, m_WindowConfig_);

	spdlog::info("after init buffers 02");

	CHECK(rprContextResolveFrameBuffer(m_Context_, m_FrameBuffer_, m_FrameBuffer2_, false))
	size_t fb_size = 0;
	CHECK(rprFrameBufferGetInfo(m_FrameBuffer2_, RPR_FRAMEBUFFER_DATA, 0, nullptr, &fb_size))

	SetWindowSize(m_WindowWidth_, m_WindowHeight_);
	ObjectManager.SetViewport(ObjectManager.GetActiveCameraId(), 0, 0, m_WindowWidth_, m_WindowHeight_);
	m_SampleCount_ = 1;

	spdlog::info("Radeon successfully resized in : Width : {}, Height : {}", m_WindowWidth_, m_WindowHeight_);
	Console.AddLog(" [info] Radeon successfully resized in : Width : %d, Height : %d ", m_WindowWidth_, m_WindowHeight_);
}

void HorusRadeon::RenderEngine()
{
	const auto TimeUpdateStarts = std::chrono::high_resolution_clock::now();

	if (BenchmarkStart == InvalidTime)
		BenchmarkStart = TimeUpdateStarts;
	if (BenchmarkNumberOfRenderIteration >= 100)
	{
		double ElapsedTimeMs = std::chrono::duration<double, std::milli>(TimeUpdateStarts - BenchmarkStart).count();
		double renderPerSecond = static_cast<double>(BenchmarkNumberOfRenderIteration) * 1000.0 / ElapsedTimeMs;
		//std::cout << renderPerSecond << " iterations per second." << std::endl;
		spdlog::info("Rendering -> {} iterations per second.", renderPerSecond);
		BenchmarkNumberOfRenderIteration = 0;
		BenchmarkStart = TimeUpdateStarts;
	}

	RenderProgressCallback.Clear();

	if (m_IsDirty_)
	{
		m_RenderThread_ = std::thread(RenderJob, m_Context_, &RenderProgressCallback);

		m_ThreadRunning_ = true;
	}

	while (!RenderProgressCallback.Done)
	{
		if (!m_IsDirty_ && m_MaxSamples_ != -1 && m_SampleCount_ >= m_MaxSamples_)
		{
			break;
		}

		if (RenderProgressCallback.HasUpdate)
		{
			RenderMutex.lock();

			m_SampleCount_++;

			if (rpr_int Status = rprContextResolveFrameBuffer(m_Context_, m_FrameBuffer_, m_FrameBuffer2_, false); Status != RPR_SUCCESS) 
			{
				std::cout << "RPR Error: " << Status << '\n';
			}

			size_t FramebufferSize = 0;
			CHECK(rprFrameBufferGetInfo(m_FrameBuffer2_, RPR_FRAMEBUFFER_DATA, 0, NULL, &FramebufferSize))

			if (FramebufferSize != m_WindowWidth_ * m_WindowHeight_ * 4 * sizeof(float))
			{
				CHECK(RPR_ERROR_INTERNAL_ERROR)
			}

			CHECK(rprFrameBufferGetInfo(m_FrameBuffer2_, RPR_FRAMEBUFFER_DATA, FramebufferSize, m_FbData_.get(), NULL))

			glBindTexture(GL_TEXTURE_2D, m_TextureBuffer_);

			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_WindowWidth_, m_WindowHeight_, GL_RGBA, GL_FLOAT,
				static_cast<const GLvoid*>(m_FbData_.get()));

			glBindTexture(GL_TEXTURE_2D, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureBuffer_, 0);

			RenderProgressCallback.HasUpdate = false;

			RenderMutex.unlock();

			break;

		}
	}

	if (m_ThreadRunning_)
	{
		m_RenderThread_.join();
		m_ThreadRunning_ = false;
	}

	GetRenderProgress();

	BenchmarkNumberOfRenderIteration += m_BatchSize_;
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

	RenderMultiTiles(FbMetadata, ObjectManager.GetScene(), m_Context_, MaxIterationPerTiles);

	if (!stbi_write_png("FinalRender.png", m_WindowWidth_, m_WindowHeight_, 3, &FbMetadata.FbData[0], m_WindowWidth_ * 3))
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

void HorusRadeon::RenderMultiTiles(HorusFrameBufferMetadata& fb_metadata, rpr_scene, rpr_context context, rpr_uint MaxIterationRendering)
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	rprCameraGetInfo(m_Camera_, RPR_CAMERA_LENS_SHIFT, sizeof(m_PreviousLensShift_), &m_PreviousLensShift_, nullptr);

	CHECK_GT(fb_metadata.RenderTargetSizeX, fb_metadata.TileSizeX)
	CHECK_GT(fb_metadata.RenderTargetSizeY, fb_metadata.TileSizeY)

	fb_metadata.FbData.resize(fb_metadata.RenderTargetSizeX * fb_metadata.RenderTargetSizeY * 3);

	float TileSizeXf = fb_metadata.RenderTargetSizeX / float(fb_metadata.TileSizeX);
	float TileSizeYf = fb_metadata.RenderTargetSizeY / float(fb_metadata.TileSizeY);

	int TileSizeX = int(ceil(TileSizeXf));
	int TileSizeY = int(ceil(TileSizeYf));

	spdlog::info("RenderMultiTiles : ");
	spdlog::info("Virtual resolution : {} x {}", fb_metadata.RenderTargetSizeX, fb_metadata.RenderTargetSizeY);
	spdlog::info("Tile resolution : {} x {}", fb_metadata.TileSizeX, fb_metadata.TileSizeY);
	spdlog::info("Tiled resolution : {} x {}", TileSizeXf, TileSizeYf);
	spdlog::info("Tiled offset : {} x {}", -TileSizeXf / 2.0f + 0.5f, -TileSizeYf / 2.0f + 0.5f);

	rpr_framebuffer_desc Desc = { rpr_uint(fb_metadata.TileSizeX), rpr_uint(fb_metadata.TileSizeY) };
	rpr_framebuffer_format Fmt = { 4, RPR_COMPONENT_TYPE_FLOAT32 };

	rpr_framebuffer FrameBuffer = nullptr; rpr_int Status = rprContextCreateFrameBuffer(context, Fmt, &Desc, &FrameBuffer); CHECK(Status)
	rpr_framebuffer FrameBufferResolved = nullptr; Status = rprContextCreateFrameBuffer(context, Fmt, &Desc, &FrameBufferResolved); CHECK(Status)
	Status = rprContextSetAOV(context, RPR_AOV_COLOR, FrameBuffer); CHECK(Status)

	size_t FrameBufferDatasize = 0;
	Status = rprFrameBufferGetInfo(FrameBuffer, RPR_FRAMEBUFFER_DATA, 0, nullptr, &FrameBufferDatasize); CHECK(Status)
	CHECK_EQ(FrameBufferDatasize, fb_metadata.TileSizeX * fb_metadata.TileSizeY * 4 * sizeof(float))
	auto FrameBufferData = (float*)malloc(FrameBufferDatasize);

	rpr_camera camera = nullptr;
	Status = rprSceneGetCamera(ObjectManager.GetScene(), &camera); CHECK(Status)
	//status = rprCameraSetSensorSize(camera, fb_metadata.RenderTargetSizeX, fb_metadata.RenderTargetSizeY); CHECK(status);

	float ShiftY = -(TileSizeYf / 2.0f) + 0.5f;
	for (int YTiles = 0; YTiles < TileSizeY; YTiles++)
	{
		float DeltaY = 1.0f;
		float ShiftX = -(TileSizeXf / 2.0f) + 0.5f;

		for (int XTiles = 0; XTiles < TileSizeX; XTiles++)
		{
			float DeltaX = 1.0f;
			Status = rprCameraSetLensShift(camera, ShiftX, ShiftY); CHECK(Status)
			Status = rprFrameBufferClear(FrameBuffer); CHECK(Status)

			for (rpr_uint i = 0; i < MaxIterationRendering; i++)
			{
				Status = rprContextSetParameterByKey1u(context, RPR_CONTEXT_FRAMECOUNT, i); CHECK(Status)
				Status = rprContextRender(context); CHECK(Status)
			}

			Status = rprContextResolveFrameBuffer(context, FrameBuffer, FrameBufferResolved, false); CHECK(Status)
			Status = rprFrameBufferGetInfo(FrameBufferResolved, RPR_FRAMEBUFFER_DATA, FrameBufferDatasize, FrameBufferData, nullptr); CHECK(Status)

			int OffsetInRenderTargetX = XTiles * fb_metadata.TileSizeX;
			int OffsetInRenderTargetY = YTiles * fb_metadata.TileSizeY;

			for (unsigned j = 0; j < fb_metadata.TileSizeY; j++)
			{
				for (unsigned i = 0; i < fb_metadata.TileSizeX; i++)
				{
					int DstX = OffsetInRenderTargetX + i;
					int DstY = OffsetInRenderTargetY + j;

					if (DstX >= fb_metadata.RenderTargetSizeX || DstY >= fb_metadata.RenderTargetSizeY)
						continue;

					int Idx = DstX + (fb_metadata.RenderTargetSizeY - DstY - 1) * fb_metadata.RenderTargetSizeX;
					int SIdx = i + (fb_metadata.TileSizeY - j - 1) * fb_metadata.TileSizeX;

					[[maybe_unused]] float INVW = 1.0f / FrameBufferData[4 * SIdx + 3];

					fb_metadata.FbData[3 * Idx + 0] = Clamp(int(FrameBufferData[4 * SIdx] * 255.f), 0, 255);
					fb_metadata.FbData[3 * Idx + 1] = Clamp(int(FrameBufferData[4 * SIdx + 1] * 255.f), 0, 255);
					fb_metadata.FbData[3 * Idx + 2] = Clamp(int(FrameBufferData[4 * SIdx + 2] * 255.f), 0, 255);
				}
			}
			ShiftX += DeltaX;
		}
		ShiftY += DeltaY;
	}

	Status = rprCameraSetLensShift(camera, m_PreviousLensShift_, m_PreviousLensShift_); CHECK(Status)

	free(FrameBufferData); FrameBufferData = nullptr;
	if (FrameBuffer) { Status = rprObjectDelete(FrameBuffer); CHECK(Status)
		FrameBuffer = nullptr; }
	if (FrameBufferResolved) { Status = rprObjectDelete(FrameBufferResolved); CHECK(Status)
		FrameBufferResolved = nullptr; }
}

float HorusRadeon::GetRenderProgress()
{
	int MaxSpp = m_MaxSamples_;
	float Progress = MaxSpp <= 0 ? 0.0f : m_SampleCount_ * 100.0f / MaxSpp;

	if (Progress >= 100.0f)
	{
		m_IsDirty_ = false;
	}

	return Progress;
}