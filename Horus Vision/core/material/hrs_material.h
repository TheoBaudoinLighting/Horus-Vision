#pragma once
#pragma warning(disable : 4996)
#pragma warning(disable : 4244)
#pragma warning(disable : 4267)
#pragma warning(disable : 4005)

// Basic includes
#include <map>
#include <optional>
#include <string>
#include <variant>
#include <array>

// External includes
#include <RadeonProRender.hpp>

// Project includes
#include "hrs_garbage_collector.h"
#include "glm/fwd.hpp"
#include "glm/detail/type_vec3.hpp"
#include "glm/detail/_noise.hpp"
#include "glm/detail/_noise.hpp"
#include "glm/detail/_noise.hpp"

struct HorusMaterialParameters
{
	std::optional<float> BaseColorWeight;
	std::optional<std::variant<std::array<float, 3>, std::string>> BaseColor;

	std::optional<float> RoughnessWeight;
	std::optional<std::variant<std::array<float, 3>, std::string>> Roughness;

	std::optional<float> NormalWeight;
	std::optional<std::variant<std::array<float, 3>, std::string>> NormalMap;

	std::optional<float> MetallicWeight;
	std::optional<std::variant<std::array<float, 3>, std::string>> Metallic;

	std::optional<float> EmissiveWeight;
	std::optional<std::variant<std::array<float, 3>, std::string>> Emissive;

	std::optional<float> OpacityWeight;
	std::optional<std::variant<std::array<float, 3>, std::string>> Opacity;

	std::optional<float> IorWeight;
	std::optional<float> Ior;
};

class HorusMaterial
{
public:

	HorusMaterial() {}

	void Init();

	rpr_material_node CreateMaterial();
	void DestroyMaterial();

	// Getters
	glm::vec4 GetBaseColor() { return m_BaseColor_; }
	glm::vec4 GetRoughness() { return m_Roughness_; }
	glm::vec4 GetNormal() { return m_Normal_; }
	glm::vec4 GetMetallic() { return m_Metallic_; }
	glm::vec4 GetEmissive() { return m_Emissive_; }
	glm::vec4 GetOpacity() { return m_Opacity_; }


	// Setters
	void SetBaseColor(const std::string& TexturePath);
	void SetBaseColor(glm::vec4 Color);

	void SetRoughness(const std::string& TexturePath);
	void SetRoughness(glm::vec4 color);

	void SetNormal(const std::string& texturePath);
	void SetNormal(glm::vec4 color);

	void SetMetallic(const std::string& texturePath);
	void SetMetallic(glm::vec4 color);

	void SetEmissive(const std::string& texturePath);
	void SetEmissive(glm::vec4 color);

	void SetOpacity(const std::string& texturePath);
	void SetOpacity(glm::vec4 color);

	void SetDiffuseWeight(glm::vec4 color);
	void SetDiffuseWeight(const std::string& texturePath);

	void SetReflectionColor(glm::vec4 color);
	void SetReflectionColor(const std::string& texturePath);

	void SetReflectionWeight(glm::vec4 color);
	void SetReflectionWeight(const std::string& texturePath);

	void SetReflectionRoughness(glm::vec4 color);
	void SetReflectionRoughness(const std::string& texturePath);

	void SetRefractionColor(glm::vec4 color);
	void SetRefractionColor(const std::string& texturePath);

	void SetRefractionWeight(glm::vec4 color);
	void SetRefractionWeight(const std::string& texturePath);

	void SetRefractionRoughness(glm::vec4 color);
	void SetRefractionRoughness(const std::string& texturePath);

	void SetCoatingColor(glm::vec4 color);
	void SetCoatingColor(const std::string& texturePath);

	void SetCoatingWeight(glm::vec4 color);
	void SetCoatingWeight(const std::string& texturePath);

	void SetCoatingRoughness(glm::vec4 color);
	void SetCoatingRoughness(const std::string& texturePath);

	void SetSheen(glm::vec4 color);
	void SetSheen(const std::string& texturePath);

	void SetSheenTint(glm::vec4 color);
	void SetSheenTint(const std::string& texturePath);

	void SetEmissionWeight(glm::vec4 color);
	void SetEmissionWeight(const std::string& texturePath);

	void SetTransparency(glm::vec4 color);
	void SetTransparency(const std::string& texturePath);

	void SetSssScatterColor(glm::vec4 color);
	void SetSssScatterColor(const std::string& texturePath);

	void SetSssScatterDistance(glm::vec4 color);
	void SetSssScatterDistance(const std::string& texturePath);

	void SetBackscatterWeight(glm::vec4 color);
	void SetBackscatterWeight(const std::string& texturePath);

	void SetBackscatterColor(glm::vec4 color);
	void SetBackscatterColor(const std::string& texturePath);

	void SetSheenWeight(glm::vec4 weight);
	void SetSheenWeight(const std::string& texturePath);

	void SetIor(float ior);

	void SetReflectionMode(int mode);
	void SetCoatingMode(int mode);

	// Other ----------------------------------------

	void SetBackgroundImage(const std::string& texturePath);

	void UnsetBackgroundImage();

	rpr_material_node GetMaterial()
	{
		return m_Material_;
	}

	// ----------------------------------------------
	// Texture object -------------------------------

	rpr_image LoadTexture(std::string path);

	// ----------------------------------------------


private:

	bool m_IsMaterialCreated_ = false;

	rpr_material_node m_Material_ = nullptr;

	// Material Values (Set for performance, when the object editor calls the setters,
	// it will set the values here and don't call the Radeon functions at each frame)

	glm::vec4 m_BaseColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };
	glm::vec4 m_DiffuseWeight_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_Roughness_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_Normal_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_Metallic_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_Emissive_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_Opacity_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_ReflectionColor_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_ReflectionWeight_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_ReflectionRoughness_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_RefractionColor_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_RefractionWeight_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_RefractionRoughness_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_CoatingColor_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_CoatingWeight_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_CoatingRoughness_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_Sheen_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_SheenWeight_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_SssScatterColor_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_SssScatterDistance_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_BackscatterColor_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_BackscatterWeight_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_EmissionWeight_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_Transparency_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	float m_Ior_ = 0.0f;
	int m_ReflectionMode_ = 0;
	int m_CoatingMode_ = 0;

};
