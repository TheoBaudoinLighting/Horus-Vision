
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
#include "hrs_horus_parameters.h"

#include "hrs_imgui_core.h"

#include "hrs_about.h"
#include "hrs_help.h"
#include "hrs_import_mesh_ui.h"
#include "hrs_om_group_shapes.h"
#include "hrs_opengl_viewport.h"
#include "hrs_radeon_viewport.h"
#include "hrs_statistics.h"
#include "hrs_utils.h"
#include "../scene/hrs_importer_manager.h"
#include "imgui_internal.h"
#include "stb_image.h"

// TODO : Move this to HorusUI

bool LoadTextureFromFile(const char* Filename, GLuint* OutTexture, int* OutWidth, int* OutHeight)
{
	int ImageWidth = 0;
	int ImageHeight = 0;
	unsigned char* ImageData = stbi_load(Filename, &ImageWidth, &ImageHeight, nullptr, 4);
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

	*OutTexture = ImageTexture;
	*OutWidth = ImageWidth;
	*OutHeight = ImageHeight;

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

// TODO : Move this to HorusUI
void IndependentThread()
{
	std::cout << "Starting concurrent thread.\n";
	HorusObjectManager::GetInstance().ShowJaguardXKSS();
	//std::this_thread::sleep_for(std::chrono::seconds(10));
	std::cout << "Exiting concurrent thread.\n";
}

void threadCaller()
{
	std::cout << "Starting thread caller.\n";
	std::thread t(IndependentThread);
	t.detach();
	std::cout << "Exiting thread caller.\n";
}

void ImportObjectThread(std::string FilePath)
{
	spdlog::debug("Starting Import thread.");

	const std::string MeshName = std::filesystem::path(FilePath).stem().string();
	// Change by HorusgroupShapeManager
	HorusGroupShapeManager::GetInstance().CreateGroupShape(MeshName, FilePath.c_str());

	spdlog::debug("Exiting Import thread.");
}

void HorusUI::MainMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (m_LoadLogoTexture_ == true)
		{
			bool Ret = LoadTextureFromFile("resources/Icons/Horus_Logo_100x20.png", &m_LogoTexture_, &m_LogoWidth_, &m_LogoHeight_);
			if (!Ret)
			{
				spdlog::error("Can't load Horus logo texture.");

				m_LogoTexture_ = false;
				return;
			}

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
			ImGui::BeginDisabled(true);
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
			ShowHandCursorOnHover();

			if (ImGui::MenuItem("Save scene", "Ctrl+S"))
			{
				if (std::string FilePath = Utils::HorusFileDialog::SaveFile("Horus Scene (*.hrs)\0*.hrs\0"); !FilePath.empty())
				{
					spdlog::info("Save file : {}.hrs", FilePath);
				}
			}
			ShowHandCursorOnHover();

			if (ImGui::MenuItem("Save scene as..", "Ctrl+Alt+S"))
			{
				//threadCaller();
			}
			ShowHandCursorOnHover();
			ImGui::EndDisabled();

			if (ImGui::MenuItem("Load test scene", "Ctrl+L"))
			{
				HorusObjectManager::GetInstance().ShowLookdevScene();
				//HorusObjectManager::GetInstance().ShowJaguardXKSS();
			}
			ShowHandCursorOnHover();

			ImGui::Separator();

			ImGui::MenuItem("Import Mesh..", nullptr, &mShowImportMesh_); ShowHandCursorOnHover();

			/*if (ImGui::MenuItem("Export mesh.."))
			{
				std::string FilePath = Utils::HorusFileDialog::OpenFile("Horus Scene (*.hrs)\0*.hrs\0");

				if (!FilePath.empty())
				{
					spdlog::info("Export mesh : {}", FilePath);
				}
			}
			ShowHandCursorOnHover();*/

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
			ShowHandCursorOnHover();

			// Import GLTF file
			ImGui::BeginDisabled(true);
			if (ImGui::MenuItem("Import GLTF Scene"))
			{
				HorusImporterManager::GetInstance().ImportGltf("C:/Users/WS_THEO/Desktop/gltf/exporttest.gltf", HorusRadeon::GetInstance().GetContext(), HorusRadeon::GetInstance().GetMatsys(), &HorusObjectManager::GetInstance().GetScene());

				/*if (std::string FilePath = Utils::HorusFileDialog::OpenFile("Horus GLTF Scene (*.gltf)\0*.gltf\0"); !FilePath.empty())
				{


					spdlog::info("Import GLTF : {}", FilePath);
				}*/
			}
			ShowHandCursorOnHover();
			ImGui::EndDisabled();

			ImGui::Separator();

			if (ImGui::MenuItem("Quit"))
			{
				HorusEngine::GetInstance().SetIsClosing(true);
			}
			ShowHandCursorOnHover();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Cancel"))
			{

			}
			ShowHandCursorOnHover();

			if (ImGui::MenuItem("Revert back"))
			{

			}
			ShowHandCursorOnHover();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{
			ImGui::SeparatorText("Show / Hide");

			ImGui::MenuItem(HORUS_UI_NAME_OPENGLVIEWPORT.c_str(), nullptr, &mShowOpenGlViewport_); ShowHandCursorOnHover();
			ImGui::MenuItem(HORUS_UI_NAME_RADEONVIEWPORT.c_str(), nullptr, &mShowRadeonViewport_); ShowHandCursorOnHover();
			ImGui::MenuItem(HORUS_UI_NAME_OUTLINER.c_str(), nullptr, &mShowOutliner_); ShowHandCursorOnHover();
			ImGui::MenuItem(HORUS_UI_NAME_CONSOLE.c_str(), nullptr, &mShowConsole_); ShowHandCursorOnHover();
			ImGui::MenuItem(HORUS_UI_NAME_INSPECTOR.c_str(), nullptr, &mShowInspector_); ShowHandCursorOnHover();
			ImGui::MenuItem(HORUS_UI_NAME_STATISTICS.c_str(), nullptr, &mShowStatistics_); ShowHandCursorOnHover();

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu(HORUS_UI_NAME_HELP.c_str()))
		{
			ImGui::MenuItem("Documentation", nullptr, &mShowHelp_); ShowHandCursorOnHover();
			ImGui::MenuItem(HORUS_UI_NAME_ABOUT.c_str(), nullptr, &mShowAbout_); ShowHandCursorOnHover();

			ImGui::EndMenu();
		}

		const float WindowWidth = ImGui::GetWindowWidth();
		const float TextWidth = ImGui::CalcTextSize("Version 0.0.0").x; // for size of the text only
		ImGui::SetCursorPosX(WindowWidth - TextWidth - ImGui::GetStyle().ItemSpacing.x);
		ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), HORUS_APP_VERSION.c_str());

		ImGui::EndMainMenuBar();
	}
}

void HorusUI::RenderUI()
{
	MainMenuBar();

	if (mShowOpenGlViewport_)
	{
		HorusViewportOpenGL::GetInstance().ViewportOpenGL(&mShowOpenGlViewport_);
	}

	if (mShowRadeonViewport_)
	{
		HorusViewportRadeon::GetInstance().ViewportRadeon(&mShowRadeonViewport_);
	}

	if (mShowInspector_)
	{
		HorusInspector::GetInstance().Inspector(&mShowInspector_);
	}

	if (mShowOutliner_)
	{
		HorusOutliner::GetInstance().Outliner(&mShowOutliner_);
	}

	if (mShowConsole_)
	{
		HorusConsole::GetInstance().Console(&mShowConsole_);
	}

	if (mShowStatistics_)
	{
		HorusStatistics::GetInstance().Statistics(&mShowStatistics_);
	}

	if (mShowAbout_)
	{
		HorusAbout::GetInstance().About(&mShowAbout_);
	}

	if (mShowHelp_)
	{
		HorusHelp::GetInstance().RenderHelp(&mShowHelp_);
	}

	if (mShowImportMesh_)
	{
		HorusImportMeshUI::GetInstance().ImportMeshUI(&mShowImportMesh_);
	}

}

void HorusUI::ResetBuffers()
{
	HorusResetBuffers::GetInstance().CallResetBuffers();
}
