#pragma once
// File of utility functions for Horus Vision 4305

#pragma warning(disable: 4305)
#pragma warning(disable: 4244)

#define NOMINMAX
#include <Windows.h> 
#include <shellapi.h> 

// External includes
#include "hrs_garbage_collector.h"
#include <Math/mathutils.h>

#include "glm/glm.hpp"

#include "spdlog/spdlog.h"

#include <cassert>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

#include "imgui.h"

inline void OsOpenInShell(const char* path)
{
	::ShellExecuteA(NULL, "open", path, NULL, NULL, SW_SHOWDEFAULT);
}

using namespace std;

struct Vertex
{
	rpr_float Pos[3];
	rpr_float Norm[3];
	rpr_float Tex[2];
};

inline rpr_shape CreateQuad(HorusGarbageCollector& Gc, rpr_context Context, rpr_scene Scene, Vertex* MeshVertices, unsigned int MeshVerticesNbOfElement)
{
	rpr_int Indices[] = { 3,2,1,0, };
	rpr_int NumFaceVertices[] = { 4, };

	const unsigned int NumFaceVerticesNbOfElement = sizeof(NumFaceVertices) / sizeof(NumFaceVertices[0]);

	rpr_shape Mesh = nullptr;

	CHECK(rprContextCreateMesh(Context,
		(rpr_float const*)&MeshVertices[0], MeshVerticesNbOfElement, sizeof(Vertex),
		(rpr_float const*)((char*)&MeshVertices[0] + sizeof(rpr_float) * 3), MeshVerticesNbOfElement, sizeof(Vertex),
		(rpr_float const*)((char*)&MeshVertices[0] + sizeof(rpr_float) * 6), MeshVerticesNbOfElement, sizeof(Vertex),
		(rpr_int const*)Indices, sizeof(rpr_int),
		(rpr_int const*)Indices, sizeof(rpr_int),
		(rpr_int const*)Indices, sizeof(rpr_int),
		NumFaceVertices, NumFaceVerticesNbOfElement, &Mesh));
	Gc.Add(Mesh);

	if (Scene) { CHECK(rprSceneAttachShape(Scene, Mesh)); }

	return Mesh;
}

inline rpr_shape CreateQuadXy(HorusGarbageCollector& Gc, rpr_context Context, rpr_scene Scene, float ax, float ay, float bx, float by, float Z, float Normal)
{
	Vertex MeshVertices[] =
	{
		{{ax, by, Z}, {0.0f, 0.0f, Normal}, {0.0f, 0.0f}},
		{{bx, by, Z}, {0.0f, 0.0f, Normal}, {1.0f, 0.0f}},
		{{bx, ay, Z}, {0.0f, 0.0f, Normal}, {1.0f, 1.0f}},
		{{ax, ay, Z}, {0.0f, 0.0f, Normal}, {0.0f, 1.0f}},
	};

	const unsigned int MeshVerticesNbOfElement = sizeof(MeshVertices) / sizeof(MeshVertices[0]);

	rpr_shape Mesh = CreateQuad(Gc, Context, Scene, MeshVertices, MeshVerticesNbOfElement);
	return Mesh;
}

inline rpr_shape CreateSquare(HorusGarbageCollector& Gc, rpr_context Context, rpr_scene Scene, float PosX, float PosY, const float Size, const float Margin)
{
	// Basic Check
	if (Size <= 0.0f)
	{
		spdlog::error("Size must be greater than 0.0f");
		return nullptr;
	}

	if (Context == nullptr)
	{
		spdlog::error("Context is null");
		return nullptr;
	}

	if (Scene == nullptr)
	{
		spdlog::error("Scene is null");
		return nullptr;
	}

	float HalfSize = Size / 2.0f;

	rpr_shape Quad = CreateQuadXy(Gc, Context, Scene,
		-HalfSize + PosX * (Margin + Size), -HalfSize + PosY * (Margin + Size),
		HalfSize + PosX * (Margin + Size), HalfSize + PosY * (Margin + Size),
		0.0, 1.0f);

	return Quad;
}

// ImGui utility functions
inline void ShowHandCursorOnHover()
{
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
	}
}

inline void ShowResizeCursorOnHover()
{
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
	}
}

inline void ShowResizeHorizontalCursorOnHover()
{
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
	}
}

inline void ShowResizeVerticalCursorOnHover()
{
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
	}
}

inline void ShowResizeTopLeftBottomRightCursorOnHover()
{
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNESW);
	}
}

inline void ShowResizeTopRightBottomLeftCursorOnHover()
{
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNWSE);
	}
}

inline void ShowTextInputCursorOnHover()
{
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput);
	}
}

inline void ShowNotAllowedCursorOnHover()
{
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_NotAllowed);
	}
}

inline void ShowVerticalResizeCursorOnHover()
{
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
	}
}

inline void ShowHorizontalResizeCursorOnHover()
{
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
	}
}

inline void ShowHandCursorOnHover(const char* tooltip)
{
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		ImGui::SetTooltip(tooltip);
	}
}		

inline void ShowResizeCursorOnHover(const char* tooltip)
{
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
		ImGui::SetTooltip(tooltip);
	}
}

inline void ShowResizeHorizontalCursorOnHover(const char* tooltip)
{
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
		ImGui::SetTooltip(tooltip);
	}
}

// ImGui Link 
inline void OpenLink(const char* text, const char* url)
{
	ImGui::TextUnformatted(text);
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		if (ImGui::IsMouseClicked(0))
		{
			OsOpenInShell(url);
		}
	}
}

inline void OpenLink(const char* text, const char* url, const ImVec4& color)
{
	ImGui::TextColored(color, text);
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		if (ImGui::IsMouseClicked(0))
		{
			OsOpenInShell(url);
		}
	}
}

inline void OpenLink(const char* text, const char* url, const ImVec4& color, float fontSize)
{
	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
	ImGui::TextColored(color, text);
	ImGui::PopFont();
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		if (ImGui::IsMouseClicked(0))
		{
			OsOpenInShell(url);
		}
	}
}

inline void OpenLink(const char* text, const char* url, const ImVec4& color, float fontSize, float width)
{
	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
	ImGui::SetCursorPosX((width - ImGui::CalcTextSize(text).x) / 2);
	ImGui::TextColored(color, text);
	ImGui::PopFont();
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		if (ImGui::IsMouseClicked(0))
		{
			OsOpenInShell(url);
		}
	}
}

// ImGui Draw a rectangle
inline void DrawRectangle(const ImVec2& position, const ImVec2& size, const ImVec4& color)
{
	ImGui::GetWindowDrawList()->AddRectFilled(position, ImVec2(position.x + size.x, position.y + size.y), ImGui::GetColorU32(color));
}

// ImGui Draw a rectangle with a border
inline void DrawRectangle(const ImVec2& position, const ImVec2& size, const ImVec4& color, const ImVec4& borderColor)
{
	ImGui::GetWindowDrawList()->AddRectFilled(position, ImVec2(position.x + size.x, position.y + size.y), ImGui::GetColorU32(color));
	ImGui::GetWindowDrawList()->AddRect(position, ImVec2(position.x + size.x, position.y + size.y), ImGui::GetColorU32(borderColor));
}

// ImGui Draw a rectangle around item
inline void DrawRectangleAroundItem(const ImVec4& color)
{
	ImVec2 min = ImGui::GetItemRectMin();
	ImVec2 max = ImGui::GetItemRectMax();
	ImGui::GetWindowDrawList()->AddRect(min, max, ImGui::GetColorU32(color));
}





// Set the tooltip of the current item
inline void	SetTooltip(const char* text)
{
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip(text);
	}
}
inline void SetTooltip(const char* text, const ImVec4& color)
{
	if (ImGui::IsItemHovered())
	{
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::SetTooltip(text);
		ImGui::PopStyleColor();
	}
}
inline void SetTooltip(const char* text, const ImVec4& color, float fontSize)
{
	if (ImGui::IsItemHovered())
	{
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
		ImGui::SetTooltip(text);
		ImGui::PopFont();
		ImGui::PopStyleColor();
	}
}
inline void SetTooltip(const char* text, const ImVec4& color, float fontSize, float width)
{
	if (ImGui::IsItemHovered())
	{
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
		ImGui::SetTooltip(text);
		ImGui::PopFont();
		ImGui::PopStyleColor();
	}
}
inline void SetTooltip(const char* text, const ImVec4& color, float fontSize, float width, float height)
{
	if (ImGui::IsItemHovered())
	{
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
		ImGui::SetTooltip(text);
		ImGui::PopFont();
		ImGui::PopStyleColor();
	}
}
inline void SetTooltip(const char* text, const ImVec4& color, float fontSize, float width, float height, bool centered)
{
	if (ImGui::IsItemHovered())
	{
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
		ImGui::SetTooltip(text);
		ImGui::PopFont();
		ImGui::PopStyleColor();
	}
}








// Text utility functions

inline void TextCentered(const char* Text)
{
	ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(Text).x) / 2);
	ImGui::Text(Text);
}

inline void TextCentered(const char* Text, const ImVec4& Color)
{
	ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(Text).x) / 2);
	ImGui::PushStyleColor(ImGuiCol_Text, Color);
	ImGui::Text(Text);
	ImGui::PopStyleColor();
}

inline void TextCentered(const char* Text, const ImVec4& Color, float FontSize)
{
	ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(Text).x) / 2);
	ImGui::PushStyleColor(ImGuiCol_Text, Color);
	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
	ImGui::Text(Text);
	ImGui::PopFont();
	ImGui::PopStyleColor();
}

inline void TextCentered(const char* Text, const ImVec4& Color, float FontSize, float Width)
{
	ImGui::SetCursorPosX((Width - ImGui::CalcTextSize(Text).x) / 2);
	ImGui::PushStyleColor(ImGuiCol_Text, Color);
	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
	ImGui::Text(Text);
	ImGui::PopFont();
	ImGui::PopStyleColor();
}

// Convert a string to a wide string
inline std::wstring StringToWideString(const std::string& str)
{
	std::wstring wstr(str.length(), L' ');
	std::copy(str.begin(), str.end(), wstr.begin());
	return wstr;
}

// Convert a wide string to a string
inline std::string WideStringToString(const std::wstring& wstr)
{
	std::string str(wstr.length(), ' ');
	std::copy(wstr.begin(), wstr.end(), str.begin());
	return str;
}

// Convert const char* to std::string
inline std::string CTS(const char* Char)
{
	std::string Str(Char);
	return Str;
}

// Convert std::string to const char*
inline const char* STC(const std::string& Str)
{
	return Str.c_str();
}

// Extract the file name from a path
inline std::string ExtractFileName(const std::string& Path)
{
	size_t LastSlash = Path.find_last_of("\\/");
	return Path.substr(LastSlash + 1);
}

// Extract the file extension from a path
inline std::string ExtractFileExtension(const std::string& Path)
{
	size_t LastDot = Path.find_last_of(".");
	return Path.substr(LastDot + 1);
}

// Extract the file name without the extension from a path
inline std::string ExtractFileNameWithoutExtension(const std::string& Path)
{
	std::string FileName = ExtractFileName(Path);
	size_t LastDot = FileName.find_last_of(".");
	return FileName.substr(0, LastDot);
}

// Check if a file exists
inline bool FileExists(const std::string& Path)
{
	std::ifstream File(Path);
	return File.good();
}

// Read text file
inline std::string ReadTextFile(const std::string& filePath)
{
	std::ifstream fileStream(filePath);
	std::stringstream buffer;
	buffer << fileStream.rdbuf();
	return buffer.str();
}

// Write text file
inline void WriteTextFile(const std::string& filePath, const std::string& content)
{
	std::ofstream fileStream(filePath);
	fileStream << content;
}

// read binary file
inline std::vector<char> ReadBinaryFile(const std::string& filePath)
{
	std::ifstream fileStream(filePath, std::ios::binary | std::ios::ate);
	std::streamsize size = fileStream.tellg();
	fileStream.seekg(0, std::ios::beg);
	std::vector<char> buffer(size);
	if (fileStream.read(buffer.data(), size)) {
		return buffer;
	}
	else {
		return {};
	}
}

// write binary file
inline void WriteBinaryFile(const std::string& filePath, const std::vector<char>& data)
{
	std::ofstream fileStream(filePath, std::ios::binary);
	fileStream.write(data.data(), data.size());
}

// Check extension
inline bool CheckExtension(const std::string& fileName, const std::string& extension)
{
	size_t pos = fileName.rfind('.');
	if (pos != std::string::npos) {
		return fileName.substr(pos + 1) == extension;
	}
	return false;
}

inline std::string ConcatenatePath(const std::string& directory, const std::string& fileName)
{
	return directory + ((directory.back() == '/' || directory.back() == '\\') ? "" : "/") + fileName;
}

inline std::string FormatPath(std::string path)
{
	std::replace(path.begin(), path.end(), '\\', '/');
	return path;
}

inline std::vector<std::string> SplitString(const std::string& str, char delimiter)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(str);
	while (std::getline(tokenStream, token, delimiter)) {
		tokens.push_back(token);
	}
	return tokens;
}

//-------------------------------------------------------- DEBUGGING AND MESSAGE ----------------------------------//

// Show an info message
inline void LogInfo(const std::string& message) {
	std::cout << "Info: " << message << '\n';
}

// Show an error message
inline void LogError(const std::string& message) {
	std::cerr << "Error: " << message << '\n';
}

// Show a warning message
inline void LogWarning(const std::string& message) {
	std::cout << "Warning: " << message << '\n';
}

// AssertCondition: Check a condition and display a message if it is not met.
inline void AssertCondition(bool condition, const std::string& message) {
	if (!condition) {
		std::cerr << "Assertion failed: " << message << '\n';
		abort();
	}
}

// DisplayFPS: Calculate and display frames per second.
inline void DisplayFPS(double deltaTime)
{
	static int frameCount = 0;
	static double totalTime = 0.0;
	totalTime += deltaTime;
	++frameCount;
	if (totalTime > 1.0)
	{
		std::cout << "FPS: " << frameCount / totalTime << '\n';
		frameCount = 0;
		totalTime = 0.0;
	}
}

// Show the components of a vector
inline void DebugVector(const glm::vec3& vec) {
	std::cout << "Vector: (" << vec.x << ", " << vec.y << ", " << vec.z << ")" << '\n';
}

// Show the components of a matrix
inline void DebugMatrix(const glm::mat4& mat)
{
	std::cout << "Matrix: " << '\n';
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			std::cout << mat[i][j] << " ";
		}
		std::cout << '\n';
	}
}

// This function is a template function that takes a function and its arguments 
// and measures the time it takes to execute the function.
// Exemple of use: ExecutionTime("FunctionName", FunctionName, arg1, arg2, arg3);
#include <chrono>
template<typename Func, typename... Args>
void ExecutionTime(const std::string& functionName, Func func, Args&&... args)
{
	auto start = std::chrono::high_resolution_clock::now();
	func(std::forward<Args>(args)...);
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> duration = end - start;
	std::cout << functionName << " executed in " << duration.count() << " ms" << std::endl;
}

// ListResources: Display a list of resources.
inline void ListResources(const std::vector<std::string>& resources) {
	std::cout << "Loaded Resources:" << '\n';
	for (const auto& resource : resources) {
		std::cout << "- " << resource << '\n';
	}
}



