#include "hrs_viewport.h"
#include <hrs_object_manager.h>

#include <iostream>
using namespace std;

//---------------------------------------RADEON SECTION---------------------------------------//


void HorusViewportRadeon::ViewportRadeon(bool* p_open)
{
	HorusRadeon& Radeon = HorusRadeon::get_instance();

	ImVec4 GreenColor = ImVec4(0.6f, 1.0f, 0.6f, 1.0f);
	ImVec4 PinkColor = ImVec4(1.0f, 0.6f, 0.7f, 1.0f);

	static bool IsZooming = false;
	static bool use_text_color_for_tint = false;
	float zoom = 4.0f;

	static bool isResizable = false;
	static ImVec2 lastSize = ImVec2(0, 0);
	static int customX = 800;
	static int customY = 600;
	static int lastCustomX = customX;
	static int lastCustomY = customY;
	static float offsetX_adjust = 0.0f;
	static float offsetY_adjust = 0.0f;
	float WidthPerItems = 100;

	int BottomMenu = 30;

	int ElementsNumber = 8; // number of ImGui elements in my viewport

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

	if (ImGui::Begin("Viewer", p_open, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | window_flags))
	{
		ImVec2 available = ImGui::GetContentRegionAvail();

		float AvailableWidth = ImGui::GetContentRegionAvail().x;
		float AvailableHeight = ImGui::GetContentRegionAvail().y;
		WidthPerItems = AvailableWidth / ElementsNumber;
		float windowWidth = available.x;
		float windowHeight = available.y - ImGui::GetFrameHeightWithSpacing();


		if (ImGui::BeginMenuBar())
		{

			ImGui::SetNextItemWidth(WidthPerItems);
			ImGui::Text("Size : ");
			ImGui::SameLine();

			ImGui::SetNextItemWidth(WidthPerItems);
			ImGui::DragInt("", &customX);
			ImGui::SameLine();

			ImGui::SetNextItemWidth(WidthPerItems);
			ImGui::Text(" x ");
			ImGui::SameLine();

			ImGui::SetNextItemWidth(WidthPerItems);
			ImGui::DragInt("", &customY);
			ImGui::SameLine();

			ImGui::SetNextItemWidth(WidthPerItems);
			if (ImGui::Combo("Predefined Sizes", &item_current, items, IM_ARRAYSIZE(items)))
			{
				sscanf_s(items[item_current], "%dx%d", &customX, &customY);
			}

			ImGui::SameLine();

			ImGui::Checkbox("Resizable", &isResizable);

			ImGui::SameLine();

			ImGui::Checkbox("Zooming", &IsZooming);

			ImGui::SameLine();

			/*if (ImGui::Button("Render Region"))
			{
				if (m_EnableRenderRegion_)
				{
					m_EnableRenderRegion_ = false;
				}
				else
				{
					m_EnableRenderRegion_ = true;
				}
			}

			ImGui::SameLine();*/

			ImGui::SetNextItemWidth(WidthPerItems);
			if (ImGui::BeginCombo("##Options de Zone de Rendu", NULL, ImGuiComboFlags_NoArrowButton))
			{
				static float opacity = 0.5f;

				ImGui::SliderFloat("Opacity", &opacity, 0.0f, 1.0f, "%.1f");

				ImGui::EndCombo();
			}



			//ImGui::SameLine();

			ImGui::EndMenuBar();
		}

		// Simple security
		if (m_IsFirstLaunch)
		{
			Radeon.resize_render(customX, customY);
			lastCustomX = customX;
			lastCustomY = customY;

			m_IsFirstLaunch = false;
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
			m_AspectRationRender_ = aspect_ratio_image;

			if (m_viewer_size.x / m_viewer_size.y > aspect_ratio_image)
			{
				m_ImageSize_.y = m_viewer_size.y;
				m_ImageSize_.x = m_viewer_size.y * aspect_ratio_image;
			}
			else
			{
				m_ImageSize_.x = m_viewer_size.x;
				m_ImageSize_.y = m_viewer_size.x / aspect_ratio_image;
			}

			float offsetTop = -25;
			float offsetDown = -25;

			float offsetX = (m_viewer_size.x - m_ImageSize_.x - 60) * 0.5f;
			float offsetY = (m_viewer_size.y - m_ImageSize_.y) * 0.5f;

			//offsetY += offsetTop;

			ImGui::SetCursorPos(ImVec2(offsetX + 30, offsetY + 30));
			ImVec2 pos = ImGui::GetCursorScreenPos();

			ImVec2 uv_min = ImVec2(0.0f, 0.0f);
			ImVec2 uv_max = ImVec2(1.0f, 1.0f);

			ImGui::Image((void*)(intptr_t)TextureID, m_ImageSize_, uv_min, uv_max);

			// Begin Tooltip 
			if (IsZooming)
			{


				if (ImGui::BeginItemTooltip())
				{
					float TextureH = m_ImageSize_.y;
					float TextureW = m_ImageSize_.x;

					float region_sz = 32.0f;
					float region_x = io.MousePos.x - pos.x - region_sz * 0.5f;
					float region_y = io.MousePos.y - pos.y - region_sz * 0.5f;

					ImVec4 tint_col = use_text_color_for_tint ? ImGui::GetStyleColorVec4(ImGuiCol_Text) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
					ImVec4 border_col = ImGui::GetStyleColorVec4(ImGuiCol_Border);

					if (region_x < 0.0f) { region_x = 0.0f; }
					else if (region_x > TextureW - region_sz) { region_x = TextureW - region_sz; }
					if (region_y < 0.0f) { region_y = 0.0f; }
					else if (region_y > TextureH - region_sz) { region_y = TextureH - region_sz; }
					ImGui::Text("Min: (%.2f, %.2f)", region_x, region_y);
					ImGui::Text("Max: (%.2f, %.2f)", region_x + region_sz, region_y + region_sz);
					ImVec2 uv0 = ImVec2((region_x) / TextureW, (region_y) / TextureH);
					ImVec2 uv1 = ImVec2((region_x + region_sz) / TextureW, (region_y + region_sz) / TextureH);
					ImGui::Image((void*)(intptr_t)TextureID, ImVec2(region_sz * zoom, region_sz * zoom), uv0, uv1, tint_col, border_col);
					ImGui::EndTooltip();
				}
			}


			float middleX = offsetX + 30 + m_ImageSize_.x / 2.0f;
			float middleY = offsetY + 60 + m_ImageSize_.y / 2.0f;

			m_StoreImagePosition_ = ImVec2(middleX, middleY);
		}

		viewer_window_pos = ImGui::GetWindowPos();
		viewer_window_size = ImGui::GetWindowSize();
	}

	//----------------------------- BOTTOM BAR -----------------------------------------

	{
		ImGui::SetCursorPosY(ImGui::GetWindowHeight() - BottomMenu);
		ImGui::Separator();
		ImGui::BeginGroup();

		// progress bar
		float progress = Radeon.get_render_progress();
		bool isRenderComplete = (progress >= 100.0f);

		int max_samples = Radeon.get_max_samples();
		int current_samples = static_cast<int>(progress / 100.0f * max_samples);

		ImGui::SetNextItemWidth(WidthPerItems);

		if (isRenderComplete)
		{
			ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "Progress : ");

			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, GreenColor);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.f, 0.f, 0.f, 1.f));
		}
		else
		{
			ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.7f, 1.0f), "Progress : ");

			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, PinkColor);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 1.f));
		}

		ImGui::SetNextItemWidth(WidthPerItems + 100);

		ImGui::SameLine();

		char overlayText[32];
		sprintf_s(overlayText, "%d sur %d", current_samples, max_samples);
		ImGui::ProgressBar(progress / 100.0f, ImVec2(0.0f, 0.0f), overlayText);

		ImGui::PopStyleColor(2);




		if (progress <= m_MinSamples_ && !m_HasStartedRender_)
		{
			m_StartTime_ = std::chrono::high_resolution_clock::now();
			m_HasStartedRender_ = true;
			HorusUI::get_instance().SetOptionsChanged(false);
		}

		if (progress >= 99.f && m_HasStartedRender_)
		{
			auto end_time = std::chrono::high_resolution_clock::now();
			auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - m_StartTime_).count();

			total_seconds = duration_ms / 1000;
			hours = total_seconds / 3600;
			minutes = (total_seconds % 3600) / 60;
			seconds = total_seconds % 60;
			milliseconds = duration_ms % 1000;

			m_chrono_time_ = duration;
			m_HasStartedRender_ = false;
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
		m_LastProgress_ = progress;

		ImGui::SameLine();

		float windowWidth = ImGui::GetWindowWidth();
		float textWidth = ImGui::CalcTextSize("Renderer 5.1.1    ").x;
		ImGui::SetCursorPosX(windowWidth - textWidth - ImGui::GetStyle().ItemSpacing.x);
		ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "Renderer 5.1.1");

		ImGui::EndGroup();
	}

	// Process input
	if (ImGui::IsWindowFocused())
	{
		HorusViewportInput::get_instance().ProcessInput();
	}

	ImGui::End();
}

void HorusViewportRadeon::OverlayRadeon(bool* p_open)
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

void HorusViewportRadeon::ResetBuffers()
{
	HorusResetBuffers::get_instance().CallResetBuffers();
}


//---------------------------------------OPENGL SECTION---------------------------------------//

void HorusViewportOpenGL::ViewportOpenGL(bool* p_open)
{
	ImVec4 GreenColor = ImVec4(0.6f, 1.0f, 0.6f, 1.0f);
	ImVec4 PinkColor = ImVec4(1.0f, 0.6f, 0.7f, 1.0f);

	static bool IsZooming = false;
	static bool use_text_color_for_tint = false;
	float zoom = 4.0f;

	static bool isResizable = false;
	static ImVec2 lastSize = ImVec2(0, 0);
	static int customX = 800;
	static int customY = 600;
	static int lastCustomX = customX;
	static int lastCustomY = customY;
	static float offsetX_adjust = 0.0f;
	static float offsetY_adjust = 0.0f;
	float WidthPerItems = 100;

	int BottomMenu = 30;

	int ElementsNumber = 8; // number of ImGui elements in my viewport

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
		ImVec2 available = ImGui::GetContentRegionAvail();

		float AvailableWidth = ImGui::GetContentRegionAvail().x;
		float AvailableHeight = ImGui::GetContentRegionAvail().y;
		WidthPerItems = AvailableWidth / ElementsNumber;
		float windowWidth = available.x;
		float windowHeight = available.y - ImGui::GetFrameHeightWithSpacing();


		if (ImGui::BeginMenuBar())
		{

			ImGui::SetNextItemWidth(WidthPerItems);
			ImGui::Text("Size : ");
			ImGui::SameLine();

			ImGui::SetNextItemWidth(WidthPerItems);
			ImGui::DragInt("", &customX);
			ImGui::SameLine();

			ImGui::SetNextItemWidth(WidthPerItems);
			ImGui::Text(" x ");
			ImGui::SameLine();

			ImGui::SetNextItemWidth(WidthPerItems);
			ImGui::DragInt("", &customY);
			ImGui::SameLine();

			ImGui::SetNextItemWidth(WidthPerItems);
			if (ImGui::Combo("Predefined Sizes", &item_current, items, IM_ARRAYSIZE(items)))
			{
				sscanf_s(items[item_current], "%dx%d", &customX, &customY);
			}

			ImGui::SameLine();

			ImGui::Checkbox("Resizable", &isResizable);

			ImGui::SameLine();

			ImGui::Checkbox("Zooming", &IsZooming);

			ImGui::SameLine();

			/*if (ImGui::Button("Render Region"))
			{
				if (m_EnableRenderRegion_)
				{
					m_EnableRenderRegion_ = false;
				}
				else
				{
					m_EnableRenderRegion_ = true;
				}
			}

			ImGui::SameLine();*/

			ImGui::SetNextItemWidth(WidthPerItems);
			if (ImGui::BeginCombo("##Options de Zone de Rendu", NULL, ImGuiComboFlags_NoArrowButton))
			{
				static float opacity = 0.5f;

				ImGui::SliderFloat("Opacity", &opacity, 0.0f, 1.0f, "%.1f");

				ImGui::EndCombo();
			}



			//ImGui::SameLine();

			ImGui::EndMenuBar();
		}

		// Simple security
		if (m_IsFirstLaunch)
		{
			// Do something
			lastCustomX = customX;
			lastCustomY = customY;

			m_IsFirstLaunch = false;
		}

		if (customX != lastCustomX || customY != lastCustomY)
		{

			lastCustomX = customX;
			lastCustomY = customY;
		}

		GLuint TextureID = 0; //Radeon.get_texture_buffer();
		ImVec2 m_viewer_size = ImGui::GetContentRegionAvail();

		if (isResizable)
		{
			if (m_viewer_size.x != lastSize.x || m_viewer_size.y != lastSize.y)
			{
				//Radeon.resize_render(static_cast<int>(m_viewer_size.x), static_cast<int>(m_viewer_size.y));
				lastSize = m_viewer_size;
			}

			ImGui::Image((void*)(intptr_t)TextureID, ImVec2(m_viewer_size.x, m_viewer_size.y));
		}
		else
		{
			float aspect_ratio_image = static_cast<float>(customX) / static_cast<float>(customY);
			m_AspectRationRender_ = aspect_ratio_image;

			if (m_viewer_size.x / m_viewer_size.y > aspect_ratio_image)
			{
				m_ImageSize_.y = m_viewer_size.y;
				m_ImageSize_.x = m_viewer_size.y * aspect_ratio_image;
			}
			else
			{
				m_ImageSize_.x = m_viewer_size.x;
				m_ImageSize_.y = m_viewer_size.x / aspect_ratio_image;
			}

			float offsetTop = -25;
			float offsetDown = -25;

			float offsetX = (m_viewer_size.x - m_ImageSize_.x - 60) * 0.5f;
			float offsetY = (m_viewer_size.y - m_ImageSize_.y) * 0.5f;

			//offsetY += offsetTop;

			ImGui::SetCursorPos(ImVec2(offsetX + 30, offsetY + 30));
			ImVec2 pos = ImGui::GetCursorScreenPos();

			ImVec2 uv_min = ImVec2(0.0f, 0.0f);
			ImVec2 uv_max = ImVec2(1.0f, 1.0f);

			ImGui::Image((void*)(intptr_t)TextureID, m_ImageSize_, uv_min, uv_max);



			float middleX = offsetX + 30 + m_ImageSize_.x / 2.0f;
			float middleY = offsetY + 60 + m_ImageSize_.y / 2.0f;

			m_StoreImagePosition_ = ImVec2(middleX, middleY);
		}

		viewer_window_pos = ImGui::GetWindowPos();
		viewer_window_size = ImGui::GetWindowSize();
	}

	//----------------------------- BOTTOM BAR -----------------------------------------

	{
		ImGui::SetCursorPosY(ImGui::GetWindowHeight() - BottomMenu);
		ImGui::Separator();
		ImGui::BeginGroup();

		float windowWidth = ImGui::GetWindowWidth();
		float textWidth = ImGui::CalcTextSize("Renderer 5.1.1    ").x;
		ImGui::SetCursorPosX(windowWidth - textWidth - ImGui::GetStyle().ItemSpacing.x);
		ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "Renderer 5.1.1");

		ImGui::EndGroup();
	}

	ImGui::End();

	//ProcessInput(); // TODO : implement this function
}

void HorusViewportOpenGL::ResetBuffers()
{
	HorusResetBuffers::get_instance().CallResetBuffers();
}

void HorusViewportInput::ProcessInput()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::get_instance();
	ImGuiIO& io = ImGui::GetIO();

	static ImVec2 last_mouse_pos = ImGui::GetMousePos();
	bool isCameraMove = false;
	float CameraSpeed = 0.1f;

	ImVec2 mouse_pos = ImGui::GetMousePos();
	int state_left = ImGui::IsMouseDown(ImGuiMouseButton_Left) ? GLFW_PRESS : GLFW_RELEASE;
	int state_right = ImGui::IsMouseDown(ImGuiMouseButton_Right) ? GLFW_PRESS : GLFW_RELEASE;
	int state_middle = ImGui::IsMouseDown(ImGuiMouseButton_Middle) ? GLFW_PRESS : GLFW_RELEASE;

	if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_F)))
	{
		isCameraMove = true;
		RadeonProRender::float3 pivot = { 0.f, 0.f, 0.f };
		ObjectManager.set_camera_lookat(ObjectManager.get_active_camera_id(), pivot);
	}

	if (io.KeyAlt)
	{

		if (ImGui::IsMouseDown(0)) // left mouse
		{
			isCameraMove = true;

			ImVec2 WindowSize = ImGui::GetWindowSize();
			ImVec2 MouseDelta = ImGui::GetMouseDragDelta(0, 0.0f);

			MouseDelta.x /= WindowSize.x;
			MouseDelta.y /= WindowSize.y;

			ObjectManager.TumbleCamera(ObjectManager.get_active_camera_id(), MouseDelta.x, MouseDelta.y);
			ImGui::ResetMouseDragDelta(0);
		}

		if (ImGui::IsMouseDown(1)) // right mouse
		{
			isCameraMove = true;
			ImVec2 MouseDelta = ImGui::GetMouseDragDelta(1, 0);

			ObjectManager.ZoomCamera(ObjectManager.get_active_camera_id(), CameraSpeed * MouseDelta.y);

			ImGui::ResetMouseDragDelta(1);
		}

		if (ImGui::IsMouseDown(2)) // middle mouse
		{
			isCameraMove = true;
			ImVec2 WindowSize = ImGui::GetWindowSize();
			ImVec2 MouseDelta = ImGui::GetMouseDragDelta(2, 0);

			/*MouseDelta.x /= WindowSize.x;
			MouseDelta.y /= WindowSize.y;*/

			ObjectManager.PanCamera(ObjectManager.get_active_camera_id(), MouseDelta.x, MouseDelta.y);

			ImGui::ResetMouseDragDelta(2);
		}

		if (io.MouseWheel != 0)
		{
			isCameraMove = true;
			float scroll_delta = io.MouseWheel;
			ObjectManager.scroll_camera(ObjectManager.get_active_camera_id(), scroll_delta);
		}
	}

	if (isCameraMove)
	{
		HorusResetBuffers::get_instance().CallResetBuffers();
	}
}

