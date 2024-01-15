#pragma once

// Project includes
#include "hrs_reset_buffers.h" // nothing

// External includes
#include "imgui.h"
#include "imgui_internal.h"
#include "imnodes.h"
#include "ImGuizmo.h"
//#include "L2DFileDialog.h"

// Basic includes
#include <filesystem>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <regex>
#include <chrono>

class HorusInspector
{
public:

	enum class InspectorType
	{
		CAMERA,
		LIGHT,
		MATERIAL,
		MESH,
		TEXTURE,
		SCENE,
		RENDER,
		PROJECT
	};

	static HorusInspector& get_instance()
	{
		static HorusInspector instance;
		return instance;
	}

	HorusInspector(HorusInspector const&) = delete;
	void operator=(HorusInspector const&) = delete;

	void Inspector(bool* p_open);
	InspectorType GetInspectorType();

	void SetInspectorType(InspectorType type);

	//void InspectorTransform();
	void InspectorCamera();
	void InspectorLight();
	void InspectorMaterial();
	void InspectorMesh();
	void InspectorTexture();
	void InspectorScene();
	void InspectorRender();
	void InspectorProjectProperty();

	void ShowhelpMarker(const char* desc);

	// Getters
	void CallResetBuffer() { HorusResetBuffers::get_instance().CallResetBuffers(); }

private:

	HorusInspector() {}

	// Main inspector
	InspectorType SelectionType = InspectorType::PROJECT;



	// Transform inspector

	// Camera inspector
	float m_pitch_ = 0.0f;
	float m_heading_ = 0.0f;

	// Light inspector

	// Material inspector

	// Mesh inspector

	// Texture inspector

	// Scene inspector

	// Render inspector

	// Project inspector
	bool m_ResetBuffer_ = false;
	bool enable_render_region_ = false;
	bool previous_enable_adaptive_sampling_ = false;
	bool previous_enable_AA_ = false;
	bool previous_reset_mode_ = false;
	bool enable_backdrop_image_ = false;
	bool previous_enable_backdrop_image_ = false;
	bool enable_denoiser_ = false;
	bool enable_russian_roulette_ = false;
	bool previous_enable_russian_roulette_ = false;
	bool previous_enable_transparent_background = false;
	bool enable_transparent_background = false;
	bool enable_adaptive_sampling_ = false;
	bool enable_preview_mode_ = false;
	bool enable_AA_ = true;
	bool show_object_contour_ = false;

	int samplerType = RPR_CONTEXT_SAMPLER_TYPE_SOBOL;
	int m_min_samples_ = 4;
	int m_max_samples_ = 32;
	int suffix = 001;

	float m_adaptive_threshold_ = 0.01f;
	float desired_width_ = 800.0f;

	char userInput[256] = "";
	char GPU00N[1024];
	char GPU01N[1024];

	rpr_render_statistics render_statistics_;

	

};