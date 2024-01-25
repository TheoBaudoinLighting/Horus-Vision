
#include "hrs_outliner.h" // nothing
#include <hrs_ui.h> // nothing
#include <hrs_object_manager.h> // glfw3.h
#include <hrs_console.h>
#include <hrs_inspector.h>

#include "ImGuizmo.h"

void HorusOutliner::Outliner(bool* p_open)
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	ImGuizmo::BeginFrame();
	{
		ImGui::Begin("Outliner", p_open);
		ImGui::Text("Outliner");

		// toggle button

		std::vector<std::string> meshes;
		ObjectManager.GetOutlinerMeshes(meshes);

		std::vector<std::string> materials;
		ObjectManager.GetOutlinerMaterials(materials);

		std::vector<std::string> cameras;
		ObjectManager.GetOutlinerCameras(cameras);

		std::vector<std::string> lights;
		ObjectManager.GetOutlinerLights(lights);

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Debug Options");

		if (ImGui::CollapsingHeader("Debug Tools"))
		{

			if (ImGui::Button("Show Property panel"))
			{
				HorusInspector::GetInstance().SetInspectorType(HorusInspector::InspectorType::PROJECT);
			}

			if (ImGui::Button("Show Render panel"))
			{
				HorusInspector::GetInstance().SetInspectorType(HorusInspector::InspectorType::RENDER);
			}

			if (ImGui::Button("Show Scene panel"))
			{
				HorusInspector::GetInstance().SetInspectorType(HorusInspector::InspectorType::SCENE);
			}

			if (ImGui::Button("Show Mesh panel"))
			{
				HorusInspector::GetInstance().SetInspectorType(HorusInspector::InspectorType::MESH);
			}

			if (ImGui::Button("Show Material panel"))
			{
				HorusInspector::GetInstance().SetInspectorType(HorusInspector::InspectorType::MATERIAL);
			}

			if (ImGui::Button("Show Camera panel"))
			{
				HorusInspector::GetInstance().SetInspectorType(HorusInspector::InspectorType::CAMERA);
			}

			if (ImGui::Button("Show Light panel"))
			{
				HorusInspector::GetInstance().SetInspectorType(HorusInspector::InspectorType::LIGHT);
			}

			if (ImGui::Button("Show Texture panel"))
			{
				HorusInspector::GetInstance().SetInspectorType(HorusInspector::InspectorType::TEXTURE);
			}
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		if (ImGui::TreeNode("Meshes"))
		{
			for (const auto& mesh : meshes)
			{
				bool IsSelected = (mesh == m_SelectedObject_);

				ImGui::PushStyleColor(ImGuiCol_Text, IsSelected ? IM_COL32(255, 0, 0, 255) : IM_COL32(255, 255, 255, 255));

				if (ImGui::Selectable(mesh.c_str(), IsSelected))
				{
					m_SelectedObject_ = mesh;
					spdlog::info("Mesh selected : {}", mesh);
					Console.AddLog(" [info] Mesh selected : %s ", mesh.c_str());
					int Id = ObjectManager.GetIdByName(mesh);
					spdlog::info("Mesh id : {}", Id);
					Console.AddLog(" [info] Mesh id : %d ", Id);

					ObjectManager.SetActualSelectedId(Id);
					ObjectManager.SetActiveMesh(Id);
					HorusInspector::GetInstance().SetInspectorType(HorusInspector::InspectorType::MESH);
				}
				ImGui::PopStyleColor();
			}
			ImGui::TreePop();
		}

		ImGui::Separator();

		if (ImGui::TreeNode("Materials"))
		{
			for (const auto& material : materials)
			{
				bool IsSelected = (material == m_SelectedObject_);

				ImGui::PushStyleColor(ImGuiCol_Text, IsSelected ? IM_COL32(255, 0, 0, 255) : IM_COL32(255, 255, 255, 255));

				if (ImGui::Selectable(material.c_str(), IsSelected))
				{
					m_SelectedObject_ = material;
					spdlog::info("Material selected: {}", material);
					Console.AddLog(" [info] Material selected : %s ", material.c_str());
					int Id = ObjectManager.GetIdByName(material);
					spdlog::info("Material id: {}", Id);
					Console.AddLog(" [info] Material id : %d ", Id);

					ObjectManager.SetActualSelectedId(Id);
					ObjectManager.SetActiveMaterialId(Id);
					HorusInspector::GetInstance().SetInspectorType(HorusInspector::InspectorType::MATERIAL);
				}
				ImGui::PopStyleColor();
			}
			ImGui::TreePop();
		}

		ImGui::Separator();

		if (ImGui::TreeNode("Cameras"))
		{
			for (const auto& Camera : cameras)
			{
				bool IsSelected = (Camera == m_SelectedObject_);

				ImGui::PushStyleColor(ImGuiCol_Text, IsSelected ? IM_COL32(255, 0, 0, 255) : IM_COL32(255, 255, 255, 255));

				if (ImGui::Selectable(Camera.c_str(), IsSelected))
				{
					m_SelectedObject_ = Camera;
					spdlog::info("Camera selected: {}", Camera);
					Console.AddLog(" [info] Camera selected : %s ", Camera.c_str());
					int Id = ObjectManager.GetIdByName(Camera);
					spdlog::info("Camera id: {}", Id);
					Console.AddLog(" [info] Camera id : %d ", Id);

					ObjectManager.SetActualSelectedId(Id);
					HorusInspector::GetInstance().SetInspectorType(HorusInspector::InspectorType::CAMERA);
					HorusInspector::GetInstance().PopulateCameraInfos();
				}
				ImGui::PopStyleColor();
			}
			ImGui::TreePop();
		}

		ImGui::Separator();

		if (ImGui::TreeNode("Lights"))
		{
			for (const auto& Light : lights)
			{
				bool IsSelected = (Light == m_SelectedObject_);

				ImGui::PushStyleColor(ImGuiCol_Text, IsSelected ? IM_COL32(255, 0, 0, 255) : IM_COL32(255, 255, 255, 255));

				if (ImGui::Selectable(Light.c_str(), IsSelected))
				{
					m_SelectedObject_ = Light;
					spdlog::info("Light selected: {}", Light);
					Console.AddLog(" [info] Light selected : %s ", Light.c_str());
					int Id = ObjectManager.GetIdByName(Light);
					spdlog::info("Light id: {}", Id);
					Console.AddLog(" [info] Light id : %d ", Id);

					ObjectManager.SetActualSelectedId(Id);
					ObjectManager.SetActiveLightId(Id);
					HorusInspector::GetInstance().SetInspectorType(HorusInspector::InspectorType::LIGHT);
				}
				ImGui::PopStyleColor();
			}
			ImGui::TreePop();
		}
		ImGui::End();
	}
}
