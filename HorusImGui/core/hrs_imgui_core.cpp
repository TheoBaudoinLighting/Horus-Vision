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
	std::cout << "Hello from HorusImGuiCore" << std::endl;
}

bool HorusImGuiCore::Initialize(const int Width, const int Height, HorusWindowConfig* WindowConfig)
{
	spdlog::info("Initialize ImGui..");

	__super::Init(Width, Height, WindowConfig);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImNodes::CreateContext();

	ImGui::SetCurrentContext(ImGui::GetCurrentContext());

	ImGuiIO& Io = ImGui::GetIO(); (void)Io;

	Io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	Io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	Io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	Io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	Io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;

	ImGui::StyleColorsDark();               // Apply the dark theme

	ImGuiStyle& Style = ImGui::GetStyle();  // Get the style structure
	ImVec4* Colors = Style.Colors;          // Get the colors array

	if (Io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		// Background

		Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.105f, 0.11f, 1.0f);

		// Headers

		Colors[ImGuiCol_Header] = ImVec4(0.2f, 0.205f, 0.21f, 1.0f);

		// Headers Hovered

		Colors[ImGuiCol_HeaderHovered] = ImVec4(0.3f, 0.305f, 0.31f, 1.0f);

		// Headers Active

		Colors[ImGuiCol_HeaderActive] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);

		// Buttons

		Colors[ImGuiCol_Button] = ImVec4(0.2f, 0.205f, 0.21f, 1.0f);

		// Buttons Hovered

		Colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.305f, 0.31f, 1.0f);

		// Buttons Active

		Colors[ImGuiCol_ButtonActive] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);

		// Frame Background

		Colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.205f, 0.21f, 1.0f);

		// Frame Background Hovered

		Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.3f, 0.305f, 0.31f, 1.0f);

		// Frame Background Active

		Colors[ImGuiCol_FrameBgActive] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);

		// Tabs

		Colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);

		// Tabs Hovered

		Colors[ImGuiCol_TabHovered] = ImVec4(0.39f, 0.4f, 0.41f, 1.0f);

		// Tabs Active

		Colors[ImGuiCol_TabActive] = ImVec4(0.28f, 0.28f, 0.28f, 1.0f);

		// Title

		Colors[ImGuiCol_TitleBg] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);

		// Title Active

		Colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);

		// Title Text

		Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);

		// List Box Background

		Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.15f, 0.1505f, 0.151f, 1.0f);

		// New color

		// Docking Preview

		Colors[ImGuiCol_DockingPreview] = ImVec4(0.6f, 1.0f, 0.6f, 1.0f); // Green

		// Tab Unfocused

		Colors[ImGuiCol_TabUnfocused] = ImVec4(1.0f, 0.6f, 0.7f, 1.0f); // Pink

		// Nav Highlight

		Colors[ImGuiCol_NavHighlight] = ImVec4(0.6f, 1.0f, 0.6f, 1.0f); // Green

		// Separator

		//Colors[ImGuiCol_Separator] = ImVec4(0.2f, 0.6f, 0.2f, 1.0f); // Green Dark

		// Header

		//Colors[ImGuiCol_Header] = ImVec4(0.6f, 1.0f, 0.6f, 1.0f); // Green

		// Slider Grab

		Colors[ImGuiCol_SliderGrab] = ImVec4(0.6f, 1.0f, 0.6f, 1.0f); // Green

		// Slider Grab Active

		Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 0.6f, 0.7f, 1.0f); // Pink

		// Check Mark

		Colors[ImGuiCol_CheckMark] = ImVec4(0.6f, 1.0f, 0.6f, 1.0f); // Green

		// Resize Grip Hovered

		Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.6f, 1.0f, 0.6f, 1.0f); // Green

		// Resize Grip Active

		Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.0f, 0.6f, 0.7f, 1.0f); // Pink

		// Tab Unfocused Active

		Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.35f, 0.63f, 0.39f, 1.0f); // Dark Green

		// Tab Active

		Colors[ImGuiCol_TabActive] = ImVec4(0.35f, 0.63f, 0.39f, 1.0f); // Dark Green

		// Tab Hovered

		Colors[ImGuiCol_TabHovered] = ImVec4(0.35f, 0.63f, 0.39f, 1.0f); // Dark Green




		//style->WindowRounding = 0.0f;
	}

	ImFontConfig FontCfg;
	FontCfg.FontDataOwnedByAtlas = false;
	FontCfg.PixelSnapH = false;
	FontCfg.OversampleH = 5;
	FontCfg.OversampleV = 5;
	FontCfg.RasterizerMultiply = 1.7f;

	//io.Fonts->AddFontFromFileTTF(m_font_SemiBold_path_.c_str(), 16.0f, &font_cfg);
	Io.Fonts->AddFontFromMemoryTTF(Montserrat_SemiBold, sizeof(Montserrat_SemiBold), 16.0f, &FontCfg);

	// Set font size
	FontCfg.SizePixels = 14.0f;

	static constexpr ImWchar IconsRanges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	ImFontConfig IconsConfig;
	IconsConfig.MergeMode = true;
	IconsConfig.PixelSnapH = true;
	IconsConfig.OversampleH = 5;
	IconsConfig.OversampleV = 5;
	IconsConfig.RasterizerMultiply = 1.7f;

	m_IconFont_ = Io.Fonts->AddFontFromMemoryCompressedTTF(font_awesome_data, font_awesome_size, 19.5f, &IconsConfig, IconsRanges);

	ImGui_ImplGlfw_InitForOpenGL(m_Window_->get_window(), true);
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

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	ImGui::Begin("Horus_MainDockspace", nullptr, WindowFlags);

	ImGui::PopStyleVar(3);
	const ImGuiID DockspaceId = ImGui::GetID("Horus_MainDockspace");
	ImGui::DockSpace(DockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

	ImGui::End();
}

void HorusImGuiCore::PostRender()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	ImGuiIO& io = ImGui::GetIO();

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* BackupCurrentContext = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(BackupCurrentContext);
	}
}

void HorusImGuiCore::QuitRender()
{
	spdlog::info("Unload ImGui..");
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImNodes::DestroyContext();
	ImGui::DestroyContext();
	spdlog::info("ImGui unloaded.");
}
