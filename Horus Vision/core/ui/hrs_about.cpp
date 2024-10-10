#include "hrs_about.h"

#include "hrs_horus_parameters.h"
#include "hrs_utils.h"

#include "imgui.h"


void HorusAbout::About(bool* p_open)
{
	if (!ImGui::Begin(HORUS_UI_NAME_ABOUT.c_str(), p_open))
	{
		ImGui::End();
		return;
	}

	const char* GithubLink = "https://github.com/TheoBaudoinLighting/Horus-Vision";
	const char* ArtstationLink = "https://www.artstation.com/theobaudoin";
	const char* LinkedInLink = "https://www.linkedin.com/in/theobaudoin/";

	// Credit links
	const char* SonghoAhnLink = "https://www.songho.ca/opengl/index.html";
	const char* RayTracingInOneWeekendLink = "https://raytracing.github.io/books/RayTracingInOneWeekend.html";
	const char* RadeonProRenderSDKLink = "https://github.com/GPUOpen-LibrariesAndSDKs/RadeonProRenderSDK";
	const char* GLSLPathTracerLink = "https://github.com/knightcrawler25/GLSL-PathTracer";

	ImGui::TextWrapped("Horus Vision");

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::TextWrapped(HORUS_APP_VERSION.c_str());
	ImGui::Separator();
	ImGui::TextWrapped("Horus Vision is an advanced, physically-based rendering engine powered by Radeon Pro Render. Designed for high-quality, realistic output, it provides a range of features to cater to diverse rendering needs.");
	ImGui::Separator();
	ImGui::TextWrapped(
		"This project is strictly personal and comes with no warranty of functionality, "
		"reliability, or suitability for any purpose. Use it at your own risk. "
		"The developer assumes no liability for any damages or loss of data that may occur "
		"from its use. Please ensure you understand the implications of running this software "
		"before proceeding."
	);
	ImGui::Separator();
	ImGui::TextWrapped("Developped by Theo Baudoin Malnoe");
	ImGui::TextWrapped("For more information, please visit :");

	// Link to the Horus Vision GitHub repository
	ImGui::TextWrapped("Github :");
	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.00f));
	ImGui::TextWrapped(GithubLink);
	ImGui::PopStyleColor();
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
	}
	if (ImGui::IsItemClicked())
	{
		OsOpenInShell(GithubLink);
	}

	// Link to artsation
	ImGui::TextWrapped("Artstation :");
	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.00f));
	ImGui::TextWrapped(ArtstationLink);
	ImGui::PopStyleColor();
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
	}
	if (ImGui::IsItemClicked())
	{
		OsOpenInShell(ArtstationLink);
	}

	// Link to LinkedIn
	ImGui::TextWrapped("LinkedIn :");
	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.00f));
	ImGui::TextWrapped(LinkedInLink);
	ImGui::PopStyleColor();
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
	}
	if (ImGui::IsItemClicked())
	{
		OsOpenInShell(LinkedInLink);
	}

	ImGui::Separator();
	ImGui::TextWrapped("Horus Vision is licensed under the GNU Open 3 License.");

	// Credits
	ImGui::Separator();
	ImGui::Text("Credits");
	ImGui::Separator();

	// RadeonProRender SDK
	ImGui::TextWrapped("RadeonProRender SDK: Key for providing a high-performance rendering engine foundation, "
		"enabling advanced features like real-time ray tracing.");

	ImGui::TextWrapped("Visit RadeonProRender SDK :");
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.00f));
	ImGui::SameLine();
	ImGui::TextWrapped(RadeonProRenderSDKLink);
	ImGui::PopStyleColor();
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
	}
	if (ImGui::IsItemClicked())
	{
		OsOpenInShell(RadeonProRenderSDKLink);
	}

	ImGui::Separator();

	// Ray Tracing in One Weekend
	ImGui::TextWrapped("Ray Tracing in One Weekend: Instrumental in understanding the basics of rendering engines and "
		"ray tracing techniques, contributing significantly to the project's development.");

	ImGui::TextWrapped("Visit Ray Tracing in One Weekend :");
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.00f));
	ImGui::SameLine();
	ImGui::TextWrapped(RayTracingInOneWeekendLink);
	ImGui::PopStyleColor();
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
	}
	if (ImGui::IsItemClicked())
	{
		OsOpenInShell(RayTracingInOneWeekendLink);
	}
	ImGui::Separator();

	ImGui::TextWrapped("Songho Ahn's OpenGL Tutorials (https://www.songho.ca/opengl/index.html): Vital resource for "
		"comprehending transformations, rendering pipelines, and quaternions in graphics programming.");
	ImGui::TextWrapped("Visit Songho Ahn's OpenGL Tutorials :");
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.00f));
	ImGui::SameLine();
	ImGui::TextWrapped(SonghoAhnLink);
	ImGui::PopStyleColor();
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
	}
	if (ImGui::IsItemClicked())
	{
		OsOpenInShell(SonghoAhnLink);
	}

	ImGui::Separator();
	ImGui::TextWrapped("GLSL Path Tracer: Provided crucial confirmations and references for implementing realistic "
		"lighting and shading techniques, enhancing the overall quality and accuracy of the rendering engine.");
	ImGui::TextWrapped("Visit GLSL Path Tracer :");
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.00f));
	ImGui::SameLine();
	ImGui::TextWrapped(GLSLPathTracerLink);
	ImGui::PopStyleColor();
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
	}
	if (ImGui::IsItemClicked())
	{
		OsOpenInShell(GLSLPathTracerLink);
	}
	ImGui::Separator();

	ImGui::End();
}
