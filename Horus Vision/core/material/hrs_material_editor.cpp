#include "hrs_material_editor.h"

#include <iostream>

#include "hrs_engine.h"
#include "spdlog/spdlog.h"

#include "hrs_radeon.h"
#include "imgui_internal.h"
#include "imnodes.h"
#include "L2DFileDialog.h"
#include "objects/hrs_object_manager.h"

HorusRadeon& radeon_matedit = HorusRadeon::get_instance();
HorusObjectManager& g_object_impedit = HorusObjectManager::get_instance();
HorusEngine& g_engine_impediet = HorusEngine::get_instance();


template<class T>
T clamp(T x, T a, T b)
{
	return std::min(b, std::max(x, a));
}

static float current_time_seconds = 0.f;
static bool  emulate_three_button_mouse = false;

rpr_image load_texture(std::string path)
{
	rpr_image image = nullptr;

	if (rprContextCreateImageFromFile(radeon_matedit.get_context(), path.c_str(), &image) != RPR_SUCCESS)
	{
		spdlog::error("Error: Texture -> {} not found.", path);
		return nullptr;
	}

	radeon_matedit.get_gc().GCAdd(image);

	spdlog::info("Texture -> {} loaded.", path);

	return image;
}

rpr_material_node HorusMaterialEditor::evaluate_rpr_material_node(const HorusGraph<HorusNode>& graph, const int root_node_id)
{
	m_garbage_collector_.GCClean();

	std::stack<int> post_order_stack;

	dfs_traverse(
		graph, root_node_id, [&post_order_stack](const int node_id) -> void { post_order_stack.push(node_id); });

	std::stack<rpr_material_node> material_node_stack;
	std::unordered_map<int, rpr_material_node> node_cache;

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

			rpr_material_node new_node = material_node_stack.top();
			material_node_stack.pop();

			rprMaterialSystemCreateNode(radeon_matedit.get_matsys(), RPR_MATERIAL_NODE_UBERV2, &new_node);
			m_garbage_collector_.GCAdd(new_node);



			/*if (node.m_use_diffuse_texture_)
			{
				CHECK(rprMaterialNodeSetInputNByKey(new_node, RPR_MATERIAL_INPUT_COLOR, material_node_stack.top()));

				spdlog::info("Diffuse texture used.");

			}
			else
			{
				CHECK(rprMaterialNodeSetInputFByKey(new_node, RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR, node.m_color_diffuse_.x, node.m_color_diffuse_.y, node.m_color_diffuse_.z, node.m_color_diffuse_.w));
				spdlog::info("Diffuse texture not used.");
			}*/



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














			material_node_stack.push(new_node);

		}
		break;




		case HorusNodeType::Add:
		{
			const HorusNode& node = graph.node(current_node_id);

			rpr_material_node operand1 = material_node_stack.top(); material_node_stack.pop();
			rpr_material_node operand2 = material_node_stack.top(); material_node_stack.pop();

			rpr_material_node new_node = nullptr;


			auto it = node_cache.find(current_node_id);
			if (it != node_cache.end())
			{
				new_node = it->second;

			}
			else
			{

				rprMaterialSystemCreateNode(radeon_matedit.get_matsys(), RPR_MATERIAL_NODE_ADD, &new_node);
				m_garbage_collector_.GCAdd(new_node);

				node_cache[current_node_id] = new_node;
			}

			rprMaterialNodeSetInputNByKey(new_node, RPR_MATERIAL_INPUT_COLOR0, operand1);
			rprMaterialNodeSetInputNByKey(new_node, RPR_MATERIAL_INPUT_COLOR1, operand2);

			material_node_stack.push(new_node);
		}
		break;

		case HorusNodeType::Blend:
		{

			const HorusNode& node = graph.node(current_node_id);

			rpr_material_node baseLayer = material_node_stack.top(); material_node_stack.pop();
			rpr_material_node topLayer = material_node_stack.top(); material_node_stack.pop();
			rpr_material_node weight = material_node_stack.top(); material_node_stack.pop();

			rpr_material_node new_node = nullptr;

			auto it = node_cache.find(current_node_id);
			if (it != node_cache.end())
			{

				new_node = it->second;

			}
			else
			{

				rprMaterialSystemCreateNode(radeon_matedit.get_matsys(), RPR_MATERIAL_NODE_BLEND, &new_node);
				m_garbage_collector_.GCAdd(new_node);

				node_cache[current_node_id] = new_node;
			}

			rprMaterialNodeSetInputNByKey(new_node, RPR_MATERIAL_INPUT_COLOR0, baseLayer);
			rprMaterialNodeSetInputNByKey(new_node, RPR_MATERIAL_INPUT_COLOR1, topLayer);
			rprMaterialNodeSetInputNByKey(new_node, RPR_MATERIAL_INPUT_WEIGHT, weight);


			material_node_stack.push(new_node);
		}
		break;

		case HorusNodeType::Arithmetic:
		{
			const HorusNode& node = graph.node(current_node_id); // Get current node information

			rpr_material_node new_node = nullptr;

			// Pop the operands off the stack. Assume that they are already there.
			rpr_material_node operand1 = material_node_stack.top();
			material_node_stack.pop();
			m_garbage_collector_.GCAdd(operand1);
			rpr_material_node operand2 = material_node_stack.top();
			material_node_stack.pop();
			m_garbage_collector_.GCAdd(operand2);

			switch (node.m_arithmetic_op_) {
			case HorusArithmeticOperation::ADD:

			{
				rprMaterialSystemCreateNode(radeon_matedit.get_matsys(), RPR_MATERIAL_NODE_ARITHMETIC, &new_node);
				rprMaterialNodeSetInputNByKey(new_node, RPR_MATERIAL_INPUT_COLOR0, operand1);
				rprMaterialNodeSetInputNByKey(new_node, RPR_MATERIAL_INPUT_COLOR1, operand2);
				rprMaterialNodeSetInputUByKey(new_node, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_ADD);
			}
			break;
			case HorusArithmeticOperation::SUB:
			{
				rprMaterialSystemCreateNode(radeon_matedit.get_matsys(), RPR_MATERIAL_NODE_ARITHMETIC, &new_node);
				rprMaterialNodeSetInputNByKey(new_node, RPR_MATERIAL_INPUT_COLOR0, operand1);
				rprMaterialNodeSetInputNByKey(new_node, RPR_MATERIAL_INPUT_COLOR1, operand2);
				rprMaterialNodeSetInputUByKey(new_node, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_SUB);
			}

			break;
			case HorusArithmeticOperation::MUL:
			{
				rprMaterialSystemCreateNode(radeon_matedit.get_matsys(), RPR_MATERIAL_NODE_ARITHMETIC, &new_node);
				rprMaterialNodeSetInputNByKey(new_node, RPR_MATERIAL_INPUT_COLOR0, operand1);
				rprMaterialNodeSetInputNByKey(new_node, RPR_MATERIAL_INPUT_COLOR1, operand2);
				rprMaterialNodeSetInputUByKey(new_node, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_MUL);
			}
			break;
			case HorusArithmeticOperation::DIV:
			{
				rprMaterialSystemCreateNode(radeon_matedit.get_matsys(), RPR_MATERIAL_NODE_ARITHMETIC, &new_node);
				rprMaterialNodeSetInputNByKey(new_node, RPR_MATERIAL_INPUT_COLOR0, operand1);
				rprMaterialNodeSetInputNByKey(new_node, RPR_MATERIAL_INPUT_COLOR1, operand2);
				rprMaterialNodeSetInputUByKey(new_node, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_DIV);
			}
			break;
			case HorusArithmeticOperation::SIN:
			{
				rprMaterialSystemCreateNode(radeon_matedit.get_matsys(), RPR_MATERIAL_NODE_ARITHMETIC, &new_node);
				rprMaterialNodeSetInputNByKey(new_node, RPR_MATERIAL_INPUT_COLOR0, operand1);
				rprMaterialNodeSetInputNByKey(new_node, RPR_MATERIAL_INPUT_COLOR1, operand2);
				rprMaterialNodeSetInputUByKey(new_node, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_SIN);
			}
			break;
			case HorusArithmeticOperation::COS:
			{
				rprMaterialSystemCreateNode(radeon_matedit.get_matsys(), RPR_MATERIAL_NODE_ARITHMETIC, &new_node);
				rprMaterialNodeSetInputNByKey(new_node, RPR_MATERIAL_INPUT_COLOR0, operand1);
				rprMaterialNodeSetInputNByKey(new_node, RPR_MATERIAL_INPUT_COLOR1, operand2);
				rprMaterialNodeSetInputUByKey(new_node, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_COS);
			}
			break;
			case HorusArithmeticOperation::TAN:
			{
				rprMaterialSystemCreateNode(radeon_matedit.get_matsys(), RPR_MATERIAL_NODE_ARITHMETIC, &new_node);
				rprMaterialNodeSetInputNByKey(new_node, RPR_MATERIAL_INPUT_COLOR0, operand1);
				rprMaterialNodeSetInputNByKey(new_node, RPR_MATERIAL_INPUT_COLOR1, operand2);
				rprMaterialNodeSetInputUByKey(new_node, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_TAN);
			}
			break;
			case HorusArithmeticOperation::SELECT_X:
			{
				rprMaterialSystemCreateNode(radeon_matedit.get_matsys(), RPR_MATERIAL_NODE_ARITHMETIC, &new_node);
				rprMaterialNodeSetInputNByKey(new_node, RPR_MATERIAL_INPUT_COLOR0, operand1);
				rprMaterialNodeSetInputNByKey(new_node, RPR_MATERIAL_INPUT_COLOR1, operand2);
				rprMaterialNodeSetInputUByKey(new_node, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_SELECT_X);
			}
			break;
			case HorusArithmeticOperation::SELECT_Y:
			{
				rprMaterialSystemCreateNode(radeon_matedit.get_matsys(), RPR_MATERIAL_NODE_ARITHMETIC, &new_node);

				rprMaterialNodeSetInputNByKey(new_node, RPR_MATERIAL_INPUT_COLOR0, operand1);
				rprMaterialNodeSetInputNByKey(new_node, RPR_MATERIAL_INPUT_COLOR1, operand2);

				rprMaterialNodeSetInputUByKey(new_node, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_SELECT_Y);
			}
			break;
			case HorusArithmeticOperation::SELECT_Z:
			{
				rprMaterialSystemCreateNode(radeon_matedit.get_matsys(), RPR_MATERIAL_NODE_ARITHMETIC, &new_node);

				rprMaterialNodeSetInputNByKey(new_node, RPR_MATERIAL_INPUT_COLOR0, operand1);
				rprMaterialNodeSetInputNByKey(new_node, RPR_MATERIAL_INPUT_COLOR1, operand2);

				rprMaterialNodeSetInputUByKey(new_node, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_SELECT_Z);
			}
			case HorusArithmeticOperation::COMBINE:
			{
				rprMaterialSystemCreateNode(radeon_matedit.get_matsys(), RPR_MATERIAL_NODE_ARITHMETIC, &new_node);

				rprMaterialNodeSetInputNByKey(new_node, RPR_MATERIAL_INPUT_COLOR0, operand1);
				rprMaterialNodeSetInputNByKey(new_node, RPR_MATERIAL_INPUT_COLOR1, operand2);

				rprMaterialNodeSetInputUByKey(new_node, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_COMBINE);
			}






			// ... autres cas ...
			default:
				// Gérer les cas non pris en charge ou les erreurs
				break;
			}

			// Commun à tous les types de nœuds arithmétiques
			if (new_node != nullptr) {
				// Ajouter le nouveau nœud à la pile
				material_node_stack.push(new_node);

				// Ajouter le nouveau nœud au garbage collector
				m_garbage_collector_.GCAdd(new_node);
			}
		}
		break;

		case HorusNodeType::Checker_texture:
		{
			// Supprimer cette ligne si vous allez utiliser node.uv_value_ pour les UV
			// rpr_material_node uv = material_node_stack.top();
			// material_node_stack.pop();

			const HorusNode& node = graph.node(current_node_id);

			rpr_material_node new_node = nullptr;
			rprMaterialSystemCreateNode(radeon_matedit.get_matsys(), RPR_MATERIAL_NODE_CHECKER_TEXTURE, &new_node);
			m_garbage_collector_.GCAdd(new_node);


			rprMaterialNodeSetInputFByKey(new_node, RPR_MATERIAL_INPUT_UV, node.m_uv_scale_.x, node.m_uv_scale_.y, 0, 1);

			material_node_stack.push(new_node);
		}
		break;

		case HorusNodeType::Image_texture:
		{
			const HorusNode& node = graph.node(current_node_id);

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

				rpr_material_node new_node = material_node_stack.top();
				material_node_stack.pop();

				CHECK(rprMaterialSystemCreateNode(radeon_matedit.get_matsys(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &new_node));
				m_garbage_collector_.GCAdd(new_node);

				CHECK(rprMaterialNodeSetInputImageDataByKey(new_node, RPR_MATERIAL_INPUT_DATA, image));

				material_node_stack.push(new_node);
			}

				
		}
		break;

		case HorusNodeType::Debug_color01:
		{

			// ID ok ici -> bonne methode pour le get
			const HorusNode& node = graph.node(current_node_id);

			rpr_material_node new_node = material_node_stack.top();
			material_node_stack.pop();


			rprMaterialSystemCreateNode(radeon_matedit.get_matsys(), RPR_MATERIAL_NODE_UBERV2, &new_node);
			m_garbage_collector_.GCAdd(new_node);

			rprMaterialNodeSetInputFByKey(new_node, RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR, node.m_color_.x, node.m_color_.y, node.m_color_.z, node.m_color_.w);

			material_node_stack.push(new_node);
		}
		break;

		case HorusNodeType::Value:
		{
			if (graph.num_edges_from_node(current_node_id) == 0ull)
			{
				material_node_stack.push(current_type.m_value_);
			}
		}
		break;
		default:
			break;
		}
	}


	if (!material_node_stack.empty())
	{
		m_out_modified_ = material_node_stack.top();

		//spdlog::info("m_out_modified_: {}", (void*)m_out_modified_);  // Pour des pointeurs
		return m_out_modified_;
	}

}

void HorusMaterialEditor::init()
{
	RPRGarbageCollector m_gco = radeon_matedit.get_gc();

	CHECK(rprMaterialSystemCreateNode(radeon_matedit.get_matsys(), RPR_MATERIAL_NODE_UBERV2, &m_out_node_));
	m_gco.GCAdd(m_out_node_);

	CHECK(rprMaterialNodeSetInputFByKey(m_out_node_, RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR, 1.f, 0.0f, 0.5f, 1.0f));
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




	CHECK(rprMaterialSystemCreateNode(radeon_matedit.get_matsys(), RPR_MATERIAL_NODE_UBERV2, &m_out_modified_));
	m_gco.GCAdd(m_out_modified_);

	CHECK(rprMaterialNodeSetInputFByKey(m_out_modified_, RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR, 0.0f, 0.5f, 1.0f, 1.0f));
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





	//g_object_impedit.set_material_from_editor_node(1, m_out_node_);
}

void HorusMaterialEditor::update()
{
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

					m_nodes_.push_back(ui_node);
					ImNodes::SetNodeScreenSpacePos(ui_node.id_, click_pos_on_canvas);
				}

				if (ImGui::MenuItem("Horus Material"))
				{

				}

				if (ImGui::MenuItem("Horus SSS Material"))
				{

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
					ImNodes::SetNodeScreenSpacePos(ui_node.id_, click_pos_on_canvas);
				}

				if (ImGui::MenuItem("Bump_map"))
				{
					// ... (Votre code pour ajouter un nœud Bump_map ici)
				}

				if (ImGui::MenuItem("Normal_map"))
				{
					// ... (Votre code pour ajouter un nœud Normal_map ici)
				}

				if (ImGui::MenuItem("Constant Color"))
				{
					const HorusNode value(HorusNodeType::Value, 0.0f);
					const HorusNode op(HorusNodeType::Constant_texture);

					HorusUiNode ui_node = {};
					ui_node.type_ = HorusUiNodeType::Constant_texture;
					ui_node.constant_texture.input0 = m_graph_.insert_node(value);
					ui_node.id_ = m_graph_.insert_node(op);

					m_graph_.insert_edge(ui_node.id_, ui_node.constant_texture.input0);
					m_nodes_.push_back(ui_node);

					ImNodes::SetNodeScreenSpacePos(ui_node.id_, click_pos_on_canvas);
				}

				if (ImGui::MenuItem("Noise2D_texture"))
				{
					// ... (Votre code pour ajouter un nœud Noise2D_texture ici)
				}

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

			if (ImGui::BeginMenu("Math"))
			{
				if (ImGui::MenuItem("Add"))
				{
					const HorusNode value(HorusNodeType::Value, 0.0f);
					const HorusNode op(HorusNodeType::Add);

					HorusUiNode ui_node = {};
					ui_node.type_ = HorusUiNodeType::Add;
					ui_node.add.input0 = m_graph_.insert_node(value);
					ui_node.add.input1 = m_graph_.insert_node(value);
					ui_node.id_ = m_graph_.insert_node(op);

					m_graph_.insert_edge(ui_node.id_, ui_node.add.input0);
					m_graph_.insert_edge(ui_node.id_, ui_node.add.input1);

					m_nodes_.push_back(ui_node);
					ImNodes::SetNodeScreenSpacePos(ui_node.id_, click_pos_on_canvas);
				}

				if (ImGui::MenuItem("Arithmetic")) // A voir, je crois qu'il n'est pas censer etre la , mais plutot de servir a creer des operations arithmetique
				{
					const HorusNode value(HorusNodeType::Value, 0.0f);
					const HorusNode op(HorusNodeType::Arithmetic);

					HorusUiNode ui_node = {};
					ui_node.type_ = HorusUiNodeType::Arithmetic;
					ui_node.arithmetic.input0 = m_graph_.insert_node(value);
					ui_node.arithmetic.input1 = m_graph_.insert_node(value);
					ui_node.arithmetic.input2 = m_graph_.insert_node(value);
					ui_node.arithmetic.input3 = m_graph_.insert_node(value);
					ui_node.id_ = m_graph_.insert_node(op);

					m_graph_.insert_edge(ui_node.id_, ui_node.arithmetic.input0);
					m_graph_.insert_edge(ui_node.id_, ui_node.arithmetic.input1);
					m_graph_.insert_edge(ui_node.id_, ui_node.arithmetic.input2);
					m_graph_.insert_edge(ui_node.id_, ui_node.arithmetic.input3);

					m_nodes_.push_back(ui_node);
					ImNodes::SetNodeScreenSpacePos(ui_node.id_, click_pos_on_canvas);
				}

				if (ImGui::MenuItem("Blend"))
				{
					const HorusNode value(HorusNodeType::Value, 0.0f);
					const HorusNode op(HorusNodeType::Blend);

					HorusUiNode ui_node = {};
					ui_node.type_ = HorusUiNodeType::Blend;
					ui_node.blend.input0 = m_graph_.insert_node(value);
					ui_node.blend.input1 = m_graph_.insert_node(value);
					ui_node.blend.input2 = m_graph_.insert_node(value);
					ui_node.id_ = m_graph_.insert_node(op);

					m_graph_.insert_edge(ui_node.id_, ui_node.blend.input0);
					m_graph_.insert_edge(ui_node.id_, ui_node.blend.input1);
					m_graph_.insert_edge(ui_node.id_, ui_node.blend.input2);

					m_nodes_.push_back(ui_node);
					ImNodes::SetNodeScreenSpacePos(ui_node.id_, click_pos_on_canvas);
				}

				if (ImGui::MenuItem("Blend_value"))
				{
					// ... (Votre code pour ajouter un nœud Blend_value ici)
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Utility"))
			{
				if (ImGui::MenuItem("Checker_texture"))
				{
					HorusNode value(HorusNodeType::Value, 0.0f);
					HorusNode op(HorusNodeType::Checker_texture);
					op.m_uv_scale_ = { 1.0f, 1.0f };

					HorusUiNode ui_node = {};
					ui_node.type_ = HorusUiNodeType::Checker_texture;
					ui_node.checker_texture.input0 = m_graph_.insert_node(value);
					ui_node.id_ = m_graph_.insert_node(op);

					m_graph_.insert_edge(ui_node.id_, ui_node.checker_texture.input0);

					m_nodes_.push_back(ui_node);
					ImNodes::SetNodeScreenSpacePos(ui_node.id_, click_pos_on_canvas);
				}

				if (ImGui::MenuItem("Constant_texture"))
				{
					// ... (Votre code pour ajouter un nœud Constant_texture ici)
				}

				if (ImGui::MenuItem("Gradient_texture"))
				{
					// ... (Votre code pour ajouter un nœud Gradient_texture ici)
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Shader"))
			{
				if (ImGui::MenuItem("Ao_map"))
				{
					std::cout << "Ao_map" << std::endl;
				}

				if (ImGui::MenuItem("Diffuse"))
				{
					// ... (Votre code pour ajouter un nœud Diffuse ici)
				}

				if (ImGui::MenuItem("Diffuse_refraction"))
				{
					// ... (Votre code pour ajouter un nœud Diffuse_refraction ici)
				}

				if (ImGui::MenuItem("Dot_texture"))
				{
					// ... (Votre code pour ajouter un nœud Dot_texture ici)
				}

				if (ImGui::MenuItem("Emissive"))
				{
					// ... (Votre code pour ajouter un nœud Emissive ici)
				}

				if (ImGui::MenuItem("Fresnel"))
				{
					// ... (Votre code pour ajouter un nœud Fresnel ici)
				}

				if (ImGui::MenuItem("Fresnel_schlick"))
				{
					// ... (Votre code pour ajouter un nœud Fresnel_schlick ici)
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Vector"))
			{
				if (ImGui::MenuItem("Input_lookup"))
				{
					// ... (Votre code pour ajouter un nœud Input_lookup ici)
				}

				if (ImGui::MenuItem("Microfacet"))
				{
					// ... (Votre code pour ajouter un nœud Microfacet ici)
				}

				if (ImGui::MenuItem("Microfacet_anisotropic_reflection"))
				{
					// ... (Votre code pour ajouter un nœud Microfacet_anisotropic_reflection ici)
				}

				if (ImGui::MenuItem("Microfacet_anisotropic_refraction"))
				{
					// ... (Votre code pour ajouter un nœud Microfacet_anisotropic_refraction ici)
				}

				if (ImGui::MenuItem("Microfacet_beckmann"))
				{
					// ... (Votre code pour ajouter un nœud Microfacet_beckmann ici)
				}

				if (ImGui::MenuItem("Microfacet_refraction"))
				{
					// ... (Votre code pour ajouter un nœud Microfacet_refraction ici)
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Converter"))
			{




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

					ImNodes::SetNodeScreenSpacePos(ui_node.id_, click_pos_on_canvas);
				}

				if (ImGui::MenuItem("Color 02"))
				{
					// ... (Votre code pour ajouter un nœud Debug ici)
				}

				if (ImGui::MenuItem("Color 03"))
				{
					// ... (Votre code pour ajouter un nœud Debug ici)
				}

				if (ImGui::MenuItem("Print"))
				{
					// ... (Votre code pour ajouter un nœud Debug ici)
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
		case HorusUiNodeType::Horus_PBR_Material:
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
					else
					{
						nodeU.m_use_diffuse_texture_ = true;
						spdlog::info("Diffuse texture");
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











		case HorusUiNodeType::Add:
		{
			const float node_width = 100.f;

			ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(173, 216, 230, 255)); // Light blue
			ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(135, 206, 250, 255)); // Light sky blue
			ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(100, 149, 237, 255)); // Cornflower blue
			ImNodes::BeginNode(node.id_);
			ImNodes::BeginNodeTitleBar();
			ImGui::TextColored(ImVec4(0.0f, 0.0f, 0.0f, 1.0f), "Add");
			ImNodes::EndNodeTitleBar();
			{
				ImNodes::BeginInputAttribute(node.add.input0);
				const float label_width = ImGui::CalcTextSize("Value 0").x;
				ImGui::Text("Value 0");

				if (m_graph_.num_edges_from_node(node.add.input0) == 0ull)
				{
					ImGui::SameLine();
					ImGui::PushItemWidth(node_width - label_width);
					//ImGui::DragFloat("##value0", &m_graph_.node(node.ui.add.input0).m_value_, 0.01f);

					ImGui::PopItemWidth();
				}
				ImNodes::EndInputAttribute();
			}
			{
				ImNodes::BeginInputAttribute(node.add.input1);
				const float label_width = ImGui::CalcTextSize("Value 1").x;
				ImGui::Text("Value 1");
				if (m_graph_.num_edges_from_node(node.add.input1) == 0ull)
				{
					ImGui::SameLine();
					ImGui::PushItemWidth(node_width - label_width);
					//ImGui::DragFloat("##value1", &m_graph_.node(node.ui.add.input1).m_value_, 0.01f);
					ImGui::PopItemWidth();
				}
				ImNodes::EndInputAttribute();
			}

			ImGui::Spacing();
			{
				ImNodes::BeginOutputAttribute(node.id_);
				const float label_width = ImGui::CalcTextSize("Result").x;
				ImGui::Indent(node_width - label_width);
				ImGui::Text("Result");
				ImNodes::EndOutputAttribute();
			}

			ImNodes::EndNode();
		}
		break;
		case HorusUiNodeType::Blend:
		{
			const float node_width = 100.f;

			ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(152, 251, 152, 255)); // Pale green
			ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(124, 252, 0, 255)); // Lawn green
			ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(0, 250, 154, 255)); // Medium spring green
			ImNodes::BeginNode(node.id_);
			ImNodes::BeginNodeTitleBar();
			ImGui::TextColored(ImVec4(0.0f, 0.0f, 0.0f, 1.0f), "Blend");
			ImNodes::EndNodeTitleBar();

			HorusNode& nodeU = m_graph_.node(node.id_);

			{
				ImNodes::BeginInputAttribute(node.blend.input0);
				const float label_width = ImGui::CalcTextSize("Input 1").x;
				ImGui::Text("Input 1");

				if (m_graph_.num_edges_from_node(node.blend.input0) == 0ull)
				{
					ImGui::SameLine();
					ImGui::PushItemWidth(node_width - label_width);

					ImGui::PopItemWidth();
				}
				ImNodes::EndInputAttribute();
			}
			{
				ImNodes::BeginInputAttribute(node.blend.input1);
				const float label_width = ImGui::CalcTextSize("Input 2").x;
				ImGui::Text("Input 2");
				if (m_graph_.num_edges_from_node(node.blend.input1) == 0ull)
				{
					ImGui::SameLine();
					ImGui::PushItemWidth(node_width - label_width);
					ImGui::PopItemWidth();
				}
				ImNodes::EndInputAttribute();
			}

			{
				ImNodes::BeginInputAttribute(node.blend.input2);
				const float label_width = ImGui::CalcTextSize("Blend value").x;
				ImGui::Text("Blend value");
				if (m_graph_.num_edges_from_node(node.blend.input2) == 0ull)
				{
					ImGui::SameLine();
					ImGui::PushItemWidth(node_width - label_width);
					if (ImGui::DragFloat("##value1", &nodeU.m_blend_value_, 0.5f))
					{
						need_reevaluate = true;
						change_detected = true;

						root_node_changed();
						update_material();
					}

					ImGui::PopItemWidth();
				}
				ImNodes::EndInputAttribute();
			}

			ImGui::Spacing();
			{
				ImNodes::BeginOutputAttribute(node.id_);
				const float label_width = ImGui::CalcTextSize("Output").x;
				ImGui::Indent(node_width - label_width);
				ImGui::Text("Output");
				ImNodes::EndOutputAttribute();
			}

			ImNodes::EndNode();




		}
		break;




		case HorusUiNodeType::Arithmetic:
		{
			const float node_width = 100.f;

			ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(173, 216, 230, 255));
			ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(135, 206, 250, 255));
			ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(100, 149, 237, 255));
			ImNodes::BeginNode(node.id_);
			ImNodes::BeginNodeTitleBar();
			ImGui::TextColored(ImVec4(0.0f, 0.0f, 0.0f, 1.0f), "Arithmetic");
			ImNodes::EndNodeTitleBar();

			{
				ImNodes::BeginInputAttribute(node.arithmetic.input0);
				const float label_width = ImGui::CalcTextSize("Value 1").x;
				ImGui::Indent(node_width - label_width);
				ImGui::Text("Value 1");
				ImNodes::EndInputAttribute();
			}

			{
				ImNodes::BeginInputAttribute(node.arithmetic.input1);
				const float label_width = ImGui::CalcTextSize("Value 2").x;
				ImGui::Indent(node_width - label_width);
				ImGui::Text("Value 2");
				ImNodes::EndInputAttribute();
			}

			{
				ImNodes::BeginInputAttribute(node.arithmetic.input2);
				const float label_width = ImGui::CalcTextSize("Value 3").x;
				ImGui::Indent(node_width - label_width);
				ImGui::Text("Value 3");
				ImNodes::EndInputAttribute();
			}

			{
				ImNodes::BeginInputAttribute(node.arithmetic.input3);
				const float label_width = ImGui::CalcTextSize("Value 4").x;
				ImGui::Indent(node_width - label_width);
				ImGui::Text("Value 4");
				ImNodes::EndInputAttribute();
			}




			{
				ImNodes::BeginOutputAttribute(node.id_);
				const float label_width = ImGui::CalcTextSize("Result").x;
				ImGui::Indent(node_width - label_width);
				ImGui::Text("Result");
				ImNodes::EndOutputAttribute();
			}

			HorusNode& nodeU = m_graph_.node(node.id_);

			ImGui::SetNextItemWidth(300);


			static const char* items[] = {
													"Add", "Subtract",
													"Multiply","Divide",
													"Sine","Cosine",
													"Tangent","Select X",
													"Select Y","Select Z",
													"Combine","Dot3",
													"Cross3","Length3",
													"Normalize3","Power",
													"ArcCosine","ArcSine",
													"ArcTangent","Average XYZ",
													"Average","Min",
													"Max","Floor",
													"Modulus","Absolute",
													"Shuffle YZWX","Shuffle ZWXY",
													"Shuffle WXYZ","Matrix Multiply",
													"Select W","Dot4",
													"Log","Lower or Equal",
													"Lower","Greater or Equal",
													"Greater","Equal",
													"Not Equal","And",
													"Or","Ternary"
			};

			static int selectedItem = 0;

			if (ImGui::Combo("Operation", &selectedItem, items, IM_ARRAYSIZE(items)))
			{
				nodeU.m_arithmetic_op_ = static_cast<HorusArithmeticOperation>(selectedItem);

				need_reevaluate = true;
				change_detected = true;
				root_node_changed();
				update_material();
			}


			ImNodes::EndNode();






		}
		break;





		case HorusUiNodeType::Constant_texture:
		{
			const float node_width = 100.f;

			ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(173, 216, 230, 255));
			ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(135, 206, 250, 255));
			ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(100, 149, 237, 255));
			ImNodes::BeginNode(node.id_);
			ImNodes::BeginNodeTitleBar();
			ImGui::TextColored(ImVec4(0.0f, 0.0f, 0.0f, 1.0f), "Constant color");
			ImNodes::EndNodeTitleBar();


			{
				ImNodes::BeginInputAttribute(node.constant_texture.input0);  // Assurez-vous que lookup_value est bien défini dans votre structure

				const float label_width = ImGui::CalcTextSize("Lookup").x;
				ImGui::Text("Lookup");

				ImGui::SameLine();
				ImGui::PushItemWidth(node_width - label_width);

				const char* items[] = {
										"UV",
										"Normal",
										"XYZ (world space)",
										"Incoming Ray",
										"Outgoing Ray",
										"Second UV",
										"XYZ (object space)",
										"0 float value assigned to vertex",
										"1 float value assigned to vertex",
										"2 float value assigned to vertex",
										"3 float value assigned to vertex",
										"Random color per shape",
										"Object ID",
										"Random color per mesh"
				};

				static int item_current = static_cast<int>(node.constant_texture.input0);

				if (ImGui::Combo("##lookup_value", &item_current, items, IM_ARRAYSIZE(items)))
				{
					// Mettez à jour la valeur de recherche (lookup value) dans votre structure de noeud
					//node.ui.constant_texture.lookup_value = static_cast<HorusLookupValue>(item_current);

					need_reevaluate = true;
					change_detected = true;
					root_node_changed();
					update_material();
				}

				ImGui::PopItemWidth();
				ImNodes::EndInputAttribute();
			}






			//{
			//	ImNodes::BeginInputAttribute(node.ui.constant_texture.color);

			//	const float label_width = ImGui::CalcTextSize("Color").x;
			//	ImGui::Text("Color");


			//	if (m_graph_.num_edges_from_node(node.ui.constant_texture.color) == 0ull) // check if there is no edge
			//	{
			//		ImGui::SameLine();
			//		ImGui::PushItemWidth(node_width - label_width);







			//		{

			//			need_reevaluate = true;
			//			change_detected = true;
			//			root_node_changed();
			//			update_material();
			//		}










			//		ImGui::PopItemWidth();
			//	}
			//	ImNodes::EndInputAttribute();
			//}

			ImGui::Spacing();
			{


				ImNodes::BeginOutputAttribute(node.id_);
				const float label_width = ImGui::CalcTextSize("Result").x;
				ImGui::Indent(node_width - label_width);
				ImGui::Text("Result");
				ImNodes::EndOutputAttribute();
			}

			ImNodes::EndNode();
		}
		break;


		case HorusUiNodeType::Checker_texture:
		{
			const float node_width = 100.f;

			ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(173, 216, 230, 255));
			ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(135, 206, 250, 255));
			ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(100, 149, 237, 255));

			ImNodes::BeginNode(node.id_);
			ImNodes::BeginNodeTitleBar();
			ImGui::TextColored(ImVec4(0.0f, 0.0f, 0.0f, 1.0f), "Checker_texture");
			ImNodes::EndNodeTitleBar();

			{
				ImNodes::BeginInputAttribute(node.checker_texture.input0);
				const float label_width = ImGui::CalcTextSize("UV").x;
				ImGui::Text("UV");

				const HorusNode& nodeU = m_graph_.node(node.id_);


				if (m_graph_.num_edges_from_node(node.checker_texture.input0) == 0ull)
				{
					ImGui::SameLine();
					ImGui::PushItemWidth(node_width - label_width);
					ImGui::SetNextItemWidth(150);
					if (ImGui::DragFloat2("##UV", (float*)&nodeU.m_uv_scale_, 0.01f))
					{

						need_reevaluate = true;
						change_detected = true;
						root_node_changed();
						update_material();
					}

					ImGui::PopItemWidth();
				}
				ImNodes::EndInputAttribute();
			}

			// Sortie du résultat
			ImGui::Spacing();
			{
				ImNodes::BeginOutputAttribute(node.id_);
				const float label_width = ImGui::CalcTextSize("Result").x;
				ImGui::Indent(node_width - label_width);
				ImGui::Text("Result");
				ImNodes::EndOutputAttribute();
			}

			// Fin du nœud
			ImNodes::EndNode();
		}
		break;

		case HorusUiNodeType::Image_texture:
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


		case HorusUiNodeType::Debug_color01:
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

		case HorusUiNodeType::Output:
		{
			const float node_width = 100.f;

			ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(173, 216, 230, 255));
			ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(135, 206, 250, 255));
			ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(100, 149, 237, 255));
			ImNodes::BeginNode(node.id_);

			ImNodes::BeginNodeTitleBar();
			ImGui::TextColored(ImVec4(0.0f, 0.0f, 0.0f, 1.0f), "Output");
			ImNodes::EndNodeTitleBar();

			ImGui::Dummy(ImVec2(node_width, 0.f));
			{
				ImNodes::BeginInputAttribute(node.output.output);
				const float label_width = ImGui::CalcTextSize("Output").x;
				ImGui::Text("Output");

				if (m_graph_.num_edges_from_node(node.output.output) == 0ull)
				{
					ImGui::SameLine();
					ImGui::PushItemWidth(node_width - label_width);
					//ImGui::DragFloat("##output", &m_graph_.node(node.ui.output.output0).m_value_, 0.01f);
					ImGui::PopItemWidth();
				}
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

	{ // Handle creation of new links
		int start_attr, end_attr;
		if (ImNodes::IsLinkCreated(&start_attr, &end_attr))
		{
			const HorusNodeType start_attr_type = m_graph_.node(start_attr).m_type_;
			const HorusNodeType end_attr_type = m_graph_.node(end_attr).m_type_;

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
	root_node_changed();

	if (m_root_node_ID_ != -1)
	{
		if (m_IsOutputConnected_)
		{
			if (!material_already_set)
			{
				g_object_impedit.set_material_from_editor_node(g_object_impedit.get_mesh_id_to_set_material(), m_out_modified_);
				material_already_set = true;
				change_detected = true;
			}

			spdlog::info("m_IsOutputConnected_ is true");

			output_connected_last_frame = true;
		}
		else
		{
			if (output_connected_last_frame)
			{
				g_object_impedit.set_material_from_editor_node(g_object_impedit.get_mesh_id_to_set_material(), m_out_node_);
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

				g_object_impedit.set_material_from_editor_node(g_object_impedit.get_mesh_id_to_set_material(), m_out_modified_); // Set final material 
				spdlog::info("id to set is {}", g_object_impedit.get_mesh_id_to_set_material());

				need_reevaluate = false;

			}
			else
			{
				m_IsThisMaterial_ = false;

				g_object_impedit.set_material_from_editor_node(g_object_impedit.get_mesh_id_to_set_material(), m_out_node_);

				spdlog::info("id to set is {}", g_object_impedit.get_mesh_id_to_set_material());

				need_reevaluate = false;
			}
		}

		spdlog::info("change detected");

		g_engine_impediet.call_reset_buffer();
		change_detected = false;
	}

	else
	{
		spdlog::info("no change detected");
	}

	spdlog::info("update material");
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
	ImGui::Begin("Material Browser");

	std::vector<std::string> materials;
	g_object_impedit.get_material_editor_materials(materials);


	if (ImGui::TreeNode("Materials"))
	{
		for (const auto& material : materials)
		{
			bool isSelected = (material == selectedMaterial);
			if (ImGui::Selectable(material.c_str(), isSelected))
			{
				selectedMaterial = material;

				int id = g_object_impedit.get_material_editor_id_by_name(material.c_str());

				g_object_impedit.set_material_editor_to_show(id);
				g_object_impedit.set_material_editor_mesh_to_set_material(id);

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
