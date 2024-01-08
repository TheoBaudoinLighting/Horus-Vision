
#include "hrs_light.h"
#include "hrs_radeon.h"

#include <tbb/task_group.h>

#include <thread>
#include <mutex>
#include <future>

#include "objects/hrs_object_manager.h"

std::mutex mtx;

void HorusLight::init(const std::string& light_type, const std::string& hdri_image)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();
	RPRGarbageCollector& gc = Radeon.get_gc();

	m_light = create_light(light_type, hdri_image);

	gc.GCAdd(m_light);
}

rpr_light create_hdri_light(const std::string& hdri_image)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();
	HorusObjectManager& ObjectManager = HorusObjectManager::get_instance();
	RPRGarbageCollector& gc = Radeon.get_gc();

	rpr_light light = nullptr;
	rpr_image image = nullptr;

	if (hdri_image == "")
	{
		spdlog::error("Image hdri is empty");

		return nullptr;
	}

	CHECK(rprContextCreateEnvironmentLight(Radeon.get_context(), &light));
	gc.GCAdd(light);

	const std::string path = hdri_image;

	rpr_status status = rprContextCreateImageFromFile(Radeon.get_context(), path.c_str(), &image);

	if (status != RPR_SUCCESS)
	{
		spdlog::error("Failed to load image: {}", path);

		return nullptr;
	}
	CHECK(status);
	gc.GCAdd(image);

	CHECK(rprEnvironmentLightSetImage(light, image));
	CHECK(rprEnvironmentLightSetIntensityScale(light, 1.f));
	CHECK(rprSceneAttachLight(ObjectManager.get_scene(), light));

	spdlog::info("Light created");

	return light;
}

rpr_light HorusLight::create_light(const std::string& light_type, const std::string& hdri_image)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();
RPRGarbageCollector& gc = Radeon.get_gc();

	spdlog::info("Create light of type: {}", light_type);

	rpr_light light = nullptr;

	if (light_type == "hdri")
	{
		std::thread t([&]()
			{
				std::lock_guard<std::mutex> lock(mtx);
				light = create_hdri_light(hdri_image);
				gc.GCAdd(light);
			}); t.join();

	}

	// TODO: add other types of light

	return light;
}

void HorusLight::destroy_light()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::get_instance();

	if (m_light != nullptr)
	{
		CHECK(rprSceneDetachLight(ObjectManager.get_scene(), m_light));
		CHECK(rprObjectDelete(m_light));
		m_light = nullptr;
	}
}

void HorusLight::set_position(const RadeonProRender::float3& position){}

void HorusLight::set_rotation(const RadeonProRender::float3& rotation){}

void HorusLight::set_scale(const RadeonProRender::float3& scale){}
