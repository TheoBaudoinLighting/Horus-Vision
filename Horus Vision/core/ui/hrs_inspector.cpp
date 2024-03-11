
#include "hrs_platform.h"
#include "hrs_inspector.h"
#include "hrs_radeon.h" // glfw3.h
#include "hrs_object_manager.h" // glfw3.h
#include "hrs_utils.h"

#include <string>
#include <functional> 

#include <filesystem>
#include <hrs_console.h>

#include "hrs_dragdrop.h"
#include "hrs_horus_parameters.h"
#include "hrs_om_camera.h"
#include "RadeonImageFilter/include/stb_image_write.h"

#pragma warning(disable: 4002)

using UpdateCallback = std::function<void(const std::string&)>; // Callback for update Color or File path

void HorusInspector::Inspector(bool* p_open)
{
	ImGui::Begin(HORUS_UI_NAME_INSPECTOR.c_str(), p_open);

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
	case InspectorType::NONE:
		InspectorNone();
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
	HorusOmCamera& Camera = HorusOmCamera::GetInstance();
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
			Camera.PrintCameraInfo(Camera.GetActiveCameraID());
		}

		if (ImGui::Button("Reset camera"))
		{
			Camera.ResetCamera(Camera.GetActiveCameraID());

			CallResetBuffer();
		}

		ImGui::Separator();

		if (ImGui::Button("Move Camera Forward"))
		{
			Camera.SetMoveCameraForward(Camera.GetActiveCameraID());

			CallResetBuffer();
		}

		ImGui::SameLine();

		if (ImGui::Button("Move Camera Backward"))
		{
			Camera.SetMoveCameraBackward(Camera.GetActiveCameraID());

			CallResetBuffer();
		}

		if (ImGui::Button("Move Camera Left"))
		{
			Camera.SetMoveCameraLeft(Camera.GetActiveCameraID());

			CallResetBuffer();
		}

		ImGui::SameLine();

		if (ImGui::Button("Move Camera Right"))
		{
			Camera.SetMoveCameraRight(Camera.GetActiveCameraID());

			CallResetBuffer();
		}

		if (ImGui::Button("Move Camera Up"))
		{
			Camera.SetMoveCameraUp(Camera.GetActiveCameraID());

			CallResetBuffer();
		}

		ImGui::SameLine();

		if (ImGui::Button("Move Camera Down"))
		{
			Camera.SetMoveCameraDown(Camera.GetActiveCameraID());

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
			Camera.PrintCameraInfo(Camera.GetActiveCameraID());
		}
		ImGui::SameLine(); ShowHelpMarker("Print the camera info in the console.");

		if (ImGui::Button("Reset camera"))
		{
			Camera.ResetCamera(Camera.GetActiveCameraID());

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

	/*ImGui::InputFloat("Near Clip", &m_CameraNear_, ImGuiInputTextFlags_EnterReturnsTrue);
	ImGui::SameLine(); ShowHelpMarker("Near clip plane of the camera.");
	ImGui::InputFloat("Far Clip", &m_CameraFar_, ImGuiInputTextFlags_EnterReturnsTrue);
	ImGui::SameLine(); ShowHelpMarker("Far clip plane of the camera.");*/

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
			if (m_CameraFStop_ <= 0.0f)
			{
				m_CameraFStop_ = 0.001f;
			}

			PopulateSelectedCameraInfos();
			Camera.SetCameraFStop(Camera.GetActiveCameraID(), m_CameraFStop_);
			SetFocusPlaneToFocusPosition(m_FocusPlaneShape_);
			CallResetBuffer();
		}
		else
		{
			// Set FStop to 100 to disable DOF effect, because 100 is a very high value for FStop
			// (not really disable but it will be very hard to see)
			Camera.SetCameraFStop(Camera.GetActiveCameraID(), 100);
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
			Camera.SetCameraFocusDistance(Camera.GetActiveCameraID(), m_FocusPlaneDistance_);
			SetFocusPlaneToFocusPosition(m_FocusPlaneShape_);
			CallResetBuffer();
		}
		ImGui::SameLine(); ShowHelpMarker("Set focal plane distance for DOF effect. The focal plane is the plane that is in focus. The distance is measured from the camera position along the camera look at vector.");

		// Set FStop
		if (ImGui::SliderFloat("FStop", &m_CameraFStop_, 0.01f, 10.0f, "%.3f"))
		{
			if (m_CameraFStop_ <= 0.0f)
			{
				m_CameraFStop_ = 0.01f;
			}

			Camera.SetCameraFStop(Camera.GetActiveCameraID(), m_CameraFStop_);

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

			Camera.SetCameraApertureBlades(Camera.GetActiveCameraID(), m_ApertureBlades_);

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

	ImGui::Text("Camera id: %d ", Camera.GetActiveCameraID());
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
	HorusOmCamera& Camera = HorusOmCamera::GetInstance();

	// Getters Camera
	m_CameraName_ = Camera.GetActiveCameraName();

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
	HorusOmCamera& Camera = HorusOmCamera::GetInstance();

	int ActiveCamera = Camera.GetActiveCameraID();

	if (ActiveCamera != -1)
	{
		return Camera.GetCameraLookAt(ActiveCamera);
	}
	else
	{
		return { 0.0f, 0.0f, 0.0f };
	}
}
glm::vec3 HorusInspector::GetCameraPosition()
{
	HorusOmCamera& Camera = HorusOmCamera::GetInstance();

	int ActiveCamera = Camera.GetActiveCameraID();

	if (ActiveCamera != -1)
	{
		return Camera.GetCameraPosition(ActiveCamera);
	}
	else
	{
		return { 0.0f, 0.0f, 0.0f };
	}

}
glm::vec3 HorusInspector::GetCameraRotation()
{
	HorusOmCamera& Camera = HorusOmCamera::GetInstance();

	int ActiveCamera = Camera.GetActiveCameraID();

	if (ActiveCamera != -1)
	{
		return Camera.GetCameraRotation(ActiveCamera);
	}
	else
	{
		return { 0.0f, 0.0f, 0.0f };
	}
}
glm::vec3 HorusInspector::GetCameraScale()
{
	HorusOmCamera& Camera = HorusOmCamera::GetInstance();
	
	int ActiveCamera = Camera.GetActiveCameraID();

	if (ActiveCamera != -1)
	{
		return Camera.GetCameraScale(ActiveCamera);
	}
	else
	{
		return { 0.0f, 0.0f, 0.0f };
	}
}
float HorusInspector::GetCameraNear()
{
	HorusOmCamera& Camera = HorusOmCamera::GetInstance();

	int ActiveCamera = Camera.GetActiveCameraID();

	if (ActiveCamera != -1)
	{
		return Camera.GetCameraNear(ActiveCamera);
	}
	else
	{
		return 0.0f;
	}
}
float HorusInspector::GetCameraFar()
{
	HorusOmCamera& Camera = HorusOmCamera::GetInstance();

	int ActiveCamera = Camera.GetActiveCameraID();

	if (ActiveCamera != -1)
	{
		return Camera.GetCameraFar(ActiveCamera);
	}
	else
	{
		return 0.0f;
	}
}
float HorusInspector::GetFov()
{
	HorusOmCamera& Camera = HorusOmCamera::GetInstance();

	int ActiveCamera = Camera.GetActiveCameraID();

	if (ActiveCamera != -1)
	{
		return Camera.GetCameraFov(ActiveCamera);
	}
	else
	{
		return 0.0f;
	}
}
float HorusInspector::GetFStop()
{
	HorusOmCamera& Camera = HorusOmCamera::GetInstance();

	int ActiveCamera = Camera.GetActiveCameraID();

	if (ActiveCamera != -1)
	{
		return Camera.GetCameraFStop(ActiveCamera);
	}
	else
	{
		return 0.0f;
	}
}
float HorusInspector::GetFocusDistance()
{
	HorusOmCamera& Camera = HorusOmCamera::GetInstance();

	int ActiveCamera = Camera.GetActiveCameraID();

	if (ActiveCamera != -1)
	{
		return Camera.GetCameraFocusDistance(ActiveCamera);
	}
	else
	{
		return 0.0f;
	}
}
int HorusInspector::GetApertureBlades()
{
	HorusOmCamera& Camera = HorusOmCamera::GetInstance();

	int ActiveCamera = Camera.GetActiveCameraID();

	if (ActiveCamera != -1)
	{
		return Camera.GetCameraApertureBlades(ActiveCamera);
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
	HorusOmCamera& Camera = HorusOmCamera::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	int ActiveCamera = Camera.GetActiveCameraID();

	if (ActiveCamera != -1)
	{
		Camera.SetCameraLookAt(ActiveCamera, look_at);
	}
	else
	{
		spdlog::error("No camera selected");
		Console.AddLog(" [error] No camera selected");
	}
}
void HorusInspector::SetCameraPosition(glm::vec3 position)
{
	HorusOmCamera& Camera = HorusOmCamera::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	int ActiveCamera = Camera.GetActiveCameraID();

	if (ActiveCamera != -1)
	{
		Camera.SetCameraPosition(ActiveCamera, position);
	}
	else
	{
		spdlog::error("No camera selected");
		Console.AddLog(" [error] No camera selected");
	}
}
void HorusInspector::SetCameraRotation(glm::vec3 rotation_axis)
{
	HorusOmCamera& Camera = HorusOmCamera::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	int ActiveCamera = Camera.GetActiveCameraID();

	if (ActiveCamera != -1)
	{
		Camera.SetCameraRotation(ActiveCamera, rotation_axis);
	}
	else
	{
		spdlog::error("No camera selected");
		Console.AddLog(" [error] No camera selected");
	}
}
void HorusInspector::SetCameraScale(glm::vec3 scale)
{
	HorusOmCamera& Camera = HorusOmCamera::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	int ActiveCamera = Camera.GetActiveCameraID();

	if (ActiveCamera != -1)
	{
		Camera.SetCameraScale(ActiveCamera, scale);
	}
	else
	{
		spdlog::error("No camera selected");
		Console.AddLog(" [error] No camera selected");
	}
}
void HorusInspector::SetCameraNear(float Near)
{
	HorusOmCamera& Camera = HorusOmCamera::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	int ActiveCamera = Camera.GetActiveCameraID();

	if (ActiveCamera != -1)
	{
		Camera.SetCameraNear(ActiveCamera, Near);
	}
	else
	{
		spdlog::error("No camera selected");
		Console.AddLog(" [error] No camera selected");
	}
}
void HorusInspector::SetCameraFar(float Far)
{
	HorusOmCamera& Camera = HorusOmCamera::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	int ActiveCamera = Camera.GetActiveCameraID();

	if (ActiveCamera != -1)
	{
		Camera.SetCameraFar(ActiveCamera, Far);
	}
	else
	{
		spdlog::error("No camera selected");
		Console.AddLog(" [error] No camera selected");
	}
}
void HorusInspector::SetFStop(float FStop)
{
	HorusOmCamera& Camera = HorusOmCamera::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	int ActiveCamera = Camera.GetActiveCameraID();

	// Check if its equal to 0, if yes, set it to 0.001 to avoid division by 0
	if (FStop <= 0.0f)
	{
		FStop = 0.001f;
	}

	if (ActiveCamera != -1)
	{
		Camera.SetCameraFStop(ActiveCamera, FStop);
	}
	else
	{
		spdlog::error("No camera selected");
		Console.AddLog(" [error] No camera selected");
	}
}
void HorusInspector::SetFov(float fov)
{
	HorusOmCamera& Camera = HorusOmCamera::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	int ActiveCamera = Camera.GetActiveCameraID();

	if (ActiveCamera != -1)
	{
		Camera.SetCameraFov(ActiveCamera, fov);
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
			// HDRI path
			ImGui::PushID("HDRI");
			ImGui::TextUnformatted("HDRI Path : ");
			ImGui::InputText("##path", m_LightImage_.data(), m_LightImage_.size());
			ImGui::SameLine();

			if (ImGui::Button("Browse##path"))
			{
				std::string FilePath = Utils::HorusFileDialog::OpenFile(m_LightImageFilters_);
				if (!FilePath.empty())
				{
					std::jthread LoadThread([this, FilePath]() mutable
						{
							SetLightImageTexture(FilePath);
						});
					LoadThread.detach();
				}
			}
			ImGui::SameLine();

			if (ImGui::Button("Clear##path"))
			{
				m_LightImage_.clear();
			}
			ImGui::PopID();

			ImGui::Separator();
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
	ImGui::Text("Light name: %s ", m_LightName_.c_str());
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

void HorusInspector::PopulateSelectedLightInfos()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	// Get Light info from ObjectManager
	m_LightType_ = ObjectManager.GetLightType(ObjectManager.GetActiveLightId());
	m_LightName_ = ObjectManager.GetLightName(ObjectManager.GetActiveLightId());
	m_LightImage_ = ObjectManager.GetLightTexture(ObjectManager.GetActiveLightId());
	m_LightIntensity_ = ObjectManager.GetLightIntensity(ObjectManager.GetActiveLightId());
	m_LightPosition_ = ObjectManager.GetLightPosition(ObjectManager.GetActiveLightId());
	m_LightRotation_ = ObjectManager.GetLightRotation(ObjectManager.GetActiveLightId());
	m_LightScale_ = ObjectManager.GetLightScale(ObjectManager.GetActiveLightId());
	m_LightDirection_ = ObjectManager.GetLightDirection(ObjectManager.GetActiveLightId());


}

void HorusInspector::SetLightImageTexture(std::string path)
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	spdlog::info("Open file : {}", path);
	ObjectManager.SetLightTexture(ObjectManager.GetActiveLightId(), path);
	m_LightImage_ = path;
	CallResetBuffer();
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

	// TODO : Make visibility mesh options -> rprSetVisibilityFlag

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

	// TODO : Make visibility mesh options -> rprSetVisibilityFlag

	// Delete shape button
	if (ImGui::Button("Delete Shape"))
	{
		ObjectManager.DeleteSelectedShape(ObjectManager.GetActiveShapeId());

		CallResetBuffer();
	}
	ImGui::SameLine(); ShowHelpMarker("Delete the selected shape. Attention! No undo possible.");

	if (ImGui::CollapsingHeader("Transform", m_InputFloatFlags_))
	{
		if (ImGui::Button("Reset Transform"))
		{
			ObjectManager.SetShapeResetTransformById(ObjectManager.GetActiveShapeId());

			CallResetBuffer();
		}
		ImGui::SameLine(); ShowHelpMarker("Reset the transform of the shape.");

		ImGui::Separator();

		if (ImGui::InputFloat3("Position", &m_ShapePosition_[0], "%.1f", m_InputFloatFlags_))
		{
			ObjectManager.SetShapePositionById(ObjectManager.GetActiveShapeId(), m_ShapePosition_);

			spdlog::debug("Shape ID: {}", ObjectManager.GetActiveShapeId());

			spdlog::debug("Shape position set : {0} {1} {2}", m_ShapePosition_.x, m_ShapePosition_.y, m_ShapePosition_.z);
			CallResetBuffer();
		}

		if (ImGui::InputFloat3("Rotation", &m_ShapeRotation_[0], "%.1f", m_InputFloatFlags_))
		{
			ObjectManager.SetShapeRotationById(ObjectManager.GetActiveShapeId(), m_ShapeRotation_);
			spdlog::debug("Shape rotation set : {0} {1} {2}", m_ShapeRotation_.x, m_ShapeRotation_.y, m_ShapeRotation_.z);
			CallResetBuffer();
		}

		if (ImGui::InputFloat3("Scale", &m_ShapeScale_[0], "%.1f", m_InputFloatFlags_))
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

//-------------------------------------- MATERIAL INSPECTOR --------------------------------------

void HorusInspector::InspectorMaterial()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	// TODO : Add a checkbox for freeze the value of the material

	// TODO : Add a button for convert (/switch prefered) material to node editor material (if possible)


	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f)); // Green
	ImGui::Text("Material Inspector");
	ImGui::PopStyleColor();

	ShowBigSeparator();

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f)); // Green
	ImGui::Text("Presets : ");
	ImGui::PopStyleColor();
	ImGui::SameLine();
	if (ImGui::Combo(" ", &m_PresetIndex_, m_Presets_, IM_ARRAYSIZE(m_Presets_)))
	{
		if (m_PresetIndex_ == 0)
		{
			SetMaterialDefault();
		}
		else if (m_PresetIndex_ == 1)
		{
			SetMaterialPlastic();
		}
		else if (m_PresetIndex_ == 2)
		{
			SetMaterialMetal();
		}
		else if (m_PresetIndex_ == 3)
		{
			SetMaterialGlass();
		}
		else if (m_PresetIndex_ == 4)
		{
			SetMaterialEmissive();
		}
		else if (m_PresetIndex_ == 5)
		{
			SetMaterialMatte();
		}
		else if (m_PresetIndex_ == 6)
		{
			SetMaterialSkin();
		}
		else if (m_PresetIndex_ == 7)
		{
			SetMaterialSSS();
		}

		CallResetBuffer();
	}

	ShowBigSeparator();

	ImGui::Text("Material : ");
	ImGui::SameLine();
	ImGui::Text(m_MaterialName_.c_str());

	ShowBigSeparator();

	// Base Color - Done
	DrawBaseColorSection();
	ShowBigSeparator();

	// Reflection
	DrawReflectionSection();
	ShowBigSeparator();

	// Sheen
	DrawSheenSection();
	ShowBigSeparator();

	// Refraction
	DrawRefractionSection();
	ShowBigSeparator();

	// SSS
	DrawSssSection();
	ShowBigSeparator();

	// Coating
	DrawCoatingSection();
	ShowBigSeparator();

	// Other (Normal map - Displacement - Emissive - Transparancy)
	DrawOtherSection();
	ShowBigSeparator();

	// Collapsing Header
	if (ImGui::CollapsingHeader("Material Info"))
	{
		ImGui::Text("Material id: %d", ObjectManager.GetActiveMaterialId());
		ImGui::Text("Material Name: %s", m_MaterialName_.c_str());
		ImGui::Separator();

		// Base Color
		{
			ImGui::Text("Base color parameters");
			ImGui::Text("Material Base color : %.2f, %.2f, %.2f, %.2f", m_BaseColor_.x, m_BaseColor_.y, m_BaseColor_.z, m_BaseColor_.w);
			ImGui::Text("Material Base color weight : %.2f, %.2f, %.2f, %.2f", m_BaseColorWeight_.x, m_BaseColorWeight_.y, m_BaseColorWeight_.z, m_BaseColorWeight_.w);
			ImGui::Text("Material Base color roughness : %.2f, %.2f, %.2f, %.2f", m_BaseColorRoughness_.x, m_BaseColorRoughness_.y, m_BaseColorRoughness_.z, m_BaseColorRoughness_.w);
			ImGui::Text("Material Backscatter color : %.2f, %.2f, %.2f, %.2f", m_BackscatterColor_.x, m_BackscatterColor_.y, m_BackscatterColor_.z, m_BackscatterColor_.w);
			ImGui::Text("Material Backscatter weight : %.2f, %.2f, %.2f, %.2f", m_BackscatterWeight_.x, m_BackscatterWeight_.y, m_BackscatterWeight_.z, m_BackscatterWeight_.w);
			ImGui::Separator();
		}

		// Reflection
		{
			ImGui::Text("Material Reflection color : %.2f, %.2f, %.2f, %.2f", m_ReflectionColor_.x, m_ReflectionColor_.y, m_ReflectionColor_.z, m_ReflectionColor_.w);
			ImGui::Text("Material Reflection roughness : %.2f, %.2f, %.2f, %.2f", m_ReflectionRoughness_.x, m_ReflectionRoughness_.y, m_ReflectionRoughness_.z, m_ReflectionRoughness_.w);
			ImGui::Text("Material Reflection weight : %.2f, %.2f, %.2f, %.2f", m_ReflectionWeight_.x, m_ReflectionWeight_.y, m_ReflectionWeight_.z, m_ReflectionWeight_.w);
			ImGui::Text("Material Reflection mode : %d", m_ReflectionMode_);
			ImGui::Text("Material IOR : %.2f", m_Ior_);
			ImGui::Separator();
		}

		// Sheen
		{
			ImGui::Text("Material Sheen color : %.2f, %.2f, %.2f, %.2f", m_SheenColor_.x, m_SheenColor_.y, m_SheenColor_.z, m_SheenColor_.w);
			ImGui::Text("Material Sheen weight : %.2f, %.2f, %.2f, %.2f", m_SheenWeight_.x, m_SheenWeight_.y, m_SheenWeight_.z, m_SheenWeight_.w);
			ImGui::Text("Material Sheen tint : %.2f, %.2f, %.2f, %.2f", m_SheenTint_.x, m_SheenTint_.y, m_SheenTint_.z, m_SheenTint_.w);
			ImGui::Separator();
		}

		// Refraction
		{
			ImGui::Text("Material Refraction color : %.2f, %.2f, %.2f, %.2f", m_RefractionColor_.x, m_RefractionColor_.y, m_RefractionColor_.z, m_RefractionColor_.w);
			ImGui::Text("Material Refraction roughness : %.2f, %.2f, %.2f, %.2f", m_RefractionRoughness_.x, m_RefractionRoughness_.y, m_RefractionRoughness_.z, m_RefractionRoughness_.w);
			ImGui::Text("Material Refraction weight : %.2f, %.2f, %.2f, %.2f", m_RefractionWeight_.x, m_RefractionWeight_.y, m_RefractionWeight_.z, m_RefractionWeight_.w);
			ImGui::Text("Material Refraction IOR : %.2f", m_Ior_);
			ImGui::Text("Material Refraction thin surface : %d", m_RefractionThinSurface_);
			ImGui::Text("Material Refraction absorption color : %.2f, %.2f, %.2f, %.2f", m_RefractionAbsorptionColor_.x, m_RefractionAbsorptionColor_.y, m_RefractionAbsorptionColor_.z, m_RefractionAbsorptionColor_.w);
			ImGui::Text("Material Refraction absorption distance : %.2f, %.2f, %.2f, %.2f", m_RefractionAbsorptionDistance_.x, m_RefractionAbsorptionDistance_.y, m_RefractionAbsorptionDistance_.z, m_RefractionAbsorptionDistance_.w);
			ImGui::Text("Material Refraction caustics : %d", m_RefractionCaustics_);
			ImGui::Separator();
		}

		// SSS
		{
			ImGui::Text("Material SSS scatter color : %.2f, %.2f, %.2f, %.2f", m_SssScatterColor_.x, m_SssScatterColor_.y, m_SssScatterColor_.z, m_SssScatterColor_.w);
			ImGui::Text("Material SSS scatter weight : %.2f, %.2f, %.2f, %.2f", m_SssScatterWeight_.x, m_SssScatterWeight_.y, m_SssScatterWeight_.z, m_SssScatterWeight_.w);
			ImGui::Text("Material SSS scatter distance : %.2f, %.2f, %.2f, %.2f", m_SssScatterDistance_.x, m_SssScatterDistance_.y, m_SssScatterDistance_.z, m_SssScatterDistance_.w);
			ImGui::Text("Material SSS scatter direction : %.2f, %.2f, %.2f, %.2f", m_SssScatterDirection_.x, m_SssScatterDirection_.y, m_SssScatterDirection_.z, m_SssScatterDirection_.w);
			ImGui::Text("Material SSS use multi scattering : %d", m_SssUseMultiScattering_);
			ImGui::Text("Material SSS use schlick approximation : %d", m_SssUseSchlickApproximation_);
			ImGui::Separator();
		}

		// Coating
		{
			ImGui::Text("Material Coating color : %.2f, %.2f, %.2f, %.2f", m_CoatingColor_.x, m_CoatingColor_.y, m_CoatingColor_.z, m_CoatingColor_.w);
			ImGui::Text("Material Coating weight : %.2f, %.2f, %.2f, %.2f", m_CoatingWeight_.x, m_CoatingWeight_.y, m_CoatingWeight_.z, m_CoatingWeight_.w);
			ImGui::Text("Material Coating roughness : %.2f, %.2f, %.2f, %.2f", m_CoatingRoughness_.x, m_CoatingRoughness_.y, m_CoatingRoughness_.z, m_CoatingRoughness_.w);
			ImGui::Text("Material Coating IOR : %.2f", m_CoatingIor_);
			ImGui::Text("Material Coating thickness : %.2f, %.2f, %.2f, %.2f", m_CoatingThickness_.x, m_CoatingThickness_.y, m_CoatingThickness_.z, m_CoatingThickness_.w);
			ImGui::Text("Material Coating transmission color : %.2f, %.2f, %.2f, %.2f", m_CoatingTransmissionColor_.x, m_CoatingTransmissionColor_.y, m_CoatingTransmissionColor_.z, m_CoatingTransmissionColor_.w);
			ImGui::Text("Material Coating metalness : %.2f, %.2f, %.2f, %.2f", m_CoatingMetalness_.x, m_CoatingMetalness_.y, m_CoatingMetalness_.z, m_CoatingMetalness_.w);
			ImGui::Text("Material Coating mode : %d", m_CoatingMode_);
			ImGui::Separator();
		}

	}

	ShowBigSeparator();
}

void HorusInspector::DrawParameterWithFileDialog(std::string& filePath, bool& parameterEnabled, const std::string& buttonID, const char* fileFilter, const std::string& parameterName, UpdateCallback onUpdate, UpdateCallback onEnable)
{
	HorusDragAndDrop& DragAndDrop = HorusDragAndDrop::GetInstance();

	ImGui::PushID(buttonID.c_str());
	ImGui::TextUnformatted((parameterName + " Path: ").c_str());
	ImGui::SameLine();
	ImGui::InputText(("##path" + buttonID).c_str(), filePath.data(), filePath.size());

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("IMAGE"))
		{
			spdlog::debug("File dropped : {}", (const char*)payload->Data);

			std::string FilePath = (const char*)payload->Data;
			if (!FilePath.empty() && IsValidFileImage(FilePath)) {
				spdlog::info("Open file : {}", FilePath);
				filePath = FilePath;
				onUpdate(filePath);
			}
			else {
				spdlog::error("Invalid file path or not an image: {}", FilePath);
			}
		}
	}

	ImGui::SameLine();

	if (ImGui::Button(("Browse##" + buttonID).c_str()))
	{
		std::string FilePath = Utils::HorusFileDialog::OpenFile(fileFilter);
		if (!FilePath.empty() && IsValidFileImage(FilePath)) {
			spdlog::info("Open file : {}", FilePath);
			filePath = FilePath;
			onUpdate(filePath);
		}
		else {
			spdlog::error("Invalid file path or not an image: {}", FilePath);
		}

	}
	ImGui::SameLine();

	if (ImGui::Button(("Clear##" + buttonID).c_str()))
	{
		filePath.clear();
		onUpdate(filePath);
	}

	ImGui::PopID();
}
bool HorusInspector::IsValidFileImage(const std::string& filePath)
{
	static const std::vector<std::string> ValidImageExtensions = { ".png", ".jpg", ".jpeg", ".bmp", ".gif", ".tga", ".psd", ".hdr", ".pic", ".exr" };

	std::filesystem::path pathObj(filePath);
	if (!pathObj.has_extension()) return false;

	std::string extension = pathObj.extension().string();
	std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

	if (std::find(ValidImageExtensions.begin(), ValidImageExtensions.end(), extension) == ValidImageExtensions.end()) {
		spdlog::error("Invalid image file extension: {}", extension);
		return false;
	}

	return true; 
}
void HorusInspector::DrawSwitchColorToTextureButton(bool& switchVariable, const char* id)
{
	ImGui::PushID(id);
	if (ImGui::Button(switchVariable ? "C" : "T")) {
		switchVariable = !switchVariable;
	}
	ImGui::PopID();
}

void HorusInspector::PopulateSelectedMaterialInfos()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	m_MaterialName_ = ObjectManager.GetMaterialName(ObjectManager.GetActiveMaterialId());

	// Base color Section - Done
	{
		// Base color - Done
		m_BaseColorImageColor_ = ObjectManager.GetBaseColorImageColor(ObjectManager.GetActiveMaterialId());
		m_UseTextureBaseColorInsteadOfColor_ = ObjectManager.GetUseTextureBaseColorInsteadOfColor(ObjectManager.GetActiveMaterialId());
		m_EnableBaseColorImage_ = ObjectManager.GetEnableBaseColorImage(ObjectManager.GetActiveMaterialId());
		m_BaseColorPath_ = ObjectManager.GetBaseColorTexturePath(ObjectManager.GetActiveMaterialId());
		m_BaseColor_ = ObjectManager.GetBaseColor(ObjectManager.GetActiveMaterialId());

		// Base color weight - Done
		m_BaseColorImageWeight_ = ObjectManager.GetBaseColorWeightImageColor(ObjectManager.GetActiveMaterialId());
		m_UseTextureBaseColorWeightInsteadOfWeight_ = ObjectManager.GetUseTextureBaseColorWeightInsteadOfColor(ObjectManager.GetActiveMaterialId());
		m_EnableBaseColorWeightImage_ = ObjectManager.GetEnableBaseColorWeightImage(ObjectManager.GetActiveMaterialId());
		m_BaseColorWeightPath_ = ObjectManager.GetBaseColorWeightTexturePath(ObjectManager.GetActiveMaterialId());
		m_BaseColorWeight_ = ObjectManager.GetBaseColorWeight(ObjectManager.GetActiveMaterialId());

		// Base color roughness - Done
		m_BaseColorImageRoughness_ = ObjectManager.GetBaseColorRoughnessImageColor(ObjectManager.GetActiveMaterialId());
		m_UseTextureBaseColorRoughnessInsteadOfRoughness_ = ObjectManager.GetUseTextureBaseColorRoughnessInsteadOfColor(ObjectManager.GetActiveMaterialId());
		m_EnableBaseColorRoughnessImage_ = ObjectManager.GetEnableBaseColorRoughnessImage(ObjectManager.GetActiveMaterialId());
		m_BaseColorRoughnessPath_ = ObjectManager.GetBaseColorRoughnessTexturePath(ObjectManager.GetActiveMaterialId());
		m_BaseColorRoughness_ = ObjectManager.GetBaseColorRoughness(ObjectManager.GetActiveMaterialId());

		// Backscatter Section - Done
		m_BackscatterImageWeight_ = ObjectManager.GetBackscatterWeightImageColor(ObjectManager.GetActiveMaterialId());
		m_UseTextureBackscatterWeightInsteadOfWeight_ = ObjectManager.GetUseTextureBackscatterWeightInsteadOfColor(ObjectManager.GetActiveMaterialId());
		m_EnableBackscatterWeightImage_ = ObjectManager.GetEnableBackscatterWeightImage(ObjectManager.GetActiveMaterialId());
		m_BackscatterColorPath_ = ObjectManager.GetBackscatterColorTexturePath(ObjectManager.GetActiveMaterialId());
		m_BackscatterColor_ = ObjectManager.GetBackscatterColor(ObjectManager.GetActiveMaterialId());

		// Backscatter weight - Done
		m_BackscatterImageColor_ = ObjectManager.GetBackscatterWeightImageColor(ObjectManager.GetActiveMaterialId());
		m_UseTextureBackscatterColorInsteadOfColor_ = ObjectManager.GetUseTextureBackscatterWeightInsteadOfColor(ObjectManager.GetActiveMaterialId());
		m_EnableBackscatterWeightImage_ = ObjectManager.GetEnableBackscatterWeightImage(ObjectManager.GetActiveMaterialId());
		m_BackscatterWeightPath_ = ObjectManager.GetBackscatterWeightTexturePath(ObjectManager.GetActiveMaterialId());
		m_BackscatterWeight_ = ObjectManager.GetBackscatterWeight(ObjectManager.GetActiveMaterialId());
		// TODO : Color normal and color normal weight
	}

	// Reflection Section - Done
	{
		// Reflection Color - Done
		m_ReflectionImageColor_ = ObjectManager.GetReflectionColorImageColor(ObjectManager.GetActiveMaterialId());
		m_UseTextureReflectionColorInsteadOfColor_ = ObjectManager.GetUseTextureReflectionColorInsteadOfColor(ObjectManager.GetActiveMaterialId());
		m_EnableReflectionColorImage_ = ObjectManager.GetEnableReflectionColorImage(ObjectManager.GetActiveMaterialId());
		m_ReflectionColorPath_ = ObjectManager.GetReflectionColorTexturePath(ObjectManager.GetActiveMaterialId());
		m_ReflectionColor_ = ObjectManager.GetReflectionColor(ObjectManager.GetActiveMaterialId());

		// Reflection Weight - Done
		m_ReflectionImageWeight_ = ObjectManager.GetReflectionWeightImageColor(ObjectManager.GetActiveMaterialId());
		m_UseTextureReflectionWeightInsteadOfWeight_ = ObjectManager.GetUseTextureReflectionWeightInsteadOfColor(ObjectManager.GetActiveMaterialId());
		m_EnableReflectionWeightImage_ = ObjectManager.GetEnableReflectionWeightImage(ObjectManager.GetActiveMaterialId());
		m_ReflectionWeightPath_ = ObjectManager.GetReflectionWeightTexturePath(ObjectManager.GetActiveMaterialId());
		m_ReflectionWeight_ = ObjectManager.GetReflectionWeight(ObjectManager.GetActiveMaterialId());

		// Reflection Roughness - Done
		m_ReflectionImageRoughness_ = ObjectManager.GetReflectionRoughnessImageColor(ObjectManager.GetActiveMaterialId());
		m_UseTextureReflectionRoughnessInsteadOfRoughness_ = ObjectManager.GetUseTextureReflectionRoughnessInsteadOfColor(ObjectManager.GetActiveMaterialId());
		m_EnableReflectionRoughnessImage_ = ObjectManager.GetEnableReflectionRoughnessImage(ObjectManager.GetActiveMaterialId());
		m_ReflectionRoughnessPath_ = ObjectManager.GetReflectionRoughnessTexturePath(ObjectManager.GetActiveMaterialId());
		m_ReflectionRoughness_ = ObjectManager.GetReflectionRoughness(ObjectManager.GetActiveMaterialId());

		// Anisotropy - Done
		m_ReflectionImageAnisotropy_ = ObjectManager.GetReflectionAnisotropyImageColor(ObjectManager.GetActiveMaterialId());
		m_UseTextureReflectionAnisotropyInsteadOfAnisotropy_ = ObjectManager.GetUseTextureReflectionAnisotropyInsteadOfColor(ObjectManager.GetActiveMaterialId());
		m_EnableReflectionAnisotropyImage_ = ObjectManager.GetEnableReflectionAnisotropyImage(ObjectManager.GetActiveMaterialId());
		m_ReflectionAnisotropyPath_ = ObjectManager.GetReflectionAnisotropyPath(ObjectManager.GetActiveMaterialId());
		m_ReflectionAnisotropy_ = ObjectManager.GetReflectionAnisotropy(ObjectManager.GetActiveMaterialId());

		// Anisotropy Rotation - Done
		m_ReflectionImageAnisotropyRotation_ = ObjectManager.GetReflectionAnisotropyRotationImageColor(ObjectManager.GetActiveMaterialId());
		m_UseTextureReflectionAnisotropyRotationInsteadOfAnisotropyRotation_ = ObjectManager.GetUseTextureReflectionAnisotropyRotationInsteadOfColor(ObjectManager.GetActiveMaterialId());
		m_EnableReflectionAnisotropyRotationImage_ = ObjectManager.GetEnableReflectionAnisotropyRotationImage(ObjectManager.GetActiveMaterialId());
		m_ReflectionAnisotropyRotationPath_ = ObjectManager.GetReflectionAnisotropyRotationPath(ObjectManager.GetActiveMaterialId());
		m_ReflectionAnisotropyRotation_ = ObjectManager.GetReflectionAnisotropyRotation(ObjectManager.GetActiveMaterialId());

		// Reflection Metalness - Done
		m_ReflectionImageMetalness_ = ObjectManager.GetReflectionMetalnessImageColor(ObjectManager.GetActiveMaterialId());
		m_UseTextureReflectionMetalnessInsteadOfMetalness_ = ObjectManager.GetUseTextureReflectionMetalnessInsteadOfColor(ObjectManager.GetActiveMaterialId());
		m_EnableReflectionMetalnessImage_ = ObjectManager.GetEnableReflectionMetalnessImage(ObjectManager.GetActiveMaterialId());
		m_ReflectionMetalnessPath_ = ObjectManager.GetReflectionMetalnessTexturePath(ObjectManager.GetActiveMaterialId());
		m_ReflectionMetalness_ = ObjectManager.GetReflectionMetalness(ObjectManager.GetActiveMaterialId());

		// Reflection Mode - Done
		m_ReflectionMode_ = ObjectManager.GetReflectionMode(ObjectManager.GetActiveMaterialId());

		if (m_ReflectionMode_ == HorusMaterial::ReflectionTypePBR)
		{
			m_ReflectionModeIndex_ = 0;
		}
		else if (m_ReflectionMode_ == HorusMaterial::ReflectionTypeMetalness)
		{
			m_ReflectionModeIndex_ = 1;
		}

		// IOR - Done
		m_Ior_ = ObjectManager.GetIor(ObjectManager.GetActiveMaterialId());
		// TODO : Implement reflection normal and reflection normal weight
	}

	// Sheen Section - Done
	{
		// Sheen Color - Done
		m_SheenImageColor_ = ObjectManager.GetSheenColorImageColor(ObjectManager.GetActiveMaterialId());
		m_UseTextureSheenColorInsteadOfColor_ = ObjectManager.GetUseTextureSheenColorInsteadOfColor(ObjectManager.GetActiveMaterialId());
		m_EnableSheenColorImage_ = ObjectManager.GetEnableSheenColorImage(ObjectManager.GetActiveMaterialId());
		m_SheenColorPath_ = ObjectManager.GetSheenColorTexturePath(ObjectManager.GetActiveMaterialId());
		m_SheenColor_ = ObjectManager.GetSheenColor(ObjectManager.GetActiveMaterialId());

		// Sheen Weight - Done
		m_SheenImageWeight_ = ObjectManager.GetSheenWeightImageColor(ObjectManager.GetActiveMaterialId());
		m_UseTextureSheenWeightInsteadOfWeight_ = ObjectManager.GetUseTextureSheenWeightInsteadOfColor(ObjectManager.GetActiveMaterialId());
		m_EnableSheenWeightImage_ = ObjectManager.GetEnableSheenWeightImage(ObjectManager.GetActiveMaterialId());
		m_SheenWeightPath_ = ObjectManager.GetSheenWeightTexturePath(ObjectManager.GetActiveMaterialId());
		m_SheenWeight_ = ObjectManager.GetSheenWeight(ObjectManager.GetActiveMaterialId());

		// Sheen Tint - Done
		m_SheenImageTint_ = ObjectManager.GetSheenTintImageColor(ObjectManager.GetActiveMaterialId());
		m_UseTextureSheenTintInsteadOfTint_ = ObjectManager.GetUseTextureSheenTintInsteadOfColor(ObjectManager.GetActiveMaterialId());
		m_EnableSheenTintImage_ = ObjectManager.GetEnableSheenTintImage(ObjectManager.GetActiveMaterialId());
		m_SheenTintPath_ = ObjectManager.GetSheenTintTexturePath(ObjectManager.GetActiveMaterialId());
		m_SheenTint_ = ObjectManager.GetSheenTint(ObjectManager.GetActiveMaterialId());
	}

	// Refraction Section - Done
	{
		// Refraction Color - Done
		m_RefractionImageColor_ = ObjectManager.GetRefractionColorImageColor(ObjectManager.GetActiveMaterialId());
		m_UseTextureRefractionColorInsteadOfColor_ = ObjectManager.GetUseTextureRefractionColorInsteadOfColor(ObjectManager.GetActiveMaterialId());
		m_EnableRefractionColorImage_ = ObjectManager.GetEnableRefractionColorImage(ObjectManager.GetActiveMaterialId());
		m_RefractionColorPath_ = ObjectManager.GetRefractionColorTexturePath(ObjectManager.GetActiveMaterialId());
		m_RefractionColor_ = ObjectManager.GetRefractionColor(ObjectManager.GetActiveMaterialId());

		// Refraction Weight - Done
		m_RefractionImageWeight_ = ObjectManager.GetRefractionWeightImageColor(ObjectManager.GetActiveMaterialId());
		m_UseTextureRefractionWeightInsteadOfWeight_ = ObjectManager.GetUseTextureRefractionWeightInsteadOfColor(ObjectManager.GetActiveMaterialId());
		m_EnableRefractionWeightImage_ = ObjectManager.GetEnableRefractionWeightImage(ObjectManager.GetActiveMaterialId());
		m_RefractionWeightPath_ = ObjectManager.GetRefractionWeightTexturePath(ObjectManager.GetActiveMaterialId());
		m_RefractionWeight_ = ObjectManager.GetRefractionWeight(ObjectManager.GetActiveMaterialId());

		// Refraction Normal map
		m_RefractionImageNormalMap_ = ObjectManager.GetRefractionNormalImageColor(ObjectManager.GetActiveMaterialId());
		m_UseTextureRefractionNormalMapInsteadOfNormalMap_ = ObjectManager.GetUseTextureRefractionNormalInsteadOfColor(ObjectManager.GetActiveMaterialId());
		m_EnableRefractionNormalMapImage_ = ObjectManager.GetEnableRefractionNormalImage(ObjectManager.GetActiveMaterialId());
		m_RefractionNormalMapPath_ = ObjectManager.GetRefractionNormalTexturePath(ObjectManager.GetActiveMaterialId());
		m_RefractionNormalMap_ = ObjectManager.GetRefractionNormal(ObjectManager.GetActiveMaterialId());

		// Refraction Normal map weight
		m_RefractionNormalMapWeight_ = ObjectManager.GetRefractionNormalWeight(ObjectManager.GetActiveMaterialId());

		// Refraction Roughness - Done
		m_RefractionImageRoughness_ = ObjectManager.GetRefractionRoughnessImageColor(ObjectManager.GetActiveMaterialId());
		m_UseTextureRefractionRoughnessInsteadOfRoughness_ = ObjectManager.GetUseTextureRefractionRoughnessInsteadOfColor(ObjectManager.GetActiveMaterialId());
		m_EnableRefractionRoughnessImage_ = ObjectManager.GetEnableRefractionRoughnessImage(ObjectManager.GetActiveMaterialId());
		m_RefractionRoughnessPath_ = ObjectManager.GetRefractionRoughnessTexturePath(ObjectManager.GetActiveMaterialId());
		m_RefractionRoughness_ = ObjectManager.GetRefractionRoughness(ObjectManager.GetActiveMaterialId());

		// Refraction IOR - Done
		m_RefractionIor_ = ObjectManager.GetRefractionIor(ObjectManager.GetActiveMaterialId());

		// Refraction Thin Surface - Done
		m_RefractionThinSurface_ = ObjectManager.GetRefractionThinSurface(ObjectManager.GetActiveMaterialId());

		// Refraction Absorption Color - Done
		m_RefractionAbsorptionImageColor_ = ObjectManager.GetRefractionAbsorptionColorImageColor(ObjectManager.GetActiveMaterialId());
		m_UseTextureRefractionAbsorptionColorInsteadOfColor_ = ObjectManager.GetUseTextureRefractionAbsorptionColorInsteadOfColor(ObjectManager.GetActiveMaterialId());
		m_EnableRefractionAbsorptionColorImage_ = ObjectManager.GetEnableRefractionAbsorptionColorImage(ObjectManager.GetActiveMaterialId());
		m_RefractionAbsorptionColorPath_ = ObjectManager.GetRefractionAbsorptionDistanceTexturePath(ObjectManager.GetActiveMaterialId());
		m_RefractionAbsorptionColor_ = ObjectManager.GetRefractionAbsorptionColor(ObjectManager.GetActiveMaterialId());

		// Refraction Absorption Distance - Done
		m_RefractionAbsorptionImageDistance_ = ObjectManager.GetRefractionAbsorptionDistanceImageColor(ObjectManager.GetActiveMaterialId());
		m_UseTextureRefractionAbsorptionDistanceInsteadOfDistance_ = ObjectManager.GetUseTextureRefractionAbsorptionDistanceInsteadOfColor(ObjectManager.GetActiveMaterialId());
		m_EnableRefractionAbsorptionDistanceImage_ = ObjectManager.GetEnableRefractionAbsorptionDistanceImage(ObjectManager.GetActiveMaterialId());
		m_RefractionAbsorptionDistancePath_ = ObjectManager.GetRefractionAbsorptionDistanceTexturePath(ObjectManager.GetActiveMaterialId());
		m_RefractionAbsorptionDistance_ = ObjectManager.GetRefractionAbsorptionDistance(ObjectManager.GetActiveMaterialId());

		// Refraction Caustics - Done
		m_RefractionCaustics_ = ObjectManager.GetRefractionCaustics(ObjectManager.GetActiveMaterialId());
	}

	// SSS Section - Done
	{
		// SSS Color - Done
		m_SssImageColor_ = ObjectManager.GetSssScatterColorImageColor(ObjectManager.GetActiveMaterialId());
		m_UseTextureSssColorInsteadOfColor_ = ObjectManager.GetUseTextureSssScatterColorInsteadOfColor(ObjectManager.GetActiveMaterialId());
		m_EnableSssColorImage_ = ObjectManager.GetEnableSssScatterColorImage(ObjectManager.GetActiveMaterialId());
		m_SssColorPath_ = ObjectManager.GetSssScatterColorTexturePath(ObjectManager.GetActiveMaterialId());
		m_SssScatterColor_ = ObjectManager.GetSssScatterColor(ObjectManager.GetActiveMaterialId());

		// SSS Weight - Done
		m_SssImageWeight_ = ObjectManager.GetSssScatterWeightImageColor(ObjectManager.GetActiveMaterialId());
		m_UseTextureSssWeightInsteadOfWeight_ = ObjectManager.GetUseTextureSssScatterWeightInsteadOfColor(ObjectManager.GetActiveMaterialId());
		m_EnableSssWeightImage_ = ObjectManager.GetEnableSssScatterWeightImage(ObjectManager.GetActiveMaterialId());
		m_SssWeightPath_ = ObjectManager.GetSssScatterWeightTexturePath(ObjectManager.GetActiveMaterialId());
		m_SssScatterWeight_ = ObjectManager.GetSssScatterWeight(ObjectManager.GetActiveMaterialId());

		// SSS Distance - Done
		m_SssImageRadius_ = ObjectManager.GetSssScatterDistanceImageColor(ObjectManager.GetActiveMaterialId());
		m_UseTextureSssRadiusInsteadOfRadius_ = ObjectManager.GetUseTextureSssScatterDistanceInsteadOfColor(ObjectManager.GetActiveMaterialId());
		m_EnableSssRadiusImage_ = ObjectManager.GetEnableSssScatterDistanceImage(ObjectManager.GetActiveMaterialId());
		m_SssRadiusPath_ = ObjectManager.GetSssScatterDistanceTexturePath(ObjectManager.GetActiveMaterialId());
		m_SssScatterDistance_ = ObjectManager.GetSssScatterDistance(ObjectManager.GetActiveMaterialId());

		// SSS Direction - Done
		m_SssImageDirection_ = ObjectManager.GetSssScatterDirectionImageColor(ObjectManager.GetActiveMaterialId());
		m_UseTextureSssDirectionInsteadOfDirection_ = ObjectManager.GetUseTextureSssScatterDirectionInsteadOfColor(ObjectManager.GetActiveMaterialId());
		m_EnableSssDirectionImage_ = ObjectManager.GetEnableSssScatterDirectionImage(ObjectManager.GetActiveMaterialId());
		m_SssDirectionPath_ = ObjectManager.GetSssScatterDirectionTexturePath(ObjectManager.GetActiveMaterialId());
		m_SssScatterDirection_ = ObjectManager.GetSssScatterDirection(ObjectManager.GetActiveMaterialId());

		// SSS Use Multi Scattering - Done
		m_SssUseMultiScattering_ = ObjectManager.GetSssUseMultiScattering(ObjectManager.GetActiveMaterialId());

		// SSS Use Schlick Approximation - Done
		m_SssUseSchlickApproximation_ = ObjectManager.GetSssUseSchlickApproximation(ObjectManager.GetActiveMaterialId());

	}

	// Coating Section - Done
	{
		// Coating Color - Done
		m_CoatingImageColor_ = ObjectManager.GetCoatingColorImageColor(ObjectManager.GetActiveMaterialId());
		m_UseTextureCoatingColorInsteadOfColor_ = ObjectManager.GetUseTextureCoatingColorInsteadOfColor(ObjectManager.GetActiveMaterialId());
		m_EnableCoatingColorImage_ = ObjectManager.GetEnableCoatingColorImage(ObjectManager.GetActiveMaterialId());
		m_CoatingColorPath_ = ObjectManager.GetCoatingColorTexturePath(ObjectManager.GetActiveMaterialId());
		m_CoatingColor_ = ObjectManager.GetCoatingColor(ObjectManager.GetActiveMaterialId());

		// Coating Weight - Done
		m_CoatingImageWeight_ = ObjectManager.GetCoatingWeightImageColor(ObjectManager.GetActiveMaterialId());
		m_UseTextureCoatingWeightInsteadOfWeight_ = ObjectManager.GetUseTextureCoatingWeightInsteadOfColor(ObjectManager.GetActiveMaterialId());
		m_EnableCoatingWeightImage_ = ObjectManager.GetEnableCoatingWeightImage(ObjectManager.GetActiveMaterialId());
		m_CoatingWeightPath_ = ObjectManager.GetCoatingWeightTexturePath(ObjectManager.GetActiveMaterialId());
		m_CoatingWeight_ = ObjectManager.GetCoatingWeight(ObjectManager.GetActiveMaterialId());

		// Coating normal map
		m_CoatingImageNormalMap_ = ObjectManager.GetCoatingNormalImageColor(ObjectManager.GetActiveMaterialId());
		m_UseTextureCoatingNormalMapInsteadOfNormalMap_ = ObjectManager.GetUseTextureCoatingNormalInsteadOfColor(ObjectManager.GetActiveMaterialId());
		m_EnableCoatingNormalMapImage_ = ObjectManager.GetEnableCoatingNormalImage(ObjectManager.GetActiveMaterialId());
		m_CoatingNormalMapPath_ = ObjectManager.GetCoatingNormalTexturePath(ObjectManager.GetActiveMaterialId());
		m_CoatingNormalMap_ = ObjectManager.GetCoatingNormal(ObjectManager.GetActiveMaterialId());

		// Coating normal map weight
		m_CoatingNormalMapWeight_ = ObjectManager.GetCoatingNormalWeight(ObjectManager.GetActiveMaterialId());

		// Coating Roughness - Done
		m_CoatingImageRoughness_ = ObjectManager.GetCoatingRoughnessImageColor(ObjectManager.GetActiveMaterialId());
		m_UseTextureCoatingRoughnessInsteadOfRoughness_ = ObjectManager.GetUseTextureCoatingRoughnessInsteadOfColor(ObjectManager.GetActiveMaterialId());
		m_EnableCoatingRoughnessImage_ = ObjectManager.GetEnableCoatingRoughnessImage(ObjectManager.GetActiveMaterialId());
		m_CoatingRoughnessPath_ = ObjectManager.GetCoatingRoughnessTexturePath(ObjectManager.GetActiveMaterialId());
		m_CoatingRoughness_ = ObjectManager.GetCoatingRoughness(ObjectManager.GetActiveMaterialId());

		// Coating IOR - Done
		m_CoatingIor_ = ObjectManager.GetCoatingIor(ObjectManager.GetActiveMaterialId());

		// Coating Thickness - Done
		m_CoatingImageThickness_ = ObjectManager.GetCoatingThicknessImageColor(ObjectManager.GetActiveMaterialId());
		m_UseTextureCoatingThicknessInsteadOfThickness_ = ObjectManager.GetUseTextureCoatingThicknessInsteadOfColor(ObjectManager.GetActiveMaterialId());
		m_EnableCoatingThicknessImage_ = ObjectManager.GetEnableCoatingThicknessImage(ObjectManager.GetActiveMaterialId());
		m_CoatingThicknessPath_ = ObjectManager.GetCoatingThicknessTexturePath(ObjectManager.GetActiveMaterialId());
		m_CoatingThickness_ = ObjectManager.GetCoatingThickness(ObjectManager.GetActiveMaterialId());

		// Coating Transmission Color - Done
		m_CoatingImageTransmissionColor_ = ObjectManager.GetCoatingTransmissionColorImageColor(ObjectManager.GetActiveMaterialId());
		m_UseTextureCoatingTransmissionColorInsteadOfTransmissionColor_ = ObjectManager.GetUseTextureCoatingTransmissionColorInsteadOfColor(ObjectManager.GetActiveMaterialId());
		m_EnableCoatingTransmissionColorImage_ = ObjectManager.GetEnableCoatingTransmissionColorImage(ObjectManager.GetActiveMaterialId());
		m_CoatingTransmissionColorPath_ = ObjectManager.GetCoatingTransmissionColorTexturePath(ObjectManager.GetActiveMaterialId());
		m_CoatingTransmissionColor_ = ObjectManager.GetCoatingTransmissionColor(ObjectManager.GetActiveMaterialId());

		// Coating Transmission Distance - Done
		m_CoatingMode_ = ObjectManager.GetCoatingMode(ObjectManager.GetActiveMaterialId());

		// Coating Metalness - Done
		m_CoatingImageMetalness_ = ObjectManager.GetCoatingMetalnessImageColor(ObjectManager.GetActiveMaterialId());
		m_UseTextureCoatingMetalnessInsteadOfMetalness_ = ObjectManager.GetUseTextureCoatingMetalnessInsteadOfColor(ObjectManager.GetActiveMaterialId());
		m_EnableCoatingMetalnessImage_ = ObjectManager.GetEnableCoatingMetalnessImage(ObjectManager.GetActiveMaterialId());
		m_CoatingMetalnessPath_ = ObjectManager.GetCoatingMetalnessTexturePath(ObjectManager.GetActiveMaterialId());
		m_CoatingMetalness_ = ObjectManager.GetCoatingMetalness(ObjectManager.GetActiveMaterialId());

		if (m_CoatingMode_ == HorusMaterial::CoatingTypePBR)
		{
			m_CoatingModeIndex_ = 0;
		}
		else if (m_CoatingMode_ == HorusMaterial::CoatingTypeMetalness)
		{
			m_CoatingModeIndex_ = 1;
		}
	}

	// Other Section - Done	
	{
		// Normal Map - Done
		m_NormalMapImage_ = ObjectManager.GetNormalImageColor(ObjectManager.GetActiveMaterialId());
		m_UseTextureNormalMapInsteadOfNormalMap_ = ObjectManager.GetUseTextureNormalInsteadOfColor(ObjectManager.GetActiveMaterialId());
		m_EnableNormalMapImage_ = ObjectManager.GetEnableNormalImage(ObjectManager.GetActiveMaterialId());
		m_NormalMapPath_ = ObjectManager.GetNormalTexturePath(ObjectManager.GetActiveMaterialId());
		m_NormalMap_ = ObjectManager.GetNormalMap(ObjectManager.GetActiveMaterialId());

		// Normal Map Weight - Done
		m_NormalMapWeight_ = ObjectManager.GetNormalWeight(ObjectManager.GetActiveMaterialId());

		// Displacement Map - Done
		m_DisplacementMapImage_ = ObjectManager.GetDisplacementImageColor(ObjectManager.GetActiveMaterialId());
		m_UseTextureDisplacementMapInsteadOfDisplacementMap_ = ObjectManager.GetUseTextureDisplacementInsteadOfColor(ObjectManager.GetActiveMaterialId());
		m_EnableDisplacementMapImage_ = ObjectManager.GetEnableDisplacementImage(ObjectManager.GetActiveMaterialId());
		m_DisplacementMapPath_ = ObjectManager.GetDisplacementTexturePath(ObjectManager.GetActiveMaterialId());
		m_DisplacementMap_ = ObjectManager.GetDisplacementMap(ObjectManager.GetActiveMaterialId());

		// Displacement Map Weight - Done
		m_DisplacementMapWeight_ = ObjectManager.GetDisplacementWeight(ObjectManager.GetActiveMaterialId());

		// Emissive Map - Done
		m_EmissiveImage_ = ObjectManager.GetEmissiveImageColor(ObjectManager.GetActiveMaterialId());
		m_UseTextureEmissiveInsteadOfEmissive_ = ObjectManager.GetUseTextureEmissiveInsteadOfColor(ObjectManager.GetActiveMaterialId());
		m_EnableEmissiveImage_ = ObjectManager.GetEnableEmissiveImage(ObjectManager.GetActiveMaterialId());
		m_EmissivePath_ = ObjectManager.GetEmissiveTexturePath(ObjectManager.GetActiveMaterialId());
		m_Emissive_ = ObjectManager.GetEmissive(ObjectManager.GetActiveMaterialId());

		// Emissive Weight - Done
		m_EmissiveWeightImage_ = ObjectManager.GetEmissiveWeightImageColor(ObjectManager.GetActiveMaterialId());
		m_UseTextureEmissiveWeightInsteadOfEmissiveWeight_ = ObjectManager.GetUseTextureEmissiveWeightInsteadOfColor(ObjectManager.GetActiveMaterialId());
		m_EnableEmissiveWeightImage_ = ObjectManager.GetEnableEmissiveWeightImage(ObjectManager.GetActiveMaterialId());
		m_EmissiveWeightPath_ = ObjectManager.GetEmissiveWeightTexturePath(ObjectManager.GetActiveMaterialId());
		m_EmissionWeight_ = ObjectManager.GetEmissiveWeight(ObjectManager.GetActiveMaterialId());

		// Opacity Map - Done
		m_OpacityImage_ = ObjectManager.GetOpacityImageColor(ObjectManager.GetActiveMaterialId());
		m_UseTextureOpacityInsteadOfOpacity_ = ObjectManager.GetUseTextureOpacityInsteadOfColor(ObjectManager.GetActiveMaterialId());
		m_EnableOpacityImage_ = ObjectManager.GetEnableOpacityImage(ObjectManager.GetActiveMaterialId());
		m_OpacityPath_ = ObjectManager.GetOpacityTexturePath(ObjectManager.GetActiveMaterialId());
		m_Opacity_ = ObjectManager.GetOpacity(ObjectManager.GetActiveMaterialId());
	}
}

// Draw Material Inspector Section
void HorusInspector::DrawBaseColorSection()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	ImGui::PushID("BaseColor"); // Push ID -> required for naming same widgets with different values
	if (ImGui::CollapsingHeader("Base Color", m_TreeNodeFlagsOpen_))
	{
		// Base Color - Done
		DrawSwitchColorToTextureButton(m_UseTextureBaseColorInsteadOfColor_, "BCSwitch");
		ImGui::SameLine();
		if (m_UseTextureBaseColorInsteadOfColor_)
		{
			DrawParameterWithFileDialog(
				m_BaseColorPath_,
				m_EnableBaseColorImage_,
				"BaseColorPath",
				m_ImageFilters_,
				"Base Color",
				[this, &ObjectManager](const std::string& path)
				{
					ObjectManager.SetBaseColor(ObjectManager.GetActiveMaterialId(), path);
					ObjectManager.SetUseTextureBaseColorInsteadOfColor(ObjectManager.GetActiveMaterialId(), true);
					ObjectManager.SetEnableBaseColorImage(ObjectManager.GetActiveMaterialId(), true);

					CallResetBuffer();
				},
				[this, &ObjectManager](const std::string& path)
				{

				}
			);
			ImGui::SameLine(); ShowHelpMarker("To re set a simple color instead of a texture, click on the switch button and select a color. if you don't select a color, the texture will be used instead.");
		}
		else
		{
			if (ImGui::ColorEdit3("Color", &m_BaseColor_[0]))
			{
				ObjectManager.SetBaseColor(ObjectManager.GetActiveMaterialId(), m_BaseColor_);
				ObjectManager.SetUseTextureBaseColorInsteadOfColor(ObjectManager.GetActiveMaterialId(), false);
				ObjectManager.SetEnableBaseColorImage(ObjectManager.GetActiveMaterialId(), false);

				CallResetBuffer();

			}
			ImGui::SameLine();  ShowHelpMarker("The base color of the material.");
		}

		// Weight - Done
		DrawSwitchColorToTextureButton(m_UseTextureBaseColorWeightInsteadOfWeight_, "BCWheightSwitch");
		ImGui::SameLine();
		if (m_UseTextureBaseColorWeightInsteadOfWeight_)
		{
			DrawParameterWithFileDialog(
				m_BaseColorWeightPath_,
				m_EnableBaseColorWeightImage_,
				"BaseColorWeightPath",
				m_ImageFilters_,
				"Weight",
				[this, &ObjectManager](const std::string& path)
				{
					ObjectManager.SetBaseColorWeight(ObjectManager.GetActiveMaterialId(), path);
					ObjectManager.SetUseTextureBaseColorWeightInsteadOfColor(ObjectManager.GetActiveMaterialId(), true);
					ObjectManager.SetEnableBaseColorWeightImage(ObjectManager.GetActiveMaterialId(), true);

					CallResetBuffer();
				},
				[this, &ObjectManager](const std::string& path)
				{

				}
			);
		}
		else
		{
			if (ImGui::SliderFloat("Weight", &m_BaseColorWeight_.x, 0.0f, 1.0f))
			{
				m_BaseColorWeight_.y = m_BaseColorWeight_.z = m_BaseColorWeight_.w = m_BaseColorWeight_.x;
				ObjectManager.SetBaseColorWeight(ObjectManager.GetActiveMaterialId(), m_BaseColorWeight_);
				ObjectManager.SetUseTextureBaseColorWeightInsteadOfColor(ObjectManager.GetActiveMaterialId(), false);
				ObjectManager.SetEnableBaseColorWeightImage(ObjectManager.GetActiveMaterialId(), false);
				CallResetBuffer();
			}
		}

		// Roughness - Done
		DrawSwitchColorToTextureButton(m_UseTextureBaseColorRoughnessInsteadOfRoughness_, "BCRoughnessSwitch");
		ImGui::SameLine();
		if (m_UseTextureBaseColorRoughnessInsteadOfRoughness_)
		{
			DrawParameterWithFileDialog(
				m_BaseColorRoughnessPath_,
				m_EnableBaseColorRoughnessImage_,
				"BaseColorRoughnessPath",
				m_ImageFilters_,
				"Roughness",
				[this, &ObjectManager](const std::string& path)
				{
					ObjectManager.SetBaseColorRoughness(ObjectManager.GetActiveMaterialId(), path);
					ObjectManager.SetUseTextureBaseColorRoughnessInsteadOfColor(ObjectManager.GetActiveMaterialId(), true);
					ObjectManager.SetEnableBaseColorRoughnessImage(ObjectManager.GetActiveMaterialId(), true);

					CallResetBuffer();
				},
				[this, &ObjectManager](const std::string& path)
				{

				}
			);
		}
		else
		{
			if (ImGui::SliderFloat("Roughness", &m_BaseColorRoughness_.x, 0.0f, 1.0f))
			{
				m_BaseColorRoughness_.y = m_BaseColorRoughness_.z = m_BaseColorRoughness_.w = m_BaseColorRoughness_.x;
				ObjectManager.SetBaseColorRoughness(ObjectManager.GetActiveMaterialId(), m_BaseColorRoughness_);
				ObjectManager.SetUseTextureBaseColorRoughnessInsteadOfColor(ObjectManager.GetActiveMaterialId(), false);
				ObjectManager.SetEnableBaseColorRoughnessImage(ObjectManager.GetActiveMaterialId(), false);
				CallResetBuffer();
			}
		}

		// Backscattering - Done
		if (ImGui::CollapsingHeader("Backscattering", m_TreeNodeFlags_))
		{
			// Backscattering Color - Done
			DrawSwitchColorToTextureButton(m_UseTextureBackscatterColorInsteadOfColor_, "BSCSwitch");
			ImGui::SameLine();
			if (m_UseTextureBackscatterColorInsteadOfColor_)
			{
				DrawParameterWithFileDialog(
					m_BackscatterColorPath_,
					m_EnableBackscatterColorImage_,
					"BackscatteringColorPath",
					m_ImageFilters_,
					"Backscattering Color",
					[this, &ObjectManager](const std::string& path)
					{
						ObjectManager.SetBackscatterColor(ObjectManager.GetActiveMaterialId(), path);
						ObjectManager.SetUseTextureBackscatterColorInsteadOfColor(ObjectManager.GetActiveMaterialId(), true);
						ObjectManager.SetEnableBackscatterColorImage(ObjectManager.GetActiveMaterialId(), true);

						CallResetBuffer();
					},
					[this, &ObjectManager](const std::string& path)
					{

					}
				);
			}
			else
			{
				if (ImGui::ColorEdit3("Backscattering Color", &m_BackscatterColor_[0]))
				{
					ObjectManager.SetBackscatterColor(ObjectManager.GetActiveMaterialId(), m_BackscatterColor_);
					ObjectManager.SetUseTextureBackscatterColorInsteadOfColor(ObjectManager.GetActiveMaterialId(), false);
					ObjectManager.SetEnableBackscatterColorImage(ObjectManager.GetActiveMaterialId(), false);
					CallResetBuffer();
				}
			}

			// Backscattering Weight - Done
			DrawSwitchColorToTextureButton(m_UseTextureBackscatterWeightInsteadOfWeight_, "BSWheightSwitch");
			ImGui::SameLine();
			if (m_UseTextureBackscatterWeightInsteadOfWeight_)
			{
				DrawParameterWithFileDialog(
					m_BackscatterWeightPath_,
					m_EnableBackscatterWeightImage_,
					"BackscatteringWeightPath",
					m_ImageFilters_,
					"Backscattering Weight",
					[this, &ObjectManager](const std::string& path)
					{
						ObjectManager.SetBackscatterWeight(ObjectManager.GetActiveMaterialId(), path);
						ObjectManager.SetUseTextureBackscatterWeightInsteadOfColor(ObjectManager.GetActiveMaterialId(), true);
						ObjectManager.SetEnableBackscatterWeightImage(ObjectManager.GetActiveMaterialId(), true);
						CallResetBuffer();
					},
					[this, &ObjectManager](const std::string& path)
					{

					}
				);
			}
			else
			{
				if (ImGui::SliderFloat("Backscattering Weight", &m_BackscatterWeight_.x, 0.0f, 1.0f))
				{
					m_BackscatterWeight_.y = m_BackscatterWeight_.z = m_BackscatterWeight_.w = m_BackscatterWeight_.x;
					ObjectManager.SetBackscatterWeight(ObjectManager.GetActiveMaterialId(), m_BackscatterWeight_);
					ObjectManager.SetUseTextureBackscatterWeightInsteadOfColor(ObjectManager.GetActiveMaterialId(), false);
					ObjectManager.SetEnableBackscatterWeightImage(ObjectManager.GetActiveMaterialId(), false);
					CallResetBuffer();
				}
			}
		}
	}
	ImGui::PopID();
}
void HorusInspector::DrawReflectionSection()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	// Reflection Section
	{
		ImGui::PushID("Reflection");
		if (ImGui::CollapsingHeader("Reflection", m_TreeNodeFlagsOpen_))
		{
			// Reflection Color
			DrawSwitchColorToTextureButton(m_UseTextureReflectionColorInsteadOfColor_, "ReflectionColorSwitch");
			ImGui::SameLine();
			if (m_UseTextureReflectionColorInsteadOfColor_)
			{
				DrawParameterWithFileDialog(
					m_ReflectionColorPath_,
					m_EnableReflectionColorImage_,
					"ReflectionColorPath",
					m_ImageFilters_,
					"Reflection Color",
					[this, &ObjectManager](const std::string& path)
					{
						ObjectManager.SetReflectionColor(ObjectManager.GetActiveMaterialId(), path);
						ObjectManager.SetUseTextureReflectionColorInsteadOfColor(ObjectManager.GetActiveMaterialId(), true);
						ObjectManager.SetEnableReflectionColorImage(ObjectManager.GetActiveMaterialId(), true);

						CallResetBuffer();
					},
					[this, &ObjectManager](const std::string& path)
					{

					}
				);
			}
			else
			{
				if (ImGui::ColorEdit3("Color", &m_ReflectionColor_[0]))
				{
					ObjectManager.SetReflectionColor(ObjectManager.GetActiveMaterialId(), m_ReflectionColor_);
					ObjectManager.SetUseTextureReflectionColorInsteadOfColor(ObjectManager.GetActiveMaterialId(), false);
					ObjectManager.SetEnableReflectionColorImage(ObjectManager.GetActiveMaterialId(), false);

					CallResetBuffer();
				}
			}

			// Reflection Weight
			DrawSwitchColorToTextureButton(m_UseTextureReflectionWeightInsteadOfWeight_, "ReflectionWeightSwitch");
			ImGui::SameLine();
			if (m_UseTextureReflectionWeightInsteadOfWeight_)
			{
				DrawParameterWithFileDialog(
					m_ReflectionWeightPath_,
					m_EnableReflectionWeightImage_,
					"ReflectionWeightPath",
					m_ImageFilters_,
					"Reflection Weight",
					[this, &ObjectManager](const std::string& path)
					{
						ObjectManager.SetReflectionWeight(ObjectManager.GetActiveMaterialId(), path);
						ObjectManager.SetUseTextureReflectionWeightInsteadOfColor(ObjectManager.GetActiveMaterialId(), true);
						ObjectManager.SetEnableReflectionWeightImage(ObjectManager.GetActiveMaterialId(), true);

						CallResetBuffer();
					},
					[this, &ObjectManager](const std::string& path)
					{

					}
				);
			}
			else
			{
				if (ImGui::SliderFloat("Weight", &m_ReflectionWeight_.x, 0.0f, 1.0f))
				{
					m_ReflectionWeight_.y = m_ReflectionWeight_.z = m_ReflectionWeight_.w = m_ReflectionWeight_.x;
					ObjectManager.SetReflectionWeight(ObjectManager.GetActiveMaterialId(), m_ReflectionWeight_);

					CallResetBuffer();
				}
			}

			// Reflection Roughness
			DrawSwitchColorToTextureButton(m_UseTextureReflectionRoughnessInsteadOfRoughness_, "ReflectionRoughnessSwitch");
			ImGui::SameLine();
			if (m_UseTextureReflectionRoughnessInsteadOfRoughness_)
			{
				DrawParameterWithFileDialog(
					m_ReflectionRoughnessPath_,
					m_EnableReflectionRoughnessImage_,
					"ReflectionRoughnessPath",
					m_ImageFilters_,
					"Reflection Roughness",
					[this, &ObjectManager](const std::string& path)
					{
						ObjectManager.SetReflectionRoughness(ObjectManager.GetActiveMaterialId(), path);
						ObjectManager.SetUseTextureReflectionRoughnessInsteadOfColor(ObjectManager.GetActiveMaterialId(), true);
						ObjectManager.SetEnableReflectionRoughnessImage(ObjectManager.GetActiveMaterialId(), true);

						CallResetBuffer();
					},
					[this, &ObjectManager](const std::string& path)
					{

					}
				);
			}
			else
			{
				if (ImGui::SliderFloat("Roughness", &m_ReflectionRoughness_.x, 0.0f, 1.0f))
				{
					m_ReflectionRoughness_.y = m_ReflectionRoughness_.z = m_ReflectionRoughness_.w = m_ReflectionRoughness_.x;
					ObjectManager.SetReflectionRoughness(ObjectManager.GetActiveMaterialId(), m_ReflectionRoughness_);

					CallResetBuffer();
				}
			}

			// Reflection Mode
			if (ImGui::Combo("Reflection Mode", &m_ReflectionModeIndex_, m_ReflectionModes_, IM_ARRAYSIZE(m_ReflectionModes_)))
			{
				if (m_ReflectionModeIndex_ == 0)
				{
					ObjectManager.SetReflectionMode(ObjectManager.GetActiveMaterialId(), HorusMaterial::ReflectionTypePBR);
				}
				else if (m_ReflectionModeIndex_ == 1)
				{
					ObjectManager.SetReflectionMode(ObjectManager.GetActiveMaterialId(), HorusMaterial::ReflectionTypeMetalness);
				}

				CallResetBuffer();
			}

			// If metalness mode
			{
				if (m_ReflectionModeIndex_ == 0)
				{
					m_ShowMetallicUi_ = false;
				}
				else if (m_ReflectionModeIndex_ == 1)
				{
					m_ShowMetallicUi_ = true;
				}

				if (m_ShowMetallicUi_)
				{
					DrawSwitchColorToTextureButton(m_UseTextureReflectionMetalnessInsteadOfMetalness_, "MetallicSwitch");
					ImGui::SameLine();
					if (m_UseTextureReflectionMetalnessInsteadOfMetalness_)
					{
						DrawParameterWithFileDialog(
							m_ReflectionMetalnessPath_,
							m_EnableReflectionMetalnessImage_,
							"MetallicPath",
							m_ImageFilters_,
							"Metallic",
							[this, &ObjectManager](const std::string& path)
							{
								ObjectManager.SetReflectionMetalness(ObjectManager.GetActiveMaterialId(), path);
								ObjectManager.SetUseTextureReflectionMetalnessInsteadOfColor(ObjectManager.GetActiveMaterialId(), true);
								ObjectManager.SetEnableReflectionMetalnessImage(ObjectManager.GetActiveMaterialId(), true);

								CallResetBuffer();
							},
							[this, &ObjectManager](const std::string& path)
							{

							}
						);
					}
					else
					{
						if (ImGui::SliderFloat("Metallic", &m_ReflectionMetalness_.x, 0.0f, 1.0f))
						{
							m_ReflectionMetalness_.y = m_ReflectionMetalness_.z = m_ReflectionMetalness_.w = m_ReflectionMetalness_.x;
							ObjectManager.SetReflectionMetalness(ObjectManager.GetActiveMaterialId(), m_ReflectionMetalness_);
							ObjectManager.SetUseTextureReflectionMetalnessInsteadOfColor(ObjectManager.GetActiveMaterialId(), false);
							ObjectManager.SetEnableReflectionMetalnessImage(ObjectManager.GetActiveMaterialId(), false);
							CallResetBuffer();
						}
					}
				}
			}

			// Set IOR

			if (ImGui::SliderFloat("IOR", &m_Ior_, 0.0f, 2.0f, "%.3f"))
			{
				ObjectManager.SetIor(ObjectManager.GetActiveMaterialId(), m_Ior_);

				CallResetBuffer();
			}

		}
		ImGui::PopID();
	}
}
void HorusInspector::DrawSheenSection()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	// Sheen Section
	{
		ImGui::PushID("Sheen");
		if (ImGui::CollapsingHeader("Sheen"))
		{
			// Sheen Color
			DrawSwitchColorToTextureButton(m_UseTextureSheenColorInsteadOfColor_, "SheenColorSwitch");
			ImGui::SameLine();
			if (m_UseTextureSheenColorInsteadOfColor_)
			{
				DrawParameterWithFileDialog(
					m_SheenColorPath_,
					m_EnableSheenColorImage_,
					"SheenColorPath",
					m_ImageFilters_,
					"Color",
					[this, &ObjectManager](const std::string& path)
					{
						ObjectManager.SetSheenColor(ObjectManager.GetActiveMaterialId(), path);
						ObjectManager.SetUseTextureSheenColorInsteadOfColor(ObjectManager.GetActiveMaterialId(), true);
						ObjectManager.SetEnableSheenColorImage(ObjectManager.GetActiveMaterialId(), true);

						CallResetBuffer();
					},
					[this, &ObjectManager](const std::string& path)
					{

					}
				);
			}
			else
			{
				if (ImGui::ColorEdit3("Color", &m_SheenColor_[0]))
				{
					ObjectManager.SetSheenColor(ObjectManager.GetActiveMaterialId(), m_SheenColor_);
					ObjectManager.SetUseTextureSheenColorInsteadOfColor(ObjectManager.GetActiveMaterialId(), false);
					ObjectManager.SetEnableSheenColorImage(ObjectManager.GetActiveMaterialId(), false);

					CallResetBuffer();
				}
			}

			// Sheen Weight
			DrawSwitchColorToTextureButton(m_UseTextureSheenWeightInsteadOfWeight_, "SheenWeightSwitch");
			ImGui::SameLine();
			if (m_UseTextureSheenWeightInsteadOfWeight_)
			{
				DrawParameterWithFileDialog(
					m_SheenWeightPath_,
					m_EnableSheenWeightImage_,
					"SheenWeightPath",
					m_ImageFilters_,
					"Weight",
					[this, &ObjectManager](const std::string& path)
					{
						ObjectManager.SetSheenWeight(ObjectManager.GetActiveMaterialId(), path);
						ObjectManager.SetUseTextureSheenWeightInsteadOfColor(ObjectManager.GetActiveMaterialId(), true);
						ObjectManager.SetEnableSheenWeightImage(ObjectManager.GetActiveMaterialId(), true);

						CallResetBuffer();
					},
					[this, &ObjectManager](const std::string& path)
					{

					}
				);
			}
			else
			{
				if (ImGui::SliderFloat("Weight", &m_SheenWeight_.x, 0.0f, 1.0f))
				{
					m_SheenWeight_.y = m_SheenWeight_.z = m_SheenWeight_.w = m_SheenWeight_.x;
					ObjectManager.SetSheenWeight(ObjectManager.GetActiveMaterialId(), m_SheenWeight_);
					ObjectManager.SetUseTextureSheenWeightInsteadOfColor(ObjectManager.GetActiveMaterialId(), false);
					ObjectManager.SetEnableSheenWeightImage(ObjectManager.GetActiveMaterialId(), false);

					CallResetBuffer();
				}
			}

			// Sheen Tint
			DrawSwitchColorToTextureButton(m_UseTextureSheenTintInsteadOfTint_, "SheenTintSwitch");
			ImGui::SameLine();
			if (m_UseTextureSheenTintInsteadOfTint_)
			{
				DrawParameterWithFileDialog(
					m_SheenTintPath_,
					m_EnableSheenTintImage_,
					"SheenTintPath",
					m_ImageFilters_,
					"Tint",
					[this, &ObjectManager](const std::string& path)
					{
						ObjectManager.SetSheenTint(ObjectManager.GetActiveMaterialId(), path);
						ObjectManager.SetUseTextureSheenTintInsteadOfColor(ObjectManager.GetActiveMaterialId(), true);
						ObjectManager.SetEnableSheenTintImage(ObjectManager.GetActiveMaterialId(), true);

						CallResetBuffer();
					},
					[this, &ObjectManager](const std::string& path)
					{

					}
				);
			}
			else
			{
				if (ImGui::ColorEdit3("Tint", &m_SheenTint_[0]))
				{
					ObjectManager.SetSheenTint(ObjectManager.GetActiveMaterialId(), m_SheenTint_);
					ObjectManager.SetUseTextureSheenTintInsteadOfColor(ObjectManager.GetActiveMaterialId(), false);
					ObjectManager.SetEnableSheenTintImage(ObjectManager.GetActiveMaterialId(), false);

					CallResetBuffer();
				}
			}
		}
		ImGui::PopID();
	}
}
void HorusInspector::DrawRefractionSection()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	// Refraction Section
	{
		ImGui::PushID("Refraction");
		if (ImGui::CollapsingHeader("Refraction"))
		{
			// Refraction Color
			DrawSwitchColorToTextureButton(m_UseTextureRefractionColorInsteadOfColor_, "RefractionColorSwitch");
			ImGui::SameLine();
			if (m_UseTextureRefractionColorInsteadOfColor_)
			{
				DrawParameterWithFileDialog(
					m_RefractionColorPath_,
					m_EnableRefractionColorImage_,
					"RefractionColorPath",
					m_ImageFilters_,
					"Color",
					[this, &ObjectManager](const std::string& path)
					{
						ObjectManager.SetRefractionColor(ObjectManager.GetActiveMaterialId(), path);
						ObjectManager.SetUseTextureRefractionColorInsteadOfColor(ObjectManager.GetActiveMaterialId(), true);
						ObjectManager.SetEnableRefractionColorImage(ObjectManager.GetActiveMaterialId(), true);

						CallResetBuffer();
					},
					[this, &ObjectManager](const std::string& path)
					{

					}
				);
			}
			else
			{
				if (ImGui::ColorEdit3("Color", &m_RefractionColor_[0]))
				{
					ObjectManager.SetRefractionColor(ObjectManager.GetActiveMaterialId(), m_RefractionColor_);
					ObjectManager.SetUseTextureRefractionColorInsteadOfColor(ObjectManager.GetActiveMaterialId(), false);
					ObjectManager.SetEnableRefractionColorImage(ObjectManager.GetActiveMaterialId(), false);

					CallResetBuffer();
				}
			}

			// Refraction Weigh
			DrawSwitchColorToTextureButton(m_UseTextureRefractionWeightInsteadOfWeight_, "RefractionWeightSwitch");
			ImGui::SameLine();
			if (m_UseTextureRefractionWeightInsteadOfWeight_)
			{
				DrawParameterWithFileDialog(
					m_RefractionWeightPath_,
					m_EnableRefractionWeightImage_,
					"RefractionWeightPath",
					m_ImageFilters_,
					"Weight",
					[this, &ObjectManager](const std::string& path)
					{
						ObjectManager.SetRefractionWeight(ObjectManager.GetActiveMaterialId(), path);
						ObjectManager.SetUseTextureRefractionWeightInsteadOfColor(ObjectManager.GetActiveMaterialId(), true);
						ObjectManager.SetEnableRefractionWeightImage(ObjectManager.GetActiveMaterialId(), true);

						CallResetBuffer();
					},
					[this, &ObjectManager](const std::string& path)
					{

					}
				);
			}
			else
			{
				if (ImGui::SliderFloat("Weight", &m_RefractionWeight_.x, 0.0f, 1.0f))
				{
					m_RefractionWeight_.y = m_RefractionWeight_.z = m_RefractionWeight_.w = m_RefractionWeight_.x;
					ObjectManager.SetRefractionWeight(ObjectManager.GetActiveMaterialId(), m_RefractionWeight_);
					ObjectManager.SetUseTextureRefractionWeightInsteadOfColor(ObjectManager.GetActiveMaterialId(), false);
					ObjectManager.SetEnableRefractionWeightImage(ObjectManager.GetActiveMaterialId(), false);

					CallResetBuffer();
				}
			}

			// Refraction Normal Map Weigh
			if (ImGui::SliderFloat("Normal Map Weight", &m_RefractionNormalMapWeight_, 0.0f, 1.0f))
			{
				ObjectManager.SetRefractionNormalWeight(ObjectManager.GetActiveMaterialId(), m_RefractionNormalMapWeight_);

				CallResetBuffer();
			}

			// Refraction Normal Map
			DrawSwitchColorToTextureButton(m_UseTextureRefractionNormalMapInsteadOfNormalMap_, "RefractionNormalMapSwitch");
			ImGui::SameLine();
			if (m_UseTextureRefractionNormalMapInsteadOfNormalMap_)
			{
				DrawParameterWithFileDialog(
					m_RefractionNormalMapPath_,
					m_EnableRefractionNormalMapImage_,
					"RefractionNormalMapPath",
					m_ImageFilters_,
					"Normal Map",
					[this, &ObjectManager](const std::string& path)
					{
						ObjectManager.SetRefractionNormal(ObjectManager.GetActiveMaterialId(), path);
						ObjectManager.SetUseTextureRefractionNormalInsteadOfColor(ObjectManager.GetActiveMaterialId(), true);
						ObjectManager.SetEnableRefractionNormalImage(ObjectManager.GetActiveMaterialId(), true);

						CallResetBuffer();
					},
					[this, &ObjectManager](const std::string& path)
					{

					}
				);
			}
			else
			{
				if (ImGui::SliderFloat("Normal Map", &m_RefractionNormalMap_.x, 0.0f, 1.0f))
				{
					m_RefractionNormalMap_.y = m_RefractionNormalMap_.z = m_RefractionNormalMap_.w = m_RefractionNormalMap_.x;
					ObjectManager.SetRefractionNormal(ObjectManager.GetActiveMaterialId(), m_RefractionNormalMap_);
					ObjectManager.SetUseTextureRefractionNormalInsteadOfColor(ObjectManager.GetActiveMaterialId(), false);
					ObjectManager.SetEnableRefractionNormalImage(ObjectManager.GetActiveMaterialId(), false);

					CallResetBuffer();
				}
			}

			// Refraction Roughness
			DrawSwitchColorToTextureButton(m_UseTextureRefractionRoughnessInsteadOfRoughness_, "RefractionRoughnessSwitch");
			ImGui::SameLine();
			if (m_UseTextureRefractionRoughnessInsteadOfRoughness_)
			{
				DrawParameterWithFileDialog(
					m_RefractionRoughnessPath_,
					m_EnableRefractionRoughnessImage_,
					"RefractionRoughnessPath",
					m_ImageFilters_,
					"Roughness",
					[this, &ObjectManager](const std::string& path)
					{
						ObjectManager.SetRefractionRoughness(ObjectManager.GetActiveMaterialId(), path);
						ObjectManager.SetUseTextureRefractionRoughnessInsteadOfColor(ObjectManager.GetActiveMaterialId(), true);
						ObjectManager.SetEnableRefractionRoughnessImage(ObjectManager.GetActiveMaterialId(), true);

						CallResetBuffer();
					},
					[this, &ObjectManager](const std::string& path)
					{

					}
				);
			}
			else
			{
				if (ImGui::SliderFloat("Roughness", &m_RefractionRoughness_.x, 0.0f, 1.0f))
				{
					m_RefractionRoughness_.y = m_RefractionRoughness_.z = m_RefractionRoughness_.w = m_RefractionRoughness_.x;
					ObjectManager.SetRefractionRoughness(ObjectManager.GetActiveMaterialId(), m_RefractionRoughness_);
					ObjectManager.SetUseTextureRefractionRoughnessInsteadOfColor(ObjectManager.GetActiveMaterialId(), false);
					ObjectManager.SetEnableRefractionRoughnessImage(ObjectManager.GetActiveMaterialId(), false);

					CallResetBuffer();
				}
			}

			// IOR
			if (ImGui::SliderFloat("IOR", &m_Ior_, 0.0f, 2.0f, "%.3f"))
			{
				ObjectManager.SetRefractionIor(ObjectManager.GetActiveMaterialId(), m_Ior_);

				CallResetBuffer();
			}

			// Thin Surface
			if (ImGui::Checkbox("Thin Surface", &m_RefractionThinSurface_))
			{
				ObjectManager.SetRefractionThinSurface(ObjectManager.GetActiveMaterialId(), m_RefractionThinSurface_);

				CallResetBuffer();
			}

			// Absorption Color
			DrawSwitchColorToTextureButton(m_UseTextureRefractionAbsorptionColorInsteadOfColor_, "RefractionAbsorptionColorSwitch");
			ImGui::SameLine();
			if (m_UseTextureRefractionAbsorptionColorInsteadOfColor_)
			{
				DrawParameterWithFileDialog(
					m_RefractionAbsorptionColorPath_,
					m_EnableRefractionAbsorptionColorImage_,
					"RefractionAbsorptionColorPath",
					m_ImageFilters_,
					"Absorption Color",
					[this, &ObjectManager](const std::string& path)
					{
						ObjectManager.SetRefractionAbsorptionColor(ObjectManager.GetActiveMaterialId(), path);
						ObjectManager.SetUseTextureRefractionAbsorptionColorInsteadOfColor(ObjectManager.GetActiveMaterialId(), true);
						ObjectManager.SetEnableRefractionAbsorptionColorImage(ObjectManager.GetActiveMaterialId(), true);

						CallResetBuffer();
					},
					[this, &ObjectManager](const std::string& path)
					{

					}
				);
			}
			else
			{
				if (ImGui::ColorEdit3("Absorption Color", &m_RefractionAbsorptionColor_[0]))
				{
					ObjectManager.SetRefractionAbsorptionColor(ObjectManager.GetActiveMaterialId(), m_RefractionAbsorptionColor_);
					ObjectManager.SetUseTextureRefractionAbsorptionColorInsteadOfColor(ObjectManager.GetActiveMaterialId(), false);
					ObjectManager.SetEnableRefractionAbsorptionColorImage(ObjectManager.GetActiveMaterialId(), false);

					CallResetBuffer();
				}
			}

			// Absorption Distance
			DrawSwitchColorToTextureButton(m_UseTextureRefractionAbsorptionDistanceInsteadOfDistance_, "RefractionAbsorptionDistanceSwitch");
			ImGui::SameLine();
			if (m_UseTextureRefractionAbsorptionDistanceInsteadOfDistance_)
			{
				DrawParameterWithFileDialog(
					m_RefractionAbsorptionDistancePath_,
					m_EnableRefractionAbsorptionDistanceImage_,
					"RefractionAbsorptionDistancePath",
					m_ImageFilters_,
					"Absorption Distance",
					[this, &ObjectManager](const std::string& path)
					{
						ObjectManager.SetRefractionAbsorptionDistance(ObjectManager.GetActiveMaterialId(), path);
						ObjectManager.SetUseTextureRefractionAbsorptionDistanceInsteadOfColor(ObjectManager.GetActiveMaterialId(), true);
						ObjectManager.SetEnableRefractionAbsorptionDistanceImage(ObjectManager.GetActiveMaterialId(), true);

						CallResetBuffer();
					},
					[this, &ObjectManager](const std::string& path)
					{

					}
				);
			}
			else
			{
				if (ImGui::SliderFloat("Absorption Distance", &m_RefractionAbsorptionDistance_.x, 0.0f, 1.0f))
				{
					m_RefractionAbsorptionDistance_.y = m_RefractionAbsorptionDistance_.z = m_RefractionAbsorptionDistance_.w = m_RefractionAbsorptionDistance_.x;
					ObjectManager.SetRefractionAbsorptionDistance(ObjectManager.GetActiveMaterialId(), m_RefractionAbsorptionDistance_);
					ObjectManager.SetUseTextureRefractionAbsorptionDistanceInsteadOfColor(ObjectManager.GetActiveMaterialId(), false);
					ObjectManager.SetEnableRefractionAbsorptionDistanceImage(ObjectManager.GetActiveMaterialId(), false);
					CallResetBuffer();
				}
			}

			// Caustics
			if (ImGui::Checkbox("Caustics", &m_RefractionCaustics_))
			{
				ObjectManager.SetRefractionCaustics(ObjectManager.GetActiveMaterialId(), m_RefractionCaustics_);

				CallResetBuffer();
			}
		}
		ImGui::PopID();
	}
}
void HorusInspector::DrawSssSection()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	// Sub-Surface Scattering Section
	{
		ImGui::PushID("Sub-Surface Scattering");
		if (ImGui::CollapsingHeader("Sub-Surface Scattering"))
		{
			// SSS Color
			DrawSwitchColorToTextureButton(m_UseTextureSssColorInsteadOfColor_, "SSSColorSwitch");
			ImGui::SameLine();
			if (m_UseTextureSssColorInsteadOfColor_)
			{
				DrawParameterWithFileDialog(
					m_SssColorPath_,
					m_EnableSssColorImage_,
					"SSSColorPath",
					m_ImageFilters_,
					"Color",
					[this, &ObjectManager](const std::string& path)
					{
						(ObjectManager.GetActiveMaterialId(), path);
						ObjectManager.SetUseTextureSssScatterColorInsteadOfColor(ObjectManager.GetActiveMaterialId(), true);
						ObjectManager.SetEnableSssScatterColorImage(ObjectManager.GetActiveMaterialId(), true);

						CallResetBuffer();
					},
					[this, &ObjectManager](const std::string& path)
					{

					}
				);
			}
			else
			{
				if (ImGui::ColorEdit3("Color", &m_SssScatterColor_[0]))
				{
					ObjectManager.SetSssScatterColor(ObjectManager.GetActiveMaterialId(), m_SssScatterColor_);
					ObjectManager.SetUseTextureSssScatterColorInsteadOfColor(ObjectManager.GetActiveMaterialId(), false);
					ObjectManager.SetEnableSssScatterColorImage(ObjectManager.GetActiveMaterialId(), false);

					CallResetBuffer();
				}
			}


			// SSS Weight
			DrawSwitchColorToTextureButton(m_UseTextureSssWeightInsteadOfWeight_, "SSSWeightSwitch");
			ImGui::SameLine();
			if (m_UseTextureSssWeightInsteadOfWeight_)
			{
				DrawParameterWithFileDialog(
					m_SssWeightPath_,
					m_EnableSssWeightImage_,
					"SSSWeightPath",
					m_ImageFilters_,
					"Weight",
					[this, &ObjectManager](const std::string& path)
					{
						ObjectManager.SetSssScatterWeight(ObjectManager.GetActiveMaterialId(), path);
						ObjectManager.SetUseTextureSssScatterWeightInsteadOfColor(ObjectManager.GetActiveMaterialId(), true);
						ObjectManager.SetEnableSssScatterWeightImage(ObjectManager.GetActiveMaterialId(), true);

						CallResetBuffer();
					},
					[this, &ObjectManager](const std::string& path)
					{

					}
				);
			}
			else
			{
				if (ImGui::SliderFloat("Weight", &m_SssScatterWeight_.x, 0.0f, 1.0f))
				{
					m_SssScatterWeight_.y = m_SssScatterWeight_.z = m_SssScatterWeight_.w = m_SssScatterWeight_.x;
					ObjectManager.SetSssScatterWeight(ObjectManager.GetActiveMaterialId(), m_SssScatterWeight_);
					ObjectManager.SetUseTextureSssScatterWeightInsteadOfColor(ObjectManager.GetActiveMaterialId(), false);
					ObjectManager.SetEnableSssScatterWeightImage(ObjectManager.GetActiveMaterialId(), false);

					CallResetBuffer();
				}
			}

			// SSS Distance
			DrawSwitchColorToTextureButton(m_UseTextureSssRadiusInsteadOfRadius_, "SSSDistanceSwitch");
			ImGui::SameLine();
			if (m_UseTextureSssRadiusInsteadOfRadius_)
			{
				DrawParameterWithFileDialog(
					m_SssRadiusPath_,
					m_EnableSssRadiusImage_,
					"SSSDistancePath",
					m_ImageFilters_,
					"Distance",
					[this, &ObjectManager](const std::string& path)
					{
						ObjectManager.SetSssScatterDistance(ObjectManager.GetActiveMaterialId(), path);
						ObjectManager.SetUseTextureSssScatterDistanceInsteadOfColor(ObjectManager.GetActiveMaterialId(), true);
						ObjectManager.SetEnableSssScatterDistanceImage(ObjectManager.GetActiveMaterialId(), true);

						CallResetBuffer();
					},
					[this, &ObjectManager](const std::string& path)
					{

					}
				);
			}
			else
			{
				if (ImGui::SliderFloat("Distance", &m_SssScatterDistance_.x, 0.0f, 1.0f))
				{
					m_SssScatterDistance_.y = m_SssScatterDistance_.z = m_SssScatterDistance_.w = m_SssScatterDistance_.x;
					ObjectManager.SetSssScatterDistance(ObjectManager.GetActiveMaterialId(), m_SssScatterDistance_);
					ObjectManager.SetUseTextureSssScatterDistanceInsteadOfColor(ObjectManager.GetActiveMaterialId(), false);
					ObjectManager.SetEnableSssScatterDistanceImage(ObjectManager.GetActiveMaterialId(), false);

					CallResetBuffer();
				}
			}

			// SSS Direction
			DrawSwitchColorToTextureButton(m_UseTextureSssDirectionInsteadOfDirection_, "SSSDirectionSwitch");
			ImGui::SameLine();
			if (m_UseTextureSssDirectionInsteadOfDirection_)
			{
				DrawParameterWithFileDialog(
					m_SssDirectionPath_,
					m_EnableSssDirectionImage_,
					"SSSDirectionPath",
					m_ImageFilters_,
					"Direction",
					[this, &ObjectManager](const std::string& path)
					{
						ObjectManager.SetSssScatterDirection(ObjectManager.GetActiveMaterialId(), path);
						ObjectManager.SetUseTextureSssScatterDirectionInsteadOfColor(ObjectManager.GetActiveMaterialId(), true);
						ObjectManager.SetEnableSssScatterDirectionImage(ObjectManager.GetActiveMaterialId(), true);

						CallResetBuffer();
					},
					[this, &ObjectManager](const std::string& path)
					{

					}
				);
			}
			else
			{
				if (ImGui::SliderFloat("Direction", &m_SssScatterDirection_.x, 0.0f, 1.0f))
				{
					m_SssScatterDirection_.y = m_SssScatterDirection_.z = m_SssScatterDirection_.w = m_SssScatterDirection_.x;
					ObjectManager.SetSssScatterDirection(ObjectManager.GetActiveMaterialId(), m_SssScatterDirection_);
					ObjectManager.SetUseTextureSssScatterDirectionInsteadOfColor(ObjectManager.GetActiveMaterialId(), false);
					ObjectManager.SetEnableSssScatterDirectionImage(ObjectManager.GetActiveMaterialId(), false);

					CallResetBuffer();
				}
			}

			// SSS Use Multi Scattering
			if (ImGui::Checkbox("Multiple Scattering", &m_SssUseMultiScattering_))
			{
				ObjectManager.SetSssUseMultiScattering(ObjectManager.GetActiveMaterialId(), m_SssUseMultiScattering_);

				CallResetBuffer();
			}

			// Checkbox use schlick approx
			if (ImGui::Checkbox("Use Schlick Approximation", &m_SssUseSchlickApproximation_))
			{
				ObjectManager.SetSssUseSchlickApproximation(ObjectManager.GetActiveMaterialId(), m_SssUseSchlickApproximation_);

				CallResetBuffer();
			}
		}
		ImGui::PopID();
	}
}
void HorusInspector::DrawCoatingSection()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	// Coating Section
	{
		ImGui::PushID("Coating");
		if (ImGui::CollapsingHeader("Coating"))
		{
			// Coating Color
			DrawSwitchColorToTextureButton(m_UseTextureCoatingColorInsteadOfColor_, "CoatingColorSwitch");
			ImGui::SameLine();
			if (m_UseTextureCoatingColorInsteadOfColor_)
			{
				DrawParameterWithFileDialog(
					m_CoatingColorPath_,
					m_EnableCoatingColorImage_,
					"CoatingColorPath",
					m_ImageFilters_,
					"Color",
					[this, &ObjectManager](const std::string& path)
					{
						ObjectManager.SetCoatingColor(ObjectManager.GetActiveMaterialId(), path);
						ObjectManager.SetUseTextureCoatingColorInsteadOfColor(ObjectManager.GetActiveMaterialId(), true);
						ObjectManager.SetEnableCoatingColorImage(ObjectManager.GetActiveMaterialId(), true);

						CallResetBuffer();
					},
					[this, &ObjectManager](const std::string& path)
					{

					}
				);
			}
			else
			{
				if (ImGui::ColorEdit3("Color", &m_CoatingColor_[0]))
				{
					ObjectManager.SetCoatingColor(ObjectManager.GetActiveMaterialId(), m_CoatingColor_);
					ObjectManager.SetUseTextureCoatingColorInsteadOfColor(ObjectManager.GetActiveMaterialId(), false);
					ObjectManager.SetEnableCoatingColorImage(ObjectManager.GetActiveMaterialId(), false);

					CallResetBuffer();
				}
			}

			// Coating Weight
			DrawSwitchColorToTextureButton(m_UseTextureCoatingWeightInsteadOfWeight_, "CoatingWeightSwitch");
			ImGui::SameLine();
			if (m_UseTextureCoatingWeightInsteadOfWeight_)
			{
				DrawParameterWithFileDialog(
					m_CoatingWeightPath_,
					m_EnableCoatingWeightImage_,
					"CoatingWeightPath",
					m_ImageFilters_,
					"Weight",
					[this, &ObjectManager](const std::string& path)
					{
						ObjectManager.SetCoatingWeight(ObjectManager.GetActiveMaterialId(), path);
						ObjectManager.SetUseTextureCoatingWeightInsteadOfColor(ObjectManager.GetActiveMaterialId(), true);
						ObjectManager.SetEnableCoatingWeightImage(ObjectManager.GetActiveMaterialId(), true);

						CallResetBuffer();
					},
					[this, &ObjectManager](const std::string& path)
					{

					}
				);
			}
			else
			{
				if (ImGui::SliderFloat("Weight", &m_CoatingWeight_.x, 0.0f, 1.0f))
				{
					m_CoatingWeight_.y = m_CoatingWeight_.z = m_CoatingWeight_.w = m_CoatingWeight_.x;
					ObjectManager.SetCoatingWeight(ObjectManager.GetActiveMaterialId(), m_CoatingWeight_);
					ObjectManager.SetUseTextureCoatingWeightInsteadOfColor(ObjectManager.GetActiveMaterialId(), false);
					ObjectManager.SetEnableCoatingWeightImage(ObjectManager.GetActiveMaterialId(), false);

					CallResetBuffer();
				}
			}

			// Coating Normal Map
			DrawSwitchColorToTextureButton(m_UseTextureCoatingNormalMapInsteadOfNormalMap_, "CoatingNormalMapSwitch");
			ImGui::SameLine();
			if (m_UseTextureCoatingNormalMapInsteadOfNormalMap_)
			{
				DrawParameterWithFileDialog(
					m_CoatingNormalMapPath_,
					m_EnableCoatingNormalMapImage_,
					"CoatingNormalMapPath",
					m_ImageFilters_,
					"Normal Map",
					[this, &ObjectManager](const std::string& path)
					{
						ObjectManager.SetCoatingNormalMap(ObjectManager.GetActiveMaterialId(), path);
						ObjectManager.SetUseTextureCoatingNormalInsteadOfColor(ObjectManager.GetActiveMaterialId(), true);
						ObjectManager.SetEnableCoatingNormalImage(ObjectManager.GetActiveMaterialId(), true);

						CallResetBuffer();
					},
					[this, &ObjectManager](const std::string& path)
					{

					}
				);
			}
			else
			{
				if (ImGui::SliderFloat("Normal Map", &m_CoatingNormalMap_.x, 0.0f, 1.0f))
				{
					m_CoatingNormalMap_.y = m_CoatingNormalMap_.z = m_CoatingNormalMap_.w = m_CoatingNormalMap_.x;
					ObjectManager.SetCoatingNormalMap(ObjectManager.GetActiveMaterialId(), m_CoatingNormalMap_);
					ObjectManager.SetUseTextureCoatingNormalInsteadOfColor(ObjectManager.GetActiveMaterialId(), false);
					ObjectManager.SetEnableCoatingNormalImage(ObjectManager.GetActiveMaterialId(), false);

					CallResetBuffer();
				}
			}

			// Coating Normal Map Weight
			if (ImGui::SliderFloat("Normal Map Weight", &m_CoatingNormalMapWeight_, 0.0f, 1.0f))
			{
				ObjectManager.SetCoatingNormalWeight(ObjectManager.GetActiveMaterialId(), m_CoatingNormalMapWeight_);

				CallResetBuffer();
			}

			// Coating Roughness
			DrawSwitchColorToTextureButton(m_UseTextureCoatingRoughnessInsteadOfRoughness_, "CoatingRoughnessSwitch");
			ImGui::SameLine();
			if (m_UseTextureCoatingRoughnessInsteadOfRoughness_)
			{
				DrawParameterWithFileDialog(
					m_CoatingRoughnessPath_,
					m_EnableCoatingRoughnessImage_,
					"CoatingRoughnessPath",
					m_ImageFilters_,
					"Roughness",
					[this, &ObjectManager](const std::string& path)
					{
						ObjectManager.SetCoatingRoughness(ObjectManager.GetActiveMaterialId(), path);
						ObjectManager.SetUseTextureCoatingRoughnessInsteadOfColor(ObjectManager.GetActiveMaterialId(), true);
						ObjectManager.SetEnableCoatingRoughnessImage(ObjectManager.GetActiveMaterialId(), true);

						CallResetBuffer();
					},
					[this, &ObjectManager](const std::string& path)
					{

					}
				);
			}
			else
			{
				if (ImGui::SliderFloat("Roughness", &m_CoatingRoughness_.x, 0.0f, 1.0f))
				{
					m_CoatingRoughness_.y = m_CoatingRoughness_.z = m_CoatingRoughness_.w = m_CoatingRoughness_.x;
					ObjectManager.SetCoatingRoughness(ObjectManager.GetActiveMaterialId(), m_CoatingRoughness_);
					ObjectManager.SetUseTextureCoatingRoughnessInsteadOfColor(ObjectManager.GetActiveMaterialId(), false);
					ObjectManager.SetEnableCoatingRoughnessImage(ObjectManager.GetActiveMaterialId(), false);

					CallResetBuffer();
				}
			}

			// Coating IOR
			if (ImGui::SliderFloat("IOR", &m_CoatingIor_, 0.0f, 2.0f, "%.3f"))
			{
				ObjectManager.SetCoatingIor(ObjectManager.GetActiveMaterialId(), m_CoatingIor_);

				CallResetBuffer();
			}

			// Coating Thickness
			DrawSwitchColorToTextureButton(m_UseTextureCoatingThicknessInsteadOfThickness_, "CoatingThicknessSwitch");
			ImGui::SameLine();
			if (m_UseTextureCoatingThicknessInsteadOfThickness_)
			{
				DrawParameterWithFileDialog(
					m_CoatingThicknessPath_,
					m_EnableCoatingThicknessImage_,
					"CoatingThicknessPath",
					m_ImageFilters_,
					"Thickness",
					[this, &ObjectManager](const std::string& path)
					{
						ObjectManager.SetCoatingThickness(ObjectManager.GetActiveMaterialId(), path);
						ObjectManager.SetUseTextureCoatingThicknessInsteadOfColor(ObjectManager.GetActiveMaterialId(), true);
						ObjectManager.SetEnableCoatingThicknessImage(ObjectManager.GetActiveMaterialId(), true);

						CallResetBuffer();
					},
					[this, &ObjectManager](const std::string& path)
					{

					}
				);
			}
			else
			{
				if (ImGui::SliderFloat("Thickness", &m_CoatingThickness_.x, 0.0f, 1.0f))
				{
					m_CoatingThickness_.y = m_CoatingThickness_.z = m_CoatingThickness_.w = m_CoatingThickness_.x;
					ObjectManager.SetCoatingThickness(ObjectManager.GetActiveMaterialId(), m_CoatingThickness_);
					ObjectManager.SetUseTextureCoatingThicknessInsteadOfColor(ObjectManager.GetActiveMaterialId(), false);
					ObjectManager.SetEnableCoatingThicknessImage(ObjectManager.GetActiveMaterialId(), false);

					CallResetBuffer();
				}
			}

			// Coating Transmission Color
			DrawSwitchColorToTextureButton(m_UseTextureCoatingTransmissionColorInsteadOfTransmissionColor_, "CoatingTransmissionColorSwitch");
			ImGui::SameLine();
			if (m_UseTextureCoatingTransmissionColorInsteadOfTransmissionColor_)
			{
				DrawParameterWithFileDialog(
					m_CoatingTransmissionColorPath_,
					m_EnableCoatingTransmissionColorImage_,
					"CoatingTransmissionColorPath",
					m_ImageFilters_,
					"Transmission Color",
					[this, &ObjectManager](const std::string& path)
					{
						ObjectManager.SetCoatingTransmissionColor(ObjectManager.GetActiveMaterialId(), path);
						ObjectManager.SetUseTextureCoatingTransmissionColorInsteadOfColor(ObjectManager.GetActiveMaterialId(), true);
						ObjectManager.SetEnableCoatingTransmissionColorImage(ObjectManager.GetActiveMaterialId(), true);

						CallResetBuffer();
					},
					[this, &ObjectManager](const std::string& path)
					{

					}
				);
			}
			else
			{
				if (ImGui::ColorEdit3("Transmission Color", &m_CoatingTransmissionColor_[0]))
				{
					ObjectManager.SetCoatingTransmissionColor(ObjectManager.GetActiveMaterialId(), m_CoatingTransmissionColor_);
					ObjectManager.SetUseTextureCoatingTransmissionColorInsteadOfColor(ObjectManager.GetActiveMaterialId(), false);
					ObjectManager.SetEnableCoatingTransmissionColorImage(ObjectManager.GetActiveMaterialId(), false);

					CallResetBuffer();
				}
			}

			// Combo Coating Mode
			if (ImGui::Combo("Coating Mode", &m_CoatingModeIndex_, m_CoatingModes_, IM_ARRAYSIZE(m_CoatingModes_)))
			{
				if (m_CoatingModeIndex_ == 0)
				{
					ObjectManager.SetCoatingMode(ObjectManager.GetActiveMaterialId(), HorusMaterial::CoatingTypePBR);
				}
				else if (m_CoatingModeIndex_ == 1)
				{
					ObjectManager.SetCoatingMode(ObjectManager.GetActiveMaterialId(), HorusMaterial::CoatingTypeMetalness);
				}

				CallResetBuffer();
			}

			// If metalness
			if (m_CoatingModeIndex_ == 0)
			{
				m_CoatingMetalnessUI_ = false;
			}
			else if (m_CoatingModeIndex_ == 1)
			{
				m_CoatingMetalnessUI_ = true;
			}

			if (m_CoatingMetalnessUI_)
			{
				// Coating Metalness
				DrawSwitchColorToTextureButton(m_UseTextureCoatingMetalnessInsteadOfMetalness_, "CoatingMetalnessSwitch");
				ImGui::SameLine();
				if (m_UseTextureCoatingMetalnessInsteadOfMetalness_)
				{
					DrawParameterWithFileDialog(
						m_CoatingMetalnessPath_,
						m_EnableCoatingMetalnessImage_,
						"CoatingMetalnessPath",
						m_ImageFilters_,
						"Metallic",
						[this, &ObjectManager](const std::string& path)
						{
							ObjectManager.SetCoatingMetalness(ObjectManager.GetActiveMaterialId(), path);
							ObjectManager.SetUseTextureCoatingMetalnessInsteadOfColor(ObjectManager.GetActiveMaterialId(), true);
							ObjectManager.SetEnableCoatingMetalnessImage(ObjectManager.GetActiveMaterialId(), true);

							CallResetBuffer();
						},
						[this, &ObjectManager](const std::string& path)
						{

						}
					);
				}
				else
				{
					if (ImGui::SliderFloat("Metallic", &m_CoatingMetalness_.x, 0.0f, 1.0f))
					{
						m_CoatingMetalness_.y = m_CoatingMetalness_.z = m_CoatingMetalness_.w = m_CoatingMetalness_.x;
						ObjectManager.SetCoatingMetalness(ObjectManager.GetActiveMaterialId(), m_CoatingMetalness_);
						ObjectManager.SetUseTextureCoatingMetalnessInsteadOfColor(ObjectManager.GetActiveMaterialId(), false);
						ObjectManager.SetEnableCoatingMetalnessImage(ObjectManager.GetActiveMaterialId(), false);

						CallResetBuffer();
					}
				}
			}

		}
		ImGui::PopID();
	}
}
void HorusInspector::DrawOtherSection()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	// Other Section
	{
		ImGui::PushID("Other");
		if (ImGui::CollapsingHeader("Other"))
		{
			// Normal Map
			DrawSwitchColorToTextureButton(m_UseTextureNormalMapInsteadOfNormalMap_, "NormalMapSwitch");
			ImGui::SameLine();
			if (m_UseTextureNormalMapInsteadOfNormalMap_)
			{
				DrawParameterWithFileDialog(
					m_NormalMapPath_,
					m_EnableNormalMapImage_,
					"NormalMapPath",
					m_ImageFilters_,
					"Normal Map",
					[this, &ObjectManager](const std::string& path)
					{
						ObjectManager.SetNormalMap(ObjectManager.GetActiveMaterialId(), path);
						ObjectManager.SetUseTextureNormalInsteadOfColor(ObjectManager.GetActiveMaterialId(), true);
						ObjectManager.SetEnableNormalImage(ObjectManager.GetActiveMaterialId(), true);

						CallResetBuffer();
					},
					[this, &ObjectManager](const std::string& path)
					{

					}
				);
			}
			else
			{
				if (ImGui::SliderFloat("Normal Map", &m_NormalMap_.x, 0.0f, 1.0f))
				{
					m_NormalMap_.y = m_NormalMap_.z = m_NormalMap_.w = m_NormalMap_.x;
					ObjectManager.SetNormalMap(ObjectManager.GetActiveMaterialId(), m_NormalMap_);
					ObjectManager.SetUseTextureNormalInsteadOfColor(ObjectManager.GetActiveMaterialId(), false);
					ObjectManager.SetEnableNormalImage(ObjectManager.GetActiveMaterialId(), false);

					CallResetBuffer();
				}
			}

			// Normal Map Weight
			if (ImGui::SliderFloat("Normal Map Weight", &m_NormalMapWeight_.x, 0.0f, 0.1f))
			{
				m_NormalMapWeight_.y = m_NormalMapWeight_.z = m_NormalMapWeight_.w = m_NormalMapWeight_.x;
				ObjectManager.SetNormalWeight(ObjectManager.GetActiveMaterialId(), m_NormalMapWeight_);

				CallResetBuffer();
			}

			// Disabling this part
			ImGui::BeginDisabled();
			// Displacement Map
			DrawSwitchColorToTextureButton(m_UseTextureDisplacementMapInsteadOfDisplacementMap_, "DisplacementMapSwitch");
			ImGui::SameLine();
			if (m_UseTextureDisplacementMapInsteadOfDisplacementMap_)
			{
				DrawParameterWithFileDialog(
					m_DisplacementMapPath_,
					m_EnableDisplacementMapImage_,
					"DisplacementMapPath",
					m_ImageFilters_,
					"Displacement Map",
					[this, &ObjectManager](const std::string& path)
					{
						ObjectManager.SetDisplacementMap(ObjectManager.GetActiveMaterialId(), path);
						ObjectManager.SetUseTextureDisplacementInsteadOfColor(ObjectManager.GetActiveMaterialId(), true);
						ObjectManager.SetEnableDisplacementImage(ObjectManager.GetActiveMaterialId(), true);

						CallResetBuffer();
					},
					[this, &ObjectManager](const std::string& path)
					{

					}
				);
			}
			else
			{
				if (ImGui::SliderFloat("Displacement Map", &m_DisplacementMap_.x, 0.0f, 1.0f))
				{
					m_DisplacementMap_.y = m_DisplacementMap_.z = m_DisplacementMap_.w = m_DisplacementMap_.x;
					ObjectManager.SetDisplacementMap(ObjectManager.GetActiveMaterialId(), m_DisplacementMap_);
					ObjectManager.SetUseTextureDisplacementInsteadOfColor(ObjectManager.GetActiveMaterialId(), false);
					ObjectManager.SetEnableDisplacementImage(ObjectManager.GetActiveMaterialId(), false);

					CallResetBuffer();
				}
			}

			// Displacement Map Weight
			if (ImGui::SliderFloat("Displacement Map Weight", &m_DisplacementMapWeight_.x, 0.0f, 1.0f))
			{
				m_DisplacementMapWeight_.y = m_DisplacementMapWeight_.z = m_DisplacementMapWeight_.w = m_DisplacementMapWeight_.x;
				ObjectManager.SetDisplacementWeight(ObjectManager.GetActiveMaterialId(), m_DisplacementMapWeight_);

				CallResetBuffer();
			}
			ImGui::EndDisabled();

			// Emissive Color
			DrawSwitchColorToTextureButton(m_UseTextureEmissiveInsteadOfEmissive_, "EmissiveColorSwitch");
			ImGui::SameLine();
			if (m_UseTextureEmissiveInsteadOfEmissive_)
			{
				DrawParameterWithFileDialog(
					m_EmissivePath_,
					m_EnableEmissiveImage_,
					"EmissivePath",
					m_ImageFilters_,
					"Emissive Color",
					[this, &ObjectManager](const std::string& path)
					{
						ObjectManager.SetEmissive(ObjectManager.GetActiveMaterialId(), path);
						ObjectManager.SetUseTextureEmissiveInsteadOfColor(ObjectManager.GetActiveMaterialId(), true);
						ObjectManager.SetEnableEmissiveImage(ObjectManager.GetActiveMaterialId(), true);

						CallResetBuffer();
					},
					[this, &ObjectManager](const std::string& path)
					{

					}
				);
			}
			else
			{
				if (ImGui::ColorEdit3("Emissive Color", &m_Emissive_[0]))
				{
					ObjectManager.SetEmissive(ObjectManager.GetActiveMaterialId(), m_Emissive_);
					ObjectManager.SetUseTextureEmissiveInsteadOfColor(ObjectManager.GetActiveMaterialId(), false);
					ObjectManager.SetEnableEmissiveImage(ObjectManager.GetActiveMaterialId(), false);

					CallResetBuffer();
				}
			}

			// Emissive Weight
			DrawSwitchColorToTextureButton(m_UseTextureEmissiveWeightInsteadOfEmissiveWeight_, "EmissiveWeightSwitch");
			ImGui::SameLine();
			if (m_UseTextureEmissiveWeightInsteadOfEmissiveWeight_)
			{
				DrawParameterWithFileDialog(
					m_EmissiveWeightPath_,
					m_EnableEmissiveWeightImage_,
					"EmissiveWeightPath",
					m_ImageFilters_,
					"Emissive Weight",
					[this, &ObjectManager](const std::string& path)
					{
						ObjectManager.SetEmissiveWeight(ObjectManager.GetActiveMaterialId(), path);
						ObjectManager.SetUseTextureEmissiveWeightInsteadOfColor(ObjectManager.GetActiveMaterialId(), true);
						ObjectManager.SetEnableEmissiveWeightImage(ObjectManager.GetActiveMaterialId(), true);

						CallResetBuffer();
					},
					[this, &ObjectManager](const std::string& path)
					{

					}
				);
			}
			else
			{
				if (ImGui::SliderFloat("Emissive Weight", &m_EmissionWeight_.x, 0.0f, 1.0f))
				{
					m_EmissionWeight_.y = m_EmissionWeight_.z = m_EmissionWeight_.w = m_EmissionWeight_.x;
					ObjectManager.SetEmissiveWeight(ObjectManager.GetActiveMaterialId(), m_EmissionWeight_);
					ObjectManager.SetUseTextureEmissiveWeightInsteadOfColor(ObjectManager.GetActiveMaterialId(), false);
					ObjectManager.SetEnableEmissiveWeightImage(ObjectManager.GetActiveMaterialId(), false);

					CallResetBuffer();
				}
			}

			// Opacity
			DrawSwitchColorToTextureButton(m_UseTextureOpacityInsteadOfOpacity_, "OpacitySwitch");
			ImGui::SameLine();
			if (m_UseTextureOpacityInsteadOfOpacity_)
			{
				DrawParameterWithFileDialog(
					m_OpacityPath_,
					m_EnableOpacityImage_,
					"OpacityPath",
					m_ImageFilters_,
					"Opacity",
					[this, &ObjectManager](const std::string& path)
					{
						ObjectManager.SetOpacity(ObjectManager.GetActiveMaterialId(), path);
						ObjectManager.SetUseTextureOpacityInsteadOfColor(ObjectManager.GetActiveMaterialId(), true);
						ObjectManager.SetEnableOpacityImage(ObjectManager.GetActiveMaterialId(), true);

						CallResetBuffer();
					},
					[this, &ObjectManager](const std::string& path)
					{

					}
				);
			}
			else
			{
				if (ImGui::SliderFloat("Opacity", &m_Opacity_.x, 0.0f, 1.0f))
				{
					m_Opacity_.y = m_Opacity_.z = m_Opacity_.w = m_Opacity_.x;
					ObjectManager.SetOpacity(ObjectManager.GetActiveMaterialId(), m_Opacity_);
					ObjectManager.SetUseTextureOpacityInsteadOfColor(ObjectManager.GetActiveMaterialId(), false);
					ObjectManager.SetEnableOpacityImage(ObjectManager.GetActiveMaterialId(), false);

					CallResetBuffer();
				}
			}
		}
		ImGui::PopID();
	}
}

// Preset Material system (TODO)
void HorusInspector::SetMaterialDefault()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	// Set default material
	// Base Color
	ObjectManager.SetBaseColor(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetBaseColorWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetBaseColorRoughness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetBackscatterColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetBackscatterWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

	// Reflection
	ObjectManager.SetReflectionColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetReflectionWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetReflectionRoughness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetReflectionAnisotropy(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetIor(ObjectManager.GetActiveMaterialId(), 1.36f);
	ObjectManager.SetReflectionMode(ObjectManager.GetActiveMaterialId(), HorusMaterial::ReflectionTypeMetalness);
	ObjectManager.SetReflectionMetalness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

	// Sheen
	ObjectManager.SetSheenColor(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetSheenWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetSheenTint(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

	// Refraction
	ObjectManager.SetRefractionColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetRefractionWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetRefractionRoughness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetRefractionNormal(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetRefractionIor(ObjectManager.GetActiveMaterialId(), 1.36f);
	ObjectManager.SetRefractionAbsorptionColor(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetRefractionAbsorptionDistance(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetRefractionCaustics(ObjectManager.GetActiveMaterialId(), false);

	// SSS
	ObjectManager.SetSssScatterColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetSssScatterWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetSssScatterDirection(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetSssScatterDistance(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetSssUseMultiScattering(ObjectManager.GetActiveMaterialId(), false);
	ObjectManager.SetSssUseSchlickApproximation(ObjectManager.GetActiveMaterialId(), false);

	// Coating
	ObjectManager.SetCoatingColor(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetCoatingWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetCoatingNormalMap(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetCoatingNormalWeight(ObjectManager.GetActiveMaterialId(), 0.0f);
	ObjectManager.SetCoatingRoughness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetCoatingIor(ObjectManager.GetActiveMaterialId(), 1.36f);
	ObjectManager.SetCoatingThickness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetCoatingTransmissionColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetCoatingMode(ObjectManager.GetActiveMaterialId(), HorusMaterial::CoatingTypeMetalness);
	ObjectManager.SetCoatingMetalness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

	// Other
	ObjectManager.SetNormalWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetDisplacementMap(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetDisplacementWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetEmissive(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetEmissiveWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetOpacity(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)); // 0.0f = opaque, 1.0f = transparent

	PopulateSelectedMaterialInfos();
	CallResetBuffer();
}
void HorusInspector::SetMaterialPlastic()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	// Set default material
	// Base Color
	ObjectManager.SetBaseColor(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.2f, 0.65f, 1.0f));
	ObjectManager.SetBaseColorWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetBaseColorRoughness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
	ObjectManager.SetBackscatterColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetBackscatterWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.05f, 0.05f, 0.05f, 1.0f));

	// Reflection
	ObjectManager.SetReflectionColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetReflectionWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
	ObjectManager.SetReflectionRoughness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
	ObjectManager.SetReflectionAnisotropy(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetIor(ObjectManager.GetActiveMaterialId(), 1.36f);
	ObjectManager.SetReflectionMode(ObjectManager.GetActiveMaterialId(), HorusMaterial::ReflectionTypePBR);
	ObjectManager.SetReflectionMetalness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

	// Sheen
	ObjectManager.SetSheenColor(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetSheenWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetSheenTint(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

	// Refraction
	ObjectManager.SetRefractionColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetRefractionWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetRefractionRoughness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetRefractionNormal(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetRefractionIor(ObjectManager.GetActiveMaterialId(), 1.36f);
	ObjectManager.SetRefractionAbsorptionColor(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetRefractionAbsorptionDistance(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetRefractionCaustics(ObjectManager.GetActiveMaterialId(), false);

	// SSS
	ObjectManager.SetSssScatterColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 0.8f, 0.8f, 1.0f));
	ObjectManager.SetSssScatterWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
	ObjectManager.SetSssScatterDirection(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetSssScatterDistance(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetSssUseMultiScattering(ObjectManager.GetActiveMaterialId(), true);
	ObjectManager.SetSssUseSchlickApproximation(ObjectManager.GetActiveMaterialId(), true);

	// Coating
	ObjectManager.SetCoatingColor(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetCoatingWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetCoatingNormalMap(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetCoatingNormalWeight(ObjectManager.GetActiveMaterialId(), 0.0f);
	ObjectManager.SetCoatingRoughness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetCoatingIor(ObjectManager.GetActiveMaterialId(), 1.36f);
	ObjectManager.SetCoatingThickness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetCoatingTransmissionColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetCoatingMode(ObjectManager.GetActiveMaterialId(), HorusMaterial::CoatingTypePBR);
	ObjectManager.SetCoatingMetalness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

	// Other
	ObjectManager.SetNormalWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetDisplacementMap(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetDisplacementWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetEmissive(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetEmissiveWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetOpacity(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)); // 0.0f = opaque, 1.0f = transparent

	PopulateSelectedMaterialInfos();
	CallResetBuffer();
}
void HorusInspector::SetMaterialMetal()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	// Set default material
	// Base Color
	ObjectManager.SetBaseColor(ObjectManager.GetActiveMaterialId(), glm::vec4(0.8f, 0.7f, 0.5f, 1.0f));
	ObjectManager.SetBaseColorWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetBaseColorRoughness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.6f, 0.5f, 0.4f, 1.0f));
	ObjectManager.SetBackscatterColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetBackscatterWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

	// Reflection
	ObjectManager.SetReflectionColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetReflectionWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetReflectionRoughness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.15f, 0.15f, 0.15f, 1.0f));
	ObjectManager.SetReflectionAnisotropy(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetIor(ObjectManager.GetActiveMaterialId(), 1.55f);
	ObjectManager.SetReflectionMode(ObjectManager.GetActiveMaterialId(), HorusMaterial::ReflectionTypeMetalness);
	ObjectManager.SetReflectionMetalness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.75f, 0.75f, 0.75f, 1.0f));

	// Sheen
	ObjectManager.SetSheenColor(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetSheenWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetSheenTint(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

	// Refraction
	ObjectManager.SetRefractionColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetRefractionWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetRefractionRoughness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetRefractionNormal(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetRefractionIor(ObjectManager.GetActiveMaterialId(), 1.36f);
	ObjectManager.SetRefractionAbsorptionColor(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetRefractionAbsorptionDistance(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetRefractionCaustics(ObjectManager.GetActiveMaterialId(), false);

	// SSS
	ObjectManager.SetSssScatterColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetSssScatterWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetSssScatterDirection(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetSssScatterDistance(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetSssUseMultiScattering(ObjectManager.GetActiveMaterialId(), false);
	ObjectManager.SetSssUseSchlickApproximation(ObjectManager.GetActiveMaterialId(), false);

	// Coating
	ObjectManager.SetCoatingColor(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetCoatingWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetCoatingNormalMap(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetCoatingNormalWeight(ObjectManager.GetActiveMaterialId(), 0.0f);
	ObjectManager.SetCoatingRoughness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetCoatingIor(ObjectManager.GetActiveMaterialId(), 1.36f);
	ObjectManager.SetCoatingThickness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetCoatingTransmissionColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetCoatingMode(ObjectManager.GetActiveMaterialId(), HorusMaterial::CoatingTypeMetalness);
	ObjectManager.SetCoatingMetalness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

	// Other
	ObjectManager.SetNormalWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetDisplacementMap(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetDisplacementWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetEmissive(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetEmissiveWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetOpacity(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)); // 0.0f = opaque, 1.0f = transparent

	PopulateSelectedMaterialInfos();
	CallResetBuffer();
}
void HorusInspector::SetMaterialGlass() // TODO : Assign correct value 
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	// Set default material
	// Base Color
	ObjectManager.SetBaseColor(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetBaseColorWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetBaseColorRoughness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetBackscatterColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetBackscatterWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

	// Reflection
	ObjectManager.SetReflectionColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetReflectionWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetReflectionRoughness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetReflectionAnisotropy(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetIor(ObjectManager.GetActiveMaterialId(), 1.36f);
	ObjectManager.SetReflectionMode(ObjectManager.GetActiveMaterialId(), HorusMaterial::ReflectionTypeMetalness);
	ObjectManager.SetReflectionMetalness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

	// Sheen
	ObjectManager.SetSheenColor(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetSheenWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetSheenTint(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

	// Refraction
	ObjectManager.SetRefractionColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetRefractionWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetRefractionRoughness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetRefractionNormal(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetRefractionIor(ObjectManager.GetActiveMaterialId(), 1.36f);
	ObjectManager.SetRefractionAbsorptionColor(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetRefractionAbsorptionDistance(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetRefractionCaustics(ObjectManager.GetActiveMaterialId(), false);

	// SSS
	ObjectManager.SetSssScatterColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetSssScatterWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetSssScatterDirection(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetSssScatterDistance(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetSssUseMultiScattering(ObjectManager.GetActiveMaterialId(), false);
	ObjectManager.SetSssUseSchlickApproximation(ObjectManager.GetActiveMaterialId(), false);

	// Coating
	ObjectManager.SetCoatingColor(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetCoatingWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetCoatingNormalMap(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetCoatingNormalWeight(ObjectManager.GetActiveMaterialId(), 0.0f);
	ObjectManager.SetCoatingRoughness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetCoatingIor(ObjectManager.GetActiveMaterialId(), 1.36f);
	ObjectManager.SetCoatingThickness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetCoatingTransmissionColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetCoatingMode(ObjectManager.GetActiveMaterialId(), HorusMaterial::CoatingTypeMetalness);
	ObjectManager.SetCoatingMetalness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

	// Other
	ObjectManager.SetNormalWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetDisplacementMap(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetDisplacementWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetEmissive(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetEmissiveWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetOpacity(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	PopulateSelectedMaterialInfos();
	CallResetBuffer();
}
void HorusInspector::SetMaterialEmissive()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	// Set default material
	// Base Color
	ObjectManager.SetBaseColor(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetBaseColorWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetBaseColorRoughness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetBackscatterColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetBackscatterWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

	// Reflection
	ObjectManager.SetReflectionColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetReflectionWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetReflectionRoughness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetReflectionAnisotropy(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetIor(ObjectManager.GetActiveMaterialId(), 1.36f);
	ObjectManager.SetReflectionMode(ObjectManager.GetActiveMaterialId(), HorusMaterial::ReflectionTypeMetalness);
	ObjectManager.SetReflectionMetalness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

	// Sheen
	ObjectManager.SetSheenColor(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetSheenWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetSheenTint(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

	// Refraction
	ObjectManager.SetRefractionColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetRefractionWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetRefractionRoughness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetRefractionNormal(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetRefractionIor(ObjectManager.GetActiveMaterialId(), 1.36f);
	ObjectManager.SetRefractionAbsorptionColor(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetRefractionAbsorptionDistance(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetRefractionCaustics(ObjectManager.GetActiveMaterialId(), false);

	// SSS
	ObjectManager.SetSssScatterColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetSssScatterWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetSssScatterDirection(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetSssScatterDistance(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetSssUseMultiScattering(ObjectManager.GetActiveMaterialId(), false);
	ObjectManager.SetSssUseSchlickApproximation(ObjectManager.GetActiveMaterialId(), false);

	// Coating
	ObjectManager.SetCoatingColor(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetCoatingWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetCoatingNormalMap(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetCoatingNormalWeight(ObjectManager.GetActiveMaterialId(), 0.0f);
	ObjectManager.SetCoatingRoughness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetCoatingIor(ObjectManager.GetActiveMaterialId(), 1.36f);
	ObjectManager.SetCoatingThickness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetCoatingTransmissionColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetCoatingMode(ObjectManager.GetActiveMaterialId(), HorusMaterial::CoatingTypeMetalness);
	ObjectManager.SetCoatingMetalness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

	// Other
	ObjectManager.SetNormalWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetDisplacementMap(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetDisplacementWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetEmissive(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetEmissiveWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetOpacity(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)); // 0.0f = opaque, 1.0f = transparent

	PopulateSelectedMaterialInfos();
	CallResetBuffer();
}
void HorusInspector::SetMaterialMatte()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	// Set default material
	// Base Color
	ObjectManager.SetBaseColor(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetBaseColorWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetBaseColorRoughness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetBackscatterColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetBackscatterWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

	// Reflection
	ObjectManager.SetReflectionColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetReflectionWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetReflectionRoughness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetReflectionAnisotropy(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetIor(ObjectManager.GetActiveMaterialId(), 1.36f);
	ObjectManager.SetReflectionMode(ObjectManager.GetActiveMaterialId(), HorusMaterial::ReflectionTypeMetalness);
	ObjectManager.SetReflectionMetalness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

	// Sheen
	ObjectManager.SetSheenColor(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetSheenWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetSheenTint(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

	// Refraction
	ObjectManager.SetRefractionColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetRefractionWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetRefractionRoughness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetRefractionNormal(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetRefractionIor(ObjectManager.GetActiveMaterialId(), 1.36f);
	ObjectManager.SetRefractionAbsorptionColor(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetRefractionAbsorptionDistance(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetRefractionCaustics(ObjectManager.GetActiveMaterialId(), false);

	// SSS
	ObjectManager.SetSssScatterColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetSssScatterWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetSssScatterDirection(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetSssScatterDistance(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetSssUseMultiScattering(ObjectManager.GetActiveMaterialId(), false);
	ObjectManager.SetSssUseSchlickApproximation(ObjectManager.GetActiveMaterialId(), false);

	// Coating
	ObjectManager.SetCoatingColor(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetCoatingWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetCoatingNormalMap(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetCoatingNormalWeight(ObjectManager.GetActiveMaterialId(), 0.0f);
	ObjectManager.SetCoatingRoughness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetCoatingIor(ObjectManager.GetActiveMaterialId(), 1.36f);
	ObjectManager.SetCoatingThickness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetCoatingTransmissionColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetCoatingMode(ObjectManager.GetActiveMaterialId(), HorusMaterial::CoatingTypeMetalness);
	ObjectManager.SetCoatingMetalness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

	// Other
	ObjectManager.SetNormalWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetDisplacementMap(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetDisplacementWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetEmissive(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetEmissiveWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetOpacity(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	PopulateSelectedMaterialInfos();
	CallResetBuffer();
}
void HorusInspector::SetMaterialSkin()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	// Set default material
	// Base Color
	ObjectManager.SetBaseColor(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetBaseColorWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetBaseColorRoughness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetBackscatterColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetBackscatterWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

	// Reflection
	ObjectManager.SetReflectionColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetReflectionWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetReflectionRoughness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetReflectionAnisotropy(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetIor(ObjectManager.GetActiveMaterialId(), 1.36f);
	ObjectManager.SetReflectionMode(ObjectManager.GetActiveMaterialId(), HorusMaterial::ReflectionTypeMetalness);
	ObjectManager.SetReflectionMetalness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

	// Sheen
	ObjectManager.SetSheenColor(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetSheenWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetSheenTint(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

	// Refraction
	ObjectManager.SetRefractionColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetRefractionWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetRefractionRoughness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetRefractionNormal(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetRefractionIor(ObjectManager.GetActiveMaterialId(), 1.36f);
	ObjectManager.SetRefractionAbsorptionColor(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetRefractionAbsorptionDistance(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetRefractionCaustics(ObjectManager.GetActiveMaterialId(), false);

	// SSS
	ObjectManager.SetSssScatterColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetSssScatterWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetSssScatterDirection(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetSssScatterDistance(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetSssUseMultiScattering(ObjectManager.GetActiveMaterialId(), false);
	ObjectManager.SetSssUseSchlickApproximation(ObjectManager.GetActiveMaterialId(), false);

	// Coating
	ObjectManager.SetCoatingColor(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetCoatingWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetCoatingNormalMap(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetCoatingNormalWeight(ObjectManager.GetActiveMaterialId(), 0.0f);
	ObjectManager.SetCoatingRoughness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetCoatingIor(ObjectManager.GetActiveMaterialId(), 1.36f);
	ObjectManager.SetCoatingThickness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetCoatingTransmissionColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetCoatingMode(ObjectManager.GetActiveMaterialId(), HorusMaterial::CoatingTypeMetalness);
	ObjectManager.SetCoatingMetalness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

	// Other
	ObjectManager.SetNormalWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetDisplacementMap(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetDisplacementWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetEmissive(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetEmissiveWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetOpacity(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	PopulateSelectedMaterialInfos();
	CallResetBuffer();
}
void HorusInspector::SetMaterialSSS()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	// Set default material
	// Base Color
	ObjectManager.SetBaseColor(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetBaseColorWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetBaseColorRoughness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetBackscatterColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetBackscatterWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

	// Reflection
	ObjectManager.SetReflectionColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetReflectionWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetReflectionRoughness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetReflectionAnisotropy(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetIor(ObjectManager.GetActiveMaterialId(), 1.36f);
	ObjectManager.SetReflectionMode(ObjectManager.GetActiveMaterialId(), HorusMaterial::ReflectionTypeMetalness);
	ObjectManager.SetReflectionMetalness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

	// Sheen
	ObjectManager.SetSheenColor(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetSheenWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetSheenTint(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

	// Refraction
	ObjectManager.SetRefractionColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetRefractionWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetRefractionRoughness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetRefractionNormal(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetRefractionIor(ObjectManager.GetActiveMaterialId(), 1.36f);
	ObjectManager.SetRefractionAbsorptionColor(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetRefractionAbsorptionDistance(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetRefractionCaustics(ObjectManager.GetActiveMaterialId(), false);

	// SSS
	ObjectManager.SetSssScatterColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetSssScatterWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetSssScatterDirection(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetSssScatterDistance(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetSssUseMultiScattering(ObjectManager.GetActiveMaterialId(), false);
	ObjectManager.SetSssUseSchlickApproximation(ObjectManager.GetActiveMaterialId(), false);

	// Coating
	ObjectManager.SetCoatingColor(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetCoatingWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetCoatingNormalMap(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetCoatingNormalWeight(ObjectManager.GetActiveMaterialId(), 0.0f);
	ObjectManager.SetCoatingRoughness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetCoatingIor(ObjectManager.GetActiveMaterialId(), 1.36f);
	ObjectManager.SetCoatingThickness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetCoatingTransmissionColor(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetCoatingMode(ObjectManager.GetActiveMaterialId(), HorusMaterial::CoatingTypeMetalness);
	ObjectManager.SetCoatingMetalness(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

	// Other
	ObjectManager.SetNormalWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetDisplacementMap(ObjectManager.GetActiveMaterialId(), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
	ObjectManager.SetDisplacementWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetEmissive(ObjectManager.GetActiveMaterialId(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ObjectManager.SetEmissiveWeight(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	ObjectManager.SetOpacity(ObjectManager.GetActiveMaterialId(), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

	PopulateSelectedMaterialInfos();
	CallResetBuffer();
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

	// Render info moved to Statistic UI

	// Render Modes
	{
		ImGui::Text("Select render mode :");
		ImGui::SameLine();
		if (ImGui::Combo("Render mode", &m_SelectedRenderMode_, m_RenderModes_, IM_ARRAYSIZE(m_RenderModes_)))
		{
			Radeon.SetVisualizationRenderMode(m_SelectedRenderMode_);
			CallResetBuffer();
		}ShowHandCursorOnHover("Select render mode");
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
		}ShowHandCursorOnHover("Select AOV can be used to visualize different render passes");
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
		}ShowHandCursorOnHover();
		ImGui::SameLine();
		// Clear Path button
		if (ImGui::Button("Clear##path"))
		{
			m_BackgroundPath_.clear();
		} ShowHandCursorOnHover();

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
		}ShowHandCursorOnHover();
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
		}ShowHandCursorOnHover();

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
						} ShowResizeHorizontalCursorOnHover();

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
						}ShowResizeHorizontalCursorOnHover();

						ShowBigSeparator();

						// Russian roulette
						// Russian roulette
						if (ImGui::Checkbox("Russian Roulette", &m_EnableRussianRoulette_))
						{
							m_ResetBuffer_ = m_EnableRussianRoulette_ != m_PreviousEnableRussianRoulette_;
							m_PreviousEnableRussianRoulette_ = m_EnableRussianRoulette_;
						}ShowHandCursorOnHover("Russian roulette is a technique used to reduce the number of rays that are traced in a path tracer. It is used to reduce the computational cost of rendering by terminating rays that have a low contribution to the final image.");

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
						}ShowHandCursorOnHover("Russian roulette depth, the higher the value, the more likely the ray will be terminated");

						ShowBigSeparator();

						// TODO : Fix Adaptative sampling, (problem with allocating m_FbData_ in Radeon.cpp)
						ImGui::BeginDisabled(true);
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
						} ShowHandCursorOnHover("Adaptive sampling is a technique used to reduce the number of samples in areas of the image that are less important. It is used to reduce the computational cost of rendering by terminating rays that have a low contribution to the final image.");

						ImVec4 Color = m_EnableAdaptiveSampling_ ? ImVec4(0.6f, 1.0f, 0.6f, 1.0f) : ImVec4(1.0f, 0.6f, 0.7f, 1.0f);
						ImGui::TextColored(Color, m_EnableAdaptiveSampling_ ? "Adaptive sampling is enabled" : "Adaptive sampling is disabled");

						// Display and handle adaptive threshold slider
						ImGui::Text("Threshold: %.2f", m_AdaptiveThreshold_);
						ImGui::SameLine();
						if (ImGui::SliderFloat("Threshold", &m_AdaptiveThreshold_, 0.01f, 0.5f, "%.1f"))
						{
							Radeon.SetAdaptiveSamplingThreshold(m_AdaptiveThreshold_);
							CallResetBuffer();
						} ShowResizeHorizontalCursorOnHover("Adaptive sampling threshold, the lower the value, the more likely the adaptive sampling will be used");
						ImGui::EndDisabled();
					}

					ShowBigSeparator();

					// checkbox for enable preview mode
					// Handle the enable preview checkbox and update preview mode settings
					if (bool CheckboxChanged = ImGui::Checkbox("Enable preview", &m_EnablePreviewMode_); CheckboxChanged && (m_EnablePreviewMode_ != m_PreviousResetMode_)) {
						m_ResetBuffer_ = true;
						m_PreviousResetMode_ = m_EnablePreviewMode_;
						Radeon.SetLockPreviewMode(m_EnablePreviewMode_);
						PopulateSelectedProjectInfos();
					} ShowHandCursorOnHover("Enable preview mode is used for faster rendering, it is useful for previewing the scene before the final render.");

					// Display preview mode status
					ImVec4 colorPM = m_EnablePreviewMode_ ? ImVec4(0.6f, 1.0f, 0.6f, 1.0f) : ImVec4(1.0f, 0.6f, 0.7f, 1.0f);
					ImGui::TextColored(colorPM, m_EnablePreviewMode_ ? "Preview mode is enabled" : "Preview mode is disabled");

					ShowBigSeparator();

					// Anti Aliasing
					bool CheckboxAaChanged = ImGui::Checkbox("Enable AA", &m_EnableAa_); ShowHandCursorOnHover("Enable anti-aliasing is used to reduce the jagged edges of the rendered image");
					if (CheckboxAaChanged) {
						m_ResetBuffer_ = true;
						m_PreviousEnableAa_ = m_EnableAa_;
						rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_AA_ENABLED, m_EnableAa_ ? 1 : 0);
					}

					ImVec4 colorAA = m_EnableAa_ ? ImVec4(0.6f, 1.0f, 0.6f, 1.0f) : ImVec4(1.0f, 0.6f, 0.7f, 1.0f);
					ImGui::TextColored(colorAA, m_EnableAa_ ? "Anti aliasing is enabled" : "Anti aliasing is disabled");

					ImGui::Separator();

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
					} ShowHandCursorOnHover("Select filter can be used to change the filter used for anti-aliasing");

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
						} ShowResizeHorizontalCursorOnHover("Diffuse ray depth, the higher the value, the more likely the diffuse rays will be traced per pixel at each bounce");

						static int maxDepthGlossy = 1;
						if (ImGui::SliderInt("Reflection Ray Depth", &maxDepthGlossy, 1, 64))
						{
							rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_MAX_DEPTH_GLOSSY, maxDepthGlossy);
							CallResetBuffer();
						} ShowResizeHorizontalCursorOnHover("Reflection ray depth, the higher the value, the more likely the reflection rays will be traced per pixel at each bounce");

						static int maxDepthRefraction = 1;
						if (ImGui::SliderInt("Refraction Ray Depth", &maxDepthRefraction, 1, 64))
						{
							rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_MAX_DEPTH_REFRACTION, maxDepthRefraction);
							CallResetBuffer();
						} ShowResizeHorizontalCursorOnHover("Refraction ray depth, the higher the value, the more likely the refraction rays will be traced per pixel at each bounce");

						static int maxDepthGlossyRefraction = 1;
						if (ImGui::SliderInt("Glossy Refraction Ray Depth", &maxDepthGlossyRefraction, 1, 64))
						{
							rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_MAX_DEPTH_GLOSSY_REFRACTION, maxDepthGlossyRefraction);
							CallResetBuffer();
						} ShowResizeHorizontalCursorOnHover("Glossy refraction ray depth, the higher the value, the more likely the glossy refraction rays will be traced per pixel at each bounce");

						static int maxDepthShadow = 1;
						if (ImGui::SliderInt("Shadow Ray Depth", &maxDepthShadow, 1, 64))
						{
							rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_MAX_DEPTH_SHADOW, maxDepthShadow);
							CallResetBuffer();
						} ShowResizeHorizontalCursorOnHover("Shadow ray depth, the higher the value, the more likely the shadow rays will be traced per pixel at each bounce");
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
						} ShowResizeHorizontalCursorOnHover("Trace depth, the higher the value, make the rays to be traced more times and produce more accurate results (but slower), it's useful for deep reflections and refractions");
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

	static char SavePath[256] = "Path\\To\\Your\\SaveFolder";
	static char FileName[256] = "Render.png";
	if (ImGui::CollapsingHeader("Export render", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Button("Browse##SavePath"))
		{
			std::string FolderPath = Utils::HorusFileDialog::OpenFolder();
			if (!FolderPath.empty())
			{
				strncpy_s(SavePath, FolderPath.c_str(), sizeof(SavePath) - 1);
				spdlog::info("Save path selected: {}", FolderPath);
			}
		}
		ImGui::SameLine();
		ImGui::InputText("Save Path", SavePath, sizeof(SavePath));

		ImGui::InputText("Filename", FileName, sizeof(FileName));

		// Checkbox emergency render
		static bool m_EnableEmergencyRender_ = true;
		ImGui::Checkbox("E", &m_EnableEmergencyRender_);

		ImGui::SameLine();
		static bool m_Overwrite_ = true;
		ImGui::Checkbox("O", &m_Overwrite_);
		
		ImGui::SameLine();
		if (ImGui::Button("Export"))
		{
			std::filesystem::path FullFilePath = std::filesystem::path(SavePath) / FileName;

			if (std::filesystem::exists(FullFilePath))
			{
				spdlog::warn("File already exists: {}", FullFilePath.string());
			}

			if (ExportToFile(FullFilePath.string().c_str(), m_Overwrite_, m_EnableEmergencyRender_))
			{
				spdlog::info("Render exported with success: {}", FullFilePath.string());
			}
			else
			{
				spdlog::error("Failed to save frame buffer to file: {}", FullFilePath.string());
			}
		}
		ShowHandCursorOnHover();
		ImGui::SameLine(); ShowHelpMarker("E = Emergency render, if the render fails to save with Radeon ProRender, it will try to save with stb_image\nO = Overwrite, if the file already exists, it will overwrite it");
	}

	ShowBigSeparator();
}

bool HorusInspector::ExportToFile(const char* filepath, bool Overwrite = false, bool EmergencyRender = true)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();

	spdlog::info("Exporting frame buffer to file: {}", filepath);

	if (Radeon.GetFrameBufferResolved() == nullptr)
	{
		spdlog::error("Frame buffer is null");
		return false;
	}

	if (std::filesystem::exists(filepath) && !Overwrite)
	{
		spdlog::warn("File already exists: {}", filepath);
	}

	if (std::filesystem::exists(filepath) && Overwrite)
	{
		spdlog::warn("Overwriting file: {}", filepath);
	}

	if (rprFrameBufferSaveToFile(Radeon.GetFrameBufferResolved(), filepath) != RPR_SUCCESS)
	{
		spdlog::error("Failed to save frame buffer to file: {}", filepath);

		// Trying to save with a different method (stb_image)

		if (stbi_write_png(filepath, Radeon.GetFramebufferDestSizeX(), Radeon.GetFramebufferDestSizeY(), 4, Radeon.GetFbData(), 0) == 0 && EmergencyRender)
		{
			spdlog::critical("Emergency render failed to save frame buffer to file with stb_image: {}", filepath);
			return false;
		}
		else
		{
			spdlog::info("Emergency render exported with success: {}", filepath);
			return true;
		}






		return false;
	}



	return true;
}

void HorusInspector::InspectorNone()
{
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f)); // Vert clair pour le titre
	ImGui::Text("Inspector");
	ImGui::PopStyleColor();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 0.8f));
	ImGui::TextWrapped("No object is currently selected for inspection. Please select an object from the Outliner or Scene to view its properties and perform actions.");
	ImGui::PopStyleColor();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();


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

	m_MinSamples_ = Radeon.GetMinSamples();
	m_MaxSamples_ = Radeon.GetMaxSamples();
	m_SelectedRenderMode_ = Radeon.GetVisualizationRenderMode();
}