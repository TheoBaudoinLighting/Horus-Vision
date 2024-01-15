
#include "hrs_outliner.h" // nothing
#include <hrs_ui.h> // nothing
#include <hrs_object_manager.h> // glfw3.h
#include <hrs_console.h>
#include <hrs_inspector.h>

void HorusOutliner::Outliner(bool* p_open)
{
	HorusObjectManager& ObjectManager = HorusObjectManager::get_instance();
	HorusConsole& Console = HorusConsole::get_instance();

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

		std::vector<std::string> lights;
		ObjectManager.get_outliner_lights(lights);

		if (ImGui::Button("Show Property panel"))
		{
			HorusInspector::get_instance().SetInspectorType(HorusInspector::InspectorType::PROJECT);
		}

		if (ImGui::Button("Show Render panel"))
		{
			HorusInspector::get_instance().SetInspectorType(HorusInspector::InspectorType::RENDER);
		}

		if (ImGui::Button("Show Scene panel"))
		{
			HorusInspector::get_instance().SetInspectorType(HorusInspector::InspectorType::SCENE);
		}

		if (ImGui::Button("Show Mesh panel"))
		{
			HorusInspector::get_instance().SetInspectorType(HorusInspector::InspectorType::MESH);
		}

		if (ImGui::Button("Show Material panel"))
		{
			HorusInspector::get_instance().SetInspectorType(HorusInspector::InspectorType::MATERIAL);
		}

		if (ImGui::Button("Show Camera panel"))
		{
			HorusInspector::get_instance().SetInspectorType(HorusInspector::InspectorType::CAMERA);
		}

		if (ImGui::Button("Show Light panel"))
		{
			HorusInspector::get_instance().SetInspectorType(HorusInspector::InspectorType::LIGHT);
		}

		if (ImGui::Button("Show Texture panel"))
		{
			HorusInspector::get_instance().SetInspectorType(HorusInspector::InspectorType::TEXTURE);
		}

		ImGui::Separator();

		if (ImGui::TreeNode("Meshes"))
		{
			for (const auto& mesh : meshes)
			{
				bool isSelected = (mesh == selectedObject);

				ImGui::PushStyleColor(ImGuiCol_Text, isSelected ? IM_COL32(255, 0, 0, 255) : IM_COL32(255, 255, 255, 255));

				if (ImGui::Selectable(mesh.c_str(), isSelected))
				{
					selectedObject = mesh;
					spdlog::info("Mesh selected : {}", mesh);
					Console.AddLog(" [info] Mesh selected : %s ", mesh.c_str());
					int id = ObjectManager.GetIdByName(mesh);
					spdlog::info("Mesh id : {}", id);
					Console.AddLog(" [info] Mesh id : %d ", id);

					HorusInspector::get_instance().SetInspectorType(HorusInspector::InspectorType::MESH);
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
				bool isSelected = (material == selectedObject);

				ImGui::PushStyleColor(ImGuiCol_Text, isSelected ? IM_COL32(255, 0, 0, 255) : IM_COL32(255, 255, 255, 255));

				if (ImGui::Selectable(material.c_str(), isSelected))
				{
					selectedObject = material;
					spdlog::info("Material selected: {}", material);
					Console.AddLog(" [info] Material selected : %s ", material.c_str());
					int id = ObjectManager.GetIdByName(material);
					spdlog::info("Material id: {}", id);
					Console.AddLog(" [info] Material id : %d ", id);

					HorusInspector::get_instance().SetInspectorType(HorusInspector::InspectorType::MATERIAL);
				}
				ImGui::PopStyleColor();
			}
			ImGui::TreePop();
		}

		ImGui::Separator();

		if (ImGui::TreeNode("Cameras"))
		{
			for (const auto& camera : cameras)
			{
				bool isSelected = (camera == selectedObject);

				ImGui::PushStyleColor(ImGuiCol_Text, isSelected ? IM_COL32(255, 0, 0, 255) : IM_COL32(255, 255, 255, 255));

				if (ImGui::Selectable(camera.c_str(), isSelected))
				{
					selectedObject = camera;
					spdlog::info("Camera selected: {}", camera);
					Console.AddLog(" [info] Camera selected : %s ", camera.c_str());
					int id = ObjectManager.GetIdByName(camera);
					spdlog::info("Camera id: {}", id);
					Console.AddLog(" [info] Camera id : %d ", id);

					HorusInspector::get_instance().SetInspectorType(HorusInspector::InspectorType::CAMERA);
				}
				ImGui::PopStyleColor();
			}
			ImGui::TreePop();
		}

		ImGui::Separator();

		if (ImGui::TreeNode("Lights"))
		{
			for (const auto& light : lights)
			{
				bool isSelected = (light == selectedObject);

				ImGui::PushStyleColor(ImGuiCol_Text, isSelected ? IM_COL32(255, 0, 0, 255) : IM_COL32(255, 255, 255, 255));

				if (ImGui::Selectable(light.c_str(), isSelected))
				{
					selectedObject = light;
					spdlog::info("Light selected: {}", light);
					Console.AddLog(" [info] Light selected : %s ", light.c_str());
					int id = ObjectManager.GetIdByName(light);
					spdlog::info("Light id: {}", id);
					Console.AddLog(" [info] Light id : %d ", id);

					HorusInspector::get_instance().SetInspectorType(HorusInspector::InspectorType::LIGHT);
				}
				ImGui::PopStyleColor();
			}
			ImGui::TreePop();
		}



		// Transform section

		//if (ImGui::CollapsingHeader("Transforms", ImGuiTreeNodeFlags_DefaultOpen))
		//{
		//	ImGui::Text("Transforms");

		//	float viewMatrix[16];
		//	float projectionMatrix[16];

		//	auto io = ImGui::GetIO();

		//	ObjectManager.compute_view_projection_matrix(0, viewMatrix, projectionMatrix, io.DisplaySize.x / io.DisplaySize.y);

		//	RadeonProRender::matrix viewMatrixRPR = ObjectManager.get_mesh_transform(0);

		//	//edit_transform_with_gizmo(viewMatrix, projectionMatrix, (float*)&viewMatrix);




		//	ImGuizmo::Manipulate(viewMatrix, projectionMatrix, ImGuizmo::TRANSLATE, ImGuizmo::WORLD, (float*)&viewMatrixRPR);

		//	if (memcmp(&viewMatrix, &viewMatrixRPR, sizeof(float) * 16) != 0)
		//	{
		//		//ObjectManager.set_mesh_transform(0, viewMatrixRPR);
		//	}


		//}




		ImGui::Separator();

		ImGui::End();

	}
}
