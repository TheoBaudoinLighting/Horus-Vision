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
	std::optional<float> base_color_weight;
	std::optional<std::variant<std::array<float, 3>, std::string>> base_color;

	std::optional<float> roughness_weight;
	std::optional<std::variant<std::array<float, 3>, std::string>> roughness;

	std::optional<float> normal_weight;
	std::optional<std::variant<std::array<float, 3>, std::string>> normal_map;

	std::optional<float> metallic_weight;
	std::optional<std::variant<std::array<float, 3>, std::string>> metallic;

	std::optional<float> emissive_weight;
	std::optional<std::variant<std::array<float, 3>, std::string>> emissive;

	std::optional<float> opacity_weight;
	std::optional<std::variant<std::array<float, 3>, std::string>> opacity;

	std::optional<float> ior_weight;
	std::optional<float> ior;
};

class HorusMaterial
{
public:

	HorusMaterial() {}

	void init();

	rpr_material_node create_material();
	void destroy_material();

	// Basic and principled material parameters

	void set_base_color(const std::string& texturePath);
	void set_base_color(const std::array<float, 3>& color);

	void set_roughness(const std::string& texturePath);
	void set_roughness(const std::array<float, 3>& color);

	void set_normal(const std::string& texturePath);
	void set_normal(const std::array<float, 3>& color);

	void set_metallic(const std::string& texturePath);
	void set_metallic(const std::array<float, 3>& color);

	void set_emissive(const std::string& texturePath);
	void set_emissive(const std::array<float, 3>& color);

	void set_opacity(const std::string& texturePath);
	void set_opacity(const std::array<float, 3>& color);

	void set_diffuse_weight(const std::array<float, 3>& color);
	void set_diffuse_weight(const std::string& texturePath);

	void set_reflection_color(const std::array<float, 3>& color);
	void set_reflection_color(const std::string& texturePath);

	void set_reflection_weight(const std::array<float, 3>& color);
	void set_reflection_weight(const std::string& texturePath);

	void set_reflection_roughness(const std::array<float, 3>& color);
	void set_reflection_roughness(const std::string& texturePath);

	void set_refraction_color(const std::array<float, 3>& color);
	void set_refraction_color(const std::string& texturePath);

	void set_refraction_weight(const std::array<float, 3>& color);
	void set_refraction_weight(const std::string& texturePath);

	void set_refraction_roughness(const std::array<float, 3>& color);
	void set_refraction_roughness(const std::string& texturePath);

	void set_coating_color(const std::array<float, 3>& color);
	void set_coating_color(const std::string& texturePath);

	void set_coating_weight(const std::array<float, 3>& color);
	void set_coating_weight(const std::string& texturePath);

	void set_coating_roughness(const std::array<float, 3>& color);
	void set_coating_roughness(const std::string& texturePath);

	void set_sheen(const std::array<float, 3>& color);
	void set_sheen(const std::string& texturePath);

	void set_sheen_tint(const std::array<float, 3>& color);
	void set_sheen_tint(const std::string& texturePath);

	void set_emission_weight(const std::array<float, 3>& color);
	void set_emission_weight(const std::string& texturePath);

	void set_transparency(const std::array<float, 3>& color);
	void set_transparency(const std::string& texturePath);

	void set_sss_scatter_color(const std::array<float, 3>& color);
	void set_sss_scatter_color(const std::string& texturePath);

	void set_sss_scatter_distance(const std::array<float, 3>& color);
	void set_sss_scatter_distance(const std::string& texturePath);

	void set_backscatter_weight(const std::array<float, 3>& color);
	void set_backscatter_weight(const std::string& texturePath);

	void set_backscatter_color(const std::array<float, 3>& color);
	void set_backscatter_color(const std::string& texturePath);

	void set_sheen_weight(const std::array<float, 3>& weight);
	void set_sheen_weight(const std::string& texturePath);

	void set_ior(float ior);

	void set_reflection_mode(int mode);
	void set_coating_mode(int mode);

	// Other ----------------------------------------

	void set_background_image(const std::string& texturePath);

	void unset_background_image();

	rpr_material_node get_material()
	{
		return m_material_;
	}

	// ----------------------------------------------
	// Texture object -------------------------------

	rpr_image load_texture(std::string path);

	// ----------------------------------------------


private:

	bool m_IsMaterialCreated = false;

	rpr_material_node m_material_ = nullptr;

};
