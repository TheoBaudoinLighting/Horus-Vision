#include "hrs_engine.h"
#include "hrs_logger.h"

#include "imgui.h"
#include "imnodes.h"

#include <filesystem>
#include <regex>
#include <vector>
#include <chrono>

#include "ImGuizmo.h"
#include "objects/hrs_object_manager.h"

#include "L2DFileDialog.h"

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

void reset_buffer()
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	options_changed = true;
	Radeon.set_is_dirty(true);
	Radeon.set_sample_count(1);
	CHECK(rprFrameBufferClear(Radeon.get_frame_buffer()));
}

void process_move()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::get_instance();

	ImGuiIO& io = ImGui::GetIO();

	bool isCameraMove = false;

	bool isLeftAltPressed = io.KeyAlt;

	const float cameraSpeed = 0.1f;

	// Just boring for now, to put it back later, but all is good here
	/*std::vector<std::pair<int, std::function<void()>>> key_actions =
	{
			{ImGuiKey_Z, [&]() { ObjectManager.move_camera_forward(0); }},
			{ImGuiKey_S, [&]() { ObjectManager.move_camera_backward(0); }},
			{ImGuiKey_Q, [&]() { ObjectManager.move_camera_left(0); }},
			{ImGuiKey_D, [&]() { ObjectManager.move_camera_right(0); }},
			{ImGuiKey_A, [&]() { ObjectManager.move_camera_up(0); }},
			{ImGuiKey_E, [&]() { ObjectManager.move_camera_down(0); }}
	};

	for (const auto& key_action : key_actions)
	{
		if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(static_cast<ImGuiKey>(key_action.first))))
		{
			isCameraMove = true;
			key_action.second();
		}
	}*/

	if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_F)))
	{
		isCameraMove = true;
		RadeonProRender::float3 pivot = (0.f, 0.f, 0.f);

		ObjectManager.set_camera_lookat(0, pivot);
	}

	if (isLeftAltPressed)
	{
		if (ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow) && ImGui::IsAnyMouseDown())
		{
			ImGuiIO& io = ImGui::GetIO();

			if (ImGui::IsMouseDown(0)) // left mouse
			{
				isCameraMove = true;

				ImVec2 windowSize = ImGui::GetWindowSize();
				ImVec2 mouse_delta = ImGui::GetMouseDragDelta(0, 0);

				mouse_delta.x /= windowSize.x;
				mouse_delta.y /= windowSize.y;

				ObjectManager.tumble_camera(0, mouse_delta.x, mouse_delta.y);
				ImGui::ResetMouseDragDelta(0);
			}

			if (ImGui::IsMouseDown(1)) // right mouse
			{
				isCameraMove = true;
				ImVec2 mouse_delta = ImGui::GetMouseDragDelta(1, 0);
				ObjectManager.dolly_camera(0, cameraSpeed * mouse_delta.y);
				ImGui::ResetMouseDragDelta(1);
			}

			if (ImGui::IsMouseDown(2)) // middle mouse
			{
				isCameraMove = true;
				ImVec2 windowSize = ImGui::GetWindowSize();
				ImVec2 mouse_delta = ImGui::GetMouseDragDelta(2, 0);

				mouse_delta.x /= windowSize.x;
				mouse_delta.y /= windowSize.y;

				ObjectManager.track_camera(0, mouse_delta.x, mouse_delta.y);
				ImGui::ResetMouseDragDelta(2);
			}

			if (ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow) && io.MouseWheel != 0)
			{
				isCameraMove = true;
				float scroll_delta = io.MouseWheel;
				ObjectManager.scroll_camera(0, scroll_delta);
			}
		}
	}

	if (isCameraMove)
	{
		reset_buffer();
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

void HorusEngine::ui_init()
{
	if (show_winui_viewer)           ui_viewer(&show_winui_viewer);
	if (show_winui_property_editor)  ui_property_editor(&show_winui_property_editor);
	if (show_winui_outliner)         ui_outliner(&show_winui_outliner);
	if (show_winui_material_editor)  ui_material_editor(&show_winui_material_editor);
	if (show_winui_viewer_rt)        ui_viewer_rt(&show_winui_viewer_rt);
	//if (show_winui_main_menu_bar)    ui_main_menu_bar(&show_winui_main_menu_bar);

	//ImGui::ShowDemoWindow();

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New"))
			{

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
				m_is_closing_ = true;
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

			ImGui::MenuItem("Viewer", NULL, &show_winui_viewer);
			ImGui::MenuItem("Property Editor", NULL, &show_winui_property_editor);
			ImGui::MenuItem("Outliner", NULL, &show_winui_outliner);
			ImGui::MenuItem("Material Editor", NULL, &show_winui_material_editor);
			ImGui::MenuItem("RT parameters", NULL, &show_winui_viewer_rt);
			//ImGui::MenuItem("Main Menu Bar", NULL, &show_winui_main_menu_bar);


			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("About"))
			{

			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}


}

void HorusEngine::ui_viewer(bool* p_open)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	static bool isResizable = false;
	static ImVec2 lastSize = ImVec2(0, 0);
	static int customX = 800;
	static int customY = 600;
	static int lastCustomX = customX;
	static int lastCustomY = customY;
	static float offsetX_adjust = 0.0f;
	static float offsetY_adjust = 0.0f;

	ImVec2 available = ImGui::GetContentRegionAvail();
	float windowWidth = available.x;
	float windowHeight = available.y - ImGui::GetFrameHeightWithSpacing();

	static ImVec2 viewer_window_pos;
	static ImVec2 viewer_window_size;

	ImGuiIO& io = ImGui::GetIO();
	bool isLeftAltPressed = io.KeyAlt;

	ImGuiWindowFlags window_flags = 0;

	if (isLeftAltPressed) {
		window_flags |= ImGuiWindowFlags_NoMove;
	}

	const char* items[] = { "800x600", "1024x768", "1280x536", "1280x720", "1920x1080", "1920x803", "2048x858" };
	static int item_current = 0;

	if (ImGui::Begin("Viewer", p_open, ImGuiWindowFlags_MenuBar | window_flags))
	{
		if (ImGui::BeginMenuBar())
		{

			ImGui::SetNextItemWidth(120);
			ImGui::Text("Size : ");
			ImGui::SameLine();

			ImGui::SetNextItemWidth(120);
			ImGui::DragInt("", &customX);
			ImGui::SameLine();

			ImGui::SetNextItemWidth(10);
			ImGui::Text(" x ");
			ImGui::SameLine();

			ImGui::SetNextItemWidth(120);
			ImGui::DragInt("", &customY);
			ImGui::SameLine();

			ImGui::SetNextItemWidth(150);
			if (ImGui::Combo("Predefined Sizes", &item_current, items, IM_ARRAYSIZE(items)))
			{
				sscanf_s(items[item_current], "%dx%d", &customX, &customY);
			}

			ImGui::SameLine();

			ImGui::Checkbox("Resizable", &isResizable);

			ImGui::SameLine();

			if (ImGui::Button("Render Region"))
			{
				if (enable_render_region_)
				{
					enable_render_region_ = false;
				}
				else
				{
					enable_render_region_ = true;
				}
			}

			ImGui::SameLine();

			ImGui::SetNextItemWidth(20);
			if (ImGui::BeginCombo("##Options de Zone de Rendu", NULL, ImGuiComboFlags_NoArrowButton))
			{
				static float opacity = 0.5f;

				ImGui::SliderFloat("Opacity", &opacity, 0.0f, 1.0f, "%.1f");

				ImGui::EndCombo();
			}



			ImGui::SameLine();

			// progress bar
			float progress = Radeon.get_render_progress();
			int max_samples = Radeon.get_max_samples();
			int current_samples = static_cast<int>(progress / 100.0f * max_samples);
			ImGui::SetNextItemWidth(100);
			ImGui::Text("Progress : ");
			ImGui::SetNextItemWidth(250);
			char overlayText[32];
			sprintf_s(overlayText, "%d sur %d", current_samples, max_samples);
			ImGui::ProgressBar(progress / 100.0f, ImVec2(0.0f, 0.0f), overlayText);
			bool isRenderComplete = (progress >= 100.0f);
			ImGui::EndMenuBar();

			if (progress <= m_min_samples_ && !has_started)
			{
				start_time = std::chrono::high_resolution_clock::now();
				has_started = true;
				options_changed = false;
			}

			if (progress >= 99.f && has_started)
			{
				auto end_time = std::chrono::high_resolution_clock::now();
				auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

				total_seconds = duration_ms / 1000;
				hours = total_seconds / 3600;
				minutes = (total_seconds % 3600) / 60;
				seconds = total_seconds % 60;
				milliseconds = duration_ms % 1000;

				m_chrono_time_ = duration;
				has_started = false;
			}

			if (isRenderComplete)
			{
				char timeString[100];
				snprintf(timeString, sizeof(timeString), "Rendering finish in : %lldh %lldm %llds %lldms", hours, minutes, seconds, milliseconds);




				ImVec2 overlay_pos = ImVec2(
					viewer_window_pos.x + viewer_window_size.x / 2.0f,
					viewer_window_pos.y + viewer_window_size.y - 30.0f
				);

				ImGuiWindowFlags overlay_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

				ImGui::SetNextWindowPos(overlay_pos, ImGuiCond_Always, ImVec2(0.5f, 1.0f));

				ImGui::SetNextWindowBgAlpha(0.35f);
				if (ImGui::Begin("finish_render", NULL, overlay_flags))
				{
					ImGui::Separator();
					ImGui::Text("%s", timeString);
					ImGui::Separator();
				}
				ImGui::End();


			}
			last_progress = progress;
		}

		if (customX != lastCustomX || customY != lastCustomY)
		{
			Radeon.resize_render(customX, customY);
			lastCustomX = customX;
			lastCustomY = customY;
		}

		GLuint TextureID = Radeon.get_texture_buffer();
		ImVec2 m_viewer_size = ImGui::GetContentRegionAvail();

		if (isResizable)
		{
			if (m_viewer_size.x != lastSize.x || m_viewer_size.y != lastSize.y)
			{
				Radeon.resize_render(static_cast<int>(m_viewer_size.x), static_cast<int>(m_viewer_size.y));
				lastSize = m_viewer_size;
			}

			ImGui::Image((void*)(intptr_t)TextureID, ImVec2(m_viewer_size.x, m_viewer_size.y));
		}
		else
		{
			float aspect_ratio_image = static_cast<float>(customX) / static_cast<float>(customY);
			aspect_ratio_viewer = aspect_ratio_image;

			if (m_viewer_size.x / m_viewer_size.y > aspect_ratio_image)
			{
				img_size.y = m_viewer_size.y;
				img_size.x = m_viewer_size.y * aspect_ratio_image;
			}
			else
			{
				img_size.x = m_viewer_size.x;
				img_size.y = m_viewer_size.x / aspect_ratio_image;
			}

			float offsetX = (m_viewer_size.x - img_size.x - 60) * 0.5f;
			float offsetY = (m_viewer_size.y - img_size.y) * 0.5f;

			ImGui::SetCursorPos(ImVec2(offsetX + 30, offsetY + 60));

			ImGui::Image((void*)(intptr_t)TextureID, img_size);

			float middleX = offsetX + 30 + img_size.x / 2.0f;
			float middleY = offsetY + 60 + img_size.y / 2.0f;

			stored_image_position_ = ImVec2(middleX, middleY);
		}

		viewer_window_pos = ImGui::GetWindowPos();
		viewer_window_size = ImGui::GetWindowSize();
	}

	ImGui::End();

	process_move();
}

void HorusEngine::ui_end_rendering_overlay(bool* p_open)
{
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

	const float DISTANCE_PAD = 10.0f;
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImVec2 window_pos = ImVec2(viewport->WorkPos.x + viewport->WorkSize.x - DISTANCE_PAD, viewport->WorkPos.y + viewport->WorkSize.y - DISTANCE_PAD);
	ImVec2 window_pos_pivot = ImVec2(1.0f, 1.0f);
	window_pos = ImVec2(viewport->WorkPos.x + viewport->WorkSize.x - DISTANCE_PAD, viewport->WorkPos.y + viewport->WorkSize.y - DISTANCE_PAD);
	window_pos_pivot = ImVec2(1.0f, 1.0f);

	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	ImGui::SetNextWindowViewport(viewport->ID);
	window_flags |= ImGuiWindowFlags_NoMove;




	if (ImGui::Begin("render_overlay", p_open, window_flags))
	{
		ImGui::Text("Render finish in : (wip)");
		ImGui::Separator();
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("Fermer", NULL, p_open))
				*p_open = false;
			ImGui::EndPopup();
		}
	}
	ImGui::End();
}

void HorusEngine::ui_property_editor(bool* p_open)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();
	HorusObjectManager& ObjectManager = HorusObjectManager::get_instance();

	m_ResetBuffer_ = false;

	static char* file_dialog_buffer = nullptr;
	static char path[500] = "";
	static std::string last_used_path;
	static bool was_backdrop_image_enabled = false;
	static bool just_disabled_backdrop = false;

	GPU00N[0] = '\0';
	GPU00N[1] = '\0';

	rprContextGetInfo(Radeon.get_context(), RPR_CONTEXT_GPU0_NAME, sizeof(GPU00N), GPU00N, nullptr);
	rprContextGetInfo(Radeon.get_context(), RPR_CONTEXT_GPU1_NAME, sizeof(GPU01N), GPU01N, nullptr);
	rprContextGetInfo(Radeon.get_context(), RPR_CONTEXT_RENDER_STATISTICS, sizeof(render_statistics_), &render_statistics_, 0);

	ImGui::Begin("Property Panel", p_open);

	// gpu name
	rpr_longlong memory_usage = render_statistics_.gpumem_usage / 1024 / 1024;
	rpr_longlong system_memory_usage = render_statistics_.sysmem_usage / 1024 / 1024;
	rpr_longlong gpu_max_allocation = render_statistics_.gpumem_max_allocation / 1024 / 1024;
	rpr_longlong gpu_total = render_statistics_.gpumem_total / 1024 / 1024; // 

	ImGui::Separator();

	if (ImGui::CollapsingHeader("Render Info"))
	{
		ImGui::Text("GPU name: %s", GPU00N); // %s is a placeholder for a string
		ImGui::Text("GPU name: %s", GPU01N);
		ImGui::Separator();
		ImGui::Text("Samples: %d", Radeon.get_sample_count()); // %d is a placeholder for an integer
		ImGui::Separator();
		ImGui::Text("System memory usage : %d MB", system_memory_usage);
		ImGui::Text("GPU Memory usage : %d MB", memory_usage);
		ImGui::Text("GPU max allocation : %d MB", gpu_max_allocation);
		ImGui::Text("GPU total : %d MB", gpu_total);
	}

	ImGui::Separator();

	const char* renderModes[] = {
		"Diffuse only lighting",
		"Normal GI lighting",
		"Only direct lighting",
		"Direct lighting with no shadowing",
		"White objects with black wireframe",
		"Output only Material indices",
		"Output P positions for each pixel hit",
		"Output normal values",
		"Output UV coordinates",
		"Output an AO render"
	};
	static int selectedRenderMode = 1;

	ImGui::Text("Select render mode :");
	if (ImGui::Combo("Render mode", &selectedRenderMode, renderModes, IM_ARRAYSIZE(renderModes)))
	{
		switch (selectedRenderMode)
		{
		case 0:
			rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_RENDER_MODE, RPR_RENDER_MODE_DIFFUSE);
			break;
		case 1:
			rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_RENDER_MODE, RPR_RENDER_MODE_GLOBAL_ILLUMINATION);
			break;
		case 2:
			rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_RENDER_MODE, RPR_RENDER_MODE_DIRECT_ILLUMINATION);
			break;
		case 3:
			rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_RENDER_MODE, RPR_RENDER_MODE_DIRECT_ILLUMINATION_NO_SHADOW);
			break;
		case 4:
			rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_RENDER_MODE, RPR_RENDER_MODE_WIREFRAME);
			break;
		case 5:
			rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_RENDER_MODE, RPR_RENDER_MODE_MATERIAL_INDEX);
			break;
		case 6:
			rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_RENDER_MODE, RPR_RENDER_MODE_POSITION);
			break;
		case 7:
			rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_RENDER_MODE, RPR_RENDER_MODE_NORMAL);
			break;
		case 8:
			rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_RENDER_MODE, RPR_RENDER_MODE_TEXCOORD);
			break;
		case 9:
			rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_RENDER_MODE, RPR_RENDER_MODE_AMBIENT_OCCLUSION);
			break;
		}
		reset_buffer();
	}

	ImGui::Separator();

	ImGui::TextUnformatted("Path: ");
	ImGui::InputText("##path", path, sizeof(path));
	ImGui::SameLine();
	if (ImGui::Button("Browse##path"))
	{
		file_dialog_buffer = path;
		FileDialog::file_dialog_open = true;
		FileDialog::file_dialog_open_type = FileDialog::FileDialogType::OpenFile;
	}

	if (FileDialog::file_dialog_open)
	{
		FileDialog::ShowFileDialog(&FileDialog::file_dialog_open, file_dialog_buffer, sizeof(file_dialog_buffer), FileDialog::file_dialog_open_type);
	}

	// Backdrop image
	bool checkboxbeckdropChanged = ImGui::Checkbox("Background image", &enable_backdrop_image_);

	if (checkboxbeckdropChanged && (enable_backdrop_image_ != previous_enable_backdrop_image_))
	{
		m_ResetBuffer_ = true;
		previous_enable_backdrop_image_ = enable_backdrop_image_;
	}

	if (enable_backdrop_image_ == true)
	{
		if (path != last_used_path || just_disabled_backdrop)
		{
			ObjectManager.set_background_image(path);
			last_used_path = path;
			just_disabled_backdrop = false;
			reset_buffer();
		}
		was_backdrop_image_enabled = true;
	}
	else if (was_backdrop_image_enabled)
	{
		ObjectManager.unset_background_image();
		was_backdrop_image_enabled = false;
		just_disabled_backdrop = true;
		reset_buffer();
	}

	if (was_backdrop_image_enabled)
	{
		ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "Backdrop image is enabled");
	}
	else
	{
		ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.7f, 1.0f), "Backdrop image is disabled");
	}

	static bool was_transparent_background_enabled = false;

	bool checkboxTransparentBackgroundChanged = ImGui::Checkbox("Transparent background", &enable_transparent_background);

	if (checkboxTransparentBackgroundChanged)
	{
		if (enable_transparent_background && !was_transparent_background_enabled)
		{
			rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_TRANSPARENT_BACKGROUND, 1);
			spdlog::info("Transparent background enabled");
			was_transparent_background_enabled = true;
			m_ResetBuffer_ = true;
		}
		else if (!enable_transparent_background && was_transparent_background_enabled)
		{
			rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_TRANSPARENT_BACKGROUND, 0);
			spdlog::info("Transparent background disabled");
			was_transparent_background_enabled = false;
			m_ResetBuffer_ = true;
		}
	}

	if (was_transparent_background_enabled)
	{
		ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "Transparent background is enabled");
	}
	else
	{
		ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.7f, 1.0f), "Transparent background is disabled");
	}

	ImGui::Separator();

	if (ImGui::CollapsingHeader("Render settings", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginTabBar("rnds_settings"))
		{
			if (ImGui::BeginTabItem("Sampling"))
			{
				// Sampling parameters

				// Interactive render ?
				// Progressive passes ?

				// min and max samples set with slider and text input

				if (ImGui::CollapsingHeader("Sampling", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::Text("Min samples: %d", m_min_samples_);
					ImGui::SameLine();
					if (ImGui::SliderInt("Min samples", &m_min_samples_, 1, 99))
					{

						if (m_min_samples_ > m_max_samples_)
						{
							m_min_samples_ = m_max_samples_;
						}

						Radeon.set_min_samples(m_min_samples_);
						reset_buffer();
					}

					ImGui::Text("Max samples: %d", m_max_samples_);
					ImGui::SameLine();
					if (ImGui::SliderInt("Max samples", &m_max_samples_, 1, 1024))
					{
						if (m_max_samples_ < m_min_samples_)
						{
							m_max_samples_ = m_min_samples_;
						}

						Radeon.set_max_samples(m_max_samples_);
						reset_buffer();
					}

					ImGui::Separator();

					// Russian roulette
					static bool rrParametersChanged = false;

					bool checkboxRRChanged = ImGui::Checkbox("Russian Roulette", &enable_russian_roulette_);

					if (checkboxRRChanged && (enable_russian_roulette_ != previous_enable_russian_roulette_))
					{
						m_ResetBuffer_ = true;
						previous_enable_russian_roulette_ = enable_russian_roulette_;
						rrParametersChanged = true;
					}

					if (enable_russian_roulette_)
					{
						ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "Russian roulette is enabled");

						static float russianRouletteDepth = 5;

						if (ImGui::SliderFloat("Depth", &russianRouletteDepth, 1.0f, 20.0f))  // Min 1.0, Max 20.0
						{
							rprContextSetParameterByKey1f(Radeon.get_context(), RPR_CONTEXT_RUSSIAN_ROULETTE_DEPTH, russianRouletteDepth);
							rrParametersChanged = true;
						}

					}
					else
					{
						ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.7f, 1.0f), "Russian roulette is disabled");
					}

					if (rrParametersChanged)
					{
						reset_buffer();
						rrParametersChanged = false;
					}

					ImGui::Separator();

					static bool parametersChanged = false;

					bool checkboxadsChanged = ImGui::Checkbox("Enable adaptive sampling", &enable_adaptive_sampling_);

					if (checkboxadsChanged && (enable_adaptive_sampling_ != previous_enable_adaptive_sampling_))
					{
						m_ResetBuffer_ = true;
						previous_enable_adaptive_sampling_ = enable_adaptive_sampling_;
						parametersChanged = true;
					}

					if (enable_adaptive_sampling_)
					{
						ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "Adaptive sampling is enabled");
					}
					else
					{
						ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.7f, 1.0f), "Adaptive sampling is disabled");
					}

					if (parametersChanged)
					{
						if (enable_adaptive_sampling_ == true)
						{
							rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_ADAPTIVE_SAMPLING_TILE_SIZE, 16);
							rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_ADAPTIVE_SAMPLING_MIN_SPP, m_min_samples_);
							rprContextSetParameterByKey1f(Radeon.get_context(), RPR_CONTEXT_ADAPTIVE_SAMPLING_THRESHOLD, m_adaptive_threshold_);
						}
						else
						{
							rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_ADAPTIVE_SAMPLING_TILE_SIZE, 0);
							rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_ADAPTIVE_SAMPLING_MIN_SPP, 0);
							rprContextSetParameterByKey1f(Radeon.get_context(), RPR_CONTEXT_ADAPTIVE_SAMPLING_THRESHOLD, 0.0f);
						}

						parametersChanged = false; 
					}

					ImGui::Text("Threshold: %.2f", m_adaptive_threshold_);
					ImGui::SameLine();
					if (ImGui::SliderFloat("Threshold", &m_adaptive_threshold_, 0.0f, 1.0f))
					{
						reset_buffer();
						parametersChanged = true;
					}

					ImGui::Separator();
				}

				ImGui::Separator();

				// checkbox for enable preview mode
				static bool previewParametersChanged = false;

				bool checkboxChanged = ImGui::Checkbox("Enable preview", &enable_preview_mode_);

				if (checkboxChanged && (enable_preview_mode_ != previous_reset_mode_))
				{
					m_ResetBuffer_ = true;
					previous_reset_mode_ = enable_preview_mode_;
					previewParametersChanged = true;
				}

				if (enable_preview_mode_)
				{
					ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "Preview mode is enabled");
				}
				else
				{
					ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.7f, 1.0f), "Preview mode is disabled");
				}

				if (previewParametersChanged)
				{
					if (enable_preview_mode_ == true)
					{
						rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_PREVIEW, 1);
					}
					else
					{
						rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_PREVIEW, 0);
					}

					previewParametersChanged = false;
				}

				ImGui::Separator();

				// Anti Aliasing
				static bool aaParametersChanged = false;

				bool checkboxAAChanged = ImGui::Checkbox("Enable AA", &enable_AA_);

				if (checkboxAAChanged)
				{
					m_ResetBuffer_ = true;
					previous_enable_AA_ = enable_AA_;
					aaParametersChanged = true;
				}

				if (enable_AA_)
				{
					ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "Anti aliasing is enabled"); // green

				}
				else
				{
					ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.7f, 1.0f), "Anti aliasing is disabled"); // pink

				}

				if (aaParametersChanged)
				{
					if (enable_AA_)
					{
						rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_AA_ENABLED, 1);
					}
					else
					{
						rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_AA_ENABLED, 0);
					}

					aaParametersChanged = false;
				}

				const char* filters[] = { "Box", "Triangle", "Gaussian", "Mitchell", "Lanczos", "Blackmanharris", "None" };
				static int selectedFilter = 5; // Gaussian by default

				ImGui::Text("Select filter :");
				if (ImGui::Combo("Filter", &selectedFilter, filters, IM_ARRAYSIZE(filters)))
				{
					switch (selectedFilter)
					{
					case 0:
						rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_IMAGE_FILTER_TYPE, RPR_FILTER_BOX);
						break;
					case 1:
						rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_IMAGE_FILTER_TYPE, RPR_FILTER_TRIANGLE);
						break;
					case 2:
						rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_IMAGE_FILTER_TYPE, RPR_FILTER_GAUSSIAN);
						break;
					case 3:
						rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_IMAGE_FILTER_TYPE, RPR_FILTER_MITCHELL);
						break;
					case 4:
						rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_IMAGE_FILTER_TYPE, RPR_FILTER_LANCZOS);
						break;
					case 5:
						rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_IMAGE_FILTER_TYPE, RPR_FILTER_BLACKMANHARRIS);
						break;
					case 6:
						rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_IMAGE_FILTER_TYPE, RPR_FILTER_NONE);
						break;
					}
					reset_buffer();
				}

				ImGui::Separator();


				// collapsing header for change sampler type

				/*bool showSamplerType = false;

				if (ImGui::CollapsingHeader("Sampler Type"))
				{
					showSamplerType = true;
				}

				if (showSamplerType)
				{
					ImGui::RadioButton("Sobol", &samplerType, RPR_CONTEXT_SAMPLER_TYPE_SOBOL);
					ImGui::RadioButton("Random", &samplerType, RPR_CONTEXT_SAMPLER_TYPE_RANDOM);
					ImGui::RadioButton("CMJ", &samplerType, RPR_CONTEXT_SAMPLER_TYPE_CMJ);

					rprContextSetParameterByKey1u(context, RPR_CONTEXT_SAMPLER_TYPE, samplerType);
				}

				ImGui::Separator();*/


				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Globals"))
			{
				// Trace depth
				if (ImGui::CollapsingHeader("Trace Depth", ImGuiTreeNodeFlags_DefaultOpen))
				{
					static int maxDepthDiffuse = 1;
					if (ImGui::SliderInt("Diffuse Ray Depth", &maxDepthDiffuse, 1, 64))
					{
						rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_MAX_DEPTH_DIFFUSE, maxDepthDiffuse);
						reset_buffer();
					}

					static int maxDepthGlossy = 1;
					if (ImGui::SliderInt("Reflection Ray Depth", &maxDepthGlossy, 1, 64))
					{
						rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_MAX_DEPTH_GLOSSY, maxDepthGlossy);
						reset_buffer();
					}

					static int maxDepthRefraction = 1;
					if (ImGui::SliderInt("Refraction Ray Depth", &maxDepthRefraction, 1, 64))
					{
						rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_MAX_DEPTH_REFRACTION, maxDepthRefraction);
						reset_buffer();
					}

					static int maxDepthGlossyRefraction = 1;
					if (ImGui::SliderInt("Glossy Refraction Ray Depth", &maxDepthGlossyRefraction, 1, 64))
					{
						rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_MAX_DEPTH_GLOSSY_REFRACTION, maxDepthGlossyRefraction);
						reset_buffer();
					}

					static int maxDepthShadow = 1;
					if (ImGui::SliderInt("Shadow Ray Depth", &maxDepthShadow, 1, 64))
					{
						rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_MAX_DEPTH_SHADOW, maxDepthShadow);
						reset_buffer();
					}


				}

				// Color management

				if (ImGui::CollapsingHeader("Color Management", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::Text("Color Space");


				}

				// Options

				if (ImGui::CollapsingHeader("Options", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::Text("Color Space");


				}

				// Overrides

				if (ImGui::CollapsingHeader("Override", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::Text("Color Space");


				}

				// material override

				if (ImGui::CollapsingHeader("Material Override", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::Text("Color Space");


				}

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Motion blur"))
			{
				if (ImGui::CollapsingHeader("Motion Blur", ImGuiTreeNodeFlags_DefaultOpen))
				{


				}

				if (ImGui::CollapsingHeader("Shutter", ImGuiTreeNodeFlags_DefaultOpen))
				{


				}

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("GI"))
			{
				if (ImGui::CollapsingHeader("GI", ImGuiTreeNodeFlags_DefaultOpen))
				{
					static int maxRecursion = 4;

					if (ImGui::SliderInt("Trace Depth", &maxRecursion, 1, 64))
					{
						rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_MAX_RECURSION, maxRecursion);
						reset_buffer();
					}
				}

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Caustics"))
			{
				if (ImGui::CollapsingHeader("Caustics", ImGuiTreeNodeFlags_DefaultOpen))
				{



				}

				if (ImGui::CollapsingHeader("Trace Depth", ImGuiTreeNodeFlags_DefaultOpen))
				{



				}

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("AOV"))
			{
				if (ImGui::CollapsingHeader("AOVs", ImGuiTreeNodeFlags_DefaultOpen))
				{



				}

				if (ImGui::CollapsingHeader("Active AOVs", ImGuiTreeNodeFlags_DefaultOpen))
				{



				}

				if (ImGui::CollapsingHeader("Processing", ImGuiTreeNodeFlags_DefaultOpen))
				{
					if (ImGui::CollapsingHeader("Deep Output", ImGuiTreeNodeFlags_DefaultOpen))
					{


					}

				}

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Optimizations"))
			{
				// SSS
				if (ImGui::CollapsingHeader("Sub-Surface Scattering", ImGuiTreeNodeFlags_DefaultOpen))
				{


				}

				// Cut-off thresholds
				if (ImGui::CollapsingHeader("Cut-off Thresholds", ImGuiTreeNodeFlags_DefaultOpen))
				{


				}

				// Ray tracing acceleration
				if (ImGui::CollapsingHeader("Ray Tracing Acceleration", ImGuiTreeNodeFlags_DefaultOpen))
				{

				}

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("System"))
			{
				// Log and feedback
				if (ImGui::CollapsingHeader("Log and Feedback", ImGuiTreeNodeFlags_DefaultOpen))
				{



				}

				// Bucket rendering
				if (ImGui::CollapsingHeader("Bucket Rendering", ImGuiTreeNodeFlags_DefaultOpen))
				{

				}

				// Units
				if (ImGui::CollapsingHeader("Units", ImGuiTreeNodeFlags_DefaultOpen))
				{

				}

				// Shadow Linking
				if (ImGui::CollapsingHeader("Shadow Linking", ImGuiTreeNodeFlags_DefaultOpen))
				{

				}

				// Memory
				if (ImGui::CollapsingHeader("Memory", ImGuiTreeNodeFlags_DefaultOpen))
				{
					/*static float rayCastEpsilon = 0.002f;
					if (ImGui::SliderFloat("Ray Cast Epsilon", &rayCastEpsilon, 0.0f, 1.0f, "%.5f"))
					{
						rprContextSetParameterByKey1f(context, RPR_CONTEXT_RAY_CAST_EPISLON, rayCastEpsilon);
						reset_buffer();
					}*/

					static int cpuThreadLimit = 4;
					if (ImGui::SliderInt("CPU Thread limit", &cpuThreadLimit, 1, 16))
					{
						rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_CPU_THREAD_LIMIT, cpuThreadLimit);
						reset_buffer();
					}

					static int gpuMemoryLimitMB = 1024;
					if (ImGui::SliderInt("GPU Thread limit (MB)", &gpuMemoryLimitMB, 256, 16384))
					{
						rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_GPU_MEMORY_LIMIT, gpuMemoryLimitMB * 1024 * 1024);
						reset_buffer();
					}
				}

				// Global preferences
				if (ImGui::CollapsingHeader("Global Preferences", ImGuiTreeNodeFlags_DefaultOpen))
				{

				}

				// Compute devices
				if (ImGui::CollapsingHeader("Compute Devices", ImGuiTreeNodeFlags_DefaultOpen))
				{

				}

				ImGui::EndTabItem();
			}

		}
		ImGui::EndTabBar();
	}



	if (ImGui::CollapsingHeader("Export render"))
	{
		// Export render logic
		static int export_format = 0;
		rpr_char const* formats[] = { ".png", ".jpeg", ".tiff", ".bmp", ".exr", ".hdr", ".tga" };
		const int numFormats = sizeof(formats) / sizeof(formats[0]);

		std::string baseFilename = "HorusRender_ExportedFile";
		std::string filename = baseFilename + std::string(formats[export_format]);

		if (ImGui::BeginCombo("Export format", formats[export_format]))
		{
			for (int i = 0; i < numFormats; i++)
			{
				bool isSelected = (export_format == i);
				if (ImGui::Selectable(formats[i], isSelected))
				{
					export_format = i;
				}
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		ImGui::InputText("Filename", userInput, 256);
		if (ImGui::Button("Export"))
		{
			if (strlen(userInput) > 0)
			{
				if (std::filesystem::exists(userInput + std::string(formats[export_format])))
				{
					suffix++;
					filename = userInput + ("_" + std::to_string(suffix) + "_" + std::to_string(Radeon.get_sample_count()) + std::string(formats[export_format]));
					spdlog::warn("File already exists, renaming to: {}", filename);
				}
				else
				{
					suffix = 0;
					filename = userInput + ("_" + std::to_string(Radeon.get_sample_count()) + std::string(formats[export_format]));
					spdlog::info("Render exported with success: {}", filename);
				}
			}
			else
			{
				if (std::filesystem::exists(baseFilename + std::string(formats[export_format])))
				{
					suffix++;
					filename = baseFilename + ("_" + std::to_string(suffix) + std::to_string(Radeon.get_sample_count()) + std::string(formats[export_format]));
					spdlog::warn("File already exists, renaming to: {}", filename);
				}
				else
				{
					suffix = 0;
					filename = baseFilename + ("_" + std::to_string(Radeon.get_sample_count()) + std::string(formats[export_format]));
					spdlog::info("Render exported with success: {}", filename);
				}
			}

			rpr_int status = rprFrameBufferSaveToFile(Radeon.get_frame_buffer_resolved(), filename.c_str());

			if (status != RPR_SUCCESS)
			{
				spdlog::error("Failed to save frame buffer to file: {}", filename);
			}
		}



		// Get object logic
		/*ImGui::Text("Get scene components");
		ImGui::Separator();
		ImGui::Text("Camera         : %d", camera_number_);
		ImGui::Separator();
		ImGui::Text("Light          : %d", light_number_);
		ImGui::Separator();
		ImGui::Text("Mesh           : %d", mesh_number_);
		ImGui::Separator();
		ImGui::Text("Grid           : %d", grid_number_);
		ImGui::Separator();
		ImGui::Text("Buffer         : %d", buffer_number_);
		ImGui::Separator();
		ImGui::Text("Image          : %d", image_number_);
		ImGui::Separator();
		ImGui::Text("FrameBuffer    : %d", framebuffer_number_);
		ImGui::Separator();
		ImGui::Text("Scene          : %d", scene_number_);
		ImGui::Separator();
		ImGui::Text("Curve          : %d", curve_number_);
		ImGui::Separator();
		ImGui::Text("Material Sys   : %d", material_node_number_);
		ImGui::Separator();
		ImGui::Text("Composite      : %d", composite_number_);
		ImGui::Separator();
		ImGui::Text("LUT            : %d", lut_number_);
		ImGui::Separator();*/


	}

	ImGui::Separator();

	ImGui::End();

	if (m_ResetBuffer_)
	{
		reset_buffer();

		m_ResetBuffer_ = false;
	}
}

void HorusEngine::ui_outliner(bool* p_open)
{
	HorusObjectManager& ObjectManager = HorusObjectManager::get_instance();

	ImGuizmo::BeginFrame();
	{
		ImGui::Begin("Outliner", p_open);
		ImGui::Text("Outliner");

		// toggle button

		std::vector<std::string> meshes;
		ObjectManager.get_outliner_meshes(meshes);

		std::vector<std::string> materials;
		ObjectManager.get_outliner_materials(materials);

		std::vector<std::string> cameras;
		ObjectManager.get_outliner_cameras(cameras);

		ImGui::Separator();

		if (ImGui::TreeNode("Meshes"))
		{
			for (const auto& mesh : meshes)
			{
				bool isSelected = (mesh == selectedMesh);
				ImGui::PushStyleColor(ImGuiCol_Text, isSelected ? IM_COL32(255, 0, 0, 255) : IM_COL32(255, 255, 255, 255));
				if (ImGui::Selectable(mesh.c_str(), isSelected))
				{
					selectedMesh = mesh;
					spdlog::info("Mesh selected: {}", mesh);

				}
				ImGui::PopStyleColor();
			}
			ImGui::TreePop();
		}


		//if (ImGui::TreeNode("Meshes"))
		//{
		//    for (const auto& mesh : meshes)
		//    {
		//        bool isSelected = (mesh == selectedMesh);
		//        if (ImGui::Selectable(mesh.c_str(), isSelected))
		//        {
		//            selectedMesh = mesh;
		//            spdlog::info("Mesh selected: {}", mesh);
		//        }
		//    }
		//    ImGui::TreePop();
		//}

		ImGui::Separator();

		if (ImGui::TreeNode("Materials"))
		{
			for (const auto& material : materials)
			{
				bool isSelected = (material == selectedMaterial);
				if (ImGui::Selectable(material.c_str(), isSelected))
				{
					selectedMaterial = material;
					spdlog::info("Material selected: {}", material);
				}
			}
			ImGui::TreePop();
		}

		ImGui::Separator();

		if (ImGui::TreeNode("Cameras"))
		{
			for (const auto& camera : cameras)
			{
				bool isSelected = (camera == selectedCamera);
				if (ImGui::Selectable(camera.c_str(), isSelected))
				{
					selectedCamera = camera;
					spdlog::info("Camera selected: {}", camera);
				}
			}
			ImGui::TreePop();
		}

		// Transform section

		if (ImGui::CollapsingHeader("Transforms", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Transforms");

			float viewMatrix[16];
			float projectionMatrix[16];

			auto io = ImGui::GetIO();

			ObjectManager.compute_view_projection_matrix(0, viewMatrix, projectionMatrix, io.DisplaySize.x / io.DisplaySize.y);

			RadeonProRender::matrix viewMatrixRPR = ObjectManager.get_mesh_transform(0);

			edit_transform_with_gizmo(viewMatrix, projectionMatrix, (float*)&viewMatrix);




			ImGuizmo::Manipulate(viewMatrix, projectionMatrix, ImGuizmo::TRANSLATE, ImGuizmo::WORLD, (float*)&viewMatrixRPR);

			if (memcmp(&viewMatrix, &viewMatrixRPR, sizeof(float) * 16) != 0)
			{
				//ObjectManager.set_mesh_transform(0, viewMatrixRPR);
			}


		}




		ImGui::Separator();

		ImGui::End();

	}
}

void HorusEngine::ui_material_editor(bool* p_open)
{
	HorusObjectManager& ObjectManager = HorusObjectManager::get_instance();

	//----------------------------------------------------------------------------------------------------------------------



	ObjectManager.show_material_editor(ObjectManager.get_material_editor_to_show());

	ObjectManager.open_material_editor_browser();

	//----------------------------------------------------------------------------------------------------------------------

	//ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);

	//if (!ImGui::Begin("Material editor", p_open))
	//{

	//	ImGui::End();
	//	return;
	//}

	//if (ImGui::BeginMenuBar())
	//{
	//	if (ImGui::BeginMenu("File"))
	//	{
	//		if (ImGui::MenuItem("Ouvrir...")) { /* Fonctionnalit d'ouverture */ }
	//		if (ImGui::MenuItem("Save")) { /* Fonctionnalit d'enregistrement */ }
	//		if (ImGui::MenuItem("Quit")) { *p_open = false; }
	//		ImGui::EndMenu();
	//	}

	//	ImGui::EndMenuBar();
	//}


	//ImGuiID dockspace_id = ImGui::GetID("MaterialEditorDockSpace");
	//ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);


	//if (ImGui::Begin("Create", p_open, ImGuiWindowFlags_NoCollapse))
	//{

	//	std::vector<std::string> m_node_names_ = { "Add", "Blend", "Multiply" };
	//	std::vector<const char*> m_node_names_cstr;
	//	for (const auto& str : m_node_names_) {
	//		m_node_names_cstr.push_back(str.c_str());
	//	}

	//	int selectedItem = -1;


	//	if (ImGui::ListBox("Create node", &selectedItem, m_node_names_cstr.data(), m_node_names_cstr.size()))
	//	{
	//		if (selectedItem != -1)
	//		{
	//			std::cout << "Selected item: " << m_node_names_[selectedItem] << std::endl;

	//			HorusNodeType type = determineNodeTypeBasedOnSelection(selectedItem);
	//			//g_material_editor.add_node(1, type);
	//		}
	//	}








	//	ImGui::End();
	//}

	//if (ImGui::Begin("Browser", p_open, ImGuiWindowFlags_NoCollapse))
	//{

	//	ImGui::Text("Navigateur de matriaux ici.");
	//	ImGui::End();
	//}


	//if (ImGui::Begin("Node Editor", p_open, ImGuiWindowFlags_NoCollapse))
	//{
	//	g_material_editor.update();
	//	ImGui::End();
	//}

	//ImGui::End();
}

void HorusEngine::ui_viewer_rt(bool* p_open)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	//ImGui::ShowDemoWindow(p_open);

	int fps = ImGui::GetIO().Framerate;

	ImGui::Begin("Realtime parameters view", p_open);

	ImGui::Text("Samples: %d", Radeon.get_sample_count());

	if (samples_mkr.size() > 100)
	{
		for (int i = 1; i < samples_mkr.size(); i++)
		{
			samples_mkr[i - 1] = samples_mkr[i];
		}
		samples_mkr[samples_mkr.size() - 1] = Radeon.get_sample_count();
	}
	else
	{
		samples_mkr.push_back(Radeon.get_sample_count());
	}

	ImGui::PlotLines("Samples", samples_mkr.data(), samples_mkr.size(), 0, nullptr, 0.0f, 200.0f, ImVec2(0, 80));


	// fps plot line
	ImGui::Text("FPS: %d", fps);

	// get frames
	if (frame.size() > 100)
	{
		for (int i = 1; i < frame.size(); i++)
		{
			frame[i - 1] = frame[i];
		}
		frame[frame.size() - 1] = fps;
	}
	else
	{
		frame.push_back(fps);
	}

	ImGui::PlotLines("FPS", frame.data(), frame.size(), 0, nullptr, 0.0f, 200.0f, ImVec2(0, 80));



	ImGui::End();
}

void HorusEngine::call_reset_buffer()
{
	reset_buffer();
}



