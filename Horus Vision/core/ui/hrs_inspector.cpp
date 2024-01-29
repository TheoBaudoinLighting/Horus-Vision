
#include "hrs_platform.h"
#include "hrs_inspector.h"
#include "hrs_radeon.h" // glfw3.h
#include "hrs_object_manager.h" // glfw3.h
#include "hrs_utils.h"

#include <string>

#include <filesystem>
#include <hrs_console.h>

void HorusInspector::Inspector(bool* p_open)
{
	ImGui::Begin("Inspector", p_open);

	switch (m_SelectionType_)
	{
	case InspectorType::CAMERA:
		InspectorCamera();
		break;
	case InspectorType::LIGHT:
		InspectorLight();
		break;
	case InspectorType::MATERIAL:
		InspectorMaterial();
		break;
	case InspectorType::GROUPSHAPE:
		InspectorGroupShape();
		break;
	case InspectorType::SHAPE:
		InspectorShape();
		break;
	case InspectorType::TEXTURE:
		InspectorTexture();
		break;
	case InspectorType::PROJECT:
		InspectorProjectProperty();
		break;
	default:
		break;
	}

	ImGui::End();

	if (m_ResetBuffer_)
	{
		CallResetBuffer();
		m_ResetBuffer_ = false;
	}
}
void HorusInspector::Init()
{
	HorusGarbageCollector& GarbageCollector = HorusGarbageCollector::GetInstance();
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusRadeon& Radeon = HorusRadeon::GetInstance();

	// Init Focus Plane and camera
	m_FocusPlaneShape_ = CreateSquare(GarbageCollector, Radeon.GetContext(), ObjectManager.GetScene(), 0.0f, 0.0f, 2.0f, 0.1f);

	rpr_material_node Emissive;
	{
		CHECK(rprMaterialSystemCreateNode(Radeon.GetMatsys(), RPR_MATERIAL_NODE_EMISSIVE, &Emissive));
		CHECK(rprMaterialNodeSetInputFByKey(Emissive, RPR_MATERIAL_INPUT_COLOR, 0.4f, 0.6f, 0.45f, 0.5f));
		CHECK(rprShapeSetMaterial(m_FocusPlaneShape_, Emissive));
	}
	GarbageCollector.Add(Emissive);
	spdlog::debug("Focus plane initialized.");
	m_DrawFocusPlane_ = false;
	CHECK(rprShapeSetVisibilityFlag(m_FocusPlaneShape_, RPR_SHAPE_VISIBILITY_PRIMARY_ONLY_FLAG, true));
	CHECK(rprShapeSetVisibilityFlag(m_FocusPlaneShape_, RPR_SHAPE_VISIBILITY_RECEIVE_SHADOW, false));
	CHECK(rprShapeSetVisibilityFlag(m_FocusPlaneShape_, RPR_SHAPE_VISIBILITY_LIGHT, false));
	CHECK(rprShapeSetVisibilityFlag(m_FocusPlaneShape_, RPR_SHAPE_VISIBILITY_SHADOW, false));

	m_CameraPosition_ = GetCameraPosition();
	m_CameraLookAt_ = GetCameraLookAt();
	m_FocusPlaneDistance_ = GetFocusDistance();

	CHECK(rprShapeSetVisibility(m_FocusPlaneShape_, false));
	SetFocusPlaneToFocusPosition(m_FocusPlaneShape_);

	// Init Project Inspector
	PopulateSelectedProjectInfos();



}
void HorusInspector::SetInspectorType(InspectorType Type)
{
	m_SelectionType_ = Type;
}

//-------------------------------------- CAMERA INSPECTOR --------------------------------------

void HorusInspector::InspectorCamera()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusRadeon& Radeon = HorusRadeon::GetInstance();

	//ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "Green");
	//ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.7f, 1.0f), "Pink");

	// Camera Inspector
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f)); // Green
	ImGui::Text("Camera Inspector");
	ImGui::PopStyleColor();
	ImGui::Separator();

	// Basic attributes
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f)); // Green
	ImGui::Text("General Information");
	ImGui::PopStyleColor();

	// Debug tools
	if (ImGui::CollapsingHeader("Debug Tools"))
	{
		if (ImGui::Button("Print camera info"))
		{
			ObjectManager.PrintCameraInfo(ObjectManager.GetActiveRadeonCameraId());
		}

		if (ImGui::Button("Reset camera"))
		{
			ObjectManager.ResetCamera(ObjectManager.GetActiveRadeonCameraId());

			CallResetBuffer();
		}

		ImGui::Separator();

		if (ImGui::Button("Move Camera Forward"))
		{
			ObjectManager.SetMoveCameraForward(ObjectManager.GetActiveRadeonCameraId());

			CallResetBuffer();
		}

		ImGui::SameLine();

		if (ImGui::Button("Move Camera Backward"))
		{
			ObjectManager.SetMoveCameraBackward(ObjectManager.GetActiveRadeonCameraId());

			CallResetBuffer();
		}

		if (ImGui::Button("Move Camera Left"))
		{
			ObjectManager.SetMoveCameraLeft(ObjectManager.GetActiveRadeonCameraId());

			CallResetBuffer();
		}

		ImGui::SameLine();

		if (ImGui::Button("Move Camera Right"))
		{
			ObjectManager.SetMoveCameraRight(ObjectManager.GetActiveRadeonCameraId());

			CallResetBuffer();
		}

		if (ImGui::Button("Move Camera Up"))
		{
			ObjectManager.SetMoveCameraUp(ObjectManager.GetActiveRadeonCameraId());

			CallResetBuffer();
		}

		ImGui::SameLine();

		if (ImGui::Button("Move Camera Down"))
		{
			ObjectManager.SetMoveCameraDown(ObjectManager.GetActiveRadeonCameraId());

			CallResetBuffer();
		}
	}
	// End Debug tools

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Button("Print camera info"))
		{
			ObjectManager.PrintCameraInfo(ObjectManager.GetActiveRadeonCameraId());
		}
		ImGui::SameLine(); ShowHelpMarker("Print the camera info in the console.");

		if (ImGui::Button("Reset camera"))
		{
			ObjectManager.ResetCamera(ObjectManager.GetActiveRadeonCameraId());

			CallResetBuffer();
		}
		ImGui::SameLine(); ShowHelpMarker("Reset the transform of the camera.");

		ImGui::Separator();

		if (ImGui::InputFloat3("Look At", &m_CameraLookAt_[0]))
		{
			SetCameraLookAt(m_CameraLookAt_);

			CallResetBuffer();
		}
		ImGui::SameLine(); ShowHelpMarker("Point the camera is looking at.");

		if (ImGui::InputFloat3("Position", &m_CameraPosition_[0]))
		{
			SetCameraPosition(m_CameraPosition_);

			CallResetBuffer();
		}

		if (ImGui::InputFloat3("Rotation", &m_CameraRotation_[0]))
		{
			SetCameraRotation(m_CameraRotation_);

			CallResetBuffer();
		}

		if (ImGui::SliderFloat("Uniform Scale", &m_UniformScale_, 0.1f, 1.0f, "%.1f"))
		{
			m_CameraScale_ = glm::vec3(m_UniformScale_);
			SetCameraScale(m_CameraScale_);
			CallResetBuffer();
		}
		ImGui::SameLine(); ShowHelpMarker("Set uniform scale for the camera.");
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// Advanced attributes
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f)); // Green
	ImGui::Text("Advanced attributes");
	ImGui::PopStyleColor();

	ImGui::Separator();

	if (ImGui::SliderFloat("FOV", &m_CameraFov_, 1.0f, 180.0f))
	{
		SetFov(m_CameraFov_);

		CallResetBuffer();
	}
	ImGui::SameLine(); ShowHelpMarker("Focal length of the camera lens.");

	ImGui::InputFloat("Near Clip", &m_CameraNear_);
	ImGui::SameLine(); ShowHelpMarker("Near clip plane of the camera.");
	ImGui::InputFloat("Far Clip", &m_CameraFar_);
	ImGui::SameLine(); ShowHelpMarker("Far clip plane of the camera.");

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f)); // Green
	ImGui::Text("Depth of Field");
	ImGui::PopStyleColor();

	if (ImGui::Checkbox("Enable DOF", &m_EnableDof_))
	{
		if (m_EnableDof_)
		{
			PopulateSelectedCameraInfos();
			ObjectManager.SetFStop(ObjectManager.GetActiveRadeonCameraId(), m_CameraFStop_);
			SetFocusPlaneToFocusPosition(m_FocusPlaneShape_);
			CallResetBuffer();
		}
		else
		{
			// Set FStop to 100 to disable DOF effect, because 100 is a very high value for FStop
			// (not really disable but it will be very hard to see)
			ObjectManager.SetFStop(ObjectManager.GetActiveRadeonCameraId(), 100);
			CallResetBuffer();
		}
	}

	if (m_EnableDof_)
	{
		// Draw Focus Plane
		if (ImGui::Checkbox("Draw Focus Plane", &m_DrawFocusPlane_))
		{
			SetFocusPlaneToFocusPosition(m_FocusPlaneShape_);
			ShowHideFocusPlane(m_FocusPlaneShape_);

			spdlog::debug("Draw Focus Plane : {}", m_DrawFocusPlane_);
			Radeon.SetLockPreviewMode(m_DrawFocusPlane_);
			CallResetBuffer();
		}

		if (ImGui::SliderFloat("Focus Distance", &m_FocusPlaneDistance_, 0.1f, 15.0f, "%.3f"))
		{
			ObjectManager.SetFocusDistance(ObjectManager.GetActiveRadeonCameraId(), m_FocusPlaneDistance_);
			SetFocusPlaneToFocusPosition(m_FocusPlaneShape_);
			CallResetBuffer();
		}
		ImGui::SameLine(); ShowHelpMarker("Set focal plane distance for DOF effect. The focal plane is the plane that is in focus. The distance is measured from the camera position along the camera look at vector.");

		// Set FStop
		if (ImGui::SliderFloat("FStop", &m_CameraFStop_, 0.001f, 10.0f, "%.3f"))
		{
			ObjectManager.SetFStop(ObjectManager.GetActiveRadeonCameraId(), m_CameraFStop_);

			CallResetBuffer();
		}
		ImGui::SameLine(); ShowHelpMarker("Set FStop for DOF effect. FStop is the ratio of the lens focal length to the diameter of the entrance pupil.");

		if (ImGui::SliderInt("Aperture Blades", &m_ApertureBlades_, 3, 20))
		{
			// Simple check to avoid irrealistic values
			if (m_ApertureBlades_ < 3)
			{
				m_ApertureBlades_ = 3;
			}

			ObjectManager.SetApertureBlades(ObjectManager.GetActiveRadeonCameraId(), m_ApertureBlades_);

			CallResetBuffer();
		}

	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f)); // Green
	ImGui::Text("Camera Infos");
	ImGui::PopStyleColor();

	ImGui::Separator();

	ImGui::Text("Camera id: %d ", ObjectManager.GetActiveRadeonCameraId());
	ImGui::Text("Camera name: %s ", m_CameraName_.c_str());
	ImGui::Text("Camera look at: %.2f %.2f %.2f ", m_CameraLookAt_.x, m_CameraLookAt_.y, m_CameraLookAt_.z);
	ImGui::Text("Camera position: %.2f %.2f %.2f ", m_CameraPosition_.x, m_CameraPosition_.y, m_CameraPosition_.z);
	ImGui::Text("Camera rotation: %.2f %.2f %.2f ", m_CameraRotation_.x, m_CameraRotation_.y, m_CameraRotation_.z);
	ImGui::Text("Camera scale: %.2f %.2f %.2f ", m_CameraScale_.x, m_CameraScale_.y, m_CameraScale_.z);
	ImGui::Text("Camera FOV: %.2f ", m_CameraFov_);
	ImGui::Text("Camera near clip: %.2f ", m_CameraNear_);
	ImGui::Text("Camera far clip: %.2f ", m_CameraFar_);
	ImGui::Text("Camera FStop: %.2f ", m_CameraFStop_);
	ImGui::Text("Camera Focus Distance: %.2f ", m_FocusPlaneDistance_);
	ImGui::Text("Camera Aperture Blades: %d ", m_ApertureBlades_);

	ImGui::Separator();
}

void HorusInspector::PopulateSelectedCameraInfos()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	// Getters Camera
	m_CameraName_ = ObjectManager.GetCameraNameById(ObjectManager.GetActiveRadeonCameraId());

	m_CameraLookAt_ = GetCameraLookAt();
	m_CameraPosition_ = GetCameraPosition();
	m_CameraRotation_ = GetCameraRotation();
	m_CameraScale_ = GetCameraScale();
	m_CameraNear_ = GetCameraNear();
	m_CameraFar_ = GetCameraFar();
	m_CameraFov_ = GetFov();
	m_CameraFStop_ = GetFStop();
	m_ApertureBlades_ = GetApertureBlades();
}

// Camera Getters
glm::vec3 HorusInspector::GetCameraLookAt()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	int ActiveCamera = ObjectManager.GetActiveRadeonCameraId();

	if (ActiveCamera != -1)
	{
		return ObjectManager.GetCameraLookAt(ActiveCamera);
	}
	else
	{
		return { 0.0f, 0.0f, 0.0f };
	}
}
glm::vec3 HorusInspector::GetCameraPosition()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	int ActiveCamera = ObjectManager.GetActiveRadeonCameraId();

	if (ActiveCamera != -1)
	{
		return ObjectManager.GetCameraPosition(ActiveCamera);
	}
	else
	{
		return { 0.0f, 0.0f, 0.0f };
	}

}
glm::vec3 HorusInspector::GetCameraRotation()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	int ActiveCamera = ObjectManager.GetActiveRadeonCameraId();

	if (ActiveCamera != -1)
	{
		return ObjectManager.GetCameraRotation(ActiveCamera);
	}
	else
	{
		return { 0.0f, 0.0f, 0.0f };
	}
}
glm::vec3 HorusInspector::GetCameraScale()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	int ActiveCamera = ObjectManager.GetActiveRadeonCameraId();

	if (ActiveCamera != -1)
	{
		return ObjectManager.GetCameraScale(ActiveCamera);
	}
	else
	{
		return { 0.0f, 0.0f, 0.0f };
	}
}
float HorusInspector::GetCameraNear()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	int ActiveCamera = ObjectManager.GetActiveRadeonCameraId();

	if (ActiveCamera != -1)
	{
		return ObjectManager.GetCameraNearPlane(ActiveCamera);
	}
	else
	{
		return 0.0f;
	}
}
float HorusInspector::GetCameraFar()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	int ActiveCamera = ObjectManager.GetActiveRadeonCameraId();

	if (ActiveCamera != -1)
	{
		return ObjectManager.GetCameraFarPlane(ActiveCamera);
	}
	else
	{
		return 0.0f;
	}
}
float HorusInspector::GetFov()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	int ActiveCamera = ObjectManager.GetActiveRadeonCameraId();

	if (ActiveCamera != -1)
	{
		return ObjectManager.GetCameraFov(ActiveCamera);
	}
	else
	{
		return 0.0f;
	}
}
float HorusInspector::GetFStop()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	int ActiveCamera = ObjectManager.GetActiveRadeonCameraId();

	if (ActiveCamera != -1)
	{
		return ObjectManager.GetCameraFStop(ActiveCamera);
	}
	else
	{
		return 0.0f;
	}
}
float HorusInspector::GetFocusDistance()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	int ActiveCamera = ObjectManager.GetActiveRadeonCameraId();

	if (ActiveCamera != -1)
	{
		return ObjectManager.GetCameraFocusDistance(ActiveCamera);
	}
	else
	{
		return 0.0f;
	}
}
int HorusInspector::GetApertureBlades()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	int ActiveCamera = ObjectManager.GetActiveRadeonCameraId();

	if (ActiveCamera != -1)
	{
		return ObjectManager.GetCameraApertureBlades(ActiveCamera);
	}
	else
	{
		return 0;
	}
}
void HorusInspector::SetFocusPlaneToFocusPosition(rpr_shape& Plane)
{
	if (m_DrawFocusPlane_)
	{
		PopulateSelectedCameraInfos();
		glm::vec3 FocusDir = glm::normalize(m_CameraLookAt_ - m_CameraPosition_);

		glm::vec3 FocusPos = m_CameraPosition_ + FocusDir * m_FocusPlaneDistance_;
		glm::vec3 FocusNormal = -FocusDir;

		glm::mat4 FocusTransform = glm::mat4(1.0f);
		FocusTransform = glm::translate(FocusTransform, FocusPos);

		glm::vec3 WorldUp = { 0.0f, 1.0f, 0.0f };
		glm::vec3 WorldRight = glm::normalize(glm::cross(WorldUp, FocusNormal));
		glm::vec3 Up = glm::cross(FocusNormal, WorldRight);

		glm::mat4 RotationMatrix = glm::mat4(glm::vec4(WorldRight, 0.0f),
			glm::vec4(Up, 0.0f),
			glm::vec4(FocusNormal, 0.0f),
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

		FocusTransform = FocusTransform * RotationMatrix;
		CHECK(rprShapeSetTransform(Plane, false, &FocusTransform[0][0]));
	}
}
void HorusInspector::ShowHideFocusPlane(rpr_shape& Plane)
{
	CHECK(rprShapeSetVisibility(Plane, m_DrawFocusPlane_));
}

// Camera Setters
void HorusInspector::SetCameraLookAt(glm::vec3 look_at)
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	int ActiveCamera = ObjectManager.GetActiveRadeonCameraId();

	if (ActiveCamera != -1)
	{
		ObjectManager.SetCameraLookat(ActiveCamera, look_at);
	}
	else
	{
		spdlog::error("No camera selected");
		Console.AddLog(" [error] No camera selected");
	}
}
void HorusInspector::SetCameraPosition(glm::vec3 position)
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	int ActiveCamera = ObjectManager.GetActiveRadeonCameraId();

	if (ActiveCamera != -1)
	{
		ObjectManager.SetCameraPosition(ActiveCamera, position);
	}
	else
	{
		spdlog::error("No camera selected");
		Console.AddLog(" [error] No camera selected");
	}
}
void HorusInspector::SetCameraRotation(glm::vec3 rotation_axis)
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	int ActiveCamera = ObjectManager.GetActiveRadeonCameraId();

	if (ActiveCamera != -1)
	{
		ObjectManager.SetCameraRotation(ActiveCamera, rotation_axis);
	}
	else
	{
		spdlog::error("No camera selected");
		Console.AddLog(" [error] No camera selected");
	}
}
void HorusInspector::SetCameraScale(glm::vec3 scale)
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	int ActiveCamera = ObjectManager.GetActiveRadeonCameraId();

	if (ActiveCamera != -1)
	{
		ObjectManager.SetCameraScale(ActiveCamera, scale);
	}
	else
	{
		spdlog::error("No camera selected");
		Console.AddLog(" [error] No camera selected");
	}
}
void HorusInspector::SetCameraNear(float Near)
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	int ActiveCamera = ObjectManager.GetActiveRadeonCameraId();

	if (ActiveCamera != -1)
	{
		ObjectManager.SetCameraNear(ActiveCamera, Near);
	}
	else
	{
		spdlog::error("No camera selected");
		Console.AddLog(" [error] No camera selected");
	}
}
void HorusInspector::SetCameraFar(float Far)
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	int ActiveCamera = ObjectManager.GetActiveRadeonCameraId();

	if (ActiveCamera != -1)
	{
		ObjectManager.SetCameraFar(ActiveCamera, Far);
	}
	else
	{
		spdlog::error("No camera selected");
		Console.AddLog(" [error] No camera selected");
	}
}
void HorusInspector::SetFStop(float FStop)
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	int ActiveCamera = ObjectManager.GetActiveRadeonCameraId();

	if (ActiveCamera != -1)
	{
		ObjectManager.SetFStop(ActiveCamera, FStop);
	}
	else
	{
		spdlog::error("No camera selected");
		Console.AddLog(" [error] No camera selected");
	}
}
void HorusInspector::SetFov(float fov)
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	int ActiveCamera = ObjectManager.GetActiveRadeonCameraId();

	if (ActiveCamera != -1)
	{
		ObjectManager.SetCameraFov(ActiveCamera, fov);
	}
	else
	{
		spdlog::error("No camera selected");
		Console.AddLog(" [error] No camera selected");
	}
}

//-------------------------------------- LIGHT INSPECTOR --------------------------------------

void HorusInspector::InspectorLight()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	// Get Light info from ObjectManager
	std::string& LightName = ObjectManager.GetLightName(ObjectManager.GetActiveLightId());
	m_LightPosition_ = ObjectManager.GetLightPosition(ObjectManager.GetActiveLightId());
	m_LightRotation_ = ObjectManager.GetLightRotation(ObjectManager.GetActiveLightId());
	m_LightScale_ = ObjectManager.GetLightScale(ObjectManager.GetActiveLightId());

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f)); // Green
	ImGui::Text("Light Inspector");
	ImGui::PopStyleColor();
	ImGui::Separator();

	// Combo box light type
	m_LightType_ = ObjectManager.GetLightType(ObjectManager.GetActiveLightId()); // 0 to 7 (8 types) -> 0 = point, 1 = directional, 2 = spot, 3 = environment(hdri), 4 = sky, 5 = ies, 6 = sphere, 7 = disk

	/*if (ImGui::Combo("Light Type", &m_LightType_, "Point\0Directional\0Spot\0Environment\0Sky\0IES\0Sphere\0Disk\0"))
	{
		ObjectManager.SetLightType(ObjectManager.GetActiveLightId(), m_LightType_);
		CallResetBuffer();
	}*/

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f)); // Green
	ImGui::Text("General Informations");
	ImGui::PopStyleColor();

	ImGui::Separator();

	if (ImGui::Checkbox("Visible", &m_IsLightVisible_))
	{
		ObjectManager.SetLightVisibility(ObjectManager.GetActiveLightId(), m_IsLightVisible_);

		CallResetBuffer();
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// show correct ui depending on the light type
	switch (m_LightType_)
	{
	case 0: // Point Light

		if (ImGui::CollapsingHeader("Point Light", ImGuiTreeNodeFlags_DefaultOpen))
		{
			static bool IsRgbMode = false;

			static float GlobalIntensity = 10.0f;
			if (!IsRgbMode)
			{
				if (ImGui::SliderFloat("Global Intensity", &GlobalIntensity, 0.0f, 10.0f, "%.1f"))
				{
					m_LightIntensity_ = glm::vec3(GlobalIntensity, GlobalIntensity, GlobalIntensity);
					ObjectManager.SetLightIntensity(ObjectManager.GetActiveLightId(), m_LightIntensity_);
					CallResetBuffer();
				}

				ImGui::SameLine();
				if (ImGui::Button("-> RGB"))
				{
					IsRgbMode = !IsRgbMode;
				}
			}

			if (IsRgbMode)
			{
				if (ImGui::SliderFloat3("Intensity", &m_LightIntensity_[0], 0.0f, 10.0f, "%.1f"))
				{
					ObjectManager.SetLightIntensity(ObjectManager.GetActiveLightId(), m_LightIntensity_);
					CallResetBuffer();
				}
				ImGui::SameLine();
				if (ImGui::Button("-> Global"))
				{
					IsRgbMode = !IsRgbMode;
				}
				ImGui::SameLine();
				ShowHelpMarker("Intensity of the light in Watts. Horus Vision measures radiant power in Watts, with a close approximation to real-world illuminative powers. Note, the RGB values can go beyond the range of [0;1].");
			}

			ImGui::Separator();
			ImGui::Spacing();
		}

		break;
	case 1: // Directional Light

		if (ImGui::CollapsingHeader("Directionnal Light", ImGuiTreeNodeFlags_DefaultOpen))
		{
			static bool IsRgbMode = false;

			static float GlobalIntensity = 10.0f;
			if (!IsRgbMode)
			{
				if (ImGui::SliderFloat("Global Intensity", &GlobalIntensity, 0.0f, 10.0f, "%.1f"))
				{
					m_LightIntensity_ = glm::vec3(GlobalIntensity, GlobalIntensity, GlobalIntensity);
					ObjectManager.SetLightIntensity(ObjectManager.GetActiveLightId(), m_LightIntensity_);
					CallResetBuffer();
				}

				ImGui::SameLine();
				if (ImGui::Button("-> RGB"))
				{
					IsRgbMode = !IsRgbMode;
				}
			}

			if (IsRgbMode)
			{
				if (ImGui::SliderFloat3("Intensity", &m_LightIntensity_[0], 0.0f, 10.0f, "%.1f"))
				{
					ObjectManager.SetLightIntensity(ObjectManager.GetActiveLightId(), m_LightIntensity_);
					CallResetBuffer();
				}
				ImGui::SameLine();
				if (ImGui::Button("-> Global"))
				{
					IsRgbMode = !IsRgbMode;
				}
				ImGui::SameLine();
				ShowHelpMarker("Intensity of the light in Watts. Horus Vision measures radiant power in Watts, with a close approximation to real-world illuminative powers. Note, the RGB values can go beyond the range of [0;1].");
			}

			ImGui::Separator();
			ImGui::Spacing();

			// Set Shadow Softness Angle

			if (ImGui::SliderFloat("Shadow Softness Angle", &m_DirectionalLightShadowSoftnessAngle_, 0.0f, 1.0f))
			{
				ObjectManager.SetDirectionalLightShadowSoftnessAngle(ObjectManager.GetActiveLightId(), m_DirectionalLightShadowSoftnessAngle_);

				CallResetBuffer();
			}
		}

		break;
	case 2: // Spot Light

		if (ImGui::CollapsingHeader("Spot Light", ImGuiTreeNodeFlags_DefaultOpen))
		{
			static bool IsRgbMode = false;

			static float GlobalIntensity = 10.0f;
			if (!IsRgbMode)
			{
				if (ImGui::SliderFloat("Global Intensity", &GlobalIntensity, 0.0f, 10.0f, "%.1f"))
				{
					m_LightIntensity_ = glm::vec3(GlobalIntensity, GlobalIntensity, GlobalIntensity);
					ObjectManager.SetLightIntensity(ObjectManager.GetActiveLightId(), m_LightIntensity_);
					CallResetBuffer();
				}

				ImGui::SameLine();
				if (ImGui::Button("-> RGB"))
				{
					IsRgbMode = !IsRgbMode;
				}
			}

			if (IsRgbMode)
			{
				if (ImGui::SliderFloat3("Intensity", &m_LightIntensity_[0], 0.0f, 10.0f, "%.1f"))
				{
					ObjectManager.SetLightIntensity(ObjectManager.GetActiveLightId(), m_LightIntensity_);
					CallResetBuffer();
				}
				ImGui::SameLine();
				if (ImGui::Button("-> Global"))
				{
					IsRgbMode = !IsRgbMode;
				}
				ImGui::SameLine();
				ShowHelpMarker("Intensity of the light in Watts. Horus Vision measures radiant power in Watts, with a close approximation to real-world illuminative powers. Note, the RGB values can go beyond the range of [0;1].");
			}

			ImGui::Separator();
			ImGui::Spacing();

			// Spot light shape
			if (ImGui::SliderFloat2("Cone Shape", &m_SpotLightCone_[0], 0.0f, 1.0f))
			{
				// Control the angle of the inner and outer circles of the spot light
				if (m_SpotLightCone_.x > m_SpotLightCone_.y)
				{
					m_SpotLightCone_.y = m_SpotLightCone_.x;
				}

				ObjectManager.SetConeShape(ObjectManager.GetActiveLightId(), m_SpotLightCone_.x, m_SpotLightCone_.y);

				CallResetBuffer();
			}
			ImGui::SameLine(); ShowHelpMarker("Spot lights produce a smooth penumbra in the region between their inner and outer circles. The area inside the inner circle receives full power while the area beyond the outer one is fully in shadow. The angle creating the outer circle should be greater than or equal to the angle of the inner circle.");

			// Spot light image
			// TODO : Implement spot light image

		}

		break;
	case 3: // Environment Light

		if (ImGui::CollapsingHeader("HDRI", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::SliderFloat("Intensity", &m_LightIntensity_[0], 0.0f, 1.0f, "%.1f"))
			{
				ObjectManager.SetLightIntensity(ObjectManager.GetActiveLightId(), m_LightIntensity_);

				CallResetBuffer();
			}

			// TODO : Implement backplate

			// TODO : Implement HDRI Image selection

			// TODO : Implement HDRI Image rotation
		}

		break;
	case 4: // Sky Light

		if (ImGui::CollapsingHeader("Sky Light", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::SliderFloat("Intensity", &m_LightIntensity_[0], 0.0f, 1.0f, "%.1f"))
			{
				ObjectManager.SetLightIntensity(ObjectManager.GetActiveLightId(), m_LightIntensity_);

				CallResetBuffer();
			}

			if (ImGui::SliderFloat("Turbidity", &m_SkyLightTurbidity_, 0.0f, 1.0f, "%.1f"))
			{
				ObjectManager.SetSkyLightTurbidity(ObjectManager.GetActiveLightId(), m_SkyLightTurbidity_);

				CallResetBuffer();
			}

			if (ImGui::SliderFloat("Albedo", &m_SkyLightAlbedo_[0], 0.0f, 1.0f, "%.1f"))
			{
				ObjectManager.SetSkyLightAlbedo(ObjectManager.GetActiveLightId(), m_SkyLightAlbedo_);

				CallResetBuffer();
			}

			if (ImGui::SliderFloat("Scale", &m_SkyLightScale_[0], 0.0f, 1.0f, "%.1f"))
			{
				ObjectManager.SetLightScale(ObjectManager.GetActiveLightId(), m_SkyLightScale_);

				CallResetBuffer();
			}

			if (ImGui::SliderFloat3("Direction", &m_SkyLightDirection_[0], 0.0f, 1.0f, "%.1f"))
			{
				ObjectManager.SetSkyLightDirection(ObjectManager.GetActiveLightId(), m_SkyLightDirection_);

				CallResetBuffer();
			}
		}

		break;
	case 5: // IES Light

		if (ImGui::CollapsingHeader("IES Light", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::SliderFloat("Intensity", &m_LightIntensity_[0], 0.0f, 1.0f, "%.1f"))
			{
				ObjectManager.SetLightIntensity(ObjectManager.GetActiveLightId(), m_LightIntensity_);

				CallResetBuffer();
			}

			// TODO : Implement IES Image Import between IES data and image
		}

		break;
	case 6: // Sphere Light

		if (ImGui::CollapsingHeader("Sphere Light", ImGuiTreeNodeFlags_DefaultOpen))
		{
			static bool IsRgbMode = false;

			static float GlobalIntensity = 10.0f;
			if (!IsRgbMode)
			{
				if (ImGui::SliderFloat("Global Intensity", &GlobalIntensity, 0.0f, 10.0f, "%.1f"))
				{
					m_LightIntensity_ = glm::vec3(GlobalIntensity, GlobalIntensity, GlobalIntensity);
					ObjectManager.SetLightIntensity(ObjectManager.GetActiveLightId(), m_LightIntensity_);
					CallResetBuffer();
				}

				ImGui::SameLine();
				if (ImGui::Button("-> RGB"))
				{
					IsRgbMode = !IsRgbMode;
				}
			}

			if (IsRgbMode)
			{
				if (ImGui::SliderFloat3("Intensity", &m_LightIntensity_[0], 0.0f, 10.0f, "%.1f"))
				{
					ObjectManager.SetLightIntensity(ObjectManager.GetActiveLightId(), m_LightIntensity_);
					CallResetBuffer();
				}
				ImGui::SameLine();
				if (ImGui::Button("-> Global"))
				{
					IsRgbMode = !IsRgbMode;
				}
				ImGui::SameLine();
				ShowHelpMarker("Intensity of the light in Watts. Horus Vision measures radiant power in Watts, with a close approximation to real-world illuminative powers. Note, the RGB values can go beyond the range of [0;1].");
			}

			ImGui::Separator();
			ImGui::Spacing();

			if (ImGui::SliderFloat("Radius", &m_SphereLightRadius_, 0.0f, 1.0f))
			{
				ObjectManager.SetSphereLightRadius(ObjectManager.GetActiveLightId(), m_SphereLightRadius_);

				CallResetBuffer();
			}
		}

		break;
	case 7: // Disk Light

		if (ImGui::CollapsingHeader("Disk Light", ImGuiTreeNodeFlags_DefaultOpen))
		{
			static bool IsRgbMode = false;

			static float GlobalIntensity = 10.0f;
			if (!IsRgbMode)
			{
				if (ImGui::SliderFloat("Global Intensity", &GlobalIntensity, 0.0f, 10.0f, "%.1f"))
				{
					m_LightIntensity_ = glm::vec3(GlobalIntensity, GlobalIntensity, GlobalIntensity);
					ObjectManager.SetLightIntensity(ObjectManager.GetActiveLightId(), m_LightIntensity_);
					CallResetBuffer();
				}

				ImGui::SameLine();
				if (ImGui::Button("-> RGB"))
				{
					IsRgbMode = !IsRgbMode;
				}
			}

			if (IsRgbMode)
			{
				if (ImGui::SliderFloat3("Intensity", &m_LightIntensity_[0], 0.0f, 10.0f, "%.1f"))
				{
					ObjectManager.SetLightIntensity(ObjectManager.GetActiveLightId(), m_LightIntensity_);
					CallResetBuffer();
				}
				ImGui::SameLine();
				if (ImGui::Button("-> Global"))
				{
					IsRgbMode = !IsRgbMode;
				}
				ImGui::SameLine();
				ShowHelpMarker("Intensity of the light in Watts. Horus Vision measures radiant power in Watts, with a close approximation to real-world illuminative powers. Note, the RGB values can go beyond the range of [0;1].");
			}

			ImGui::Separator();
			ImGui::Spacing();

			if (ImGui::SliderFloat("Radius", &m_DiskLightRadius_, 0.0f, 1.0f))
			{
				ObjectManager.SetSphereLightRadius(ObjectManager.GetActiveLightId(), m_DiskLightRadius_);

				CallResetBuffer();
			}
			ImGui::SameLine(); ShowHelpMarker("The radius of the disk light.");

			ImGui::Separator();

			if (ImGui::SliderFloat("Inner disk angle", &m_DiskLightInnerAngle_, 0.0f, 1.0f))
			{
				ObjectManager.SetDiskLightInnerAngle(ObjectManager.GetActiveLightId(), m_DiskLightInnerAngle_);

				CallResetBuffer();
			}
			ImGui::SameLine(); ShowHelpMarker("The area inside the inner circle receives full power while the area beyond the outer one is fully in shadow.");

			if (ImGui::SliderFloat("Outer disk angle", &m_DiskLightAngle, 0.0f, 1.0f))
			{
				ObjectManager.SetDiskLightAngle(ObjectManager.GetActiveLightId(), m_DiskLightAngle);

				CallResetBuffer();
			}
		}

		break;
	}

	ImGui::Separator();

	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Button("Reset Transform"))
		{
			ObjectManager.SetLightPosition(ObjectManager.GetActiveLightId(), glm::vec3(0.0f, 0.0f, 0.0f));
			ObjectManager.SetLightRotation(ObjectManager.GetActiveLightId(), glm::vec3(0.0f, 0.0f, 0.0f));
			ObjectManager.SetLightScale(ObjectManager.GetActiveLightId(), glm::vec3(1.0f, 1.0f, 1.0f));

			CallResetBuffer();
		}

		if (m_LightType_ == 3)
		{
			ImGui::BeginDisabled(true);
			if (ImGui::InputFloat3("Position", &m_LightPosition_[0]))
			{
				ObjectManager.SetLightPosition(ObjectManager.GetActiveLightId(), m_LightPosition_);

				CallResetBuffer();
			}
			ImGui::EndDisabled();
		}
		else
		{
			if (ImGui::InputFloat3("Position", &m_LightPosition_[0]))
			{
				ObjectManager.SetLightPosition(ObjectManager.GetActiveLightId(), m_LightPosition_);

				CallResetBuffer();
			}
		}

		if (ImGui::InputFloat3("Rotation", &m_LightRotation_[0]))
		{
			ObjectManager.SetLightRotation(ObjectManager.GetActiveLightId(), m_LightRotation_);

			CallResetBuffer();
		}

		if (m_LightType_ == 3)
		{
			ImGui::BeginDisabled(true);
			if (ImGui::InputFloat3("Scale", &m_LightScale_[0]))
			{
				ObjectManager.SetLightScale(ObjectManager.GetActiveLightId(), m_LightScale_);

				CallResetBuffer();
			}
			ImGui::EndDisabled();
		}
		else
		{
			if (ImGui::InputFloat3("Scale", &m_LightScale_[0]))
			{
				ObjectManager.SetLightScale(ObjectManager.GetActiveLightId(), m_LightScale_);

				CallResetBuffer();
			}
		}
	}

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f)); // Green
	ImGui::Text("Light Infos");
	ImGui::PopStyleColor();

	ImGui::Separator();

	ImGui::Text("Light id: %d ", ObjectManager.GetActiveLightId());
	ImGui::Text("Light name: %s ", LightName.c_str());
	ImGui::Text("Light type: %d ", m_LightType_);

	ImGui::Separator();

	ImGui::Text("Light position: %.2f %.2f %.2f ", m_LightPosition_.x, m_LightPosition_.y, m_LightPosition_.z);
	ImGui::Text("Light rotation: %.2f %.2f %.2f ", m_LightRotation_.x, m_LightRotation_.y, m_LightRotation_.z);
	ImGui::Text("Light scale: %.2f %.2f %.2f ", m_LightScale_.x, m_LightScale_.y, m_LightScale_.z);

	if (m_LightType_ == 3)
	{
		ImGui::Text("Light intensity: %.2f ", m_LightIntensity_.x);
	}
	else
	{
		ImGui::Text("Light intensity: %.2f %.2f %.2f ", m_LightIntensity_.x, m_LightIntensity_.y, m_LightIntensity_.z);
	}

	// Special characteristics depending on the light type
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	if (m_LightType_ == 1)
	{
		ImGui::Text("Shadow Softness Angle: %.2f ", m_DirectionalLightShadowSoftnessAngle_);
	}
	else if (m_LightType_ == 2)
	{
		ImGui::Text("Spot Light Cone: %.2f %.2f ", m_SpotLightCone_.x, m_SpotLightCone_.y);
	}
	else if (m_LightType_ == 3)
	{

	}
	else if (m_LightType_ == 4)
	{
		ImGui::Text("Sky Light Turbidity: %.2f ", m_SkyLightTurbidity_);
		ImGui::Text("Sky Light Albedo: %.2f %.2f %.2f ", m_SkyLightAlbedo_.x, m_SkyLightAlbedo_.y, m_SkyLightAlbedo_.z);
		ImGui::Text("Sky Light Scale: %.2f %.2f %.2f ", m_SkyLightScale_.x, m_SkyLightScale_.y, m_SkyLightScale_.z);
		ImGui::Text("Sky Light Direction: %.2f %.2f %.2f ", m_SkyLightDirection_.x, m_SkyLightDirection_.y, m_SkyLightDirection_.z);
	}
	else if (m_LightType_ == 5)
	{

	}
	else if (m_LightType_ == 6)
	{
		ImGui::Text("Sphere Light Radius: %.2f ", m_SphereLightRadius_);
	}
	else if (m_LightType_ == 7)
	{
		ImGui::Text("Disk Light Radius: %.2f ", m_DiskLightRadius_);
		ImGui::Text("Disk Light Inner Angle: %.2f ", m_DiskLightInnerAngle_);
		ImGui::Text("Disk Light Angle: %.2f ", m_DiskLightAngle);
	}

}

//-------------------------------------- MATERIAL INSPECTOR --------------------------------------

void HorusInspector::InspectorMaterial()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	// TODO : Add a checkbox for freeze the value of the material

	// Getters Material
	//std::string& MaterialName = ObjectManager.GetMaterialName(ObjectManager.GetActiveMaterialId());

	//ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f)); // Green
	//ImGui::Text("Material Inspector");
	//ImGui::PopStyleColor();
	//ImGui::Separator();

	//ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f)); // Green
	//ImGui::Text("Presets");
	//ImGui::PopStyleColor();
	//// Preset system
	//if (ImGui::CollapsingHeader("Presets", ImGuiTreeNodeFlags_DefaultOpen))
	//{
	//	if (ImGui::Button("Default"))
	//	{

	//		CallResetBuffer();
	//	}
	//	ImGui::SameLine(); ShowHelpMarker("Default material preset.");

	//	if (ImGui::Button("Plastic"))
	//	{

	//		CallResetBuffer();
	//	}
	//	ImGui::SameLine(); ShowHelpMarker("Plastic material preset.");

	//	if (ImGui::Button("Metal"))
	//	{

	//		CallResetBuffer();
	//	}
	//	ImGui::SameLine(); ShowHelpMarker("Metal material preset.");

	//	if (ImGui::Button("Glass"))
	//	{

	//		CallResetBuffer();
	//	}
	//	ImGui::SameLine(); ShowHelpMarker("Glass material preset.");

	//	if (ImGui::Button("Fabric"))
	//	{

	//		CallResetBuffer();
	//	}
	//	ImGui::SameLine(); ShowHelpMarker("Fabric material preset.");

	//	if (ImGui::Button("Skin"))
	//	{

	//		CallResetBuffer();
	//	}
	//	ImGui::SameLine(); ShowHelpMarker("Skin material preset.");

	//	if (ImGui::Button("Hair"))
	//	{

	//		CallResetBuffer();
	//	}
	//	ImGui::SameLine(); ShowHelpMarker("Hair material preset.");

	//	if (ImGui::Button("Emissive"))
	//	{

	//		CallResetBuffer();
	//	}
	//	ImGui::SameLine(); ShowHelpMarker("Emissive material preset.");

	//	if (ImGui::Button("Car Paint"))
	//	{

	//		CallResetBuffer();
	//	}
	//	ImGui::SameLine(); ShowHelpMarker("Car Paint material preset.");
	//}

	//ImGui::Spacing();
	//ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f)); // Green
	//ImGui::Text("Diffuse");
	//ImGui::PopStyleColor();
	//ImGui::Separator();
	//ImGui::Spacing();

	//if (ImGui::CollapsingHeader("Diffuse", ImGuiTreeNodeFlags_DefaultOpen))
	//{
	//	if (ImGui::CollapsingHeader("Diffuse", ImGuiTreeNodeFlags_DefaultOpen))
	//	{
	//		if (ImGui::ColorEdit3("Color", &m_DiffuseColor_[0]))
	//		{
	//			ObjectManager.SetBaseColor(ObjectManager.GetActiveMaterialId(), m_DiffuseColor_);

	//			CallResetBuffer();
	//		}

	//		if (ImGui::CollapsingHeader("Weight"))
	//		{
	//			if (ImGui::SliderFloat("Weight", &m_DiffuseWeight_[0], 0.0f, 1.0f))
	//			{
	//				ObjectManager.SetDiffuseWeight(ObjectManager.GetActiveMaterialId(), m_DiffuseWeight_);

	//				CallResetBuffer();
	//			}
	//		}
	//	}

	//	ImGui::Spacing();
	//	ImGui::Separator();
	//	ImGui::Spacing();

	//	if (ImGui::CollapsingHeader("Roughness", ImGuiTreeNodeFlags_DefaultOpen))
	//	{
	//		if (ImGui::SliderFloat("Roughness", &m_Roughness_[0], 0.0f, 1.0f))
	//		{
	//			ObjectManager.SetRoughness(ObjectManager.GetActiveMaterialId(), m_Roughness_);

	//			CallResetBuffer();
	//		}
	//	}

	//	ImGui::Spacing();
	//	ImGui::Separator();
	//	ImGui::Spacing();

	//	if (ImGui::CollapsingHeader("Metallic", ImGuiTreeNodeFlags_DefaultOpen))
	//	{
	//		if (ImGui::SliderFloat("Metallic", &m_Metallic_[0], 0.0f, 1.0f))
	//		{
	//			ObjectManager.SetMetallic(ObjectManager.GetActiveMaterialId(), m_Metallic_);

	//			CallResetBuffer();
	//		}
	//	}

	//	ImGui::Spacing();
	//	ImGui::Separator();
	//	ImGui::Spacing();

	//	if (ImGui::CollapsingHeader("Normal", ImGuiTreeNodeFlags_DefaultOpen))
	//	{
	//		if (ImGui::SliderFloat("Normal", &m_Normal_[0], 0.0f, 1.0f))
	//		{
	//			ObjectManager.SetNormal(ObjectManager.GetActiveMaterialId(), m_Normal_);

	//			CallResetBuffer();
	//		}
	//	}

	//	ImGui::Spacing();
	//	ImGui::Separator();
	//	ImGui::Spacing();

	//	if (ImGui::CollapsingHeader("Opacity", ImGuiTreeNodeFlags_DefaultOpen))
	//	{
	//		if (ImGui::SliderFloat("Height", &m_Opacity_[0], 0.0f, 1.0f))
	//		{
	//			ObjectManager.SetOpacity(ObjectManager.GetActiveMaterialId(), m_Opacity_);

	//			CallResetBuffer();
	//		}
	//	}

	//	ImGui::Spacing();
	//	ImGui::Separator();
	//	ImGui::Spacing();

	//	if (ImGui::CollapsingHeader("Reflection", ImGuiTreeNodeFlags_DefaultOpen))
	//	{
	//		if (ImGui::Checkbox("Reflection mode", &m_ReflectionMode_))
	//		{
	//			ObjectManager.SetReflectionMode(ObjectManager.GetActiveMaterialId(), m_ReflectionMode_);

	//			CallResetBuffer();
	//		}

	//		if (ImGui::SliderFloat("Reflection Color", &m_ReflectionColor_[0], 0.0f, 1.0f))
	//		{
	//			ObjectManager.SetReflectionColor(ObjectManager.GetActiveMaterialId(), m_ReflectionColor_);

	//			CallResetBuffer();
	//		}

	//		if (ImGui::CollapsingHeader("Weight - Roughness"))
	//		{
	//			if (ImGui::SliderFloat("Reflection Weight", &m_ReflectionWeight_[0], 0.0f, 1.0f))
	//			{
	//				ObjectManager.SetReflectionWeight(ObjectManager.GetActiveMaterialId(), m_ReflectionWeight_);

	//				CallResetBuffer();
	//			}

	//			if (ImGui::SliderFloat("Reflection Roughness", &m_ReflectionRoughness_[0], 0.0f, 1.0f))
	//			{
	//				ObjectManager.SetReflectionRoughness(ObjectManager.GetActiveMaterialId(), m_ReflectionRoughness_);

	//				CallResetBuffer();
	//			}
	//		}
	//	}

	//	ImGui::Spacing();
	//	ImGui::Separator();
	//	ImGui::Spacing();

	//	// Refraction
	//	if (ImGui::CollapsingHeader("Refraction", ImGuiTreeNodeFlags_DefaultOpen))
	//	{
	//		if (ImGui::SliderFloat("Refraction Color", &m_RefractionColor_[0], 0.0f, 1.0f))
	//		{
	//			ObjectManager.SetRefractionColor(ObjectManager.GetActiveMaterialId(), m_RefractionColor_);

	//			CallResetBuffer();
	//		}

	//		if (ImGui::CollapsingHeader("Weight - Roughness"))
	//		{
	//			if (ImGui::SliderFloat("Refraction Weight", &m_RefractionWeight_[0], 0.0f, 1.0f))
	//			{
	//				ObjectManager.SetRefractionWeight(ObjectManager.GetActiveMaterialId(), m_RefractionWeight_);

	//				CallResetBuffer();
	//			}

	//			if (ImGui::SliderFloat("Refraction Roughness", &m_RefractionRoughness_[0], 0.0f, 1.0f))
	//			{
	//				ObjectManager.SetRefractionRoughness(ObjectManager.GetActiveMaterialId(), m_RefractionRoughness_);

	//				CallResetBuffer();
	//			}
	//		}
	//	}

	//	ImGui::Spacing();
	//	ImGui::Separator();
	//	ImGui::Spacing();

	//	if (ImGui::CollapsingHeader("Clear Coat", ImGuiTreeNodeFlags_DefaultOpen))
	//	{
	//		if (ImGui::Checkbox("Clearcoat mode", &m_CoatingMode_))
	//		{
	//			ObjectManager.SetCoatingMode(ObjectManager.GetActiveMaterialId(), m_CoatingMode_);

	//			CallResetBuffer();
	//		}

	//		if (ImGui::SliderFloat("Clear Coat Color", &m_CoatingColor_[0], 0.0f, 1.0f))
	//		{
	//			ObjectManager.SetCoatingColor(ObjectManager.GetActiveMaterialId(), m_CoatingColor_);

	//			CallResetBuffer();
	//		}

	//		if (ImGui::CollapsingHeader("Weight - Roughness"))
	//		{
	//			if (ImGui::SliderFloat("Clear Coat Weight", &m_CoatingWeight_[0], 0.0f, 1.0f))
	//			{
	//				ObjectManager.SetCoatingWeight(ObjectManager.GetActiveMaterialId(), m_CoatingWeight_);

	//				CallResetBuffer();
	//			}

	//			if (ImGui::SliderFloat("Clear Coat Roughness", &m_CoatingRoughness_[0], 0.0f, 1.0f))
	//			{
	//				ObjectManager.SetCoatingRoughness(ObjectManager.GetActiveMaterialId(), m_CoatingRoughness_);

	//				CallResetBuffer();
	//			}
	//		}
	//	}

	//	ImGui::Spacing();
	//	ImGui::Separator();
	//	ImGui::Spacing();

	//	// Sheen
	//	if (ImGui::CollapsingHeader("Sheen", ImGuiTreeNodeFlags_DefaultOpen))
	//	{
	//		if (ImGui::SliderFloat("Sheen Color", &m_Sheen_[0], 0.0f, 1.0f))
	//		{
	//			ObjectManager.SetSheen(ObjectManager.GetActiveMaterialId(), m_Sheen_);

	//			CallResetBuffer();
	//		}

	//		if (ImGui::CollapsingHeader("Weight"))
	//		{
	//			if (ImGui::SliderFloat("Sheen Weight", &m_SheenWeight_[0], 0.0f, 1.0f))
	//			{
	//				ObjectManager.SetSheenWeight(ObjectManager.GetActiveMaterialId(), m_SheenWeight_);

	//				CallResetBuffer();
	//			}
	//		}
	//	}

	//	ImGui::Spacing();
	//	ImGui::Separator();
	//	ImGui::Spacing();

	//	// SSS
	//	if (ImGui::CollapsingHeader("SSS", ImGuiTreeNodeFlags_DefaultOpen))
	//	{
	//		if (ImGui::SliderFloat("SSS Color", &m_SssScatterColor_[0], 0.0f, 1.0f))
	//		{
	//			ObjectManager.SetSssScatterColor(ObjectManager.GetActiveMaterialId(), m_SssScatterColor_);

	//			CallResetBuffer();
	//		}

	//		if (ImGui::CollapsingHeader("Distance - Weight - Backscatter"))
	//		{
	//			if (ImGui::SliderFloat("SSS Distance", &m_SssScatterDistance_[0], 0.0f, 1.0f))
	//			{
	//				ObjectManager.SetSssScatterDistance(ObjectManager.GetActiveMaterialId(), m_SssScatterDistance_);

	//				CallResetBuffer();
	//			}
	//		}
	//	}



	//}






}

//-------------------------------------- GROUP SHAPE INSPECTOR --------------------------------------

void HorusInspector::InspectorGroupShape()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f)); // Green
	ImGui::Text("Group Shape Inspector");
	ImGui::PopStyleColor();
	ImGui::Separator();

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f)); // Green
	ImGui::Text("Group : ");
	ImGui::SameLine();
	ImGui::Text(m_GroupShapeName_.c_str());
	ImGui::PopStyleColor();

	// TODO : Show a text input for the name of the group shape
	//ImGui::Text("Group Shape name: %s ", MeshName.c_str());

	// Only Transform 
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Button("Reset Transform"))
		{
			ObjectManager.SetGroupShapeResetTransform(ObjectManager.GetActiveGroupShapeId());

			PopulateSelectedGroupShapeInfos();
			CallResetBuffer();
		}
		ImGui::SameLine(); ShowHelpMarker("Reset the transform of the group shape.");

		ImGui::Separator();

		if (ImGui::InputFloat3("Position", &m_GroupShapePosition_[0]))
		{
			ObjectManager.SetGroupShapePosition(ObjectManager.GetActiveGroupShapeId(), m_GroupShapePosition_);

			CallResetBuffer();
		}

		if (ImGui::InputFloat3("Rotation", &m_GroupShapeRotation_[0]))
		{
			ObjectManager.SetGroupShapeRotation(ObjectManager.GetActiveGroupShapeId(), m_GroupShapeRotation_);

			CallResetBuffer();
		}

		if (ImGui::InputFloat3("Scale", &m_GroupShapeScale_[0]))
		{
			ObjectManager.SetGroupShapeScale(ObjectManager.GetActiveGroupShapeId(), m_GroupShapeScale_);

			CallResetBuffer();
		}
	}

	// Visualisation
	static bool showNormals = false;
	ImGui::Checkbox("Show Normals", &showNormals);

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f)); // Green
	ImGui::Text("Group Shape Infos");
	ImGui::PopStyleColor();

	ImGui::Separator();

	ImGui::Text("Group shape id: %d ", ObjectManager.GetActiveGroupShapeId());
	ImGui::Text("Group shape name: %s ", m_GroupShapeName_.c_str());
	ImGui::Text("Group shape position: %.2f %.2f %.2f ", m_GroupShapePosition_.x, m_GroupShapePosition_.y, m_GroupShapePosition_.z);
	ImGui::Text("Group shape rotation: %.2f %.2f %.2f ", m_GroupShapeRotation_.x, m_GroupShapeRotation_.y, m_GroupShapeRotation_.z);
	ImGui::Text("Group shape scale: %.2f %.2f %.2f ", m_GroupShapeScale_.x, m_GroupShapeScale_.y, m_GroupShapeScale_.z);
	ImGui::Text("Group shape number of shapes count: %d ", 0);
	ImGui::Text("Group shape all cumuled vertices count: %d ", 0);
	ImGui::Text("Group shape number of materials in : %d ", 0);

	ImGui::Separator();
}

void HorusInspector::PopulateSelectedGroupShapeInfos()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	m_GroupShapeName_ = ObjectManager.GetGroupShapeName(ObjectManager.GetActiveGroupShapeId());

	m_GroupShapePosition_ = ObjectManager.GetGroupShapePosition(ObjectManager.GetActiveGroupShapeId());
	m_GroupShapeRotation_ = ObjectManager.GetGroupShapeRotation(ObjectManager.GetActiveGroupShapeId());
	m_GroupShapeScale_ = ObjectManager.GetGroupShapeScale(ObjectManager.GetActiveGroupShapeId());





}

void HorusInspector::SetGroupShapeName(std::string name)
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	ObjectManager.SetGroupShapeName(ObjectManager.GetActiveGroupShapeId(), name.c_str());

	//CallResetBuffer();
}
void HorusInspector::SetGroupShapePosition(glm::vec3 position)
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	ObjectManager.SetGroupShapePosition(ObjectManager.GetActiveGroupShapeId(), position);

	CallResetBuffer();
}
void HorusInspector::SetGroupShapeRotation(glm::vec3 rotation)
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	ObjectManager.SetGroupShapeRotation(ObjectManager.GetActiveGroupShapeId(), rotation);

	CallResetBuffer();
}
void HorusInspector::SetGroupShapeScale(glm::vec3 scale)
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	ObjectManager.SetGroupShapeScale(ObjectManager.GetActiveGroupShapeId(), scale);

	CallResetBuffer();
}



// -------------------------------------- SHAPE INSPECTOR --------------------------------------

void HorusInspector::InspectorShape()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	//ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "Green");
	//ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.7f, 1.0f), "Pink");

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f)); // Green
	ImGui::Text("Shape Inspector");
	ImGui::PopStyleColor();
	ImGui::Separator();

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f)); // Green
	ImGui::Text("General Informations");
	ImGui::PopStyleColor();

	int vertexCount = 0;
	int faceCount = 0;
	ImGui::Text("Vertices: %d", vertexCount);
	ImGui::Text("Faces: %d", faceCount);

	// Delete shape button
	if (ImGui::Button("Delete Shape"))
	{
		ObjectManager.DeleteSelectedShape(ObjectManager.GetActiveShapeId());

		CallResetBuffer();
	}
	ImGui::SameLine(); ShowHelpMarker("Delete the selected shape. Attention! No undo possible.");

	if (ImGui::CollapsingHeader("Transform", InputFloatFlags))
	{
		if (ImGui::Button("Reset Transform"))
		{
			ObjectManager.SetShapeResetTransformById(ObjectManager.GetActiveShapeId());

			CallResetBuffer();
		}
		ImGui::SameLine(); ShowHelpMarker("Reset the transform of the shape.");

		ImGui::Separator();

		if (ImGui::InputFloat3("Position", &m_ShapePosition_[0], "%.1f", InputFloatFlags))
		{
			ObjectManager.SetShapePositionById(ObjectManager.GetActiveShapeId(), m_ShapePosition_);

			spdlog::debug("Shape ID: {}", ObjectManager.GetActiveShapeId());

			spdlog::debug("Shape position set : {0} {1} {2}", m_ShapePosition_.x, m_ShapePosition_.y, m_ShapePosition_.z);
			CallResetBuffer();
		}

		if (ImGui::InputFloat3("Rotation", &m_ShapeRotation_[0], "%.1f", InputFloatFlags))
		{
			ObjectManager.SetShapeRotationById(ObjectManager.GetActiveShapeId(), m_ShapeRotation_);
			spdlog::debug("Shape rotation set : {0} {1} {2}", m_ShapeRotation_.x, m_ShapeRotation_.y, m_ShapeRotation_.z);
			CallResetBuffer();
		}

		if (ImGui::InputFloat3("Scale", &m_ShapeScale_[0], "%.1f", InputFloatFlags))
		{
			ObjectManager.SetShapeScaleById(ObjectManager.GetActiveShapeId(), m_ShapeScale_);
			spdlog::debug("Shape scale set : {0} {1} {2}", m_ShapeScale_.x, m_ShapeScale_.y, m_ShapeScale_.z);
			CallResetBuffer();
		}
	}

	ImGui::Text("Materials");
	// TODO : See the assigned materials here

	if (ImGui::Button("Recalculate Normals"))
	{

	}
	ImGui::SameLine(); ShowHelpMarker("Recalculate the normals of the shape.");

	// Visualisation
	static bool showNormals = false;
	ImGui::Checkbox("Show Normals", &showNormals);

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f)); // Green
	ImGui::Text("Shape Infos");
	ImGui::PopStyleColor();

	ImGui::Separator();

	ImGui::Text("Shape id: %d ", ObjectManager.GetActiveShapeId());
	ImGui::Text("Shape name: %s ", m_ShapeName_.c_str());
	ImGui::Text("Shape position: %.2f %.2f %.2f ", m_ShapePosition_.x, m_ShapePosition_.y, m_ShapePosition_.z);
	ImGui::Text("Shape rotation: %.2f %.2f %.2f ", m_ShapeRotation_.x, m_ShapeRotation_.y, m_ShapeRotation_.z);
	ImGui::Text("Shape scale: %.2f %.2f %.2f ", m_ShapeScale_.x, m_ShapeScale_.y, m_ShapeScale_.z);
	ImGui::Text("Shape vertex count: %d ", vertexCount);
	ImGui::Text("Shape face count: %d ", faceCount);
	ImGui::Text("Shape materials: %d ", 0);

	ImGui::Separator();
}

void HorusInspector::PopulateSelectedShapeInfos()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	m_ShapeName_ = ObjectManager.GetShapeNameById(ObjectManager.GetActiveShapeId());
	m_ShapePosition_ = ObjectManager.GetShapePositionById(ObjectManager.GetActiveShapeId());
	m_ShapeRotation_ = ObjectManager.GetShapeRotationById(ObjectManager.GetActiveShapeId());
	m_ShapeScale_ = ObjectManager.GetShapeScaleById(ObjectManager.GetActiveShapeId());



}

//-------------------------------------- TEXTURE INSPECTOR --------------------------------------

void HorusInspector::InspectorTexture()
{
	ImGui::Text("Texture Inspector");
	ImGui::Separator();

	/*GLuint textureId = 0;
	ImGui::Image((void*)(intptr_t)textureId, ImVec2(100, 100));

	static char texturePath[128] = "path/to/texture.png";
	ImGui::InputText("Path", texturePath, IM_ARRAYSIZE(texturePath));

	int width = 0;
	int height = 0;
	ImGui::Text("Resolution: %dx%d", width, height);

	static bool repeat = true;
	ImGui::Checkbox("Repeat", &repeat);

	static bool mirror = false;
	ImGui::Checkbox("Mirror", &mirror);

	static bool useMipmaps = false;
	ImGui::Checkbox("Use Mipmaps", &useMipmaps);

	if (ImGui::Button("Reload Texture")) {

	}
	if (ImGui::Button("Save Changes")) {

	}

	ImGui::Spacing();*/
}

//-------------------------------------- PROJECT INSPECTOR --------------------------------------

void HorusInspector::InspectorProjectProperty()
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f)); // Green
	ImGui::Text("Project Inspector");
	ImGui::PopStyleColor();

	m_ResetBuffer_ = false;

	// Render Informations
	{
		//rprContextGetInfo(Radeon.GetContext(), RPR_CONTEXT_GPU0_NAME, sizeof(m_Gpu00N_), m_Gpu00N_, nullptr); // TODO : Fix exception here
		//rprContextGetInfo(Radeon.GetContext(), RPR_CONTEXT_GPU1_NAME, sizeof(m_Gpu01N_), m_Gpu01N_, nullptr); // TODO : Fix exception here
		//rprContextGetInfo(Radeon.GetContext(), RPR_CONTEXT_RENDER_STATISTICS, sizeof(m_RenderStatistics_), &m_RenderStatistics_, 0); // TODO : Fix exception here

		//ImGui::Begin("Property Panel");

		// gpu name
		/*rpr_longlong MemoryUsage = m_RenderStatistics_.gpumem_usage / 1024 / 1024;
		rpr_longlong SystemMemoryUsage = m_RenderStatistics_.sysmem_usage / 1024 / 1024;
		rpr_longlong GpuMaxAllocation = m_RenderStatistics_.gpumem_max_allocation / 1024 / 1024;
		rpr_longlong GpuTotal = m_RenderStatistics_.gpumem_total / 1024 / 1024;*/

		/*long long MemoryUsage = 0;
		long long SystemMemoryUsage = 0;
		long long GpuMaxAllocation = 0;
		long long GpuTotal = 0;*/

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		if (ImGui::CollapsingHeader("Render Info"))
		{
			ImGui::Text("GPU N01 : %s", m_Gpu00N_);
			ImGui::Text("GPU N02 : %s", m_Gpu01N_);
			ImGui::Separator();
			ImGui::Text("Samples: %d", Radeon.GetSampleCount()); // %d is a placeholder for an integer
			ImGui::Separator();
			/*ImGui::Text("System memory usage : %d MB", SystemMemoryUsage);
			ImGui::Text("GPU Memory usage : %d MB", MemoryUsage);
			ImGui::Text("GPU max allocation : %d MB", GpuMaxAllocation);
			ImGui::Text("GPU total : %d MB", GpuTotal);*/
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
	}

	// Render Modes
	{
		ImGui::Text("Select render mode :");
		ImGui::SameLine();
		if (ImGui::Combo("Render mode", &m_SelectedRenderMode_, m_RenderModes_, IM_ARRAYSIZE(m_RenderModes_)))
		{
			Radeon.SetVisualizationRenderMode(m_SelectedRenderMode_);
			CallResetBuffer();
		}
	}

	ShowBigSeparator();

	// Show AOVs
	{
		ImGui::Spacing();

		const char* AovItems[] = {
	"No AOVs",
	"Opacity",
	"World Coordinate",
	"UV",
	"Material Index",
	"Geometric Normal",
	"Shading Normal",
	"Depth",
	"Object Index",
	"Object Group ID",
	"Shadow Catcher",
	"Background",
	"Emission",
	"Velocity",
	"Direct Illumination",
	"Indirect Illumination",
	"Ambient Occlusion",
	"Direct Diffuse",
	"Direct Reflect",
	"Indirect Diffuse",
	"Indirect Reflect",
	"Refract",
	"Volume",
	"Diffuse Albedo",
	"Variance",
	"View Shading Normal",
	"Reflection Catcher",
	"Light Group 0",
	"Light Group 1",
	"Light Group 2",
	"Light Group 3",
	"Light Group 4",
	"Light Group 5",
	"Light Group 6",
	"Light Group 7",
	"Light Group 8",
	"Light Group 9",
	"Light Group 10",
	"Light Group 11",
	"Light Group 12",
	"Light Group 13",
	"Light Group 14",
	"Light Group 15"
		};

		ImGui::Text("Select AOV :");
		ImGui::SameLine();
		if (ImGui::Combo("AOVs", &m_SelectedAov_, AovItems, IM_ARRAYSIZE(AovItems)))
		{
			Radeon.SetShowAOVsMode(m_SelectedAov_);
			CallResetBuffer();
		}
	}


	ShowBigSeparator();

	// Background image and transparent background
	{
		ImGui::TextUnformatted("Path: ");
		ImGui::SameLine();
		ImGui::InputText("##path", m_BackgroundPath_.data(), sizeof(m_BackgroundPath_));
		ImGui::SameLine();
		if (ImGui::Button("Browse##path"))
		{
			std::string FilePath = Utils::HorusFileDialog::OpenFile("Image (*.*)\0*.jpg\0*.png\0*.exr\0*.tiff\0");
			if (!FilePath.empty())
			{
				spdlog::info("Open file : {}", FilePath);
			}

			m_BackgroundPath_ = FilePath;
		}
		ImGui::SameLine();
		// Clear Path button
		if (ImGui::Button("Clear##path"))
		{
			m_BackgroundPath_.clear();
		}

		if (ImGui::Checkbox("Background image", &m_EnableBackdropImage_))
		{
			m_ResetBuffer_ = true;
			if (m_EnableBackdropImage_ != m_PreviousEnableBackdropImage_)
			{
				m_PreviousEnableBackdropImage_ = m_EnableBackdropImage_;
				if (m_EnableBackdropImage_)
				{
					if (m_BackgroundPath_ != m_LastBackgroundPath_ || m_JustDisabledBackdropImage_)
					{
						ObjectManager.SetBackgroundImage(m_BackgroundPath_);
						m_LastBackgroundPath_ = m_BackgroundPath_;
						m_JustDisabledBackdropImage_ = false;
						CallResetBuffer();
					}
					m_WasBackdropImageEnabled_ = true;
				}
				else
				{
					if (m_WasBackdropImageEnabled_)
					{
						ObjectManager.UnsetBackgroundImage();
						m_WasBackdropImageEnabled_ = false;
						m_JustDisabledBackdropImage_ = true;
						CallResetBuffer();
					}
				}
			}
		}
		ImGui::TextColored(m_WasBackdropImageEnabled_ ? ImVec4(0.6f, 1.0f, 0.6f, 1.0f) : ImVec4(1.0f, 0.6f, 0.7f, 1.0f),
			m_WasBackdropImageEnabled_ ? "Backdrop image is enabled" : "Backdrop image is disabled");

		ShowBigSeparator();

		static bool WasTransparentBackgroundEnabled = false;

		if (bool CheckboxTransparentBackgroundChanged = ImGui::Checkbox("Transparent background", &m_EnableTransparentBackground_))
		{
			if (m_EnableTransparentBackground_ && !WasTransparentBackgroundEnabled)
			{
				rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_TRANSPARENT_BACKGROUND, 1);
				spdlog::info("Transparent background enabled");
				Console.AddLog(" [info] Transparent background enabled");
				WasTransparentBackgroundEnabled = true;
				m_ResetBuffer_ = true;
			}
			else if (!m_EnableTransparentBackground_ && WasTransparentBackgroundEnabled)
			{
				rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_TRANSPARENT_BACKGROUND, 0);
				spdlog::info("Transparent background disabled");
				Console.AddLog(" [info] Transparent background disabled");
				WasTransparentBackgroundEnabled = false;
				m_ResetBuffer_ = true;
			}
		}

		if (WasTransparentBackgroundEnabled)
		{
			ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "Transparent background is enabled");
		}
		else
		{
			ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.7f, 1.0f), "Transparent background is disabled");
		}
	}

	ShowBigSeparator();

	// Render settings
	{
		if (ImGui::CollapsingHeader("Render settings", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::BeginTabBar("rnds_settings"))
			{
				if (ImGui::BeginTabItem("Sampling"))
				{
					ShowBigSeparator();

					if (ImGui::CollapsingHeader("Sampling", ImGuiTreeNodeFlags_DefaultOpen))
					{
						// Max samples
						ImGui::Text("Min samples: %d", m_MinSamples_);
						ImGui::SameLine();
						if (ImGui::SliderInt("Min samples", &m_MinSamples_, 1, 99))
						{

							if (m_MinSamples_ > m_MaxSamples_)
							{
								m_MinSamples_ = m_MaxSamples_;
							}

							Radeon.SetMinSamples(m_MinSamples_);
							CallResetBuffer();
							PopulateSelectedProjectInfos();
						}

						// Max samples
						ImGui::Text("Max samples: %d", m_MaxSamples_);
						ImGui::SameLine();
						if (ImGui::SliderInt("Max samples", &m_MaxSamples_, 1, 1024))
						{
							if (m_MaxSamples_ < m_MinSamples_)
							{
								m_MaxSamples_ = m_MinSamples_;
							}

							Radeon.SetMaxSamples(m_MaxSamples_);
							CallResetBuffer();
							PopulateSelectedProjectInfos();
						}

						ShowBigSeparator();

						// Russian roulette
						// Russian roulette
						if (ImGui::Checkbox("Russian Roulette", &m_EnableRussianRoulette_))
						{
							m_ResetBuffer_ = m_EnableRussianRoulette_ != m_PreviousEnableRussianRoulette_;
							m_PreviousEnableRussianRoulette_ = m_EnableRussianRoulette_;
						}

						ImVec4 rrColor = m_EnableRussianRoulette_ ? ImVec4(0.6f, 1.0f, 0.6f, 1.0f) : ImVec4(1.0f, 0.6f, 0.7f, 1.0f);
						ImGui::TextColored(rrColor, m_EnableRussianRoulette_ ? "Russian roulette is enabled" : "Russian roulette is disabled");

						if (m_EnableRussianRoulette_)
						{
							static float RussianRouletteDepth = 5;
							if (ImGui::SliderFloat("Depth", &RussianRouletteDepth, 1.0f, 20.0f))
							{
								rprContextSetParameterByKey1f(Radeon.GetContext(), RPR_CONTEXT_RUSSIAN_ROULETTE_DEPTH, RussianRouletteDepth);
								CallResetBuffer();
							}
						}

						ShowBigSeparator();

						// Handle adaptive sampling checkbox and update render mode
						if (ImGui::Checkbox("Adaptive sampling", &m_EnableAdaptiveSampling_))
						{
							CallResetBuffer();
							if (m_EnableAdaptiveSampling_)
							{
								Radeon.SetAdaptiveRender();
							}
							else {
								Radeon.SetClassicRender();
							}
						}

						ImVec4 color = m_EnableAdaptiveSampling_ ? ImVec4(0.6f, 1.0f, 0.6f, 1.0f) : ImVec4(1.0f, 0.6f, 0.7f, 1.0f);
						ImGui::TextColored(color, m_EnableAdaptiveSampling_ ? "Adaptive sampling is enabled" : "Adaptive sampling is disabled");

						// Display and handle adaptive threshold slider
						ImGui::Text("Threshold: %.2f", m_AdaptiveThreshold_);
						ImGui::SameLine();
						if (ImGui::SliderFloat("Threshold", &m_AdaptiveThreshold_, 0.01f, 0.5f, "%.1f"))
						{
							Radeon.SetAdaptiveSamplingThreshold(m_AdaptiveThreshold_);
							CallResetBuffer();
						}
					}

					ShowBigSeparator();

					// checkbox for enable preview mode
					// Handle the enable preview checkbox and update preview mode settings
					if (bool CheckboxChanged = ImGui::Checkbox("Enable preview", &m_EnablePreviewMode_); CheckboxChanged && (m_EnablePreviewMode_ != m_PreviousResetMode_)) {
						m_ResetBuffer_ = true;
						m_PreviousResetMode_ = m_EnablePreviewMode_;
						Radeon.SetLockPreviewMode(m_EnablePreviewMode_);
						PopulateSelectedProjectInfos();
					}

					// Display preview mode status
					ImVec4 colorPM = m_EnablePreviewMode_ ? ImVec4(0.6f, 1.0f, 0.6f, 1.0f) : ImVec4(1.0f, 0.6f, 0.7f, 1.0f);
					ImGui::TextColored(colorPM, m_EnablePreviewMode_ ? "Preview mode is enabled" : "Preview mode is disabled");

					ShowBigSeparator();

					// Anti Aliasing
					bool CheckboxAaChanged = ImGui::Checkbox("Enable AA", &m_EnableAa_);
					if (CheckboxAaChanged) {
						m_ResetBuffer_ = true;
						m_PreviousEnableAa_ = m_EnableAa_;
						rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_AA_ENABLED, m_EnableAa_ ? 1 : 0);
					}

					ImVec4 colorAA = m_EnableAa_ ? ImVec4(0.6f, 1.0f, 0.6f, 1.0f) : ImVec4(1.0f, 0.6f, 0.7f, 1.0f);
					ImGui::TextColored(colorAA, m_EnableAa_ ? "Anti aliasing is enabled" : "Anti aliasing is disabled");

					// Display and handle filter 
					const char* Filters[] = { "Box", "Triangle", "Gaussian", "Mitchell", "Lanczos", "Blackmanharris", "None" };
					static int SelectedFilter = 2;

					ImGui::Text("Select filter :");
					if (ImGui::Combo("Filter", &SelectedFilter, Filters, IM_ARRAYSIZE(Filters)))
					{
						switch (SelectedFilter)
						{
						case 0:
							rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_IMAGE_FILTER_TYPE, RPR_FILTER_BOX);
							break;
						case 1:
							rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_IMAGE_FILTER_TYPE, RPR_FILTER_TRIANGLE);
							break;
						case 2:
							rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_IMAGE_FILTER_TYPE, RPR_FILTER_GAUSSIAN);
							break;
						case 3:
							rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_IMAGE_FILTER_TYPE, RPR_FILTER_MITCHELL);
							break;
						case 4:
							rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_IMAGE_FILTER_TYPE, RPR_FILTER_LANCZOS);
							break;
						case 5:
							rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_IMAGE_FILTER_TYPE, RPR_FILTER_BLACKMANHARRIS);
							break;
						case 6:
							rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_IMAGE_FILTER_TYPE, RPR_FILTER_NONE);
							break;
						}
						CallResetBuffer();
					}

					ShowBigSeparator();

					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Globals"))
				{
					ShowBigSeparator();

					// Trace depth
					if (ImGui::CollapsingHeader("Trace Depth", ImGuiTreeNodeFlags_DefaultOpen))
					{
						static int maxDepthDiffuse = 1;
						if (ImGui::SliderInt("Diffuse Ray Depth", &maxDepthDiffuse, 1, 64))
						{
							rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_MAX_DEPTH_DIFFUSE, maxDepthDiffuse);
							CallResetBuffer();
						}

						static int maxDepthGlossy = 1;
						if (ImGui::SliderInt("Reflection Ray Depth", &maxDepthGlossy, 1, 64))
						{
							rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_MAX_DEPTH_GLOSSY, maxDepthGlossy);
							CallResetBuffer();
						}

						static int maxDepthRefraction = 1;
						if (ImGui::SliderInt("Refraction Ray Depth", &maxDepthRefraction, 1, 64))
						{
							rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_MAX_DEPTH_REFRACTION, maxDepthRefraction);
							CallResetBuffer();
						}

						static int maxDepthGlossyRefraction = 1;
						if (ImGui::SliderInt("Glossy Refraction Ray Depth", &maxDepthGlossyRefraction, 1, 64))
						{
							rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_MAX_DEPTH_GLOSSY_REFRACTION, maxDepthGlossyRefraction);
							CallResetBuffer();
						}

						static int maxDepthShadow = 1;
						if (ImGui::SliderInt("Shadow Ray Depth", &maxDepthShadow, 1, 64))
						{
							rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_MAX_DEPTH_SHADOW, maxDepthShadow);
							CallResetBuffer();
						}
					}

					// Color management

					if (ImGui::CollapsingHeader("Color Management", ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::Text("Color Space");


					}

					// Options

					if (ImGui::CollapsingHeader("Options", ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::Text("Color Space");


					}

					// Overrides

					if (ImGui::CollapsingHeader("Override", ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::Text("Color Space");


					}

					// material override

					if (ImGui::CollapsingHeader("Material Override", ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::Text("Color Space");


					}

					ShowBigSeparator();

					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Motion blur"))
				{
					ShowBigSeparator();

					if (ImGui::CollapsingHeader("Motion Blur", ImGuiTreeNodeFlags_DefaultOpen))
					{


					}

					if (ImGui::CollapsingHeader("Shutter", ImGuiTreeNodeFlags_DefaultOpen))
					{


					}

					ShowBigSeparator();

					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("GI"))
				{
					ShowBigSeparator();

					if (ImGui::CollapsingHeader("GI", ImGuiTreeNodeFlags_DefaultOpen))
					{
						static int MaxRecursion = 4;

						if (ImGui::SliderInt("Trace Depth", &MaxRecursion, 1, 64))
						{
							rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_MAX_RECURSION, MaxRecursion);
							CallResetBuffer();
						}
					}

					ShowBigSeparator();

					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Caustics"))
				{
					ShowBigSeparator();

					if (ImGui::CollapsingHeader("Caustics", ImGuiTreeNodeFlags_DefaultOpen))
					{



					}

					if (ImGui::CollapsingHeader("Trace Depth", ImGuiTreeNodeFlags_DefaultOpen))
					{



					}

					ShowBigSeparator();

					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("AOV"))
				{
					ShowBigSeparator();

					if (ImGui::CollapsingHeader("AOVs", ImGuiTreeNodeFlags_DefaultOpen))
					{

					}

					ImGui::Spacing();

					if (ImGui::CollapsingHeader("Active AOVs", ImGuiTreeNodeFlags_DefaultOpen))
					{



					}

					ImGui::Spacing();

					if (ImGui::CollapsingHeader("Processing", ImGuiTreeNodeFlags_DefaultOpen))
					{
						if (ImGui::CollapsingHeader("Deep Output", ImGuiTreeNodeFlags_DefaultOpen))
						{


						}

					}

					ShowBigSeparator();

					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Optimizations"))
				{
					ShowBigSeparator();

					// SSS
					if (ImGui::CollapsingHeader("Sub-Surface Scattering", ImGuiTreeNodeFlags_DefaultOpen))
					{


					}

					// Cut-off thresholds
					if (ImGui::CollapsingHeader("Cut-off Thresholds", ImGuiTreeNodeFlags_DefaultOpen))
					{


					}

					// Ray tracing acceleration
					if (ImGui::CollapsingHeader("Ray Tracing Acceleration", ImGuiTreeNodeFlags_DefaultOpen))
					{

					}

					ShowBigSeparator();

					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("System"))
				{
					ShowBigSeparator();

					// Log and feedback
					if (ImGui::CollapsingHeader("Log and Feedback", ImGuiTreeNodeFlags_DefaultOpen))
					{


					}

					// Tile rendering
					if (ImGui::CollapsingHeader("Tile Rendering", ImGuiTreeNodeFlags_DefaultOpen))
					{


					}

					// Shadow Linking
					if (ImGui::CollapsingHeader("Shadow Linking", ImGuiTreeNodeFlags_DefaultOpen))
					{

					}

					// Memory
					if (ImGui::CollapsingHeader("Memory", ImGuiTreeNodeFlags_DefaultOpen))
					{
						/*static float rayCastEpsilon = 0.002f;
						if (ImGui::SliderFloat("Ray Cast Epsilon", &rayCastEpsilon, 0.0f, 1.0f, "%.5f"))
						{
							rprContextSetParameterByKey1f(context, RPR_CONTEXT_RAY_CAST_EPISLON, rayCastEpsilon);
							CallResetBuffer();
						}*/

						static int CpuThreadLimit = 4;
						if (ImGui::SliderInt("CPU Thread limit", &CpuThreadLimit, 1, 16))
						{
							rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_CPU_THREAD_LIMIT, CpuThreadLimit);
							CallResetBuffer();
						}

						static int gpuMemoryLimitMB = 1024;
						if (ImGui::SliderInt("GPU Thread limit (MB)", &gpuMemoryLimitMB, 256, 16384))
						{
							rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_GPU_MEMORY_LIMIT, gpuMemoryLimitMB);
							CallResetBuffer();
						}
					}

					// Global preferences
					if (ImGui::CollapsingHeader("Global Preferences", ImGuiTreeNodeFlags_DefaultOpen))
					{


					}

					// Compute devices
					if (ImGui::CollapsingHeader("Compute Devices", ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::Text("GPU name: %s", m_Gpu00N_);
						ImGui::SameLine();
						ImGui::Text("GPU name: %s", m_Gpu01N_);
					}

					ShowBigSeparator();

					ImGui::EndTabItem();
				}

			}

			ImGui::EndTabBar();
		}
	}

	ShowBigSeparator();

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f)); // Green
	ImGui::Text("Export Settings");
	ImGui::PopStyleColor();

	ShowBigSeparator();

	if (ImGui::CollapsingHeader("Export render", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// Export render logic
		static int ExportFormat = 0;
		rpr_char const* Formats[] = { ".png", ".jpeg", ".tiff", ".bmp", ".exr", ".hdr", ".tga" };
		const int NumFormats = std::size(Formats);

		std::string BaseFilename = "HorusRender_ExportedFile";
		std::string Filename = BaseFilename + std::string(Formats[ExportFormat]);

		if (ImGui::BeginCombo("Export format", Formats[ExportFormat]))
		{
			for (int i = 0; i < NumFormats; i++)
			{
				bool IsSelected = (ExportFormat == i);
				if (ImGui::Selectable(Formats[i], IsSelected))
				{
					ExportFormat = i;
				}
				if (IsSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		ImGui::InputText("Filename", m_UserInput_, 256);
		if (ImGui::Button("Browse##ExportPath"))
		{
			std::string FilePath = Utils::HorusFileDialog::OpenFolder();
			if (!FilePath.empty())
			{
				spdlog::info("Save file : {}", FilePath);
				strncpy(m_UserInput_, FilePath.c_str(), sizeof(m_UserInput_));
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Export"))
		{
			std::string ExportPath = m_UserInput_;
			std::string Extension = std::string(Formats[ExportFormat]);
			std::string Filename;

			auto GenerateFinalFilename = [&](const std::string& BasePath)
				{
					std::string Result;
					if (std::filesystem::exists(BasePath + Extension))
					{
						m_Suffix_++;
						Result = BasePath + ("_" + std::to_string(m_Suffix_) + "_" + std::to_string(Radeon.GetSampleCount()) + Extension);
						spdlog::warn("File already exists, renaming to: {}", Result);
						Console.AddLog(" [warning] File already exists, renaming to : %s ", Result.c_str());
					}
					else
					{
						m_Suffix_ = 0;
						Result = BasePath + ("_" + std::to_string(Radeon.GetSampleCount()) + Extension);
						spdlog::info("Render exported with success: {}", Result);
						Console.AddLog(" [success] Render exported with success: %s ", Result.c_str());
					}
					return Result;
				};

			if (!ExportPath.empty())
			{
				Filename = GenerateFinalFilename(ExportPath);
			}
			else
			{
				std::string BaseFilename = "Render";
				Filename = GenerateFinalFilename(BaseFilename);
			}

			if (rpr_int Status = rprFrameBufferSaveToFile(Radeon.GetFrameBufferResolved(), Filename.c_str()); Status != RPR_SUCCESS)
			{
				spdlog::error("Failed to save frame buffer to file: {}", Filename);
				Console.AddLog(" [error] Failed to save frame buffer to file: %s ", Filename.c_str());
			}
		}

		{
			// Get object logic
		/*ImGui::Text("Get scene components");
		ImGui::Separator();
		ImGui::Text("Camera         : %d", camera_number_);
		ImGui::Separator();
		ImGui::Text("Light          : %d", light_number_);
		ImGui::Separator();
		ImGui::Text("Mesh           : %d", mesh_number_);
		ImGui::Separator();
		ImGui::Text("Grid           : %d", grid_number_);
		ImGui::Separator();
		ImGui::Text("Buffer         : %d", buffer_number_);
		ImGui::Separator();
		ImGui::Text("Image          : %d", image_number_);
		ImGui::Separator();
		ImGui::Text("FrameBuffer    : %d", framebuffer_number_);
		ImGui::Separator();
		ImGui::Text("Scene          : %d", scene_number_);
		ImGui::Separator();
		ImGui::Text("Curve          : %d", curve_number_);
		ImGui::Separator();
		ImGui::Text("Material Sys   : %d", material_node_number_);
		ImGui::Separator();
		ImGui::Text("Composite      : %d", composite_number_);
		ImGui::Separator();
		ImGui::Text("LUT            : %d", lut_number_);
		ImGui::Separator();*/
		}

	}

	ShowBigSeparator();
}

void HorusInspector::ShowHelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}
void HorusInspector::ShowBigSeparator()
{
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
}

// Project
void HorusInspector::PopulateSelectedProjectInfos()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusRadeon& Radeon = HorusRadeon::GetInstance();

	// Populate Project Infos
	m_Gpu00N_[0] = 0;
	m_Gpu01N_[0] = 0;

	CHECK(rprContextGetInfo(Radeon.GetContext(), RPR_CONTEXT_GPU0_NAME, sizeof(m_Gpu00N_), m_Gpu00N_, 0));
	CHECK(rprContextGetInfo(Radeon.GetContext(), RPR_CONTEXT_GPU1_NAME, sizeof(m_Gpu01N_), m_Gpu01N_, 0));

	m_MinSamples_ = Radeon.GetMinSamples();
	m_MaxSamples_ = Radeon.GetMaxSamples();
	m_SelectedRenderMode_ = Radeon.GetVisualizationRenderMode();





}

// Project Getters
// Project Setters