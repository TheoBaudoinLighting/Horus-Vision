#include "hrs_material_editor.h"

#include <iostream>
#include <algorithm>

#include "hrs_engine.h"
#include "spdlog/spdlog.h"

#include "hrs_radeon.h"
#include "imgui_internal.h"
#include "imnodes.h"
#include "L2DFileDialog.h"
#include "objects/hrs_object_manager.h"

//HorusRadeon& Radeon = HorusRadeon::get_instance();
//HorusObjectManager& ObjectManager = HorusObjectManager::get_instance();
//HorusEngine& Engine = HorusEngine::get_instance();

static float current_time_seconds = 0.f;
static bool  emulate_three_button_mouse = false;

template<class T>
T Clamp(T x, T a, T b)
{
	return std::min(b, std::max(x, a));
}

rpr_image load_texture(std::string path)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	rpr_image image = nullptr;

	if (rprContextCreateImageFromFile(Radeon.GetContext(), path.c_str(), &image) != RPR_SUCCESS)
	{
		spdlog::error("Error: Texture -> {} not found.", path);
		return nullptr;
	}

	gc.Add(image);

	spdlog::info("Texture -> {} loaded.", path);

	return image;
}

rpr_material_node HorusMaterialEditor::evaluate_rpr_material_node(const HorusGraph<HorusNode>& graph, const int root_node_id)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusEngine& Engine = HorusEngine::GetInstance(); m_garbage_collector_.GCClean();

	std::stack<int> post_order_stack;

	dfs_traverse(
		graph, root_node_id, [&post_order_stack](const int node_id) -> void { post_order_stack.push(node_id); });



	std::stack<HorusNodeMeta> material_node_stack;

	while (!post_order_stack.empty())
	{
		const int current_node_id = post_order_stack.top();
		post_order_stack.pop();

		const HorusNode& current_type = graph.node(current_node_id);

		switch (current_type.m_type_)
		{
		case HorusNodeType::Horus_PBR_Material:
		{
			const HorusNode& node = graph.node(current_node_id);


			HorusNodeMeta meta_top = material_node_stack.top();
			material_node_stack.pop();

			rpr_material_node new_node = meta_top.get_node();


			rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_UBERV2, &new_node);
			m_garbage_collector_.GCAdd(new_node);



			// Diffuse
			CHECK(rprMaterialNodeSetInputFByKey(new_node, RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR, node.m_color_diffuse_.x, node.m_color_diffuse_.y, node.m_color_diffuse_.z, node.m_color_diffuse_.w));
			CHECK(rprMaterialNodeSetInputFByKey(new_node, RPR_MATERIAL_INPUT_UBER_DIFFUSE_WEIGHT, node.m_weight_diffuse_.x, node.m_weight_diffuse_.y, node.m_weight_diffuse_.z, node.m_weight_diffuse_.w));
			CHECK(rprMaterialNodeSetInputFByKey(new_node, RPR_MATERIAL_INPUT_UBER_DIFFUSE_ROUGHNESS, node.m_roughness_diffuse_.x, node.m_roughness_diffuse_.y, node.m_roughness_diffuse_.z, node.m_roughness_diffuse_.w));

			// Reflection
			CHECK(rprMaterialNodeSetInputFByKey(new_node, RPR_MATERIAL_INPUT_UBER_REFLECTION_COLOR, node.m_color_reflection_.x, node.m_color_reflection_.y, node.m_color_reflection_.z, node.m_color_reflection_.w));
			CHECK(rprMaterialNodeSetInputFByKey(new_node, RPR_MATERIAL_INPUT_UBER_REFLECTION_WEIGHT, node.m_weight_reflection_.x, node.m_weight_reflection_.y, node.m_weight_reflection_.z, node.m_weight_reflection_.w));
			CHECK(rprMaterialNodeSetInputFByKey(new_node, RPR_MATERIAL_INPUT_UBER_REFLECTION_ROUGHNESS, node.m_roughness_reflection_.x, node.m_roughness_reflection_.y, node.m_roughness_reflection_.z, node.m_roughness_reflection_.w));
			CHECK(rprMaterialNodeSetInputFByKey(new_node, RPR_MATERIAL_INPUT_UBER_REFLECTION_ANISOTROPY, node.m_anisotropy_reflection_.x, node.m_anisotropy_reflection_.y, node.m_anisotropy_reflection_.z, node.m_anisotropy_reflection_.w));
			CHECK(rprMaterialNodeSetInputFByKey(new_node, RPR_MATERIAL_INPUT_UBER_REFLECTION_ANISOTROPY_ROTATION, node.m_rotation_reflection_.x, node.m_rotation_reflection_.y, node.m_rotation_reflection_.z, node.m_rotation_reflection_.w));
			CHECK(rprMaterialNodeSetInputUByKey(new_node, RPR_MATERIAL_INPUT_UBER_REFLECTION_MODE, RPR_UBER_MATERIAL_IOR_MODE_PBR)); // RPR_UBER_MATERIAL_IOR_MODE_METALNESS
			CHECK(rprMaterialNodeSetInputFByKey(new_node, RPR_MATERIAL_INPUT_UBER_REFLECTION_IOR, node.m_ior_reflection_.x, node.m_ior_reflection_.y, node.m_ior_reflection_.z, node.m_ior_reflection_.w));

			// Transmission
			CHECK(rprMaterialNodeSetInputFByKey(new_node, RPR_MATERIAL_INPUT_UBER_REFRACTION_COLOR, node.m_color_transmission_.x, node.m_color_transmission_.y, node.m_color_transmission_.z, node.m_color_transmission_.w));
			CHECK(rprMaterialNodeSetInputFByKey(new_node, RPR_MATERIAL_INPUT_UBER_REFRACTION_WEIGHT, node.m_weight_transmission_.x, node.m_weight_transmission_.y, node.m_weight_transmission_.z, node.m_weight_transmission_.w));
			CHECK(rprMaterialNodeSetInputFByKey(new_node, RPR_MATERIAL_INPUT_UBER_REFRACTION_ROUGHNESS, node.m_roughness_transmission_.x, node.m_roughness_transmission_.y, node.m_roughness_transmission_.z, node.m_roughness_transmission_.w));
			CHECK(rprMaterialNodeSetInputFByKey(new_node, RPR_MATERIAL_INPUT_UBER_REFRACTION_IOR, node.m_ior_transmission_.x, node.m_ior_transmission_.y, node.m_ior_transmission_.z, node.m_ior_transmission_.w));
			CHECK(rprMaterialNodeSetInputUByKey(new_node, RPR_MATERIAL_INPUT_UBER_REFRACTION_THIN_SURFACE, node.m_thin_surface_));
			CHECK(rprMaterialNodeSetInputFByKey(new_node, RPR_MATERIAL_INPUT_UBER_REFRACTION_ABSORPTION_COLOR, node.m_absorption_color_.x, node.m_absorption_color_.y, node.m_absorption_color_.z, node.m_absorption_color_.w));
			CHECK(rprMaterialNodeSetInputFByKey(new_node, RPR_MATERIAL_INPUT_UBER_REFRACTION_ABSORPTION_DISTANCE, node.m_absorption_distance_.x, node.m_absorption_distance_.y, node.m_absorption_distance_.z, node.m_absorption_distance_.w));
			CHECK(rprMaterialNodeSetInputUByKey(new_node, RPR_MATERIAL_INPUT_UBER_REFRACTION_CAUSTICS, node.m_caustics_));

			// Clearcoat
			CHECK(rprMaterialNodeSetInputFByKey(new_node, RPR_MATERIAL_INPUT_UBER_COATING_COLOR, node.m_color_clearcoat_.x, node.m_color_clearcoat_.y, node.m_color_clearcoat_.z, node.m_color_clearcoat_.w));
			CHECK(rprMaterialNodeSetInputFByKey(new_node, RPR_MATERIAL_INPUT_UBER_COATING_WEIGHT, node.m_weight_clearcoat_.x, node.m_weight_clearcoat_.y, node.m_weight_clearcoat_.z, node.m_weight_clearcoat_.w));
			CHECK(rprMaterialNodeSetInputFByKey(new_node, RPR_MATERIAL_INPUT_UBER_COATING_ROUGHNESS, node.m_roughness_clearcoat_.x, node.m_roughness_clearcoat_.y, node.m_roughness_clearcoat_.z, node.m_roughness_clearcoat_.w));
			CHECK(rprMaterialNodeSetInputUByKey(new_node, RPR_MATERIAL_INPUT_UBER_COATING_MODE, RPR_UBER_MATERIAL_IOR_MODE_PBR)); // RPR_UBER_MATERIAL_IOR_MODE_METALNESS
			CHECK(rprMaterialNodeSetInputFByKey(new_node, RPR_MATERIAL_INPUT_UBER_COATING_IOR, node.m_ior_clearcoat_.x, node.m_ior_clearcoat_.y, node.m_ior_clearcoat_.z, node.m_ior_clearcoat_.w));
			CHECK(rprMaterialNodeSetInputFByKey(new_node, RPR_MATERIAL_INPUT_UBER_COATING_METALNESS, node.m_metalness_clearcoat_.x, node.m_metalness_clearcoat_.y, node.m_metalness_clearcoat_.z, node.m_metalness_clearcoat_.w));
			CHECK(rprMaterialNodeSetInputFByKey(new_node, RPR_MATERIAL_INPUT_UBER_COATING_TRANSMISSION_COLOR, node.m_metalness_clearcoat_.x, node.m_metalness_clearcoat_.y, node.m_metalness_clearcoat_.z, node.m_metalness_clearcoat_.w));
			CHECK(rprMaterialNodeSetInputFByKey(new_node, RPR_MATERIAL_INPUT_UBER_COATING_THICKNESS, node.m_thickness_clearcoat_.x, node.m_thickness_clearcoat_.y, node.m_thickness_clearcoat_.z, node.m_thickness_clearcoat_.w));

			// Sheen
			CHECK(rprMaterialNodeSetInputFByKey(new_node, RPR_MATERIAL_INPUT_UBER_SHEEN, node.m_color_sheen_.x, node.m_color_sheen_.y, node.m_color_sheen_.z, node.m_color_sheen_.w));
			CHECK(rprMaterialNodeSetInputFByKey(new_node, RPR_MATERIAL_INPUT_UBER_SHEEN_TINT, node.m_tint_sheen_.x, node.m_tint_sheen_.y, node.m_tint_sheen_.z, node.m_tint_sheen_.w));
			CHECK(rprMaterialNodeSetInputFByKey(new_node, RPR_MATERIAL_INPUT_UBER_SHEEN_WEIGHT, node.m_weight_sheen_.x, node.m_weight_sheen_.y, node.m_weight_sheen_.z, node.m_weight_sheen_.w));

			// Emission
			CHECK(rprMaterialNodeSetInputFByKey(new_node, RPR_MATERIAL_INPUT_UBER_EMISSION_COLOR, node.m_emission_color_.x, node.m_emission_color_.y, node.m_emission_color_.z, node.m_emission_color_.w));
			CHECK(rprMaterialNodeSetInputFByKey(new_node, RPR_MATERIAL_INPUT_UBER_EMISSION_WEIGHT, node.m_emission_weight_.x, node.m_emission_weight_.y, node.m_emission_weight_.z, node.m_emission_weight_.w));
			CHECK(rprMaterialNodeSetInputUByKey(new_node, RPR_MATERIAL_INPUT_UBER_EMISSION_MODE, 1));

			// Opacity
			CHECK(rprMaterialNodeSetInputFByKey(new_node, RPR_MATERIAL_INPUT_UBER_TRANSPARENCY, 1.0f - node.m_opacity_.x, 1.0f - node.m_opacity_.y, 1.0f - node.m_opacity_.z, 1.0f - node.m_opacity_.w));


			// SSS
			CHECK(rprMaterialNodeSetInputFByKey(new_node, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_COLOR, node.m_color_sss_.x, node.m_color_sss_.y, node.m_color_sss_.z, node.m_color_sss_.w));
			CHECK(rprMaterialNodeSetInputFByKey(new_node, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_DISTANCE, node.m_radius_sss_.x, node.m_radius_sss_.y, node.m_radius_sss_.z, node.m_radius_sss_.w));
			CHECK(rprMaterialNodeSetInputFByKey(new_node, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_DIRECTION, node.m_direction_sss_.x, node.m_direction_sss_.y, node.m_direction_sss_.z, node.m_direction_sss_.w));
			CHECK(rprMaterialNodeSetInputFByKey(new_node, RPR_MATERIAL_INPUT_UBER_SSS_WEIGHT, node.m_weight_sss_.x, node.m_weight_sss_.y, node.m_weight_sss_.z, node.m_weight_sss_.w));
			CHECK(rprMaterialNodeSetInputUByKey(new_node, RPR_MATERIAL_INPUT_UBER_SSS_MULTISCATTER, node.m_multi_scatter_));

			// Backscattering
			CHECK(rprMaterialNodeSetInputFByKey(new_node, RPR_MATERIAL_INPUT_UBER_BACKSCATTER_WEIGHT, node.m_backscattering_weight_.x, node.m_backscattering_weight_.y, node.m_backscattering_weight_.z, node.m_backscattering_weight_.w));
			CHECK(rprMaterialNodeSetInputFByKey(new_node, RPR_MATERIAL_INPUT_UBER_BACKSCATTER_COLOR, node.m_backscattering_color_.x, node.m_backscattering_color_.y, node.m_backscattering_color_.z, node.m_backscattering_color_.w));


			meta_top.set_node(new_node);
			meta_top.set_node_type(HorusNodeType::Horus_PBR_Material);
			material_node_stack.push(meta_top);

		}
		break;

		case HorusNodeType::Image_texture:
		{
			const HorusNode& node = graph.node(current_node_id);

			HorusNodeMeta meta_top = material_node_stack.top();
			material_node_stack.pop();



			rpr_image image = nullptr;

			image = load_texture(node.m_image_);
			m_garbage_collector_.GCAdd(image);

			if (node.m_image_ == nullptr)
			{
				std::cout << "Error: Image texture -> " << node.m_image_ << " not found." << std::endl;
				return nullptr;
			}
			else
			{
				std::cout << "Image texture -> " << node.m_image_ << " loaded." << std::endl;

				rpr_material_node new_node = meta_top.get_node();
				material_node_stack.pop();

				CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &new_node));
				m_garbage_collector_.GCAdd(new_node);

				CHECK(rprMaterialNodeSetInputImageDataByKey(new_node, RPR_MATERIAL_INPUT_DATA, image));


				meta_top.set_node(new_node);
				meta_top.set_node_type(HorusNodeType::Image_texture);
				meta_top.set_image(image);
				material_node_stack.push(meta_top);
			}


		}
		break;

		case HorusNodeType::Debug_color01:
		{

			// ID ok ici -> bonne methode pour le get
			const HorusNode& node = graph.node(current_node_id);


			HorusNodeMeta meta_top = material_node_stack.top();

			rpr_material_node new_node = meta_top.get_node();
			material_node_stack.pop();



			rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_UBERV2, &new_node);
			m_garbage_collector_.GCAdd(new_node);

			rprMaterialNodeSetInputFByKey(new_node, RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR, node.m_color_.x, node.m_color_.y, node.m_color_.z, node.m_color_.w);

			meta_top.set_node(new_node);
			meta_top.set_node_type(HorusNodeType::Debug_color01);

			material_node_stack.push(meta_top);
		}
		break;

		case HorusNodeType::Value:
		{
			if (graph.num_edges_from_node(current_node_id) == 0ull) // Previous code
			{
				material_node_stack.push(current_type.m_value_);
			}
		}
		break;
		}
	}

	if (!material_node_stack.empty())
	{
		HorusNodeMeta meta_top = material_node_stack.top();
		rpr_material_node new_node = meta_top.get_node();

		m_out_modified_ = new_node;

		return m_out_modified_;
	}
}

void HorusMaterialEditor::init()
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusEngine& Engine = HorusEngine::GetInstance();
	HorusGarbageCollector& gc = HorusGarbageCollector::GetInstance();

	CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_UBERV2, &m_out_node_));
	gc.Add(m_out_node_);

	CHECK(rprMaterialNodeSetInputFByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR, 1.f, 0.5f, 0.5f, 1.0f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_DIFFUSE_WEIGHT, 1.0f, 1.0f, 1.0f, 1.0f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_DIFFUSE_ROUGHNESS, 0.5f, 0.5f, 0.5f, 1.0f));

	CHECK(rprMaterialNodeSetInputFByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_REFLECTION_COLOR, 1.f, 1.f, 1.f, 1.f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_REFLECTION_WEIGHT, 1.f, 1.f, 1.f, 1.f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_REFLECTION_ROUGHNESS, 0.5f, 0.5f, 0.5f, 1.0f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_REFLECTION_ANISOTROPY, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_REFLECTION_ANISOTROPY_ROTATION, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputUByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_REFLECTION_MODE, RPR_UBER_MATERIAL_IOR_MODE_PBR)); // RPR_UBER_MATERIAL_IOR_MODE_METALNESS
	CHECK(rprMaterialNodeSetInputFByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_REFLECTION_IOR, 1.460000f, 1.460000f, 1.460000f, 1.460000f));

	CHECK(rprMaterialNodeSetInputFByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_REFRACTION_COLOR, 1.0f, 1.0f, 1.0f, 1.0f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_REFRACTION_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_REFRACTION_ROUGHNESS, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_REFRACTION_IOR, 1.500000f, 1.500000f, 1.500000f, 1.500000f));
	CHECK(rprMaterialNodeSetInputUByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_REFRACTION_THIN_SURFACE, 0));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_REFRACTION_ABSORPTION_COLOR, 1.0f, 1.0f, 1.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_REFRACTION_ABSORPTION_DISTANCE, 1.0f, 1.0f, 1.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputUByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_REFRACTION_CAUSTICS, 1));

	CHECK(rprMaterialNodeSetInputFByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_COATING_COLOR, 0.5f, 0.5f, 0.5f, 1.0f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_COATING_WEIGHT, 0.f, 0.f, 0.f, 0.f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_COATING_ROUGHNESS, 0.f, 0.f, 0.f, 0.f));
	CHECK(rprMaterialNodeSetInputUByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_COATING_MODE, RPR_UBER_MATERIAL_IOR_MODE_PBR)); // RPR_UBER_MATERIAL_IOR_MODE_METALNESS
	CHECK(rprMaterialNodeSetInputFByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_COATING_IOR, 1.460000f, 1.460000f, 1.460000f, 1.460000f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_COATING_METALNESS, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_COATING_TRANSMISSION_COLOR, 0.0f, 0.0f, 0.0f, 1.0f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_COATING_THICKNESS, 0.0f, 0.0f, 0.0f, 0.0f));

	CHECK(rprMaterialNodeSetInputFByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_SHEEN, 1.0f, 1.0f, 1.0f, 1.0f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_SHEEN_TINT, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_SHEEN_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f));

	CHECK(rprMaterialNodeSetInputFByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_EMISSION_COLOR, 1.0f, 1.0f, 1.0f, 1.0f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_EMISSION_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputUByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_EMISSION_MODE, 1));

	CHECK(rprMaterialNodeSetInputFByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_TRANSPARENCY, 0.0f, 0.0f, 0.0f, 0.0f));

	CHECK(rprMaterialNodeSetInputFByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_COLOR, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_DISTANCE, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_DIRECTION, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_SSS_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputUByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_SSS_MULTISCATTER, 0));

	CHECK(rprMaterialNodeSetInputFByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_BACKSCATTER_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_BACKSCATTER_COLOR, 0.0f, 0.0f, 0.0f, 0.0f));




	CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_UBERV2, &m_out_modified_));
	gc.Add(m_out_modified_);

	CHECK(rprMaterialNodeSetInputFByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR, 0.5f, 0.5f, 1.0f, 1.0f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_DIFFUSE_WEIGHT, 1.0f, 1.0f, 1.0f, 1.0f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_DIFFUSE_ROUGHNESS, 0.5f, 0.5f, 0.5f, 1.0f));

	CHECK(rprMaterialNodeSetInputFByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_REFLECTION_COLOR, 1.f, 1.f, 1.f, 1.f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_REFLECTION_WEIGHT, 1.f, 1.f, 1.f, 1.f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_REFLECTION_ROUGHNESS, 0.5f, 0.5f, 0.5f, 1.0f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_REFLECTION_ANISOTROPY, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_REFLECTION_ANISOTROPY_ROTATION, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputUByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_REFLECTION_MODE, RPR_UBER_MATERIAL_IOR_MODE_PBR)); // RPR_UBER_MATERIAL_IOR_MODE_METALNESS
	CHECK(rprMaterialNodeSetInputFByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_REFLECTION_IOR, 1.460000f, 1.460000f, 1.460000f, 1.460000f));

	CHECK(rprMaterialNodeSetInputFByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_REFRACTION_COLOR, 1.0f, 1.0f, 1.0f, 1.0f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_REFRACTION_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_REFRACTION_ROUGHNESS, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_REFRACTION_IOR, 1.500000f, 1.500000f, 1.500000f, 1.500000f));
	CHECK(rprMaterialNodeSetInputUByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_REFRACTION_THIN_SURFACE, 0));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_REFRACTION_ABSORPTION_COLOR, 1.0f, 1.0f, 1.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_REFRACTION_ABSORPTION_DISTANCE, 1.0f, 1.0f, 1.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputUByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_REFRACTION_CAUSTICS, 1));

	CHECK(rprMaterialNodeSetInputFByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_COATING_COLOR, 0.5f, 0.5f, 0.5f, 1.0f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_COATING_WEIGHT, 0.f, 0.f, 0.f, 0.f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_COATING_ROUGHNESS, 0.f, 0.f, 0.f, 0.f));
	CHECK(rprMaterialNodeSetInputUByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_COATING_MODE, RPR_UBER_MATERIAL_IOR_MODE_PBR)); // RPR_UBER_MATERIAL_IOR_MODE_METALNESS
	CHECK(rprMaterialNodeSetInputFByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_COATING_IOR, 1.460000f, 1.460000f, 1.460000f, 1.460000f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_COATING_METALNESS, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_COATING_TRANSMISSION_COLOR, 0.0f, 0.0f, 0.0f, 1.0f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_COATING_THICKNESS, 0.0f, 0.0f, 0.0f, 0.0f));

	CHECK(rprMaterialNodeSetInputFByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_SHEEN, 1.0f, 1.0f, 1.0f, 1.0f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_SHEEN_TINT, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_SHEEN_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f));

	CHECK(rprMaterialNodeSetInputFByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_EMISSION_COLOR, 1.0f, 1.0f, 1.0f, 1.0f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_EMISSION_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputUByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_EMISSION_MODE, 1));

	CHECK(rprMaterialNodeSetInputFByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_TRANSPARENCY, 0.0f, 0.0f, 0.0f, 0.0f));

	CHECK(rprMaterialNodeSetInputFByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_COLOR, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_DISTANCE, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_DIRECTION, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_SSS_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputUByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_SSS_MULTISCATTER, 0));

	CHECK(rprMaterialNodeSetInputFByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_BACKSCATTER_WEIGHT, 0.0f, 0.0f, 0.0f, 0.0f));
	CHECK(rprMaterialNodeSetInputFByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_BACKSCATTER_COLOR, 0.0f, 0.0f, 0.0f, 0.0f));




	//ObjectManager.set_material_from_editor_node(1, m_out_node_);
}

void HorusMaterialEditor::update()
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusEngine& Engine = HorusEngine::GetInstance();

	auto flags = ImGuiWindowFlags_MenuBar;
	ImGui::Begin("Material Editor", nullptr, flags);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New"))
			{

			}
			if (ImGui::MenuItem("Open"))
			{

			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Mini-Map"))
		{
			const char* names[] = {
					"Top Left",
					"Top Right",
					"Bottom Left",
					"Bottom Right",
			};
			int locations[] = {
				ImNodesMiniMapLocation_TopLeft,
				ImNodesMiniMapLocation_TopRight,
				ImNodesMiniMapLocation_BottomLeft,
				ImNodesMiniMapLocation_BottomRight,
			};

			for (int i = 0; i < 4; i++)
			{
				bool selected = m_mini_map_location_ == locations[i];
				if (ImGui::MenuItem(names[i], NULL, &selected))
					m_mini_map_location_ = locations[i];
			}

			ImGui::EndMenu();
		}



		ImGui::EndMenuBar();
	}

	ImNodes::BeginNodeEditor();
	{
		const bool open_popup = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && ImNodes::IsEditorHovered() && ImGui::IsKeyPressedMap(ImGuiKey_MouseRight);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.f, 8.f));
		if (!ImGui::IsAnyItemHovered() && open_popup)
		{
			ImGui::OpenPopup("Create Nodes");
		}

		// Menu for creating nodes
		if (ImGui::BeginPopup("Create Nodes"))
		{
			const ImVec2 click_pos_on_canvas = ImGui::GetMousePosOnOpeningCurrentPopup();

			if (ImGui::BeginMenu("Material"))
			{
				if (ImGui::MenuItem("Horus PBR Material"))
				{
					const HorusNode value(HorusNodeType::Value, 0.0f);
					const HorusNode op(HorusNodeType::Horus_PBR_Material);

					HorusUiNode ui_node = {};
					ui_node.type_ = HorusUiNodeType::Horus_PBR_Material;

					// Diffuse
					ui_node.pbr_material.input_color_diffuse = m_graph_.insert_node(value);
					ui_node.pbr_material.input_weight_diffuse = m_graph_.insert_node(value);
					ui_node.pbr_material.input_roughness_diffuse = m_graph_.insert_node(value);

					// Reflection
					ui_node.pbr_material.input_color_reflection = m_graph_.insert_node(value);
					ui_node.pbr_material.input_weight_reflection = m_graph_.insert_node(value);
					ui_node.pbr_material.input_roughness_reflection = m_graph_.insert_node(value);
					ui_node.pbr_material.input_BRDF_reflection = m_graph_.insert_node(value);
					ui_node.pbr_material.input_anisotropy_reflection = m_graph_.insert_node(value);
					ui_node.pbr_material.input_rotation_reflection = m_graph_.insert_node(value);
					ui_node.pbr_material.input_mode_reflection = m_graph_.insert_node(value);
					ui_node.pbr_material.input_ior_reflection = m_graph_.insert_node(value);

					// Sheen
					ui_node.pbr_material.input_color_sheen = m_graph_.insert_node(value);
					ui_node.pbr_material.input_weight_sheen = m_graph_.insert_node(value);
					ui_node.pbr_material.input_tint_sheen = m_graph_.insert_node(value);

					// Clear Coat
					ui_node.pbr_material.input_color_clearcoat = m_graph_.insert_node(value);
					ui_node.pbr_material.input_weight_clearcoat = m_graph_.insert_node(value);
					ui_node.pbr_material.input_roughness_clearcoat = m_graph_.insert_node(value);
					ui_node.pbr_material.input_ior_clearcoat = m_graph_.insert_node(value);

					// Transmission
					ui_node.pbr_material.input_color_transmission = m_graph_.insert_node(value);
					ui_node.pbr_material.input_weight_transmission = m_graph_.insert_node(value);
					ui_node.pbr_material.input_roughness_transmission = m_graph_.insert_node(value);
					ui_node.pbr_material.input_ior_transmission = m_graph_.insert_node(value);

					// Normal, Bump, Displacement
					ui_node.pbr_material.input_normal = m_graph_.insert_node(value);
					ui_node.pbr_material.input_bump = m_graph_.insert_node(value);
					ui_node.pbr_material.input_displacement = m_graph_.insert_node(value);

					// Emission
					ui_node.pbr_material.input_emission_color = m_graph_.insert_node(value);
					ui_node.pbr_material.input_emission_weight = m_graph_.insert_node(value);

					// Opacity
					ui_node.pbr_material.input_opacity = m_graph_.insert_node(value);
					ui_node.pbr_material.input_opacity_weight = m_graph_.insert_node(value);

					// Backscatter
					ui_node.pbr_material.input_backscattering_weight = m_graph_.insert_node(value);
					ui_node.pbr_material.input_backscattering_color = m_graph_.insert_node(value);

					// SSS
					ui_node.pbr_material.input_color_sss = m_graph_.insert_node(value);
					ui_node.pbr_material.input_weight_sss = m_graph_.insert_node(value);
					ui_node.pbr_material.input_radius_sss = m_graph_.insert_node(value);
					ui_node.pbr_material.input_direction_sss = m_graph_.insert_node(value);
					ui_node.pbr_material.input_multi_scatter = m_graph_.insert_node(value);

					// ID
					ui_node.id_ = m_graph_.insert_node(op);

					m_graph_.insert_edge(ui_node.id_, ui_node.pbr_material.input_color_diffuse);
					m_graph_.insert_edge(ui_node.id_, ui_node.pbr_material.input_weight_diffuse);
					m_graph_.insert_edge(ui_node.id_, ui_node.pbr_material.input_roughness_diffuse);
					m_graph_.insert_edge(ui_node.id_, ui_node.pbr_material.input_color_reflection);
					m_graph_.insert_edge(ui_node.id_, ui_node.pbr_material.input_weight_reflection);
					m_graph_.insert_edge(ui_node.id_, ui_node.pbr_material.input_roughness_reflection);
					m_graph_.insert_edge(ui_node.id_, ui_node.pbr_material.input_BRDF_reflection);
					m_graph_.insert_edge(ui_node.id_, ui_node.pbr_material.input_anisotropy_reflection);
					m_graph_.insert_edge(ui_node.id_, ui_node.pbr_material.input_rotation_reflection);
					m_graph_.insert_edge(ui_node.id_, ui_node.pbr_material.input_mode_reflection);
					m_graph_.insert_edge(ui_node.id_, ui_node.pbr_material.input_ior_reflection);
					m_graph_.insert_edge(ui_node.id_, ui_node.pbr_material.input_color_sheen);
					m_graph_.insert_edge(ui_node.id_, ui_node.pbr_material.input_weight_sheen);
					m_graph_.insert_edge(ui_node.id_, ui_node.pbr_material.input_tint_sheen);
					m_graph_.insert_edge(ui_node.id_, ui_node.pbr_material.input_color_clearcoat);
					m_graph_.insert_edge(ui_node.id_, ui_node.pbr_material.input_weight_clearcoat);
					m_graph_.insert_edge(ui_node.id_, ui_node.pbr_material.input_roughness_clearcoat);
					m_graph_.insert_edge(ui_node.id_, ui_node.pbr_material.input_ior_clearcoat);
					m_graph_.insert_edge(ui_node.id_, ui_node.pbr_material.input_color_transmission);
					m_graph_.insert_edge(ui_node.id_, ui_node.pbr_material.input_weight_transmission);
					m_graph_.insert_edge(ui_node.id_, ui_node.pbr_material.input_roughness_transmission);
					m_graph_.insert_edge(ui_node.id_, ui_node.pbr_material.input_ior_transmission);
					m_graph_.insert_edge(ui_node.id_, ui_node.pbr_material.input_normal);
					m_graph_.insert_edge(ui_node.id_, ui_node.pbr_material.input_bump);
					m_graph_.insert_edge(ui_node.id_, ui_node.pbr_material.input_displacement);
					m_graph_.insert_edge(ui_node.id_, ui_node.pbr_material.input_emission_color);
					m_graph_.insert_edge(ui_node.id_, ui_node.pbr_material.input_emission_weight);
					m_graph_.insert_edge(ui_node.id_, ui_node.pbr_material.input_opacity);
					m_graph_.insert_edge(ui_node.id_, ui_node.pbr_material.input_opacity_weight);
					m_graph_.insert_edge(ui_node.id_, ui_node.pbr_material.input_backscattering_weight);
					m_graph_.insert_edge(ui_node.id_, ui_node.pbr_material.input_backscattering_color);
					m_graph_.insert_edge(ui_node.id_, ui_node.pbr_material.input_color_sss);
					m_graph_.insert_edge(ui_node.id_, ui_node.pbr_material.input_weight_sss);
					m_graph_.insert_edge(ui_node.id_, ui_node.pbr_material.input_radius_sss);
					m_graph_.insert_edge(ui_node.id_, ui_node.pbr_material.input_direction_sss);
					m_graph_.insert_edge(ui_node.id_, ui_node.pbr_material.input_multi_scatter);

					HorusNodeMeta meta;
					meta.set_id(ui_node.id_);
					meta.set_type(HorusNodeType::Horus_PBR_Material);

					m_nodes_.push_back(ui_node);
					ImNodes::SetNodeScreenSpacePos(ui_node.id_, click_pos_on_canvas);
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Input"))
			{
				if (ImGui::MenuItem("Image_texture"))
				{
					const HorusNode value(HorusNodeType::Value, 0.0f);
					const HorusNode op(HorusNodeType::Image_texture);

					HorusUiNode ui_node = {};
					ui_node.type_ = HorusUiNodeType::Image_texture;
					ui_node.image_texture.input0 = m_graph_.insert_node(value);
					ui_node.id_ = m_graph_.insert_node(op);

					m_graph_.insert_edge(ui_node.id_, ui_node.image_texture.input0);
					m_nodes_.push_back(ui_node);

					HorusNodeMeta meta;
					meta.set_id(ui_node.id_);
					meta.set_type(HorusNodeType::Image_texture);

					ImNodes::SetNodeScreenSpacePos(ui_node.id_, click_pos_on_canvas);
				}

				// ....

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Output"))
			{
				if (ImGui::MenuItem("Output") && m_root_node_ID_ == -1)
				{
					const HorusNode value(HorusNodeType::Value, 0.0f);
					const HorusNode out(HorusNodeType::Output);

					HorusUiNode ui_node = {};
					ui_node.type_ = HorusUiNodeType::Output;
					ui_node.output.output = m_graph_.insert_node(value);
					ui_node.id_ = m_graph_.insert_node(out);

					m_graph_.insert_edge(ui_node.id_, ui_node.output.output);

					m_nodes_.push_back(ui_node);
					ImNodes::SetNodeScreenSpacePos(ui_node.id_, click_pos_on_canvas);
					m_root_node_ID_ = ui_node.id_;
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Debug"))
			{
				if (ImGui::MenuItem("Color 01"))
				{
					// TODO Fix this

					HorusNode value(HorusNodeType::Value, 0.0f);
					HorusNode op(HorusNodeType::Debug_color01);
					op.m_color_ = { 1.0f, 0.0f, 0.0f, 1.0f };


					HorusUiNode ui_node;
					ui_node.type_ = HorusUiNodeType::Debug_color01;
					ui_node.Debug_color01.input = m_graph_.insert_node(value);
					ui_node.id_ = m_graph_.insert_node(op);

					m_graph_.insert_edge(ui_node.id_, ui_node.Debug_color01.input);
					m_nodes_.push_back(ui_node);

					HorusNodeMeta meta;
					meta.set_id(ui_node.id_);
					meta.set_type(HorusNodeType::Debug_color01);

					ImNodes::SetNodeScreenSpacePos(ui_node.id_, click_pos_on_canvas);
				}

				ImGui::EndMenu();
			}

			ImGui::EndPopup();
		}
		ImGui::PopStyleVar();
	}

	for (const HorusUiNode& node : m_nodes_) // Draw nodes
	{
		switch (node.type_)
		{
		case HorusUiNodeType::Horus_PBR_Material: // Horus PBR Material, First test with this node
		{
			const float node_width = 100.f;

			ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(255, 204, 204, 255));
			ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(255, 153, 153, 255));
			ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(255, 102, 102, 255));

			ImNodes::BeginNode(node.id_);
			HorusNode& nodeU = m_graph_.node(node.id_);


			ImNodes::BeginNodeTitleBar();
			ImGui::TextColored(ImVec4(0.0f, 0.0f, 0.0f, 1.0f), "PRB Material");
			ImNodes::EndNodeTitleBar();

			static bool needUpdate = false;
			static float width_node = 130;
			float max_label_width = ImGui::CalcTextSize("Roughness Clearcoat").x;
			float cursor_x = max_label_width + 10.0f;
			float speed_value = 0.015f;

			{
				ImNodes::BeginOutputAttribute(node.id_);
				const float label_width = ImGui::CalcTextSize("Result").x;
				ImGui::Indent(width_node);
				ImGui::Text("Result");
				ImNodes::EndOutputAttribute();
			}

			// Diffuse Section ---------------------------------------------------------------------------------------- Diffuse Section

			{
				{
					ImNodes::BeginInputAttribute(node.pbr_material.input_color_diffuse);
					const float label_width = ImGui::CalcTextSize("Diffuse color").x;
					ImGui::Text("Diffuse color");

					ImGui::SameLine(max_label_width + 10);
					ImGui::SetNextItemWidth(width_node);

					if (m_graph_.num_edges_from_node(node.pbr_material.input_color_diffuse) == 0ull)
					{

						ImGui::SameLine(max_label_width + 10);
						ImGui::SetNextItemWidth(width_node);

						if (ImGui::ColorEdit4("##color_diffuse", (float*)&nodeU.m_color_diffuse_))
						{
							nodeU.m_use_diffuse_texture_ = false;
							spdlog::info("not Diffuse color");
							needUpdate = true;
						}
					}
					else if (m_graph_.num_edges_from_node(node.pbr_material.input_color_diffuse) == 1ull)
					{
						nodeU.m_use_diffuse_texture_ = true;
					}
					else if (m_graph_.num_edges_from_node(node.pbr_material.input_color_diffuse) > 1ull)
					{
						nodeU.m_use_diffuse_texture_ = true;
					}



					ImNodes::EndInputAttribute();
				}

				{
					ImNodes::BeginInputAttribute(node.pbr_material.input_weight_diffuse);
					const float label_width = ImGui::CalcTextSize("Diffuse weight ").x;
					ImGui::Text("Diffuse weight");

					if (m_graph_.num_edges_from_node(node.pbr_material.input_weight_diffuse) == 0ull)
					{

						ImGui::SameLine(max_label_width + 10);
						ImGui::SetNextItemWidth(width_node);

						if (ImGui::DragFloat("##weight_diffuse", (float*)&nodeU.m_weight_diffuse_, speed_value, 0.f, 1.f)) // Speed, VMin, Vmax
						{
							needUpdate = true;
						}
					}
					ImNodes::EndInputAttribute();
				}

				{
					ImNodes::BeginInputAttribute(node.pbr_material.input_roughness_diffuse);
					const float label_width = ImGui::CalcTextSize("Roughness Diffuse").x;
					ImGui::Text("Roughness Diffuse");

					if (m_graph_.num_edges_from_node(node.pbr_material.input_roughness_diffuse) == 0ull)
					{
						ImGui::SameLine(max_label_width + 10);
						ImGui::SetNextItemWidth(width_node);
						if (ImGui::DragFloat("##roughness_diffuse", (float*)&nodeU.m_roughness_diffuse_, speed_value, 0.0f, 1.0f))
						{
							needUpdate = true;
						}
					}
					ImNodes::EndInputAttribute();
				}

				{
					ImNodes::BeginInputAttribute(node.pbr_material.input_backscattering_color);
					const float label_width = ImGui::CalcTextSize("Backscattering color").x;
					ImGui::Text("Backscattering color");
					if (m_graph_.num_edges_from_node(node.pbr_material.input_backscattering_color) == 0ull)
					{
						ImGui::SameLine(max_label_width + 10);
						ImGui::SetNextItemWidth(width_node);
						if (ImGui::ColorEdit4("##backscattering_color", (float*)&nodeU.m_backscattering_color_))
						{
							needUpdate = true;
						}
					}
					ImNodes::EndInputAttribute();
				}

				{
					ImNodes::BeginInputAttribute(node.pbr_material.input_backscattering_weight);
					const float label_width = ImGui::CalcTextSize("Backscattering weight").x;
					ImGui::Text("Backscattering weight");
					if (m_graph_.num_edges_from_node(node.pbr_material.input_backscattering_weight) == 0ull)
					{
						ImGui::SameLine(max_label_width + 10);
						ImGui::SetNextItemWidth(width_node);
						if (ImGui::DragFloat("##backscattering_weight", (float*)&nodeU.m_backscattering_weight_, speed_value, 0.0f, 1.0f))
						{
							needUpdate = true;
						}
					}
					ImNodes::EndInputAttribute();
				}
			}

			// Reflection Section ------------------------------------------------------------------------------------- Reflection Section

			{
				{
					ImNodes::BeginInputAttribute(node.pbr_material.input_color_reflection);
					const float label_width = ImGui::CalcTextSize("Reflection color").x;
					ImGui::Text("Reflection color");

					if (m_graph_.num_edges_from_node(node.pbr_material.input_color_reflection) == 0ull)
					{
						ImGui::SameLine(max_label_width + 10);
						ImGui::SetNextItemWidth(width_node);
						if (ImGui::ColorEdit4("##color_reflection", (float*)&nodeU.m_color_reflection_))
						{
							needUpdate = true;
						}
					}
					ImNodes::EndInputAttribute();
				}

				{
					ImNodes::BeginInputAttribute(node.pbr_material.input_weight_reflection);
					const float label_width = ImGui::CalcTextSize("Reflection weight").x;
					ImGui::Text("Reflection weight");

					if (m_graph_.num_edges_from_node(node.pbr_material.input_weight_reflection) == 0ull)
					{
						ImGui::SameLine(max_label_width + 10);
						ImGui::SetNextItemWidth(width_node);
						if (ImGui::DragFloat("##weight_reflection", (float*)&nodeU.m_weight_reflection_, speed_value, 0.0f, 1.0f))
						{
							needUpdate = true;
						}
					}
					ImNodes::EndInputAttribute();
				}

				{
					ImNodes::BeginInputAttribute(node.pbr_material.input_roughness_reflection);
					const float label_width = ImGui::CalcTextSize("Roughness Reflection").x;
					ImGui::Text("Roughness Reflection");

					if (m_graph_.num_edges_from_node(node.pbr_material.input_roughness_reflection) == 0ull)
					{
						ImGui::SameLine(max_label_width + 10);
						ImGui::SetNextItemWidth(width_node);
						if (ImGui::DragFloat("##roughness_reflection", (float*)&nodeU.m_roughness_reflection_, speed_value, 0.0f, 1.0f))
						{
							needUpdate = true;
						}
					}
					ImNodes::EndInputAttribute();
				}

				{
					ImNodes::BeginInputAttribute(node.pbr_material.input_BRDF_reflection);
					const float label_width = ImGui::CalcTextSize("BRDF Reflection").x;
					ImGui::Text("BRDF Reflection");

					if (m_graph_.num_edges_from_node(node.pbr_material.input_BRDF_reflection) == 0ull)
					{
						ImGui::SameLine(max_label_width + 10);
						ImGui::SetNextItemWidth(width_node);
						if (ImGui::Combo("##BRDF_reflection", (int*)&nodeU.m_BRDF_reflection_, "Blinn-Phong\0Cook-Torrance\0\0"))
						{
							needUpdate = true;
						}
					}
					ImNodes::EndInputAttribute();
				}

				{
					ImNodes::BeginInputAttribute(node.pbr_material.input_anisotropy_reflection);
					const float label_width = ImGui::CalcTextSize("Anisotropy Reflection").x;
					ImGui::Text("Anisotropy Reflection");

					if (m_graph_.num_edges_from_node(node.pbr_material.input_anisotropy_reflection) == 0ull)
					{
						ImGui::SameLine(max_label_width + 10);
						ImGui::SetNextItemWidth(width_node);
						if (ImGui::DragFloat("##anisotropy_reflection", (float*)&nodeU.m_anisotropy_reflection_, speed_value, 0.0f, 1.0f))
						{
							needUpdate = true;
						}


					}
					ImNodes::EndInputAttribute();
				}

				{
					ImNodes::BeginInputAttribute(node.pbr_material.input_rotation_reflection);
					const float label_width = ImGui::CalcTextSize("Rotation Reflection").x;
					ImGui::Text("Rotation Reflection");

					if (m_graph_.num_edges_from_node(node.pbr_material.input_rotation_reflection) == 0ull)
					{
						ImGui::SameLine(max_label_width + 10);
						ImGui::SetNextItemWidth(width_node);
						if (ImGui::DragFloat("##rotation_reflection", (float*)&nodeU.m_rotation_reflection_, speed_value, 0.0f, 1.0f))
						{
							needUpdate = true;
						}
					}
					ImNodes::EndInputAttribute();
				}

				{
					ImNodes::BeginInputAttribute(node.pbr_material.input_mode_reflection);
					const float label_width = ImGui::CalcTextSize("Mode Reflection").x;
					ImGui::Text("Mode Reflection");

					if (m_graph_.num_edges_from_node(node.pbr_material.input_mode_reflection) == 0ull)
					{
						ImGui::SameLine(max_label_width + 10);
						ImGui::SetNextItemWidth(width_node);
						if (ImGui::Combo("##mode_reflection", (int*)&nodeU.m_mode_reflection_, "GGX\0Beckmann\0\0"))
						{
							needUpdate = true;
						}
					}
					ImNodes::EndInputAttribute();
				}

				{
					ImNodes::BeginInputAttribute(node.pbr_material.input_ior_reflection);
					const float label_width = ImGui::CalcTextSize("IOR Reflection").x;
					ImGui::Text("IOR Reflection");

					if (m_graph_.num_edges_from_node(node.pbr_material.input_ior_reflection) == 0ull)
					{
						ImGui::SameLine(max_label_width + 10);
						ImGui::SetNextItemWidth(width_node);
						if (ImGui::DragFloat("##ior_reflection", (float*)&nodeU.m_ior_reflection_, speed_value, 1.0f, 2.0f))
						{
							needUpdate = true;
						}

					}
					ImNodes::EndInputAttribute();
				}
			}

			// Sheen Section ------------------------------------------------------------------------------------------ Sheen Section

			{
				ImNodes::BeginInputAttribute(node.pbr_material.input_color_sheen);
				const float label_width = ImGui::CalcTextSize("Sheen color").x;
				ImGui::Text("Sheen color");

				if (m_graph_.num_edges_from_node(node.pbr_material.input_color_sheen) == 0ull)
				{
					ImGui::SameLine(max_label_width + 10);
					ImGui::SetNextItemWidth(width_node);
					if (ImGui::ColorEdit3("##color_sheen", (float*)&nodeU.m_color_sheen_))
					{
						needUpdate = true;
					}
				}
				ImNodes::EndInputAttribute();
			}

			{
				ImNodes::BeginInputAttribute(node.pbr_material.input_weight_sheen);
				const float label_width = ImGui::CalcTextSize("Sheen weight").x;
				ImGui::Text("Sheen weight");

				if (m_graph_.num_edges_from_node(node.pbr_material.input_weight_sheen) == 0ull)
				{
					ImGui::SameLine(max_label_width + 10);
					ImGui::SetNextItemWidth(width_node);
					if (ImGui::DragFloat("##weight_sheen", (float*)&nodeU.m_weight_sheen_, speed_value, 0.f, 1.f))
					{
						needUpdate = true;
					}
				}
				ImNodes::EndInputAttribute();
			}

			{
				ImNodes::BeginInputAttribute(node.pbr_material.input_tint_sheen);
				const float label_width = ImGui::CalcTextSize("Sheen Tint").x;
				ImGui::Text("Sheen Tint");

				if (m_graph_.num_edges_from_node(node.pbr_material.input_tint_sheen) == 0ull)
				{
					ImGui::SameLine(max_label_width + 10);
					ImGui::SetNextItemWidth(width_node);
					if (ImGui::DragFloat("##tint_sheen", (float*)&nodeU.m_tint_sheen_, speed_value, 0.f, 1.f))
					{
						needUpdate = true;
					}
				}
				ImNodes::EndInputAttribute();
			}

			// Clearcoat Section ---------------------------------------------------------------------------------------- Clearcoat Section

			{
				ImNodes::BeginInputAttribute(node.pbr_material.input_color_clearcoat);
				const float label_width = ImGui::CalcTextSize("Clearcoat color").x;
				ImGui::Text("Clearcoat color");

				if (m_graph_.num_edges_from_node(node.pbr_material.input_color_clearcoat) == 0ull)
				{
					ImGui::SameLine(max_label_width + 10);
					ImGui::SetNextItemWidth(width_node);
					if (ImGui::ColorEdit4("##color_clearcoat", (float*)&nodeU.m_color_clearcoat_))
					{
						needUpdate = true;
					}
				}
				ImNodes::EndInputAttribute();
			}

			{
				ImNodes::BeginInputAttribute(node.pbr_material.input_weight_clearcoat);
				const float label_width = ImGui::CalcTextSize("Clearcoat weight").x;
				ImGui::Text("Clearcoat weight");

				if (m_graph_.num_edges_from_node(node.pbr_material.input_weight_clearcoat) == 0ull)
				{
					ImGui::SameLine(max_label_width + 10);
					ImGui::SetNextItemWidth(width_node);
					if (ImGui::DragFloat("##weight_clearcoat", (float*)&nodeU.m_weight_clearcoat_, speed_value, 0.f, 1.f))
					{
						needUpdate = true;
					}




				}
				ImNodes::EndInputAttribute();
			}

			{
				ImNodes::BeginInputAttribute(node.pbr_material.input_roughness_clearcoat);
				const float label_width = ImGui::CalcTextSize("Roughness Clearcoat").x;
				ImGui::Text("Roughness Clearcoat");

				if (m_graph_.num_edges_from_node(node.pbr_material.input_roughness_clearcoat) == 0ull)
				{
					ImGui::SameLine(max_label_width + 10);
					ImGui::SetNextItemWidth(width_node);
					if (ImGui::DragFloat("##roughness_clearcoat", (float*)&nodeU.m_roughness_clearcoat_, speed_value, 0.f, 1.f))
					{
						needUpdate = true;
					}
				}
				ImNodes::EndInputAttribute();
			}

			{
				ImNodes::BeginInputAttribute(node.pbr_material.input_ior_clearcoat);
				const float label_width = ImGui::CalcTextSize("IOR Clearcoat").x;
				ImGui::Text("IOR Clearcoat");

				if (m_graph_.num_edges_from_node(node.pbr_material.input_ior_clearcoat) == 0ull)
				{
					ImGui::SameLine(max_label_width + 10);
					ImGui::SetNextItemWidth(width_node);
					if (ImGui::DragFloat("##ior_clearcoat", (float*)&nodeU.m_ior_clearcoat_, speed_value, 1.f, 2.f))
					{
						needUpdate = true;
					}
				}
				ImNodes::EndInputAttribute();
			}

			// Transmission Section ---------------------------------------------------------------------------------------- Transmission Section

			{
				ImNodes::BeginInputAttribute(node.pbr_material.input_color_transmission);
				const float label_width = ImGui::CalcTextSize("Transmission color").x;
				ImGui::Text("Transmission color");

				if (m_graph_.num_edges_from_node(node.pbr_material.input_color_transmission) == 0ull)
				{
					ImGui::SameLine(max_label_width + 10);
					ImGui::SetNextItemWidth(width_node);
					if (ImGui::ColorEdit4("##color_transmission", (float*)&nodeU.m_color_transmission_))
					{
						needUpdate = true;
					}
				}
				ImNodes::EndInputAttribute();
			}

			{
				ImNodes::BeginInputAttribute(node.pbr_material.input_weight_transmission);
				const float label_width = ImGui::CalcTextSize("Transmission weight").x;
				ImGui::Text("Transmission weight");

				if (m_graph_.num_edges_from_node(node.pbr_material.input_weight_transmission) == 0ull)
				{
					ImGui::SameLine(max_label_width + 10);
					ImGui::SetNextItemWidth(width_node);
					if (ImGui::DragFloat("##weight_transmission", (float*)&nodeU.m_weight_transmission_, speed_value, 0.f, 0.f))
					{
						needUpdate = true;
					}
				}
				ImNodes::EndInputAttribute();
			}

			// Normal, Bump, Disp Section ------------------------------------------------------------------------------------- Normal, Bump, Disp Section

			{
				ImNodes::BeginInputAttribute(node.pbr_material.input_normal);
				const float label_width = ImGui::CalcTextSize("Normal").x;
				ImGui::Text("Normal");

				if (m_graph_.num_edges_from_node(node.pbr_material.input_normal) == 0ull)
				{
					ImGui::SameLine(max_label_width + 10);
					ImGui::SetNextItemWidth(width_node);
					if (ImGui::Combo("##normal", (int*)&nodeU.m_normal_, "Normal\0Bump\0Displacement\0\0"))
					{
						needUpdate = true;
					}
				}
				ImNodes::EndInputAttribute();
			}

			{
				ImNodes::BeginInputAttribute(node.pbr_material.input_bump);
				const float label_width = ImGui::CalcTextSize("Bump").x;
				ImGui::Text("Bump");

				if (m_graph_.num_edges_from_node(node.pbr_material.input_bump) == 0ull)
				{
					ImGui::SameLine(max_label_width + 10);
					ImGui::SetNextItemWidth(width_node);
					if (ImGui::Combo("##bump", (int*)&nodeU.m_bump_, "Normal\0Bump\0Displacement\0\0"))
					{
						needUpdate = true;
					}
				}
				ImNodes::EndInputAttribute();
			}

			{
				ImNodes::BeginInputAttribute(node.pbr_material.input_displacement);
				const float label_width = ImGui::CalcTextSize("Displacement").x;
				ImGui::Text("Displacement");

				if (m_graph_.num_edges_from_node(node.pbr_material.input_displacement) == 0ull)
				{
					ImGui::SameLine(max_label_width + 10);
					ImGui::SetNextItemWidth(width_node);
					if (ImGui::Combo("##disp", (int*)&nodeU.m_displacement_, "Normal\0Bump\0Displacement\0\0"))
					{
						needUpdate = true;
					}
				}
				ImNodes::EndInputAttribute();
			}

			// Emissive Section ---------------------------------------------------------------------------------------- Emissive Section

			{
				ImNodes::BeginInputAttribute(node.pbr_material.input_displacement);
				const float label_width = ImGui::CalcTextSize("Emissive color").x;
				ImGui::Text("Emissive color");

				if (m_graph_.num_edges_from_node(node.pbr_material.input_displacement) == 0ull)
				{
					ImGui::SameLine(max_label_width + 10);
					ImGui::SetNextItemWidth(width_node);
					if (ImGui::ColorEdit4("##color_emissive", (float*)&nodeU.m_emission_color_))
					{
						needUpdate = true;
					}
				}
				ImNodes::EndInputAttribute();
			}

			{
				ImNodes::BeginInputAttribute(node.pbr_material.input_emission_weight);
				const float label_width = ImGui::CalcTextSize("Emissive weight").x;
				ImGui::Text("Emissive weight");

				if (m_graph_.num_edges_from_node(node.pbr_material.input_emission_weight) == 0ull)
				{
					ImGui::SameLine(max_label_width + 10);
					ImGui::SetNextItemWidth(width_node);
					if (ImGui::DragFloat("##weight_emissive", (float*)&nodeU.m_emission_weight_, speed_value, 0.f, 2.f))
					{
						needUpdate = true;
					}
				}
				ImNodes::EndInputAttribute();
			}

			// Opacity Section ---------------------------------------------------------------------------------------- Opacity Section

			{
				ImNodes::BeginInputAttribute(node.pbr_material.input_opacity);
				const float label_width = ImGui::CalcTextSize("Opacity").x;
				ImGui::Text("Opacity");

				if (m_graph_.num_edges_from_node(node.pbr_material.input_opacity) == 0ull)
				{
					ImGui::SameLine(max_label_width + 10);
					ImGui::SetNextItemWidth(width_node);
					if (ImGui::DragFloat("##opacity", (float*)&nodeU.m_opacity_, speed_value, 0.f, 1.f))
					{
						needUpdate = true;
					}
				}
				ImNodes::EndInputAttribute();
			}

			if (needUpdate)
			{

				need_reevaluate = true;
				change_detected = true;
				needUpdate = false;
				root_node_changed();
				update_material();
			}

			ImNodes::EndNode();
		}
		break;

		case HorusUiNodeType::Image_texture: // Image texture node for development purposes, not a final node
		{


			const float node_width = 100.f;

			ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(173, 216, 230, 255));
			ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(135, 206, 250, 255));
			ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(100, 149, 237, 255));
			ImNodes::BeginNode(node.id_);
			ImNodes::BeginNodeTitleBar();
			ImGui::TextColored(ImVec4(0.0f, 0.0f, 0.0f, 1.0f), "Image file");
			ImNodes::EndNodeTitleBar();

			{
				ImNodes::BeginOutputAttribute(node.id_);
				const float label_width = ImGui::CalcTextSize("Result").x;
				ImGui::Indent(node_width - label_width);
				ImGui::Text("Result");
				ImNodes::EndOutputAttribute();
			}

			HorusNode& nodeU = m_graph_.node(node.id_);

			ImGui::TextUnformatted("Path: ");
			ImGui::InputText("##path", nodeU.path, sizeof(nodeU.path));
			ImGui::SameLine();

			if (ImGui::Button("Browse##path"))
			{
				nodeU.local_file_dialog_open = true;
			}


			if (nodeU.local_file_dialog_open)
			{
				char buffer[1024];

				// Show a file dialog to select a file
				FileDialog::ShowFileDialog(&nodeU.local_file_dialog_open, buffer, sizeof(buffer), FileDialog::FileDialogType::OpenFile, true);

				// If a file has been selected
				if (strlen(buffer) < sizeof(nodeU.path))
				{
					strncpy_s(nodeU.path, buffer, sizeof(nodeU.path));
					nodeU.local_file_dialog_open = false; // Reset if file has been selected
				}
				else
				{
					nodeU.path[sizeof(nodeU.path) - 1] = '\0';
				}



			}





			static bool needUpdate = false;


			if (strlen(nodeU.path) > 0)
			{
				if (nodeU.last_used_path != std::string(nodeU.path))
				{
					nodeU.last_used_path = _strdup(nodeU.path);

					needUpdate = true;
				}
				nodeU.m_image_ = nodeU.last_used_path;
			}








			if (needUpdate)
			{

				need_reevaluate = true;
				change_detected = true;
				needUpdate = false;
				root_node_changed();
				update_material();
			}


			ImNodes::EndNode();

		}
		break;


		case HorusUiNodeType::Debug_color01: // Debug Color Node for development purposes, not a final node
		{
			const float node_width = 100.f;

			ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(173, 216, 230, 255));
			ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(135, 206, 250, 255));
			ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(100, 149, 237, 255));
			ImNodes::BeginNode(node.id_);
			ImNodes::BeginNodeTitleBar();
			ImGui::TextColored(ImVec4(0.0f, 0.0f, 0.0f, 1.0f), "Debug Color 01");
			ImNodes::EndNodeTitleBar();

			{
				ImNodes::BeginOutputAttribute(node.id_);
				const float label_width = ImGui::CalcTextSize("Result").x;
				ImGui::Indent(node_width - label_width);
				ImGui::Text("Result");
				ImNodes::EndOutputAttribute();
			}

			const HorusNode& nodeU = m_graph_.node(node.id_);

			ImGui::SetNextItemWidth(300);

			if (ImGui::ColorEdit4("Debug Color", (float*)&nodeU.m_color_))
			{

				need_reevaluate = true;
				change_detected = true;
				root_node_changed();
				update_material();
			}


			ImNodes::EndNode();

		}
		break;

		case HorusUiNodeType::Output: // Output node for development purposes, not a final node
		{
			const float node_width = 100.f;

			ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(204, 229, 255, 255)); 
			ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(179, 217, 255, 255)); 
			ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(153, 204, 255, 255)); 

			ImNodes::BeginNode(node.id_);

			ImNodes::BeginNodeTitleBar();
			ImGui::TextColored(ImVec4(0.0f, 0.0f, 0.0f, 1.0f), "Output");
			ImNodes::EndNodeTitleBar();

			ImGui::Dummy(ImVec2(node_width, 0.f));
			{
				ImNodes::BeginInputAttribute(node.output.output);
				const float label_width = ImGui::CalcTextSize("Output").x;
				ImGui::Text("Output");

				ImNodes::EndInputAttribute();
			}

			ImNodes::EndNode();
			ImNodes::PopColorStyle();
		}
		break;

		}
	}

	for (const auto& edge : m_graph_.edges())
	{
		if (m_graph_.node(edge.from).m_type_ != HorusNodeType::Value)
			continue;

		ImNodes::Link(edge.id, edge.from, edge.to);
	}

	ImNodes::MiniMap(0.2f, m_mini_map_location_);
	ImNodes::EndNodeEditor();


	// Create and destroy nodes / Links here.

	{ // Handle creation of new links
		int start_attr, end_attr;
		if (ImNodes::IsLinkCreated(&start_attr, &end_attr)) // check if a link is being created
		{
			HorusNodeType start_attr_type = m_graph_.node(start_attr).m_type_; // get the type of the starting attribute
			HorusNodeType end_attr_type = m_graph_.node(end_attr).m_type_; // get the type of the ending attribute



			const bool valid_link = start_attr_type != end_attr_type;
			if (valid_link)
			{


				if (start_attr_type != HorusNodeType::Value)
				{
					std::swap(start_attr, end_attr);
				}

				m_graph_.insert_edge(start_attr, end_attr);
				need_reevaluate = true;
				change_detected = true;
				update_material();
			}

		}
	}

	{ // Handle destruction of links
		int link_id;
		if (ImNodes::IsLinkDestroyed(&link_id))
		{
			m_graph_.erase_edge(link_id);
			change_detected = true;
			update_material();
		}
	}

	{ // Handle deletion of nodes
		const int num_selected = ImNodes::NumSelectedLinks();
		if (num_selected > 0 && ImGui::IsKeyPressed(ImGuiKey_MouseMiddle))
		{
			static std::vector<int> selected_links;
			selected_links.resize(static_cast<size_t>(num_selected));
			ImNodes::GetSelectedLinks(selected_links.data());
			for (const int edge_id : selected_links)
			{
				m_graph_.erase_edge(edge_id);

			}
			change_detected = true;
			update_material();
		}
	}

	{ // Handle deletion of nodes
		const int num_selected_nodes = ImNodes::NumSelectedNodes();
		if (num_selected_nodes > 0 && ImGui::IsKeyPressedMap(ImGuiKey_K))
		{
			static std::vector<int> selected_nodes;
			selected_nodes.resize(static_cast<size_t>(num_selected_nodes));
			ImNodes::GetSelectedNodes(selected_nodes.data());
			for (const int node_id : selected_nodes)
			{
				m_graph_.erase_node(node_id);
				auto iter = std::find_if(
					m_nodes_.begin(), m_nodes_.end(), [node_id](const HorusUiNode& node)->bool {
						return node.id_ == node_id;
					});

				switch (iter->type_)
				{
				case HorusUiNodeType::Horus_PBR_Material:
				{
					// base color
					m_graph_.erase_node(iter->pbr_material.input_color_diffuse);
					m_graph_.erase_node(iter->pbr_material.input_weight_diffuse);
					m_graph_.erase_node(iter->pbr_material.input_roughness_diffuse);
					m_graph_.erase_node(iter->pbr_material.input_backscattering_color);
					m_graph_.erase_node(iter->pbr_material.input_backscattering_weight);

					// reflection
					m_graph_.erase_node(iter->pbr_material.input_color_reflection);
					m_graph_.erase_node(iter->pbr_material.input_roughness_reflection);
					m_graph_.erase_node(iter->pbr_material.input_weight_reflection);
					m_graph_.erase_node(iter->pbr_material.input_ior_reflection);
					m_graph_.erase_node(iter->pbr_material.input_BRDF_reflection);
					m_graph_.erase_node(iter->pbr_material.input_anisotropy_reflection);
					m_graph_.erase_node(iter->pbr_material.input_rotation_reflection);
					m_graph_.erase_node(iter->pbr_material.input_mode_reflection);

					// refraction
					m_graph_.erase_node(iter->pbr_material.input_color_transmission);
					m_graph_.erase_node(iter->pbr_material.input_ior_transmission);
					m_graph_.erase_node(iter->pbr_material.input_roughness_transmission);
					m_graph_.erase_node(iter->pbr_material.input_weight_transmission);

					// normal
					m_graph_.erase_node(iter->pbr_material.input_normal);
					m_graph_.erase_node(iter->pbr_material.input_bump);
					m_graph_.erase_node(iter->pbr_material.input_displacement);

					// emission
					m_graph_.erase_node(iter->pbr_material.input_emission_color);
					m_graph_.erase_node(iter->pbr_material.input_emission_weight);

					// opacity
					m_graph_.erase_node(iter->pbr_material.input_opacity);
					m_graph_.erase_node(iter->pbr_material.input_opacity_weight);

					// sss
					m_graph_.erase_node(iter->pbr_material.input_color_sss);
					m_graph_.erase_node(iter->pbr_material.input_weight_sss);
					m_graph_.erase_node(iter->pbr_material.input_radius_sss);
					m_graph_.erase_node(iter->pbr_material.input_weight_sss);

					// Sheen
					m_graph_.erase_node(iter->pbr_material.input_color_sheen);
					m_graph_.erase_node(iter->pbr_material.input_weight_sheen);
					m_graph_.erase_node(iter->pbr_material.input_tint_sheen);

					// Clearcoat
					m_graph_.erase_node(iter->pbr_material.input_color_clearcoat);
					m_graph_.erase_node(iter->pbr_material.input_weight_clearcoat);
					m_graph_.erase_node(iter->pbr_material.input_roughness_clearcoat);
					m_graph_.erase_node(iter->pbr_material.input_ior_clearcoat);



					break;
				}



				case HorusUiNodeType::Add:
				{
					m_graph_.erase_node(iter->add.input0);
					m_graph_.erase_node(iter->add.input1);
					break;
				}




				case HorusUiNodeType::Constant_texture:
				{
					m_graph_.erase_node(iter->constant_texture.input0);
					break;
				}

				case HorusUiNodeType::Checker_texture:
				{
					m_graph_.erase_node(iter->checker_texture.input0);
					break;
				}





				case HorusUiNodeType::Debug_color01:
				{
					m_graph_.erase_node(iter->Debug_color01.input);
					break;
				}



				case HorusUiNodeType::Output:
				{
					m_graph_.erase_node(iter->output.output);
					m_root_node_ID_ = -1;
					break;
				}
				default:
					break;
				}
				m_nodes_.erase(iter);
			}

			change_detected = true;
			update_material();
		}
	}

	ImGui::End();
}

void HorusMaterialEditor::update_material()
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusEngine& Engine = HorusEngine::GetInstance();

	root_node_changed();

	if (m_root_node_ID_ != -1)
	{
		if (m_IsOutputConnected_)
		{
			if (!material_already_set)
			{
				// TODO : Check here if the material is already set or if it's correctly set after a change

				ObjectManager.SetMaterialFromEditorNode(ObjectManager.GetMeshIdToSetMaterial(), m_out_modified_);
				material_already_set = true;
				change_detected = true;
			}

			output_connected_last_frame = true;
		}
		else
		{
			if (output_connected_last_frame)
			{
				ObjectManager.SetMaterialFromEditorNode(ObjectManager.GetMeshIdToSetMaterial(), m_out_node_);
				material_already_set = false;
				change_detected = true;
			}

			output_connected_last_frame = false;
		}
	}
	else  // m_root_node_ID_ is -1
	{
		output_connected_last_frame = false;
	}

	if (change_detected)
	{
		if (need_reevaluate)
		{
			if (m_root_node_ID_ != -1)
			{
				// get nodes from graph
							//const HorusNode& node = m_graph_.node(); // get good id
							//std::cout << "Node color is : " << node.m_color_.x << " " << node.m_color_.y << " " << node.m_color_.z << " " << node.m_color_.w << std::endl;

							// print root id

				m_IsThisMaterial_ = true;

				m_out_modified_ = evaluate_rpr_material_node(m_graph_, m_root_node_ID_);

				ObjectManager.SetMaterialFromEditorNode(ObjectManager.GetMeshIdToSetMaterial(), m_out_modified_); // Set final material 
				spdlog::info("id to set is {}", ObjectManager.GetMeshIdToSetMaterial());

				need_reevaluate = false;

			}
			else
			{
				m_IsThisMaterial_ = false;

				ObjectManager.SetMaterialFromEditorNode(ObjectManager.GetMeshIdToSetMaterial(), m_out_node_);

				spdlog::info("id to set is {}", ObjectManager.GetMeshIdToSetMaterial());

				need_reevaluate = false;
			}
		}

		Engine.CallResetBuffer();
		change_detected = false;
	}
}

bool HorusMaterialEditor::root_node_changed()
{
	if (m_root_node_ID_ != -1)
	{
		if (m_graph_.num_edges_from_node(m_root_node_ID_) > 0ull)
		{
			m_IsOutputConnected_ = true;
		}
	}

	return m_IsOutputConnected_;
}

void HorusMaterialEditor::quit()
{
	m_garbage_collector_.GCClean();
}

void HorusMaterialEditorBrowser::init()
{
}

void HorusMaterialEditorBrowser::update()
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusEngine& Engine = HorusEngine::GetInstance();

	ImGui::Begin("Material Browser");

	std::vector<std::string> materials;

	/*if (!materials.empty()) {
		selectedMaterial = materials[0];
	}*/

	ObjectManager.GetMaterialEditorMaterials(materials);



	if (ImGui::TreeNode("Materials"))
	{
		for (const auto& material : materials)
		{
			bool isSelected = (material == selectedMaterial);
			if (ImGui::Selectable(material.c_str(), isSelected))
			{
				selectedMaterial = material;

				int id = ObjectManager.GetMaterialEditorIdByName(material.c_str());

				ObjectManager.SetMaterialEditorMeshToSetMaterial(id);
				ObjectManager.SetMaterialEditorToShow(id);

			}
		}
		ImGui::TreePop();
	}

	ImGui::End();
}

void HorusMaterialEditorBrowser::show_material_browser()
{
}

void HorusMaterialEditorBrowser::hide_material_browser()
{
}

void HorusMaterialEditorBrowser::set_material_list()
{
}

void HorusMaterialEditorBrowser::quit()
{
}
