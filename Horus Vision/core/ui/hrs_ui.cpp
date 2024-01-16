
// Project includes
#include "hrs_window.h" // glad.h
#include "hrs_viewport.h" // glad.h
#include "hrs_engine.h" // glad.h
#include "hrs_reset_buffers.h" // nothing
#include "hrs_ui.h" // nothing
#include "hrs_object_manager.h" // glfw3.h
#include "hrs_radeon.h" // glfw3.h
#include "hrs_outliner.h"
#include "hrs_inspector.h"
#include "hrs_console.h"

#include "hrs_imgui.h"

#define STB_IMAGE_IMPLEMENTATION
#include "imgui_internal.h"
#include "stb_image.h"

// TODO : Move this to HorusUI


bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
{
	int ImageWidth = 0;
	int ImageHeight = 0;
	unsigned char* ImageData = stbi_load(filename, &ImageWidth, &ImageHeight, nullptr, 4);
	if (ImageData == nullptr)
		return false;

	GLuint ImageTexture;
	glGenTextures(1, &ImageTexture);
	glBindTexture(GL_TEXTURE_2D, ImageTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 

#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ImageWidth, ImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, ImageData);
	stbi_image_free(ImageData);

	*out_texture = ImageTexture;
	*out_width = ImageWidth;
	*out_height = ImageHeight;

	return true;
}

void reset_buffer()
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();

	HorusUI::GetInstance().SetOptionsChanged(true);
	Radeon.SetIsDirty(true);
	Radeon.SetSampleCount(1);
	CHECK(rprFrameBufferClear(Radeon.GetFrameBuffer()));
}

void HorusUI::Init()
{
	HorusConsole::GetInstance().InitConsole();
}

void HorusUI::MainMenuBar()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	if (ImGui::BeginMainMenuBar())
	{
		HorusImGui& ImGui = HorusImGui::GetInstance();

		/*ImGui::PushFont(ImGui.get_icon_font());
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f));
		ImGui::Text(ICON_FA_CANNABIS);
		ImGui::PopStyleColor();
		ImGui::PopFont();*/

		if (m_LoadLogoTexture_ == true)
		{
			bool Ret = LoadTextureFromFile("resources/Icons/Horus_Logo_100x20.png", &m_LogoTexture_, &m_LogoWidth_, &m_LogoHeight_);
			IM_ASSERT(Ret);
			spdlog::info("Horus logo loaded.");
			m_LoadLogoTexture_ = false;
		}

		ImGui::Image((void*)intptr_t(m_LogoTexture_), ImVec2(m_LogoWidth_, m_LogoHeight_));

		/*ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "Horus Vision");*/

		ImGui::SameLine();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
		ImGui::SameLine();

		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New"))
			{
				ObjectManager.DestroyScene(ObjectManager.GetActiveSceneId());
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
				HorusEngine::GetInstance().SetIsClosing(true);
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

			ImGui::MenuItem("Viewer", nullptr, &m_ShowOpenGLViewport_);
			ImGui::MenuItem("Render", nullptr, &m_ShowRadeonViewport_);
			ImGui::MenuItem("Outliner", nullptr, &m_ShowOutliner_);
			ImGui::MenuItem("Console", nullptr, &m_ShowConsole_);
			ImGui::MenuItem("Scene", nullptr, &m_ShowScene_);
			ImGui::MenuItem("Inspector", nullptr, &m_ShowInspector_);

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("About"))
			{

			}

			ImGui::EndMenu();
		}

		float WindowWidth = ImGui::GetWindowWidth();
		float TextWidth = ImGui::CalcTextSize("Version 1.0.0").x;
		ImGui::SetCursorPosX(WindowWidth - TextWidth - ImGui::GetStyle().ItemSpacing.x);
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
		HorusViewportOpenGL::GetInstance().ViewportOpenGL(&m_ShowOpenGLViewport_);
	}

	if (m_ShowRadeonViewport_)
	{
		HorusViewportRadeon::GetInstance().ViewportRadeon(&m_ShowRadeonViewport_);
	}

	if (m_ShowInspector_)
	{
		HorusInspector::GetInstance().Inspector(&m_ShowInspector_);
	}
	
	if (m_ShowOutliner_)
	{
		HorusOutliner::GetInstance().Outliner(&m_ShowOutliner_);
	}

	if (m_ShowConsole_)
	{
		HorusConsole::GetInstance().Console(&m_ShowConsole_);
	}

	// Create and load logo 
	/*if (m_LoadUILogoTexture_ == true)
	{
		bool ret = LoadTextureFromFile("resources/Icons/Horus_Logo_100x41.png", &m_UILogoTexture_, &m_UILogoWidth_, &m_UILogoHeight_);
		IM_ASSERT(ret);
		spdlog::info("Horus UI logo loaded.");
		m_LoadUILogoTexture_ = false;
	}

	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 windowSize = ImGui::GetWindowSize();
	ImVec2 logoSize(m_UILogoWidth_, m_UILogoHeight_);
	ImVec2 logoPosition = ImVec2(windowPos.x + windowSize.x - logoSize.x - 10, windowPos.y + windowSize.y - logoSize.y - 10);

	ImGui::SetCursorScreenPos(logoPosition);
	ImGui::Image((void*)(intptr_t)m_UILogoTexture_, ImVec2(m_UILogoWidth_, m_UILogoHeight_));*/
}

void HorusUI::ResetBuffers()
{
	HorusResetBuffers::GetInstance().CallResetBuffers();
}
