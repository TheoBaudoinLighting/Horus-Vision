
#include "hrs_light.h"
#include "hrs_radeon.h"

#include <thread>
#include <mutex>

#include "objects/hrs_object_manager.h"
#include <hrs_console.h>

#include "glm/detail/_noise.hpp"
#include "glm/detail/_noise.hpp"

std::mutex Mtx;

void HorusLight::Init(const std::string& LightType, const std::string& ImagePath)
{
	HorusGarbageCollector& Gc = HorusGarbageCollector::GetInstance();

	m_Light_ = CreateLight(LightType, ImagePath);
	m_ImagePath_ = ImagePath;

	Gc.Add(m_Light_);
}

void HorusLight::GetInfo()
{
	rprLightGetInfo(m_Light_, RPR_LIGHT_TYPE, sizeof(rpr_light_type), &m_LightType_, nullptr);
	rprLightGetInfo(m_Light_, RPR_LIGHT_TRANSFORM, sizeof(glm::mat4), &m_Transform_, nullptr);
	rprLightGetInfo(m_Light_, RPR_LIGHT_VISIBILITY_LIGHT, sizeof(rpr_bool), &m_IsLightVisible_, nullptr);

	switch (m_LightType_)
	{
	case RPR_LIGHT_TYPE_POINT:
		rprLightGetInfo(m_Light_, RPR_POINT_LIGHT_RADIANT_POWER, sizeof(float), &m_PointLightRadiantPower_, nullptr);
		break;
	case RPR_LIGHT_TYPE_DIRECTIONAL:
		rprLightGetInfo(m_Light_, RPR_DIRECTIONAL_LIGHT_RADIANT_POWER, sizeof(float), &m_DirectionalLightRadiantPower_, nullptr);
		rprLightGetInfo(m_Light_, RPR_DIRECTIONAL_LIGHT_SHADOW_SOFTNESS_ANGLE, sizeof(float), &m_DirectionalLightShadowSoftnessAngle_, nullptr);
		break;
	case RPR_LIGHT_TYPE_SPOT:
		rprLightGetInfo(m_Light_, RPR_SPOT_LIGHT_RADIANT_POWER, sizeof(float), &m_SpotLightRadiansPower_, nullptr);
		rprLightGetInfo(m_Light_, RPR_SPOT_LIGHT_CONE_SHAPE, sizeof(float), &m_SpotLightConeShape_, nullptr);
		rprLightGetInfo(m_Light_, RPR_SPOT_LIGHT_IMAGE, sizeof(rpr_image), &m_SpotLightImage_, nullptr);
	case RPR_LIGHT_TYPE_ENVIRONMENT:
		rprLightGetInfo(m_Light_, RPR_ENVIRONMENT_LIGHT_IMAGE, sizeof(rpr_image), &m_EnvironmentLightImage_, nullptr);
		rprLightGetInfo(m_Light_, RPR_ENVIRONMENT_LIGHT_INTENSITY_SCALE, sizeof(float), &m_EnvironmentLightIntensity_, nullptr);
		rprLightGetInfo(m_Light_, RPR_ENVIRONMENT_LIGHT_OVERRIDE_REFLECTION, sizeof(rpr_uint), &m_EnvironmentLightOverrideReflection_, nullptr);
		rprLightGetInfo(m_Light_, RPR_ENVIRONMENT_LIGHT_OVERRIDE_REFRACTION, sizeof(rpr_uint), &m_EnvironmentLightOverrideRefraction_, nullptr);
		rprLightGetInfo(m_Light_, RPR_ENVIRONMENT_LIGHT_OVERRIDE_TRANSPARENCY, sizeof(rpr_uint), &m_EnvironmentLightOverrideTransparency_, nullptr);
		rprLightGetInfo(m_Light_, RPR_ENVIRONMENT_LIGHT_OVERRIDE_BACKGROUND, sizeof(rpr_uint), &m_EnvironmentLightOverrideBackground_, nullptr);
		rprLightGetInfo(m_Light_, RPR_ENVIRONMENT_LIGHT_OVERRIDE_IRRADIANCE, sizeof(rpr_uint), &m_EnvironmentLightOverrideIrradiance_, nullptr);
		rprLightGetInfo(m_Light_, RPR_ENVIRONMENT_LIGHT_PORTAL_LIST, sizeof(rpr_light), &m_LightPortalList_, nullptr);
		rprLightGetInfo(m_Light_, RPR_ENVIRONMENT_LIGHT_PORTAL_COUNT, sizeof(int), &m_LightPortalNumber_, nullptr);
		break;
	case RPR_LIGHT_TYPE_SKY:
		rprLightGetInfo(m_Light_, RPR_SKY_LIGHT_TURBIDITY, sizeof(float), &m_SkyLightTurbidity_, nullptr);
		rprLightGetInfo(m_Light_, RPR_SKY_LIGHT_ALBEDO, sizeof(float), &m_SkyLightAlbedo_, nullptr);
		rprLightGetInfo(m_Light_, RPR_SKY_LIGHT_SCALE, sizeof(float), &m_SkyLightScale_, nullptr);
		rprLightGetInfo(m_Light_, RPR_SKY_LIGHT_DIRECTION, sizeof(glm::vec3), &m_SkyLightDirection_, nullptr);
		rprLightGetInfo(m_Light_, RPR_SKY_LIGHT_PORTAL_LIST, sizeof(rpr_light), &m_SkyLightPortalList_, nullptr);
		rprLightGetInfo(m_Light_, RPR_SKY_LIGHT_PORTAL_COUNT, sizeof(int), &m_SkyLightPortalNumber_, nullptr);
		break;
	case RPR_LIGHT_TYPE_IES:
		rprLightGetInfo(m_Light_, RPR_IES_LIGHT_RADIANT_POWER, sizeof(float), &m_IesLightRadiantPower_, nullptr);
		rprLightGetInfo(m_Light_, RPR_IES_LIGHT_IMAGE_DESC, sizeof(rpr_image), &m_IesLightImage_, nullptr);
		break;
	case RPR_LIGHT_TYPE_SPHERE:
		rprLightGetInfo(m_Light_, RPR_SPHERE_LIGHT_RADIANT_POWER, sizeof(float), &m_SphereLightRadiantPower_, nullptr);
		rprLightGetInfo(m_Light_, RPR_SPHERE_LIGHT_RADIUS, sizeof(float), &m_SphereLightRadius_, nullptr);
		break;
	case RPR_LIGHT_TYPE_DISK:
		rprLightGetInfo(m_Light_, RPR_DISK_LIGHT_RADIANT_POWER, sizeof(float), &m_PointLightRadiantPower_, nullptr);
		rprLightGetInfo(m_Light_, RPR_DISK_LIGHT_RADIUS, sizeof(float), &m_DiskLightRadius_, nullptr);
		rprLightGetInfo(m_Light_, RPR_DISK_LIGHT_ANGLE, sizeof(float), &m_DiskLightAngle_, nullptr);
		rprLightGetInfo(m_Light_, RPR_DISK_LIGHT_INNER_ANGLE, sizeof(float), &m_DiskLightInnerAngle_, nullptr);
		break;
	}

}

rpr_light CreateHdriLight(const std::string& HdriImage)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusGarbageCollector& Gc = HorusGarbageCollector::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	rpr_light Light = nullptr;
	rpr_image Image = nullptr;

	if (HdriImage.empty())
	{
		spdlog::error("Image hdri is empty");
		Console.AddLog(" [error] Image hdri is empty");

		return nullptr;
	}

	CHECK(rprContextCreateEnvironmentLight(Radeon.GetContext(), &Light))
		Gc.Add(Light);

	const std::string Path = HdriImage;

	rpr_status Status = rprContextCreateImageFromFile(Radeon.GetContext(), Path.c_str(), &Image);

	if (Status != RPR_SUCCESS)
	{
		spdlog::error("Failed to load image: {}", Path);
		Console.AddLog(" [error] Failed to load image : %s ", Path.c_str());

		return nullptr;
	}
	CHECK(Status);
	Gc.Add(Image);

	CHECK(rprEnvironmentLightSetImage(Light, Image));
	CHECK(rprEnvironmentLightSetIntensityScale(Light, 1.f));
	CHECK(rprSceneAttachLight(ObjectManager.GetScene(), Light));
	CHECK(rprLightSetVisibilityFlag(Light, RPR_LIGHT_VISIBILITY_LIGHT, true));

	spdlog::info("Light created");
	Console.AddLog(" [success] Light created");

	return Light;
}
rpr_light CreatePointLight()
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	rpr_light Light = nullptr;

	CHECK(rprContextCreatePointLight(Radeon.GetContext(), &Light));
	gc.Add(Light);

	CHECK(rprPointLightSetRadiantPower3f(Light, 10.0f, 10.0f, 10.0f));
	CHECK(rprSceneAttachLight(ObjectManager.GetScene(), Light));
	CHECK(rprLightSetVisibilityFlag(Light, RPR_LIGHT_VISIBILITY_LIGHT, true));

	spdlog::info("Point light created");
	Console.AddLog(" [success] Point light created");

	return Light;
}
rpr_light CreateDirectionalLight()
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusGarbageCollector& Gc = HorusGarbageCollector::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	rpr_light Light = nullptr;

	CHECK(rprContextCreateDirectionalLight(Radeon.GetContext(), &Light));
	Gc.Add(Light);

	CHECK(rprDirectionalLightSetRadiantPower3f(Light, 10.0f, 10.0f, 10.0f));
	CHECK(rprSceneAttachLight(ObjectManager.GetScene(), Light));
	CHECK(rprLightSetVisibilityFlag(Light, RPR_LIGHT_VISIBILITY_LIGHT, true));

	spdlog::info("Directional light created");
	Console.AddLog(" [success] Directional light created");

	return Light;
}
rpr_light CreateSpotLight(const std::string& ImagePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusGarbageCollector& Gc = HorusGarbageCollector::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	rpr_light Light = nullptr;
	bool IsImage = false;

	if (ImagePath.empty())
	{
		IsImage = false;
	}
	else
	{
		IsImage = true;
	}

	CHECK(rprContextCreateSpotLight(Radeon.GetContext(), &Light));
	Gc.Add(Light);

	if (IsImage)
	{
		rpr_image Image = nullptr;
		const std::string& Path = ImagePath;

		rpr_status Status = rprContextCreateImageFromFile(Radeon.GetContext(), Path.c_str(), &Image);

		if (Status != RPR_SUCCESS)
		{
			spdlog::error("Failed to load image: {}", Path);
			Console.AddLog(" [error] Failed to load image : %s ", Path.c_str());

			return nullptr;
		}

		CHECK(Status)
			Gc.Add(Image);

		if (Image != nullptr)
		{
			CHECK(rprSpotLightSetImage(Light, Image));
		}
	}

	CHECK(rprSpotLightSetRadiantPower3f(Light, 10.0f, 10.0f, 10.0f));
	CHECK(rprSceneAttachLight(ObjectManager.GetScene(), Light));
	CHECK(rprLightSetVisibilityFlag(Light, RPR_LIGHT_VISIBILITY_LIGHT, true));

	spdlog::info("Spot light created");
	Console.AddLog(" [success] Spot light created");

	return Light;
}
rpr_light CreateSkyLight()
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusGarbageCollector& Gc = HorusGarbageCollector::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	rpr_light Light = nullptr;

	CHECK(rprContextCreateSkyLight(Radeon.GetContext(), &Light))
		Gc.Add(Light);

	rpr_float albedo = 1.0f;

	CHECK(rprSkyLightSetTurbidity(Light, 1.0f));
	CHECK(rprSkyLightSetAlbedo(Light, albedo));
	CHECK(rprSkyLightSetScale(Light, 1.0f));


	CHECK(rprSceneAttachLight(ObjectManager.GetScene(), Light));

	spdlog::info("Sky light created");
	Console.AddLog(" [success] Sky light created");

	return Light;
}
rpr_light CreateSphereLight()
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusGarbageCollector& Gc = HorusGarbageCollector::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	rpr_light Light = nullptr;

	CHECK(rprContextCreateSphereLight(Radeon.GetContext(), &Light));
	Gc.Add(Light);

	CHECK(rprSphereLightSetRadiantPower3f(Light, 10.0f, 10.0f, 10.0f));
	CHECK(rprSphereLightSetRadius(Light, 1.f));

	CHECK(rprSceneAttachLight(ObjectManager.GetScene(), Light));

	spdlog::info("Sphere light created");
	Console.AddLog(" [success] Sphere light created");

	return Light;
}
rpr_light CreateDiskLight()
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusGarbageCollector& Gc = HorusGarbageCollector::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	rpr_light Light = nullptr;

	CHECK(rprContextCreateDiskLight(Radeon.GetContext(), &Light));
	Gc.Add(Light);

	CHECK(rprDiskLightSetRadiantPower3f(Light, 10.0f, 10.0f, 10.0f));
	CHECK(rprDiskLightSetRadius(Light, 1.0f));
	CHECK(rprDiskLightSetAngle(Light, 1.0f));
	CHECK(rprDiskLightSetInnerAngle(Light, 1.0f));


	CHECK(rprSceneAttachLight(ObjectManager.GetScene(), Light));

	spdlog::info("Disk light created");
	Console.AddLog(" [success] Disk light created");

	return Light;
}
rpr_light CreateIesLight(const std::string& ImagePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusGarbageCollector& Gc = HorusGarbageCollector::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	rpr_light Light = nullptr;

	CHECK(rprContextCreateIESLight(Radeon.GetContext(), &Light))
		Gc.Add(Light);

	CHECK(rprIESLightSetRadiantPower3f(Light, 10.0f, 10.0f, 10.0f))
		CHECK(rprSceneAttachLight(ObjectManager.GetScene(), Light))

		spdlog::info("Ies light created");
	Console.AddLog(" [success] Ies light created");

	return Light;
}
//rpr_light CreatePortalLight()
//{
//										HorusRadeon& Radeon = HorusRadeon::GetInstance();
//	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
//	HorusGarbageCollector& Gc = HorusGarbageCollector::GetInstance();
//	HorusConsole& Console = HorusConsole::GetInstance();
//
//	rpr_light Light = nullptr;
//
//	CHECK(rprContextCreatePortalLight(Radeon.GetContext(), &Light))
//		Gc.Add(Light);
//
//	CHECK(rprPortalLightSetRadiantPower3f(Light, 10.0f, 10.0f, 10.0f))
//		CHECK(rprSceneAttachLight(ObjectManager.GetScene(), Light))
//
//		spdlog::info("Portal light created");
//	Console.AddLog(" [success] Portal light created");
//
//	return Light;
//}
//rpr_light CreateAreaLight()
//{
//	
//}

rpr_light HorusLight::CreateLight(const std::string& LightType, const std::string& ImagePath)
{
	HorusGarbageCollector& Gc = HorusGarbageCollector::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	if (LightType.empty())
	{
		spdlog::error("Light type is empty");
		Console.AddLog(" [error] Light type is empty");

		return nullptr;
	}

	spdlog::info("Create light of type: {}", LightType);
	Console.AddLog(" [info] Create light of type: %s ", LightType.c_str());

	if (LightType == "hdri")
	{
		if (ImagePath.empty())
		{
			spdlog::error("Image path is empty");
			Console.AddLog(" [error] Image path is empty");

			return nullptr;
		}

		m_IsLightHdri_ = true;
		m_LightType_ = RPR_LIGHT_TYPE_ENVIRONMENT;
		m_Intensity_ = { 1.0f, 1.0f, 1.0f };

		std::thread T([&]()
			{
				std::lock_guard Lock(Mtx);
				m_Light_ = CreateHdriLight(ImagePath);
				Gc.Add(m_Light_);
			}); T.join();

	}
	if (LightType == "point")
	{
		m_IsLightHdri_ = false;
		m_LightType_ = RPR_LIGHT_TYPE_POINT;
		m_Intensity_ = { 10.0f, 10.0f, 10.0f };

		std::thread T([&]()
			{
				std::lock_guard Lock(Mtx);
				m_Light_ = CreatePointLight();
				Gc.Add(m_Light_);
			}); T.join();

	}
	if (LightType == "directional")
	{
		m_IsLightHdri_ = false;
		m_LightType_ = RPR_LIGHT_TYPE_DIRECTIONAL;
		m_Intensity_ = { 10.0f, 10.0f, 10.0f };

		std::thread T([&]()
			{
				std::lock_guard Lock(Mtx);
				m_Light_ = CreateDirectionalLight();
				Gc.Add(m_Light_);
			}); T.join();

	}
	if (LightType == "spot")
	{
		m_IsLightHdri_ = false;
		m_LightType_ = RPR_LIGHT_TYPE_SPOT;
		m_Intensity_ = { 10.0f, 10.0f, 10.0f };

		std::thread T([&]()
			{
				std::lock_guard Lock(Mtx);
				m_Light_ = CreateSpotLight(ImagePath);
				Gc.Add(m_Light_);
			}); T.join();

	}
	if (LightType == "sky")
	{
		m_IsLightHdri_ = false;
		m_LightType_ = RPR_LIGHT_TYPE_SKY;
		m_Intensity_ = { 1.0f, 1.0f, 1.0f };

		std::thread T([&]()
			{
				std::lock_guard Lock(Mtx);
				m_Light_ = CreateSkyLight();
				Gc.Add(m_Light_);
			}); T.join();

	}
	if (LightType == "sphere")
	{
		m_IsLightHdri_ = false;
		m_LightType_ = RPR_LIGHT_TYPE_SPHERE;
		m_Intensity_ = { 10.0f, 10.0f, 10.0f };

		std::thread T([&]()
			{
				std::lock_guard Lock(Mtx);
				m_Light_ = CreateSphereLight();
				Gc.Add(m_Light_);
			}); T.join();

	}
	if (LightType == "disk")
	{
		m_IsLightHdri_ = false;
		m_LightType_ = RPR_LIGHT_TYPE_DISK;
		m_Intensity_ = { 10.0f, 10.0f, 10.0f };

		std::thread T([&]()
			{
				std::lock_guard Lock(Mtx);
				m_Light_ = CreateDiskLight();
				Gc.Add(m_Light_);
			}); T.join();

	}
	if (LightType == "ies")
	{
		m_IsLightHdri_ = false;
		m_LightType_ = RPR_LIGHT_TYPE_IES;
		m_Intensity_ = { 10.0f, 10.0f, 10.0f };

		std::thread T([&]()
			{
				std::lock_guard Lock(Mtx);
				m_Light_ = CreateIesLight(ImagePath);
				Gc.Add(m_Light_);
			}); T.join();
	}

	return m_Light_;
}
void HorusLight::DestroyLight()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	if (m_Light_ != nullptr)
	{
		CHECK(rprSceneDetachLight(ObjectManager.GetScene(), m_Light_));
		CHECK(rprObjectDelete(m_Light_));
		m_Light_ = nullptr;
		m_IsLightHdri_ = false;

	}
}

// General Setters
void HorusLight::SetLightType(const std::string& LightType)
{
	GetInfo();

	rpr_light_type TempLightType = m_LightType_;

	if (LightType == "point")
	{
		m_LightType_ = RPR_LIGHT_TYPE_POINT;
	}
	if (LightType == "directional")
	{
		m_LightType_ = RPR_LIGHT_TYPE_DIRECTIONAL;
	}
	if (LightType == "spot")
	{
		m_LightType_ = RPR_LIGHT_TYPE_SPOT;
	}
	if (LightType == "environment")
	{
		m_LightType_ = RPR_LIGHT_TYPE_ENVIRONMENT;
	}
	if (LightType == "sky")
	{
		m_LightType_ = RPR_LIGHT_TYPE_SKY;
	}
	if (LightType == "ies")
	{
		m_LightType_ = RPR_LIGHT_TYPE_IES;
	}
	if (LightType == "sphere")
	{
		m_LightType_ = RPR_LIGHT_TYPE_SPHERE;
	}
	if (LightType == "disk")
	{
		m_LightType_ = RPR_LIGHT_TYPE_DISK;
	}

	if (TempLightType != m_LightType_)
	{
		DestroyLight();
		m_Light_ = CreateLight(LightType, m_ImagePath_);
	}
}
void HorusLight::SetPosition(const glm::vec3& Position)
{
	GetInfo();

	m_Position_ = Position;

	UpdateLight();
}
void HorusLight::SetRotation(const glm::vec3& RotationAxis)
{
	GetInfo();

	m_Rotation_ = RotationAxis;

	UpdateLight();
}
void HorusLight::SetScale(const glm::vec3& Scale)
{
	GetInfo();

	m_Scale_ = Scale;

	UpdateLight();
}
void HorusLight::SetDirection(const glm::vec3& Direction)
{
	GetInfo();

	m_Direction_ = Direction;

	UpdateLight();
}
void HorusLight::SetIntensity(glm::vec3& Intensity)
{
	switch (m_LightType_)
	{
	case RPR_LIGHT_TYPE_POINT:
		CHECK(rprPointLightSetRadiantPower3f(m_Light_, Intensity.x, Intensity.y, Intensity.z))
			break;
	case RPR_LIGHT_TYPE_DIRECTIONAL:
		CHECK(rprDirectionalLightSetRadiantPower3f(m_Light_, Intensity.x, Intensity.y, Intensity.z))
			break;
	case RPR_LIGHT_TYPE_SPOT:
		CHECK(rprSpotLightSetRadiantPower3f(m_Light_, Intensity.x, Intensity.y, Intensity.z))
			break;
	case RPR_LIGHT_TYPE_ENVIRONMENT:
		CHECK(rprEnvironmentLightSetIntensityScale(m_Light_, Intensity.x))
			break;
	case RPR_LIGHT_TYPE_SKY:
		spdlog::info("Sky light intensity is not implemented yet");
		break;
	case RPR_LIGHT_TYPE_IES:
		CHECK(rprIESLightSetRadiantPower3f(m_Light_, Intensity.x, Intensity.y, Intensity.z))
			break;
	case RPR_LIGHT_TYPE_SPHERE:
		CHECK(rprSphereLightSetRadiantPower3f(m_Light_, Intensity.x, Intensity.y, Intensity.z))
			break;
	case RPR_LIGHT_TYPE_DISK:
		CHECK(rprDiskLightSetRadiantPower3f(m_Light_, Intensity.x, Intensity.y, Intensity.z))
			break;
	}
}
void HorusLight::SetLightVisibility(bool Visibility)
{
	if (Visibility)
	{
		CHECK(rprLightSetVisibilityFlag(m_Light_, RPR_LIGHT_VISIBILITY_LIGHT, true))
	}
	else
	{
		CHECK(rprLightSetVisibilityFlag(m_Light_, RPR_LIGHT_VISIBILITY_LIGHT, false))
	}
}

// Directional 
void HorusLight::SetDirectionalLightShadowSoftnessAngle(float Coef)
{
	CHECK(rprDirectionalLightSetShadowSoftnessAngle(m_Light_, Coef))
}

// Spot
void HorusLight::SetConeShape(float InAngle, float OutAngle)
{
	CHECK(rprSpotLightSetConeShape(m_Light_, InAngle, OutAngle))
}
void HorusLight::SetSpotLightImage(const std::string& ImagePath)
{
	rpr_image Image = nullptr;
	const std::string& Path = ImagePath;

	if (ImagePath.empty())
	{
		spdlog::error("Image path is empty");
		HorusConsole::GetInstance().AddLog(" [error] Image path is empty");

		return;
	}

	rpr_status Status = rprContextCreateImageFromFile(HorusRadeon::GetInstance().GetContext(), Path.c_str(), &Image);

	if (Status != RPR_SUCCESS)
	{
		spdlog::error("Failed to load image: {}", Path);
		HorusConsole::GetInstance().AddLog(" [error] Failed to load image : %s ", Path.c_str());

		return;
	}

	CHECK(Status);
	HorusGarbageCollector::GetInstance().Add(Image);

	CHECK(rprSpotLightSetImage(m_Light_, Image))
}

// Environment
void HorusLight::SetEnvironmentLightSetImage(const std::string& ImagePath)
{
	rpr_image Image = nullptr;
	const std::string& Path = ImagePath;

	if (ImagePath.empty())
	{
		spdlog::error("Image path is empty");
		HorusConsole::GetInstance().AddLog(" [error] Image path is empty");

		return;
	}

	rpr_status Status = rprContextCreateImageFromFile(HorusRadeon::GetInstance().GetContext(), Path.c_str(), &Image);

	if (Status != RPR_SUCCESS)
	{
		spdlog::error("Failed to load image: {}", Path);
		HorusConsole::GetInstance().AddLog(" [error] Failed to load image : %s ", Path.c_str());

		return;
	}

	CHECK(Status);
	HorusGarbageCollector::GetInstance().Add(Image);

	CHECK(rprEnvironmentLightSetImage(m_Light_, Image))
}
void HorusLight::SetShapeEnvironmentLight(rpr_shape Shape, bool IsEnvLight)
{
	CHECK(rprShapeSetEnvironmentLight(Shape, IsEnvLight))
}
void HorusLight::SetEnvironmentLightAttachPortal(rpr_scene Scene, rpr_light Light, rpr_shape Shape)
{
	CHECK(rprEnvironmentLightAttachPortal(Scene, Light, Shape))
}
void HorusLight::SetEnvironmentLightDetachPortal(rpr_scene Scene, rpr_light Light, rpr_shape Shape)
{
	CHECK(rprEnvironmentLightDetachPortal(Scene, Light, Shape))
}

// Sky
void HorusLight::SetSkyLightTurbidity(float Turbidity)
{
	CHECK(rprSkyLightSetTurbidity(m_Light_, Turbidity))
}
void HorusLight::SetSkyLightAlbedo(const glm::vec3& Albedo)
{
	CHECK(rprSkyLightSetAlbedo(m_Light_, Albedo.x))
}
void HorusLight::SetSkyLightScale(const glm::vec3& Scale)
{
	CHECK(rprSkyLightSetScale(m_Light_, Scale.x))
}
void HorusLight::SetSkyLightDirection(const glm::vec3& Direction)
{
	CHECK(rprSkyLightSetDirection(m_Light_, Direction.x, Direction.y, Direction.z))
}
void HorusLight::SetSkyLightAttachPortal(rpr_scene Scene, rpr_light Light, rpr_shape Shape)
{
	CHECK(rprSkyLightAttachPortal(Scene, Light, Shape))
}
void HorusLight::SetSkyLightDetachPortal(rpr_scene Scene, rpr_light Light, rpr_shape Shape)
{
	CHECK(rprSkyLightDetachPortal(Scene, Light, Shape))
}

// Ies
void HorusLight::SetIesLightImage(const std::string& ImagePath, int ImgSizeX, int ImgSizeY)
{
	const char* Path = ImagePath.c_str();

	if (ImagePath.empty())
	{
		spdlog::error("Image path is empty");
		HorusConsole::GetInstance().AddLog(" [error] Image path is empty");

		return;
	}

	CHECK(rprIESLightSetImageFromFile(m_Light_, Path, ImgSizeX, ImgSizeY))
}
void HorusLight::SetIesLightImage(const char* IesData, int ImgSizeX, int ImgSizeY)
{
	if (IesData == nullptr)
	{
		spdlog::error("Image data is empty");
		HorusConsole::GetInstance().AddLog(" [error] Image data is empty");

		return;
	}

	CHECK(rprIESLightSetImageFromIESdata(m_Light_, IesData, ImgSizeX, ImgSizeY))
}

// Sphere
void HorusLight::SetSphereLightRadius(float Radius)
{
	CHECK(rprSphereLightSetRadius(m_Light_, Radius))
}

// Disk
void HorusLight::SetDiskLightRadius(const float& Radius)
{
	CHECK(rprDiskLightSetRadius(m_Light_, Radius))
}
void HorusLight::SetDiskLightAngle(const float& Angle)
{
	CHECK(rprDiskLightSetAngle(m_Light_, Angle))
}
void HorusLight::SetDiskLightInnerAngle(const float& InnerAngle)
{
	CHECK(rprDiskLightSetInnerAngle(m_Light_, InnerAngle))
}

// Update transform
void HorusLight::UpdateLight()
{
	glm::mat4 Translate = glm::translate(glm::mat4(1.0f), m_Position_);
	glm::mat4 RotationX = glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation_.x), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 RotationY = glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation_.y), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 RotationZ = glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation_.z), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 Scale = glm::scale(glm::mat4(1.0f), m_Scale_);

	glm::mat4 Rotation = RotationX * RotationY * RotationZ;

	m_Transform_ = Translate * Rotation * Scale;

	rprLightSetTransform(m_Light_, RPR_FALSE, &m_Transform_[0][0]); // RPR_TRUE = transpose matrix in row
}