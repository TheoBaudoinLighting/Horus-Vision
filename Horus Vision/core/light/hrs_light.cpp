
#include "hrs_light.h"
#include "hrs_radeon.h"

#include <tbb/task_group.h>
#include <Math/mathutils.h>

#include <thread>
#include <mutex>
#include <future>

#include "objects/hrs_object_manager.h"
#include <hrs_console.h>

std::mutex mtx;

void HorusLight::init(const std::string& light_type, const std::string& hdri_image)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();
	HorusGarbageCollector& gc = HorusGarbageCollector::get_instance();

	m_light = create_light(light_type, hdri_image);

	gc.add(m_light);
}

rpr_light create_hdri_light(const std::string& hdri_image)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();
	HorusObjectManager& ObjectManager = HorusObjectManager::get_instance();
	HorusGarbageCollector& gc = HorusGarbageCollector::get_instance();
	HorusConsole& Console = HorusConsole::get_instance();

	rpr_light light = nullptr;
	rpr_image image = nullptr;

	if (hdri_image == "")
	{
		spdlog::error("Image hdri is empty");
		Console.AddLog(" [error] Image hdri is empty");

		return nullptr;
	}

	CHECK(rprContextCreateEnvironmentLight(Radeon.get_context(), &light));
	gc.add(light);

	const std::string path = hdri_image;

	rpr_status status = rprContextCreateImageFromFile(Radeon.get_context(), path.c_str(), &image);

	if (status != RPR_SUCCESS)
	{
		spdlog::error("Failed to load image: {}", path);
		Console.AddLog(" [error] Failed to load image : %s ", path.c_str());

		return nullptr;
	}
	CHECK(status);
	gc.add(image);

	CHECK(rprEnvironmentLightSetImage(light, image));
	CHECK(rprEnvironmentLightSetIntensityScale(light, 1.f));
	CHECK(rprSceneAttachLight(ObjectManager.get_scene(), light));

	spdlog::info("Light created");
	Console.AddLog(" [success] Light created");

	return light;
}

rpr_light HorusLight::create_light(const std::string& light_type, const std::string& hdri_image)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();
	HorusGarbageCollector& gc = HorusGarbageCollector::get_instance();
	HorusConsole& Console = HorusConsole::get_instance();

	spdlog::info("Create light of type: {}", light_type);
	Console.AddLog(" [info] Create light of type: %s ", light_type.c_str());

	rpr_light light = nullptr;

	if (light_type == "hdri")
	{
		std::thread t([&]()
			{
				std::lock_guard<std::mutex> lock(mtx);
				light = create_hdri_light(hdri_image);
				gc.add(light);
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

void HorusLight::get_info()
{
	rpr_light_type type;
	CHECK(rprLightGetInfo(m_light, RPR_LIGHT_TYPE, sizeof(rpr_light_type), &type, nullptr));
}

void HorusLight::set_position(const RadeonProRender::float3& position) 
{
	get_info();

	m_position_ = position;

	update_light();
}

void HorusLight::set_rotation(RadeonProRender::float3 rotation_axis, float rotation_angle)
{
	get_info();

	m_rotation_axis_ = rotation_axis;
	m_rotation_angle_ = rotation_angle;

	update_light();
}

void HorusLight::set_scale(const RadeonProRender::float3& scale) 
{
	get_info();

	m_scale_ = scale;

	update_light();
}

void HorusLight::update_light()
{
	m_transform_ = RadeonProRender::translation(m_position_) * RadeonProRender::rotation(m_rotation_axis_, m_rotation_angle_) * RadeonProRender::scale(m_scale_);

	CHECK(rprLightSetTransform(m_light, RPR_TRUE, &m_transform_.m00));
}