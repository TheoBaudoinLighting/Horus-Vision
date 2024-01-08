#include "hrs_material.h"

#include <thread>
#include <mutex>

#include "common.h"
#include "hrs_radeon.h"
#include "objects/hrs_object_manager.h"

//HorusRadeon& Radeon = HorusRadeon::get_instance();
//HorusObjectManager& g_object_impm = HorusObjectManager::get_instance();

std::mutex material_mutex;

// Material management ---------------------------------------------------------------------------

void HorusMaterial::init()
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();
	RPRGarbageCollector m_gc = Radeon.get_gc();

	m_material_ = create_material();

	m_gc.GCAdd(m_material_);
}

rpr_material_node HorusMaterial::create_material()
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();
	RPRGarbageCollector m_gc = Radeon.get_gc();

	rpr_material_node material_node_uberv2 = nullptr;

	CHECK(rprMaterialSystemCreateNode(Radeon.get_matsys(), RPR_MATERIAL_NODE_UBERV2, &material_node_uberv2));
	m_gc.GCAdd(material_node_uberv2);

	//CHECK(rprObjectSetName(material_node_uberv2, "Uber_0"));
	CHECK(rprMaterialNodeSetInputFByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR, 0.5f, 0.5f, 0.5f, 1.0f));
	CHECK(rprMaterialNodeSetInputFByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_DIFFUSE_WEIGHT, 1.0f, 1.0f, 1.0f, 1.0f));
	CHECK(rprMaterialNodeSetInputFByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_DIFFUSE_ROUGHNESS, 0.5f, 0.5f, 0.5f, 1.0f));

	CHECK(rprMaterialNodeSetInputFByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_REFLECTION_COLOR, 1.f, 1.f, 1.f, 1.f));
	CHECK(rprMaterialNodeSetInputFByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_REFLECTION_WEIGHT, 1.f, 1.f, 1.f, 1.f));
	CHECK(rprMaterialNodeSetInputFByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_REFLECTION_ROUGHNESS, 0.5f, 0.5f, 0.5f, 1.0f));
	CHECK(rprMaterialNodeSetInputFByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_REFLECTION_ANISOTROPY, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_REFLECTION_ANISOTROPY_ROTATION, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputUByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_REFLECTION_MODE, RPR_UBER_MATERIAL_IOR_MODE_PBR)); // RPR_UBER_MATERIAL_IOR_MODE_METALNESS
	CHECK(rprMaterialNodeSetInputFByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_REFLECTION_IOR, 1.460000f, 1.460000f, 1.460000f, 1.460000f));

	CHECK(rprMaterialNodeSetInputFByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_REFRACTION_COLOR, 1.0f, 1.0f, 1.0f, 1.0f));
	CHECK(rprMaterialNodeSetInputFByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_REFRACTION_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_REFRACTION_ROUGHNESS, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_REFRACTION_IOR, 1.500000f, 1.500000f, 1.500000f, 1.500000f));
	CHECK(rprMaterialNodeSetInputUByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_REFRACTION_THIN_SURFACE, 0));
	CHECK(rprMaterialNodeSetInputFByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_REFRACTION_ABSORPTION_COLOR, 1.0f, 1.0f, 1.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_REFRACTION_ABSORPTION_DISTANCE, 1.0f, 1.0f, 1.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputUByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_REFRACTION_CAUSTICS, 1));

	CHECK(rprMaterialNodeSetInputFByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_COATING_COLOR, 0.5f, 0.5f, 0.5f, 1.0f));
	CHECK(rprMaterialNodeSetInputFByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_COATING_WEIGHT, 0.f, 0.f, 0.f, 0.f));
	CHECK(rprMaterialNodeSetInputFByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_COATING_ROUGHNESS, 0.f, 0.f, 0.f, 0.f));
	CHECK(rprMaterialNodeSetInputUByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_COATING_MODE, RPR_UBER_MATERIAL_IOR_MODE_PBR)); // RPR_UBER_MATERIAL_IOR_MODE_METALNESS
	CHECK(rprMaterialNodeSetInputFByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_COATING_IOR, 1.460000f, 1.460000f, 1.460000f, 1.460000f));
	CHECK(rprMaterialNodeSetInputFByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_COATING_METALNESS, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_COATING_TRANSMISSION_COLOR, 0.0f, 0.0f, 0.0f, 1.0f));
	CHECK(rprMaterialNodeSetInputFByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_COATING_THICKNESS, 0.0f, 0.0f, 0.0f, 0.0f));

	CHECK(rprMaterialNodeSetInputFByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_SHEEN, 1.0f, 1.0f, 1.0f, 1.0f));
	CHECK(rprMaterialNodeSetInputFByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_SHEEN_TINT, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_SHEEN_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f));

	CHECK(rprMaterialNodeSetInputFByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_EMISSION_COLOR, 1.0f, 1.0f, 1.0f, 1.0f));
	CHECK(rprMaterialNodeSetInputFByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_EMISSION_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputUByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_EMISSION_MODE, 1));

	CHECK(rprMaterialNodeSetInputFByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_TRANSPARENCY, 0.0f, 0.0f, 0.0f, 0.0f));

	CHECK(rprMaterialNodeSetInputFByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_COLOR, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_DISTANCE, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_DIRECTION, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_SSS_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputUByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_SSS_MULTISCATTER, 0));

	CHECK(rprMaterialNodeSetInputFByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_BACKSCATTER_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(material_node_uberv2, RPR_MATERIAL_INPUT_UBER_BACKSCATTER_COLOR, 0.0f, 0.0f, 0.0f, 0.0f));

	return material_node_uberv2;
}

void HorusMaterial::destroy_material()
{
	CHECK(rprObjectDelete(m_material_));

	if (m_material_ != nullptr)
	{
		m_material_ = nullptr;
	}
}

void HorusMaterial::set_base_color(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR, 0.5f, 0.5f, 0.5f, 1.0f));
	}
	else
	{
		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(material_mutex);

				rpr_image texture_base_color = load_texture(texturePath.c_str());

				rpr_material_node uberMat2_img_basecolor_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.get_matsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &uberMat2_img_basecolor_texture));
				Radeon.get_gc().GCAdd(uberMat2_img_basecolor_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_basecolor_texture, RPR_MATERIAL_INPUT_DATA, texture_base_color));

				CHECK(rprMaterialNodeSetInputNByKey(m_material_, RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR, uberMat2_img_basecolor_texture));
				CHECK(rprMaterialNodeSetInputNByKey(m_material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_COLOR, uberMat2_img_basecolor_texture));


			});
		loadTextureThread.join();
	}
}

void HorusMaterial::set_base_color(const std::array<float, 3>& color)
{
	if (color.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR, 0.5f, 0.5f, 0.5f, 1.0f));
	}
	else
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR, color[0], color[1], color[2], 1));
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_COLOR, color[0], color[1], color[2], 1));
	}
}

void HorusMaterial::set_roughness(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_ROUGHNESS, 0.5f, 0.5f, 0.5f, 1.0f));
	}
	else
	{
		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(material_mutex);

				rpr_image texture_roughness = load_texture(texturePath.c_str());

				rpr_material_node uberMat2_img_roughness_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.get_matsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &uberMat2_img_roughness_texture));
				Radeon.get_gc().GCAdd(uberMat2_img_roughness_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_roughness_texture, RPR_MATERIAL_INPUT_DATA, texture_roughness));

				CHECK(rprMaterialNodeSetInputNByKey(m_material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_ROUGHNESS, uberMat2_img_roughness_texture));
			});

		loadTextureThread.join();
	}
}

void HorusMaterial::set_roughness(const std::array<float, 3>& color)
{
	if (color.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_ROUGHNESS, 0.5f, 0.5f, 0.5f, 1.0f));
	}
	else
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_ROUGHNESS, color[0], color[1], color[2], 1));
	}

}

void HorusMaterial::set_normal(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_SCALE, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(material_mutex);

				rpr_image texture_normal = load_texture(texturePath.c_str());

				rpr_material_node uberMat2_img_normal_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.get_matsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &uberMat2_img_normal_texture));
				Radeon.get_gc().GCAdd(uberMat2_img_normal_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_normal_texture, RPR_MATERIAL_INPUT_DATA, texture_normal));

				rpr_material_node uberMat2_bumpmap = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.get_matsys(), RPR_MATERIAL_NODE_NORMAL_MAP, &uberMat2_bumpmap));
				Radeon.get_gc().GCAdd(uberMat2_bumpmap);
				CHECK(rprMaterialNodeSetInputNByKey(uberMat2_bumpmap, RPR_MATERIAL_INPUT_COLOR, uberMat2_img_normal_texture));
				CHECK(rprMaterialNodeSetInputFByKey(uberMat2_bumpmap, RPR_MATERIAL_INPUT_SCALE, 1.0f, 1.0f, 1.0f, 1.0f));

				CHECK(rprMaterialNodeSetInputNByKey(m_material_, RPR_MATERIAL_INPUT_UBER_DIFFUSE_NORMAL, uberMat2_bumpmap));
				CHECK(rprMaterialNodeSetInputNByKey(m_material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_NORMAL, uberMat2_bumpmap));

			});

		loadTextureThread.join();
	}

}

void HorusMaterial::set_normal(const std::array<float, 3>& color)
{
	if (color.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_SCALE, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_SCALE, color[0], color[1], color[2], 1));
	}

}

void HorusMaterial::set_metallic(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_METALNESS, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(material_mutex);

				rpr_image texture_metallic = load_texture(texturePath.c_str());

				rpr_material_node uberMat2_img_metallic_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.get_matsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &uberMat2_img_metallic_texture));
				Radeon.get_gc().GCAdd(uberMat2_img_metallic_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_metallic_texture, RPR_MATERIAL_INPUT_DATA, texture_metallic));

				CHECK(rprMaterialNodeSetInputNByKey(m_material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_METALNESS, uberMat2_img_metallic_texture));

			});

		loadTextureThread.join();
	}

}

void HorusMaterial::set_metallic(const std::array<float, 3>& color)
{
	if (color.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_METALNESS, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_METALNESS, color[0], color[1], color[2], 1));
	}


}

void HorusMaterial::set_emissive(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_EMISSION_COLOR, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(material_mutex);

				rpr_image texture_emissive = load_texture(texturePath.c_str());

				rpr_material_node uberMat2_img_emissive_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.get_matsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &uberMat2_img_emissive_texture));
				Radeon.get_gc().GCAdd(uberMat2_img_emissive_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_emissive_texture, RPR_MATERIAL_INPUT_DATA, texture_emissive));

				CHECK(rprMaterialNodeSetInputNByKey(m_material_, RPR_MATERIAL_INPUT_UBER_EMISSION_COLOR, uberMat2_img_emissive_texture));

			});

		loadTextureThread.join();
	}


}

void HorusMaterial::set_emissive(const std::array<float, 3>& color)
{
	if (color.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_EMISSION_COLOR, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_EMISSION_COLOR, color[0], color[1], color[2], 1));
	}


}

void HorusMaterial::set_opacity(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_TRANSPARENCY, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(material_mutex);

				rpr_image texture_opacity = load_texture(texturePath.c_str());

				rpr_material_node uberMat2_img_opacity_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.get_matsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &uberMat2_img_opacity_texture));
				Radeon.get_gc().GCAdd(uberMat2_img_opacity_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_opacity_texture, RPR_MATERIAL_INPUT_DATA, texture_opacity));

				CHECK(rprMaterialNodeSetInputNByKey(m_material_, RPR_MATERIAL_INPUT_UBER_TRANSPARENCY, uberMat2_img_opacity_texture));

			});

		loadTextureThread.join();
	}

}

void HorusMaterial::set_opacity(const std::array<float, 3>& color)
{
	if (color.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_TRANSPARENCY, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_TRANSPARENCY, color[0], color[1], color[2], 1));
	}

}

void HorusMaterial::set_diffuse_weight(const std::array<float, 3>& color)
{
	if (color.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_DIFFUSE_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_DIFFUSE_WEIGHT, color[0], color[1], color[2], 1));
	}
}

void HorusMaterial::set_diffuse_weight(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_DIFFUSE_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(material_mutex);

				rpr_image texture_diffuse_weight = load_texture(texturePath.c_str());

				rpr_material_node uberMat2_img_diffuse_weight_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.get_matsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &uberMat2_img_diffuse_weight_texture));
				Radeon.get_gc().GCAdd(uberMat2_img_diffuse_weight_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_diffuse_weight_texture, RPR_MATERIAL_INPUT_DATA, texture_diffuse_weight));

				CHECK(rprMaterialNodeSetInputNByKey(m_material_, RPR_MATERIAL_INPUT_UBER_DIFFUSE_WEIGHT, uberMat2_img_diffuse_weight_texture));

			});

		loadTextureThread.join();
	}

}

void HorusMaterial::set_reflection_color(const std::array<float, 3>& color)
{
	if (color.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_COLOR, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_COLOR, color[0], color[1], color[2], 1));
	}
}

void HorusMaterial::set_reflection_color(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_COLOR, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(material_mutex);

				rpr_image texture_reflection_color = load_texture(texturePath.c_str());

				rpr_material_node uberMat2_img_reflection_color_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.get_matsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &uberMat2_img_reflection_color_texture));
				Radeon.get_gc().GCAdd(uberMat2_img_reflection_color_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_reflection_color_texture, RPR_MATERIAL_INPUT_DATA, texture_reflection_color));

				CHECK(rprMaterialNodeSetInputNByKey(m_material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_COLOR, uberMat2_img_reflection_color_texture));

			});

		loadTextureThread.join();
	}

}

void HorusMaterial::set_reflection_weight(const std::array<float, 3>& color)
{
	if (color.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_WEIGHT, color[0], color[1], color[2], 1));
	}
}

void HorusMaterial::set_reflection_weight(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(material_mutex);

				rpr_image texture_reflection_weight = load_texture(texturePath.c_str());

				rpr_material_node uberMat2_img_reflection_weight_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.get_matsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &uberMat2_img_reflection_weight_texture));
				Radeon.get_gc().GCAdd(uberMat2_img_reflection_weight_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_reflection_weight_texture, RPR_MATERIAL_INPUT_DATA, texture_reflection_weight));

				CHECK(rprMaterialNodeSetInputNByKey(m_material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_WEIGHT, uberMat2_img_reflection_weight_texture));

			});

		loadTextureThread.join();
	}

}

void HorusMaterial::set_reflection_roughness(const std::array<float, 3>& color)
{
	if (color.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_ROUGHNESS, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_ROUGHNESS, color[0], color[1], color[2], 1));
	}
}

void HorusMaterial::set_reflection_roughness(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_ROUGHNESS, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{

		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(material_mutex);

				rpr_image texture_reflection_roughness = load_texture(texturePath.c_str());

				rpr_material_node uberMat2_img_reflection_roughness_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.get_matsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &uberMat2_img_reflection_roughness_texture));
				Radeon.get_gc().GCAdd(uberMat2_img_reflection_roughness_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_reflection_roughness_texture, RPR_MATERIAL_INPUT_DATA, texture_reflection_roughness));

				CHECK(rprMaterialNodeSetInputNByKey(m_material_, RPR_MATERIAL_INPUT_UBER_REFLECTION_ROUGHNESS, uberMat2_img_reflection_roughness_texture));

			});

		loadTextureThread.join();
	}

}

void HorusMaterial::set_refraction_color(const std::array<float, 3>& color)
{
	if (color.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_COLOR, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_COLOR, color[0], color[1], color[2], 1));
	}
}

void HorusMaterial::set_refraction_color(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_COLOR, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{

		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(material_mutex);

				rpr_image texture_refraction_color = load_texture(texturePath.c_str());

				rpr_material_node uberMat2_img_refraction_color_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.get_matsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &uberMat2_img_refraction_color_texture));
				Radeon.get_gc().GCAdd(uberMat2_img_refraction_color_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_refraction_color_texture, RPR_MATERIAL_INPUT_DATA, texture_refraction_color));

				CHECK(rprMaterialNodeSetInputNByKey(m_material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_COLOR, uberMat2_img_refraction_color_texture));

			});

		loadTextureThread.join();
	}
}

void HorusMaterial::set_refraction_weight(const std::array<float, 3>& color)
{
	if (color.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{

		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_WEIGHT, color[0], color[1], color[2], 1));
	}
}

void HorusMaterial::set_refraction_weight(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{

		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(material_mutex);

				rpr_image texture_refraction_weight = load_texture(texturePath.c_str());

				rpr_material_node uberMat2_img_refraction_weight_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.get_matsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &uberMat2_img_refraction_weight_texture));
				Radeon.get_gc().GCAdd(uberMat2_img_refraction_weight_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_refraction_weight_texture, RPR_MATERIAL_INPUT_DATA, texture_refraction_weight));

				CHECK(rprMaterialNodeSetInputNByKey(m_material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_WEIGHT, uberMat2_img_refraction_weight_texture));

			});

		loadTextureThread.join();
	}
}

void HorusMaterial::set_refraction_roughness(const std::array<float, 3>& color)
{
	if (color.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_ROUGHNESS, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{

		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_ROUGHNESS, color[0], color[1], color[2], 1));
	}
}

void HorusMaterial::set_refraction_roughness(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_ROUGHNESS, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{

		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(material_mutex);

				rpr_image texture_refraction_roughness = load_texture(texturePath.c_str());

				rpr_material_node uberMat2_img_refraction_roughness_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.get_matsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &uberMat2_img_refraction_roughness_texture));
				Radeon.get_gc().GCAdd(uberMat2_img_refraction_roughness_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_refraction_roughness_texture, RPR_MATERIAL_INPUT_DATA, texture_refraction_roughness));

				CHECK(rprMaterialNodeSetInputNByKey(m_material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_ROUGHNESS, uberMat2_img_refraction_roughness_texture));

			});

		loadTextureThread.join();
	}
}

void HorusMaterial::set_coating_color(const std::array<float, 3>& color)
{
	if (color.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_COATING_COLOR, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{

		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_COATING_COLOR, color[0], color[1], color[2], 1));
	}
}

void HorusMaterial::set_coating_color(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_COATING_COLOR, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{

		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(material_mutex);

				rpr_image texture_coating_color = load_texture(texturePath.c_str());

				rpr_material_node uberMat2_img_coating_color_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.get_matsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &uberMat2_img_coating_color_texture));
				Radeon.get_gc().GCAdd(uberMat2_img_coating_color_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_coating_color_texture, RPR_MATERIAL_INPUT_DATA, texture_coating_color));

				CHECK(rprMaterialNodeSetInputNByKey(m_material_, RPR_MATERIAL_INPUT_UBER_COATING_COLOR, uberMat2_img_coating_color_texture));

			});

		loadTextureThread.join();
	}
}

void HorusMaterial::set_coating_weight(const std::array<float, 3>& color)
{
	if (color.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_COATING_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{

		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_COATING_WEIGHT, color[0], color[1], color[2], 1));
	}
}

void HorusMaterial::set_coating_weight(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_COATING_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{

		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(material_mutex);

				rpr_image texture_coating_weight = load_texture(texturePath.c_str());

				rpr_material_node uberMat2_img_coating_weight_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.get_matsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &uberMat2_img_coating_weight_texture));
				Radeon.get_gc().GCAdd(uberMat2_img_coating_weight_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_coating_weight_texture, RPR_MATERIAL_INPUT_DATA, texture_coating_weight));

				CHECK(rprMaterialNodeSetInputNByKey(m_material_, RPR_MATERIAL_INPUT_UBER_COATING_WEIGHT, uberMat2_img_coating_weight_texture));

			});

		loadTextureThread.join();
	}
}

void HorusMaterial::set_coating_roughness(const std::array<float, 3>& color)
{
	if (color.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_COATING_ROUGHNESS, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{

		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_COATING_ROUGHNESS, color[0], color[1], color[2], 1));
	}
}

void HorusMaterial::set_coating_roughness(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_COATING_ROUGHNESS, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{

		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(material_mutex);

				rpr_image texture_coating_roughness = load_texture(texturePath.c_str());

				rpr_material_node uberMat2_img_coating_roughness_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.get_matsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &uberMat2_img_coating_roughness_texture));
				Radeon.get_gc().GCAdd(uberMat2_img_coating_roughness_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_coating_roughness_texture, RPR_MATERIAL_INPUT_DATA, texture_coating_roughness));

				CHECK(rprMaterialNodeSetInputNByKey(m_material_, RPR_MATERIAL_INPUT_UBER_COATING_ROUGHNESS, uberMat2_img_coating_roughness_texture));

			});

		loadTextureThread.join();
	}
}

void HorusMaterial::set_sheen(const std::array<float, 3>& color)
{
	if (color.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_SHEEN, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{

		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_SHEEN, color[0], color[1], color[2], 1));
	}
}

void HorusMaterial::set_sheen(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_SHEEN, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{

		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(material_mutex);

				rpr_image texture_sheen = load_texture(texturePath.c_str());

				rpr_material_node uberMat2_img_sheen_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.get_matsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &uberMat2_img_sheen_texture));
				Radeon.get_gc().GCAdd(uberMat2_img_sheen_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_sheen_texture, RPR_MATERIAL_INPUT_DATA, texture_sheen));

				CHECK(rprMaterialNodeSetInputNByKey(m_material_, RPR_MATERIAL_INPUT_UBER_SHEEN, uberMat2_img_sheen_texture));

			});

		loadTextureThread.join();
	}
}

void HorusMaterial::set_sheen_tint(const std::array<float, 3>& color)
{
	if (color.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_SHEEN_TINT, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{

		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_SHEEN_TINT, color[0], color[1], color[2], 1));
	}
}

void HorusMaterial::set_sheen_tint(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_SHEEN_TINT, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{

		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(material_mutex);

				rpr_image texture_sheen_tint = load_texture(texturePath.c_str());

				rpr_material_node uberMat2_img_sheen_tint_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.get_matsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &uberMat2_img_sheen_tint_texture));
				Radeon.get_gc().GCAdd(uberMat2_img_sheen_tint_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_sheen_tint_texture, RPR_MATERIAL_INPUT_DATA, texture_sheen_tint));

				CHECK(rprMaterialNodeSetInputNByKey(m_material_, RPR_MATERIAL_INPUT_UBER_SHEEN_TINT, uberMat2_img_sheen_tint_texture));

			});

		loadTextureThread.join();
	}
}

void HorusMaterial::set_emission_weight(const std::array<float, 3>& color)
{
	if (color.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_EMISSION_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{

		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_EMISSION_WEIGHT, color[0], color[1], color[2], 1));
	}
}

void HorusMaterial::set_emission_weight(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_EMISSION_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{

		std::thread loadTextureThread([&]()
			{
				std::lock_guard<std::mutex> lock(material_mutex);

				rpr_image texture_emission_weight = load_texture(texturePath.c_str());

				rpr_material_node uberMat2_img_emission_weight_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.get_matsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &uberMat2_img_emission_weight_texture));
				Radeon.get_gc().GCAdd(uberMat2_img_emission_weight_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_emission_weight_texture, RPR_MATERIAL_INPUT_DATA, texture_emission_weight));

				CHECK(rprMaterialNodeSetInputNByKey(m_material_, RPR_MATERIAL_INPUT_UBER_EMISSION_WEIGHT, uberMat2_img_emission_weight_texture));

			});

		loadTextureThread.join();
	}
}

void HorusMaterial::set_transparency(const std::array<float, 3>& color)
{
	if (color.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_TRANSPARENCY, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{

		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_TRANSPARENCY, color[0], color[1], color[2], 1));
	}
}

void HorusMaterial::set_transparency(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_TRANSPARENCY, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{

		std::thread loadTextureThread([&]()
			{

				std::lock_guard<std::mutex> lock(material_mutex);

				rpr_image texture_opacity = load_texture(texturePath.c_str());

				rpr_material_node uberMat2_img_opacity_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.get_matsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &uberMat2_img_opacity_texture));
				Radeon.get_gc().GCAdd(uberMat2_img_opacity_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_opacity_texture, RPR_MATERIAL_INPUT_DATA, texture_opacity));

				CHECK(rprMaterialNodeSetInputNByKey(m_material_, RPR_MATERIAL_INPUT_UBER_TRANSPARENCY, uberMat2_img_opacity_texture));

			});

		loadTextureThread.join();
	}
}

void HorusMaterial::set_sss_scatter_color(const std::array<float, 3>& color)
{
	if (color.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_COLOR, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{

		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_COLOR, color[0], color[1], color[2], 1));
	}
}

void HorusMaterial::set_sss_scatter_color(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_COLOR, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{

		std::thread loadTextureThread([&]()
			{

				std::lock_guard<std::mutex> lock(material_mutex);

				rpr_image texture_sss_scatter_color = load_texture(texturePath.c_str());

				rpr_material_node uberMat2_img_sss_scatter_color_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.get_matsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &uberMat2_img_sss_scatter_color_texture));
				Radeon.get_gc().GCAdd(uberMat2_img_sss_scatter_color_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_sss_scatter_color_texture, RPR_MATERIAL_INPUT_DATA, texture_sss_scatter_color));

				CHECK(rprMaterialNodeSetInputNByKey(m_material_, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_COLOR, uberMat2_img_sss_scatter_color_texture));

			});

		loadTextureThread.join();
	}
}

void HorusMaterial::set_sss_scatter_distance(const std::array<float, 3>& color)
{
	if (color.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_DISTANCE, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{

		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_DISTANCE, color[0], color[1], color[2], 1));
	}
}

void HorusMaterial::set_sss_scatter_distance(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_DISTANCE, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{

		std::thread loadTextureThread([&]()
			{

				std::lock_guard<std::mutex> lock(material_mutex);

				rpr_image texture_sss_scatter_distance = load_texture(texturePath.c_str());

				rpr_material_node uberMat2_img_sss_scatter_distance_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.get_matsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &uberMat2_img_sss_scatter_distance_texture));
				Radeon.get_gc().GCAdd(uberMat2_img_sss_scatter_distance_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_sss_scatter_distance_texture, RPR_MATERIAL_INPUT_DATA, texture_sss_scatter_distance));

				CHECK(rprMaterialNodeSetInputNByKey(m_material_, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_DISTANCE, uberMat2_img_sss_scatter_distance_texture));

			});

		loadTextureThread.join();
	}
}

void HorusMaterial::set_backscatter_weight(const std::array<float, 3>& color)
{
	if (color.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_BACKSCATTER_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{

		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_BACKSCATTER_WEIGHT, color[0], color[1], color[2], 1));
	}
}

void HorusMaterial::set_backscatter_weight(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_BACKSCATTER_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{

		std::thread loadTextureThread([&]()
			{

				std::lock_guard<std::mutex> lock(material_mutex);

				rpr_image texture_backscatter_weight = load_texture(texturePath.c_str());

				rpr_material_node uberMat2_img_backscatter_weight_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.get_matsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &uberMat2_img_backscatter_weight_texture));
				Radeon.get_gc().GCAdd(uberMat2_img_backscatter_weight_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_backscatter_weight_texture, RPR_MATERIAL_INPUT_DATA, texture_backscatter_weight));

				CHECK(rprMaterialNodeSetInputNByKey(m_material_, RPR_MATERIAL_INPUT_UBER_BACKSCATTER_WEIGHT, uberMat2_img_backscatter_weight_texture));

			});

		loadTextureThread.join();
	}
}

void HorusMaterial::set_backscatter_color(const std::array<float, 3>& color)
{
	if (color.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_BACKSCATTER_COLOR, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{

		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_BACKSCATTER_COLOR, color[0], color[1], color[2], 1));
	}
}

void HorusMaterial::set_backscatter_color(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_BACKSCATTER_COLOR, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		std::thread loadTextureThread([&]()
			{

				std::lock_guard<std::mutex> lock(material_mutex);

				rpr_image texture_backscatter_color = load_texture(texturePath.c_str());

				rpr_material_node uberMat2_img_backscatter_color_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.get_matsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &uberMat2_img_backscatter_color_texture));
				Radeon.get_gc().GCAdd(uberMat2_img_backscatter_color_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_backscatter_color_texture, RPR_MATERIAL_INPUT_DATA, texture_backscatter_color));

				CHECK(rprMaterialNodeSetInputNByKey(m_material_, RPR_MATERIAL_INPUT_UBER_BACKSCATTER_COLOR, uberMat2_img_backscatter_color_texture));

			});

		loadTextureThread.join();
	}
}

void HorusMaterial::set_sheen_weight(const std::array<float, 3>& weight)
{
	if (weight.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_SHEEN_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_SHEEN_WEIGHT, weight[0], weight[1], weight[2], 1));
	}
}

void HorusMaterial::set_sheen_weight(const std::string& texturePath)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	if (texturePath.empty())
	{
		CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_SHEEN_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f));
	}
	else
	{
		std::thread loadTextureThread([&]()
			{

				std::lock_guard<std::mutex> lock(material_mutex);

				rpr_image texture_sheen_weight = load_texture(texturePath.c_str());

				rpr_material_node uberMat2_img_sheen_weight_texture = nullptr;
				CHECK(rprMaterialSystemCreateNode(Radeon.get_matsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &uberMat2_img_sheen_weight_texture));
				Radeon.get_gc().GCAdd(uberMat2_img_sheen_weight_texture);
				CHECK(rprMaterialNodeSetInputImageDataByKey(uberMat2_img_sheen_weight_texture, RPR_MATERIAL_INPUT_DATA, texture_sheen_weight));

				CHECK(rprMaterialNodeSetInputNByKey(m_material_, RPR_MATERIAL_INPUT_UBER_SHEEN_WEIGHT, uberMat2_img_sheen_weight_texture));

			});

		loadTextureThread.join();
	}
}

void HorusMaterial::set_ior(float ior)
{
	CHECK(rprMaterialNodeSetInputFByKey(m_material_, RPR_MATERIAL_INPUT_UBER_REFRACTION_IOR, ior, ior, ior, 1));
}

// -----------------------------------------------------------------------------------------------
// Texture management ----------------------------------------------------------------------------

rpr_image HorusMaterial::load_texture(std::string path)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	rpr_image image = nullptr;

	if (rprContextCreateImageFromFile(Radeon.get_context(), path.c_str(), &image) != RPR_SUCCESS)
	{
		spdlog::error("Error: Texture -> {} not found.", path);
		return nullptr;
	}

	Radeon.get_gc().GCAdd(image);

	spdlog::info("Texture -> {} loaded.", path);

	return image;
}

// -----------------------------------------------------------------------------------------------
// Other functions --------------------------------------------------------------------------------

void HorusMaterial::set_background_image(const std::string& texturePath)
{
	HorusObjectManager& ObjectManager = HorusObjectManager::get_instance();
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	if (texturePath.empty())
	{
		spdlog::error("Error: Texture plate not found.");

		unset_background_image();
	}
	else
	{
		std::thread loadTextureThread([&]()
			{

				std::lock_guard<std::mutex> lock(material_mutex);

				rpr_image texture_background = load_texture(texturePath.c_str());

				rpr_status status = rprSceneSetBackgroundImage(ObjectManager.get_scene(), texture_background);

				Radeon.get_gc().GCAdd(texture_background);

			});

		loadTextureThread.join();
	}
}

void HorusMaterial::unset_background_image()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::get_instance();

	rpr_status status = rprSceneSetBackgroundImage(ObjectManager.get_scene(), nullptr);
}









