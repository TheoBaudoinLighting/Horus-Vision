
// External includes
#include "imgui.h"
#include "imnodes.h"

// Basic includes
#include <filesystem>
#include <regex>
#include <vector>
#include <chrono>

// Project includes
#include "hrs_engine.h" // glad.h
#include "hrs_object_manager.h" // glfw3.h
#include "hrs_logger.h" // nothing
#include "hrs_ui.h" // nothing
#include "hrs_reset_buffers.h" // glfw3.h

// External includes
#include "ImGuizmo.h"
#include "L2DFileDialog.h"
#include <hrs_console.h>

static long long duration = 0;
bool options_changed = false;

int HorusEngine::context_info(rpr_context& context)
{
	rpr_int status = RPR_SUCCESS;

	std::vector<rpr_context_info> context_info_list;

	context_info_list.push_back(RPR_CONTEXT_LIST_CREATED_CAMERAS);
	context_info_list.push_back(RPR_CONTEXT_LIST_CREATED_MATERIALNODES);
	context_info_list.push_back(RPR_CONTEXT_LIST_CREATED_LIGHTS);
	context_info_list.push_back(RPR_CONTEXT_LIST_CREATED_SHAPES);
	context_info_list.push_back(RPR_CONTEXT_LIST_CREATED_POSTEFFECTS);
	context_info_list.push_back(RPR_CONTEXT_LIST_CREATED_HETEROVOLUMES);
	context_info_list.push_back(RPR_CONTEXT_LIST_CREATED_GRIDS);
	context_info_list.push_back(RPR_CONTEXT_LIST_CREATED_BUFFERS);
	context_info_list.push_back(RPR_CONTEXT_LIST_CREATED_IMAGES);
	context_info_list.push_back(RPR_CONTEXT_LIST_CREATED_FRAMEBUFFERS);
	context_info_list.push_back(RPR_CONTEXT_LIST_CREATED_SCENES);
	context_info_list.push_back(RPR_CONTEXT_LIST_CREATED_CURVES);
	context_info_list.push_back(RPR_CONTEXT_LIST_CREATED_MATERIALSYSTEM);
	context_info_list.push_back(RPR_CONTEXT_LIST_CREATED_COMPOSITE);
	context_info_list.push_back(RPR_CONTEXT_LIST_CREATED_LUT);

	std::unordered_map<std::string, int> object_count;

	for (int i = 0; i < context_info_list.size(); i++)
	{
		size_t count = 0;
		status = rprContextGetInfo(context, context_info_list[i], 0, nullptr, &count);

		int size = count / sizeof(void*);

		if (size > 0)
		{
			std::string objectType;
			int countPerObject = 0;

			if (context_info_list[i] == RPR_CONTEXT_LIST_CREATED_CAMERAS)
			{
				objectType = "camera";
				countPerObject = size;
				camera_number_ = size;
			}
			else if (context_info_list[i] == RPR_CONTEXT_LIST_CREATED_MATERIALNODES)
			{
				objectType = "material node";
				countPerObject = size;
				material_number_ = size;
			}
			else if (context_info_list[i] == RPR_CONTEXT_LIST_CREATED_LIGHTS)
			{
				objectType = "light";
				countPerObject = size;
				light_number_ = size;
			}
			else if (context_info_list[i] == RPR_CONTEXT_LIST_CREATED_SHAPES)
			{
				objectType = "shape";
				countPerObject = size;
				mesh_number_ = size;
			}
			else if (context_info_list[i] == RPR_CONTEXT_LIST_CREATED_POSTEFFECTS)
			{
				objectType = "postEffect";
				countPerObject = size;
				post_effect_number_ = size;
			}
			else if (context_info_list[i] == RPR_CONTEXT_LIST_CREATED_HETEROVOLUMES)
			{
				objectType = "heteroVolume";
				countPerObject = size;
				volume_number_ = size;
			}
			else if (context_info_list[i] == RPR_CONTEXT_LIST_CREATED_GRIDS)
			{
				objectType = "grid";
				countPerObject = size;
				grid_number_ = size;
			}
			else if (context_info_list[i] == RPR_CONTEXT_LIST_CREATED_BUFFERS)
			{
				objectType = "buffer";
				countPerObject = size;
				buffer_number_ = size;
			}
			else if (context_info_list[i] == RPR_CONTEXT_LIST_CREATED_IMAGES)
			{
				objectType = "image";
				countPerObject = size;
				image_number_ = size;
			}
			else if (context_info_list[i] == RPR_CONTEXT_LIST_CREATED_FRAMEBUFFERS)
			{
				objectType = "framebuffer";
				countPerObject = size;
				framebuffer_number_ = size;
			}
			else if (context_info_list[i] == RPR_CONTEXT_LIST_CREATED_SCENES)
			{
				objectType = "scene";
				countPerObject = size;
				scene_number_ = size;
			}
			else if (context_info_list[i] == RPR_CONTEXT_LIST_CREATED_CURVES)
			{
				objectType = "curve";
				countPerObject = size;
				curve_number_ = size;
			}
			else if (context_info_list[i] == RPR_CONTEXT_LIST_CREATED_MATERIALSYSTEM)
			{
				objectType = "material system";
				countPerObject = size;
				material_node_number_ = size;
			}
			else if (context_info_list[i] == RPR_CONTEXT_LIST_CREATED_COMPOSITE)
			{
				objectType = "composite";
				countPerObject = size;
				composite_number_ = size;
			}
			else if (context_info_list[i] == RPR_CONTEXT_LIST_CREATED_LUT)
			{
				objectType = "lut";
				countPerObject = size;
				lut_number_ = size;
			}
			else
			{
				objectType = "unknown";
				countPerObject = size;
			}

			object_count[objectType] = countPerObject;
		}
	}

	return 0;
}

void HorusEngine::Init(int width, int height, const std::string& title, const std::string& SavePath)
{
	m_window_ = std::make_unique<HorusWindow>();
	m_window_->init_window(width, height, title);
}

void HorusEngine::InitContexts(int width, int height, HorusWindow* window)
{
	HorusOpenGL& OpenGL = HorusOpenGL::get_instance();
	HorusImGui& ImGui = HorusImGui::get_instance();
	HorusRadeon& Radeon = HorusRadeon::get_instance();
	HorusUI& UI = HorusUI::get_instance();
	HorusConsole& Console = HorusConsole::get_instance();

	UI.Init();
	OpenGL.init(width, height, window);
	ImGui.init(width, height, window);
	Radeon.init(width, height, window);
	
	spdlog::info("OpenGL, ImGui and Radeon contexts initialized.");
	Console.AddLog(" [info] OpenGL, ImGui and Radeon contexts initialized.");
}

void HorusEngine::PreRender()
{
	HorusOpenGL& OpenGL = HorusOpenGL::get_instance();
	HorusImGui& ImGui = HorusImGui::get_instance();
	HorusRadeon& Radeon = HorusRadeon::get_instance();
	
	m_IsClosing_ = get_is_closing();

	OpenGL.init_render();
	ImGui.init_render();
	Radeon.init_render();
}

void HorusEngine::Render()
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();
	HorusUI& UI = HorusUI::get_instance();
	HorusObjectManager& ObjectManager = HorusObjectManager::get_instance();

	//glm::mat4 model, view, projection; // Later for OpenGL

	Radeon.render_engine();
	//ObjectManager.UpdateCamera(ObjectManager.get_active_camera_id());
	//ObjectManager.GetMatrices(ObjectManager.get_active_camera_id(), model, view, projection);

	//glm::mat4 mvp = model * view * projection; // Later for OpenGL
	//glLoadMatrixf(glm::value_ptr(mvp)); // Later for OpenGL

	UI.RenderUI();
}

void HorusEngine::PostRender()
{
	HorusOpenGL& OpenGL = HorusOpenGL::get_instance();
	HorusImGui& ImGui = HorusImGui::get_instance();
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	ImGui.post_render();
	OpenGL.post_render();
	Radeon.post_render();

	//process_input();

	if (m_IsClosing_)
	{
		Close();
	}
}

int get_existing_file_suffix(std::string base_name, std::string extension)
{
	std::string regex_string = base_name + R"((_\d+)?)";
	std::regex regex(regex_string + extension + "$");

	int max_suffix = -1;
	for (auto& file : std::filesystem::directory_iterator("."))
	{
		if (std::filesystem::is_regular_file(file) && std::regex_match(file.path().filename().string(), regex))
		{
			std::string suffix_str = file.path().filename().string().substr(base_name.size());
			if (!suffix_str.empty())
			{
				int suffix = std::stoi(suffix_str.substr(1));
				if (suffix > max_suffix)
				{
					max_suffix = suffix;
				}
			}
		}
	}

	return max_suffix;
}

void edit_transform_with_gizmo(const float* view, const float* projection, float* matrix)
{
	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);

	ImGuiIO& io = ImGui::GetIO();
	bool isLeftAltPressed = io.KeyAlt;

	// radio button translation
	if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
	{
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	}

	// radio button rotation
	if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
	{
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	}

	// radio button scale
	if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
	{
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	}

	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
	ImGui::InputFloat3("Translation", matrixTranslation);
	ImGui::InputFloat3("Rotation", matrixRotation);
	ImGui::InputFloat3("Scale", matrixScale);
	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);

	if (mCurrentGizmoOperation != ImGuizmo::SCALE)
	{
		if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
		{
			mCurrentGizmoMode = ImGuizmo::LOCAL;
		}

		if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
		{
			mCurrentGizmoMode = ImGuizmo::WORLD;
		}
	}


	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::Manipulate(view, projection, mCurrentGizmoOperation, mCurrentGizmoMode, matrix, NULL, NULL);

}

void HorusEngine::Close()
{
	HorusOpenGL& OpenGL = HorusOpenGL::get_instance();
	HorusImGui& ImGui = HorusImGui::get_instance();
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	Radeon.quit_render();
	ImGui.quit_render();
	OpenGL.quit_render();

	m_IsRunning_ = false;
}

void HorusEngine::call_reset_buffer()
{
	HorusResetBuffers::get_instance().CallResetBuffers();
}



