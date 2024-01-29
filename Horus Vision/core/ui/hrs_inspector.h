#pragma once

// Project includes
#include "hrs_reset_buffers.h" // nothing
#include "imgui.h"

class HorusInspector
{
public:

	enum class InspectorType
	{
		CAMERA,
		LIGHT,
		MATERIAL,
		GROUPSHAPE,
		SHAPE,
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
	void Init();

	void SetInspectorType(InspectorType Type);

	//void InspectorTransform();
	void InspectorCamera();
	void InspectorLight();
	void InspectorMaterial();
	void InspectorShape();
	void InspectorGroupShape();
	void InspectorTexture();
	void InspectorProjectProperty();

	void ShowHelpMarker(const char* desc);
	void ShowBigSeparator();

	// --- GroupShape ---
	void PopulateSelectedGroupShapeInfos();
	void SetGroupShapeName(std::string name);
	void SetGroupShapePosition(glm::vec3 position);
	void SetGroupShapeRotation(glm::vec3 rotation_axis);
	void SetGroupShapeScale(glm::vec3 scale);


	// --- Shape ---
	void PopulateSelectedShapeInfos();

	glm::vec3 GetShapePosition();
	glm::vec3 GetShapeRotation();
	glm::vec3 GetShapeScale();


	// --- Camera ---

	void PopulateSelectedCameraInfos();

	// Getters Camera
	void CallResetBuffer() { PopulateSelectedCameraInfos(); HorusResetBuffers::GetInstance().CallResetBuffers(); }
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
	void SetCameraLookAt(glm::vec3 look_at);
	void SetCameraPosition(glm::vec3 position);
	void SetCameraRotation(glm::vec3 rotation_axis);
	void SetCameraScale(glm::vec3 scale);

	void SetCameraNear(float Near);
	void SetCameraFar(float Far);
	void SetFStop(float FStop);
	void SetFov(float fov);

	// --- Light ---

	// --- Project ---
	void PopulateSelectedProjectInfos();



private:



	HorusInspector() : m_Gpu00N_{}, m_Gpu01N_{}
	{
	}

	//--------------------------------------------- GENERAL ---------------------------------------------//
	InspectorType m_SelectionType_ = InspectorType::PROJECT;

	const ImGuiInputTextFlags InputFloatFlags = ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue;

	//--------------------------------------------- CAMERA INSPECTOR ---------------------------------------------//
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

	//--------------------------------------------- LIGHT INSPECTOR ---------------------------------------------//

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

	//--------------------------------------------- MATERIAL INSPECTOR ---------------------------------------------//

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

	//--------------------------------------------- GROUP SHAPE INSPECTOR ---------------------------------------------//

	std::string m_GroupShapeName_;
	glm::vec3 m_GroupShapePosition_ = { 0.0f, 0.0f, 0.0f };
	glm::vec3 m_GroupShapeRotation_ = { 0.0f, 0.0f, 0.0f };
	glm::vec3 m_GroupShapeScale_ = { 1.0f, 1.0f, 1.0f };




	//--------------------------------------------- SHAPE INSPECTOR ---------------------------------------------//
	std::string m_ShapeName_;
	glm::mat4 m_ShapeTransform_ = glm::mat4(1.0f);
	glm::vec3 m_ShapePosition_ = { 0.0f, 0.0f, 0.0f };
	glm::vec3 m_ShapeRotation_ = { 0.0f, 0.0f, 0.0f };
	glm::vec3 m_ShapeScale_ = { 1.0f, 1.0f, 1.0f };


	//--------------------------------------------- TEXTURE INSPECTOR ---------------------------------------------//

	//--------------------------------------------- SCENE INSPECTOR ---------------------------------------------//

	//--------------------------------------------- RENDER INSPECTOR ---------------------------------------------//

	//--------------------------------------------- PROJECT INSPECTOR ---------------------------------------------//

	// Backdrop image
	std::string m_BackgroundPath_ = "path/to/image.jpg";
	std::string m_LastBackgroundPath_ = "";
	bool m_WasBackdropImageEnabled_ = false;
	bool m_JustDisabledBackdropImage_ = false;

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

	int m_SelectedRenderMode_ = 1;

	enum AOV
	{
		COLOR = 0,
		OPACITY = 1,
		WORLD_COORDINATE = 2,
		UV = 3,
		MATERIAL_ID = 4,
		GEOMETRIC_NORMAL = 5,
		SHADING_NORMAL = 6,
		DEPTH = 7,
		OBJECT_ID = 8,
		OBJECT_GROUP_ID = 9,
		SHADOW_CATCHER = 10,
		BACKGROUND = 11,
		EMISSION = 12,
		VELOCITY = 13,
		DIRECT_ILLUMINATION = 14,
		INDIRECT_ILLUMINATION = 15,
		AO = 16,
		DIRECT_DIFFUSE = 17,
		DIRECT_REFLECT = 18,
		INDIRECT_DIFFUSE = 19,
		INDIRECT_REFLECT = 20,
		REFRACT = 21,
		VOLUME = 22,
		DIFFUSE_ALBEDO = 23,
		VARIANCE = 24,
		VIEW_SHADING_NORMAL = 25,
		REFLECTION_CATCHER = 26,
		LIGHT_GROUP0 = 27,
		LIGHT_GROUP1 = 28,
		LIGHT_GROUP2 = 29,
		LIGHT_GROUP3 = 30,
		LIGHT_GROUP4 = 31,
		LIGHT_GROUP5 = 32,
		LIGHT_GROUP6 = 33,
		LIGHT_GROUP7 = 34,
		LIGHT_GROUP8 = 35,
		LIGHT_GROUP9 = 36,
		LIGHT_GROUP10 = 37,
		LIGHT_GROUP11 = 38,
		LIGHT_GROUP12 = 39,
		LIGHT_GROUP13 = 40,
		LIGHT_GROUP14 = 41,
		LIGHT_GROUP15 = 42
	};

	int m_SelectedAov_ = AOV::COLOR;

	const char* m_RenderModes_[10] = {
		"Diffuse only lighting",
		"Normal GI lighting",
		"Only direct lighting",
		"Direct lighting with no shadowing",
		"White objects with black wireframe",
		"Output only Material indices",
		"Output P positions for each pixel hit",
		"Output normal values",
		"Output UV coordinates",
		"Output an AO render"
	};

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
