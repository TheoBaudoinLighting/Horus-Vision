
#include "hrs_radeon.h"

#include <cassert>
#include <iostream>
#include <thread>
#include <memory>
#include <array>
#include <chrono>
#include <future>
#include <array>

#include <Math/mathutils.h>

#include <tbb/task_group.h>

#include "spdlog/spdlog.h"
#include "hrs_engine.h"
#include "hrs_importer.h"
#include "hrs_scene.h"
#include "ImGuizmo.h"
#include "objects/hrs_object_manager.h"

// call engine
HorusEngine& m_engineR_ = HorusEngine::get_instance(); // each call requires a new name but get instance is the same
HorusObjectManager& g_object_manager = HorusObjectManager::get_instance();

std::mutex renderMutex; // oublie pas ton putain de mutex de merde theo !!!!!!!!! <----------------------------------LA ICIICICICI TA LE MUTEX !!!!!!! FAUT LE LAISSER !!!!

class Render_Progress_Callback
{
public:

	struct Update
	{
		Update() : m_hasUpdate(0), m_done(0), m_aborted(0), render_ready_(false), m_camUpdated(0), m_progress(0.0f) {}

		volatile int m_hasUpdate;
		volatile int m_done;
		volatile int m_aborted;
		bool render_ready_;
		int m_camUpdated;
		float m_progress;

		void clear()
		{
			m_hasUpdate = m_done = m_aborted = m_camUpdated = 0;
		}
	};

	static void notifyUpdate(float x, void* userData)
	{
		auto update = static_cast<Update*>(userData);
		update->m_hasUpdate = 1;
		update->m_progress = x;
	}
};

Render_Progress_Callback::Update render_progress_callback;

const auto invalid_time = std::chrono::time_point<std::chrono::high_resolution_clock>::max();
int benchmark_number_of_render_iteration = 0;
auto benchmark_start = invalid_time;

void render_job(rpr_context ctxt, Render_Progress_Callback::Update* update)
{
	renderMutex.lock();
	CHECK(rprContextRender(ctxt));
	//rprContextRender(ctxt);
	update->m_done = 1;
	renderMutex.unlock();
}

glm::vec2 HorusRadeon::set_window_size(int width, int height)
{
	m_window_width_ = width;
	m_window_height_ = height;

	return glm::vec2(m_window_width_, m_window_height_);
}

std::tuple<GLuint, GLuint, GLuint> get_shader_variables(GLuint program, const char* texture_name, const char* position_name, const char* texcoord_name)
{
	GLuint texture_loc = glGetUniformLocation(program, texture_name);
	GLuint position_attr_id = glGetAttribLocation(program, position_name);
	GLuint texcoord_attr_id = glGetAttribLocation(program, texcoord_name);

	return std::make_tuple(texture_loc, position_attr_id, texcoord_attr_id);
}

void HorusRadeon::create_frame_buffers(int width, int height)
{
	m_window_width_ = width;
	m_window_height_ = height;

	glDeleteTextures(1, &m_texture_buffer_);

	CHECK(rprObjectDelete(m_frame_buffer_));
	CHECK(rprObjectDelete(m_frame_buffer_2_));

	rpr_framebuffer_format fmt = { 4, RPR_COMPONENT_TYPE_FLOAT32 };
	rpr_framebuffer_desc desc = { static_cast<unsigned int>(m_window_width_), static_cast<unsigned int>(m_window_height_) };

	CHECK(rprContextCreateFrameBuffer(m_context_, fmt, &desc, &m_frame_buffer_));
	CHECK(rprContextCreateFrameBuffer(m_context_, fmt, &desc, &m_frame_buffer_2_));
}

bool HorusRadeon::init(int width, int height, HorusWindowConfig* window)
{
	m_window_width_ = width;
	m_window_height_ = height;
	m_window_config_ = window;
	m_is_dirty_ = true;

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glCullFace(GL_NONE);
	glDisable(GL_DEPTH_TEST);

	glViewport(0, 0, m_window_width_, m_window_height_);

	glEnable(GL_TEXTURE_2D);

	glGenBuffers(1, &m_vertex_buffer_id_);
	glGenBuffers(1, &m_index_buffer_id_);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer_id_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer_id_);


	constexpr std::array<float, 20> quadVertexData = { -1, -1, 0.5, 0, 0, 1, -1, 0.5, 1, 0, 1, 1, 0.5, 1, 1, -1, 1, 0.5, 0, 1 };
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertexData), quadVertexData.data(), GL_STATIC_DRAW);

	constexpr std::array<GLshort, 6> quadIndexData = { 0, 1, 3, 3, 1, 2 };
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndexData), quadIndexData.data(), GL_STATIC_DRAW);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenTextures(1, &m_texture_buffer_);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture_buffer_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_window_width_, m_window_height_, 0, GL_RGBA, GL_FLOAT, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

	spdlog::info("Radeon initialized.");

	set_window_size(m_window_width_, m_window_height_);

	return true;
}

void HorusRadeon::init_render()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer_id_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer_id_);

	m_program_ = m_shader_manager_.get_program("core/shaders/shader");
	auto [texture_location, position_location, texcoord_location] = get_shader_variables(m_program_, "g_Texture", "inPosition", "inTexcoord");


	glUseProgram(m_program_);
	glUniform1i(texture_location, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture_buffer_);

	glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, nullptr);
	glVertexAttribPointer(texcoord_location, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3));

	glEnableVertexAttribArray(position_location);
	glEnableVertexAttribArray(texcoord_location);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);

	glDisableVertexAttribArray(position_location);
	glDisableVertexAttribArray(texcoord_location);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);
}

void HorusRadeon::post_render() {}

void HorusRadeon::quit_render()
{
	spdlog::info("Unload Radeon..");
	spdlog::info("Release memory..");

	g_object_manager.destroy_all_meshes();
	g_object_manager.destroy_all_material();
	g_object_manager.destroy_all_material_editors();
	g_object_manager.destroy_all_lights();

	spdlog::info("All Radeon objects deleted..");

	glDeleteTextures(1, &m_texture_buffer_);

	CHECK(rprObjectDelete(m_matsys_)); m_matsys_ = nullptr;
	CHECK(rprObjectDelete(m_frame_buffer_)); m_frame_buffer_ = nullptr;
	CHECK(rprObjectDelete(m_frame_buffer_2_)); m_frame_buffer_2_ = nullptr;
	
	g_object_manager.destroy_all_scenes();

	m_gc_.GCClean();

	g_object_manager.destroy_all_cameras();

	rprContextClearMemory(m_context_);
	CheckNoLeak(m_context_);
	CHECK(rprObjectDelete(m_context_)); m_context_ = nullptr;

	spdlog::info("Radeon successfully unloaded..");
}

bool HorusRadeon::init_graphics()
{
	spdlog::info("Init Radeon graphics..");

	rpr_int status = RPR_SUCCESS;

	spdlog::info("RPR_API_VERSION : {}", RPR_API_VERSION);

	rpr_int pluginID = rprRegisterPlugin(RPR_PLUGIN_FILE_NAME);
	CHECK_NE(pluginID, -1);

	rpr_int plugins[] = { pluginID };
	size_t numPlugins = sizeof(plugins) / sizeof(plugins[0]);
	spdlog::info("Number of plugins : {}", numPlugins);

	status = rprCreateContext(RPR_API_VERSION, plugins, numPlugins,
		RPR_CREATION_FLAGS_ENABLE_GL_INTEROP | RPR_CREATION_FLAGS_ENABLE_GPU0 |
		RPR_CREATION_FLAGS_ENABLE_GPU1 | RPR_CREATION_FLAGS_ENABLE_CPU,
		g_contextProperties, nullptr, &m_context_);

	CHECK(status);

	CHECK(rprContextSetActivePlugin(m_context_, plugins[0]));
	CHECK(rprContextCreateMaterialSystem(m_context_, 0, &m_matsys_));

	spdlog::info("Rpr context successfully set active plugin..");
	spdlog::info("Rpr context successfully created..");

	g_object_manager.create_scene(0, "render_scene");

	m_sample_count_ = 1;

	g_object_manager.create_camera_with_id(0, "rendercam");

	// Create Scene --------------------------------------------------------------------------------------------------------
	
	g_object_manager.create_light(0, "Lgt_Dome01", "hdri", "resources/Textures/resting_place_2_2k.exr");


	g_object_manager.show_dummy_dragon();
	g_object_manager.show_dummy_plane();

	// ---------------------------------------------------------------------------------------------------------------------

	CHECK(rprContextSetParameterByKey1f(m_context_, RPR_CONTEXT_DISPLAY_GAMMA, 2.2f));

	rpr_framebuffer_desc desc = { static_cast<unsigned int>(m_window_width_), static_cast<unsigned int>(m_window_height_) };
	rpr_framebuffer_format fmt = { 4, RPR_COMPONENT_TYPE_FLOAT32 };

	CHECK(rprContextCreateFrameBuffer(m_context_, fmt, &desc, &m_frame_buffer_));
	CHECK(rprContextCreateFrameBuffer(m_context_, fmt, &desc, &m_frame_buffer_2_));

	CHECK(rprContextSetAOV(m_context_, RPR_AOV_COLOR, m_frame_buffer_));

	CHECK(rprContextSetParameterByKeyPtr(m_context_, RPR_CONTEXT_RENDER_UPDATE_CALLBACK_FUNC, (void*)Render_Progress_Callback::notifyUpdate));
	CHECK(rprContextSetParameterByKeyPtr(m_context_, RPR_CONTEXT_RENDER_UPDATE_CALLBACK_DATA, &render_progress_callback));

	CHECK(rprContextSetParameterByKey1u(m_context_, RPR_CONTEXT_ITERATIONS, 1));

	std::thread renderThread([&]() {
		CHECK(rprContextRender(m_context_));
		});
	renderThread.join();

	CHECK(rprContextSetParameterByKey1u(m_context_, RPR_CONTEXT_ITERATIONS, m_batch_size_));

	m_fb_data_ = std::shared_ptr<float>(new float[m_window_width_ * m_window_height_ * 4], std::default_delete<float[]>());

	spdlog::info("Radeon graphics successfully initialized..");

	return true;
}

void HorusRadeon::resize_render(int width, int height)
{
	m_window_width_ = width;
	m_window_height_ = height;

	glViewport(0, 0, m_window_width_, m_window_height_);

	m_fb_data_ = nullptr;

	create_frame_buffers(m_window_width_, m_window_height_);

	CHECK(rprContextSetAOV(m_context_, RPR_AOV_COLOR, m_frame_buffer_));

	m_fb_data_ = std::shared_ptr<float>(new float[m_window_width_ * m_window_height_ * 4], std::default_delete<float[]>());

	init(m_window_width_, m_window_height_, m_window_config_);

	CHECK(rprContextResolveFrameBuffer(m_context_, m_frame_buffer_, m_frame_buffer_2_, false));
	size_t fb_size = 0;
	CHECK(rprFrameBufferGetInfo(m_frame_buffer_2_, RPR_FRAMEBUFFER_DATA, 0, nullptr, &fb_size));

	set_window_size(m_window_width_, m_window_height_);
	m_sample_count_ = 1;

	spdlog::info("Radeon successfully resized in : Width : {}, Height : {}", m_window_width_, m_window_height_);
}

void HorusRadeon::render_engine()
{
	const auto timeUpdateStarts = std::chrono::high_resolution_clock::now();

	if (benchmark_start == invalid_time)
		benchmark_start = timeUpdateStarts;
	if (benchmark_number_of_render_iteration >= 100)
	{
		double elapsed_time_ms = std::chrono::duration<double, std::milli>(timeUpdateStarts - benchmark_start).
			count();
		double renderPerSecond = static_cast<double>(benchmark_number_of_render_iteration) * 1000.0 /
			elapsed_time_ms;
		//std::cout << renderPerSecond << " iterations per second." << std::endl;
		spdlog::info("Rendering -> {} iterations per second.", renderPerSecond);
		benchmark_number_of_render_iteration = 0;
		benchmark_start = timeUpdateStarts;
	}

	render_progress_callback.clear();

	if (m_is_dirty_)
	{
		m_render_thread_ = std::thread(render_job, m_context_, &render_progress_callback);

		m_thread_running_ = true;
	}

	while (!render_progress_callback.m_done)
	{
		if (!m_is_dirty_ && m_max_samples_ != -1 && m_sample_count_ >= m_max_samples_)
		{
			break;
		}

		if (render_progress_callback.m_hasUpdate)
		{
			renderMutex.lock();

			m_sample_count_++;

			rpr_int status = rprContextResolveFrameBuffer(m_context_, m_frame_buffer_, m_frame_buffer_2_, false);
			if (status != RPR_SUCCESS) {
				std::cout << "RPR Error: " << status << std::endl;
			}

			size_t framebuffer_size = 0;
			CHECK(rprFrameBufferGetInfo(m_frame_buffer_2_, RPR_FRAMEBUFFER_DATA, 0, NULL, &framebuffer_size));

			if (framebuffer_size != m_window_width_ * m_window_height_ * 4 * sizeof(float))
			{
				CHECK(RPR_ERROR_INTERNAL_ERROR);
			}

			CHECK(rprFrameBufferGetInfo(m_frame_buffer_2_, RPR_FRAMEBUFFER_DATA, framebuffer_size, m_fb_data_.get(), NULL));

			glBindTexture(GL_TEXTURE_2D, m_texture_buffer_);

			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_window_width_, m_window_height_, GL_RGBA, GL_FLOAT,
				static_cast<const GLvoid*>(m_fb_data_.get()));

			glBindTexture(GL_TEXTURE_2D, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture_buffer_, 0);

			render_progress_callback.m_hasUpdate = false;

			renderMutex.unlock();

			break;

		}
	}

	if (m_thread_running_)
	{
		m_render_thread_.join();
		m_thread_running_ = false;
	}

	get_render_progress();

	benchmark_number_of_render_iteration += m_batch_size_;
}

float HorusRadeon::get_render_progress()
{
	int maxSpp = m_max_samples_;
	float progress = maxSpp <= 0 ? 0.0f : m_sample_count_ * 100.0f / maxSpp;

	if (progress >= 100.0f)
	{
		m_is_dirty_ = false;
	}

	return progress;
}