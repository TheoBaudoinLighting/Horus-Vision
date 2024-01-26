
#include "hrs_object_manager.h" // glad.h
#include "hrs_material.h"

#include <thread>
#include <mutex>

#include "common.h"
#include "hrs_radeon.h"
#include <hrs_console.h>

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
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR, 0.5f, 0.5f, 0.5f,
		1.0f))
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_DIFFUSE_WEIGHT, 1.0f, 1.0f, 1.0f,
		1.0f))
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_DIFFUSE_ROUGHNESS, 0.5f, 0.5f, 0.5f,
		1.0f))

	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_REFLECTION_COLOR, 1.f, 1.f, 1.f, 1.f
	))
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_REFLECTION_WEIGHT, 1.f, 1.f, 1.f,
		1.f))
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_REFLECTION_ROUGHNESS, 0.5f, 0.5f,
		0.5f, 1.0f))
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_REFLECTION_ANISOTROPY, 0.0f, 0.0f,
		0.0f, 0.0f))
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_REFLECTION_ANISOTROPY_ROTATION, 0.0f
		, 0.0f, 0.0f, 0.0f))
	CHECK(rprMaterialNodeSetInputUByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_REFLECTION_MODE,
		RPR_UBER_MATERIAL_IOR_MODE_METALNESS))
	// RPR_UBER_MATERIAL_IOR_MODE_METALNESS // RPR_UBER_MATERIAL_IOR_MODE_PBR
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_REFLECTION_IOR, 1.460000f, 1.460000f
		, 1.460000f, 1.460000f))

	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_REFRACTION_COLOR, 1.0f, 1.0f, 1.0f,
		1.0f))
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_REFRACTION_WEIGHT, 0.0f, 0.0f, 0.0f,
		0.0f))
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_REFRACTION_ROUGHNESS, 0.0f, 0.0f,
		0.0f, 0.0f))
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_REFRACTION_IOR, 1.500000f, 1.500000f
		, 1.500000f, 1.500000f))
	CHECK(rprMaterialNodeSetInputUByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_REFRACTION_THIN_SURFACE, 1))
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_REFRACTION_ABSORPTION_COLOR, 1.0f,
		1.0f, 1.0f, 0.0f))
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_REFRACTION_ABSORPTION_DISTANCE, 1.0f
		, 1.0f, 1.0f, 0.0f))
	CHECK(rprMaterialNodeSetInputUByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_REFRACTION_CAUSTICS, 1))

	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_COATING_COLOR, 0.5f, 0.5f, 0.5f,
		1.0f))
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_COATING_WEIGHT, 0.f, 0.f, 0.f, 0.f))
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_COATING_ROUGHNESS, 0.f, 0.f, 0.f,
		0.f))
	CHECK(rprMaterialNodeSetInputUByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_COATING_MODE, RPR_UBER_MATERIAL_IOR_MODE_METALNESS))
	// RPR_UBER_MATERIAL_IOR_MODE_METALNESS // RPR_UBER_MATERIAL_IOR_MODE_PBR
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_COATING_IOR, 1.460000f, 1.460000f,
		1.460000f, 1.460000f))
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_COATING_METALNESS, 0.0f, 0.0f, 0.0f,
		0.0f))
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_COATING_TRANSMISSION_COLOR, 0.0f,
		0.0f, 0.0f, 1.0f))
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_COATING_THICKNESS, 0.0f, 0.0f, 0.0f,
		0.0f))

	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_SHEEN, 1.0f, 1.0f, 1.0f, 1.0f))
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_SHEEN_TINT, 0.0f, 0.0f, 0.0f, 0.0f))
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_SHEEN_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f
	))

	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_EMISSION_COLOR, 1.0f, 1.0f, 1.0f,
		1.0f))
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_EMISSION_WEIGHT, 0.0f, 0.0f, 0.0f,
		0.0f))
	CHECK(rprMaterialNodeSetInputUByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_EMISSION_MODE, 1))

	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_TRANSPARENCY, 0.0f, 0.0f, 0.0f, 0.0f
	))

	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_COLOR, 0.0f, 0.0f, 0.0f,
		0.0f))
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_DISTANCE, 0.0f, 0.0f,
		0.0f, 0.0f))
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_DIRECTION, 0.0f, 0.0f,
		0.0f, 0.0f))
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_SSS_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f))
	CHECK(rprMaterialNodeSetInputUByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_SSS_MULTISCATTER, 0))

	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_BACKSCATTER_WEIGHT, 0.0f, 0.0f, 0.0f
		, 0.0f))
	CHECK(rprMaterialNodeSetInputFByKey(MaterialNodeUberv2, RPR_MATERIAL_INPUT_UBER_BACKSCATTER_COLOR, 0.0f, 0.0f, 0.0f,
		0.0f))

	return MaterialNodeUberv2;
}

void HorusMaterial::DestroyMaterial()
{
	CHECK(rprObjectDelete(m_Material_))

	if (m_Material_ != nullptr)
	{
		m_Material_ = nullptr;
	}
}

// Setters
void HorusMaterial::SetBaseColor(const std::string& TexturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& Gc = HorusGarbageCollector::GetInstance();

	if (TexturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR, 0.5f, 0.5f, 0.5f, 1.0f))
	}
	else
	{
		std::thread LoadTextureThread([&]()
		{
			std::lock_guard<std::mutex> Lock(MaterialMutex);

			rpr_image TextureBaseColor = LoadTexture(TexturePath);

			rpr_material_node UberMat2ImgBasecolorTexture = nullptr;
			CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &
				UberMat2ImgBasecolorTexture))
			Gc.Add(UberMat2ImgBasecolorTexture);
			CHECK(rprMaterialNodeSetInputImageDataByKey(UberMat2ImgBasecolorTexture, RPR_MATERIAL_INPUT_DATA,
				TextureBaseColor))

			CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR,
				UberMat2ImgBasecolorTexture))
			CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_COLOR,
				UberMat2ImgBasecolorTexture))
		});
		LoadTextureThread.join();
	}
}
void HorusMaterial::SetBaseColor(glm::vec4 Color)
{
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR, Color.x, Color.y, Color.z,
		Color.w));
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_COLOR, Color.x, Color.y, Color.z
		, Color.w));
}

void HorusMaterial::SetRoughness(const std::string& TexturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& Gc = HorusGarbageCollector::GetInstance();

	if (TexturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_ROUGHNESS, 0.5f, 0.5f, 0.5f,
			1.0f))
	}
	else
	{
		std::thread LoadTextureThread([&]()
		{
			std::lock_guard<std::mutex> Lock(MaterialMutex);

			rpr_image TextureRoughness = LoadTexture(TexturePath);

			rpr_material_node UberMat2ImgRoughnessTexture = nullptr;
			CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &
				UberMat2ImgRoughnessTexture))
			Gc.Add(UberMat2ImgRoughnessTexture);
			CHECK(rprMaterialNodeSetInputImageDataByKey(UberMat2ImgRoughnessTexture, RPR_MATERIAL_INPUT_DATA,
				TextureRoughness))

			CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_ROUGHNESS,
				UberMat2ImgRoughnessTexture))
		});

		LoadTextureThread.join();
	}
}
void HorusMaterial::SetRoughness(glm::vec4 color)
{
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_ROUGHNESS, color.x, color.y,
		color.z, color.w));
}

void HorusMaterial::SetNormal(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_SCALE, 0.0f, 0.0f, 0.0f, 0.0f))
	}
	else
	{
		std::thread loadTextureThread([&]()
		{
			std::lock_guard<std::mutex> lock(MaterialMutex);

			rpr_image texture_normal = LoadTexture(texturePath);

			rpr_material_node uberMat2_img_normal_texture = nullptr;
			CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &
				uberMat2_img_normal_texture))
			gc.Add(uberMat2_img_normal_texture);
			CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_normal_texture, RPR_MATERIAL_INPUT_DATA,
				texture_normal))

			rpr_material_node uberMat2_bumpmap = nullptr;
			CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_NORMAL_MAP, &uberMat2_bumpmap))
			gc.Add(uberMat2_bumpmap);
			CHECK(rprMaterialNodeSetInputNByKey(uberMat2_bumpmap, RPR_MATERIAL_INPUT_COLOR, uberMat2_img_normal_texture
			))
			CHECK(rprMaterialNodeSetInputFByKey(uberMat2_bumpmap, RPR_MATERIAL_INPUT_SCALE, 1.0f, 1.0f, 1.0f, 1.0f))

			CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_DIFFUSE_NORMAL, uberMat2_bumpmap))
			CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_NORMAL, uberMat2_bumpmap
			))
		});

		loadTextureThread.join();
	}
}
void HorusMaterial::SetNormal(glm::vec4 color)
{
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_SCALE, color.x, color.y, color.z, color.w));
}

void HorusMaterial::SetMetallic(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_METALNESS, 0.0f, 0.0f, 0.0f,
			0.0f))
	}
	else
	{
		std::thread loadTextureThread([&]()
		{
			std::lock_guard<std::mutex> lock(MaterialMutex);

			rpr_image texture_metallic = LoadTexture(texturePath);

			rpr_material_node uberMat2_img_metallic_texture = nullptr;
			CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &
				uberMat2_img_metallic_texture))
			gc.Add(uberMat2_img_metallic_texture);
			CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_metallic_texture, RPR_MATERIAL_INPUT_DATA,
				texture_metallic))

			CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_METALNESS,
				uberMat2_img_metallic_texture))
		});

		loadTextureThread.join();
	}
}
void HorusMaterial::SetMetallic(glm::vec4 color)
{
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_METALNESS, color.x, color.y,
		color.z, color.w));
}

void HorusMaterial::SetEmissive(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_EMISSION_COLOR, 0.0f, 0.0f, 0.0f, 0.0f
		))
	}
	else
	{
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
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_EMISSION_COLOR, color.x, color.y, color.z,
		color.w));
}

void HorusMaterial::SetOpacity(const std::string& texturePath)
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
void HorusMaterial::SetOpacity(glm::vec4 color)
{
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_TRANSPARENCY, color.x, color.y, color.z,
		color.w));
}

void HorusMaterial::SetDiffuseWeight(glm::vec4 color)
{
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_DIFFUSE_WEIGHT, color.x, color.y, color.z,
		color.w));
}
void HorusMaterial::SetDiffuseWeight(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_DIFFUSE_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f
		))
	}
	else
	{
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

void HorusMaterial::SetReflectionColor(glm::vec4 color)
{
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_COLOR, color.x, color.y,
		color.z, color.w));
}
void HorusMaterial::SetReflectionColor(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_COLOR, 0.0f, 0.0f, 0.0f,
			0.0f))
	}
	else
	{
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

void HorusMaterial::SetReflectionWeight(glm::vec4 color)
{
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_WEIGHT, color.x, color.y,
		color.z, color.w));
}
void HorusMaterial::SetReflectionWeight(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_WEIGHT, 0.0f, 0.0f, 0.0f,
			0.0f))
	}
	else
	{
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

void HorusMaterial::SetReflectionRoughness(glm::vec4 color)
{
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_ROUGHNESS, color.x, color.y,
		color.z, color.w));
}
void HorusMaterial::SetReflectionRoughness(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_ROUGHNESS, 0.0f, 0.0f, 0.0f,
			0.0f))
	}
	else
	{
		std::thread loadTextureThread([&]()
		{
			std::lock_guard<std::mutex> lock(MaterialMutex);

			rpr_image texture_reflection_roughness = LoadTexture(texturePath);

			rpr_material_node uberMat2_img_reflection_roughness_texture = nullptr;
			CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &
				uberMat2_img_reflection_roughness_texture))
			gc.Add(uberMat2_img_reflection_roughness_texture);
			CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_reflection_roughness_texture,
				RPR_MATERIAL_INPUT_DATA, texture_reflection_roughness))

			CHECK(rprMaterialNodeSetInputNByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_ROUGHNESS,
				uberMat2_img_reflection_roughness_texture))
		});

		loadTextureThread.join();
	}
}

void HorusMaterial::SetRefractionColor(glm::vec4 color)
{
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_COLOR, color.x, color.y,
		color.z, color.w));
}
void HorusMaterial::SetRefractionColor(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_COLOR, 0.0f, 0.0f, 0.0f,
			0.0f))
	}
	else
	{
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
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_WEIGHT, color.x, color.y,
		color.z, color.w));
}
void HorusMaterial::SetRefractionWeight(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_WEIGHT, 0.0f, 0.0f, 0.0f,
			0.0f))
	}
	else
	{
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
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_ROUGHNESS, color.x, color.y,
		color.z, color.w));
}
void HorusMaterial::SetRefractionRoughness(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_ROUGHNESS, 0.0f, 0.0f, 0.0f,
			0.0f))
	}
	else
	{
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

void HorusMaterial::SetCoatingColor(glm::vec4 color)
{
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_COATING_COLOR, color.x, color.y, color.
		z, color.w));
}
void HorusMaterial::SetCoatingColor(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_COATING_COLOR, 0.0f, 0.0f, 0.0f, 0.0f))
	}
	else
	{
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
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_COATING_WEIGHT, color.x, color.y, color
		.z, color.w));
}
void HorusMaterial::SetCoatingWeight(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_COATING_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f
		))
	}
	else
	{
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
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_COATING_ROUGHNESS, color.x, color.y,
		color.z, color.w));
}
void HorusMaterial::SetCoatingRoughness(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_COATING_ROUGHNESS, 0.0f, 0.0f, 0.0f,
			0.0f))
	}
	else
	{
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

void HorusMaterial::SetSheen(glm::vec4 color)
{
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_SHEEN, color.x, color.y, color.z, color
		.w));
}
void HorusMaterial::SetSheen(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_SHEEN, 0.0f, 0.0f, 0.0f, 0.0f))
	}
	else
	{
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
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_SHEEN_TINT, color.x, color.y, color.z,
		color.w));
}
void HorusMaterial::SetSheenTint(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_SHEEN_TINT, 0.0f, 0.0f, 0.0f, 0.0f))
	}
	else
	{
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

void HorusMaterial::SetEmissionWeight(glm::vec4 color)
{
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_EMISSION_WEIGHT, color.x, color.y,
		color.z, color.w));
}
void HorusMaterial::SetEmissionWeight(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_EMISSION_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f
		))
	}
	else
	{
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

void HorusMaterial::SetSssScatterColor(glm::vec4 color)
{
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_COLOR, color.x, color.y,
		color.z, color.w));
}
void HorusMaterial::SetSssScatterColor(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_COLOR, 0.0f, 0.0f, 0.0f,
			0.0f))
	}
	else
	{
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

void HorusMaterial::SetSssScatterDistance(glm::vec4 color)
{
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_DISTANCE, color.x, color.y,
		color.z, color.w));
}
void HorusMaterial::SetSssScatterDistance(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_DISTANCE, 0.0f, 0.0f, 0.0f,
			0.0f))
	}
	else
	{
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

void HorusMaterial::SetBackscatterWeight(glm::vec4 color)
{
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_BACKSCATTER_WEIGHT, color.x, color.y, color
		.z, color.w));
}
void HorusMaterial::SetBackscatterWeight(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_BACKSCATTER_WEIGHT, 0.0f, 0.0f, 0.0f,
			0.0f))
	}
	else
	{
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
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_BACKSCATTER_COLOR, color.x, color.y, color.
		z, color.w));
}
void HorusMaterial::SetBackscatterColor(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_BACKSCATTER_COLOR, 0.0f, 0.0f, 0.0f,
			0.0f))
	}
	else
	{
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

void HorusMaterial::SetSheenWeight(glm::vec4 weight)
{
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_SHEEN_WEIGHT, weight.x, weight.y, weight.z,
		weight.w));
}
void HorusMaterial::SetSheenWeight(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_SHEEN_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f))
	}
	else
	{
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

void HorusMaterial::SetIor(float ior)
{
	CHECK(rprMaterialNodeSetInputFByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_IOR, ior, ior, ior, 1))
}
void HorusMaterial::SetReflectionMode(int mode)
{
	if (mode == 0)
	{
		CHECK(rprMaterialNodeSetInputUByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_MODE,
			RPR_UBER_MATERIAL_IOR_MODE_PBR))
	}
	else if (mode == 1)
	{
		CHECK(rprMaterialNodeSetInputUByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_MODE,
			RPR_UBER_MATERIAL_IOR_MODE_METALNESS))
	}
}
void HorusMaterial::SetCoatingMode(int mode)
{
	if (mode == 0)
	{
		CHECK(rprMaterialNodeSetInputUByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_COATING_MODE,
			RPR_UBER_MATERIAL_IOR_MODE_PBR))
	}
	else if (mode == 1)
	{
		CHECK(rprMaterialNodeSetInputUByKey(m_Material_, RPR_MATERIAL_INPUT_UBER_COATING_MODE,
			RPR_UBER_MATERIAL_IOR_MODE_METALNESS))
	}
}

// -----------------------------------------------------------------------------------------------
// Texture management ----------------------------------------------------------------------------

rpr_image HorusMaterial::LoadTexture(std::string path)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	rpr_image Image = nullptr;

	if (rprContextCreateImageFromFile(Radeon.GetContext(), path.c_str(), &Image) != RPR_SUCCESS)
	{
		spdlog::error("Error: Texture -> {} not found.", path);
		Console.AddLog(" [error] Texture -> %s not found.", path.c_str());
		return nullptr;
	}

	gc.Add(Image);

	spdlog::info("Texture -> {} loaded.", path);
	Console.AddLog(" [info] Texture -> %s loaded.", path.c_str());

	return Image;
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
