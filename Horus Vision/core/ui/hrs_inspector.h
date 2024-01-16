#pragma once

// Project includes
#include "hrs_reset_buffers.h" // nothing

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

	static HorusInspector& GetInstance()
	{
		static HorusInspector Instance;
		return Instance;
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
	void InspectorProjectProperty();

	void ShowHelpMarker(const char* desc);

	// Getters
	void CallResetBuffer() { HorusResetBuffers::GetInstance().CallResetBuffers(); }

	// Getters Camera
	glm::vec3 GetCameraLookAt();
	glm::vec3 GetCameraPosition();
	glm::vec3 GetCameraRotation();
	glm::vec3 GetCameraScale();

	float GetFov();

	// Setters

	// Setters Camera
	void SetCameraLookAt(glm::vec3 look_at);
	void SetCameraPosition(glm::vec3 position);
	void SetCameraRotation(glm::vec3 rotation_axis);
	void SetCameraScale(glm::vec3 scale);

	void SetFov(float fov);

private:

	HorusInspector(): m_Gpu00N_{}, m_Gpu01N_{}, m_RenderStatistics_()
	{
	}

	// Main inspector
	InspectorType m_SelectionType_ = InspectorType::PROJECT;

	// Transform inspector

	// Camera inspector
	float m_Pitch_ = 0.0f;
	float m_Heading_ = 0.0f;

	// Light inspector

	// Material inspector

	// Mesh inspector

	// Texture inspector

	// Scene inspector

	// Render inspector

	// Project inspector
	bool m_ResetBuffer_ = false;
	bool m_EnableRenderRegion_ = false;
	bool m_PreviousEnableAdaptiveSampling_ = false;
	bool m_PreviousEnableAa_ = false;
	bool m_PreviousResetMode_ = false;
	bool m_EnableBackdropImage_ = false;
	bool m_PreviousEnableBackdropImage_ = false;
	bool m_EnableDenoiser_ = false;
	bool m_EnableRussianRoulette_ = false;
	bool m_PreviousEnableRussianRoulette_ = false;
	bool m_PreviousEnableTransparentBackground_ = false;
	bool m_EnableTransparentBackground_ = false;
	bool m_EnableAdaptiveSampling_ = false;
	bool m_EnablePreviewMode_ = false;
	bool m_EnableAa_ = true;
	bool m_ShowObjectContour_ = false;

	int m_SamplerType_ = RPR_CONTEXT_SAMPLER_TYPE_SOBOL;
	int m_MinSamples_ = 4;
	int m_MaxSamples_ = 32;
	int m_Suffix_ = 001;

	float m_AdaptiveThreshold_ = 0.01f;
	float m_DesiredWidth_ = 800.0f;

	char m_UserInput_[256] = "";
	char m_Gpu00N_[1024];
	char m_Gpu01N_[1024];

	rpr_render_statistics m_RenderStatistics_;
};