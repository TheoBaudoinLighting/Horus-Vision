
#include "hrs_light.h"
#include "hrs_radeon.h"

#include <tbb/task_group.h>

#include <thread>
#include <mutex>
#include <future>

#include "objects/hrs_object_manager.h"

HorusRadeon& radeon_ImLigth = HorusRadeon::get_instance();
HorusObjectManager& g_object_imp = HorusObjectManager::get_instance();

RPRGarbageCollector m_gc = radeon_ImLigth.get_gc();

std::mutex mtx;

void HorusLight::init(const std::string& light_type, const std::string& hdri_image)
{
	m_light = create_light(light_type, hdri_image);

	m_gc.GCAdd(m_light);
}

rpr_light create_hdri_light(const std::string& hdri_image)
{
	rpr_light light = nullptr;
	rpr_image image = nullptr;

	if (hdri_image == "")
	{
		spdlog::error("Image hdri is empty");

		return nullptr;
	}

	CHECK(rprContextCreateEnvironmentLight(radeon_ImLigth.get_context(), &light));
	m_gc.GCAdd(light);

	const std::string path = hdri_image;

	rpr_status status = rprContextCreateImageFromFile(radeon_ImLigth.get_context(), path.c_str(), &image);

	if (status != RPR_SUCCESS)
	{
		spdlog::error("Failed to load image: {}", path);

		return nullptr;
	}
	CHECK(status);
	m_gc.GCAdd(image);

	CHECK(rprEnvironmentLightSetImage(light, image));
	CHECK(rprEnvironmentLightSetIntensityScale(light, 1.f));
	CHECK(rprSceneAttachLight(g_object_imp.get_scene(), light));

	spdlog::info("Light created");

	return light;
}

rpr_light HorusLight::create_light(const std::string& light_type, const std::string& hdri_image)
{
	spdlog::info("Create light of type: {}", light_type);

	rpr_light light = nullptr;

	if (light_type == "hdri")
	{
		std::thread t([&]()
			{
				std::lock_guard<std::mutex> lock(mtx);
				light = create_hdri_light(hdri_image);
				m_gc.GCAdd(light);
			}); t.join();

	}

	// TODO: add other types of light

	return light;
}

void HorusLight::destroy_light()
{
	if (m_light != nullptr)
	{
		CHECK(rprSceneDetachLight(g_object_imp.get_scene(), m_light));
		CHECK(rprObjectDelete(m_light));
		m_light = nullptr;
	}
}

void HorusLight::set_position(const RadeonProRender::float3& position){}

void HorusLight::set_rotation(const RadeonProRender::float3& rotation){}

void HorusLight::set_scale(const RadeonProRender::float3& scale){}
