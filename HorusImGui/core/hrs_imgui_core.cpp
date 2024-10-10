#include "hrs_imgui_core.h"

#include "spdlog/spdlog.h"

#include "hrs_fonts.h"
#include "imnodes.h"

#include "GLFW/glfw3.h"
#include <hrs_icons.h>

#include "hrs_icons_fa.h"
#include "ImGuizmo.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void HorusImGuiCore::PrintMessage()
{
	// Print a message for debugging purposes
	std::cout << "Hello from HorusImGuiCore" << '\n';
}

bool HorusImGuiCore::Init(const int Width, const int Height, HorusWindowConfig* WindowConfig)
{
	spdlog::info("Initialize ImGui..");

	__super::Init(Width, Height, WindowConfig);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImNodes::CreateContext();

	ImGui::SetCurrentContext(ImGui::GetCurrentContext());

	ImGuiIO& Io = ImGui::GetIO(); (void)Io;

	// Fonts
	ImFontConfig FontCfg;
	FontCfg.FontDataOwnedByAtlas = false;
	FontCfg.PixelSnapH = false;
	FontCfg.OversampleH = 5;
	FontCfg.OversampleV = 5;
	FontCfg.RasterizerMultiply = 1.7f;
	FontCfg.SizePixels = 14.0f;

	// Icons
	static constexpr ImWchar IconsRanges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	ImFontConfig IconsConfig;
	IconsConfig.MergeMode = true;
	IconsConfig.PixelSnapH = true;
	IconsConfig.OversampleH = 5;
	IconsConfig.OversampleV = 5;
	IconsConfig.RasterizerMultiply = 1.7f;

	Io.Fonts->Clear();
	//io.Fonts->AddFontFromFileTTF(m_font_SemiBold_path_.c_str(), 16.0f, &font_cfg);
	Io.Fonts->AddFontFromMemoryTTF(Montserrat_SemiBold, sizeof(Montserrat_SemiBold), 16.0f, &FontCfg);
	m_IconFont_ = Io.Fonts->AddFontFromMemoryCompressedTTF(font_awesome_data, font_awesome_size, 19.5f, &IconsConfig, IconsRanges);
	Io.Fonts->Build();


	Io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	Io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	//Io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	//Io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	Io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
	Io.ConfigDockingWithShift = true;

	ImGui::StyleColorsDark();            

	// Get the style structure
	ImGuiStyle* Style = &ImGui::GetStyle();
	ImVec4* Colors = Style->Colors;          // Get the colors array

	if (Io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		// GreenHorusTheme
		Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.105f, 0.11f, 1.0f);
		Colors[ImGuiCol_Header] = ImVec4(0.2f, 0.205f, 0.21f, 1.0f);
		Colors[ImGuiCol_HeaderHovered] = ImVec4(0.3f, 0.305f, 0.31f, 1.0f);
		Colors[ImGuiCol_HeaderActive] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
		Colors[ImGuiCol_Button] = ImVec4(0.2f, 0.205f, 0.21f, 1.0f);
		Colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.305f, 0.31f, 1.0f);
		Colors[ImGuiCol_ButtonActive] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
		Colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.205f, 0.21f, 1.0f);
		Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.3f, 0.305f, 0.31f, 1.0f);
		Colors[ImGuiCol_FrameBgActive] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
		Colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
		Colors[ImGuiCol_TabHovered] = ImVec4(0.39f, 0.4f, 0.41f, 1.0f);
		Colors[ImGuiCol_TabActive] = ImVec4(0.28f, 0.28f, 0.28f, 1.0f);
		Colors[ImGuiCol_TitleBg] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
		Colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
		Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
		Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);
		Colors[ImGuiCol_DockingPreview] = ImVec4(0.6f, 1.0f, 0.6f, 1.0f);
		Colors[ImGuiCol_TabUnfocused] = ImVec4(1.0f, 0.6f, 0.7f, 1.0f);
		Colors[ImGuiCol_NavHighlight] = ImVec4(0.6f, 1.0f, 0.6f, 1.0f);
		Colors[ImGuiCol_SliderGrab] = ImVec4(0.6f, 1.0f, 0.6f, 1.0f);
		Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 0.6f, 0.7f, 1.0f);
		Colors[ImGuiCol_CheckMark] = ImVec4(0.6f, 1.0f, 0.6f, 1.0f);
		Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.6f, 1.0f, 0.6f, 1.0f);
		Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.0f, 0.6f, 0.7f, 1.0f);
		Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.35f, 0.63f, 0.39f, 1.0f);
		Colors[ImGuiCol_TabActive] = ImVec4(0.35f, 0.63f, 0.39f, 1.0f);
		Colors[ImGuiCol_TabHovered] = ImVec4(0.35f, 0.63f, 0.39f, 1.0f);
	}

	ImGui_ImplGlfw_InitForOpenGL(m_Window_->GetWindow(), true);
	ImGui_ImplOpenGL3_Init(m_GlslVersion_);

	spdlog::info("ImGui initialized.");

	return true;
}

void HorusImGuiCore::InitRender()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::SetOrthographic(false);
	ImGuizmo::BeginFrame();

	const ImGuiWindowFlags WindowFlags =
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_MenuBar |
		ImGuiWindowFlags_AlwaysAutoResize;

	const ImGuiViewport* Viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(Viewport->Pos);
	ImGui::SetNextWindowSize(Viewport->Size);
	ImGui::SetNextWindowViewport(Viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 3.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	if (!ImGui::Begin("Horus_MainDockspace", nullptr, WindowFlags))
	{
		ImGui::End();
		return;
	}

	ImGui::PopStyleVar(3);
	const ImGuiID DockspaceId = ImGui::GetID("Horus_MainDockspace");
	ImGui::DockSpace(DockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

	ImGui::End();
}

void HorusImGuiCore::PostRender()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	ImGuiIO& Io = ImGui::GetIO();

	if (Io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* BackupCurrentContext = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(BackupCurrentContext);
	}
}

void HorusImGuiCore::QuitRender()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImNodes::DestroyContext();
	ImGui::DestroyContext();
	spdlog::info("ImGui has been unloaded.");
}
