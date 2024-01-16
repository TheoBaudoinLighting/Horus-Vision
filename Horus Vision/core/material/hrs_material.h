#pragma once

// Basic includes
#include <map>
#include <optional>
#include <string>
#include <variant>
#include <array>

// External includes
#include <RadeonProRender_v2.h>

// Project includes
#include "hrs_garbage_collector.h"

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

	// Basic and principled material parameters

	void SetBaseColor(const std::string& texturePath);
	void SetBaseColor(const std::array<float, 3>& color);

	void SetRoughness(const std::string& TexturePath);
	void SetRoughness(const std::array<float, 3>& color);

	void SetNormal(const std::string& texturePath);
	void SetNormal(const std::array<float, 3>& color);

	void SetMetallic(const std::string& texturePath);
	void SetMetallic(const std::array<float, 3>& color);

	void SetEmissive(const std::string& texturePath);
	void SetEmissive(const std::array<float, 3>& color);

	void SetOpacity(const std::string& texturePath);
	void SetOpacity(const std::array<float, 3>& color);

	void SetDiffuseWeight(const std::array<float, 3>& color);
	void SetDiffuseWeight(const std::string& texturePath);

	void SetReflectionColor(const std::array<float, 3>& color);
	void SetReflectionColor(const std::string& texturePath);

	void SetReflectionWeight(const std::array<float, 3>& color);
	void SetReflectionWeight(const std::string& texturePath);

	void SetReflectionRoughness(const std::array<float, 3>& color);
	void SetReflectionRoughness(const std::string& texturePath);

	void SetRefractionColor(const std::array<float, 3>& color);
	void SetRefractionColor(const std::string& texturePath);

	void SetRefractionWeight(const std::array<float, 3>& color);
	void SetRefractionWeight(const std::string& texturePath);

	void SetRefractionRoughness(const std::array<float, 3>& color);
	void SetRefractionRoughness(const std::string& texturePath);

	void SetCoatingColor(const std::array<float, 3>& color);
	void SetCoatingColor(const std::string& texturePath);

	void SetCoatingWeight(const std::array<float, 3>& color);
	void SetCoatingWeight(const std::string& texturePath);

	void SetCoatingRoughness(const std::array<float, 3>& color);
	void SetCoatingRoughness(const std::string& texturePath);

	void SetSheen(const std::array<float, 3>& color);
	void SetSheen(const std::string& texturePath);

	void SetSheenTint(const std::array<float, 3>& color);
	void SetSheenTint(const std::string& texturePath);

	void SetEmissionWeight(const std::array<float, 3>& color);
	void SetEmissionWeight(const std::string& texturePath);

	void SetTransparency(const std::array<float, 3>& color);
	void SetTransparency(const std::string& texturePath);

	void SetSssScatterColor(const std::array<float, 3>& color);
	void SetSssScatterColor(const std::string& texturePath);

	void SetSssScatterDistance(const std::array<float, 3>& color);
	void SetSssScatterDistance(const std::string& texturePath);

	void SetBackscatterWeight(const std::array<float, 3>& color);
	void SetBackscatterWeight(const std::string& texturePath);

	void SetBackscatterColor(const std::array<float, 3>& color);
	void SetBackscatterColor(const std::string& texturePath);

	void SetSheenWeight(const std::array<float, 3>& weight);
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

};
