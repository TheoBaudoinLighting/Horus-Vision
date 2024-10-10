#pragma once
#pragma warning(disable : 4996)
#pragma warning(disable : 4244)
#pragma warning(disable : 4267)
#pragma warning(disable : 4005)

// Basic includes
#include <optional>
#include <string>
// Basic includes
#include <variant>
#include <array>

// Project includes
#include "hrs_garbage_collector.h"
#include "glm/glm.hpp"
#include "glm/fwd.hpp"
#include "glm/detail/type_vec3.hpp"

class HorusMaterial
{
public:

	enum ReflectionType
	{
		ReflectionTypeMetalness = 1,
		ReflectionTypePBR = 0
	};

	enum CoatingType
	{
		CoatingTypeMetalness = 1,
		CoatingTypePBR = 0
	};

	HorusMaterial() {}

	void Init();

	rpr_material_node CreateMaterial();
	rpr_material_node GetMaterialNode() { return m_Material_; }
	void DestroyMaterial();
	void LoadAndApplyUDIMTexture(const std::string& UDIMRootPath,
		const std::vector<std::pair<int, std::string>>& UDIMTiles);

	// Getters
	// Base Color Section --------------------------------------------------------------------------------//
	bool GetBaseColorImageColor() { return m_BaseColorImageColor_; }
	bool GetUseTextureBaseColorInsteadOfColor() { return m_UseTextureBaseColorInsteadOfColor_; }
	bool GetEnableBaseColorImage() { return m_EnableBaseColorImage_; }
	std::string GetBaseColorPath() { return m_BaseColorPath_; }
	glm::vec4 GetBaseColor() { return m_BaseColor_; }

	// Base color weight
	bool GetBaseColorImageWeight() { return m_BaseColorImageWeight_; }
	bool GetUseTextureBaseColorWeightInsteadOfWeight() { return m_UseTextureBaseColorWeightInsteadOfWeight_; }
	bool GetEnableBaseColorWeightImage() { return m_EnableBaseColorWeightImage_; }
	std::string GetBaseColorWeightPath() { return m_BaseColorWeightPath_; }
	glm::vec4 GetBaseColorWeight() { return m_BaseColorWeight_; }

	// Base color roughness
	bool GetBaseColorImageRoughness() { return m_BaseColorImageRoughness_; }
	bool GetUseTextureBaseColorRoughnessInsteadOfRoughness() { return m_UseTextureBaseColorRoughnessInsteadOfRoughness_; }
	bool GetEnableBaseColorRoughnessImage() { return m_EnableBaseColorRoughnessImage_; }
	std::string GetBaseColorRoughnessPath() { return m_BaseColorRoughnessPath_; }
	glm::vec4 GetBaseColorRoughness() { return m_BaseColorRoughness_; }

	// Backscatter weight
	bool GetBackscatterImageWeight() { return m_BackscatterImageWeight_; }
	bool GetUseTextureBackscatterWeightInsteadOfWeight() { return m_UseTextureBackscatterWeightInsteadOfWeight_; }
	bool GetEnableBackscatterWeightImage() { return m_EnableBackscatterWeightImage_; }
	std::string GetBackscatterWeightPath() { return m_BackscatterWeightPath_; }
	glm::vec4 GetBackscatterColor() { return m_BackscatterColor_; }

	// Backscatter color
	bool GetBackscatterImageColor() { return m_BackscatterImageColor_; }
	bool GetUseTextureBackscatterColorInsteadOfColor() { return m_UseTextureBackscatterColorInsteadOfColor_; }
	bool GetEnableBackscatterColorImage() { return m_EnableBackscatterColorImage_; }
	std::string GetBackscatterColorPath() { return m_BackscatterColorPath_; }
	glm::vec4 GetBackscatterWeight() { return m_BackscatterWeight_; }

	// Reflection Section --------------------------------------------------------------------------------//
	// Reflection color
	bool GetReflectionImageColor() { return m_ReflectionImageColor_; }
	bool GetUseTextureReflectionColorInsteadOfColor() { return m_UseTextureReflectionColorInsteadOfColor_; }
	bool GetEnableReflectionColorImage() { return m_EnableReflectionColorImage_; }
	std::string GetReflectionColorPath() { return m_ReflectionColorPath_; }
	glm::vec4 GetReflectionColor() { return m_ReflectionColor_; }

	// Reflection weight
	bool GetReflectionImageWeight() { return m_ReflectionImageWeight_; }
	bool GetUseTextureReflectionWeightInsteadOfWeight() { return m_UseTextureReflectionWeightInsteadOfWeight_; }
	bool GetEnableReflectionWeightImage() { return m_EnableReflectionWeightImage_; }
	std::string GetReflectionWeightPath() { return m_ReflectionWeightPath_; }
	glm::vec4 GetReflectionWeight() { return m_ReflectionWeight_; }

	// Reflection roughness
	bool GetReflectionImageRoughness() { return m_ReflectionImageRoughness_; }
	bool GetUseTextureReflectionRoughnessInsteadOfRoughness() { return m_UseTextureReflectionRoughnessInsteadOfRoughness_; }
	bool GetEnableReflectionRoughnessImage() { return m_EnableReflectionRoughnessImage_; }
	std::string GetReflectionRoughnessPath() { return m_ReflectionRoughnessPath_; }
	glm::vec4 GetReflectionRoughness() { return m_ReflectionRoughness_; }

	// Reflection anisotropy
	bool GetReflectionImageAnisotropy() { return m_ReflectionAnisotropyImage_; }
	bool GetUseTextureReflectionAnisotropyInsteadOfAnisotropy() { return m_UseTextureReflectionAnisotropyInsteadOfAnisotropy_; }
	bool GetEnableReflectionAnisotropyImage() { return m_EnableReflectionAnisotropyImage_; }
	std::string GetReflectionAnisotropyPath() { return m_ReflectionAnisotropyPath_; }
	glm::vec4 GetReflectionAnisotropy() { return m_ReflectionAnisotropy_; }

	// Reflection anisotropy rotation
	bool GetReflectionImageAnisotropyRotation() { return m_ReflectionAnisotropyRotationImage_; }
	bool GetUseTextureReflectionAnisotropyRotationInsteadOfAnisotropyRotation() { return m_UseTextureReflectionAnisotropyRotationInsteadOfAnisotropyRotation_; }
	bool GetEnableReflectionAnisotropyRotationImage() { return m_EnableReflectionAnisotropyRotationImage_; }
	std::string GetReflectionAnisotropyRotationPath() { return m_ReflectionAnisotropyRotationPath_; }
	glm::vec4 GetReflectionAnisotropyRotation() { return m_ReflectionAnisotropyRotation_; }

	ReflectionType GetReflectionMode() { return m_ReflectionMode_; }
	float GetIor() { return m_Ior_; }

	// Reflection Metalness
	bool GetReflectionImageMetalness() { return m_ReflectionImageMetalness_; }
	bool GetUseTextureReflectionMetalnessInsteadOfMetalness() { return m_UseTextureReflectionMetalnessInsteadOfMetalness_; }
	bool GetEnableReflectionMetalnessImage() { return m_EnableReflectionMetalnessImage_; }
	std::string GetReflectionMetalnessPath() { return m_ReflectionMetalnessPath_; }
	glm::vec4 GetReflectionMetalness() { return m_ReflectionMetalness_; }

	// Sheen Section --------------------------------------------------------------------------------//
	// Sheen color
	bool GetSheenImageColor() { return m_SheenImageColor_; }
	bool GetUseTextureSheenColorInsteadOfColor() { return m_UseTextureSheenColorInsteadOfColor_; }
	bool GetEnableSheenColorImage() { return m_EnableSheenColorImage_; }
	std::string GetSheenColorPath() { return m_SheenColorPath_; }
	glm::vec4 GetSheenColor() { return m_SheenColor_; }

	// Sheen weight
	bool GetSheenImageWeight() { return m_SheenImageWeight_; }
	bool GetUseTextureSheenWeightInsteadOfWeight() { return m_UseTextureSheenWeightInsteadOfWeight_; }
	bool GetEnableSheenWeightImage() { return m_EnableSheenWeightImage_; }
	std::string GetSheenWeightPath() { return m_SheenWeightPath_; }
	glm::vec4 GetSheenWeight() { return m_SheenWeight_; }

	// Sheen tint
	bool GetSheenImageTint() { return m_SheenImageTint_; }
	bool GetUseTextureSheenTintInsteadOfTint() { return m_UseTextureSheenTintInsteadOfTint_; }
	bool GetEnableSheenTintImage() { return m_EnableSheenTintImage_; }
	std::string GetSheenTintPath() { return m_SheenTintPath_; }
	glm::vec4 GetSheenTint() { return m_SheenTint_; }

	// Refraction Section --------------------------------------------------------------------------------//
// Refraction color
	bool GetRefractionImageColor() { return m_RefractionImageColor_; }
	bool GetUseTextureRefractionColorInsteadOfColor() { return m_UseTextureRefractionColorInsteadOfColor_; }
	bool GetEnableRefractionColorImage() { return m_EnableRefractionColorImage_; }
	std::string GetRefractionColorPath() { return m_RefractionColorPath_; }
	glm::vec4 GetRefractionColor() { return m_RefractionColor_; }

	// Refraction weight
	bool GetRefractionImageWeight() { return m_RefractionImageWeight_; }
	bool GetUseTextureRefractionWeightInsteadOfWeight() { return m_UseTextureRefractionWeightInsteadOfWeight_; }
	bool GetEnableRefractionWeightImage() { return m_EnableRefractionWeightImage_; }
	std::string GetRefractionWeightPath() { return m_RefractionWeightPath_; }
	glm::vec4 GetRefractionWeight() { return m_RefractionWeight_; }

	// Refraction Normal Map
	bool GetRefractionImageNormal() { return m_RefractionImageNormal_; }
	bool GetUseTextureRefractionNormalInsteadOfNormal() { return m_UseTextureRefractionNormalInsteadOfNormal_; }
	bool GetEnableRefractionNormalImage() { return m_EnableRefractionNormalImage_; }
	std::string GetRefractionNormalPath() { return m_RefractionNormalPath_; }
	glm::vec4 GetRefractionNormal() { return m_RefractionNormal_; }

	// Refraction Normal Map Weight
	float GetRefractionNormalWeight() { return m_RefractionNormalWeight_; }

	// Refraction roughness
	bool GetRefractionImageRoughness() { return m_RefractionImageRoughness_; }
	bool GetUseTextureRefractionRoughnessInsteadOfRoughness() { return m_UseTextureRefractionRoughnessInsteadOfRoughness_; }
	bool GetEnableRefractionRoughnessImage() { return m_EnableRefractionRoughnessImage_; }
	std::string GetRefractionRoughnessPath() { return m_RefractionRoughnessPath_; }
	glm::vec4 GetRefractionRoughness() { return m_RefractionRoughness_; }

	float GetRefractionIor() { return m_RefractionIor_; }
	bool GetRefractionThinSurface() { return m_RefractionThinSurface_; }

	// Refraction Absorption
	bool GetRefractionImageAbsorptionColor() { return m_RefractionImageAbsorptionColor_; }
	bool GetUseTextureRefractionAbsorptionColorInsteadOfAbsorptionColor() { return m_UseTextureRefractionAbsorptionColorInsteadOfAbsorptionColor_; }
	bool GetEnableRefractionAbsorptionColorImage() { return m_EnableRefractionAbsorptionColorImage_; }
	std::string GetRefractionAbsorptionColorPath() { return m_RefractionAbsorptionColorPath_; }
	glm::vec4 GetRefractionAbsorptionColor() { return m_RefractionAbsorptionColor_; }

	// Refraction Absorption distance
	bool GetRefractionImageAbsorptionDistance() { return m_RefractionImageAbsorptionDistance_; }
	bool GetUseTextureRefractionAbsorptionDistanceInsteadOfAbsorptionDistance() { return m_UseTextureRefractionAbsorptionDistanceInsteadOfAbsorptionDistance_; }
	bool GetEnableRefractionAbsorptionDistanceImage() { return m_EnableRefractionAbsorptionDistanceImage_; }
	std::string GetRefractionAbsorptionDistancePath() { return m_RefractionAbsorptionDistancePath_; }
	glm::vec4 GetRefractionAbsorptionDistance() { return m_RefractionAbsorptionDistance_; }

	bool GetRefractionCaustics() { return m_RefractionCaustics_; }

	// SSS Section --------------------------------------------------------------------------------//
// SSS scatter color
	bool GetSssImageScatterColor() { return m_SssImageScatterColor_; }
	bool GetUseTextureSssScatterColorInsteadOfScatterColor() { return m_UseTextureSssScatterColorInsteadOfScatterColor_; }
	bool GetEnableSssScatterColorImage() { return m_EnableSssScatterColorImage_; }
	std::string GetSssScatterColorPath() { return m_SssScatterColorPath_; }
	glm::vec4 GetSssScatterColor() { return m_SssScatterColor_; }

	// SSS scatter weight
	bool GetSssImageScatterWeight() { return m_SssImageScatterWeight_; }
	bool GetUseTextureSssScatterWeightInsteadOfScatterWeight() { return m_UseTextureSssScatterWeightInsteadOfScatterWeight_; }
	bool GetEnableSssScatterWeightImage() { return m_EnableSssScatterWeightImage_; }
	std::string GetSssScatterWeightPath() { return m_SssScatterWeightPath_; }
	glm::vec4 GetSssScatterWeight() { return m_SssScatterWeight_; }

	// SSS scatter distance
	bool GetSssImageScatterDistance() { return m_SssImageScatterDistance_; }
	bool GetUseTextureSssScatterDistanceInsteadOfScatterDistance() { return m_UseTextureSssScatterDistanceInsteadOfScatterDistance_; }
	bool GetEnableSssScatterDistanceImage() { return m_EnableSssScatterDistanceImage_; }
	std::string GetSssScatterDistancePath() { return m_SssScatterDistancePath_; }
	glm::vec4 GetSssScatterDistance() { return m_SssScatterDistance_; }

	// SSS scatter direction
	bool GetSssImageScatterDirection() { return m_SssImageScatterDirection_; }
	bool GetUseTextureSssScatterDirectionInsteadOfScatterDirection() { return m_UseTextureSssScatterDirectionInsteadOfScatterDirection_; }
	bool GetEnableSssScatterDirectionImage() { return m_EnableSssScatterDirectionImage_; }
	std::string GetSssScatterDirectionPath() { return m_SssScatterDirectionPath_; }
	glm::vec4 GetSssScatterDirection() { return m_SssScatterDirection_; }


	bool GetSssUseMultiScattering() { return m_SssUseMultiScattering_; }
	bool GetSssUseSchlickApproximation() { return m_SssUseSchlickApproximation_; }

	// Coating Section --------------------------------------------------------------------------------//
// Coating color
	bool GetCoatingImageColor() { return m_CoatingImageColor_; }
	bool GetUseTextureCoatingColorInsteadOfColor() { return m_UseTextureCoatingColorInsteadOfColor_; }
	bool GetEnableCoatingColorImage() { return m_EnableCoatingColorImage_; }
	std::string GetCoatingColorPath() { return m_CoatingColorPath_; }
	glm::vec4 GetCoatingColor() { return m_CoatingColor_; }

	// Coating weight
	bool GetCoatingImageWeight() { return m_CoatingImageWeight_; }
	bool GetUseTextureCoatingWeightInsteadOfWeight() { return m_UseTextureCoatingWeightInsteadOfWeight_; }
	bool GetEnableCoatingWeightImage() { return m_EnableCoatingWeightImage_; }
	std::string GetCoatingWeightPath() { return m_CoatingWeightPath_; }
	glm::vec4 GetCoatingWeight() { return m_CoatingWeight_; }

	// Coating Normal Map 
	bool GetCoatingImageNormal() { return m_CoatingImageNormal_; }
	bool GetUseTextureCoatingNormalInsteadOfNormal() { return m_UseTextureCoatingNormalInsteadOfNormal_; }
	bool GetEnableCoatingNormalImage() { return m_EnableCoatingNormalImage_; }
	std::string GetCoatingNormalPath() { return m_CoatingNormalPath_; }
	glm::vec4 GetCoatingNormal() { return m_CoatingNormal_; }

	// Coainting normal weight
	float GetCoatingNormalWeight() { return m_CoatingNormalWeight_; }

	// Coating roughness
	bool GetCoatingImageRoughness() { return m_CoatingImageRoughness_; }
	bool GetUseTextureCoatingRoughnessInsteadOfRoughness() { return m_UseTextureCoatingRoughnessInsteadOfRoughness_; }
	bool GetEnableCoatingRoughnessImage() { return m_EnableCoatingRoughnessImage_; }
	std::string GetCoatingRoughnessPath() { return m_CoatingRoughnessPath_; }
	glm::vec4 GetCoatingRoughness() { return m_CoatingRoughness_; }

	// Coating thickness
	bool GetCoatingImageThickness() { return m_CoatingImageThickness_; }
	bool GetUseTextureCoatingThicknessInsteadOfThickness() { return m_UseTextureCoatingThicknessInsteadOfThickness_; }
	bool GetEnableCoatingThicknessImage() { return m_EnableCoatingThicknessImage_; }
	std::string GetCoatingThicknessPath() { return m_CoatingThicknessPath_; }
	glm::vec4 GetCoatingThickness() { return m_CoatingThickness_; }

	// Coating transmission color
	bool GetCoatingImageTransmissionColor() { return m_CoatingImageTransmissionColor_; }
	bool GetUseTextureCoatingTransmissionColorInsteadOfTransmissionColor() { return m_UseTextureCoatingTransmissionColorInsteadOfTransmissionColor_; }
	bool GetEnableCoatingTransmissionColorImage() { return m_EnableCoatingTransmissionColorImage_; }
	std::string GetCoatingTransmissionColorPath() { return m_CoatingTransmissionColorPath_; }
	glm::vec4 GetCoatingTransmissionColor() { return m_CoatingTransmissionColor_; }

	// Coating Ior
	float GetCoatingIor() { return m_CoatingIor_; }
	CoatingType GetCoatingMode() { return m_CoatingMode_; }

	// Coating metalness
	bool GetCoatingImageMetalness() { return m_CoatingImageMetalness_; }
	bool GetUseTextureCoatingMetalnessInsteadOfMetalness() { return m_UseTextureCoatingMetalnessInsteadOfMetalness_; }
	bool GetEnableCoatingMetalnessImage() { return m_EnableCoatingMetalnessImage_; }
	std::string GetCoatingMetalnessPath() { return m_CoatingMetalnessPath_; }
	glm::vec4 GetCoatingMetalness() { return m_CoatingMetalness_; }

	// Other Section --------------------------------------------------------------------------------//
	// Normal map
	bool GetNormalMapImage() { return m_NormalMapImage_; }
	bool GetUseTextureNormalMapInsteadOfNormal() { return m_UseTextureNormalMapInsteadOfNormal_; }
	bool GetEnableNormalMapImage() { return m_EnableNormalMapImage_; }
	std::string GetNormalMapPath() { return m_NormalMapPath_; }
	glm::vec4 GetNormalMap() { return m_NormalMap_; }

	// Normal map weight
	glm::vec4 GetNormalMapWeight() { return m_NormalMapWeight_; }

	// Displacement map
	bool GetDisplacementMapImage() { return m_DisplacementMapImage_; }
	bool GetUseTextureDisplacementMapInsteadOfDisplacement() { return m_UseTextureDisplacementMapInsteadOfDisplacementMap_; }
	bool GetEnableDisplacementMapImage() { return m_EnableDisplacementMapImage_; }
	std::string GetDisplacementMapPath() { return m_DisplacementMapPath_; }
	glm::vec4 GetDisplacementMap() { return m_DisplacementMap_; }

	// Displacement map weight
	glm::vec4 GetDisplacementMapWeight() { return m_DisplacementMapWeight_; }

	// Emissive
	bool GetEmissiveImage() { return m_EmissiveImage_; }
	bool GetUseTextureEmissiveInsteadOfEmissive() { return m_UseTextureEmissiveInsteadOfEmissive_; }
	bool GetEnableEmissiveImage() { return m_EnableEmissiveImage_; }
	std::string GetEmissivePath() { return m_EmissivePath_; }
	glm::vec4 GetEmissive() { return m_Emissive_; }

	// Emission weight
	bool GetEmissionWeightImage() { return m_EmissionWeightImage_; }
	bool GetUseTextureEmissionWeightInsteadOfEmissionWeight() { return m_UseTextureEmissionWeightInsteadOfEmissionWeight_; }
	bool GetEnableEmissionWeightImage() { return m_EnableEmissionWeightImage_; }
	std::string GetEmissionWeightPath() { return m_EmissionWeightPath_; }
	glm::vec4 GetEmissionWeight() { return m_EmissionWeight_; }

	// Opacity
	bool GetOpacityImage() { return m_OpacityImage_; }
	bool GetUseTextureOpacityInsteadOfOpacity() { return m_UseTextureOpacityInsteadOfOpacity_; }
	bool GetEnableOpacityImage() { return m_EnableOpacityImage_; }
	std::string GetOpacityPath() { return m_OpacityPath_; }
	glm::vec4 GetOpacity() { return m_Opacity_; }



	// Setters
	// Base color section --------------------------------------------------------------------------------//
	// Base color
	void SetBaseColorImageColor(bool value) { m_BaseColorImageColor_ = value; }
	void SetUseTextureBaseColorInsteadOfColor(bool value) { m_UseTextureBaseColorInsteadOfColor_ = value; }
	void SetEnableBaseColorImage(bool value) { m_EnableBaseColorImage_ = value; }
	void SetBaseColor(const std::string& TexturePath);
	void SetBaseColor(glm::vec4 Color);

	// Base color weight
	void SetBaseColorWeightImage(bool value) { m_BaseColorImageWeight_ = value; }
	void SetUseTextureBaseColorWeightInsteadOfWeight(bool value) { m_UseTextureBaseColorWeightInsteadOfWeight_ = value; }
	void SetEnableBaseColorWeightImage(bool value) { m_EnableBaseColorWeightImage_ = value; }
	void SetBaseColorWeight(glm::vec4 Color);
	void SetBaseColorWeight(const std::string& texturePath);

	// Base color roughness
	void SetBaseColorRoughnessImage(bool value) { m_BaseColorImageRoughness_ = value; }
	void SetUseTextureBaseColorRoughnessInsteadOfRoughness(bool value) { m_UseTextureBaseColorRoughnessInsteadOfRoughness_ = value; }
	void SetEnableBaseColorRoughnessImage(bool value) { m_EnableBaseColorRoughnessImage_ = value; }
	void SetBaseColorRoughness(glm::vec4 Color);
	void SetBaseColorRoughness(const std::string& texturePath);

	// Backscatter weight
	void SetBackscatterImageWeight(bool value) { m_BackscatterImageWeight_ = value; }
	void SetUseTextureBackscatterWeightInsteadOfWeight(bool value) { m_UseTextureBackscatterWeightInsteadOfWeight_ = value; }
	void SetEnableBackscatterWeightImage(bool value) { m_EnableBackscatterWeightImage_ = value; }
	void SetBackscatterWeight(glm::vec4 color);
	void SetBackscatterWeight(const std::string& texturePath);

	// Backscatter color
	void SetBackscatterImageColor(bool value) { m_BackscatterImageColor_ = value; }
	void SetUseTextureBackscatterColorInsteadOfColor(bool value) { m_UseTextureBackscatterColorInsteadOfColor_ = value; }
	void SetEnableBackscatterColorImage(bool value) { m_EnableBackscatterColorImage_ = value; }
	void SetBackscatterColor(glm::vec4 color);
	void SetBackscatterColor(const std::string& texturePath);

	// Reflection secion --------------------------------------------------------------------------------//
		// Reflection color
	void SetReflectionImageColor(bool value) { m_ReflectionImageColor_ = value; }
	void SetUseTextureReflectionColorInsteadOfColor(bool value) { m_UseTextureReflectionColorInsteadOfColor_ = value; }
	void SetEnableReflectionColorImage(bool value) { m_EnableReflectionColorImage_ = value; }
	void SetReflectionColor(glm::vec4 color);
	void SetReflectionColor(const std::string& texturePath);

	// Reflection weight
	void SetReflectionImageWeight(bool value) { m_ReflectionImageWeight_ = value; }
	void SetUseTextureReflectionWeightInsteadOfWeight(bool value) { m_UseTextureReflectionWeightInsteadOfWeight_ = value; }
	void SetEnableReflectionWeightImage(bool value) { m_EnableReflectionWeightImage_ = value; }
	void SetReflectionWeight(glm::vec4 Color);
	void SetReflectionWeight(const std::string& texturePath);

	// Reflection roughness
	void SetReflectionImageRoughness(bool value) { m_ReflectionImageRoughness_ = value; }
	void SetUseTextureReflectionRoughnessInsteadOfRoughness(bool value) { m_UseTextureReflectionRoughnessInsteadOfRoughness_ = value; }
	void SetEnableReflectionRoughnessImage(bool value) { m_EnableReflectionRoughnessImage_ = value; }
	void SetReflectionRoughness(glm::vec4 color);
	void SetReflectionRoughness(const std::string& TexturePath);

	// Reflection anisotropy
	void SetReflectionImageAnisotropy(bool value) { m_ReflectionAnisotropyImage_ = value; }
	void SetUseTextureReflectionAnisotropyInsteadOfAnisotropy(bool value) { m_UseTextureReflectionAnisotropyInsteadOfAnisotropy_ = value; }
	void SetEnableReflectionAnisotropyImage(bool value) { m_EnableReflectionAnisotropyImage_ = value; }
	void SetReflectionAnisotropy(glm::vec4 color);
	void SetReflectionAnisotropy(const std::string& texturePath);

	// Reflection anisotropy rotation
	void SetReflectionImageAnisotropyRotation(bool value) { m_ReflectionAnisotropyRotationImage_ = value; }
	void SetUseTextureReflectionAnisotropyRotationInsteadOfAnisotropyRotation(bool value) { m_UseTextureReflectionAnisotropyRotationInsteadOfAnisotropyRotation_ = value; }
	void SetEnableReflectionAnisotropyRotationImage(bool value) { m_EnableReflectionAnisotropyRotationImage_ = value; }
	void SetReflectionAnisotropyRotation(glm::vec4 color);
	void SetReflectionAnisotropyRotation(const std::string& texturePath);

	void SetReflectionMode(ReflectionType = ReflectionTypePBR);
	void SetIor(float ior) { m_Ior_ = ior; }

	// Reflection Metalness
	void SetReflectionImageMetalness(bool value) { m_ReflectionImageMetalness_ = value; }
	void SetUseTextureReflectionMetalnessInsteadOfMetalness(bool value) { m_UseTextureReflectionMetalnessInsteadOfMetalness_ = value; }
	void SetEnableReflectionMetalnessImage(bool value) { m_EnableReflectionMetalnessImage_ = value; }
	void SetReflectionMetalness(glm::vec4 color);
	void SetReflectionMetalness(const std::string& texturePath);

	// Sheen Section --------------------------------------------------------------------------------//
// Sheen color
	void SetSheenImageColor(bool value) { m_SheenImageColor_ = value; }
	void SetUseTextureSheenColorInsteadOfColor(bool value) { m_UseTextureSheenColorInsteadOfColor_ = value; }
	void SetEnableSheenColorImage(bool value) { m_EnableSheenColorImage_ = value; }
	void SetSheenColor(glm::vec4 color);
	void SetSheenColor(const std::string& texturePath);

	// Sheen weight
	void SetSheenImageWeight(bool value) { m_SheenImageWeight_ = value; }
	void SetUseTextureSheenWeightInsteadOfWeight(bool value) { m_UseTextureSheenWeightInsteadOfWeight_ = value; }
	void SetEnableSheenWeightImage(bool value) { m_EnableSheenWeightImage_ = value; }
	void SetSheenTint(glm::vec4 color);
	void SetSheenTint(const std::string& texturePath);

	// Sheen tint
	void SetSheenImageTint(bool value) { m_SheenImageTint_ = value; }
	void SetUseTextureSheenTintInsteadOfTint(bool value) { m_UseTextureSheenTintInsteadOfTint_ = value; }
	void SetEnableSheenTintImage(bool value) { m_EnableSheenTintImage_ = value; }
	void SetSheenWeight(glm::vec4 weight);
	void SetSheenWeight(const std::string& texturePath);

	// Refraction Section --------------------------------------------------------------------------------//
// Refraction color
	void SetRefractionImageColor(bool value) { m_RefractionImageColor_ = value; }
	void SetUseTextureRefractionColorInsteadOfColor(bool value) { m_UseTextureRefractionColorInsteadOfColor_ = value; }
	void SetEnableRefractionColorImage(bool value) { m_EnableRefractionColorImage_ = value; }
	void SetRefractionColor(glm::vec4 color);
	void SetRefractionColor(const std::string& texturePath);

	// Refraction weight
	void SetRefractionImageWeight(bool value) { m_RefractionImageWeight_ = value; }
	void SetUseTextureRefractionWeightInsteadOfWeight(bool value) { m_UseTextureRefractionWeightInsteadOfWeight_ = value; }
	void SetEnableRefractionWeightImage(bool value) { m_EnableRefractionWeightImage_ = value; }
	void SetRefractionWeight(glm::vec4 color);
	void SetRefractionWeight(const std::string& texturePath);

	// Refraction Normal Map
	void SetRefractionImageNormal(bool value) { m_RefractionImageNormal_ = value; }
	void SetUseTextureRefractionNormalInsteadOfNormal(bool value) { m_UseTextureRefractionNormalInsteadOfNormal_ = value; }
	void SetEnableRefractionNormalImage(bool value) { m_EnableRefractionNormalImage_ = value; }
	void SetRefractionNormal(glm::vec4 color);
	void SetRefractionNormal(const std::string& texturePath);

	// refraction Normal Map Weight
	void SetRefractionNormalWeight(float weight) { m_RefractionNormalWeight_ = weight; }

	// Refraction roughness
	void SetRefractionImageRoughness(bool value) { m_RefractionImageRoughness_ = value; }
	void SetUseTextureRefractionRoughnessInsteadOfRoughness(bool value) { m_UseTextureRefractionRoughnessInsteadOfRoughness_ = value; }
	void SetEnableRefractionRoughnessImage(bool value) { m_EnableRefractionRoughnessImage_ = value; }
	void SetRefractionRoughness(glm::vec4 color);
	void SetRefractionRoughness(const std::string& texturePath);

	void SetRefractionIor(float ior) { m_RefractionIor_ = ior; }

	void SetRefractionThinSurface(bool value) { m_RefractionThinSurface_ = value; }

	// Refraction Absorption
	void SetRefractionImageAbsorptionColor(bool value) { m_RefractionImageAbsorptionColor_ = value; }
	void SetUseTextureRefractionAbsorptionColorInsteadOfAbsorptionColor(bool value) { m_UseTextureRefractionAbsorptionColorInsteadOfAbsorptionColor_ = value; }
	void SetEnableRefractionAbsorptionColorImage(bool value) { m_EnableRefractionAbsorptionColorImage_ = value; }
	void SetRefractionAbsorptionColor(glm::vec4 color);
	void SetRefractionAbsorptionColor(const std::string& texturePath);

	// Refraction Absorption distance
	void SetRefractionImageAbsorptionDistance(bool value) { m_RefractionImageAbsorptionDistance_ = value; }
	void SetUseTextureRefractionAbsorptionDistanceInsteadOfAbsorptionDistance(bool value) { m_UseTextureRefractionAbsorptionDistanceInsteadOfAbsorptionDistance_ = value; }
	void SetEnableRefractionAbsorptionDistanceImage(bool value) { m_EnableRefractionAbsorptionDistanceImage_ = value; }
	void SetRefractionAbsorptionDistance(glm::vec4 color);
	void SetRefractionAbsorptionDistance(const std::string& texturePath);

	void SetRefractionCaustics(bool value) { m_RefractionCaustics_ = value; }

	// SSS Section --------------------------------------------------------------------------------//
// SSS scatter color
	void SetSssImageScatterColor(bool value) { m_SssImageScatterColor_ = value; }
	void SetUseTextureSssScatterColorInsteadOfScatterColor(bool value) { m_UseTextureSssScatterColorInsteadOfScatterColor_ = value; }
	void SetEnableSssScatterColorImage(bool value) { m_EnableSssScatterColorImage_ = value; }
	void SetSssScatterColor(glm::vec4 color);
	void SetSssScatterColor(const std::string& texturePath);

	// SSS scatter weight
	void SetSssImageScatterWeight(bool value) { m_SssImageScatterWeight_ = value; }
	void SetUseTextureSssScatterWeightInsteadOfScatterWeight(bool value) { m_UseTextureSssScatterWeightInsteadOfScatterWeight_ = value; }
	void SetEnableSssScatterWeightImage(bool value) { m_EnableSssScatterWeightImage_ = value; }
	void SetSssScatterWeight(glm::vec4 color);
	void SetSssScatterWeight(const std::string& texturePath);

	// SSS scatter distance
	void SetSssImageScatterDistance(bool value) { m_SssImageScatterDistance_ = value; }
	void SetUseTextureSssScatterDistanceInsteadOfScatterDistance(bool value) { m_UseTextureSssScatterDistanceInsteadOfScatterDistance_ = value; }
	void SetEnableSssScatterDistanceImage(bool value) { m_EnableSssScatterDistanceImage_ = value; }
	void SetSssScatterDistance(glm::vec4 color);
	void SetSssScatterDistance(const std::string& texturePath);

	// SSS scatter direction
	void SetSssImageScatterDirection(bool value) { m_SssImageScatterDirection_ = value; }
	void SetUseTextureSssScatterDirectionInsteadOfScatterDirection(bool value) { m_UseTextureSssScatterDirectionInsteadOfScatterDirection_ = value; }
	void SetEnableSssScatterDirectionImage(bool value) { m_EnableSssScatterDirectionImage_ = value; }
	void SetSssScatterDirection(glm::vec4 color);
	void SetSssScatterDirection(const std::string& texturePath);

	void SetSssMultiScattering(bool value) { m_SssUseMultiScattering_ = value; }
	void SetSssUseSchlickApproximation(bool value) { m_SssUseSchlickApproximation_ = value; }

	// Coating Section --------------------------------------------------------------------------------//
// Coating color
	void SetCoatingImageColor(bool value) { m_CoatingImageColor_ = value; }
	void SetUseTextureCoatingColorInsteadOfColor(bool value) { m_UseTextureCoatingColorInsteadOfColor_ = value; }
	void SetEnableCoatingColorImage(bool value) { m_EnableCoatingColorImage_ = value; }
	void SetCoatingColor(glm::vec4 color);
	void SetCoatingColor(const std::string& texturePath);

	// Coating weight
	void SetCoatingImageWeight(bool value) { m_CoatingImageWeight_ = value; }
	void SetUseTextureCoatingWeightInsteadOfWeight(bool value) { m_UseTextureCoatingWeightInsteadOfWeight_ = value; }
	void SetEnableCoatingWeightImage(bool value) { m_EnableCoatingWeightImage_ = value; }
	void SetCoatingWeight(glm::vec4 color);
	void SetCoatingWeight(const std::string& texturePath);

	// Coating roughness
	void SetCoatingImageRoughness(bool value) { m_CoatingImageRoughness_ = value; }
	void SetUseTextureCoatingRoughnessInsteadOfRoughness(bool value) { m_UseTextureCoatingRoughnessInsteadOfRoughness_ = value; }
	void SetEnableCoatingRoughnessImage(bool value) { m_EnableCoatingRoughnessImage_ = value; }
	void SetCoatingRoughness(glm::vec4 color);
	void SetCoatingRoughness(const std::string& texturePath);

	void SetCoatingMode(CoatingType mode) { m_CoatingMode_ = mode; }

	// Coating normal
	void SetCoatingImageNormal(bool value) { m_CoatingImageNormal_ = value; }
	void SetUseTextureCoatingNormalInsteadOfNormal(bool value) { m_UseTextureCoatingNormalInsteadOfNormal_ = value; }
	void SetEnableCoatingNormalImage(bool value) { m_EnableCoatingNormalImage_ = value; }
	void SetCoatingNormal(const std::string& texturePath);
	void SetCoatingNormal(glm::vec4 color);

	// Coainting normal weight
	void SetCoatingNormalWeight(float weight) { m_CoatingNormalWeight_ = weight; }

	void SetCoatingIor(float ior) { m_CoatingIor_ = ior; }

	// Coating thickness
	void SetCoatingImageThickness(bool value) { m_CoatingImageThickness_ = value; }
	void SetUseTextureCoatingThicknessInsteadOfThickness(bool value) { m_UseTextureCoatingThicknessInsteadOfThickness_ = value; }
	void SetEnableCoatingThicknessImage(bool value) { m_EnableCoatingThicknessImage_ = value; }
	void SetCoatingThickness(glm::vec4 color);
	void SetCoatingThickness(const std::string& texturePath);

	// Coating transmission color
	void SetCoatingImageTransmissionColor(bool value) { m_CoatingImageTransmissionColor_ = value; }
	void SetUseTextureCoatingTransmissionColorInsteadOfTransmissionColor(bool value) { m_UseTextureCoatingTransmissionColorInsteadOfTransmissionColor_ = value; }
	void SetEnableCoatingTransmissionColorImage(bool value) { m_EnableCoatingTransmissionColorImage_ = value; }
	void SetCoatingTransmissionColor(glm::vec4 color);
	void SetCoatingTransmissionColor(const std::string& texturePath);

	// Coating metalness
	void SetCoatingImageMetalness(bool value) { m_CoatingImageMetalness_ = value; }
	void SetUseTextureCoatingMetalnessInsteadOfMetalness(bool value) { m_UseTextureCoatingMetalnessInsteadOfMetalness_ = value; }
	void SetEnableCoatingMetalnessImage(bool value) { m_EnableCoatingMetalnessImage_ = value; }
	void SetCoatingMetalness(glm::vec4 color);
	void SetCoatingMetalness(const std::string& texturePath);

	// Other Section --------------------------------------------------------------------------------//
	// Normal map 
	void SetNormalMapImage(bool value) { m_NormalMapImage_ = value; }
	void SetUseTextureNormalMapInsteadOfNormal(bool value) { m_UseTextureNormalMapInsteadOfNormal_ = value; }
	void SetEnableNormalMapImage(bool value) { m_EnableNormalMapImage_ = value; }
	void SetNormalMap(const std::string& texturePath);
	void SetNormalMap(glm::vec4 color);

	// Normal map weight
	void SetNormalMapWeight(glm::vec4 weight);

	// Displacement map
	void SetDisplacementMapImage(bool value) { m_DisplacementMapImage_ = value; }
	void SetUseTextureDisplacementMapInsteadOfDisplacement(bool value) { m_UseTextureDisplacementMapInsteadOfDisplacementMap_ = value; }
	void SetEnableDisplacementMapImage(bool value) { m_EnableDisplacementMapImage_ = value; }
	void SetDisplacementMap(const std::string& texturePath);
	void SetDisplacementMap(glm::vec4 color);

	// Displacement map weight
	void SetDisplacementMapWeight(glm::vec4 weight) { m_DisplacementMapWeight_ = weight; } // TODO : Implement this later (replace by glm::vec4)

	// Emissive
	void SetEmissiveImage(bool value) { m_EmissiveImage_ = value; }
	void SetUseTextureEmissiveInsteadOfEmissive(bool value) { m_UseTextureEmissiveInsteadOfEmissive_ = value; }
	void SetEnableEmissiveImage(bool value) { m_EnableEmissiveImage_ = value; }
	void SetEmissive(const std::string& texturePath);
	void SetEmissive(glm::vec4 color);

	// Emission weight
	void SetEmissionWeightImage(bool value) { m_EmissionWeightImage_ = value; }
	void SetUseTextureEmissionWeightInsteadOfEmissionWeight(bool value) { m_UseTextureEmissionWeightInsteadOfEmissionWeight_ = value; }
	void SetEnableEmissionWeightImage(bool value) { m_EnableEmissionWeightImage_ = value; }
	void SetEmissionWeight(const std::string& texturePath);
	void SetEmissionWeight(glm::vec4 color);

	// Opacity
	void SetOpacityImage(bool value) { m_OpacityImage_ = value; }
	void SetUseTextureOpacityInsteadOfOpacity(bool value) { m_UseTextureOpacityInsteadOfOpacity_ = value; }
	void SetEnableOpacityImage(bool value) { m_EnableOpacityImage_ = value; }
	void SetOpacity(const std::string& texturePath);
	void SetOpacity(glm::vec4 color);

	// Transparency
	void SetTransparency(glm::vec4 color);
	void SetTransparency(const std::string& texturePath);



	// Other ----------------------------------------

	void SetBackgroundImage(const std::string& texturePath);
	void UnsetBackgroundImage();

	rpr_material_node GetMaterial()
	{
		return m_Material_;
	}

	// ----------------------------------------------
	// Texture object -------------------------------

	rpr_image LoadTexture(std::string Path);

	// ----------------------------------------------


private:

	bool m_IsMaterialCreated_ = false;

	rpr_material_node m_Material_ = nullptr;

	// Base color secion --------------------------------------------------------------------------------//
	// Base Color
	bool m_BaseColorImageColor_ = false;
	bool m_UseTextureBaseColorInsteadOfColor_ = false;
	bool m_EnableBaseColorImage_ = false;
	std::string m_BaseColorPath_ = "path/to/BaseColor.jpg";
	glm::vec4 m_BaseColor_ = { 0.5f, 0.5f, 0.5f,1.0f };

	// Base color weight
	bool m_BaseColorImageWeight_ = false;
	bool m_UseTextureBaseColorWeightInsteadOfWeight_ = false;
	bool m_EnableBaseColorWeightImage_ = false;
	std::string m_BaseColorWeightPath_ = "path/to/BaseColorWeight.jpg";
	glm::vec4 m_BaseColorWeight_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	// Base color roughness
	bool m_BaseColorImageRoughness_ = false;
	bool m_UseTextureBaseColorRoughnessInsteadOfRoughness_ = false;
	bool m_EnableBaseColorRoughnessImage_ = false;
	std::string m_BaseColorRoughnessPath_ = "path/to/BaseColorRoughness.jpg";
	glm::vec4 m_BaseColorRoughness_ = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Backscatter
	bool m_BackscatterImageWeight_ = false;
	bool m_UseTextureBackscatterWeightInsteadOfWeight_ = false;
	bool m_EnableBackscatterWeightImage_ = false;
	std::string m_BackscatterWeightPath_ = "path/to/BackscatterWeight.jpg";
	glm::vec4 m_BackscatterWeight_ = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Backscatter color
	bool m_BackscatterImageColor_ = false;
	bool m_UseTextureBackscatterColorInsteadOfColor_ = false;
	bool m_EnableBackscatterColorImage_ = false;
	std::string m_BackscatterColorPath_ = "path/to/BackscatterColor.jpg";
	glm::vec4 m_BackscatterColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	// Reflection secion --------------------------------------------------------------------------------//
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
	bool m_ReflectionAnisotropyImage_ = false;
	bool m_UseTextureReflectionAnisotropyInsteadOfAnisotropy_ = false;
	bool m_EnableReflectionAnisotropyImage_ = false;
	std::string m_ReflectionAnisotropyPath_ = "path/to/ReflectionAnisotropy.jpg";
	glm::vec4 m_ReflectionAnisotropy_ = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Reflection anisotropy rotation
	bool m_ReflectionAnisotropyRotationImage_ = false;
	bool m_UseTextureReflectionAnisotropyRotationInsteadOfAnisotropyRotation_ = false;
	bool m_EnableReflectionAnisotropyRotationImage_ = false;
	std::string m_ReflectionAnisotropyRotationPath_ = "path/to/ReflectionAnisotropyRotation.jpg";
	glm::vec4 m_ReflectionAnisotropyRotation_ = { 0.0f, 0.0f, 0.0f, 0.0f };

	ReflectionType m_ReflectionMode_ = ReflectionTypeMetalness;
	float m_Ior_ = 1.36f;

	// Reflection Metalness
	bool m_ReflectionImageMetalness_ = false;
	bool m_UseTextureReflectionMetalnessInsteadOfMetalness_ = false;
	bool m_EnableReflectionMetalnessImage_ = false;
	std::string m_ReflectionMetalnessPath_ = "path/to/ReflectionMetalness.jpg";
	glm::vec4 m_ReflectionMetalness_ = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Sheen secion --------------------------------------------------------------------------------//
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

	// Sheen tint
	bool m_SheenImageTint_ = false;
	bool m_UseTextureSheenTintInsteadOfTint_ = false;
	bool m_EnableSheenTintImage_ = false;
	std::string m_SheenTintPath_ = "path/to/SheenTint.jpg";
	glm::vec4 m_SheenTint_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	// Refraction secion --------------------------------------------------------------------------------//
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
	bool m_RefractionImageNormal_ = false;
	bool m_UseTextureRefractionNormalInsteadOfNormal_ = false;
	bool m_EnableRefractionNormalImage_ = false;
	std::string m_RefractionNormalPath_ = "path/to/RefractionNormal.jpg";
	glm::vec4 m_RefractionNormal_ = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Refraction Normal Map Weight
	float m_RefractionNormalWeight_ = 0.0f;

	// Refraction roughness
	bool m_RefractionImageRoughness_ = false;
	bool m_UseTextureRefractionRoughnessInsteadOfRoughness_ = false;
	bool m_EnableRefractionRoughnessImage_ = false;
	std::string m_RefractionRoughnessPath_ = "path/to/RefractionRoughness.jpg";
	glm::vec4 m_RefractionRoughness_ = { 0.0f, 0.0f, 0.0f, 0.0f };

	float m_RefractionIor_ = 1.5f;
	bool m_RefractionThinSurface_ = false;

	// Refraction Absorption
	bool m_RefractionImageAbsorptionColor_ = false;
	bool m_UseTextureRefractionAbsorptionColorInsteadOfAbsorptionColor_ = false;
	bool m_EnableRefractionAbsorptionColorImage_ = false;
	std::string m_RefractionAbsorptionColorPath_ = "path/to/RefractionAbsorptionColor.jpg";
	glm::vec4 m_RefractionAbsorptionColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	// Refraction Absorption distance
	bool m_RefractionImageAbsorptionDistance_ = false;
	bool m_UseTextureRefractionAbsorptionDistanceInsteadOfAbsorptionDistance_ = false;
	bool m_EnableRefractionAbsorptionDistanceImage_ = false;
	std::string m_RefractionAbsorptionDistancePath_ = "path/to/RefractionAbsorptionDistance.jpg";
	glm::vec4 m_RefractionAbsorptionDistance_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	bool m_RefractionCaustics_ = false;

	// SSS secion --------------------------------------------------------------------------------//
	// SSS scatter color
	bool m_SssImageScatterColor_ = false;
	bool m_UseTextureSssScatterColorInsteadOfScatterColor_ = false;
	bool m_EnableSssScatterColorImage_ = false;
	std::string m_SssScatterColorPath_ = "path/to/SssScatterColor.jpg";
	glm::vec4 m_SssScatterColor_ = { 1 , 1 , 1 , 1 };

	// SSS scatter weight
	bool m_SssImageScatterWeight_ = false;
	bool m_UseTextureSssScatterWeightInsteadOfScatterWeight_ = false;
	bool m_EnableSssScatterWeightImage_ = false;
	std::string m_SssScatterWeightPath_ = "path/to/SssScatterWeight.jpg";
	glm::vec4 m_SssScatterWeight_ = { 0.0f, 0.0f, 0.0f, 0.0f };

	// SSS scatter distance
	bool m_SssImageScatterDistance_ = false;
	bool m_UseTextureSssScatterDistanceInsteadOfScatterDistance_ = false;
	bool m_EnableSssScatterDistanceImage_ = false;
	std::string m_SssScatterDistancePath_ = "path/to/SssScatterDistance.jpg";
	glm::vec4 m_SssScatterDistance_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	// SSS scatter direction
	bool m_SssImageScatterDirection_ = false;
	bool m_UseTextureSssScatterDirectionInsteadOfScatterDirection_ = false;
	bool m_EnableSssScatterDirectionImage_ = false;
	std::string m_SssScatterDirectionPath_ = "path/to/SssScatterDirection.jpg";
	glm::vec4 m_SssScatterDirection_ = { 0.0f, 0.0f, 0.0f, 0.0f };

	bool m_SssUseMultiScattering_ = false;
	bool m_SssUseSchlickApproximation_ = true;

	// Coating secion --------------------------------------------------------------------------------//
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

	// Coating Normal Map
	bool m_CoatingImageNormal_ = false;
	bool m_UseTextureCoatingNormalInsteadOfNormal_ = false;
	bool m_EnableCoatingNormalImage_ = false;
	std::string m_CoatingNormalPath_ = "path/to/CoatingNormal.jpg";
	glm::vec4 m_CoatingNormal_ = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Coating Normal Map Weight
	float m_CoatingNormalWeight_ = 0.0f;

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

	// Coating transmission color
	bool m_CoatingImageTransmissionColor_ = false;
	bool m_UseTextureCoatingTransmissionColorInsteadOfTransmissionColor_ = false;
	bool m_EnableCoatingTransmissionColorImage_ = false;
	std::string m_CoatingTransmissionColorPath_ = "path/to/CoatingTransmissionColor.jpg";
	glm::vec4 m_CoatingTransmissionColor_ = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Coating metalness
	bool m_CoatingImageMetalness_ = false;
	bool m_UseTextureCoatingMetalnessInsteadOfMetalness_ = false;
	bool m_EnableCoatingMetalnessImage_ = false;
	std::string m_CoatingMetalnessPath_ = "path/to/CoatingMetalness.jpg";
	glm::vec4 m_CoatingMetalness_ = { 0.0f, 0.0f, 0.0f, 0.0f };
	CoatingType m_CoatingMode_ = CoatingTypeMetalness;

	// Other secion --------------------------------------------------------------------------------//
	// Normal map
	bool m_NormalMapImage_ = false;
	bool m_UseTextureNormalMapInsteadOfNormal_ = false;
	bool m_EnableNormalMapImage_ = false;
	rpr_material_node m_NormalMapImageData_ = nullptr;
	std::string m_NormalMapPath_ = "path/to/NormalMap.jpg";
	glm::vec4 m_NormalMap_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	// Normal map weight
	bool m_UseNormalMapWeightImage_ = false;
	glm::vec4 m_NormalMapWeight_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	// Displacement map
	bool m_DisplacementMapImage_ = false;
	bool m_UseTextureDisplacementMapInsteadOfDisplacementMap_ = false;
	bool m_EnableDisplacementMapImage_ = false;
	std::string m_DisplacementMapPath_ = "path/to/DisplacementMap.jpg";
	glm::vec4 m_DisplacementMap_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	// Displacement map weight
	glm::vec4 m_DisplacementMapWeight_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	// Emissive
	bool m_EmissiveImage_ = false;
	bool m_UseTextureEmissiveInsteadOfEmissive_ = false;
	bool m_EnableEmissiveImage_ = false;
	std::string m_EmissivePath_ = "path/to/Emissive.jpg";
	glm::vec4 m_Emissive_ = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Emission weight
	bool m_EmissionWeightImage_ = false;
	bool m_UseTextureEmissionWeightInsteadOfEmissionWeight_ = false;
	bool m_EnableEmissionWeightImage_ = false;
	std::string m_EmissionWeightPath_ = "path/to/EmissionWeight.jpg";
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







};
