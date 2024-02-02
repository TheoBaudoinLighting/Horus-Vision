
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

		/*std::vector<std::string> meshes;
		ObjectManager.GetOutlinerGroupShapes(meshes);*/

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

		ImGui::Spacing();

		if (ImGui::Button("Show Property panel"))
		{
			HorusInspector::GetInstance().PopulateSelectedProjectInfos();
			HorusInspector::GetInstance().SetInspectorType(HorusInspector::InspectorType::PROJECT);
		}

		ImGui::Spacing();

		if (ImGui::CollapsingHeader("Debug Tools"))
		{
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
				HorusInspector::GetInstance().SetInspectorType(HorusInspector::InspectorType::GROUPSHAPE);
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


		ObjectManager.UpdateGroupShapeOutlinerData();

		ImGuiTreeNodeFlags TreeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAllColumns;

		if (ImGui::TreeNodeEx("Meshes", TreeFlags))
		{

			// Group of shapes section
			for (auto& OutlinerData = ObjectManager.GetGroupShapeWithShapesAndNames(); const auto & [fst, snd] : OutlinerData)
			{
				bool IsMeshSelected = (fst == m_SelectedMesh_);
				bool IsAnyShapeSelected = std::ranges::find(snd, m_SelectedObject_) != snd.end();

				ImGui::PushStyleColor(ImGuiCol_Text, IsMeshSelected ? IM_COL32(255, 0, 0, 255) : IM_COL32(255, 255, 255, 255));

				ImGuiTreeNodeFlags NodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

				if (IsMeshSelected && !IsAnyShapeSelected)
				{
					NodeFlags |= ImGuiTreeNodeFlags_Selected;
				}

				bool NodeOpen = ImGui::TreeNodeEx(fst.c_str(), NodeFlags);

				if (ImGui::IsItemClicked())
				{
					m_SelectedMesh_ = fst;
					m_SelectedObject_.clear();

					int Id = ObjectManager.GetIdByName(m_SelectedMesh_);
					spdlog::info("Group of shapes id : {}", Id);
					Console.AddLog(" [info] Group of shapes id : %d ", Id);
					spdlog::info("Group of shapes selected : {}", fst);
					Console.AddLog(" [info] Group of shapes selected : %s ", fst.c_str());
					ObjectManager.SetActualSelectedId(Id);
					ObjectManager.SetActiveGroupShape(Id);
					HorusInspector::GetInstance().PopulateSelectedGroupShapeInfos();
					HorusInspector::GetInstance().SetInspectorType(HorusInspector::InspectorType::GROUPSHAPE);
				}

				ImGui::PopStyleColor();

				// Shape Section
				if (NodeOpen)
				{
					for (const auto& ShapeName : snd)
					{
						bool IsShapeSelected = (ShapeName == m_SelectedObject_);

						if (IsShapeSelected)
						{
							m_SelectedMesh_.clear();
						}

						ImGui::PushStyleColor(ImGuiCol_Text, IsShapeSelected ? IM_COL32(255, 0, 0, 255) : IM_COL32(255, 255, 255, 255));

						if (ImGui::Selectable(ShapeName.c_str(), IsShapeSelected))
						{
							m_SelectedObject_ = ShapeName;

							int Id = ObjectManager.GetIdByName(m_SelectedObject_);
							spdlog::info("Shapes id : {}", Id);
							Console.AddLog(" [info] Shapes id : %d ", Id);
							spdlog::info("Shape selected : {}", m_SelectedObject_);
							Console.AddLog(" [info] Shape selected : %s ", m_SelectedObject_.c_str());
							ObjectManager.SetActualSelectedId(Id);
							ObjectManager.SetActiveShapeId(Id);
							HorusInspector::GetInstance().PopulateSelectedShapeInfos();
							HorusInspector::GetInstance().SetInspectorType(HorusInspector::InspectorType::SHAPE);
						}

						ImGui::PopStyleColor();
					}
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}


		ImGui::Separator();

		if (ImGui::TreeNodeEx("Materials", TreeFlags))
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
					HorusInspector::GetInstance().PopulateSelectedMaterialInfos();
					HorusInspector::GetInstance().SetInspectorType(HorusInspector::InspectorType::MATERIAL);
				}
				ImGui::PopStyleColor();
			}
			ImGui::TreePop();
		}

		ImGui::Separator();

		if (ImGui::TreeNodeEx("Cameras", TreeFlags))
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
					HorusInspector::GetInstance().PopulateSelectedCameraInfos();
				}
				ImGui::PopStyleColor();
			}
			ImGui::TreePop();
		}

		ImGui::Separator();

		if (ImGui::TreeNodeEx("Lights", TreeFlags))
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
