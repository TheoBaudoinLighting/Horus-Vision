
// Project includes
#include "hrs_window.h" // glad.h
#include "hrs_viewport.h" // glad.h
#include "hrs_engine.h" // glad.h
#include "hrs_platform.h" // glad.h
#include "hrs_reset_buffers.h" // nothing
#include "hrs_ui.h" // nothing
#include "hrs_object_manager.h" // glfw3.h
#include "hrs_radeon.h" // glfw3.h
#include "hrs_outliner.h"
#include "hrs_inspector.h"
#include "hrs_console.h"

#include "hrs_imgui.h"

#define STB_IMAGE_IMPLEMENTATION

#include "../scene/hrs_importer_manager.h"
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

void Reset_Buffer()
{
	HorusResetBuffers::GetInstance().CallResetBuffers();
}

void HorusUI::Init()
{
	HorusConsole::GetInstance().InitConsole();
}

void HorusUI::MainMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (m_LoadLogoTexture_ == true)
		{
			bool Ret = LoadTextureFromFile("resources/Icons/Horus_Logo_100x20.png", &m_LogoTexture_, &m_LogoWidth_, &m_LogoHeight_);
			IM_ASSERT(Ret);
			spdlog::info("Horus logo loaded.");
			m_LoadLogoTexture_ = false;
		}

		ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(m_LogoTexture_)), ImVec2(m_LogoWidth_, m_LogoHeight_));

		ImGui::SameLine();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
		ImGui::SameLine();

		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New", "Ctrl+N"))
			{
				//ObjectManager.DestroyScene(ObjectManager.GetActiveSceneId());
			}

			if (ImGui::MenuItem("Open scene..", "Ctrl+O"))
			{
				if (std::string FilePath = Utils::HorusFileDialog::OpenFile("Horus Scene (*.hrs)\0*.hrs\0"); !FilePath.empty())
				{
					spdlog::info("Open file : {}", FilePath);
				}
			}

			if (ImGui::MenuItem("Save scene", "Ctrl+S"))
			{
				if (std::string FilePath = Utils::HorusFileDialog::SaveFile("Horus Scene (*.hrs)\0*.hrs\0"); !FilePath.empty())
				{
					spdlog::info("Save file : {}.hrs", FilePath);
				}
			}

			if (ImGui::MenuItem("Save scene as..", "Ctrl+Alt+S"))
			{

			}

			if (ImGui::MenuItem("Load test scene", "Ctrl+L"))
			{
				HorusObjectManager::GetInstance().ShowLookdevScene();
				//HorusObjectManager::GetInstance().ShowJaguardXKSS();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Import mesh.."))
			{
				std::string FilePath = Utils::HorusFileDialog::OpenFile("3D Files (*.fbx;*.obj;*.dae;*.3ds;*.blend;*.stl;*.gltf;*.ply;*.max;*.c4d)\0*.fbx;*.obj;*.dae;*.3ds;*.blend;*.stl;*.gltf;*.ply;*.max;*.c4d\0");

				if (!FilePath.empty())
				{
					// Check if the file is already valid with non UTF-8 characters
					const std::string MeshName = std::filesystem::path(FilePath).stem().string();

					HorusObjectManager::GetInstance().CreateGroupShape(FilePath.c_str(), MeshName);
				}
				else if (FilePath.empty())
				{
					spdlog::error("Can't import mesh : {} file path is empty.", FilePath);
				}

			}


			/*if (ImGui::MenuItem("Export mesh.."))
			{
				std::string FilePath = Utils::HorusFileDialog::OpenFile("Horus Scene (*.hrs)\0*.hrs\0");

				if (!FilePath.empty())
				{
					spdlog::info("Export mesh : {}", FilePath);
				}
			}*/

			ImGui::Separator();

			// Export GLTF file
			if (ImGui::MenuItem("Export GLTF Scene"))
			{
				if (std::string FilePath = Utils::HorusFileDialog::SaveFile("Horus GLTF Scene (*.gltf)\0*.*\0"); !FilePath.empty())
				{
					// Adding .glft extension if not present at the end of the file name
					if (FilePath.find(".gltf") == std::string::npos)
					{
						FilePath += ".gltf";
					}

					HorusImporterManager::GetInstance().ExportGltf(FilePath, HorusRadeon::GetInstance().GetContext(), &HorusObjectManager::GetInstance().GetScene());
				}
			}

			// Import GLTF file
			if (ImGui::MenuItem("Import GLTF Scene"))
			{
				HorusImporterManager::GetInstance().ImportGltf("C:/Users/WS_THEO/Desktop/gltf/exporttest.gltf", HorusRadeon::GetInstance().GetContext(), HorusRadeon::GetInstance().GetMatsys(), &HorusObjectManager::GetInstance().GetScene());

				/*if (std::string FilePath = Utils::HorusFileDialog::OpenFile("Horus GLTF Scene (*.gltf)\0*.gltf\0"); !FilePath.empty())
				{


					spdlog::info("Import GLTF : {}", FilePath);
				}*/
			}




			ImGui::Separator();

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

		const float WindowWidth = ImGui::GetWindowWidth();
		const float TextWidth = ImGui::CalcTextSize("Version 1.0.0").x;
		ImGui::SetCursorPosX(WindowWidth - TextWidth - ImGui::GetStyle().ItemSpacing.x);
		ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "Version 1.0.0");

		ImGui::EndMainMenuBar();
	}
}

void HorusUI::RenderUI()
{
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
}

void HorusUI::ResetBuffers()
{
	HorusResetBuffers::GetInstance().CallResetBuffers();
}
