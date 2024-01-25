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

	void Init();

	void SetInspectorType(InspectorType Type);

	//void InspectorTransform();
	void InspectorCamera();
	void InspectorLight();
	void InspectorMaterial();
	void InspectorMesh();
	void InspectorTexture();
	void InspectorProjectProperty();

	void ShowHelpMarker(const char* desc);

	// Getters
	void CallResetBuffer() { PopulateCameraInfos(); HorusResetBuffers::GetInstance().CallResetBuffers(); }

	// --- Camera ---

	void PopulateCameraInfos();

	// Getters Camera
	glm::vec3 GetCameraLookAt();
	glm::vec3 GetCameraPosition();
	glm::vec3 GetCameraRotation();
	glm::vec3 GetCameraScale();

	float GetCameraNear();
	float GetCameraFar();
	float GetFov();
	float GetFStop();
	float GetFocusDistance();
	int GetApertureBlades();
	void SetFocusPlaneToFocusPosition(rpr_shape& Plane);
	void CallSetFocusPlaneToFocusPosition() { SetFocusPlaneToFocusPosition(m_FocusPlaneShape_); }
	void ShowHideFocusPlane(rpr_shape& Plane);


	// Setters

	// Setters Camera
	void SetCameraLookAt(glm::vec3 look_at);
	void SetCameraPosition(glm::vec3 position);
	void SetCameraRotation(glm::vec3 rotation_axis);
	void SetCameraScale(glm::vec3 scale);

	void SetCameraNear(float Near);
	void SetCameraFar(float Far);
	void SetFStop(float FStop);
	void SetFov(float fov);

private:

	

	HorusInspector(): m_Gpu00N_{}, m_Gpu01N_{}
	{
	}

	// Main inspector
	InspectorType m_SelectionType_ = InspectorType::PROJECT;

	// Transform inspector

	// Camera inspector
	std::string m_CameraName_ = "";
	glm::vec3 m_CameraLookAt_ = { 0.0f, 0.0f, 0.0f };
	glm::vec3 m_CameraPosition_ = { 0.0f, 0.0f, 0.0f };
	glm::vec3 m_CameraRotation_ = { 0.0f, 0.0f, 0.0f };
	glm::vec3 m_CameraScale_ = { 1.0f, 1.0f, 1.0f };
	float m_UniformScale_ = 1.0f;
	float m_CameraNear_ = 0.0f;
	float m_CameraFar_ = 0.0f;
	float m_CameraFov_ = 0.0f;
	float m_CameraFStop_ = 1.2f;
	float m_CameraFStopTemp_ = 0.0f;
	float m_FocusPlaneDistance_ = 7.5f;
	int m_ApertureBlades_ = 5;

	bool m_EnableDof_ = false;
	bool m_DrawFocusPlane_ = false;
	rpr_shape m_FocusPlaneShape_ = nullptr;
	




	// Light inspector
	int m_LightType_ = 0;

	bool m_IsLightVisible_ = false;

	glm::vec3 m_LightIntensity_ = { 10.0f, 10.0f, 10.0f };
	glm::vec3 m_LightPosition_ = { 0.0f, 0.0f, 0.0f };
	glm::vec3 m_LightRotation_ = { 0.0f, 0.0f, 0.0f };
	glm::vec3 m_LightScale_ = { 1.0f, 1.0f, 1.0f };
	glm::vec3 m_LightDirection_ = { 0.0f, 1.0f, 0.0f };

	// Directional light
	float m_DirectionalLightShadowSoftnessAngle_ = 1.0f;

	// Spot light
	glm::vec2 m_SpotLightCone_ = { 0.20f, 1.0f };
	std::string m_SpotLightImage_ = "";

	// Environment light

	// Sky light
	float m_SkyLightTurbidity_ = 0.0f;
	glm::vec3 m_SkyLightAlbedo_ = { 0.0f, 0.0f, 0.0f };
	glm::vec3 m_SkyLightScale_ = { 0.0f, 0.0f, 0.0f };
	glm::vec3 m_SkyLightDirection_ = { 0.0f, 0.0f, 0.0f };

	// IES light
	std::string m_IesLightImage_ = "";

	// Sphere light
	float m_SphereLightRadius_ = 0.0f;

	// Disk light
	float m_DiskLightRadius_ = 1.0f;
	float m_DiskLightAngle = 1.0f;
	float m_DiskLightInnerAngle_ = 0.6f;

	// Material inspector
	glm::vec4 m_DiffuseColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };
	glm::vec4 m_DiffuseWeight_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_DiffuseRoughness_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_Roughness_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_Normal_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_Metallic_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_Emissive_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_Opacity_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_ReflectionColor_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_ReflectionWeight_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_ReflectionRoughness_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_RefractionColor_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_RefractionWeight_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_RefractionRoughness_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_CoatingColor_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_CoatingWeight_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_CoatingRoughness_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_Sheen_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_SheenWeight_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_SssScatterColor_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_SssScatterDistance_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_BackscatterColor_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_BackscatterWeight_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_EmissionWeight_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 m_Transparency_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	float m_Ior_ = 0.0f;
	bool m_ReflectionMode_ = false;
	bool m_CoatingMode_ = false;



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

	float m_AdaptiveThreshold_ = 0.025f;
	float m_DesiredWidth_ = 800.0f;

	char m_UserInput_[256] = "";
	char m_Gpu00N_[1024];
	char m_Gpu01N_[1024];

	rpr_render_statistics m_RenderStatistics_ = {};
};