#include "hrs_viewport.h"
#include <hrs_object_manager.h>

#include "hrs_inspector.h"
#include "hrs_om_camera.h"
using namespace std;

//---------------------------------------OPENGL SECTION---------------------------------------//
void HorusViewportInput::ProcessInputOpenGl()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusOmCamera& Camera = HorusOmCamera::GetInstance();
	ImGuiIO& Io = ImGui::GetIO();

	bool IsCameraMove = false;

	if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_F)))
	{
		IsCameraMove = true;
		glm::vec3 Pivot = { 0.f, 0.f, 0.f };
		Camera.SetCameraLookAt(Camera.GetActiveCameraID(), Pivot);
	}

	if (Io.KeyAlt)
	{
		if (ImGui::IsMouseDown(0)) // left mouse
		{
			IsCameraMove = true;
			ImVec2 WindowSize = ImGui::GetWindowSize();
			ImVec2 MouseDelta = ImGui::GetMouseDragDelta(0, 0.0f);

			MouseDelta.x /= WindowSize.x;
			MouseDelta.y /= WindowSize.y;

			Camera.SetOrbitalCamera(Camera.GetActiveCameraID(), MouseDelta.x, MouseDelta.y, 4);

			ImGui::ResetMouseDragDelta(0);
		}

		if (ImGui::IsMouseDown(1)) // right mouse
		{
			IsCameraMove = true;
			ImVec2 MouseDelta = ImGui::GetMouseDragDelta(1, 0);

			Camera.SetZoomCamera(Camera.GetActiveCameraID(), 0.1f * MouseDelta.y);

			ImGui::ResetMouseDragDelta(1);
		}

		if (ImGui::IsMouseDown(2)) // middle mouse
		{
			IsCameraMove = true;
			ImVec2 MouseDelta = ImGui::GetMouseDragDelta(2, 0);

			Camera.SetPanCamera(Camera.GetActiveCameraID(), MouseDelta.x, MouseDelta.y, 0.01f);

			ImGui::ResetMouseDragDelta(2);
		}

		if (Io.MouseWheel != 0)
		{
			IsCameraMove = true;
			float ScrollDelta = Io.MouseWheel;
			Camera.SetScrollCamera(Camera.GetActiveCameraID(), ScrollDelta);
		}
	}

	if (IsCameraMove)
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_None);
		HorusInspector::GetInstance().CallSetFocusPlaneToFocusPosition();
		HorusInspector::GetInstance().PopulateSelectedCameraInfos();
		HorusResetBuffers::GetInstance().CallResetBuffers();
	}
	else
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
	}
}

//---------------------------------------RADEON SECTION---------------------------------------//
void HorusViewportInput::ProcessInputRadeon()
{
	HorusOmCamera& Camera = HorusOmCamera::GetInstance();
	ImGuiIO& Io = ImGui::GetIO();

	bool IsCameraMove = false;

	if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_F)))
	{
		IsCameraMove = true;
		glm::vec3 Pivot = { 0.f, 0.f, 0.f };
		Camera.SetCameraLookAt(Camera.GetActiveCameraID(), Pivot);
	}

	if (Io.KeyAlt)
	{
		if (ImGui::IsMouseDown(0)) // left mouse
		{
			IsCameraMove = true;
			ImVec2 WindowSize = ImGui::GetWindowSize();
			ImVec2 MouseDelta = ImGui::GetMouseDragDelta(0, 0.0f);

			MouseDelta.x /= WindowSize.x;
			MouseDelta.y /= WindowSize.y;

			Camera.SetOrbitalCamera(Camera.GetActiveCameraID(), MouseDelta.x, MouseDelta.y, 4);

			ImGui::ResetMouseDragDelta(0);
		}

		if (ImGui::IsMouseDown(1)) // right mouse
		{
			IsCameraMove = true;
			ImVec2 MouseDelta = ImGui::GetMouseDragDelta(1, 0);

			Camera.SetZoomCamera(Camera.GetActiveCameraID(), 0.1f * MouseDelta.y);

			ImGui::ResetMouseDragDelta(1);
		}

		if (ImGui::IsMouseDown(2)) // middle mouse
		{
			IsCameraMove = true;
			ImVec2 MouseDelta = ImGui::GetMouseDragDelta(2, 0);

			Camera.SetPanCamera(Camera.GetActiveCameraID(), MouseDelta.x, MouseDelta.y, 0.01f);

			ImGui::ResetMouseDragDelta(2);
		}

		if (Io.MouseWheel != 0)
		{
			IsCameraMove = true;
			float ScrollDelta = Io.MouseWheel;
			Camera.SetScrollCamera(Camera.GetActiveCameraID(), ScrollDelta);
		}
	}

	if (IsCameraMove)
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_None);
		HorusInspector::GetInstance().CallSetFocusPlaneToFocusPosition();
		HorusInspector::GetInstance().PopulateSelectedCameraInfos();
		HorusResetBuffers::GetInstance().CallResetBuffers();
	}
	else{
		ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
	}
}

