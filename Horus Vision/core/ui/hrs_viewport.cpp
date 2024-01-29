#include "hrs_viewport.h"
#include <hrs_object_manager.h>

#include <Windows.h>

#include "hrs_inspector.h"
#include "hrs_timer.h"
#include "imgui_internal.h"
using namespace std;

//---------------------------------------RADEON SECTION---------------------------------------//

void HorusViewportRadeon::ViewportRadeon(bool* p_open)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusTimerManager& TimerManager = HorusTimerManager::GetInstance();

	ImVec4 GreenColor = ImVec4(0.6f, 1.0f, 0.6f, 1.0f);
	ImVec4 PinkColor = ImVec4(1.0f, 0.6f, 0.7f, 1.0f);

	static ImVec2 LastSize = ImVec2(0, 0);
	static ImVec2 ViewerWindowPos;
	static ImVec2 ViewerWindowSize;

	static bool IsZooming = false;
	static bool UseTextColorForTint = false;
	static bool IsResizable = false;

	static int CustomX = 800;
	static int CustomY = 600;
	static int LastCustomX = CustomX;
	static int LastCustomY = CustomY;

	float WidthPerItems = 100;

	ImGuiIO& io = ImGui::GetIO();
	bool IsLeftAltPressed = io.KeyAlt;

	ImGuiWindowFlags WindowFlags = 0;

	if (IsLeftAltPressed) {
		WindowFlags |= ImGuiWindowFlags_NoMove;
	}

	const char* Items[] = { "800x600", "1024x768", "1280x536", "1280x720", "1920x1080", "1920x803", "2048x858" };
	static int ItemCurrent = 0;

	if (ImGui::Begin("Render view", p_open, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | WindowFlags))
	{
		int ElementsNumber = 8;
		float AvailableWidth = ImGui::GetContentRegionAvail().x;
		WidthPerItems = AvailableWidth / ElementsNumber;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::Checkbox("Stop", &m_RenderIsLocked_))
			{
				if (m_RenderIsLocked_ == true)
				{
					Radeon.SetLockingRender(true);
				}
				else if (m_RenderIsLocked_ == false)
				{
					Radeon.SetLockingRender(false);
				}
			}

			// Vertical separator
			ImGui::SameLine();
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

			ImGui::SetNextItemWidth(WidthPerItems-50);
			ImGui::Text("Size : ");
			ImGui::SameLine();

			ImGui::SetNextItemWidth(WidthPerItems- 50);
			ImGui::DragInt("", &CustomX);
			ImGui::SameLine();

			ImGui::SetNextItemWidth(WidthPerItems- 50);
			ImGui::Text(" x ");
			ImGui::SameLine();

			ImGui::SetNextItemWidth(WidthPerItems- 50);
			ImGui::DragInt("", &CustomY);
			ImGui::SameLine();

			ImGui::SameLine(); ShowHelpMarker("Resize the viewport to the desired size. The render will be automatically resized to fit the viewport. Ctrl + Click to edit directly the size.");

			ImGui::SetNextItemWidth(WidthPerItems- 50);
			if (ImGui::Combo("Predefined Sizes", &ItemCurrent, Items, IM_ARRAYSIZE(Items)))
			{
				sscanf_s(Items[ItemCurrent], "%dx%d", &CustomX, &CustomY);
			}
			ImGui::SameLine();
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

			ImGui::SameLine();
			ImGui::Checkbox("Resizable", &IsResizable);
			ImGui::SameLine(); ShowHelpMarker("Resize automatically the render to fit the viewport.");
			ImGui::SameLine();
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

			ImGui::Checkbox("Zooming", &IsZooming);
			ImGui::SameLine(); ShowHelpMarker("Zoom on the render when you rest the mouse on it.");
			ImGui::SameLine();
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

			// TODO : Implement Combo box for choosing the render space (sRGB, Linear, etc...)

			const char* ColorSpaceConfigItems[] = { "sRGB", "Linear", "ACEScg", "ACES2065-1", "ACEScc", "ACEScct", "ACESproxy" };
			static int CurrentColorSpaceConfig = 0;

			ImGui::SetNextItemWidth(WidthPerItems- 50);
			if (ImGui::Combo("Color Space", &CurrentColorSpaceConfig, ColorSpaceConfigItems, IM_ARRAYSIZE(ColorSpaceConfigItems)))
			{
				//Radeon.SetColorSpace(CurrentColorSpaceConfig);
				spdlog::info("Color Space set to : {}", ColorSpaceConfigItems[CurrentColorSpaceConfig]);
			}

			rpr_color_space ColorSpace = RPR_COLOR_SPACE_SRGB;


			ImGui::SameLine();
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

			// Create popup to confirm the full render
			if (ImGui::Button("Full Render"))
			{
				m_IsFullRender_ = true;
			}
			ImGui::SameLine();
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

			ImGui::EndMenuBar();
		}

		GLuint TextureId = Radeon.GetTextureBuffer();
		ImVec2 ViewerSize = ImGui::GetContentRegionAvail();

		if (m_IsViewportLocked_ == true)
		{
			if (IsResizable)
			{
				if (ViewerSize.x != LastSize.x || ViewerSize.y != LastSize.y)
				{
					LastSize = ViewerSize;
				}

				ImVec2 CursorPos = ImGui::GetCursorPos();
				ImVec2 CenterPos = ImVec2(CursorPos.x + ViewerSize.x / 2, CursorPos.y + ViewerSize.y / 2);

				ImDrawList* DrawList = ImGui::GetWindowDrawList();
				DrawList->AddRectFilled(ImGui::GetCursorScreenPos(), ImVec2(m_ImageSize_.x, m_ImageSize_.y), ImColor(26, 26, 51, 255));

				float RenderProgress = 10; // TODO : get the render progress of the tiled render
				ImGui::SetCursorPos(ImVec2(CenterPos.x - m_ImageSize_.x / 2, CenterPos.y - 20));
				ImGui::ProgressBar(RenderProgress, ImVec2(m_ImageSize_.x, 30), "Rendering...");

				// Bouton "Quit Render"
				ImGui::SetCursorPos(ImVec2(CenterPos.x - m_ImageSize_.x / 2, CenterPos.y + m_ImageSize_.y - 50));

				if (ImGui::Button("Quit Render"))
				{
					m_IsViewportLocked_ = false;
					m_IsFullRender_ = false;
				}
			}
			else
			{
				int CurrentTile = 64;
				int MaxTiles = 25;

				float AspectRatioImage = static_cast<float>(CustomX) / static_cast<float>(CustomY);
				m_AspectRationRender_ = AspectRatioImage;

				if (ViewerSize.x / ViewerSize.y > AspectRatioImage)
				{
					m_ImageSize_.y = ViewerSize.y;
					m_ImageSize_.x = ViewerSize.y * AspectRatioImage;
				}
				else
				{
					m_ImageSize_.x = ViewerSize.x;
					m_ImageSize_.y = ViewerSize.x / AspectRatioImage;
				}

				ImVec2 CursorPos = ImGui::GetCursorPos();
				ImVec2 CenterPos = ImVec2(CursorPos.x + m_ImageSize_.x / 2, CursorPos.y + m_ImageSize_.y / 2);

				ImDrawList* DrawList = ImGui::GetWindowDrawList();
				DrawList->AddRectFilled(ImGui::GetCursorScreenPos(), ImVec2(m_ImageSize_.x, m_ImageSize_.y), ImColor(26, 26, 51, 255));

				float RenderProgress = 80; // TODO : get the render progress of the tiled render
				bool IsTiledRenderComplete = (RenderProgress >= 100.0f);

				RenderProgress = 10; // TODO : get the render progress

				char OverlayText[32];
				sprintf_s(OverlayText, "%d On %d", MaxTiles, CurrentTile);
				ImGui::SetCursorPos(ImVec2(CenterPos.x - m_ImageSize_.x / 2, CenterPos.y - 20));
				ImGui::SetNextItemWidth(m_ImageSize_.x / 2);
				ImGui::ProgressBar(RenderProgress / 100, ImVec2(m_ImageSize_.x / 2, 30), OverlayText);

				if (IsTiledRenderComplete)
				{
					m_IsViewportLocked_ = false;
					m_IsFullRender_ = false;
				}
				else
				{
					ImGui::Text("Rendering...");
				}

				ImGui::SetCursorPos(ImVec2(CenterPos.x - m_ImageSize_.x / 2, CenterPos.y + m_ImageSize_.y - 10));
				if (ImGui::Button("Quit Render"))
				{
					m_IsViewportLocked_ = false;
					m_IsFullRender_ = false;
				}
			}
		}
		else if (m_IsViewportLocked_ == false)
		{
			if (m_IsFirstLaunch_)
			{
				Radeon.ResizeRender(CustomX, CustomY);
				LastCustomX = CustomX;
				LastCustomY = CustomY;

				m_IsFirstLaunch_ = false;
			}

			if (CustomX != LastCustomX || CustomY != LastCustomY)
			{
				Radeon.ResizeRender(CustomX, CustomY);
				LastCustomX = CustomX;
				LastCustomY = CustomY;
			}

			if (IsResizable)
			{
				if (ViewerSize.x != LastSize.x || ViewerSize.y != LastSize.y)
				{
					Radeon.ResizeRender(static_cast<int>(ViewerSize.x), static_cast<int>(ViewerSize.y));
					LastSize = ViewerSize;
				}

				ImGui::Image((void*)intptr_t(TextureId), ImVec2(ViewerSize.x, ViewerSize.y));
			}
			else
			{
				float aspect_ratio_image = static_cast<float>(CustomX) / static_cast<float>(CustomY);
				m_AspectRationRender_ = aspect_ratio_image;

				if (ViewerSize.x / ViewerSize.y > aspect_ratio_image)
				{
					m_ImageSize_.y = ViewerSize.y;
					m_ImageSize_.x = ViewerSize.y * aspect_ratio_image;
				}
				else
				{
					m_ImageSize_.x = ViewerSize.x;
					m_ImageSize_.y = ViewerSize.x / aspect_ratio_image;
				}

				float OffsetX = (ViewerSize.x - m_ImageSize_.x - 60) * 0.5f;
				float OffsetY = (ViewerSize.y - m_ImageSize_.y) * 0.5f;

				ImGui::SetCursorPos(ImVec2(OffsetX + 30, OffsetY + 30));
				ImVec2 Pos = ImGui::GetCursorScreenPos();

				ImVec2 UvMin = ImVec2(0.0f, 0.0f);
				ImVec2 UvMax = ImVec2(1.0f, 1.0f);

				ImGui::Image((void*)intptr_t(TextureId), m_ImageSize_, UvMin, UvMax);

				if (IsZooming)
				{
					if (ImGui::BeginItemTooltip())
					{
						float Zoom = 4.0f;
						float TextureH = m_ImageSize_.y;
						float TextureW = m_ImageSize_.x;

						float RegionSz = 32.0f;
						float RegionX = io.MousePos.x - Pos.x - RegionSz * 0.5f;
						float RegionY = io.MousePos.y - Pos.y - RegionSz * 0.5f;

						ImVec4 TintCol = UseTextColorForTint ? ImGui::GetStyleColorVec4(ImGuiCol_Text) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
						ImVec4 BorderCol = ImGui::GetStyleColorVec4(ImGuiCol_Border);

						if (RegionX < 0.0f) { RegionX = 0.0f; }
						else if (RegionX > TextureW - RegionSz) { RegionX = TextureW - RegionSz; }
						if (RegionY < 0.0f) { RegionY = 0.0f; }
						else if (RegionY > TextureH - RegionSz) { RegionY = TextureH - RegionSz; }
						ImGui::Text("Min: (%.2f, %.2f)", RegionX, RegionY);
						ImGui::Text("Max: (%.2f, %.2f)", RegionX + RegionSz, RegionY + RegionSz);
						ImVec2 Uv0 = ImVec2((RegionX) / TextureW, (RegionY) / TextureH);
						ImVec2 Uv1 = ImVec2((RegionX + RegionSz) / TextureW, (RegionY + RegionSz) / TextureH);
						ImGui::Image((void*)intptr_t(TextureId), ImVec2(RegionSz * Zoom, RegionSz * Zoom), Uv0, Uv1, TintCol, BorderCol);
						ImGui::EndTooltip();
					}
				}

				float MiddleX = OffsetX + 30 + m_ImageSize_.x / 2.0f;
				float MiddleY = OffsetY + 60 + m_ImageSize_.y / 2.0f;

				m_StoreImagePosition_ = ImVec2(MiddleX, MiddleY);
			}
		}

		ViewerWindowPos = ImGui::GetWindowPos();
		ViewerWindowSize = ImGui::GetWindowSize();
	}

	//----------------------------- BOTTOM BAR -----------------------------------------

	{
		int BottomMenu = 30;
		ImGui::SetCursorPosY(ImGui::GetWindowHeight() - BottomMenu);
		ImGui::Separator();
		ImGui::BeginGroup();

		// progress bar
		float Progress = Radeon.GetClassicRenderProgress();
		bool IsRenderComplete = (Progress >= 100.0f);

		int MaxSamples = Radeon.GetMaxSamples();
		int CurrentSamples = static_cast<int>(Progress / 100.0f * MaxSamples);

		ImGui::SetNextItemWidth(WidthPerItems);

		if (IsRenderComplete)
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

		char OverlayText[32];
		sprintf_s(OverlayText, "%d on %d", CurrentSamples, MaxSamples);
		ImGui::ProgressBar(Progress / 100.0f, ImVec2(0.0f, 0.0f), OverlayText);

		ImGui::PopStyleColor(2);

		if (Progress <= m_MinSamples_ && !m_HasStartedRender_)
		{
			TimerManager.StartTimer("RenderTimer");

			m_HasStartedRender_ = true;
			HorusUI::GetInstance().SetOptionsChanged(false);
		}

		if (Progress >= 99.f && m_HasStartedRender_)
		{
			auto TimerEnd = TimerManager.StopTimer("RenderTimer");

			m_TotalSeconds_ = TimerEnd / 1000;
			m_Hours_ = m_TotalSeconds_ / 3600;
			m_Minutes_ = (m_TotalSeconds_ % 3600) / 60;
			m_Seconds_ = m_TotalSeconds_ % 60;
			m_Milliseconds_ = TimerEnd % 1000;

			m_ChronoTime_ = m_Duration_;
			m_HasStartedRender_ = false;
		}

		if (IsRenderComplete)
		{
			char TimeString[100];
			snprintf(TimeString, sizeof(TimeString), "Rendering finish in : %lldh %lldm %llds %lldms", m_Hours_, m_Minutes_, m_Seconds_, m_Milliseconds_);

			ImVec2 OverlayPos = ImVec2(
				ViewerWindowPos.x + ViewerWindowSize.x / 2.0f,
				ViewerWindowPos.y + ViewerWindowSize.y - 30.0f
			);

			ImGuiWindowFlags OverlayFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

			ImGui::SetNextWindowPos(OverlayPos, ImGuiCond_Always, ImVec2(0.5f, 1.0f));

			ImGui::SetNextWindowBgAlpha(0.35f);
			if (ImGui::Begin("finish_render", nullptr, OverlayFlags))
			{
				ImGui::Separator();
				ImGui::Text("%s", TimeString);
				ImGui::Separator();
			}
			ImGui::End();


		}
		m_LastProgress_ = Progress;

		ImGui::SameLine();

		float WindowWidth = ImGui::GetWindowWidth();
		float TextWidth = ImGui::CalcTextSize("Renderer 5.1.1    ").x;
		ImGui::SetCursorPosX(WindowWidth - TextWidth - ImGui::GetStyle().ItemSpacing.x);
		ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "Renderer 5.1.1");

		ImGui::EndGroup();
	}

	if (ImGui::IsWindowFocused())
	{
		HorusViewportInput::GetInstance().ProcessInput();
	}

	if (m_CallToRenderFullImage_ == true)
	{
		m_CallToRenderFullImage_ = false;

		Sleep(2000);

		Radeon.CallRenderMultiTiles(m_TileSize_, m_MaxIterationPerTile_);
	}

	if (m_IsFullRender_ == true)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f)); // Green
		if (!ImGui::IsPopupOpen("Full Render"))
			ImGui::OpenPopup("Full Render");
		if (ImGui::BeginPopupModal("Full Render", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::PopStyleColor(1);

			ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "Are you sure you want to render the full image ?");

			if (float ItemHeight = ImGui::GetTextLineHeightWithSpacing(); ImGui::BeginChildFrame(ImGui::GetID("FullRender"), ImVec2(-FLT_MIN, 6.25f * ItemHeight)))
			{
				ImGui::TextWrapped("This will render the full image, and not only the region you selected.");
				ImGui::TextWrapped("This can take a long time, depending on the size of the image.");
				ImGui::TextWrapped("Are you sure you want to continue ?");
			}
			ImGui::EndChildFrame();

			// Parameters for the tiled render
			const char* Tilesitems[] = { "32", "64", "128", "256", "512", "1024", "2048" };
			static int TilesItemCurrent = 3;

			ImGui::SetNextItemWidth(120);
			if (ImGui::Combo("Tile for render", &TilesItemCurrent, Tilesitems, IM_ARRAYSIZE(Tilesitems)))
			{
				sscanf_s(Tilesitems[TilesItemCurrent], "%d", &m_TileSize_);
			}
			ImGui::SameLine(); ShowHelpMarker("Choose the size of the tile for the render. The bigger the tile, the faster the render will be, but the more memory it will use.");
			ImGui::SameLine();

			// Max iteration per tile
			if (ImGui::DragInt("Max Iteration per tile", &m_MaxIterationPerTile_, .1, 4, 512))
			{
				if (m_MaxIterationPerTile_ < 4)
				{
					m_MaxIterationPerTile_ = 4;
				}
			}
			ImGui::SameLine(); ShowHelpMarker("Choose the number of iteration per tile. The more iteration, the better the render will be, but the longer it will take.");

			ImVec2 ButtonSize(ImGui::GetFontSize() * 7.0f, 0.f);
			if (ImGui::Button("Yes", ButtonSize))
			{
				spdlog::info("Full Render call render yes");

				ImGui::CloseCurrentPopup();

				m_IsViewportLocked_ = true;
				m_IsFullRender_ = false;
				m_CallToRenderFullImage_ = true;
			}
			ImGui::SameLine();

			if (ImGui::Button("No", ButtonSize))
			{
				spdlog::info("Full Render Popup call No");

				ImGui::CloseCurrentPopup();

				m_IsFullRender_ = false;
				m_IsViewportLocked_ = false;
			}
			ImGui::EndPopup();
		}
	}
	ImGui::End();
}

void HorusViewportRadeon::OverlayRadeon(bool* p_open)
{
	ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

	const float DistancePad = 10.0f;
	const ImGuiViewport* Viewport = ImGui::GetMainViewport();
	auto WindowPos = ImVec2(Viewport->WorkPos.x + Viewport->WorkSize.x - DistancePad,
	                         Viewport->WorkPos.y + Viewport->WorkSize.y - DistancePad);
	ImVec2 WindowPosPivot = ImVec2(1.0f, 1.0f);

	ImGui::SetNextWindowPos(WindowPos, ImGuiCond_Always, WindowPosPivot);
	ImGui::SetNextWindowViewport(Viewport->ID);
	WindowFlags |= ImGuiWindowFlags_NoMove;

	if (ImGui::Begin("render_overlay", p_open, WindowFlags))
	{
		ImGui::Text("Render finish in : (wip)");
		ImGui::Separator();
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("Close", nullptr, p_open))
				*p_open = false;
			ImGui::EndPopup();
		}
	}
	ImGui::End();
}

void HorusViewportRadeon::ResetBuffers()
{
	HorusResetBuffers::GetInstance().CallResetBuffers();
}

void HorusViewportRadeon::ShowHelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

//---------------------------------------OPENGL SECTION---------------------------------------//

void HorusViewportOpenGL::ViewportOpenGL(bool* p_open)
{
	HorusOpenGL& OpenGL = HorusOpenGL::GetInstance();

	static int CustomX = 800;
	static int CustomY = 600;
	static int LastCustomX = CustomX;
	static int LastCustomY = CustomY;

	ImGuiIO& io = ImGui::GetIO();
	bool IsLeftAltPressed = io.KeyAlt;

	ImGuiWindowFlags WindowFlags = 0;

	if (IsLeftAltPressed) {
		WindowFlags |= ImGuiWindowFlags_NoMove;
	}

	if (ImGui::Begin("Viewer", p_open, ImGuiWindowFlags_MenuBar | WindowFlags))
	{
		int ElementsNumber = 8;

		float AvailableWidth = ImGui::GetContentRegionAvail().x;
		float WidthPerItems = AvailableWidth / ElementsNumber;

		if (ImGui::BeginMenuBar())
		{
			ImGui::SetNextItemWidth(WidthPerItems);
			if (ImGui::BeginCombo("##Options de Zone de Rendu", nullptr, ImGuiComboFlags_NoArrowButton))
			{
				static float opacity = 0.5f;

				ImGui::SliderFloat("Opacity", &opacity, 0.0f, 1.0f, "%.1f");

				ImGui::EndCombo();
			}
			ImGui::EndMenuBar();
		}

		if (m_IsFirstLaunch_)
		{
			// Do something
			LastCustomX = CustomX;
			LastCustomY = CustomY;

			m_IsFirstLaunch_ = false;
		}

		if (CustomX != LastCustomX || CustomY != LastCustomY)
		{

			LastCustomX = CustomX;
			LastCustomY = CustomY;
		}

		GLuint TextureId = OpenGL.GetOpenGlTextureBuffer();
		ImVec2 ViewerSize = ImGui::GetContentRegionAvail();

		float AspectRatioImage = static_cast<float>(CustomX) / static_cast<float>(CustomY);
		m_AspectRationRender_ = AspectRatioImage;

		if (ViewerSize.x / ViewerSize.y > AspectRatioImage)
		{
			m_ImageSize_.y = ViewerSize.y;
			m_ImageSize_.x = ViewerSize.y * AspectRatioImage;
		}
		else
		{
			m_ImageSize_.x = ViewerSize.x;
			m_ImageSize_.y = ViewerSize.x / AspectRatioImage;
		}

		float OffsetX = (ViewerSize.x - m_ImageSize_.x) * 0.5f;
		float OffsetY = (ViewerSize.y - m_ImageSize_.y) * 0.5f;

		ImGui::SetCursorPos(ImVec2(OffsetX, OffsetY));

		ImVec2 UvMin = ImVec2(0.0f, 0.0f);
		ImVec2 UvMax = ImVec2(1.0f, 1.0f);

		ImGui::Image((void*)static_cast<intptr_t>(TextureId), m_ImageSize_, UvMin, UvMax);
	}

	//----------------------------- BOTTOM BAR -----------------------------------------

	{
		int BottomMenu = 30;
		ImGui::SetCursorPosY(ImGui::GetWindowHeight() - BottomMenu);
		ImGui::Separator();
		ImGui::BeginGroup();

		float WindowWidth = ImGui::GetWindowWidth();
		float TextWidth = ImGui::CalcTextSize("Viewer 5.1.1    ").x;
		ImGui::SetCursorPosX(WindowWidth - TextWidth - ImGui::GetStyle().ItemSpacing.x);
		ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "Viewer 5.1.1");

		ImGui::EndGroup();
	}

	ImGui::End();

	//ProcessInput(); // TODO : implement this function later
}

void HorusViewportOpenGL::ResetBuffers()
{
	HorusResetBuffers::GetInstance().CallResetBuffers();
}

void HorusViewportInput::ProcessInput()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	ImGuiIO& io = ImGui::GetIO();

	bool IsCameraMove = false;

	if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_F)))
	{
		IsCameraMove = true;
		glm::vec3 Pivot = { 0.f, 0.f, 0.f };
		ObjectManager.SetCameraLookat(ObjectManager.GetActiveRadeonCameraId(), Pivot);
	}

	if (io.KeyAlt)
	{
		if (ImGui::IsMouseDown(0)) // left mouse
		{
			IsCameraMove = true;
			ImVec2 WindowSize = ImGui::GetWindowSize();
			ImVec2 MouseDelta = ImGui::GetMouseDragDelta(0, 0.0f);

			MouseDelta.x /= WindowSize.x;
			MouseDelta.y /= WindowSize.y;

			ObjectManager.SetTumbleCamera(ObjectManager.GetActiveRadeonCameraId(), MouseDelta.x, MouseDelta.y, 4);

			ImGui::ResetMouseDragDelta(0);
		}

		if (ImGui::IsMouseDown(1)) // right mouse
		{
			IsCameraMove = true;
			ImVec2 MouseDelta = ImGui::GetMouseDragDelta(1, 0);

			ObjectManager.SetZoomCamera(ObjectManager.GetActiveRadeonCameraId(), 0.1f * MouseDelta.y);

			ImGui::ResetMouseDragDelta(1);
		}

		if (ImGui::IsMouseDown(2)) // middle mouse
		{
			IsCameraMove = true;
			ImVec2 MouseDelta = ImGui::GetMouseDragDelta(2, 0);

			ObjectManager.SetPanCamera(ObjectManager.GetActiveRadeonCameraId(), MouseDelta.x, MouseDelta.y, 0.01f);

			ImGui::ResetMouseDragDelta(2);
		}

		if (io.MouseWheel != 0)
		{
			IsCameraMove = true;
			float ScrollDelta = io.MouseWheel;
			ObjectManager.SetScrollCamera(ObjectManager.GetActiveRadeonCameraId(), ScrollDelta);
		}
	}

	if (IsCameraMove)
	{
		HorusInspector::GetInstance().CallSetFocusPlaneToFocusPosition();
		HorusInspector::GetInstance().PopulateSelectedCameraInfos();
		HorusResetBuffers::GetInstance().CallResetBuffers();
	}
}

