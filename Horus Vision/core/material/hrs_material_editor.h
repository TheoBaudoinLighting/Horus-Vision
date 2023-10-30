#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <Math/float4.h>

#include "imgui.h"
#include "imnodes.h"
#include "hrs_graph.h"
#include "hrs_material.h"
#include "spdlog/spdlog.h"
#include "common.h"

class HorusMaterialEditorBrowser
{
public:

	static HorusMaterialEditorBrowser& get_instance()
	{
		static HorusMaterialEditorBrowser instance;
		return instance;
	}

	HorusMaterialEditorBrowser(HorusMaterialEditorBrowser const&) = delete;
	void operator=(HorusMaterialEditorBrowser const&) = delete;

	void init();
	void update();

	void show_material_browser();
	void hide_material_browser();

	void set_material_list();


	void quit();





private:

	HorusMaterialEditorBrowser()
	{}

	std::string selectedMaterial;



};

class HorusMaterialEditorCreateMenu
{
public:

	static HorusMaterialEditorCreateMenu& get_instance()
	{
		static HorusMaterialEditorCreateMenu instance;
		return instance;
	}

	HorusMaterialEditorCreateMenu(HorusMaterialEditorCreateMenu const&) = delete;
	void operator=(HorusMaterialEditorCreateMenu const&) = delete;





private:

	HorusMaterialEditorCreateMenu()
	{}





};

class HorusMaterialEditor
{
public:

	/*static HorusMaterialEditor& get_instance()
	{
		static HorusMaterialEditor instance;
		return instance;
	}

	HorusMaterialEditor(HorusMaterialEditor const&) = delete;
	void operator=(HorusMaterialEditor const&) = delete;*/


	HorusMaterialEditor() : m_graph_(), m_nodes_(), m_root_node_ID_(-1), m_mini_map_location_(ImNodesMiniMapLocation_BottomRight), m_out_node_(nullptr), m_out_modified_(nullptr)
	{}

	enum class HorusNodeType
	{
		Horus_PBR_Material,
		Horus_Material,
		Add,
		Ao_map,
		Arithmetic,
		Blend,
		Blend_value,
		Buffer_sampler,
		Bump_map,
		Checker_texture,
		Constant_texture,
		Diffuse,
		Diffuse_refraction,
		Dot_texture,
		Emissive,
		Fresnel,
		Fresnel_schlick,
		Gradient_texture,
		Image_texture,
		Input_lookup,
		Microfacet,
		Microfacet_anisotropic_reflection,
		Microfacet_anisotropic_refraction,
		Microfacet_beckmann,
		Microfacet_refraction,
		Noise2D_texture,
		Normal_map,
		Oren_nayar,
		Passthrough,
		Phong,
		Reflection,
		Refraction,
		Transparent,
		Twosided,
		UV_procedural_texture,
		UV_Triplanar,
		Volume,
		Ward,
		Print,
		Debug_color01,
		Debug_color02,
		Debug_color03,
		Output,
		Value,
	};

	enum class HorusLookupValue
	{
		UV = 0x0,
		Normal = 0x1,
		PositionWorld = 0x2,
		IncomingVec = 0x3,
		OutgoingVec = 0x4,
		UV1 = 0x5,
		PositionLocal = 0x6,
		VertexValue0 = 0x7,
		VertexValue1 = 0x8,
		VertexValue2 = 0x9,
		VertexValue3 = 0xa,
		ShapeRandomColor = 0xb,
		ObjectID = 0xc,
		PrimitiveRandomColor = 0xd
	};

	enum class HorusArithmeticOperation {
		ADD = 0x00,
		SUB = 0x01,
		MUL = 0x02,
		DIV = 0x03,
		SIN = 0x04,
		COS = 0x05,
		TAN = 0x06,
		SELECT_X = 0x07,
		SELECT_Y = 0x08,
		SELECT_Z = 0x09,
		COMBINE = 0x0A,
		DOT3 = 0x0B,
		CROSS3 = 0x0C,
		LENGTH3 = 0x0D,
		NORMALIZE3 = 0x0E,
		POW = 0x0F,
		ACOS = 0x10,
		ASIN = 0x11,
		ATAN = 0x12,
		AVERAGE_XYZ = 0x13,
		AVERAGE = 0x14,
		MIN = 0x15,
		MAX = 0x16,
		FLOOR = 0x17,
		MOD = 0x18,
		ABS = 0x19,
		SHUFFLE_YZWX = 0x1a,
		SHUFFLE_ZWXY = 0x1b,
		SHUFFLE_WXYZ = 0x1c,
		MAT_MUL = 0x1d,
		SELECT_W = 0x1e,
		DOT4 = 0x1f,
		LOG = 0x20,
		LOWER_OR_EQUAL = 0x21,
		LOWER = 0x22,
		GREATER_OR_EQUAL = 0x23,
		GREATER = 0x24,
		EQUAL = 0x25,
		NOT_EQUAL = 0x26,
		AND = 0x27,
		OR = 0x28,
		TERNARY = 0x29
	};

	struct HorusNode
	{
		HorusNodeType m_type_;
		rpr_material_node m_value_;
		ImVec4 m_color_;
		ImVec2 m_uv_scale_;
		HorusArithmeticOperation m_arithmetic_op_;

		// Basic PBR parameters (only for PBR material)

		// Diffuse parameters
		ImVec4 m_color_diffuse_ = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
		bool m_use_diffuse_texture_ = false;
		ImVec4 m_weight_diffuse_ = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		ImVec4 m_roughness_diffuse_ = ImVec4(0.f, 0.f, 0.f, 1.0f);

		// Reflection parameters
		ImVec4 m_color_reflection_ = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
		ImVec4 m_weight_reflection_ =  ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		ImVec4 m_roughness_reflection_ = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
		ImVec4 m_BRDF_reflection_ = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

		ImVec4 m_anisotropy_reflection_ = ImVec4(0.f, 0.f, 0.f, 1.0f);
		ImVec4 m_rotation_reflection_ = ImVec4(0.f, 0.f, 0.f, 1.0f);

		int m_mode_reflection_ = 0;
		ImVec4 m_ior_reflection_ = ImVec4(1.5f, 1.5f, 1.5f, 1.0f);

		// Sheen parameters
		ImVec4 m_color_sheen_ = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
		ImVec4 m_weight_sheen_ = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
		ImVec4 m_tint_sheen_ = ImVec4(0.f, 0.f, 0.f, 1.0f);

		// Clearcoat parameters
		ImVec4 m_color_clearcoat_ = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
		ImVec4 m_weight_clearcoat_ = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
		ImVec4 m_roughness_clearcoat_ = ImVec4(0.f, 0.f, 0.f, 1.0f);
		ImVec4 m_ior_clearcoat_ = ImVec4(1.5f, 1.5f, 1.5f, 1.0f);
		ImVec4 m_metalness_clearcoat_ = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
		ImVec4 m_transmission_clearcoat_color_ = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
		ImVec4 m_thickness_clearcoat_ = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

		// Transmission parameters
		ImVec4 m_color_transmission_ = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
		ImVec4 m_weight_transmission_ = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
		ImVec4 m_roughness_transmission_ = ImVec4(0.f, 0.f, 0.f, 1.0f);
		ImVec4 m_ior_transmission_ = ImVec4(1.5f, 1.5f, 1.5f, 1.0f);
		float m_thin_surface_ = 0.0f;
		ImVec4 m_absorption_color_ = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
		ImVec4 m_absorption_distance_ = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
		float m_caustics_ = 0.0f;

		// SSS parameters
		ImVec4 m_color_sss_ = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
		ImVec4 m_radius_sss_ = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
		ImVec4 m_direction_sss_ = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
		ImVec4 m_weight_sss_ = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
		int m_multi_scatter_ = 0;


		// Normal, bump and displacement parameters
		ImVec4 m_normal_ = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
		ImVec4 m_bump_ = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
		ImVec4 m_displacement_ = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

		rpr_image m_normal_image_ = nullptr;
		rpr_image m_bump_image_ = nullptr;
		rpr_image m_displacement_image_ = nullptr;

		// Emission parameters
		ImVec4 m_emission_color_ = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
		ImVec4 m_emission_weight_ = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

		// Opacity parameters
		ImVec4 m_opacity_ = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

		// Backscattering parameters
		ImVec4 m_backscattering_weight_ = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
		ImVec4 m_backscattering_color_ = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);



		rpr_char const* m_image_;
		bool m_texture_loaded_;

		inline static char* file_dialog_buffer = nullptr;
		char path[500] = "";
		rpr_char const* last_used_path = "";

		bool local_file_dialog_open = false;

		float m_blend_value_ = 0.5f;

		explicit HorusNode(const HorusNodeType type) : m_type_(type), m_value_(nullptr), m_image_(nullptr), m_texture_loaded_(false)
		{}

		HorusNode(const HorusNodeType type, const float value) : m_type_(type), m_value_(nullptr), m_image_(nullptr), m_texture_loaded_(false)
		{
			spdlog::info("Node instance");
		}
	};

	rpr_material_node evaluate_rpr_material_node(const HorusGraph<HorusNode>& graph, const int root_node);

	void init();
	void update();
	void update_material();
	bool root_node_changed();

	void quit();

	rpr_material_node get_out_node() const { return m_out_node_; }
	rpr_material_node get_out_modified() const { return m_out_modified_; }

	rpr_material_node get_final_material_for_export()
	{
		if (m_IsThisMaterial_)
		{
			return m_out_modified_;
		}
		else
		{
			return m_out_node_;
		}
	}




private:



	enum class HorusUiNodeType
	{
		Horus_PBR_Material,
		Horus_Material,
		Add,
		Ao_map,
		Arithmetic,
		Blend,
		Blend_value,
		Buffer_sampler,
		Bump_map,
		Checker_texture,
		Constant_texture,
		Diffuse,
		Diffuse_refraction,
		Dot_texture,
		Emissive,
		Fresnel,
		Fresnel_schlick,
		Gradient_texture,
		Image_texture,
		Input_lookup,
		Microfacet,
		Microfacet_anisotropic_reflection,
		Microfacet_anisotropic_refraction,
		Microfacet_beckmann,
		Microfacet_refraction,
		Noise2D_texture,
		Normal_map,
		Oren_nayar,
		Passthrough,
		Phong,
		Reflection,
		Refraction,
		Transparent,
		Twosided,
		UV_procedural_texture,
		UV_Triplanar,
		Volume,
		Ward,
		Print,
		Debug_color01,
		Debug_color02,
		Debug_color03,
		Output,
		Value,
	};

	struct HorusUiNode
	{
		HorusUiNodeType type_;


		// L'identifiant du nœud de l'interface utilisateur. Pour les opérations d'addition, de multiplication, de sinus et de temps,
		// il s'agit de l'identifiant du nœud "opération". Les nœuds d'entrée supplémentaires sont
		// stockés dans les structures.
		int id_;


		// Ne mettre que les inputs ici

		struct
		{
			int input_color_diffuse, input_weight_diffuse, input_roughness_diffuse;
			int input_color_reflection, input_weight_reflection, input_roughness_reflection, input_BRDF_reflection, input_anisotropy_reflection, input_rotation_reflection, input_mode_reflection, input_ior_reflection;
			int input_color_sheen, input_weight_sheen, input_tint_sheen;
			int input_color_clearcoat, input_weight_clearcoat, input_roughness_clearcoat, input_ior_clearcoat;
			int input_color_transmission, input_weight_transmission, input_roughness_transmission, input_ior_transmission;
			int input_normal, input_bump, input_displacement;
			int input_emission_color, input_emission_weight;
			int input_opacity, input_opacity_weight;
			int input_backscattering_weight, input_backscattering_color;
			int input_color_sss, input_radius_sss, input_direction_sss, input_weight_sss, input_multi_scatter;

		} pbr_material;

		struct
		{
			int input0, input1;
		} add;

		struct
		{
			int input0;
		} ao_map;

		struct
		{
			int input0, input1, input2, input3;
			int output0;
		} arithmetic;

		struct
		{
			int input0, input1, input2;
		} blend;

		struct
		{
			int input0;
		} buffer_sampler;

		struct
		{
			int input0;
			float value;
		} bump_map;

		struct
		{
			int input0;
		} checker_texture;

		struct
		{
			int input0;
		} constant_texture;


		struct
		{
			int input0;
		} constant_color;

		struct
		{
			int input0, output0;
		} image_texture;









		struct
		{
			int input;
		} Debug_color01;

		struct
		{
			int output;
		} Debug_color02;

		struct
		{
			int output;
		} Debug_color03;

		struct
		{
			int input0;
		} Print;

















		struct
		{
			int output;
		} output;



	};

	// Other

	RPRGarbageCollector m_garbage_collector_;

	bool m_IsOutputConnected_ = false;
	bool graph_modified = false;

	HorusGraph<HorusNode> m_graph_;
	std::vector<HorusUiNode> m_nodes_;
	int m_root_node_ID_ = -1;
	ImNodesMiniMapLocation m_mini_map_location_ = ImNodesMiniMapLocation_BottomRight;

	rpr_material_node m_out_node_ = nullptr;
	rpr_material_node m_out_modified_ = nullptr;
	bool m_IsThisMaterial_ = false;

	bool need_reset_buffer = false;
	bool material_already_set = false;
	bool buffer_reset_done = false;
	bool output_connected_last_frame = false;
	bool change_detected = false;
	bool need_reevaluate = false;

};

