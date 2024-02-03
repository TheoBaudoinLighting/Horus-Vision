
// External includes
#include "imgui.h"

// Basic includes
#include <filesystem>
#include <regex>
#include <vector>
#include <future>
#include <chrono>

// Project includes
#include "hrs_engine.h" // glad.h
#include "hrs_object_manager.h" // glfw3.h
#include "hrs_ui.h" // nothing
#include "hrs_reset_buffers.h" // glfw3.h

// External includes
#include "ImGuizmo.h"
#include <hrs_console.h>

#include "hrs_imgui_core.h"
#include "hrs_inspector.h"
#include "hrs_timer.h"

// Pre load data for the engine in a separate jthread
void LoadSetupEngineData()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusResetBuffers& ResetBuffers = HorusResetBuffers::GetInstance();
	HorusTimerManager::GetInstance().StartTimer("LoadData");

	int HDRI = ObjectManager.CreateLight("HDRI_01", "hdri", "core/scene/dependencies/light/niederwihl_forest_4k.exr");
	//int HDRI = ObjectManager.CreateLight("Lgt_Dome01", "hdri", "core/scene/dependencies/light/horus_hdri_main.exr");
	ObjectManager.SetLightRotation(HDRI, glm::vec3(0.0f, 1.0f, 0.0f));

	auto TimeLoad = HorusTimerManager::GetInstance().StopTimer("LoadData");
	spdlog::info("Loading Engine data finished in {} ms.", TimeLoad);

	ResetBuffers.CallResetBuffers();
}

int HorusEngine::ContextInfo(rpr_context& Context)
{
	rpr_int Status = RPR_SUCCESS;

	std::vector<rpr_context_info> ContextInfoList;

	ContextInfoList.push_back(RPR_CONTEXT_LIST_CREATED_CAMERAS);
	ContextInfoList.push_back(RPR_CONTEXT_LIST_CREATED_MATERIALNODES);
	ContextInfoList.push_back(RPR_CONTEXT_LIST_CREATED_LIGHTS);
	ContextInfoList.push_back(RPR_CONTEXT_LIST_CREATED_SHAPES);
	ContextInfoList.push_back(RPR_CONTEXT_LIST_CREATED_POSTEFFECTS);
	ContextInfoList.push_back(RPR_CONTEXT_LIST_CREATED_HETEROVOLUMES);
	ContextInfoList.push_back(RPR_CONTEXT_LIST_CREATED_GRIDS);
	ContextInfoList.push_back(RPR_CONTEXT_LIST_CREATED_BUFFERS);
	ContextInfoList.push_back(RPR_CONTEXT_LIST_CREATED_IMAGES);
	ContextInfoList.push_back(RPR_CONTEXT_LIST_CREATED_FRAMEBUFFERS);
	ContextInfoList.push_back(RPR_CONTEXT_LIST_CREATED_SCENES);
	ContextInfoList.push_back(RPR_CONTEXT_LIST_CREATED_CURVES);
	ContextInfoList.push_back(RPR_CONTEXT_LIST_CREATED_MATERIALSYSTEM);
	ContextInfoList.push_back(RPR_CONTEXT_LIST_CREATED_COMPOSITE);
	ContextInfoList.push_back(RPR_CONTEXT_LIST_CREATED_LUT);

	std::unordered_map<std::string, int> ObjectCount;

	for (auto& i : ContextInfoList)
	{
		size_t Count = 0;
		Status = rprContextGetInfo(Context, i, 0, nullptr, &Count);

		if (int Size = Count / sizeof(void*); Size > 0)
		{
			std::string ObjectType;
			int CountPerObject = 0;

			if (i == RPR_CONTEXT_LIST_CREATED_CAMERAS)
			{
				ObjectType = "camera";
				CountPerObject = Size;
				m_CameraNumber_ = Size;
			}
			else if (i == RPR_CONTEXT_LIST_CREATED_MATERIALNODES)
			{
				ObjectType = "material node";
				CountPerObject = Size;
				m_MaterialNumber_ = Size;
			}
			else if (i == RPR_CONTEXT_LIST_CREATED_LIGHTS)
			{
				ObjectType = "light";
				CountPerObject = Size;
				m_LightNumber_ = Size;
			}
			else if (i == RPR_CONTEXT_LIST_CREATED_SHAPES)
			{
				ObjectType = "shape";
				CountPerObject = Size;
				m_MeshNumber_ = Size;
			}
			else if (i == RPR_CONTEXT_LIST_CREATED_POSTEFFECTS)
			{
				ObjectType = "postEffect";
				CountPerObject = Size;
				m_PostEffectNumber_ = Size;
			}
			else if (i == RPR_CONTEXT_LIST_CREATED_HETEROVOLUMES)
			{
				ObjectType = "heteroVolume";
				CountPerObject = Size;
				m_VolumeNumber_ = Size;
			}
			else if (i == RPR_CONTEXT_LIST_CREATED_GRIDS)
			{
				ObjectType = "grid";
				CountPerObject = Size;
				m_GridNumber_ = Size;
			}
			else if (i == RPR_CONTEXT_LIST_CREATED_BUFFERS)
			{
				ObjectType = "buffer";
				CountPerObject = Size;
				m_BufferNumber_ = Size;
			}
			else if (i == RPR_CONTEXT_LIST_CREATED_IMAGES)
			{
				ObjectType = "image";
				CountPerObject = Size;
				m_ImageNumber_ = Size;
			}
			else if (i == RPR_CONTEXT_LIST_CREATED_FRAMEBUFFERS)
			{
				ObjectType = "framebuffer";
				CountPerObject = Size;
				m_FramebufferNumber_ = Size;
			}
			else if (i == RPR_CONTEXT_LIST_CREATED_SCENES)
			{
				ObjectType = "scene";
				CountPerObject = Size;
				m_SceneNumber_ = Size;
			}
			else if (i == RPR_CONTEXT_LIST_CREATED_CURVES)
			{
				ObjectType = "curve";
				CountPerObject = Size;
				m_CurveNumber_ = Size;
			}
			else if (i == RPR_CONTEXT_LIST_CREATED_MATERIALSYSTEM)
			{
				ObjectType = "material system";
				CountPerObject = Size;
				m_MaterialNodeNumber_ = Size;
			}
			else if (i == RPR_CONTEXT_LIST_CREATED_COMPOSITE)
			{
				ObjectType = "composite";
				CountPerObject = Size;
				m_CompositeNumber_ = Size;
			}
			else if (i == RPR_CONTEXT_LIST_CREATED_LUT)
			{
				ObjectType = "lut";
				CountPerObject = Size;
				m_LutNumber_ = Size;
			}
			else
			{
				ObjectType = "unknown";
				CountPerObject = Size;
			}

			ObjectCount[ObjectType] = CountPerObject;
		}
	}

	return 0;
}

void HorusEngine::Init(int Width, int Height, const std::string& Title, const std::string& SavePath)
{
	m_Window_ = std::make_unique<HorusWindow>();
	m_Window_->InitWindow(Width, Height, Title);
}

void HorusEngine::InitContexts(int Width, int Height, HorusWindow* Window)
{
	HorusOpenGL& OpenGL = HorusOpenGL::GetInstance();
	HorusImGuiCore& ImGuiCore = HorusImGuiCore::GetInstance();
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusUI& UI = HorusUI::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();
	HorusInspector& Inspector = HorusInspector::GetInstance();

	UI.Init();
	OpenGL.Init(Width, Height, Window);
	ImGuiCore.Initialize(Width, Height, Window);
	Radeon.Init(Width, Height, Window);
	Radeon.InitGraphics();
	Inspector.Init();

	// Artificial delay
	//std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	spdlog::info("OpenGL, ImGui and Radeon contexts initialized.");
	Console.AddLog(" [info] OpenGL, ImGui and Radeon contexts initialized.");
}

void HorusEngine::PreRender()
{
	HorusOpenGL& OpenGL = HorusOpenGL::GetInstance();
	HorusImGuiCore& ImGuiCore = HorusImGuiCore::GetInstance();
	HorusRadeon& Radeon = HorusRadeon::GetInstance();

	m_IsClosing_ = GetIsClosing();

	OpenGL.InitRender();
	ImGuiCore.InitRender();
	Radeon.InitRender();
}

void HorusEngine::Render()
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusUI& UI = HorusUI::GetInstance();
	[[maybe_unused]] HorusOpenGL& OpenGL = HorusOpenGL::GetInstance();
	[[maybe_unused]] HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	Radeon.RenderEngine();

	{
		//glm::mat4 model, view, projection; // Later for OpenGL
		//OpenGL.Render(); // TODO : Make it work with Radeon

		//ObjectManager.UpdateRadeonCamera(ObjectManager.get_active_camera_id());
		//ObjectManager.GetMatrices(ObjectManager.get_active_camera_id(), model, view, projection);

		//glm::mat4 mvp = model * view * projection; // Later for OpenGL
		//glLoadMatrixf(glm::value_ptr(mvp)); // Later for OpenGL
	}

	UI.RenderUI();
}

void HorusEngine::PostRender()
{
	HorusOpenGL& OpenGL = HorusOpenGL::GetInstance();
	HorusImGuiCore& ImGuiCore = HorusImGuiCore::GetInstance();
	HorusRadeon& Radeon = HorusRadeon::GetInstance();

	ImGuiCore.PostRender();
	OpenGL.PostRender();
	Radeon.PostRender();


	if (m_EngineIsReady_)
	{
		if (m_IsFirstLaunch_)
		{
			m_LoadingThread_ = std::jthread(LoadSetupEngineData);
			m_LoadingThread_.detach();

			m_IsFirstLaunch_ = false;
		}
	}

	/*if (m_IsFirstLaunch_)
	{}*/

	if (m_IsClosing_)
	{
		Close();
	}
}

int GetExistingFileSuffix(const std::string& BaseName, const std::string& Extension)
{
	std::string RegexString = BaseName + R"((_\d+)?)";
	std::regex Regex(RegexString + Extension + "$");

	int MaxSuffix = -1;
	for (auto& File : std::filesystem::directory_iterator("."))
	{
		if (std::filesystem::is_regular_file(File) && std::regex_match(File.path().filename().string(), Regex))
		{
			std::string suffix_str = File.path().filename().string().substr(BaseName.size());
			if (!suffix_str.empty())
			{
				if (int Suffix = std::stoi(suffix_str.substr(1)); Suffix > MaxSuffix)
				{
					MaxSuffix = Suffix;
				}
			}
		}
	}

	return MaxSuffix;
}

void EditTransformWithGizmo(const float* View, const float* Projection, float* Matrix)
{
	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
	static ImGuizmo::MODE MCurrentGizmoMode(ImGuizmo::WORLD);

	ImGuiIO& io = ImGui::GetIO();

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
	ImGuizmo::DecomposeMatrixToComponents(Matrix, matrixTranslation, matrixRotation, matrixScale);
	ImGui::InputFloat3("Translation", matrixTranslation);
	ImGui::InputFloat3("Rotation", matrixRotation);
	ImGui::InputFloat3("Scale", matrixScale);
	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, Matrix);

	if (mCurrentGizmoOperation != ImGuizmo::SCALE)
	{
		if (ImGui::RadioButton("Local", MCurrentGizmoMode == ImGuizmo::LOCAL))
		{
			MCurrentGizmoMode = ImGuizmo::LOCAL;
		}

		if (ImGui::RadioButton("World", MCurrentGizmoMode == ImGuizmo::WORLD))
		{
			MCurrentGizmoMode = ImGuizmo::WORLD;
		}
	}


	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::Manipulate(View, Projection, mCurrentGizmoOperation, MCurrentGizmoMode, Matrix, NULL, NULL);

}

void HorusEngine::Close()
{
	HorusOpenGL& OpenGL = HorusOpenGL::GetInstance();
	HorusImGuiCore& ImGuiCore = HorusImGuiCore::GetInstance();
	HorusRadeon& Radeon = HorusRadeon::GetInstance();

	Radeon.QuitRender();
	ImGuiCore.QuitRender();
	OpenGL.QuitRender();

	m_IsRunning_ = false;
}

void HorusEngine::CallResetBuffer()
{
	HorusResetBuffers::GetInstance().CallResetBuffers();
}



