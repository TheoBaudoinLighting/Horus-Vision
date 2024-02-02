#pragma once

// Project includes
#include "hrs_material.h"
#include "hrs_reset_buffers.h" // nothing
#include "imgui.h"

class HorusInspector
{
public:

	struct PerformanceData
	{
		static const int DataLength = 60;
		int ms_per_frame_idx = 0;
		float ms_per_frame[DataLength] = { 0 };
		float ms_per_frame_accum = 0.0f;
		float fps_values[DataLength] = { 0 };

		void Update(float deltaTime, float framerate)
		{
			ms_per_frame_accum -= ms_per_frame[ms_per_frame_idx];
			ms_per_frame[ms_per_frame_idx] = deltaTime * 1000.0f;
			ms_per_frame_accum += ms_per_frame[ms_per_frame_idx];
			fps_values[ms_per_frame_idx] = framerate;
			ms_per_frame_idx = (ms_per_frame_idx + 1) % DataLength;
		}

		float GetAverageMsPerFrame() const
		{
			return ms_per_frame_accum / DataLength;
		}
	};

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

	// --- Texture ---

	// --- Material ---
	void DrawSwitchColorToTextureButton(bool& switchVariable, const char* id);
	void DrawParameterWithFileDialog(std::string& filePath, bool& parameterEnabled, const std::string& buttonID, const char* fileFilter, const std::string&
		parameterName, std::function<void(const std::string&)> onUpdate, std::function<void(const std::string&)>
		onEnable);
	void PopulateSelectedMaterialInfos();

	void DrawBaseColorSection();
	void DrawReflectionSection();
	void DrawSheenSection();
	void DrawRefractionSection();
	void DrawSssSection();
	void DrawCoatingSection();
	void DrawOtherSection();

	// Preset
	void SetMaterialDefault();
	void SetMaterialPlastic();
	void SetMaterialMetal();
	void SetMaterialGlass();
	void SetMaterialEmissive();
	void SetMaterialMatte();
	void SetMaterialSkin();
	void SetMaterialSSS();


	// --- Project ---
	void PopulateSelectedProjectInfos();



private:

	PerformanceData m_PerformanceData_;

	HorusInspector() : m_Gpu00N_{}, m_Gpu01N_{}
	{
	}

	//--------------------------------------------- GENERAL ---------------------------------------------//
	InspectorType m_SelectionType_ = InspectorType::PROJECT;

	const ImGuiInputTextFlags m_InputFloatFlags_ = ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue;
	const ImGuiTreeNodeFlags m_TreeNodeFlags_ = ImGuiTreeNodeFlags_Framed;
	const ImGuiTreeNodeFlags m_TreeNodeFlagsOpen_ = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed;
	const char m_ImageFilters_[59] = { "Image (*.jpg;*.png;*.exr;*.tiff)\0*.jpg;*.png;*.exr;*.tiff\0" };

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

	// Parameters are stored here in order of their appearance in the material inspector
	std::string m_MaterialName_;
	int m_PresetIndex_ = 0;
	const char* m_Presets_[8] = { "Default", "Plastic", "Metal", "Glass", "Emissive", "Matte", "Skin", "Subsurface" };

	// Base color secion --------------------------------------------------------------------------------//
	// Base color - Done
	bool m_BaseColorImageColor_ = false;
	bool m_UseTextureBaseColorInsteadOfColor_ = false;
	bool m_EnableBaseColorImage_ = false;
	std::string m_BaseColorPath_ = "path/to/BaseColor.jpg";
	glm::vec4 m_BaseColor_ = { 0.5f, 0.5f, 0.5f,1.0f };

	// Base color weight - Done
	bool m_BaseColorImageWeight_ = false;
	bool m_UseTextureBaseColorWeightInsteadOfWeight_ = false;
	bool m_EnableBaseColorWeightImage_ = false;
	std::string m_BaseColorWeightPath_ = "path/to/BaseColorWeight.jpg";
	glm::vec4 m_BaseColorWeight_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	// Base color roughness - Done
	bool m_BaseColorImageRoughness_ = false;
	bool m_UseTextureBaseColorRoughnessInsteadOfRoughness_ = false;
	bool m_EnableBaseColorRoughnessImage_ = false;
	std::string m_BaseColorRoughnessPath_ = "path/to/BaseColorRoughness.jpg";
	glm::vec4 m_BaseColorRoughness_ = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Backscatter weight - Done
	bool m_BackscatterImageWeight_ = false;
	bool m_UseTextureBackscatterWeightInsteadOfWeight_ = false;
	bool m_EnableBackscatterWeightImage_ = false;
	std::string m_BackscatterWeightPath_ = "path/to/BackscatterWeight.jpg";
	glm::vec4 m_BackscatterWeight_ = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Backscatter color - Done
	bool m_BackscatterImageColor_ = false;
	bool m_UseTextureBackscatterColorInsteadOfColor_ = false;
	bool m_EnableBackscatterColorImage_ = false;
	std::string m_BackscatterColorPath_ = "path/to/BackscatterColor.jpg";
	glm::vec4 m_BackscatterColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };


	// Reflection Section --------------------------------------------------------------------------------//
	// Reflection color
	bool m_ReflectionImageColor_ = false;
	bool m_UseTextureReflectionColorInsteadOfColor_ = false;
	bool m_EnableReflectionColorImage_ = false;
	std::string m_ReflectionColorPath_ = "path/to/ReflectionColor.jpg";
	glm::vec4 m_ReflectionColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	// Reflection weight
	bool m_ReflectionImageWeight_ = false;
	bool m_UseTextureReflectionWeightInsteadOfWeight_ = false;
	bool m_EnableReflectionWeightImage_ = false;
	std::string m_ReflectionWeightPath_ = "path/to/ReflectionWeight.jpg";
	glm::vec4 m_ReflectionWeight_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	// Reflection roughness
	bool m_ReflectionImageRoughness_ = false;
	bool m_UseTextureReflectionRoughnessInsteadOfRoughness_ = false;
	bool m_EnableReflectionRoughnessImage_ = false;
	std::string m_ReflectionRoughnessPath_ = "path/to/ReflectionRoughness.jpg";
	glm::vec4 m_ReflectionRoughness_ = { 0.5f, 0.5f, 0.5f, 0.5f };

	// Reflection anisotropy
	bool m_ReflectionImageAnisotropy_ = false;
	bool m_UseTextureReflectionAnisotropyInsteadOfAnisotropy_ = false;
	bool m_EnableReflectionAnisotropyImage_ = false;
	std::string m_ReflectionAnisotropyPath_ = "path/to/ReflectionAnisotropy.jpg";
	glm::vec4 m_ReflectionAnisotropy_ = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Reflection anisotropy rotation
	bool m_ReflectionImageAnisotropyRotation_ = false;
	bool m_UseTextureReflectionAnisotropyRotationInsteadOfAnisotropyRotation_ = false;
	bool m_EnableReflectionAnisotropyRotationImage_ = false;
	std::string m_ReflectionAnisotropyRotationPath_ = "path/to/ReflectionAnisotropyRotation.jpg";
	glm::vec4 m_ReflectionAnisotropyRotation_ = { 0.0f, 0.0f, 0.0f, 0.0f };

	int m_ReflectionModeIndex_ = 0;
	const char* m_ReflectionModes_[2] = { "PBR", "Metalness" };
	bool m_ReflectionMode_ = HorusMaterial::ReflectionTypePBR;
	bool m_ShowMetallicUi_ = false;
	float m_Ior_ = 1.36f;

	// reflection metalness
	bool m_ReflectionImageMetalness_ = false;
	bool m_UseTextureReflectionMetalnessInsteadOfMetalness_ = false;
	bool m_EnableReflectionMetalnessImage_ = false;
	std::string m_ReflectionMetalnessPath_ = "path/to/ReflectionMetalness.jpg";
	glm::vec4 m_ReflectionMetalness_ = { 0.0f, 0.0f, 0.0f, 0.0f };


	// Sheen Section --------------------------------------------------------------------------------//
	// Sheen color
	bool m_SheenImageColor_ = false;
	bool m_UseTextureSheenColorInsteadOfColor_ = false;
	bool m_EnableSheenColorImage_ = false;
	std::string m_SheenColorPath_ = "path/to/SheenColor.jpg";
	glm::vec4 m_SheenColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	// Sheen weight
	bool m_SheenImageWeight_ = false;
	bool m_UseTextureSheenWeightInsteadOfWeight_ = false;
	bool m_EnableSheenWeightImage_ = false;
	std::string m_SheenWeightPath_ = "path/to/SheenWeight.jpg";
	glm::vec4 m_SheenWeight_ = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Sheen Tint
	bool m_SheenImageTint_ = false;
	bool m_UseTextureSheenTintInsteadOfTint_ = false;
	bool m_EnableSheenTintImage_ = false;
	std::string m_SheenTintPath_ = "path/to/SheenTint.jpg";
	glm::vec4 m_SheenTint_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	// Refraction Section --------------------------------------------------------------------------------//
	// Refraction color
	bool m_RefractionImageColor_ = false;
	bool m_UseTextureRefractionColorInsteadOfColor_ = false;
	bool m_EnableRefractionColorImage_ = false;
	std::string m_RefractionColorPath_ = "path/to/RefractionColor.jpg";
	glm::vec4 m_RefractionColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	// Refraction weight
	bool m_RefractionImageWeight_ = false;
	bool m_UseTextureRefractionWeightInsteadOfWeight_ = false;
	bool m_EnableRefractionWeightImage_ = false;
	std::string m_RefractionWeightPath_ = "path/to/RefractionWeight.jpg";
	glm::vec4 m_RefractionWeight_ = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Refraction NormalMap
	bool m_RefractionImageNormalMap_ = false;
	bool m_UseTextureRefractionNormalMapInsteadOfNormalMap_ = false;
	bool m_EnableRefractionNormalMapImage_ = false;
	std::string m_RefractionNormalMapPath_ = "path/to/RefractionNormalMap.jpg";
	glm::vec4 m_RefractionNormalMap_ = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Refraction NormalMap weight
	float m_RefractionNormalMapWeight_ = 0.0f;

	// Refraction roughness
	bool m_RefractionImageRoughness_ = false;
	bool m_UseTextureRefractionRoughnessInsteadOfRoughness_ = false;
	bool m_EnableRefractionRoughnessImage_ = false;
	std::string m_RefractionRoughnessPath_ = "path/to/RefractionRoughness.jpg";
	glm::vec4 m_RefractionRoughness_ = { 0.0f, 0.0f, 0.0f, 0.0f };

	float m_RefractionIor_ = 1.5f;
	bool m_RefractionThinSurface_ = false;

	// Refraction absorption
	bool m_RefractionAbsorptionImageColor_ = false;
	bool m_UseTextureRefractionAbsorptionColorInsteadOfColor_ = false;
	bool m_EnableRefractionAbsorptionColorImage_ = false;
	std::string m_RefractionAbsorptionColorPath_ = "path/to/RefractionAbsorptionColor.jpg";
	glm::vec4 m_RefractionAbsorptionColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	// Refraction absorption distance
	bool m_RefractionAbsorptionImageDistance_ = false;
	bool m_UseTextureRefractionAbsorptionDistanceInsteadOfDistance_ = false;
	bool m_EnableRefractionAbsorptionDistanceImage_ = false;
	std::string m_RefractionAbsorptionDistancePath_ = "path/to/RefractionAbsorptionDistance.jpg";
	glm::vec4 m_RefractionAbsorptionDistance_ = { 1.0f, 1.0f, 1.0f, 1.0f };
	bool m_RefractionCaustics_ = false;

	// SSS Section --------------------------------------------------------------------------------//
	// SSS color
	bool m_SssImageColor_ = false;
	bool m_UseTextureSssColorInsteadOfColor_ = false;
	bool m_EnableSssColorImage_ = false;
	std::string m_SssColorPath_ = "path/to/SssColor.jpg";
	glm::vec4 m_SssScatterColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	// SSS weight
	bool m_SssImageWeight_ = false;
	bool m_UseTextureSssWeightInsteadOfWeight_ = false;
	bool m_EnableSssWeightImage_ = false;
	std::string m_SssWeightPath_ = "path/to/SssWeight.jpg";
	glm::vec4 m_SssScatterWeight_ = { 0.0f, 0.0f, 0.0f, 0.0f };

	// SSS radius
	bool m_SssImageRadius_ = false;
	bool m_UseTextureSssRadiusInsteadOfRadius_ = false;
	bool m_EnableSssRadiusImage_ = false;
	std::string m_SssRadiusPath_ = "path/to/SssRadius.jpg";
	glm::vec4 m_SssScatterDistance_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	// SSS direction
	bool m_SssImageDirection_ = false;
	bool m_UseTextureSssDirectionInsteadOfDirection_ = false;
	bool m_EnableSssDirectionImage_ = false;
	std::string m_SssDirectionPath_ = "path/to/SssDirection.jpg";
	glm::vec4 m_SssScatterDirection_ = { 0.0f, 0.0f, 0.0f, 0.0f };

	bool m_SssUseMultiScattering_ = false;
	bool m_SssUseSchlickApproximation_ = true;

	// Coating Section --------------------------------------------------------------------------------//
	// Coating color
	bool m_CoatingImageColor_ = false;
	bool m_UseTextureCoatingColorInsteadOfColor_ = false;
	bool m_EnableCoatingColorImage_ = false;
	std::string m_CoatingColorPath_ = "path/to/CoatingColor.jpg";
	glm::vec4 m_CoatingColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	// Coating weight
	bool m_CoatingImageWeight_ = false;
	bool m_UseTextureCoatingWeightInsteadOfWeight_ = false;
	bool m_EnableCoatingWeightImage_ = false;
	std::string m_CoatingWeightPath_ = "path/to/CoatingWeight.jpg";
	glm::vec4 m_CoatingWeight_ = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Coating Normal map
	bool m_CoatingImageNormalMap_ = false;
	bool m_UseTextureCoatingNormalMapInsteadOfNormalMap_ = false;
	bool m_EnableCoatingNormalMapImage_ = false;
	std::string m_CoatingNormalMapPath_ = "path/to/CoatingNormalMap.jpg";
	glm::vec4 m_CoatingNormalMap_ = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Coating Normal weight
	float m_CoatingNormalMapWeight_ = 0.0f;

	// Coating roughness
	bool m_CoatingImageRoughness_ = false;
	bool m_UseTextureCoatingRoughnessInsteadOfRoughness_ = false;
	bool m_EnableCoatingRoughnessImage_ = false;
	std::string m_CoatingRoughnessPath_ = "path/to/CoatingRoughness.jpg";
	glm::vec4 m_CoatingRoughness_ = { 0.0f, 0.0f, 0.0f, 0.0f };

	float m_CoatingIor_ = 1.46f;

	// Coating thickness
	bool m_CoatingImageThickness_ = false;
	bool m_UseTextureCoatingThicknessInsteadOfThickness_ = false;
	bool m_EnableCoatingThicknessImage_ = false;
	std::string m_CoatingThicknessPath_ = "path/to/CoatingThickness.jpg";
	glm::vec4 m_CoatingThickness_ = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Coating metalness
	bool m_CoatingImageMetalness_ = false;
	bool m_UseTextureCoatingMetalnessInsteadOfMetalness_ = false;
	bool m_EnableCoatingMetalnessImage_ = false;
	std::string m_CoatingMetalnessPath_ = "path/to/CoatingMetalness.jpg";
	glm::vec4 m_CoatingMetalness_ = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Coating Transmission color
	bool m_CoatingImageTransmissionColor_ = false;
	bool m_UseTextureCoatingTransmissionColorInsteadOfTransmissionColor_ = false;
	bool m_EnableCoatingTransmissionColorImage_ = false;
	std::string m_CoatingTransmissionColorPath_ = "path/to/CoatingTransmissionColor.jpg";
	glm::vec4 m_CoatingTransmissionColor_ = { 0.0f, 0.0f, 0.0f, 0.0f };

	int m_CoatingModeIndex_ = 0;
	const char* m_CoatingModes_[2] = { "PBR", "Metalness" };
	bool m_CoatingMetalnessUI_ = false;
	bool m_CoatingMode_ = HorusMaterial::CoatingTypePBR;

	// Other Section --------------------------------------------------------------------------------//
	// Normal map
	bool m_NormalMapImage_ = false;
	bool m_UseTextureNormalMapInsteadOfNormalMap_ = false;
	bool m_EnableNormalMapImage_ = false;
	std::string m_NormalMapPath_ = "path/to/NormalMap.jpg";
	glm::vec4 m_NormalMap_ = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Normal weight
	glm::vec4 m_NormalMapWeight_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	// Displacement map
	bool m_DisplacementMapImage_ = false;
	bool m_UseTextureDisplacementMapInsteadOfDisplacementMap_ = false;
	bool m_EnableDisplacementMapImage_ = false;
	std::string m_DisplacementMapPath_ = "path/to/DisplacementMap.jpg";
	glm::vec4 m_DisplacementMap_ = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Displacement map weight
	glm::vec4 m_DisplacementMapWeight_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	// Emissive
	bool m_EmissiveImage_ = false;
	bool m_UseTextureEmissiveInsteadOfEmissive_ = false;
	bool m_EnableEmissiveImage_ = false;
	std::string m_EmissivePath_ = "path/to/Emissive.jpg";
	glm::vec4 m_Emissive_ = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Emissive weight
	bool m_EmissiveWeightImage_ = false;
	bool m_UseTextureEmissiveWeightInsteadOfEmissiveWeight_ = false;
	bool m_EnableEmissiveWeightImage_ = false;
	std::string m_EmissiveWeightPath_ = "path/to/EmissiveWeight.jpg";
	glm::vec4 m_EmissionWeight_ = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Opacity
	bool m_OpacityImage_ = false;
	bool m_UseTextureOpacityInsteadOfOpacity_ = false;
	bool m_EnableOpacityImage_ = false;
	std::string m_OpacityPath_ = "path/to/Opacity.jpg";
	glm::vec4 m_Opacity_ = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Transparency
	bool m_TransparencyImage_ = false;
	bool m_UseTextureTransparencyInsteadOfTransparency_ = false;
	bool m_EnableTransparencyImage_ = false;
	std::string m_TransparencyPath_ = "path/to/Transparency.jpg";
	glm::vec4 m_Transparency_ = { 0.0f, 0.0f, 0.0f, 0.0f };

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
