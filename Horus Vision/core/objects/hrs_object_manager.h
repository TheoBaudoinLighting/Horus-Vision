#pragma once


#include "hrs_scene.h" // nothing
#include "hrs_camera.h" // nothing
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

	// ----------------------------------------------
	// Outliner -------------------------------------

	void GetOutlinerMeshes(std::vector<std::string>& Meshes)
	{
		for (const auto& Key : m_Meshes_ | std::views::keys)
		{
			int Id = Key;
			std::string Name = m_MeshNames_[Id];
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
		for (const auto& Key : m_Cameras_ | std::views::keys)
		{
			int Id = Key;
			std::string Name = m_CameraNames_[Id];
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

	// ----------------------------------------------
	// Camera object ----------------------------------

	int CreateCamera(int SceneID, std::string Name);

	void DestroyCamera(int id);
	void DestroyAllCameras()
	{
		for (auto& Val : m_Cameras_ | std::views::values)
		{
			Val.Destroy();
		}
		m_Cameras_.clear();
	}

	HorusCamera& GetCamera(int id)
	{
		return m_Cameras_[id];
	}

	void BindCamera(int id);
	void UnbindCamera(int id);

	void UpdateCamera(int id);
	void GetMatrices(int id, glm::mat4& projection, glm::mat4& view, glm::mat4& model);

	void PrintCameraInfo(int id);
	void ResetCamera(int id);

	void SetViewport(int id, int x, int y, int width, int height);

	int GetActiveCameraId();
	void SetActiveCamera(int id);

	int GetCameraIdByName(const char* name);
	std::string& GetCameraNameById(int id);

	void MoveCameraForward(int id);
	void MoveCameraBackward(int id);
	void MoveCameraLeft(int id);
	void MoveCameraRight(int id);
	void MoveCameraUp(int id);
	void MoveCameraDown(int id);
	void ScrollCamera(int id, float delta);

	void SetPitch(int id, float pitch);
	void SetHeading(int id, float heading);

	void SetCameraLookat(int id, glm::vec3& pivot);

	void TumbleCamera(int id, float x, float y);
	void PanCamera(int id, float x, float y);
	void ZoomCamera(int id, float distance);

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

	// Setters for inspector

	void SetCameraFov(int id, float fov);
	void SetCameraAspectRatio(int id, float aspect_ratio);
	void SetCameraNear(int id, float near_plane);
	void SetCameraFar(int id, float far_plane);
	void SetAperture(int id, float aperture);

	void SetCameraLookAt(int id, glm::vec3 lookat);
	void SetCameraPosition(int id, glm::vec3 position);
	void SetCameraRotation(int id, glm::vec3 rotation_axis);
	void SetCameraScale(int id, glm::vec3 scale);

	// ----------------------------------------------
	// Mesh object ----------------------------------

	int CreateMesh(const char* path, const std::string& name);

	void DestroyMesh(int id);
	void DestroyAllMeshes();

	// Getters for inspector
	HorusMesh& GetMesh(int id /*, HorusMesh* mesh*/);

	rpr_shape GetMeshShape(int id);

	std::string& GetMeshName(int id);
	void SetActiveMesh(int id);
	int GetActiveMeshId();

	void GetMeshCount(int count);
	int GetMeshId(const char* name);
	void GetMeshIdByIndex(int index, int* id);
	void GetMeshIndexById(int id, int* index);
	void GetMeshIndexByName(const char* name);

	glm::mat4 GetMeshTransform(int id);
	glm::vec3 GetMeshPosition(int id);
	glm::vec3 GetMeshRotation(int id);
	glm::vec3 GetMeshScale(int id);

	// Setters for inspector

	void SetMeshName(int id, const char* name);
	void SetMeshId(int id, int new_id);

	void SetShapePosition(int id, glm::vec3 pos);
	void SetShapeRotation(int id, glm::vec3 rot);
	void SetShapeScale(int id, glm::vec3 scale);

	// ----------------------------------------------
	// Material object ------------------------------

	int CreateMaterial(std::string name)
	{
		std::string material_name = name;

		int suffix = 0;

		while (m_ObjectNameToIdMap_.contains(material_name))
		{
			material_name = name + "_" + std::to_string(suffix);
			suffix++;

			spdlog::info("Material with name {} already exists", name);
		}

		int id = IDManager::GetInstance().GetNewId();

		HorusMaterial new_material;
		new_material.Init();
		m_Materials_[id] = new_material;
		m_MaterialNames_[id] = material_name;
		m_ObjectNameToIdMap_[material_name] = id;

		spdlog::info("Creating {} material with id {}", material_name, id);

		return id;
	}

	void DestroyMaterial(int id)
	{
		auto it = m_Materials_.find(id);
		if (it != m_Materials_.end())
		{
			it->second.DestroyMaterial();
			m_Materials_.erase(it);
			m_MaterialNames_.erase(id);
		}
		else
		{
			spdlog::error("Material with id {} does not exist", id);
		}

		for (auto it = m_ObjectNameToIdMap_.begin(); it != m_ObjectNameToIdMap_.end(); ++it)
		{
			if (it->second == id)
			{
				m_ObjectNameToIdMap_.erase(it);
				break;
			}
		}

		IDManager::GetInstance().ReleaseId(id);
	}

	void DestroyAllMaterial();

	void AssignMaterial(int mesh_id, int mat_id);

	void SetBaseColor(int id, const std::string& texturePath);
	void SetBaseColor(int id, const std::array<float, 3>& color);

	void SetMetallic(int id, const std::string& texturePath);
	void SetMetallic(int id, const std::array<float, 3>& color);

	void SetRoughness(int id, const std::string& texturePath);
	void SetRoughness(int id, const std::array<float, 3>& color);

	void SetNormal(int id, const std::string& texturePath);
	void SetNormal(int id, const std::array<float, 3>& color);

	void SetOpacity(int id, const std::string& texturePath);
	void SetOpacity(int id, const std::array<float, 3>& color);

	void SetEmissive(int id, const std::string& texturePath);
	void SetEmissive(int id, const std::array<float, 3>& color);

	void SetReflectionColor(int id, const std::array<float, 3>& color);
	void SetReflectionColor(int id, const std::string& texturePath);

	void SetReflectionWeight(int id, const std::array<float, 3>& color);
	void SetReflectionWeight(int id, const std::string& texturePath);

	void SetReflectionRoughness(int id, const std::array<float, 3>& color);
	void SetReflectionRoughness(int id, const std::string& texturePath);

	void SetRefractionColor(int id, const std::array<float, 3>& color);
	void SetRefractionColor(int id, const std::string& texturePath);

	void SetCoatingColor(int id, const std::array<float, 3>& color);
	void SetCoatingColor(int id, const std::string& texturePath);

	void SetSheen(int id, const std::array<float, 3>& color);
	void SetSheen(int id, const std::string& texturePath);

	void SetSssScatterColor(int id, const std::array<float, 3>& color);
	void SetSssScatterColor(int id, const std::string& texturePath);

	void SetBackscatterColor(int id, const std::array<float, 3>& color);
	void SetBackscatterColor(int id, const std::string& texturePath);

	void SetIor(int id, float ior);

	void SetRefractionWeight(int id, const std::array<float, 3>& weight);
	void SetRefractionWeight(int id, const std::string& texturePath);

	void SetRefractionRoughness(int id, const std::array<float, 3>& roughness);
	void SetRefractionRoughness(int id, const std::string& texturePath);

	void SetCoatingWeight(int id, const std::array<float, 3>& weight);
	void SetCoatingWeight(int id, const std::string& texturePath);

	void SetCoatingRoughness(int id, const std::array<float, 3>& roughness);
	void SetCoatingRoughness(int id, const std::string& texturePath);

	void SetSheenWeight(int id, const std::array<float, 3>& weight);
	void SetSheenWeight(int id, const std::string& texturePath);

	void SetBackscatterWeight(int id, const std::array<float, 3>& weight);
	void SetBackscatterWeight(int id, const std::string& texturePath);

	void SetDiffuseWeight(int id, const std::array<float, 3>& color);
	void SetDiffuseWeight(int id, const std::string& texturePath);

	void SetEmissionWeight(int id, const std::array<float, 3>& color);
	void SetEmissionWeight(int id, const std::string& texturePath);

	void SetTransparency(int id, const std::array<float, 3>& color);
	void SetTransparency(int id, const std::string& texturePath);

	void SetSssScatterDistance(int id, const std::array<float, 3>& color);
	void SetSssScatterDistance(int id, const std::string& texturePath);

	void SetReflectionMode(int id, int mode);
	void SetCoatingMode(int id, int mode);

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

	void SetMaterialFromEditorNode(int id, rpr_material_node mat);

	int SetMaterialEditorMeshToSetMaterial(int id)
	{
		m_MaterialEditorMeshToSetMaterial_[m_MaterialEditorToShow_] = id;

		return id;
	}

	int GetMeshIdToSetMaterial()
	{
		return m_MaterialEditorMeshToSetMaterial_[m_MaterialEditorToShow_];
	}

	void ShowMaterialEditor(int id);

	void OpenMaterialEditor(int id);

	void CloseMaterialEditor(int id);

	void OpenMaterialEditorBrowser();

	void CloseMaterialEditorBrowser();

	void GetMaterialEditorMaterials(std::vector<std::string>& materials)
	{
		for (const auto& Key : m_MaterialEditors_ | std::views::keys)
		{
			int Id = Key;
			std::string Name = m_MaterialEditorNames_[Id];
			materials.push_back(Name);
		}
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

	int SetMaterialEditorToShow(int id);

	int GetMaterialEditorToShow();

	void OpenMaterialEditorCreateMenu();

	void CloseMaterialEditorCreateMenu();

	void QuitMaterialEditor(int id);

	void DestroyAllMaterialEditors();

	// ----------------------------------------------
	// Light object ---------------------------------

	int CreateLight(const std::string& Name, const std::string& LightType, const std::string& ImagePath = "");

	void DestroyLight(int Id);

	void DestroyAllLights();

	std::string& GetLightName(int id);
	int GetLightType(int id);
	HorusLight& GetLight(int Id);
	glm::vec3 GetLightPosition(int id);
	glm::vec3 GetLightRotation(int id);
	glm::vec3 GetLightScale(int id);
	glm::vec3 GetLightIntensity(int id);
	int GetActiveLightId();
	glm::vec3 GetLightColor(int id);
	glm::vec3 GetLightDirection(int id);

	void SetActiveLightId(int id);
	void SetLightName(int id, const std::string& name);
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

	// ----------------------------------------------
	// Scene object ----------------------------------

	int CreateScene(const std::string& name);
	void SetScene(int id);
	rpr_scene GetScene();
	int GetSceneIdByName(const char* name);
	int GetActiveSceneId();
	std::string& GetSceneNameById(int id);

	void DestroyScene(int id);
	void DestroyAllScenes();

	int CreateDefaultScene();

	void ShowDummyDragon();
	void ShowDummyPlane();
	void ShowLookdevScene();

	// ----------------------------------------------

private:
	HorusObjectManager()
	{
	}

	std::map<std::string, int> m_ObjectNameToIdMap_;
	int m_ActualSelectedId_ = -1;
	std::string m_Name_;

	// Camera object ----------------

	std::map<int, HorusCamera> m_Cameras_;
	int m_ActiveCameraId_ = 0;
	int m_NextCameraId_ = 0;
	std::map<int, std::string> m_CameraNames_;

	// ------------------------------
	// Mesh object ------------------

	std::map<int, HorusMesh> m_Meshes_;
	int m_ActiveMeshId_ = 0;
	int m_MeshIndex_ = 0;
	int m_MeshCount_ = 0;
	std::map<int, std::string> m_MeshNames_;

	// ------------------------------
	// Material object --------------

	std::map<int, HorusMaterial> m_Materials_;
	std::unordered_map<int, HorusMaterialParameters> m_TempMatParams_;
	int m_ActiveMaterialId_ = 0;
	int m_MaterialIndex_ = 0;
	int m_MaterialCount_ = 0;
	std::map<int, std::string> m_MaterialNames_;

	// ------------------------------
	// Material Editor object -------

	std::map<int, HorusMaterialEditor> m_MaterialEditors_;
	int m_MaterialEditorIndex_ = 0;
	int m_MaterialEditorCount_ = 0;
	std::map<int, std::string> m_MaterialEditorNames_;
	std::map<int, int> m_MaterialEditorMeshToSetMaterial_;

	int m_MaterialEditorToShow_ = 0;

	// ------------------------------
	// background image

	HorusMaterial m_BackgroundMaterial_;

	// ------------------------------
	// Light object -----------------

	std::map<int, HorusLight> m_Lights_;
	int m_ActiveLightId_ = 0;
	int m_LightIndex_ = 0;
	int m_LightCount_ = 0;
	std::map<int, std::string> m_LightNames_;
	// ------------------------------
	// Scene object -----------------

	std::map<int, HorusScene> m_Scenes_;
	int m_SceneIndex_ = 0;
	int m_SceneCount_ = 0;
	int m_ActiveSceneId_ = 0;
	std::map<int, std::string> m_SceneNames_;

	// ------------------------------
};
