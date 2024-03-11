#include "hrs_help.h"

#include "hrs_horus_parameters.h"
#include "hrs_utils.h"

#include "imgui.h"



void HorusHelp::RenderHelp(bool* p_open)
{
    if (!ImGui::Begin(HORUS_UI_NAME_HELP.c_str(), p_open))
    {
        ImGui::End();
        return;
    }

    const char* GithubIssues = "https://github.com/TheoBaudoinLighting/Horus-Vision/issues";

    // Introduction
    ImGui::TextWrapped("Welcome to the Horus Rendering Engine Help. This section provides information on how to utilize the various features available.");

    ImGui::Separator();

    // Camera Controls
    ImGui::Text("Camera Controls");
    ImGui::BulletText("Alt + Left Click to tumble (rotate) the camera");
    ImGui::BulletText("Alt + Right Click to zoom in and out");
    ImGui::BulletText("Alt + Middle Mouse Click (Scroll Wheel) to strafe (pan)");

    ImGui::Separator();

    // Rendering Engine Features
    ImGui::Text("Rendering Engine Features");
    ImGui::BulletText("Support for different material types");
    ImGui::BulletText("Dynamic lighting system");
    ImGui::BulletText("Real-time performance optimizations");

    ImGui::Separator();

    // Keyboard Shortcuts will come later as indicated
    ImGui::Text("Keyboard Shortcuts");
    ImGui::TextWrapped("Keyboard shortcuts will be provided later. They will include quick access to common features and tools.");

    ImGui::Separator();

    // Tips and Tricks
    ImGui::Text("Tips and Tricks");
    ImGui::BulletText("Use layers to organize your objects within the scene.");
    ImGui::BulletText("Experiment with material parameters for unique renderings.");

    ImGui::Separator();

    // Support and Contact
    ImGui::Text("Support and Contact");
    ImGui::TextWrapped("For additional help or to report bugs, please contact support at : ");
    ImGui::TextWrapped(GithubIssues);
    if (ImGui::IsItemHovered())
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		ImGui::SetTooltip("Click to open the Horus Vision GitHub Issues page.");
	}
    if (ImGui::IsItemClicked())
    {
        OsOpenInShell(GithubIssues);
    }

    ImGui::End();
}
