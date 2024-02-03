#pragma once
#pragma warning(disable : 4996)
#pragma warning(disable : 4244)
#pragma warning(disable : 4267)
#pragma warning(disable : 4005)

#include "hrs_opengl_camera.h" // nothing -> Maybe glad later
#include "hrs_scene.h" // nothing
#include "hrs_radeon_camera.h"// nothing
#include "hrs_importer.h" // nothing
#include "hrs_mesh.h" // nothing
#include "hrs_light.h" // nothing
#include "hrs_material.h" // nothing
#include "hrs_material_editor.h" // nothing

#include <string>
#include <queue>
#include <map>
#include <ranges>


class IDManager
{
public:
	static IDManager& GetInstance()
	{
		static IDManager Instance;
		return Instance;
	}

	IDManager(const IDManager&) = delete;
	IDManager& operator=(const IDManager&) = delete;

	int GetNewId()
	{
		if (!m_AvailableIDs_.empty())
		{
			int Id = m_AvailableIDs_.front();
			m_AvailableIDs_.pop();
			return Id;
		}
		return m_NextId_++;
	}

	void ReleaseId(int id)
	{
		m_AvailableIDs_.push(id);
	}

private:
	IDManager() : m_NextId_(0)
	{
	}

	int m_NextId_;
	std::queue<int> m_AvailableIDs_;
};

class HorusObjectManager
{
public:

	static HorusObjectManager& GetInstance()
	{
		static HorusObjectManager Instance;
		return Instance;
	}

	HorusObjectManager(const HorusObjectManager&) = delete;
	void operator=(const HorusObjectManager&) = delete;

	int GetIdByName(const std::string& Name)
	{
		if (auto It = m_ObjectNameToIdMap_.find(Name); It != m_ObjectNameToIdMap_.end())
		{
			return It->second;
		}

		return -1;
	}

	// General ---------------------------------------

	void SetActualSelectedId(int id)
	{
		m_ActualSelectedId_ = id;
	}

	// Other -----------------------------------------

	void SetBackgroundImage(const std::string& path);
	void UnsetBackgroundImage();

	// Outliner -------------------------------------

	void GetOutlinerGroupShapes(std::vector<std::string>& Meshes)
	{
		for (const auto& Key : m_GroupShape_ | std::views::keys)
		{
			int Id = Key;
			std::string Name = m_GroupShapeNames_[Id];
			Meshes.push_back(Name);
		}
	}
	void GetOutlinerMaterials(std::vector<std::string>& Materials)
	{
		for (const auto& Key : m_Materials_ | std::views::keys)
		{
			int Id = Key;
			std::string Name = m_MaterialNames_[Id];
			Materials.push_back(Name);
		}
	}
	void GetOutlinerCameras(std::vector<std::string>& Cameras)
	{
		for (const auto& Key : m_RadeonCameras_ | std::views::keys)
		{
			int Id = Key;
			std::string Name = m_RadeonCameraNames_[Id];
			Cameras.push_back(Name);
		}
	}
	void GetOutlinerLights(std::vector<std::string>& Lights)
	{
		for (const auto& Key : m_Lights_ | std::views::keys)
		{
			int Id = Key;
			std::string Name = m_LightNames_[Id];
			Lights.push_back(Name);
		}
	}

	// Camera object ----------------------------------

	void CameraExtractor(rpr_camera& Camera);

	int CreateOpenGlCamera(std::string Name); // One unique camera for OpenGL
	int CreateRadeonCamera(int SceneID, std::string Name);

	int CreateRadeonCameraFromGLTF(int SceneID, std::string Name, rpr_camera Camera);

	void TransfertDataBetwweenCamera(int id, int id2);

	void DestroyCamera(int id);
	void DestroyAllCameras()
	{
		for (auto& Val : m_RadeonCameras_ | std::views::values)
		{
			Val.Destroy();
		}
		m_RadeonCameras_.clear();
	}

	HorusRadeonCamera& GetRadeonCamera(int id) { return m_RadeonCameras_[id]; }
	HorusOpenGLCamera& GetOpenGlCamera(int id) { return m_OpenGlCameras_[id]; }

	void BindRadeonCamera(int id);
	void UnbindRadeonCamera(int id);

	void UpdateRadeonCamera(int id);
	void UpdateOpenGLCamera(int id);
	void SendToShaderOpenGLCamera(int id, const HorusShaderManager& Shader);
	void GetOpenGLCameraMatrices(int id, glm::mat4& projection, glm::mat4& view, glm::mat4& model);
	void GetMatrices(int id, glm::mat4& projection, glm::mat4& view, glm::mat4& model);

	void PrintCameraInfo(int id);
	void ResetCamera(int id);

	void SetViewport(int id, int x, int y, int width, int height);

	int GetActiveRadeonCameraId();
	int GetActiveOpenGLCameraId();
	void SetActiveRadeonCamera(int id);
	void SetActiveOpenGLCamera(int id);

	int GetCameraIdByName(const char* name);
	std::string& GetCameraNameById(int id);

	void SetMoveCameraForward(int id);
	void SetMoveCameraBackward(int id);
	void SetMoveCameraLeft(int id);
	void SetMoveCameraRight(int id);
	void SetMoveCameraUp(int id);
	void SetMoveCameraDown(int id);
	void SetScrollCamera(int id, float delta);

	void SetPitch(int id, float pitch);
	void SetHeading(int id, float heading);

	void SetCameraLookat(int id, glm::vec3& pivot);

	void SetTumbleCamera(int id, float x, float y, float sensitivity);
	void SetPanCamera(int id, float x, float y, float sensitivity);
	void SetZoomCamera(int id, float distance);

	// Getters for inspector

	glm::vec3 GetCameraLookAt(int id);
	glm::vec3 GetCameraPosition(int id);
	glm::vec3 GetCameraTranslation(int id);
	glm::vec3 GetCameraRotation(int id);
	glm::vec3 GetCameraScale(int id);
	float GetCameraFov(int id);
	float GetCameraAspectRatio(int id);
	float GetCameraNearPlane(int id);
	float GetCameraFarPlane(int id);
	float GetCameraFocusDistance(int id);
	float GetCameraFStop(int id);
	int GetCameraApertureBlades(int id);

	// Setters for inspector

	void SetCameraFov(int id, float fov);
	void SetCameraAspectRatio(int id, float aspect_ratio);
	void SetCameraNear(int id, float NearPlane);
	void SetCameraFar(int id, float FarPlane);
	void SetFocusDistance(int id, float focus_distance);
	void SetFStop(int id, float fstop);
	void SetApertureBlades(int id, int blades);

	void SetCameraLookAt(int id, glm::vec3 lookat);
	void SetCameraPosition(int id, glm::vec3 position);
	void SetCameraRotation(int id, glm::vec3 rotation_axis);
	void SetCameraScale(int id, glm::vec3 scale);

	// Group Shape object ----------------------------------

	int CreateGroupShape(const char* path, const std::string& name);

	void DestroyGroupShape(int id);
	void DestroyGroupShapeByName(const char* name);
	void DestroyAllGroupShape();

	// Getters for inspector
	HorusGroupShape& GetGroupShape(int id /*, HorusGroupShape* mesh*/);

	std::string& GetGroupShapeName(int id);
	void SetActiveGroupShape(int id);
	int GetActiveGroupShapeId();

	int GetGroupShapeCount(int count);
	int GetGroupShapeId(const char* name);
	void GetGroupShapeIdByIndex(int index, int* id);
	void GetGroupShapeIndexById(int id, int* index);
	void GetGroupShapeIndexByName(const char* name);

	std::map<std::string, std::vector<std::string>>& GetGroupShapeWithShapesAndNames();

	glm::mat4 GetGroupShapeTransform(int id);
	glm::vec3 GetGroupShapePosition(int id);
	glm::vec3 GetGroupShapeRotation(int id);
	glm::vec3 GetGroupShapeScale(int id);

	// Setters for inspector
	void SetGroupShapeName(int id, const char* name);
	void SetGroupShapeId(int id, int new_id);
	void SetGroupShapePosition(int id, glm::vec3 pos);
	void SetGroupShapeRotation(int id, glm::vec3 rot);
	void SetGroupShapeScale(int id, glm::vec3 scale);
	void SetGroupShapeResetTransform(int id);
	void UpdateGroupShapeOutlinerData();

	// Shape object ----------------------------------

	void SetShapeId(int id);
	int GetActiveShapeId();
	std::string GetShapeNameById(int id);
	void DeleteSelectedShape(int id);

	// Getters
	rpr_shape GetShapeById(int id);
	rpr_shape GetShapeFromGroup(int GroupId, const std::string& shapeName);

	glm::vec3 GetShapePositionById(int id);
	glm::vec3 GetShapeRotationById(int id);
	glm::vec3 GetShapeScaleById(int id);

	// Setters
	void UpdateShapeTransforms(rpr_shape shape);
	void SetActiveShapeId(int id) { m_ActiveShapeId_ = id; }
	void SetShapePositionById(int shapeId, const glm::vec3& newPosition);
	void SetShapeRotationById(int shapeId, const glm::vec3& newRotation);
	void SetShapeScaleById(int shapeId, const glm::vec3& newScale);
	void SetShapeResetTransformById(int shapeId);



	// Material object ------------------------------

	int GetActiveMaterialId();
	int GetMaterialIdByName(std::string Name);
	std::string& GetMaterialName(int Id);
	int CreateMaterial(std::string Name);

	HorusMaterial& GetMaterial(int Id);
	bool MaterialExists(std::string Name);

	void DestroyMaterial(int Id);
	void DestroyAllMaterial();

	// Getters for inspector

	// Base Color Section ---------------------------------------------------------------------//
	// Base Color
	bool GetBaseColorImageColor(int id) { return m_Materials_[id].GetBaseColorImageColor(); }
	bool GetUseTextureBaseColorInsteadOfColor(int id) { return m_Materials_[id].GetUseTextureBaseColorInsteadOfColor(); }
	bool GetEnableBaseColorImage(int id) { return m_Materials_[id].GetEnableBaseColorImage(); }
	std::string GetBaseColorTexturePath(int id) { return m_Materials_[id].GetBaseColorPath(); }
	glm::vec4 GetBaseColor(int id) { return m_Materials_[id].GetBaseColor(); }

	// Base Color Weight
	bool GetBaseColorWeightImageColor(int id) { return m_Materials_[id].GetBaseColorImageWeight(); }
	bool GetUseTextureBaseColorWeightInsteadOfColor(int id) { return m_Materials_[id].GetUseTextureBaseColorWeightInsteadOfWeight(); }
	bool GetEnableBaseColorWeightImage(int id) { return m_Materials_[id].GetEnableBaseColorWeightImage(); }
	std::string GetBaseColorWeightTexturePath(int id) { return m_Materials_[id].GetBaseColorWeightPath(); }
	glm::vec4 GetBaseColorWeight(int GetActiveMaterialId) { return m_Materials_[GetActiveMaterialId].GetBaseColorWeight(); }

	// Base Color Roughness
	bool GetBaseColorRoughnessImageColor(int id) { return m_Materials_[id].GetBaseColorImageRoughness(); }
	bool GetUseTextureBaseColorRoughnessInsteadOfColor(int id) { return m_Materials_[id].GetUseTextureBaseColorRoughnessInsteadOfRoughness(); }
	bool GetEnableBaseColorRoughnessImage(int id) { return m_Materials_[id].GetEnableBaseColorRoughnessImage(); }
	std::string GetBaseColorRoughnessTexturePath(int id) { return m_Materials_[id].GetBaseColorRoughnessPath(); }
	glm::vec4 GetBaseColorRoughness(int GetActiveMaterialId) { return m_Materials_[GetActiveMaterialId].GetBaseColorRoughness(); }

	// Backscatter
	bool GetBackscatterColorImageColor(int id) { return m_Materials_[id].GetBackscatterImageColor(); }
	bool GetUseTextureBackscatterColorInsteadOfColor(int id) { return m_Materials_[id].GetUseTextureBackscatterColorInsteadOfColor(); }
	bool GetEnableBackscatterColorImage(int id) { return m_Materials_[id].GetEnableBackscatterColorImage(); }
	std::string GetBackscatterColorTexturePath(int id) { return m_Materials_[id].GetBackscatterColorPath(); }
	glm::vec4 GetBackscatterColor(int id) { return m_Materials_[id].GetBackscatterColor(); }

	// Backscatter Weight
	bool GetBackscatterWeightImageColor(int id) { return m_Materials_[id].GetBackscatterImageWeight(); }
	bool GetUseTextureBackscatterWeightInsteadOfColor(int id) { return m_Materials_[id].GetUseTextureBackscatterWeightInsteadOfWeight(); }
	bool GetEnableBackscatterWeightImage(int id) { return m_Materials_[id].GetEnableBackscatterWeightImage(); }
	std::string GetBackscatterWeightTexturePath(int id) { return m_Materials_[id].GetBackscatterWeightPath(); }
	glm::vec4 GetBackscatterWeight(int id) { return m_Materials_[id].GetBackscatterWeight(); }

	// Reflection Section ---------------------------------------------------------------------//
	// Reflection Color
	bool GetReflectionColorImageColor(int id) { return m_Materials_[id].GetReflectionImageColor(); }
	bool GetUseTextureReflectionColorInsteadOfColor(int id) { return m_Materials_[id].GetUseTextureReflectionColorInsteadOfColor(); }
	bool GetEnableReflectionColorImage(int id) { return m_Materials_[id].GetEnableReflectionColorImage(); }
	std::string GetReflectionColorTexturePath(int id) { return m_Materials_[id].GetReflectionColorPath(); }
	glm::vec4 GetReflectionColor(int id) { return m_Materials_[id].GetReflectionColor(); }

	// Reflection Weight
	bool GetReflectionWeightImageColor(int id) { return m_Materials_[id].GetReflectionImageWeight(); }
	bool GetUseTextureReflectionWeightInsteadOfColor(int id) { return m_Materials_[id].GetUseTextureReflectionWeightInsteadOfWeight(); }
	bool GetEnableReflectionWeightImage(int id) { return m_Materials_[id].GetEnableReflectionWeightImage(); }
	std::string GetReflectionWeightTexturePath(int id) { return m_Materials_[id].GetReflectionWeightPath(); }
	glm::vec4 GetReflectionWeight(int id) { return m_Materials_[id].GetReflectionWeight(); }

	// Reflection Roughness
	bool GetReflectionRoughnessImageColor(int id) { return m_Materials_[id].GetReflectionImageRoughness(); }
	bool GetUseTextureReflectionRoughnessInsteadOfColor(int id) { return m_Materials_[id].GetUseTextureReflectionRoughnessInsteadOfRoughness(); }
	bool GetEnableReflectionRoughnessImage(int id) { return m_Materials_[id].GetEnableReflectionRoughnessImage(); }
	std::string GetReflectionRoughnessTexturePath(int id) { return m_Materials_[id].GetReflectionRoughnessPath(); }
	glm::vec4 GetReflectionRoughness(int id) { return m_Materials_[id].GetReflectionRoughness(); }

	// Reflection Anisotropy
	bool GetReflectionAnisotropyImageColor(int id) { return m_Materials_[id].GetReflectionImageAnisotropy(); }
	bool GetUseTextureReflectionAnisotropyInsteadOfColor(int id) { return m_Materials_[id].GetUseTextureReflectionAnisotropyInsteadOfAnisotropy(); }
	bool GetEnableReflectionAnisotropyImage(int id) { return m_Materials_[id].GetEnableReflectionAnisotropyImage(); }
	std::string GetReflectionAnisotropyPath(int id) { return m_Materials_[id].GetReflectionAnisotropyPath(); }
	glm::vec4 GetReflectionAnisotropy(int id) { return m_Materials_[id].GetReflectionAnisotropy(); }

	// Reflection Anisotropy Rotation
	bool GetReflectionAnisotropyRotationImageColor(int id) { return m_Materials_[id].GetReflectionImageAnisotropyRotation(); }
	bool GetUseTextureReflectionAnisotropyRotationInsteadOfColor(int id) { return m_Materials_[id].GetUseTextureReflectionAnisotropyRotationInsteadOfAnisotropyRotation(); }
	bool GetEnableReflectionAnisotropyRotationImage(int id) { return m_Materials_[id].GetEnableReflectionAnisotropyRotationImage(); }
	std::string GetReflectionAnisotropyRotationPath(int id) { return m_Materials_[id].GetReflectionAnisotropyRotationPath(); }
	glm::vec4 GetReflectionAnisotropyRotation(int id) { return m_Materials_[id].GetReflectionAnisotropyRotation(); }

	HorusMaterial::ReflectionType GetReflectionMode(int id) { return m_Materials_[id].GetReflectionMode(); }
	float GetIor(int id) { return m_Materials_[id].GetIor(); }

	// Reflection Metalness
	bool GetReflectionMetalnessImageColor(int id) { return m_Materials_[id].GetReflectionImageMetalness(); }
	bool GetUseTextureReflectionMetalnessInsteadOfColor(int id) { return m_Materials_[id].GetUseTextureReflectionMetalnessInsteadOfMetalness(); }
	bool GetEnableReflectionMetalnessImage(int id) { return m_Materials_[id].GetEnableReflectionMetalnessImage(); }
	std::string GetReflectionMetalnessTexturePath(int id) { return m_Materials_[id].GetReflectionMetalnessPath(); }
	glm::vec4 GetReflectionMetalness(int id) { return m_Materials_[id].GetReflectionMetalness(); }

	// Sheen Section ---------------------------------------------------------------------//
// Sheen Color
	bool GetSheenColorImageColor(int id) { return m_Materials_[id].GetSheenImageColor(); }
	bool GetUseTextureSheenColorInsteadOfColor(int id) { return m_Materials_[id].GetUseTextureSheenColorInsteadOfColor(); }
	bool GetEnableSheenColorImage(int id) { return m_Materials_[id].GetEnableSheenColorImage(); }
	std::string GetSheenColorTexturePath(int id) { return m_Materials_[id].GetSheenColorPath(); }
	glm::vec4 GetSheenColor(int id) { return m_Materials_[id].GetSheenColor(); }

	// Sheen Weight
	bool GetSheenWeightImageColor(int id) { return m_Materials_[id].GetSheenImageWeight(); }
	bool GetUseTextureSheenWeightInsteadOfColor(int id) { return m_Materials_[id].GetUseTextureSheenWeightInsteadOfWeight(); }
	bool GetEnableSheenWeightImage(int id) { return m_Materials_[id].GetEnableSheenWeightImage(); }
	std::string GetSheenWeightTexturePath(int id) { return m_Materials_[id].GetSheenWeightPath(); }
	glm::vec4 GetSheenWeight(int id) { return m_Materials_[id].GetSheenWeight(); }

	// Sheen Tint
	bool GetSheenTintImageColor(int id) { return m_Materials_[id].GetSheenImageTint(); }
	bool GetUseTextureSheenTintInsteadOfColor(int id) { return m_Materials_[id].GetUseTextureSheenTintInsteadOfTint(); }
	bool GetEnableSheenTintImage(int id) { return m_Materials_[id].GetEnableSheenTintImage(); }
	std::string GetSheenTintTexturePath(int id) { return m_Materials_[id].GetSheenTintPath(); }
	glm::vec4 GetSheenTint(int id) { return m_Materials_[id].GetSheenTint(); }

	// Refraction Section ---------------------------------------------------------------------//
// Refraction Color
	bool GetRefractionColorImageColor(int id) { return m_Materials_[id].GetRefractionImageColor(); }
	bool GetUseTextureRefractionColorInsteadOfColor(int id) { return m_Materials_[id].GetUseTextureRefractionColorInsteadOfColor(); }
	bool GetEnableRefractionColorImage(int id) { return m_Materials_[id].GetEnableRefractionColorImage(); }
	std::string GetRefractionColorTexturePath(int id) { return m_Materials_[id].GetRefractionColorPath(); }
	glm::vec4 GetRefractionColor(int id) { return m_Materials_[id].GetRefractionColor(); }

	// Refraction Weight
	bool GetRefractionWeightImageColor(int id) { return m_Materials_[id].GetRefractionImageWeight(); }
	bool GetUseTextureRefractionWeightInsteadOfColor(int id) { return m_Materials_[id].GetUseTextureRefractionWeightInsteadOfWeight(); }
	bool GetEnableRefractionWeightImage(int id) { return m_Materials_[id].GetEnableRefractionWeightImage(); }
	std::string GetRefractionWeightTexturePath(int id) { return m_Materials_[id].GetRefractionWeightPath(); }
	glm::vec4 GetRefractionWeight(int id) { return m_Materials_[id].GetRefractionWeight(); }

	// Refraction Normal map
	bool GetRefractionNormalImageColor(int id) { return m_Materials_[id].GetRefractionImageNormal(); }
	bool GetUseTextureRefractionNormalInsteadOfColor(int id) { return m_Materials_[id].GetUseTextureRefractionNormalInsteadOfNormal(); }
	bool GetEnableRefractionNormalImage(int id) { return m_Materials_[id].GetEnableRefractionNormalImage(); }
	std::string GetRefractionNormalTexturePath(int id) { return m_Materials_[id].GetRefractionNormalPath(); }
	glm::vec4 GetRefractionNormal(int id) { return m_Materials_[id].GetRefractionNormal(); }

	// Refraction Normal Map weight
	float GetRefractionNormalWeight(int id) { return m_Materials_[id].GetRefractionNormalWeight(); }

	// Refraction Roughness
	bool GetRefractionRoughnessImageColor(int id) { return m_Materials_[id].GetRefractionImageRoughness(); }
	bool GetUseTextureRefractionRoughnessInsteadOfColor(int id) { return m_Materials_[id].GetUseTextureRefractionRoughnessInsteadOfRoughness(); }
	bool GetEnableRefractionRoughnessImage(int id) { return m_Materials_[id].GetEnableRefractionRoughnessImage(); }
	std::string GetRefractionRoughnessTexturePath(int id) { return m_Materials_[id].GetRefractionRoughnessPath(); }
	glm::vec4 GetRefractionRoughness(int id) { return m_Materials_[id].GetRefractionRoughness(); }

	float GetRefractionIor(int id) { return m_Materials_[id].GetRefractionIor(); }
	bool GetRefractionThinSurface(int id) { return m_Materials_[id].GetRefractionThinSurface(); }

	// Refraction Absorption
	bool GetRefractionAbsorptionColorImageColor(int id) { return m_Materials_[id].GetRefractionImageAbsorptionColor(); }
	bool GetUseTextureRefractionAbsorptionColorInsteadOfColor(int id) { return m_Materials_[id].GetUseTextureRefractionAbsorptionColorInsteadOfAbsorptionColor(); }
	bool GetEnableRefractionAbsorptionColorImage(int id) { return m_Materials_[id].GetEnableRefractionAbsorptionColorImage(); }
	std::string GetRefractionAbsorptionDistanceTexturePath(int id) { return m_Materials_[id].GetRefractionAbsorptionDistancePath(); }
	glm::vec4 GetRefractionAbsorptionColor(int id) { return m_Materials_[id].GetRefractionAbsorptionColor(); }

	// Refraction Absorption Distance
	bool GetRefractionAbsorptionDistanceImageColor(int id) { return m_Materials_[id].GetRefractionImageAbsorptionDistance(); }
	bool GetUseTextureRefractionAbsorptionDistanceInsteadOfColor(int id) { return m_Materials_[id].GetUseTextureRefractionAbsorptionDistanceInsteadOfAbsorptionDistance(); }
	bool GetEnableRefractionAbsorptionDistanceImage(int id) { return m_Materials_[id].GetEnableRefractionAbsorptionDistanceImage(); }
	glm::vec4 GetRefractionAbsorptionDistance(int id) { return m_Materials_[id].GetRefractionAbsorptionDistance(); }
	bool GetRefractionCaustics(int id) { return m_Materials_[id].GetRefractionCaustics(); }

	// SSS Section ---------------------------------------------------------------------//
// SSS Scatter Color
	bool GetSssScatterColorImageColor(int id) { return m_Materials_[id].GetSssImageScatterColor(); }
	bool GetUseTextureSssScatterColorInsteadOfColor(int id) { return m_Materials_[id].GetUseTextureSssScatterColorInsteadOfScatterColor(); }
	bool GetEnableSssScatterColorImage(int id) { return m_Materials_[id].GetEnableSssScatterColorImage(); }
	std::string GetSssScatterColorTexturePath(int id) { return m_Materials_[id].GetSssScatterColorPath(); }
	glm::vec4 GetSssScatterColor(int id) { return m_Materials_[id].GetSssScatterColor(); }

	// SSS Scatter Weight
	bool GetSssScatterWeightImageColor(int id) { return m_Materials_[id].GetSssImageScatterWeight(); }
	bool GetUseTextureSssScatterWeightInsteadOfColor(int id) { return m_Materials_[id].GetUseTextureSssScatterWeightInsteadOfScatterWeight(); }
	bool GetEnableSssScatterWeightImage(int id) { return m_Materials_[id].GetEnableSssScatterWeightImage(); }
	std::string GetSssScatterWeightTexturePath(int id) { return m_Materials_[id].GetSssScatterWeightPath(); }
	glm::vec4 GetSssScatterWeight(int id) { return m_Materials_[id].GetSssScatterWeight(); }

	// SSS Scatter Distance
	bool GetSssScatterDistanceImageColor(int id) { return m_Materials_[id].GetSssImageScatterDistance(); }
	bool GetUseTextureSssScatterDistanceInsteadOfColor(int id) { return m_Materials_[id].GetUseTextureSssScatterDistanceInsteadOfScatterDistance(); }
	bool GetEnableSssScatterDistanceImage(int id) { return m_Materials_[id].GetEnableSssScatterDistanceImage(); }
	std::string GetSssScatterDistanceTexturePath(int id) { return m_Materials_[id].GetSssScatterDistancePath(); }
	glm::vec4 GetSssScatterDistance(int id) { return m_Materials_[id].GetSssScatterDistance(); }

	// SSS Scatter Direction
	bool GetSssScatterDirectionImageColor(int id) { return m_Materials_[id].GetSssImageScatterDirection(); }
	bool GetUseTextureSssScatterDirectionInsteadOfColor(int id) { return m_Materials_[id].GetUseTextureSssScatterDirectionInsteadOfScatterDirection(); }
	bool GetEnableSssScatterDirectionImage(int id) { return m_Materials_[id].GetEnableSssScatterDirectionImage(); }
	std::string GetSssScatterDirectionTexturePath(int id) { return m_Materials_[id].GetSssScatterDirectionPath(); }
	glm::vec4 GetSssScatterDirection(int id) { return m_Materials_[id].GetSssScatterDirection(); }

	bool GetSssUseMultiScattering(int id) { return m_Materials_[id].GetSssUseMultiScattering(); }
	bool GetSssUseSchlickApproximation(int id) { return m_Materials_[id].GetSssUseSchlickApproximation(); }

	// Coating Section ---------------------------------------------------------------------//
// Coating Color
	bool GetCoatingColorImageColor(int id) { return m_Materials_[id].GetCoatingImageColor(); }
	bool GetUseTextureCoatingColorInsteadOfColor(int id) { return m_Materials_[id].GetUseTextureCoatingColorInsteadOfColor(); }
	bool GetEnableCoatingColorImage(int id) { return m_Materials_[id].GetEnableCoatingColorImage(); }
	std::string GetCoatingColorTexturePath(int id) { return m_Materials_[id].GetCoatingColorPath(); }
	glm::vec4 GetCoatingColor(int id) { return m_Materials_[id].GetCoatingColor(); }

	// Coating Weight
	bool GetCoatingWeightImageColor(int id) { return m_Materials_[id].GetCoatingImageWeight(); }
	bool GetUseTextureCoatingWeightInsteadOfColor(int id) { return m_Materials_[id].GetUseTextureCoatingWeightInsteadOfWeight(); }
	bool GetEnableCoatingWeightImage(int id) { return m_Materials_[id].GetEnableCoatingWeightImage(); }
	std::string GetCoatingWeightTexturePath(int id) { return m_Materials_[id].GetCoatingWeightPath(); }
	glm::vec4 GetCoatingWeight(int id) { return m_Materials_[id].GetCoatingWeight(); }

	// Coating normal map
	bool GetCoatingNormalImageColor(int id) { return m_Materials_[id].GetCoatingImageNormal(); }
	bool GetUseTextureCoatingNormalInsteadOfColor(int id) { return m_Materials_[id].GetUseTextureCoatingNormalInsteadOfNormal(); }
	bool GetEnableCoatingNormalImage(int id) { return m_Materials_[id].GetEnableCoatingNormalImage(); }
	std::string GetCoatingNormalTexturePath(int id) { return m_Materials_[id].GetCoatingNormalPath(); }
	glm::vec4 GetCoatingNormal(int id) { return m_Materials_[id].GetCoatingNormal(); }

	// Coating normal map weight
	float GetCoatingNormalWeight(int id) { return m_Materials_[id].GetCoatingNormalWeight(); }

	// Coating Roughness
	bool GetCoatingRoughnessImageColor(int id) { return m_Materials_[id].GetCoatingImageRoughness(); }
	bool GetUseTextureCoatingRoughnessInsteadOfColor(int id) { return m_Materials_[id].GetUseTextureCoatingRoughnessInsteadOfRoughness(); }
	bool GetEnableCoatingRoughnessImage(int id) { return m_Materials_[id].GetEnableCoatingRoughnessImage(); }
	std::string GetCoatingRoughnessTexturePath(int id) { return m_Materials_[id].GetCoatingRoughnessPath(); }
	glm::vec4 GetCoatingRoughness(int id) { return m_Materials_[id].GetCoatingRoughness(); }

	// Coating Ior
	float GetCoatingIor(int id) { return m_Materials_[id].GetCoatingIor(); }

	// Coating Thickness
	bool GetCoatingThicknessImageColor(int id) { return m_Materials_[id].GetCoatingImageThickness(); }
	bool GetUseTextureCoatingThicknessInsteadOfColor(int id) { return m_Materials_[id].GetUseTextureCoatingThicknessInsteadOfThickness(); }
	bool GetEnableCoatingThicknessImage(int id) { return m_Materials_[id].GetEnableCoatingThicknessImage(); }
	std::string GetCoatingThicknessTexturePath(int id) { return m_Materials_[id].GetCoatingThicknessPath(); }
	glm::vec4 GetCoatingThickness(int id) { return m_Materials_[id].GetCoatingThickness(); }

	// Coating Transmission Color
	bool GetCoatingTransmissionColorImageColor(int id) { return m_Materials_[id].GetCoatingImageTransmissionColor(); }
	bool GetUseTextureCoatingTransmissionColorInsteadOfColor(int id) { return m_Materials_[id].GetUseTextureCoatingTransmissionColorInsteadOfTransmissionColor(); }
	bool GetEnableCoatingTransmissionColorImage(int id) { return m_Materials_[id].GetEnableCoatingTransmissionColorImage(); }
	std::string GetCoatingTransmissionColorTexturePath(int id) { return m_Materials_[id].GetCoatingTransmissionColorPath(); }
	glm::vec4 GetCoatingTransmissionColor(int id) { return m_Materials_[id].GetCoatingTransmissionColor(); }

	HorusMaterial::CoatingType GetCoatingMode(int id) { return m_Materials_[id].GetCoatingMode(); }

	// Coating Metalness
	bool GetCoatingMetalnessImageColor(int id) { return m_Materials_[id].GetCoatingImageMetalness(); }
	bool GetUseTextureCoatingMetalnessInsteadOfColor(int id) { return m_Materials_[id].GetUseTextureCoatingMetalnessInsteadOfMetalness(); }
	bool GetEnableCoatingMetalnessImage(int id) { return m_Materials_[id].GetEnableCoatingMetalnessImage(); }
	std::string GetCoatingMetalnessTexturePath(int id) { return m_Materials_[id].GetCoatingMetalnessPath(); }
	glm::vec4 GetCoatingMetalness(int id) { return m_Materials_[id].GetCoatingMetalness(); }

	// Other Section ---------------------------------------------------------------------//
	// Normal Map
	bool GetNormalImageColor(int id) { return m_Materials_[id].GetNormalMapImage(); }
	bool GetUseTextureNormalInsteadOfColor(int id) { return m_Materials_[id].GetUseTextureNormalMapInsteadOfNormal(); }
	bool GetEnableNormalImage(int id) { return m_Materials_[id].GetEnableNormalMapImage(); }
	std::string GetNormalTexturePath(int id) { return m_Materials_[id].GetNormalMapPath(); }
	glm::vec4 GetNormalMap(int id) { return m_Materials_[id].GetNormalMap(); }

	// Normal map weight
	glm::vec4 GetNormalWeight(int id) { return m_Materials_[id].GetNormalMapWeight(); }

	// Displacement Map
	bool GetDisplacementImageColor(int id) { return m_Materials_[id].GetDisplacementMapImage(); }
	bool GetUseTextureDisplacementInsteadOfColor(int id) { return m_Materials_[id].GetUseTextureDisplacementMapInsteadOfDisplacement(); }
	bool GetEnableDisplacementImage(int id) { return m_Materials_[id].GetEnableDisplacementMapImage(); }
	std::string GetDisplacementTexturePath(int id) { return m_Materials_[id].GetDisplacementMapPath(); }
	glm::vec4 GetDisplacementMap(int id) { return m_Materials_[id].GetDisplacementMap(); }

	// Displacement map weight
	glm::vec4 GetDisplacementWeight(int id) { return m_Materials_[id].GetDisplacementMapWeight(); }

	// Emissive
	bool GetEmissiveImageColor(int id) { return m_Materials_[id].GetEmissiveImage(); }
	bool GetUseTextureEmissiveInsteadOfColor(int id) { return m_Materials_[id].GetUseTextureEmissiveInsteadOfEmissive(); }
	bool GetEnableEmissiveImage(int id) { return m_Materials_[id].GetEnableEmissiveImage(); }
	std::string GetEmissiveTexturePath(int id) { return m_Materials_[id].GetEmissivePath(); }
	glm::vec4 GetEmissive(int id) { return m_Materials_[id].GetEmissive(); }

	// Emissive Weight
	bool GetEmissiveWeightImageColor(int id) { return m_Materials_[id].GetEmissionWeightImage(); }
	bool GetUseTextureEmissiveWeightInsteadOfColor(int id) { return m_Materials_[id].GetUseTextureEmissionWeightInsteadOfEmissionWeight(); }
	bool GetEnableEmissiveWeightImage(int id) { return m_Materials_[id].GetEnableEmissionWeightImage(); }
	std::string GetEmissiveWeightTexturePath(int id) { return m_Materials_[id].GetEmissionWeightPath(); }
	glm::vec4 GetEmissiveWeight(int id) { return m_Materials_[id].GetEmissionWeight(); }

	// Opacity
	bool GetOpacityImageColor(int id) { return m_Materials_[id].GetOpacityImage(); }
	bool GetUseTextureOpacityInsteadOfColor(int id) { return m_Materials_[id].GetUseTextureOpacityInsteadOfOpacity(); }
	bool GetEnableOpacityImage(int id) { return m_Materials_[id].GetEnableOpacityImage(); }
	std::string GetOpacityTexturePath(int id) { return m_Materials_[id].GetOpacityPath(); }
	glm::vec4 GetOpacity(int id) { return m_Materials_[id].GetOpacity(); }


	// Setters for inspector
	void SetActiveMaterialId(int id);
	void AssignMaterial(int mesh_id, int mat_id);
	void AssignMaterialDirectly(rpr_shape shape, int mat);

	rpr_material_node GetMaterialNode(int materialId);

	// Base color section ---------------------------------------------------------------------//
// Base color
	void SetBaseColorImageColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetBaseColorImageColor(value); }
	void SetUseTextureBaseColorInsteadOfColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetUseTextureBaseColorInsteadOfColor(value); }
	void SetEnableBaseColorImage(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetEnableBaseColorImage(value); }
	void SetBaseColor(int id, const std::string& texturePath) { if (m_Materials_.contains(id)) m_Materials_[id].SetBaseColor(texturePath); }
	void SetBaseColor(int id, glm::vec4 color) { if (m_Materials_.contains(id)) m_Materials_[id].SetBaseColor(color); }

	// Base color weight
	void SetBaseColorWeightImageColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetBaseColorWeightImage(value); }
	void SetUseTextureBaseColorWeightInsteadOfColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetUseTextureBaseColorWeightInsteadOfWeight(value); }
	void SetEnableBaseColorWeightImage(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetEnableBaseColorWeightImage(value); }
	void SetBaseColorWeight(int id, glm::vec4 color) { if (m_Materials_.contains(id)) m_Materials_[id].SetBaseColorWeight(color); }
	void SetBaseColorWeight(int id, const std::string& texturePath) { if (m_Materials_.contains(id)) m_Materials_[id].SetBaseColorWeight(texturePath); }

	// Base color roughness
	void SetBaseColorRoughnessImageColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetBaseColorRoughnessImage(value); }
	void SetUseTextureBaseColorRoughnessInsteadOfColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetUseTextureBaseColorRoughnessInsteadOfRoughness(value); }
	void SetEnableBaseColorRoughnessImage(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetEnableBaseColorRoughnessImage(value); }
	void SetBaseColorRoughness(int id, glm::vec4 color) { if (m_Materials_.contains(id)) m_Materials_[id].SetBaseColorRoughness(color); }
	void SetBaseColorRoughness(int id, const std::string& texturePath) { if (m_Materials_.contains(id)) m_Materials_[id].SetBaseColorRoughness(texturePath); }

	// Backscatter
	void GetBackscatterImageColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetBackscatterImageColor(value); }
	void SetUseTextureBackscatterColorInsteadOfColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetUseTextureBackscatterColorInsteadOfColor(value); }
	void SetEnableBackscatterColorImage(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetEnableBackscatterColorImage(value); }
	void SetBackscatterColor(int id, glm::vec4 color) { if (m_Materials_.contains(id)) m_Materials_[id].SetBackscatterColor(color); }
	void SetBackscatterColor(int id, const std::string& texturePath) { if (m_Materials_.contains(id)) m_Materials_[id].SetBackscatterColor(texturePath); }

	// Backscatter Weight
	void SetBackscatterWeightImageColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetBackscatterImageWeight(value); }
	void SetUseTextureBackscatterWeightInsteadOfColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetUseTextureBackscatterWeightInsteadOfWeight(value); }
	void SetEnableBackscatterWeightImage(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetEnableBackscatterWeightImage(value); }
	void SetBackscatterWeight(int id, glm::vec4 weight) { if (m_Materials_.contains(id)) m_Materials_[id].SetBackscatterWeight(weight); }
	void SetBackscatterWeight(int id, const std::string& texturePath) { if (m_Materials_.contains(id)) m_Materials_[id].SetBackscatterWeight(texturePath); }

	// Reflection Section ---------------------------------------------------------------------//
// Reflection Color
	void SetReflectionColorImageColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetReflectionImageColor(value); }
	void SetUseTextureReflectionColorInsteadOfColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetUseTextureReflectionColorInsteadOfColor(value); }
	void SetEnableReflectionColorImage(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetEnableReflectionColorImage(value); }
	void SetReflectionColor(int id, glm::vec4 color);
	void SetReflectionColor(int id, const std::string& texturePath);

	// Reflection Weight
	void SetReflectionWeightImageColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetReflectionImageWeight(value); }
	void SetUseTextureReflectionWeightInsteadOfColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetUseTextureReflectionWeightInsteadOfWeight(value); }
	void SetEnableReflectionWeightImage(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetEnableReflectionWeightImage(value); }
	void SetReflectionWeight(int id, glm::vec4 color);
	void SetReflectionWeight(int id, const std::string& texturePath);

	// Reflection Roughness
	void SetReflectionRoughnessImageColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetReflectionImageRoughness(value); }
	void SetUseTextureReflectionRoughnessInsteadOfColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetUseTextureReflectionRoughnessInsteadOfRoughness(value); }
	void SetEnableReflectionRoughnessImage(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetEnableReflectionRoughnessImage(value); }
	void SetReflectionRoughness(int id, glm::vec4 color);
	void SetReflectionRoughness(int id, const std::string& texturePath);

	// Reflection Anisotropy
	void SetReflectionAnisotropyImageColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetReflectionImageAnisotropy(value); }
	void SetUseTextureReflectionAnisotropyInsteadOfColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetUseTextureReflectionAnisotropyInsteadOfAnisotropy(value); }
	void SetEnableReflectionAnisotropyImage(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetEnableReflectionAnisotropyImage(value); }
	void SetReflectionAnisotropy(int id, glm::vec4 color);
	void SetReflectionAnisotropy(int id, const std::string& texturePath);
	void SetReflectionMode(int id, HorusMaterial::ReflectionType mode);
	void SetIor(int id, float ior) { if (m_Materials_.contains(id)) m_Materials_[id].SetIor(ior); }

	// Reflection Metalness
	void SetReflectionMetalnessImageColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetReflectionImageMetalness(value); }
	void SetUseTextureReflectionMetalnessInsteadOfColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetUseTextureReflectionMetalnessInsteadOfMetalness(value); }
	void SetEnableReflectionMetalnessImage(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetEnableReflectionMetalnessImage(value); }
	void SetReflectionMetalness(int id, glm::vec4 color);
	void SetReflectionMetalness(int id, const std::string& texturePath);


	// Sheen Section ---------------------------------------------------------------------//
// Sheen Color
	void SetSheenColorImageColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetSheenImageColor(value); }
	void SetUseTextureSheenColorInsteadOfColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetUseTextureSheenColorInsteadOfColor(value); }
	void SetEnableSheenColorImage(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetEnableSheenColorImage(value); }
	void SetSheenColor(int id, glm::vec4 color);
	void SetSheenColor(int id, const std::string& texturePath);

	// Sheen Weight
	void SetSheenWeightImageColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetSheenImageWeight(value); }
	void SetUseTextureSheenWeightInsteadOfColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetUseTextureSheenWeightInsteadOfWeight(value); }
	void SetEnableSheenWeightImage(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetEnableSheenWeightImage(value); }
	void SetSheenWeight(int id, glm::vec4 weight);
	void SetSheenWeight(int id, const std::string& texturePath);

	// Sheen Tint
	void SetSheenTintImageColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetSheenImageTint(value); }
	void SetUseTextureSheenTintInsteadOfColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetUseTextureSheenTintInsteadOfTint(value); }
	void SetEnableSheenTintImage(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetEnableSheenTintImage(value); }
	void SetSheenTint(int id, glm::vec4 tint);
	void SetSheenTint(int id, const std::string& texturePath);

	// Refraction Section ---------------------------------------------------------------------//
// Refraction Color
	void SetRefractionColorImageColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetRefractionImageColor(value); }
	void SetUseTextureRefractionColorInsteadOfColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetUseTextureRefractionColorInsteadOfColor(value); }
	void SetEnableRefractionColorImage(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetEnableRefractionColorImage(value); }
	void SetRefractionColor(int id, glm::vec4 color);
	void SetRefractionColor(int id, const std::string& texturePath);

	// Refraction Weight
	void SetRefractionWeightImageColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetRefractionImageWeight(value); }
	void SetUseTextureRefractionWeightInsteadOfColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetUseTextureRefractionWeightInsteadOfWeight(value); }
	void SetEnableRefractionWeightImage(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetEnableRefractionWeightImage(value); }
	void SetRefractionWeight(int id, glm::vec4 weight);
	void SetRefractionWeight(int id, const std::string& texturePath);

	// Refraction Normal Map
	void SetRefractionNormalImageColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetRefractionImageNormal(value); }
	void SetUseTextureRefractionNormalInsteadOfColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetUseTextureRefractionNormalInsteadOfNormal(value); }
	void SetEnableRefractionNormalImage(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetEnableRefractionNormalImage(value); }
	void SetRefractionNormal(int id, const std::string& texturePath);
	void SetRefractionNormal(int id, glm::vec4 normal);

	// Refraction Normal Map Weight
	void SetRefractionNormalWeight(int id, float value) { if (m_Materials_.contains(id)) m_Materials_[id].SetRefractionNormalWeight(value); }

	// Refraction Roughness
	void SetRefractionRoughnessImageColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetRefractionImageRoughness(value); }
	void SetUseTextureRefractionRoughnessInsteadOfColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetUseTextureRefractionRoughnessInsteadOfRoughness(value); }
	void SetEnableRefractionRoughnessImage(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetEnableRefractionRoughnessImage(value); }
	void SetRefractionRoughness(int id, glm::vec4 roughness);
	void SetRefractionRoughness(int id, const std::string& texturePath);

	void SetRefractionIor(int id, float ior) { if (m_Materials_.contains(id)) m_Materials_[id].SetRefractionIor(ior); }
	void SetRefractionThinSurface(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetRefractionThinSurface(value); }


	// Refraction Absorption
	void SetRefractionAbsorptionColorImageColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetRefractionImageAbsorptionColor(value); }
	void SetUseTextureRefractionAbsorptionColorInsteadOfColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetUseTextureRefractionAbsorptionColorInsteadOfAbsorptionColor(value); }
	void SetEnableRefractionAbsorptionColorImage(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetEnableRefractionAbsorptionColorImage(value); }
	void SetRefractionAbsorptionColor(int id, glm::vec4 color);
	void SetRefractionAbsorptionColor(int id, const std::string& texturePath);

	// Refraction Absorption Distance
	void SetRefractionAbsorptionDistanceImageColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetRefractionImageAbsorptionDistance(value); }
	void SetUseTextureRefractionAbsorptionDistanceInsteadOfColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetUseTextureRefractionAbsorptionDistanceInsteadOfAbsorptionDistance(value); }
	void SetEnableRefractionAbsorptionDistanceImage(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetEnableRefractionAbsorptionDistanceImage(value); }
	void SetRefractionAbsorptionDistance(int id, glm::vec4 color);
	void SetRefractionAbsorptionDistance(int id, const std::string& texturePath);

	void SetRefractionCaustics(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetRefractionCaustics(value); }

	// SSS Section ---------------------------------------------------------------------//
// SSS Scatter Color
	void SetSssScatterColorImageColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetSssImageScatterColor(value); }
	void SetUseTextureSssScatterColorInsteadOfColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetUseTextureSssScatterColorInsteadOfScatterColor(value); }
	void SetEnableSssScatterColorImage(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetEnableSssScatterColorImage(value); }
	void SetSssScatterColor(int id, glm::vec4 color);
	void SetSssScatterColor(int id, const std::string& texturePath);

	// SSS Scatter Weight
	void SetSssScatterWeightImageColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetSssImageScatterWeight(value); }
	void SetUseTextureSssScatterWeightInsteadOfColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetUseTextureSssScatterWeightInsteadOfScatterWeight(value); }
	void SetEnableSssScatterWeightImage(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetEnableSssScatterWeightImage(value); }
	void SetSssScatterWeight(int id, glm::vec4 weight);
	void SetSssScatterWeight(int id, const std::string& texturePath);

	// SSS Scatter Distance
	void SetSssScatterDistanceImageColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetSssImageScatterDistance(value); }
	void SetUseTextureSssScatterDistanceInsteadOfColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetUseTextureSssScatterDistanceInsteadOfScatterDistance(value); }
	void SetEnableSssScatterDistanceImage(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetEnableSssScatterDistanceImage(value); }
	void SetSssScatterDistance(int id, glm::vec4 color);
	void SetSssScatterDistance(int id, const std::string& texturePath);


	// SSS Scatter Direction
	void SetSssScatterDirectionImageColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetSssImageScatterDirection(value); }
	void SetUseTextureSssScatterDirectionInsteadOfColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetUseTextureSssScatterDirectionInsteadOfScatterDirection(value); }
	void SetEnableSssScatterDirectionImage(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetEnableSssScatterDirectionImage(value); }
	void SetSssScatterDirection(int id, glm::vec4 color);
	void SetSssScatterDirection(int id, const std::string& texturePath);


	void SetSssUseMultiScattering(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetSssMultiScattering(value); }
	void SetSssUseSchlickApproximation(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetSssUseSchlickApproximation(value); }

	// Coating Section ---------------------------------------------------------------------//
// Coating Color
	void SetCoatingColorImageColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetCoatingImageColor(value); }
	void SetUseTextureCoatingColorInsteadOfColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetUseTextureCoatingColorInsteadOfColor(value); }
	void SetEnableCoatingColorImage(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetEnableCoatingColorImage(value); }
	void SetCoatingColor(int id, glm::vec4 color);
	void SetCoatingColor(int id, const std::string& texturePath);

	// Coating Weight
	void SetCoatingWeightImageColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetCoatingImageWeight(value); }
	void SetUseTextureCoatingWeightInsteadOfColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetUseTextureCoatingWeightInsteadOfWeight(value); }
	void SetEnableCoatingWeightImage(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetEnableCoatingWeightImage(value); }
	void SetCoatingWeight(int id, glm::vec4 weight);
	void SetCoatingWeight(int id, const std::string& texturePath);

	// Coating Normal Map
	void SetCoatingNormalImageColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetCoatingImageNormal(value); }
	void SetUseTextureCoatingNormalInsteadOfColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetUseTextureCoatingNormalInsteadOfNormal(value); }
	void SetEnableCoatingNormalImage(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetEnableCoatingNormalImage(value); }
	void SetCoatingNormalMap(int id, const std::string& texturePath);
	void SetCoatingNormalMap(int id, glm::vec4 color);

	// Coating Normal map weight
	void SetCoatingNormalWeight(int id, float value) { if (m_Materials_.contains(id)) m_Materials_[id].SetCoatingNormalWeight(value); }

	// Coating Roughness
	void SetCoatingRoughnessImageColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetCoatingImageRoughness(value); }
	void SetUseTextureCoatingRoughnessInsteadOfColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetUseTextureCoatingRoughnessInsteadOfRoughness(value); }
	void SetEnableCoatingRoughnessImage(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetEnableCoatingRoughnessImage(value); }
	void SetCoatingRoughness(int id, glm::vec4 roughness);
	void SetCoatingRoughness(int id, const std::string& texturePath);

	// Coating Ior
	void SetCoatingIor(int id, float ior) { if (m_Materials_.contains(id)) m_Materials_[id].SetCoatingIor(ior); }

	// Coating Thickness
	void SetCoatingThicknessImageColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetCoatingImageThickness(value); }
	void SetUseTextureCoatingThicknessInsteadOfColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetUseTextureCoatingThicknessInsteadOfThickness(value); }
	void SetEnableCoatingThicknessImage(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetEnableCoatingThicknessImage(value); }
	void SetCoatingThickness(int id, glm::vec4 thickness);
	void SetCoatingThickness(int id, const std::string& texturePath);

	// Coating Transmission Color
	void SetCoatingTransmissionColorImageColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetCoatingImageTransmissionColor(value); }
	void SetUseTextureCoatingTransmissionColorInsteadOfColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetUseTextureCoatingTransmissionColorInsteadOfTransmissionColor(value); }
	void SetEnableCoatingTransmissionColorImage(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetEnableCoatingTransmissionColorImage(value); }
	void SetCoatingTransmissionColor(int id, glm::vec4 color);
	void SetCoatingTransmissionColor(int id, const std::string& texturePath);

	//  Coating Metalness
	void SetCoatingMetalnessImageColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetCoatingImageMetalness(value); }
	void SetUseTextureCoatingMetalnessInsteadOfColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetUseTextureCoatingMetalnessInsteadOfMetalness(value); }
	void SetEnableCoatingMetalnessImage(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetEnableCoatingMetalnessImage(value); }
	void SetCoatingMetalness(int id, glm::vec4 metalness);
	void SetCoatingMetalness(int id, const std::string& texturePath);

	void SetCoatingMode(int id, HorusMaterial::CoatingType mode);

	// Other Section ---------------------------------------------------------------------//
	// Normal Map
	void SetNormalImageColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetNormalMapImage(value); }
	void SetUseTextureNormalInsteadOfColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetUseTextureNormalMapInsteadOfNormal(value); }
	void SetEnableNormalImage(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetEnableNormalMapImage(value); }
	void SetNormalMap(int id, const std::string& texturePath);
	void SetNormalMap(int id, glm::vec4 color);

	// Normal Weight
	void SetNormalWeight(int id, glm::vec4 value) { if (m_Materials_.contains(id)) m_Materials_[id].SetNormalMapWeight(value); }

	// Displacement Map
	void SetDisplacementImageColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetDisplacementMapImage(value); }
	void SetUseTextureDisplacementInsteadOfColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetUseTextureDisplacementMapInsteadOfDisplacement(value); }
	void SetEnableDisplacementImage(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetEnableDisplacementMapImage(value); }
	void SetDisplacementMap(int id, const std::string& texturePath);
	void SetDisplacementMap(int id, glm::vec4 color);

	// Displacement Weight
	void SetDisplacementWeight(int id, glm::vec4 value) { if (m_Materials_.contains(id)) m_Materials_[id].SetDisplacementMapWeight(value); }

	// Emissive
	void SetEmissiveImageColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetEmissiveImage(value); }
	void SetUseTextureEmissiveInsteadOfColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetUseTextureEmissiveInsteadOfEmissive(value); }
	void SetEnableEmissiveImage(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetEnableEmissiveImage(value); }
	void SetEmissive(int id, const std::string& texturePath);
	void SetEmissive(int id, glm::vec4 color);

	// Emissive Weight
	void SetEmissiveWeightImageColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetEmissionWeightImage(value); }
	void SetUseTextureEmissiveWeightInsteadOfColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetUseTextureEmissionWeightInsteadOfEmissionWeight(value); }
	void SetEnableEmissiveWeightImage(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetEnableEmissionWeightImage(value); }
	void SetEmissiveWeight(int id, const std::string& texturePath);
	void SetEmissiveWeight(int id, glm::vec4 color);

	// Opacity
	void SetOpacityImageColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetOpacityImage(value); }
	void SetUseTextureOpacityInsteadOfColor(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetUseTextureOpacityInsteadOfOpacity(value); }
	void SetEnableOpacityImage(int id, bool value) { if (m_Materials_.contains(id)) m_Materials_[id].SetEnableOpacityImage(value); }
	void SetOpacity(int id, const std::string& texturePath);
	void SetOpacity(int id, glm::vec4 color);














	void SetTransparency(int id, glm::vec4 color);
	void SetTransparency(int id, const std::string& texturePath);

	// ----------------------------------------------
	// Material Editor object --------------------------

	HorusMaterialEditor& CreateMaterialEditorNode(int id, std::string Name)
	{
		if (m_MaterialEditors_.contains(id))
		{
			spdlog::error("Material Editor with id {} already exists", id);

			return m_MaterialEditors_[id];
		}

		spdlog::info("Creating material graph with id {}", id);

		HorusMaterialEditor NewMaterialEditor;
		NewMaterialEditor.init();

		m_MaterialEditors_[id] = NewMaterialEditor;
		m_MaterialEditorNames_[id] = Name;
		m_MaterialEditorMeshToSetMaterial_[id] = id;

		return m_MaterialEditors_[id];
	}

	void AssignMaterialEditorNode(int mesh_id, int mat_id);

	void QuitMaterialEditor(int id);
	void DestroyAllMaterialEditors();

	void ShowMaterialEditor(int id);

	void OpenMaterialEditor(int id);
	void OpenMaterialEditorBrowser();
	void OpenMaterialEditorCreateMenu();

	void CloseMaterialEditor(int id);
	void CloseMaterialEditorBrowser();
	void CloseMaterialEditorCreateMenu();

	int SetMaterialEditorToShow(int id);
	void SetMaterialFromEditorNode(int id, rpr_material_node mat);
	int SetMaterialEditorMeshToSetMaterial(int id)
	{
		m_MaterialEditorMeshToSetMaterial_[m_MaterialEditorToShow_] = id;

		return id;
	}

	int GetMaterialEditorToShow();
	int GetMeshIdToSetMaterial()
	{
		return m_MaterialEditorMeshToSetMaterial_[m_MaterialEditorToShow_];
	}
	int GetMaterialEditorIdByName(const char* name)
	{
		for (const auto& Key : m_MaterialEditors_ | std::views::keys)
		{
			int Id = Key;

			if (std::string MaterialName = m_MaterialEditorNames_[Id]; MaterialName == name)
			{
				return Id;
			}
		}

		return -1;
	}
	void GetMaterialEditorMaterials(std::vector<std::string>& materials)
	{
		for (const auto& Key : m_MaterialEditors_ | std::views::keys)
		{
			int Id = Key;
			std::string Name = m_MaterialEditorNames_[Id];
			materials.push_back(Name);
		}
	}

	// ----------------------------------------------
	// Light object ---------------------------------

	int CreateLight(const std::string& Name, const std::string& LightType, const std::string& ImagePath = "");
	void DestroyLight(int Id);
	void DestroyAllLights();

	std::string& GetLightName(int id);
	int GetLightType(int id);
	HorusLight& GetLight(int Id);
	std::string GetLightTexture(int id) { return m_Lights_[id].GetLightTexturePath(); }
	glm::vec3 GetLightPosition(int id) { return m_Lights_[id].GetPosition(); }
	glm::vec3 GetLightRotation(int id) { return m_Lights_[id].GetRotation(); }
	glm::vec3 GetLightScale(int id) { return m_Lights_[id].GetScale(); }
	glm::vec3 GetLightIntensity(int id) { return m_Lights_[id].GetIntensity(); }
	int GetActiveLightId() { return m_ActiveLightId_; }
	glm::vec3 GetLightColor(int id) { return m_Lights_[id].GetColor(); }
	glm::vec3 GetLightDirection(int id) { return m_Lights_[id].GetDirection(); }

	void SetActiveLightId(int id) { m_ActiveLightId_ = id; }
	void SetLightName(int id, const std::string& name);
	void SetLightTexture(int id, const std::string& ImagePath);
	void SetLightIntensity(int id, glm::vec3 Intensity);
	void SetLightType(int id, int type);
	void SetLightVisibility(int id, bool visibility);
	void SetLightPosition(int id, const glm::vec3& position);
	void SetLightRotation(int id, const glm::vec3& rotation);
	void SetLightScale(int id, const glm::vec3& scale);

	// Directional light
	void SetDirectionalLightShadowSoftnessAngle(int id, float Coef);

	// Spot light
	void SetConeShape(int id, float InAngle, float OutAngle);
	void SetSpotLightImage(int id, const std::string& ImagePath);

	// Environment light
	void SetEnvironmentLightSetImage(int id, const std::string& ImagePath);
	void SetShapeEnvironmentLight(int id, rpr_shape Shape, bool IsEnvLight);
	void SetEnvironmentLightAttachPortal(int id, rpr_scene Scene, rpr_light Light, rpr_shape Shape);
	void SetEnvironmentLightDetachPortal(int id, rpr_scene Scene, rpr_light Light, rpr_shape Shape);

	// Sky light
	void SetSkyLightTurbidity(int id, float Turbidity);
	void SetSkyLightAlbedo(int id, const glm::vec3& Albedo);
	void SetSkyLightScale(int id, const glm::vec3& Scale);
	void SetSkyLightDirection(int id, const glm::vec3& Direction);
	void SetSkyLightAttachPortal(int id, rpr_scene Scene, rpr_light Light, rpr_shape Shape);
	void SetSkyLightDetachPortal(int id, rpr_scene Scene, rpr_light Light, rpr_shape Shape);

	// Ies
	void SetIesLightImage(int id, const std::string& ImagePath, int ImgSizeX, int ImgSizeY);
	void SetIesLightImage(int id, const char* ImagePath, int ImgSizeX, int ImgSizeY);

	// Sphere light
	void SetSphereLightRadius(int id, float Radius);

	// Disk light
	void SetDiskLightRadius(int id, const float& Radius);
	void SetDiskLightAngle(int id, const float& Angle);
	void SetDiskLightInnerAngle(int id, const float& InnerAngle);

	// Scene object ----------------------------------

	int CreateScene(const std::string& name);
	void ImportGltfScene();

	void SetScene(int id);
	rpr_scene& GetScene();
	int GetSceneIdByName(const char* name);
	int GetActiveSceneId();
	std::string& GetSceneNameById(int id);

	void DestroyScene(int id);
	void DestroyAllScenes();

	int CreateDefaultScene();

	void ShowDummyDragon();
	void ShowDummyPlane();
	void ShowLookdevScene();
	void ShowJaguardXKSS();


	// ----------------------------------------------

private:

	HorusObjectManager() = default;

	std::map<std::string, int> m_ObjectNameToIdMap_;
	int m_ActualSelectedId_ = -1;
	std::string m_Name_;

	// Camera object ----------------

	std::map<int, HorusOpenGLCamera> m_OpenGlCameras_;
	int m_ActiveOpenGLCameraId_ = 0;
	int m_NextOpenGLCameraId_ = 0;
	std::map<int, std::string> m_OpenGlCameraNames_;

	std::map<int, HorusRadeonCamera> m_RadeonCameras_;
	int m_ActiveRadeonCameraId_ = 0;
	int m_NextRadeonCameraId_ = 0;
	std::map<int, std::string> m_RadeonCameraNames_;

	// Group Shape object ------------------

	std::map<std::string, std::vector<rpr_shape>> m_MeshShapeMap_;
	std::map<int, HorusGroupShape> m_GroupShape_;
	int m_ActiveGroupShapeId_ = 0;
	int m_GroupShapeIndex_ = 0;
	int m_GroupShapeCount_ = 0;
	std::map<int, std::string> m_GroupShapeNames_;
	std::map<std::string, std::vector<std::string>> m_GroupObjectOutlinerData_;

	// Shape object -----------------

	std::map<int, rpr_shape> m_Shapes_;
	rpr_shape m_ActualShape_;
	std::string m_ActualShapeName_;
	glm::mat4 m_ActualShapeTransform_;
	glm::vec3 m_ActualShapePosition_;
	glm::vec3 m_ActualShapeRotation_;
	glm::vec3 m_ActualShapeScale_;
	int m_ActiveShapeId_ = 0;
	int m_ShapeIndex_ = 0;
	int m_ShapeCount_ = 0;

	// Material object --------------

	std::map<int, HorusMaterial> m_Materials_;
	int m_ActiveMaterialId_ = 0;
	int m_MaterialIndex_ = 0;
	int m_MaterialCount_ = 0;
	std::map<int, std::string> m_MaterialNames_;

	// Material Editor object -------

	std::map<int, HorusMaterialEditor> m_MaterialEditors_;
	int m_MaterialEditorIndex_ = 0;
	int m_MaterialEditorCount_ = 0;
	std::map<int, std::string> m_MaterialEditorNames_;
	std::map<int, int> m_MaterialEditorMeshToSetMaterial_;

	int m_MaterialEditorToShow_ = 0;

	// background image -------------

	HorusMaterial m_BackgroundMaterial_;

	// Light object -----------------

	std::map<int, HorusLight> m_Lights_;
	int m_ActiveLightId_ = 0;
	int m_LightIndex_ = 0;
	int m_LightCount_ = 0;
	std::map<int, std::string> m_LightNames_;

	// Scene object -----------------

	std::map<int, HorusScene> m_Scenes_;
	int m_SceneIndex_ = 0;
	int m_SceneCount_ = 0;
	int m_ActiveSceneId_ = 0;
	std::map<int, std::string> m_SceneNames_;

	// ------------------------------
};
