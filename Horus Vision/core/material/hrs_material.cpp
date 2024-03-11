
#include "hrs_object_manager.h" // glad.h
#include "hrs_material.h"

#include <thread>
#include <mutex>

#include "common.h"
#include "hrs_radeon.h"
#include <hrs_console.h>

#include "glm/detail/_noise.hpp"
#include "glm/detail/_noise.hpp"

std::mutex MaterialMutex;

// Material management ---------------------------------------------------------------------------

void HorusMaterial::Init()
{
	HorusGarbageCollector& Gc = HorusGarbageCollector::GetInstance();

	m_Material_ = CreateMaterial();

	Gc.Add(m_Material_);
}

rpr_material_node HorusMaterial::CreateMaterial()
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& Gc = HorusGarbageCollector::GetInstance();

	rpr_material_node MaterialNodeUberv2 = nullptr;

	CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_UBERV2, &MaterialNodeUberv2))
		Gc.Add(MaterialNodeUberv2);

	//CHECK(rprObjectSetName(material_node_uberv2, "Uber_0"));

	// Base Color 
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR, 0.5f, 0.5f, 0.5f, 0.5f));
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_DIFFUSE_WEIGHT, 1.0f, 1.0f, 1.0f, 1.0f));
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_DIFFUSE_ROUGHNESS, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_BACKSCATTER_COLOR, 1.0f, 1.0f, 1.0f, 1.0f));
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_BACKSCATTER_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f));

	// Reflection
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_REFLECTION_COLOR, 1.f, 1.f, 1.f, 1.f));
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_REFLECTION_WEIGHT, 1.0f, 1.0f, 1.0f, 1.0f));
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_REFLECTION_ROUGHNESS, 0.5f, 0.5f, 0.5f, 0.5f));
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_REFLECTION_ANISOTROPY, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_REFLECTION_ANISOTROPY_ROTATION, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputUByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_REFLECTION_MODE, RPR_UBER_MATERIAL_IOR_MODE_METALNESS));
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_REFLECTION_IOR, 1.36f, 1.36f, 1.36f, 1.36f));
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_REFLECTION_METALNESS, 0.0f, 0.0f, 0.0f, 0.0f));

	// Sheen
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_SHEEN, 1.0f, 1.0f, 1.0f, 1.0f));
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_SHEEN_TINT, 1.0f, 1.0f, 1.0f, 1.0f));
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_SHEEN_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f));

	// Refraction
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_REFRACTION_COLOR, 1.0f, 1.0f, 1.0f, 1.0f));
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_REFRACTION_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_REFRACTION_ROUGHNESS, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_REFRACTION_IOR, 1.500000f, 1.500000f, 1.500000f, 1.500000f));
	CHECK(rprMaterialNodeSetInputUByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_REFRACTION_THIN_SURFACE, 0));
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_REFRACTION_ABSORPTION_COLOR, 1.0f, 1.0f, 1.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_REFRACTION_ABSORPTION_DISTANCE, 1.0f, 1.0f, 1.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputUByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_REFRACTION_CAUSTICS, 0));

	// SSS
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_COLOR, 1.0f, 1.0f, 1.0f, 1.0f));
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_DISTANCE, 1.0f, 1.0f, 1.0f, 1.0f));
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_DIRECTION, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_SSS_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputUByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_SSS_MULTISCATTER, 0));
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_SCHLICK_APPROXIMATION, 1.0f, 1.0f, 1.0f, 1.0f));

	// Coating
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_COATING_COLOR, 1.0f, 1.0f, 1.0f, 1.0f));
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_COATING_WEIGHT, 0.f, 0.f, 0.f, 0.f));
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_COATING_ROUGHNESS, 0.f, 0.f, 0.f, 0.f));
	CHECK(rprMaterialNodeSetInputUByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_COATING_MODE, RPR_UBER_MATERIAL_IOR_MODE_PBR));
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_COATING_IOR, 1.460000f, 1.460000f, 1.460000f, 1.460000f));
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_COATING_METALNESS, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_COATING_TRANSMISSION_COLOR, 0.0f, 0.0f, 0.0f, 1.0f));
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_COATING_THICKNESS, 0.0f, 0.0f, 0.0f, 0.0f));

	// Other
	// Normal Map
	/*CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_DIFFUSE_NORMAL, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_REFLECTION_NORMAL, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_REFRACTION_NORMAL, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_COATING_NORMAL, 0.0f, 0.0f, 0.0f, 0.0f));*/
	//CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_SCALE, 1.0f, 1.0f, 1.0f, 1.0f));

	// Emission
	/*CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_EMISSION_COLOR, 1.0f, 1.0f, 1.0f, 1.0f));
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_EMISSION_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputUByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_EMISSION_MODE, 0));*/


	//CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_TRANSPARENCY, 0.0f, 0.0f, 0.0f, 0.0f
	//))




	return MaterialNodeUberv2;
}

void HorusMaterial::DestroyMaterial()
{
	//CHECK(rprObjectDelete(m_Material_))

	if (m_Material_ != nullptr)
	{
		m_Material_ = nullptr;
	}
}

// Textures management 

bool IsUDIMPath(const std::string& path)
{
	return path.find("<UDIM>") != std::string::npos;
}
void HorusMaterial::LoadAndApplyUDIMTexture(const std::string& UDIMRootPath, const std::vector<std::pair<int, std::string>>& UDIMTiles)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& Gc = HorusGarbageCollector::GetInstance();

	rpr_image_format Format = { 0, RPR_COMPONENT_TYPE_UINT8 };
	rpr_image_desc Desc = {};
	rpr_image ImageUdimRoot;
	CHECK(rprContextCreateImage(Radeon.GetContext(), Format, &Desc, nullptr, &ImageUdimRoot));

	for (const auto& tile : UDIMTiles)
	{
		rpr_image imageTile = LoadTexture(tile.second);
		CHECK(rprImageSetUDIM(ImageUdimRoot, tile.first, imageTile));
		Gc.Add(imageTile);
	}

	// TODO : Apply the UDIM texture to the material

}
rpr_image HorusMaterial::LoadTexture(std::string Path)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	rpr_image Image = nullptr;

	if (rprContextCreateImageFromFile(Radeon.GetContext(), Path.c_str(), &Image) != RPR_SUCCESS)
	{
		spdlog::error("Error: Texture -> {} not found.", Path);
		Console.AddLog(" [error] Texture -> %s not found.", Path.c_str());
		return nullptr;
	}

	if (Image == nullptr)
	{
		spdlog::error("Error: Texture -> {} not found.", Path);
		Console.AddLog(" [error] Texture -> %s not found.", Path.c_str());
		return nullptr;
	}

	/*if (IsUDIMPath(Path))
	{
		spdlog::info("Texture -> {} is a UDIM texture.", Path);
		Console.AddLog(" [info] Texture -> %s is a UDIM texture.", Path.c_str());

		CHECK(rprImageSetUDIM(Image, 1001, Image));
	}*/

	//CHECK(rprImageSetOcioColorspace(Image, "Utility - Linear - sRGB"));
	CHECK(rprImageSetOcioColorspace(Image, "Utility - Raw"));

	gc.Add(Image);

	spdlog::info("Texture -> {} loaded.", Path);
	Console.AddLog(" [info] Texture -> %s loaded.", Path.c_str());

	return Image;
}


// Setters
// Base Color
void HorusMaterial::SetBaseColor(glm::vec4 Color)
{
	m_BaseColor_ = Color;
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR, Color.x, Color.y, Color.z, Color.w));
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_COLOR, Color.x, Color.y, Color.z, Color.w));
}
void HorusMaterial::SetBaseColor(const std::string& TexturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& Gc = HorusGarbageCollector::GetInstance();

	if (TexturePath.empty())
	{
		m_BaseColor_ = { 0.5f, 0.5f, 0.5f, 1.0f };
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR, 0.5f, 0.5f, 0.5f, 1.0f))
	}
	else
	{
		m_BaseColorPath_ = TexturePath;
		std::thread LoadTextureThread([&]()
			{
				std::lock_guard<std::mutex> Lock(MaterialMutex);

				rpr_image TextureBaseColor = LoadTexture(TexturePath);
				rpr_material_node UberMat2ImgBasecolorTexture = nullptr;

				if (TextureBaseColor == nullptr) {
					spdlog::error("Error: Texture -> {} is null.", TexturePath);

					m_BaseColor_ = { 0.5f, 0.5f, 0.5f, 1.0f };
					if (rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR, 0.5f, 0.5f, 0.5f, 1.0f) != RPR_SUCCESS)
					{
						spdlog::warn("Warning: Texture -> {} not found or not usable.", TexturePath);
						spdlog::warn("Warning: Texture replacing by default color.");
						return;
					}
					return;
				}
				if (rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &UberMat2ImgBasecolorTexture) != RPR_SUCCESS)
				{
					spdlog::warn("Warning: Texture -> {} not found.", TexturePath);
					m_BaseColor_ = { 0.5f, 0.5f, 0.5f, 1.0f };
					if (rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR, 0.5f, 0.5f, 0.5f, 1.0f) != RPR_SUCCESS)
					{
						spdlog::warn("Warning: Texture -> {} not found or not usable.", TexturePath);
						spdlog::warn("Warning: Texture replacing by default color.");
						return;
					}
					return;
				}

				Gc.Add(UberMat2ImgBasecolorTexture);

				if (rprMaterialNodeSetInputImageDataByKey(UberMat2ImgBasecolorTexture, RPR_MATERIAL_INPUT_DATA, TextureBaseColor) != RPR_SUCCESS)
				{
					spdlog::warn("Warning: Texture -> {} not found.", TexturePath);
					if (rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR, 0.5f, 0.5f, 0.5f, 1.0f) != RPR_SUCCESS)
					{
						spdlog::warn("Warning: Texture -> {} not found or not usable.", TexturePath);
						spdlog::warn("Warning: Texture replacing by default color.");
						return;
					}
					return;
				}
				if (rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR, UberMat2ImgBasecolorTexture) != RPR_SUCCESS)
				{
					spdlog::warn("Warning: Texture -> {} not found.", TexturePath);
					if (rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR, 0.5f, 0.5f, 0.5f, 1.0f) != RPR_SUCCESS)
					{
						spdlog::warn("Warning: Texture -> {} not found or not usable.", TexturePath);
						spdlog::warn("Warning: Texture replacing by default color.");
						return;
					}
					return;
				}
				if (rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_COLOR, UberMat2ImgBasecolorTexture) != RPR_SUCCESS)
				{
					spdlog::warn("Warning: Texture -> {} not found.", TexturePath);
					if (rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_COLOR, 0.5f, 0.5f, 0.5f, 1.0f) != RPR_SUCCESS)
					{
						spdlog::warn("Warning: Texture -> {} not found or not usable.", TexturePath);
						spdlog::warn("Warning: Texture replacing by default color.");
					}
				}
			});
		LoadTextureThread.join();
	}
}
void HorusMaterial::SetBaseColorWeight(glm::vec4 Color)
{
	m_BaseColorWeight_ = Color;
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_DIFFUSE_WEIGHT, Color.x, Color.y, Color.z, Color.w));
}
void HorusMaterial::SetBaseColorWeight(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		m_BaseColorWeight_ = { 0.0f, 0.0f, 0.0f, 0.0f };
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_DIFFUSE_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		m_BaseColorWeightPath_ = texturePath;
		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(MaterialMutex);

				rpr_image texture_diffuse_weight = LoadTexture(texturePath);

				rpr_material_node uberMat2_img_diffuse_weight_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &
					uberMat2_img_diffuse_weight_texture))
					gc.Add(uberMat2_img_diffuse_weight_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_diffuse_weight_texture, RPR_MATERIAL_INPUT_DATA,
					texture_diffuse_weight))

					CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_DIFFUSE_WEIGHT,
						uberMat2_img_diffuse_weight_texture))
			});

		loadTextureThread.join();
	}
}
void HorusMaterial::SetBaseColorRoughness(glm::vec4 Color)
{
	m_BaseColorRoughness_ = Color;
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_DIFFUSE_ROUGHNESS, Color.x, Color.y, Color.z, Color.w));
}
void HorusMaterial::SetBaseColorRoughness(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		m_BaseColorRoughness_ = { 0.0f, 0.0f, 0.0f, 0.0f };
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_DIFFUSE_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f))
	}
	else
	{
		m_BaseColorRoughnessPath_ = texturePath;
		std::thread LoadTextureThread([&]()
			{
				std::lock_guard Lock(MaterialMutex);

				rpr_image TextureDiffuseRoughness = LoadTexture(texturePath);

				rpr_material_node UberMat2ImgDiffuseRoughnessTexture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &
					UberMat2ImgDiffuseRoughnessTexture))
					gc.Add(UberMat2ImgDiffuseRoughnessTexture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(UberMat2ImgDiffuseRoughnessTexture, RPR_MATERIAL_INPUT_DATA,
					TextureDiffuseRoughness))

					CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_DIFFUSE_WEIGHT,
						UberMat2ImgDiffuseRoughnessTexture))
			});

		LoadTextureThread.join();
	}
}
void HorusMaterial::SetBackscatterWeight(glm::vec4 color)
{
	m_BackscatterWeight_ = color;
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_BACKSCATTER_WEIGHT, color.x, color.y, color.z, color.w));
}
void HorusMaterial::SetBackscatterWeight(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		m_BackscatterWeight_ = { 0.0f, 0.0f, 0.0f, 0.0f };
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_BACKSCATTER_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f))
	}
	else
	{
		m_BackscatterWeightPath_ = texturePath;
		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(MaterialMutex);

				rpr_image texture_backscatter_weight = LoadTexture(texturePath);

				rpr_material_node uberMat2_img_backscatter_weight_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &
					uberMat2_img_backscatter_weight_texture))
					gc.Add(uberMat2_img_backscatter_weight_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_backscatter_weight_texture, RPR_MATERIAL_INPUT_DATA
					, texture_backscatter_weight))

					CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_BACKSCATTER_WEIGHT,
						uberMat2_img_backscatter_weight_texture))
			});

		loadTextureThread.join();
	}
}
void HorusMaterial::SetBackscatterColor(glm::vec4 color)
{
	m_BackscatterColor_ = color;
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_BACKSCATTER_COLOR, color.x, color.y, color.z, color.w));
}
void HorusMaterial::SetBackscatterColor(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		m_BackscatterColor_ = { 0.5f, 0.5f, 0.5f, 1.0f };
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_BACKSCATTER_COLOR, 0.0f, 0.0f, 0.0f, 0.0f))
	}
	else
	{
		m_BackscatterColorPath_ = texturePath;
		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(MaterialMutex);

				rpr_image texture_backscatter_color = LoadTexture(texturePath);

				rpr_material_node uberMat2_img_backscatter_color_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &
					uberMat2_img_backscatter_color_texture))
					gc.Add(uberMat2_img_backscatter_color_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_backscatter_color_texture, RPR_MATERIAL_INPUT_DATA,
					texture_backscatter_color))

					CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_BACKSCATTER_COLOR,
						uberMat2_img_backscatter_color_texture))
			});

		loadTextureThread.join();
	}
}

// Reflection
void HorusMaterial::SetReflectionColor(glm::vec4 color)
{
	m_ReflectionColor_ = color;
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_COLOR, color.x, color.y, color.z, color.w));
}
void HorusMaterial::SetReflectionColor(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		m_RefractionColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_COLOR, 0.0f, 0.0f, 0.0f, 0.0f))
	}
	else
	{
		m_ReflectionColorPath_ = texturePath;
		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(MaterialMutex);

				rpr_image texture_reflection_color = LoadTexture(texturePath);

				rpr_material_node uberMat2_img_reflection_color_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &
					uberMat2_img_reflection_color_texture))
					gc.Add(uberMat2_img_reflection_color_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_reflection_color_texture, RPR_MATERIAL_INPUT_DATA,
					texture_reflection_color))

					CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_COLOR,
						uberMat2_img_reflection_color_texture))
			});

		loadTextureThread.join();
	}
}
void HorusMaterial::SetReflectionRoughness(glm::vec4 color)
{
	m_ReflectionRoughness_ = color;
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_ROUGHNESS, color.x, color.y, color.z, color.w));
}
void HorusMaterial::SetReflectionRoughness(const std::string& TexturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& Gc = HorusGarbageCollector::GetInstance();

	if (TexturePath.empty())
	{
		m_ReflectionRoughness_ = { 0.5f, 0.5f, 0.5f, 0.5f };
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_ROUGHNESS, 0.5f, 0.5f, 0.5f, 1.0f));
	}
	else
	{
		m_ReflectionRoughnessPath_ = TexturePath;
		std::thread LoadTextureThread([&]()
			{
				std::lock_guard<std::mutex> Lock(MaterialMutex);

				rpr_image TextureRoughness = LoadTexture(TexturePath);

				rpr_material_node UberMat2ImgRoughnessTexture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &UberMat2ImgRoughnessTexture));
				Gc.Add(UberMat2ImgRoughnessTexture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(UberMat2ImgRoughnessTexture, RPR_MATERIAL_INPUT_DATA, TextureRoughness));

				CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_ROUGHNESS, UberMat2ImgRoughnessTexture));
			});

		LoadTextureThread.join();
	}
}
void HorusMaterial::SetReflectionWeight(glm::vec4 Color)
{
	m_ReflectionWeight_ = Color;
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_WEIGHT, Color.x, Color.y, Color.z, Color.w));
}
void HorusMaterial::SetReflectionWeight(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		m_ReflectionWeight_ = { 0.0f, 0.0f, 0.0f, 0.0f };
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		m_ReflectionWeightPath_ = texturePath;
		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(MaterialMutex);

				rpr_image texture_reflection_weight = LoadTexture(texturePath);

				rpr_material_node uberMat2_img_reflection_weight_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &
					uberMat2_img_reflection_weight_texture))
					gc.Add(uberMat2_img_reflection_weight_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_reflection_weight_texture, RPR_MATERIAL_INPUT_DATA,
					texture_reflection_weight))

					CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_WEIGHT,
						uberMat2_img_reflection_weight_texture))
			});

		loadTextureThread.join();
	}
}
void HorusMaterial::SetReflectionAnisotropy(glm::vec4 color)
{
	m_ReflectionAnisotropy_ = color;
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_ANISOTROPY, color.x, color.y, color.z, color.w));
}
void HorusMaterial::SetReflectionAnisotropy(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		m_ReflectionAnisotropy_ = { 0.0f, 0.0f, 0.0f, 0.0f };
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_ANISOTROPY, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		m_ReflectionAnisotropyPath_ = texturePath;
		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(MaterialMutex);

				rpr_image texture_reflection_weight = LoadTexture(texturePath);

				rpr_material_node uberMat2_img_reflection_weight_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &
					uberMat2_img_reflection_weight_texture))
					gc.Add(uberMat2_img_reflection_weight_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_reflection_weight_texture, RPR_MATERIAL_INPUT_DATA,
					texture_reflection_weight))

					CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_ANISOTROPY,
						uberMat2_img_reflection_weight_texture))
			});

		loadTextureThread.join();
	}

}
void HorusMaterial::SetReflectionAnisotropyRotation(glm::vec4 color)
{
	m_ReflectionAnisotropyRotation_ = color;
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_ANISOTROPY_ROTATION, color.x, color.y, color.z, color.w));
}
void HorusMaterial::SetReflectionAnisotropyRotation(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& Gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		m_ReflectionAnisotropyRotation_ = { 0.0f, 0.0f, 0.0f, 0.0f };
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_ANISOTROPY_ROTATION, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		m_ReflectionAnisotropyRotationPath_ = texturePath;
		std::thread LoadTextureThread([&]()
			{
				std::lock_guard<std::mutex> Lock(MaterialMutex);

				rpr_image TextureAnisotropyRotation = LoadTexture(texturePath);

				rpr_material_node UberMat2ImgAnisotropyRotationTexture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &UberMat2ImgAnisotropyRotationTexture));
				Gc.Add(UberMat2ImgAnisotropyRotationTexture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(UberMat2ImgAnisotropyRotationTexture, RPR_MATERIAL_INPUT_DATA, TextureAnisotropyRotation));

				CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_ANISOTROPY_ROTATION, UberMat2ImgAnisotropyRotationTexture));
			});

		LoadTextureThread.join();
	}
}
void HorusMaterial::SetReflectionMode(ReflectionType mode)
{
	if (mode == 0)
	{
		m_ReflectionMode_ = ReflectionTypePBR;
		CHECK(rprMaterialNodeSetInputUByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_MODE, RPR_UBER_MATERIAL_IOR_MODE_PBR));
	}
	else if (mode == 1)
	{
		m_ReflectionMode_ = ReflectionTypeMetalness;
		CHECK(rprMaterialNodeSetInputUByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_MODE, RPR_UBER_MATERIAL_IOR_MODE_METALNESS));
	}
}
void HorusMaterial::SetReflectionMetalness(glm::vec4 color)
{
	m_ReflectionMetalness_ = color;
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_METALNESS, color.x, color.y, color.z, color.w));
}
void HorusMaterial::SetReflectionMetalness(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& Gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		m_ReflectionMetalness_ = { 0.0f, 0.0f, 0.0f, 0.0f };
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_METALNESS, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		m_ReflectionMetalnessPath_ = texturePath;
		std::thread LoadTextureThread([&]()
			{
				std::lock_guard<std::mutex> Lock(MaterialMutex);

				rpr_image TextureMetalness = LoadTexture(texturePath);

				rpr_material_node UberMat2ImgMetalnessTexture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &UberMat2ImgMetalnessTexture));
				Gc.Add(UberMat2ImgMetalnessTexture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(UberMat2ImgMetalnessTexture, RPR_MATERIAL_INPUT_DATA, TextureMetalness));

				CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_METALNESS, UberMat2ImgMetalnessTexture));
			});

		LoadTextureThread.join();
	}
}

// Sheen
void HorusMaterial::SetSheenColor(glm::vec4 color)
{
	m_SheenColor_ = color;
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_SHEEN, color.x, color.y, color.z, color.w));
}
void HorusMaterial::SetSheenColor(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		m_SheenColor_ = { 0.0f, 0.0f, 0.0f, 0.0f };
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_SHEEN, 0.0f, 0.0f, 0.0f, 0.0f))
	}
	else
	{
		m_SheenColorPath_ = texturePath;
		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(MaterialMutex);

				rpr_image texture_sheen = LoadTexture(texturePath);

				rpr_material_node uberMat2_img_sheen_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &
					uberMat2_img_sheen_texture))
					gc.Add(uberMat2_img_sheen_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_sheen_texture, RPR_MATERIAL_INPUT_DATA,
					texture_sheen))

					CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_SHEEN, uberMat2_img_sheen_texture))
			});

		loadTextureThread.join();
	}
}
void HorusMaterial::SetSheenTint(glm::vec4 color)
{
	m_SheenTint_ = color;
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_SHEEN_TINT, color.x, color.y, color.z, color.w));
}
void HorusMaterial::SetSheenTint(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		m_SheenTint_ = { 0.0f, 0.0f, 0.0f, 0.0f };
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_SHEEN_TINT, 0.0f, 0.0f, 0.0f, 0.0f))
	}
	else
	{
		m_SheenTintPath_ = texturePath;
		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(MaterialMutex);

				rpr_image texture_sheen_tint = LoadTexture(texturePath);

				rpr_material_node uberMat2_img_sheen_tint_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &
					uberMat2_img_sheen_tint_texture))
					gc.Add(uberMat2_img_sheen_tint_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_sheen_tint_texture, RPR_MATERIAL_INPUT_DATA,
					texture_sheen_tint))

					CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_SHEEN_TINT,
						uberMat2_img_sheen_tint_texture))
			});

		loadTextureThread.join();
	}
}
void HorusMaterial::SetSheenWeight(glm::vec4 weight)
{
	m_SheenWeight_ = weight;
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_SHEEN_WEIGHT, weight.x, weight.y, weight.z, weight.w));
}
void HorusMaterial::SetSheenWeight(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		m_SheenWeight_ = { 0.0f, 0.0f, 0.0f, 0.0f };
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_SHEEN_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		m_SheenWeightPath_ = texturePath;
		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(MaterialMutex);

				rpr_image texture_sheen_weight = LoadTexture(texturePath);

				rpr_material_node uberMat2_img_sheen_weight_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &
					uberMat2_img_sheen_weight_texture))
					gc.Add(uberMat2_img_sheen_weight_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_sheen_weight_texture, RPR_MATERIAL_INPUT_DATA,
					texture_sheen_weight))

					CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_SHEEN_WEIGHT,
						uberMat2_img_sheen_weight_texture))
			});

		loadTextureThread.join();
	}
}

// Refraction
void HorusMaterial::SetRefractionColor(glm::vec4 color)
{
	m_RefractionColor_ = color;
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_COLOR, color.x, color.y, color.z, color.w));
}
void HorusMaterial::SetRefractionColor(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		m_RefractionColor_ = { 0.0f, 0.0f, 0.0f, 0.0f };
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_COLOR, 0.0f, 0.0f, 0.0f, 0.0f))
	}
	else
	{
		m_RefractionColorPath_ = texturePath;
		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(MaterialMutex);

				rpr_image texture_refraction_color = LoadTexture(texturePath);

				rpr_material_node uberMat2_img_refraction_color_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &
					uberMat2_img_refraction_color_texture))
					gc.Add(uberMat2_img_refraction_color_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_refraction_color_texture, RPR_MATERIAL_INPUT_DATA,
					texture_refraction_color))

					CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_COLOR,
						uberMat2_img_refraction_color_texture))
			});

		loadTextureThread.join();
	}
}
void HorusMaterial::SetRefractionWeight(glm::vec4 color)
{
	m_RefractionWeight_ = color;
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_WEIGHT, color.x, color.y, color.z, color.w));
}
void HorusMaterial::SetRefractionWeight(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		m_RefractionWeight_ = { 0.0f, 0.0f, 0.0f, 0.0f };
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		m_RefractionWeightPath_ = texturePath;
		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(MaterialMutex);

				rpr_image texture_refraction_weight = LoadTexture(texturePath);

				rpr_material_node uberMat2_img_refraction_weight_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &
					uberMat2_img_refraction_weight_texture))
					gc.Add(uberMat2_img_refraction_weight_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_refraction_weight_texture, RPR_MATERIAL_INPUT_DATA,
					texture_refraction_weight))

					CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_WEIGHT,
						uberMat2_img_refraction_weight_texture))
			});

		loadTextureThread.join();
	}
}
void HorusMaterial::SetRefractionRoughness(glm::vec4 color)
{
	m_RefractionRoughness_ = color;
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_ROUGHNESS, color.x, color.y, color.z, color.w));
}
void HorusMaterial::SetRefractionRoughness(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		m_RefractionRoughness_ = { 0.0f, 0.0f, 0.0f, 0.0f };
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_ROUGHNESS, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		m_RefractionRoughnessPath_ = texturePath;
		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(MaterialMutex);

				rpr_image texture_refraction_roughness = LoadTexture(texturePath);

				rpr_material_node uberMat2_img_refraction_roughness_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &
					uberMat2_img_refraction_roughness_texture))
					gc.Add(uberMat2_img_refraction_roughness_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_refraction_roughness_texture,
					RPR_MATERIAL_INPUT_DATA, texture_refraction_roughness))

					CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_ROUGHNESS,
						uberMat2_img_refraction_roughness_texture))
			});

		loadTextureThread.join();
	}
}
void HorusMaterial::SetRefractionNormal(glm::vec4 color)
{
	m_RefractionNormal_ = color;
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_NORMAL, color.x, color.y, color.z, color.w));
}
void HorusMaterial::SetRefractionNormal(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& Gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		m_RefractionNormal_ = { 0.0f, 0.0f, 0.0f, 0.0f };
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_NORMAL, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		m_RefractionNormalPath_ = texturePath;
		std::thread LoadTextureThread([&]()
			{
				std::lock_guard Lock(MaterialMutex);

				rpr_image TextureNormal = LoadTexture(texturePath);

				rpr_material_node UberMat2ImgNormalTexture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &UberMat2ImgNormalTexture));
				Gc.Add(UberMat2ImgNormalTexture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(UberMat2ImgNormalTexture, RPR_MATERIAL_INPUT_DATA, TextureNormal));

				CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_NORMAL, UberMat2ImgNormalTexture));
			});

		LoadTextureThread.join();
	}
}
void HorusMaterial::SetRefractionAbsorptionColor(glm::vec4 color)
{
	m_RefractionAbsorptionColor_ = color;
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_ABSORPTION_COLOR, color.x, color.y, color.z, color.w));
}
void HorusMaterial::SetRefractionAbsorptionColor(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& Gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		m_RefractionAbsorptionColor_ = { 0.0f, 0.0f, 0.0f, 0.0f };
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_ABSORPTION_COLOR, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		m_RefractionAbsorptionColorPath_ = texturePath;
		std::thread LoadTextureThread([&]()
			{
				std::lock_guard Lock(MaterialMutex);

				rpr_image TextureAbsorptionColor = LoadTexture(texturePath);

				rpr_material_node UberMat2ImgAbsorptionColorTexture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &UberMat2ImgAbsorptionColorTexture));
				Gc.Add(UberMat2ImgAbsorptionColorTexture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(UberMat2ImgAbsorptionColorTexture, RPR_MATERIAL_INPUT_DATA, TextureAbsorptionColor));

				CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_ABSORPTION_COLOR, UberMat2ImgAbsorptionColorTexture));
			});

		LoadTextureThread.join();
	}
}
void HorusMaterial::SetRefractionAbsorptionDistance(glm::vec4 color)
{
	m_RefractionAbsorptionDistance_ = color;
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_ABSORPTION_DISTANCE, color.x, color.y, color.z, color.w));
}
void HorusMaterial::SetRefractionAbsorptionDistance(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& Gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		m_RefractionAbsorptionDistance_ = { 0.0f, 0.0f, 0.0f, 0.0f };
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_ABSORPTION_DISTANCE, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		m_RefractionAbsorptionDistancePath_ = texturePath;
		std::thread LoadTextureThread([&]()
			{
				std::lock_guard Lock(MaterialMutex);

				rpr_image TextureAbsorptionDistance = LoadTexture(texturePath);

				rpr_material_node UberMat2ImgAbsorptionDistanceTexture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &UberMat2ImgAbsorptionDistanceTexture));
				Gc.Add(UberMat2ImgAbsorptionDistanceTexture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(UberMat2ImgAbsorptionDistanceTexture, RPR_MATERIAL_INPUT_DATA, TextureAbsorptionDistance));

				CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_ABSORPTION_DISTANCE, UberMat2ImgAbsorptionDistanceTexture));
			});

		LoadTextureThread.join();
	}
}

// SSS
void HorusMaterial::SetSssScatterColor(glm::vec4 color)
{
	m_SssScatterColor_ = color;
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_COLOR, color.x, color.y, color.z, color.w));
}
void HorusMaterial::SetSssScatterColor(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		m_SssScatterColor_ = { 0.0f, 0.0f, 0.0f, 0.0f };
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_COLOR, 0.0f, 0.0f, 0.0f, 0.0f))
	}
	else
	{
		m_SssScatterColorPath_ = texturePath;
		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(MaterialMutex);

				rpr_image texture_sss_scatter_color = LoadTexture(texturePath);

				rpr_material_node uberMat2_img_sss_scatter_color_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &
					uberMat2_img_sss_scatter_color_texture))
					gc.Add(uberMat2_img_sss_scatter_color_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_sss_scatter_color_texture, RPR_MATERIAL_INPUT_DATA,
					texture_sss_scatter_color))

					CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_COLOR,
						uberMat2_img_sss_scatter_color_texture))
			});

		loadTextureThread.join();
	}
}
void HorusMaterial::SetSssScatterWeight(glm::vec4 color)
{
	m_SssScatterWeight_ = color;
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_SSS_WEIGHT, color.x, color.y, color.z, color.w));
}
void HorusMaterial::SetSssScatterWeight(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		m_SssScatterWeight_ = { 0.0f, 0.0f, 0.0f, 0.0f };
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_SSS_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f))
	}
	else
	{
		m_SssScatterWeightPath_ = texturePath;
		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(MaterialMutex);

				rpr_image texture_sss_scatter_weight = LoadTexture(texturePath);

				rpr_material_node uberMat2_img_sss_scatter_weight_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &uberMat2_img_sss_scatter_weight_texture));
				gc.Add(uberMat2_img_sss_scatter_weight_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_sss_scatter_weight_texture, RPR_MATERIAL_INPUT_DATA, texture_sss_scatter_weight));

				CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_SSS_WEIGHT, uberMat2_img_sss_scatter_weight_texture));
			});

		loadTextureThread.join();
	}

}
void HorusMaterial::SetSssScatterDistance(glm::vec4 color)
{
	m_SssScatterDistance_ = color;
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_DISTANCE, color.x, color.y, color.z, color.w));
}
void HorusMaterial::SetSssScatterDistance(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		m_SssScatterDistance_ = { 0.0f, 0.0f, 0.0f, 0.0f };
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_DISTANCE, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		m_SssScatterDistancePath_ = texturePath;
		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(MaterialMutex);

				rpr_image texture_sss_scatter_distance = LoadTexture(texturePath);

				rpr_material_node uberMat2_img_sss_scatter_distance_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &
					uberMat2_img_sss_scatter_distance_texture))
					gc.Add(uberMat2_img_sss_scatter_distance_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_sss_scatter_distance_texture,
					RPR_MATERIAL_INPUT_DATA, texture_sss_scatter_distance))

					CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_DISTANCE,
						uberMat2_img_sss_scatter_distance_texture))
			});

		loadTextureThread.join();
	}
}
void HorusMaterial::SetSssScatterDirection(glm::vec4 color)
{
	m_SssScatterDirection_ = color;
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_DIRECTION, color.x, color.y, color.z, color.w));
}
void HorusMaterial::SetSssScatterDirection(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& Gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		m_SssScatterDirection_ = { 0.0f, 0.0f, 0.0f, 0.0f };
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_DIRECTION, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		m_SssScatterDirectionPath_ = texturePath;
		std::thread LoadTextureThread([&]()
			{
				std::lock_guard Lock(MaterialMutex);

				rpr_image TextureScatterDirection = LoadTexture(texturePath);

				rpr_material_node UberMat2ImgScatterDirectionTexture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &UberMat2ImgScatterDirectionTexture));
				Gc.Add(UberMat2ImgScatterDirectionTexture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(UberMat2ImgScatterDirectionTexture, RPR_MATERIAL_INPUT_DATA, TextureScatterDirection));

				CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_DIRECTION, UberMat2ImgScatterDirectionTexture));
			});

		LoadTextureThread.join();
	}

}

// Coating
void HorusMaterial::SetCoatingColor(glm::vec4 color)
{
	m_CoatingColor_ = color;
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_COATING_COLOR, color.x, color.y, color.z, color.w));
}
void HorusMaterial::SetCoatingColor(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		m_CoatingColor_ = { 0.0f, 0.0f, 0.0f, 0.0f };
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_COATING_COLOR, 0.0f, 0.0f, 0.0f, 0.0f))
	}
	else
	{
		m_CoatingColorPath_ = texturePath;
		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(MaterialMutex);

				rpr_image texture_coating_color = LoadTexture(texturePath);

				rpr_material_node uberMat2_img_coating_color_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &
					uberMat2_img_coating_color_texture))
					gc.Add(uberMat2_img_coating_color_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_coating_color_texture, RPR_MATERIAL_INPUT_DATA,
					texture_coating_color))

					CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_COATING_COLOR,
						uberMat2_img_coating_color_texture))
			});

		loadTextureThread.join();
	}
}
void HorusMaterial::SetCoatingWeight(glm::vec4 color)
{
	m_CoatingWeight_ = color;
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_COATING_WEIGHT, color.x, color.y, color.z, color.w));
}
void HorusMaterial::SetCoatingWeight(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		m_CoatingWeight_ = { 0.0f, 0.0f, 0.0f, 0.0f };
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_COATING_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f
		));
	}
	else
	{
		m_CoatingWeightPath_ = texturePath;
		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(MaterialMutex);

				rpr_image texture_coating_weight = LoadTexture(texturePath);

				rpr_material_node uberMat2_img_coating_weight_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &
					uberMat2_img_coating_weight_texture))
					gc.Add(uberMat2_img_coating_weight_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_coating_weight_texture, RPR_MATERIAL_INPUT_DATA,
					texture_coating_weight))

					CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_COATING_WEIGHT,
						uberMat2_img_coating_weight_texture))
			});

		loadTextureThread.join();
	}
}
void HorusMaterial::SetCoatingRoughness(glm::vec4 color)
{
	m_CoatingRoughness_ = color;
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_COATING_ROUGHNESS, color.x, color.y,
		color.z, color.w));
}
void HorusMaterial::SetCoatingRoughness(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		m_CoatingRoughness_ = { 0.0f, 0.0f, 0.0f, 0.0f };
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_COATING_ROUGHNESS, 0.0f, 0.0f, 0.0f,
			0.0f))
	}
	else
	{
		m_CoatingRoughnessPath_ = texturePath;
		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(MaterialMutex);

				rpr_image texture_coating_roughness = LoadTexture(texturePath);

				rpr_material_node uberMat2_img_coating_roughness_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &
					uberMat2_img_coating_roughness_texture))
					gc.Add(uberMat2_img_coating_roughness_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_coating_roughness_texture, RPR_MATERIAL_INPUT_DATA,
					texture_coating_roughness))

					CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_COATING_ROUGHNESS,
						uberMat2_img_coating_roughness_texture))
			});

		loadTextureThread.join();
	}
}
void HorusMaterial::SetCoatingNormal(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& Gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		m_CoatingNormal_ = { 0.0f, 0.0f, 0.0f, 0.0f };
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_COATING_NORMAL, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		m_CoatingNormalPath_ = texturePath;
		std::thread LoadTextureThread([&]()
			{
				std::lock_guard Lock(MaterialMutex);

				rpr_image TextureNormal = LoadTexture(texturePath);

				rpr_material_node UberMat2ImgNormalTexture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &UberMat2ImgNormalTexture));
				Gc.Add(UberMat2ImgNormalTexture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(UberMat2ImgNormalTexture, RPR_MATERIAL_INPUT_DATA, TextureNormal));

				CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_COATING_NORMAL, UberMat2ImgNormalTexture));
			});

		LoadTextureThread.join();
	}
}
void HorusMaterial::SetCoatingNormal(glm::vec4 color)
{
	m_CoatingNormal_ = color;
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_COATING_NORMAL, color.x, color.y, color.z, color.w));
}
void HorusMaterial::SetCoatingThickness(glm::vec4 color)
{
	m_CoatingThickness_ = color;
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_COATING_THICKNESS, color.x, color.y, color.z, color.w));
}
void HorusMaterial::SetCoatingThickness(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		m_CoatingRoughness_ = { 0.0f, 0.0f, 0.0f, 0.0f };
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_COATING_THICKNESS, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		m_CoatingThicknessPath_ = texturePath;
		std::thread loadTextureThread([&]()
			{
				std::lock_guard lock(MaterialMutex);

				rpr_image TextureCoatingRoughness = LoadTexture(texturePath);

				rpr_material_node uberMat2_img_coating_roughness_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &uberMat2_img_coating_roughness_texture));
				gc.Add(uberMat2_img_coating_roughness_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_coating_roughness_texture, RPR_MATERIAL_INPUT_DATA, TextureCoatingRoughness));

				CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_COATING_THICKNESS, uberMat2_img_coating_roughness_texture));
			});

		loadTextureThread.join();
	}
}
void HorusMaterial::SetCoatingTransmissionColor(glm::vec4 color)
{
	m_CoatingTransmissionColor_ = color;
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_COATING_TRANSMISSION_COLOR, color.x, color.y, color.z, color.w));
}
void HorusMaterial::SetCoatingTransmissionColor(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& Gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		m_CoatingTransmissionColor_ = { 0.0f, 0.0f, 0.0f, 0.0f };
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_COATING_TRANSMISSION_COLOR, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		m_CoatingTransmissionColorPath_ = texturePath;
		std::thread LoadTextureThread([&]()
			{
				std::lock_guard Lock(MaterialMutex);

				rpr_image TextureTransmissionColor = LoadTexture(texturePath);

				rpr_material_node UberMat2ImgTransmissionColorTexture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &UberMat2ImgTransmissionColorTexture));
				Gc.Add(UberMat2ImgTransmissionColorTexture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(UberMat2ImgTransmissionColorTexture, RPR_MATERIAL_INPUT_DATA, TextureTransmissionColor));

				CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_COATING_TRANSMISSION_COLOR, UberMat2ImgTransmissionColorTexture));
			});

		LoadTextureThread.join();
	}
}
void HorusMaterial::SetCoatingMetalness(glm::vec4 color)
{
	m_CoatingMetalness_ = color;
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_COATING_METALNESS, color.x, color.y, color.z, color.w));

}
void HorusMaterial::SetCoatingMetalness(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& Gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		m_CoatingMetalness_ = { 0.0f, 0.0f, 0.0f, 0.0f };
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_COATING_METALNESS, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		m_CoatingMetalnessPath_ = texturePath;
		std::thread LoadTextureThread([&]()
			{
				std::lock_guard Lock(MaterialMutex);

				rpr_image TextureMetalness = LoadTexture(texturePath);

				rpr_material_node UberMat2ImgMetalnessTexture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &UberMat2ImgMetalnessTexture));
				Gc.Add(UberMat2ImgMetalnessTexture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(UberMat2ImgMetalnessTexture, RPR_MATERIAL_INPUT_DATA, TextureMetalness));

				CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_COATING_METALNESS, UberMat2ImgMetalnessTexture));
			});

		LoadTextureThread.join();
	}
}

// Other
// Normal map
void HorusMaterial::SetNormalMap(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		m_UseNormalMapWeightImage_ = false;
		m_NormalMapImageData_ = nullptr;
		m_NormalMap_ = { 0.0f, 0.0f, 0.0f, 0.0f };
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_SCALE, 0.0f, 0.0f, 0.0f, 0.0f))
	}
	else
	{
		m_UseNormalMapWeightImage_ = true;
		m_NormalMapPath_ = texturePath;
		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(MaterialMutex);

				rpr_image TextureNormal = LoadTexture(texturePath);


				rpr_material_node uberMat2_img_normal_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &uberMat2_img_normal_texture));
				gc.Add(uberMat2_img_normal_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_normal_texture, RPR_MATERIAL_INPUT_DATA, TextureNormal));

				m_NormalMapImageData_ = uberMat2_img_normal_texture;

				rpr_material_node uberMat2_bumpmap = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_NORMAL_MAP, &uberMat2_bumpmap));
				gc.Add(uberMat2_bumpmap);
				CHECK(rprMaterialNodeSetInputNByKey(uberMat2_bumpmap, RPR_MATERIAL_INPUT_COLOR, uberMat2_img_normal_texture));
				CHECK(rprMaterialNodeSetInputFByKey(uberMat2_bumpmap, RPR_MATERIAL_INPUT_SCALE, 1.0f, 1.0f, 1.0f, 1.0f));

				//CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_BASE_NORMAL, uberMat2_bumpmap));
				CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_DIFFUSE_NORMAL, uberMat2_bumpmap));
				CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_NORMAL, uberMat2_bumpmap));
				//CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_NORMAL, uberMat2_bumpmap));
				//CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_COATING_NORMAL, uberMat2_bumpmap));
			});

		loadTextureThread.join();
	}
}
void HorusMaterial::SetNormalMap(glm::vec4 color)
{
	m_UseNormalMapWeightImage_ = false;
	m_NormalMapImageData_ = nullptr;
	m_NormalMap_ = color;
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_SCALE, color.x, color.y, color.z, color.w));
}

void HorusMaterial::SetNormalMapWeight(glm::vec4 weight)
{
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	m_NormalMapWeight_ = weight;

	if (m_UseNormalMapWeightImage_)
	{
		rpr_material_node uberMat3_bumpmap = nullptr;
		CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_BUMP_MAP, &uberMat3_bumpmap));
		gc.Add(uberMat3_bumpmap);

		CHECK(rprMaterialNodeSetInputNByKey(uberMat3_bumpmap, RPR_MATERIAL_INPUT_COLOR, m_NormalMapImageData_));
		CHECK(rprMaterialNodeSetInputFByKey(uberMat3_bumpmap, RPR_MATERIAL_INPUT_SCALE, weight.x, weight.y, weight.z, weight.w));

		CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_DIFFUSE_NORMAL, uberMat3_bumpmap));
		CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_NORMAL, uberMat3_bumpmap));
	}
	else
	{
		//CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_SCALE, weight.x, weight.y, weight.z, weight.w));
	}
}

// Displacement map
void HorusMaterial::SetDisplacementMap(const std::string& texturePath)
{
	// TODO : Implement Displacement Map
}
void HorusMaterial::SetDisplacementMap(glm::vec4 color)
{
	// TODO : Implement Displacement Map
	//m_DisplacementMap_ = color;
//	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_DISPLACEMENT, color.x, color.y, color.z, color.w));
}

// Emissive
void HorusMaterial::SetEmissive(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		m_Emissive_ = { 0.0f, 0.0f, 0.0f, 0.0f };
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_EMISSION_COLOR, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		m_EmissivePath_ = texturePath;
		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(MaterialMutex);

				rpr_image texture_emissive = LoadTexture(texturePath);

				rpr_material_node uberMat2_img_emissive_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &
					uberMat2_img_emissive_texture))
					gc.Add(uberMat2_img_emissive_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_emissive_texture, RPR_MATERIAL_INPUT_DATA,
					texture_emissive))

					CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_EMISSION_COLOR,
						uberMat2_img_emissive_texture))
			});

		loadTextureThread.join();
	}
}
void HorusMaterial::SetEmissive(glm::vec4 color)
{
	m_Emissive_ = color;
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_EMISSION_COLOR, color.x, color.y, color.z, color.w));
}
void HorusMaterial::SetEmissionWeight(glm::vec4 color)
{
	m_EmissionWeight_ = color;
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_EMISSION_WEIGHT, color.x, color.y, color.z, color.w));
}
void HorusMaterial::SetEmissionWeight(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		m_EmissionWeight_ = { 0.0f, 0.0f, 0.0f, 0.0f };
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_EMISSION_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f))
	}
	else
	{
		m_EmissionWeightPath_ = texturePath;
		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(MaterialMutex);

				rpr_image texture_emission_weight = LoadTexture(texturePath);

				rpr_material_node uberMat2_img_emission_weight_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &
					uberMat2_img_emission_weight_texture))
					gc.Add(uberMat2_img_emission_weight_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_emission_weight_texture, RPR_MATERIAL_INPUT_DATA,
					texture_emission_weight))

					CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_EMISSION_WEIGHT,
						uberMat2_img_emission_weight_texture))
			});

		loadTextureThread.join();
	}
}

// Opacity
void HorusMaterial::SetOpacity(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		m_Opacity_ = { 0.0f, 0.0f, 0.0f, 0.0f };
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_TRANSPARENCY, 0.0f, 0.0f, 0.0f, 0.0f))
	}
	else
	{
		m_OpacityPath_ = texturePath;
		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(MaterialMutex);

				rpr_image texture_opacity = LoadTexture(texturePath);

				rpr_material_node uberMat2_img_opacity_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &
					uberMat2_img_opacity_texture))
					gc.Add(uberMat2_img_opacity_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_opacity_texture, RPR_MATERIAL_INPUT_DATA,
					texture_opacity))

					CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_TRANSPARENCY,
						uberMat2_img_opacity_texture))
			});

		loadTextureThread.join();
	}
}
void HorusMaterial::SetOpacity(glm::vec4 color)
{
	m_Opacity_ = color;
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_TRANSPARENCY, color.x, color.y, color.z, color.w));
}


// TODO : Verifying if this is working
// Transparency
void HorusMaterial::SetTransparency(glm::vec4 color)
{
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_TRANSPARENCY, color.x, color.y, color.z
		, color.w));
}
void HorusMaterial::SetTransparency(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_TRANSPARENCY, 0.0f, 0.0f, 0.0f, 0.0f))
	}
	else
	{
		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(MaterialMutex);

				rpr_image texture_opacity = LoadTexture(texturePath);

				rpr_material_node uberMat2_img_opacity_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &
					uberMat2_img_opacity_texture))
					gc.Add(uberMat2_img_opacity_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_opacity_texture, RPR_MATERIAL_INPUT_DATA,
					texture_opacity))

					CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_TRANSPARENCY,
						uberMat2_img_opacity_texture))
			});

		loadTextureThread.join();
	}
}

// -----------------------------------------------------------------------------------------------
// Other functions --------------------------------------------------------------------------------

void HorusMaterial::SetBackgroundImage(const std::string& texturePath)
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	if (texturePath.empty())
	{
		spdlog::error("Error: Texture plate not found.");
		Console.AddLog(" [error] Texture plate not found.");

		UnsetBackgroundImage();
	}
	else
	{
		std::thread LoadTextureThread([&]()
			{
				std::lock_guard<std::mutex> Lock(MaterialMutex);

				rpr_image TextureBackground = LoadTexture(texturePath);

				rprSceneSetBackgroundImage(ObjectManager.GetScene(), TextureBackground);

				gc.Add(TextureBackground);
			});

		LoadTextureThread.join();
	}
}
void HorusMaterial::UnsetBackgroundImage()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	rprSceneSetBackgroundImage(ObjectManager.GetScene(), nullptr);
}
