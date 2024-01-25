#pragma once

#include <string>

#include "RadeonProRender_v2.h"
#include "common.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class HorusLight
{
public:

	HorusLight() : m_LightType_(), m_Transform_()
	{
	}

	~HorusLight() = default;

	void Init(const std::string& LightType, const std::string& ImagePath = "");

	rpr_light CreateLight(const std::string& LightType, const std::string& ImagePath = "");
	void DestroyLight();

	void GetInfo();
	void UpdateLight();

	// Getters
	// General light
	[[nodiscard]] rpr_light GetLight() const { return m_Light_; }
	[[nodiscard]] rpr_light_type GetLightType() const { return m_LightType_; }
	[[nodiscard]] glm::vec3 GetPosition() const { return m_Position_; }
	[[nodiscard]] glm::vec3 GetRotation() const { return m_Rotation_; }
	[[nodiscard]] glm::vec3 GetScale() const { return m_Scale_; }
	[[nodiscard]] glm::vec3 GetDirection() const { return m_Direction_; }
 	[[nodiscard]] glm::vec3 GetColor() const { return m_Color_; }
	[[nodiscard]] glm::vec3 GetIntensity() const { return m_Intensity_; }

	// point light
	[[nodiscard]] glm::vec3 GetPointLightRadiantPower() const { return m_PointLightRadiantPower_; }

	// directional light
	[[nodiscard]] glm::vec3 GetDirectionalLightRadiantPower() const { return m_DirectionalLightRadiantPower_; }
	[[nodiscard]] float GetDirectionalLightShadowSoftnessAngle() const { return m_DirectionalLightShadowSoftnessAngle_; }

	// spot light
	[[nodiscard]] glm::vec3 GetSpotLightRadiansPower() const { return m_SpotLightRadiansPower_; }
	[[nodiscard]] float GetSpotLightConeShape() const { return m_SpotLightConeShape_; }
	[[nodiscard]] rpr_image GetSpotLightImage() const { return m_SpotLightImage_; }

	// environment light
	[[nodiscard]] float GetEnvironmentLightIntensity() const { return m_EnvironmentLightIntensity_; }
	[[nodiscard]] float GetEnvironmentLightOverrideReflection() const { return m_EnvironmentLightOverrideReflection_; }
	[[nodiscard]] float GetEnvironmentLightOverrideRefraction() const { return m_EnvironmentLightOverrideRefraction_; }
	[[nodiscard]] float GetEnvironmentLightOverrideTransparency() const { return m_EnvironmentLightOverrideTransparency_; }
	[[nodiscard]] float GetEnvironmentLightOverrideBackground() const { return m_EnvironmentLightOverrideBackground_; }
	[[nodiscard]] float GetEnvironmentLightOverrideIrradiance() const { return m_EnvironmentLightOverrideIrradiance_; }
	[[nodiscard]] rpr_image GetEnvironmentLightImage() const { return m_EnvironmentLightImage_; }
	[[nodiscard]] int GetLightPortalNumber() const { return m_LightPortalNumber_; }
	[[nodiscard]] std::vector<rpr_light> GetLightPortalList() const { return m_LightPortalList_; }

	// Sky light
	[[nodiscard]] float GetSkyLightTurbidity() const { return m_SkyLightTurbidity_; }
	[[nodiscard]] glm::vec3 GetSkyLightAlbedo() const { return m_SkyLightAlbedo_; }
	[[nodiscard]] glm::vec3 GetSkyLightScale() const { return m_SkyLightScale_; }
	[[nodiscard]] glm::vec3 GetSkyLightDirection() const { return m_SkyLightDirection_; }
	[[nodiscard]] int GetSkyLightPortalNumber() const { return m_SkyLightPortalNumber_; }

	// Setters
	void SetLightType(const std::string& LightType);
	void SetPosition(const glm::vec3& Position);
	void SetRotation(const glm::vec3& RotationAxis);
	void SetScale(const glm::vec3& Scale);
	void SetDirection(const glm::vec3& Direction);
	void SetIntensity(glm::vec3& Intensity);
	void SetLightVisibility(bool Visibility);

	// Directional light
	void SetDirectionalLightShadowSoftnessAngle(float Coef);

	// Spot light
	void SetConeShape(float InAngle, float OutAngle);
	void SetSpotLightImage(const std::string& ImagePath);

	// Environment light
	void SetEnvironmentLightSetImage(const std::string& ImagePath);
	void SetShapeEnvironmentLight(rpr_shape Shape, bool IsEnvLight);
	void SetEnvironmentLightAttachPortal(rpr_scene Scene, rpr_light Light, rpr_shape Shape);
	void SetEnvironmentLightDetachPortal(rpr_scene Scene, rpr_light Light, rpr_shape Shape);

	// Sky light
	void SetSkyLightTurbidity(float Turbidity);
	void SetSkyLightAlbedo(const glm::vec3& Albedo);
	void SetSkyLightScale(const glm::vec3& Scale);
	void SetSkyLightDirection(const glm::vec3& Direction);
	void SetSkyLightAttachPortal(rpr_scene Scene, rpr_light Light, rpr_shape Shape);
	void SetSkyLightDetachPortal(rpr_scene Scene, rpr_light Light, rpr_shape Shape);

	// IES light
	void SetIesLightImage(const std::string& ImagePath, int ImgSizeX, int ImgSizeY);
	void SetIesLightImage(const char* ImagePath, int ImgSizeX, int ImgSizeY);

	// Sphere light
	void SetSphereLightRadius(float Radius);

	// Disk light
	void SetDiskLightRadius(const float& Radius);
	void SetDiskLightAngle(const float& Angle);
	void SetDiskLightInnerAngle(const float& InnerAngle);

private:

	// General light
	rpr_light m_Light_ = nullptr;
	rpr_light_type m_LightType_;
	bool m_IsLightVisible_ = false;
	bool m_IsLightHdri_ = false;
	glm::mat4 m_Transform_;
	glm::vec3 m_Translation_ = { 0.0f, 0.0f, 0.0f };
	glm::vec3 m_Position_ = { 0.0f, 0.0f, 0.0f };
	glm::vec3 m_Rotation_ = { 0.0f, 0.0f, 0.0f };
	glm::vec3 m_Scale_ = { 1.0f, 1.0f, 1.0f };
	glm::vec3 m_Direction_ = { 0.0f, 1.0f, 0.0f };
	glm::vec3 m_Color_ = { 1.0f, 1.0f, 1.0f };
	glm::vec3 m_Intensity_ = { 1.0f, 1.0f, 1.0f };

	std::string m_ImagePath_;

	// point light
	glm::vec3 m_PointLightRadiantPower_ = { 1.0f, 1.0f, 1.0f };

	// Directionnal Light
	glm::vec3 m_DirectionalLightRadiantPower_ = { 1.0f, 1.0f, 1.0f };
	float m_DirectionalLightShadowSoftnessAngle_ = 1.0f;

	// Spot Light
	glm::vec3 m_SpotLightRadiansPower_ =  { 1.0f, 1.0f, 1.0f };
	float m_SpotLightConeShape_ = 1.0f;
	rpr_image m_SpotLightImage_ = nullptr; // TODO: verify if it's a good type

	// environment light
	float m_EnvironmentLightIntensity_ = 1.0f;
	float m_EnvironmentLightOverrideReflection_ = 0.0f; //rpr_uint 
	float m_EnvironmentLightOverrideRefraction_ = 0.0f;
	float m_EnvironmentLightOverrideTransparency_ = 0.0f;
	float m_EnvironmentLightOverrideBackground_ = 0.0f;
	float m_EnvironmentLightOverrideIrradiance_ = 0.0f;
	rpr_image m_EnvironmentLightImage_ = nullptr; // TODO: verify if it's a good type
	// environment light - portal
	int m_LightPortalNumber_ = 0;
	std::vector<rpr_light> m_LightPortalList_ = {};

	// sky light
	float m_SkyLightTurbidity_ = 1.0f;
	glm::vec3 m_SkyLightAlbedo_ = { 1.0f, 1.0f, 1.0f };
	glm::vec3 m_SkyLightScale_ = { 1.0f, 1.0f, 1.0f };
	glm::vec3 m_SkyLightDirection_ = { 0.0f, 1.0f, 0.0f };
	// sky light - portal
	int m_SkyLightPortalNumber_ = 0;
	std::vector<rpr_light> m_SkyLightPortalList_ = {};

	// IES light
	glm::vec3 m_IesLightRadiantPower_ = { 1.0f, 1.0f, 1.0f };
	rpr_image m_IesLightImage_ = nullptr; // TODO: verify if it's a good type

	// Sphere light
	glm::vec3 m_SphereLightRadiantPower_ = { 1.0f, 1.0f, 1.0f };
	float m_SphereLightRadius_ = 1.0f;

	// Disk light
	glm::vec3 m_DiskLightRadiantPower_ = { 1.0f, 1.0f, 1.0f };
	glm::vec3 m_DiskLightRadius_ = { 1.0f, 1.0f, 1.0f };
	glm::vec3 m_DiskLightAngle_ = { 0.0f, 1.0f, 0.0f };
	glm::vec3 m_DiskLightInnerAngle_ = { 0.0f, 1.0f, 0.0f };

	//RadeonProRender::matrix m_transform_;

};
