
#include "hrs_light.h"
#include "hrs_radeon.h"

#include <tbb/task_group.h>

#include <thread>
#include <mutex>
#include <future>

#include "objects/hrs_object_manager.h"
#include <hrs_console.h>

std::mutex Mtx;

void HorusLight::Init(const std::string& light_type, const std::string& hdri_image)
{
	HorusGarbageCollector& Gc = HorusGarbageCollector::get_instance();

	m_Light_ = CreateLight(light_type, hdri_image);

	Gc.Add(m_Light_);
}

rpr_light CreateHdriLight(const std::string& hdri_image)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::get_instance();
	HorusConsole& Console = HorusConsole::GetInstance();

	rpr_light Light = nullptr;
	rpr_image Image = nullptr;

	if (hdri_image == "")
	{
		spdlog::error("Image hdri is empty");
		Console.AddLog(" [error] Image hdri is empty");

		return nullptr;
	}

	CHECK(rprContextCreateEnvironmentLight(Radeon.GetContext(), &Light))
	gc.Add(Light);

	const std::string Path = hdri_image;

	rpr_status Status = rprContextCreateImageFromFile(Radeon.GetContext(), Path.c_str(), &Image);

	if (Status != RPR_SUCCESS)
	{
		spdlog::error("Failed to load image: {}", Path);
		Console.AddLog(" [error] Failed to load image : %s ", Path.c_str());

		return nullptr;
	}
	CHECK(Status)
	gc.Add(Image);

	CHECK(rprEnvironmentLightSetImage(Light, Image))
	CHECK(rprEnvironmentLightSetIntensityScale(Light, 1.f))
	CHECK(rprSceneAttachLight(ObjectManager.GetScene(), Light))

	spdlog::info("Light created");
	Console.AddLog(" [success] Light created");

	return Light;
}

rpr_light HorusLight::CreateLight(const std::string& light_type, const std::string& hdri_image)
{
	HorusGarbageCollector& Gc = HorusGarbageCollector::get_instance();
	HorusConsole& Console = HorusConsole::GetInstance();

	spdlog::info("Create light of type: {}", light_type);
	Console.AddLog(" [info] Create light of type: %s ", light_type.c_str());

	rpr_light Light = nullptr;

	if (light_type == "hdri")
	{
		std::thread T([&]()
			{
				std::lock_guard<std::mutex> Lock(Mtx);
				Light = CreateHdriLight(hdri_image);
				Gc.Add(Light);
			}); T.join();

	}

	// TODO: add other types of light

	return Light;
}

void HorusLight::DestroyLight()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	if (m_Light_ != nullptr)
	{
		CHECK(rprSceneDetachLight(ObjectManager.GetScene(), m_Light_))
		CHECK(rprObjectDelete(m_Light_))
		m_Light_ = nullptr;
	}
}

void HorusLight::GetInfo()
{
	rpr_light_type Type;
	rprLightGetInfo(m_Light_, RPR_LIGHT_TYPE, sizeof(rpr_light_type), &Type, nullptr);
}

void HorusLight::SetPosition(const glm::vec3& position)
{
	GetInfo();

	m_Position_ = position;

	UpdateLight();
}

void HorusLight::SetRotation(const glm::vec3& rotation_axis, float rotation_angle)
{
	GetInfo();

	m_RotationAxis_ = rotation_axis;
	m_RotationAngle_ = rotation_angle;

	UpdateLight();
}

void HorusLight::SetScale(const glm::vec3& scale)
{
	GetInfo();

	m_Scale_ = scale;

	UpdateLight();
}

void HorusLight::UpdateLight()
{

	m_Transform_ = glm::translate(m_Position_) * glm::rotate(m_RotationAngle_, m_RotationAxis_) * glm::scale(m_Scale_);

	rprLightSetTransform(m_Light_, RPR_TRUE, &m_Transform_[0][0]);


	/*m_transform_ = glm::translate(m_position_) * glm::rotate(m_rotation_angle_, m_rotation_axis_) * glm::scale(m_scale_);
	CHECK(rprLightSetTransform(m_light, RPR_TRUE, &m_transform_));*/
}