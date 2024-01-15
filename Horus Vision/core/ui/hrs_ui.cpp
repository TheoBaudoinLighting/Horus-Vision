
// Project includes
#include "hrs_window.h" // glad.h
#include "hrs_viewport.h" // glad.h
#include "hrs_engine.h" // glad.h
#include "hrs_material_editor.h" // nothing
#include "hrs_reset_buffers.h" // nothing
#include "hrs_ui.h" // nothing
#include "hrs_object_manager.h" // glfw3.h
#include "hrs_radeon.h" // glfw3.h
#include "hrs_outliner.h"
#include "hrs_inspector.h"
#include "hrs_console.h"

void reset_buffer()
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	HorusUI::get_instance().SetOptionsChanged(true);
	Radeon.set_is_dirty(true);
	Radeon.set_sample_count(1);
	CHECK(rprFrameBufferClear(Radeon.get_frame_buffer()));
}

void HorusUI::Init()
{
	HorusConsole::get_instance().InitConsole();

	

	/*if (m_ShowOpenGLViewport_) ViewportOpenGL(&m_ShowOpenGLViewport_);
	if (m_ShowRadeonViewport_) ViewportRadeon(&m_ShowRadeonViewport_);
	if (m_ShowOutliner_) Outliner(&m_ShowOutliner_);
	if (m_ShowConsole_) Console(&m_ShowConsole_);
	if (m_ShowScene_) Scene(&m_ShowScene_);
	if (m_ShowInspector_) Inspector(&m_ShowInspector_);*/
}

void HorusUI::MainMenuBar()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::get_instance();

	if (ImGui::BeginMainMenuBar())
	{
		ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "Horus Vision");

		ImGui::SameLine();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
		ImGui::SameLine();

		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New"))
			{
				ObjectManager.destroy_scene(ObjectManager.GetActiveSceneId());
			}

			if (ImGui::MenuItem("Open"))
			{

			}

			if (ImGui::MenuItem("Save scene"))
			{

			}

			if (ImGui::MenuItem("Save scene as.."))
			{

			}

			if (ImGui::MenuItem("Quit"))
			{
				HorusEngine::get_instance().set_is_closing(true);
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Cancel"))
			{

			}

			if (ImGui::MenuItem("Revert back"))
			{

			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Show"))
		{
			if (ImGui::MenuItem("Zoom +"))
			{

			}

			if (ImGui::MenuItem("Zoom -"))
			{

			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{
			ImGui::SeparatorText("Show / Hide");

			ImGui::MenuItem("Viewer", NULL, &m_ShowOpenGLViewport_);
			ImGui::MenuItem("Render", NULL, &m_ShowRadeonViewport_);
			ImGui::MenuItem("Outliner", NULL, &m_ShowOutliner_);
			ImGui::MenuItem("Console", NULL, &m_ShowConsole_);
			ImGui::MenuItem("Scene", NULL, &m_ShowScene_);
			ImGui::MenuItem("Inspector", NULL, &m_ShowInspector_);

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("About"))
			{

			}

			ImGui::EndMenu();
		}

		float windowWidth = ImGui::GetWindowWidth();
		float textWidth = ImGui::CalcTextSize("Version 1.0.0").x;
		ImGui::SetCursorPosX(windowWidth - textWidth - ImGui::GetStyle().ItemSpacing.x);
		ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "Version 1.0.0");

		ImGui::EndMainMenuBar();
	}
}

void HorusUI::RenderUI()
{
	//ImGui::ShowDemoWindow();

	MainMenuBar();

	if (m_ShowOpenGLViewport_)
	{
		HorusViewportOpenGL::get_instance().ViewportOpenGL(&m_ShowOpenGLViewport_);
	}

	if (m_ShowRadeonViewport_)
	{
		HorusViewportRadeon::get_instance().ViewportRadeon(&m_ShowRadeonViewport_);
	}

	if (m_ShowInspector_)
	{
		HorusInspector::get_instance().Inspector(&m_ShowInspector_);
	}
	
	if (m_ShowOutliner_)
	{
		HorusOutliner::get_instance().Outliner(&m_ShowOutliner_);
	}

	if (m_ShowConsole_)
	{
		HorusConsole::get_instance().Console(&m_ShowConsole_);
	}
}

void HorusUI::ResetBuffers()
{
	HorusResetBuffers::get_instance().CallResetBuffers();
}
