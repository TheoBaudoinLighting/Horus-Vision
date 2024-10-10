#include "hrs_opengl_viewport.h"

#include "hrs_viewport.h"
#include <hrs_object_manager.h>

#include "hrs_horus_parameters.h"
#include "hrs_om_camera.h"
#include "hrs_utils.h"
#include "ImGuizmo.h"
#include "imgui_internal.h"
using namespace std;

void HorusViewportOpenGL::ViewportOpenGL(bool* p_open)
{
	HorusOpenGL& OpenGL = HorusOpenGL::GetInstance();

	ImGuiIO& Io = ImGui::GetIO();
	const bool IsLeftAltPressed = Io.KeyAlt;

	ImGuiWindowFlags WindowFlags = 0;

	if (IsLeftAltPressed) {
		WindowFlags |= ImGuiWindowFlags_NoMove;
	}

	if (ImGui::Begin(HORUS_UI_NAME_OPENGLVIEWPORT.c_str(), p_open, ImGuiWindowFlags_MenuBar | WindowFlags))
	{
		if (ImGui::BeginMenuBar())
		{
			/*if (ImGui::BeginCombo("##Options de Zone de Rendu", nullptr, ImGuiComboFlags_NoArrowButton))
			{
				static float Opacity = 0.5f;

				ImGui::SliderFloat("Opacity", &Opacity, 0.0f, 1.0f, "%.1f");

				ImGui::EndCombo();
			}*/
			ImGui::EndMenuBar();
		}

		const float ViewportWidth = ImGui::GetContentRegionAvail().x;
		const float ViewportHeight = ImGui::GetContentRegionAvail().y;
		static float LastViewportWidth = ViewportWidth;
		static float LastViewportHeight = ViewportHeight;

		if (m_IsFirstLaunch_)
		{
			OpenGL.ResizeOpenGLViewportFrameBuffer(ViewportWidth, ViewportHeight);
			LastViewportWidth = ViewportWidth;
			LastViewportHeight = ViewportHeight;
		}

		if (ViewportWidth != LastViewportWidth || ViewportHeight != LastViewportHeight)
		{
			OpenGL.ResizeOpenGLViewportFrameBuffer(ViewportWidth, ViewportHeight);
			LastViewportWidth = ViewportWidth;
			LastViewportHeight = ViewportHeight;
		}

		ImVec2 CursorPos = ImGui::GetCursorScreenPos();
		GLuint TextureID = OpenGL.GetOpenGlTextureBuffer();

		ImGui::GetWindowDrawList()->AddImage(
			reinterpret_cast<void*>(TextureID),
			ImVec2(CursorPos.x, CursorPos.y),
			ImVec2(CursorPos.x + ViewportWidth, CursorPos.y + ViewportHeight),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);

		const float Padding = 1.0f;
		const float RectThickness = 1.0f;
		ImU32 GreenColor = ImColor(0.6f, 1.0f, 0.6f, 1.0f);

		ImGui::GetWindowDrawList()->AddRect(
			ImVec2(CursorPos.x - Padding, CursorPos.y - Padding),
			ImVec2(CursorPos.x + ViewportWidth + Padding, CursorPos.y + ViewportHeight + Padding),
			GreenColor,
			0.0f,
			0,
			RectThickness
		);

		// SAVEME : Alternative way to draw the image and the rectangle around it
		/*ImGui::Image((void*)TextureID, ImVec2(ViewportWidth, ViewportHeight), ImVec2(0, 1), ImVec2(1, 0));
		DrawRectangleAroundItem(m_GreenColor_);*/
	}

	// Show View Manipulate
	{
		HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
		HorusOmCamera& CameraManager = HorusOmCamera::GetInstance();

		auto& Camera = CameraManager.GetOpenGLCamera(CameraManager.GetActiveOpenGLCameraID());
		static ImGuiWindowFlags GizmoWindowFlags = 0;

		ImGuizmo::SetDrawlist();
		float WindowWidth = (float)ImGui::GetWindowWidth();
		float WindowHeight = (float)ImGui::GetWindowHeight();
		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, WindowWidth, WindowHeight);
		float ViewManipulateRight = ImGui::GetWindowPos().x + WindowWidth;
		float ViewManipulateTop = ImGui::GetCursorScreenPos().y;
		ImGuiWindow* Window = ImGui::GetCurrentWindow();
		GizmoWindowFlags = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(Window->InnerRect.Min, Window->InnerRect.Max) ? ImGuiWindowFlags_NoMove : 0;

		glm::mat4 ViewMatrix = Camera.GetViewMatrix();
		glm::mat4 ProjectionMatrix = Camera.GetProjectionMatrix();

		// Update the view matrix after manipulation
		ImGuizmo::ViewManipulate(&ViewMatrix[0][0], 8, ImVec2(ViewManipulateRight - 128, ViewManipulateTop), ImVec2(128, 128), 0x10101010);

		// Set the updated view matrix back to the camera
		Camera.SetViewMatrix(ViewMatrix);
	}

	// Here we process the input for the viewport (need to be set between the ImGui::Begin and the ImGui::End for get the focus)
	if (ImGui::IsWindowFocused())
	{
		HorusViewportInput::GetInstance().ProcessInputOpenGl();
	}

	//----------------------------- BOTTOM BAR -----------------------------------------

	{
		constexpr int BottomMenu = 30;
		ImGui::SetCursorPosY(ImGui::GetWindowHeight() - BottomMenu);
		ImGui::Separator();
		ImGui::BeginGroup();

		float WindowWidth = ImGui::GetWindowWidth();
		float TextWidth = ImGui::CalcTextSize("Viewer 5.1.1    ").x; // For spacing purpose
		ImGui::SetCursorPosX(WindowWidth - TextWidth - ImGui::GetStyle().ItemSpacing.x);
		ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), HORUS_APP_VERSION_VIEWER.c_str());

		ImGui::EndGroup();
	}

	if (m_IsFirstLaunch_)
		m_IsFirstLaunch_ = false;

	ImGui::End();
}

void HorusViewportOpenGL::ResetBuffers()
{
	HorusResetBuffers::GetInstance().CallResetBuffers();
}
