
#include "hrs_inspector.h"
#include "hrs_radeon.h" // glfw3.h
#include "hrs_object_manager.h" // glfw3.h

#include <string>

// External includes
#include "L2DFileDialog.h"
#include <hrs_console.h>

void HorusInspector::Inspector(bool* p_open)
{
	ImGui::Begin("Inspector", p_open);

	switch (m_SelectionType_)
	{
	case InspectorType::CAMERA:
		InspectorCamera();
		break;
	case InspectorType::LIGHT:
		InspectorLight();
		break;
	case InspectorType::MATERIAL:
		InspectorMaterial();
		break;
	case InspectorType::MESH:
		InspectorMesh();
		break;
	case InspectorType::TEXTURE:
		InspectorTexture();
		break;
	case InspectorType::PROJECT:
		InspectorProjectProperty();
		break;
	default:
		break;
	}

	ImGui::End();

	if (m_ResetBuffer_)
	{
		CallResetBuffer();
		m_ResetBuffer_ = false;
	}
}

void HorusInspector::SetInspectorType(InspectorType type)
{
	m_SelectionType_ = type;
}

void HorusInspector::InspectorCamera()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	//ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "Green");
	//ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.7f, 1.0f), "Pink");

	// Getters Camera
	std::string& CameraName = ObjectManager.GetCameraNameById(ObjectManager.GetActiveCameraId());

	glm::vec3 CamLookAt = GetCameraLookAt();
	glm::vec3 CamPosition = GetCameraPosition();
	glm::vec3 CamRotation = GetCameraRotation();
	glm::vec3 CamScale = GetCameraScale();

	float fov = GetFov();

	// Camera Inspector
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f)); // Green
	ImGui::Text("Camera Inspector");
	ImGui::PopStyleColor();
	ImGui::Separator();

	// Basic attributes
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f)); // Green
	ImGui::Text("General Information");
	ImGui::PopStyleColor();

	// Debug tools
	if (ImGui::CollapsingHeader("Debug Tools"))
	{
		if (ImGui::Button("Print camera info"))
		{
			ObjectManager.PrintCameraInfo(ObjectManager.GetActiveCameraId());
		}

		if (ImGui::Button("Reset camera"))
		{
			ObjectManager.ResetCamera(ObjectManager.GetActiveCameraId());

			CallResetBuffer();
		}

		ImGui::Separator();

		if (ImGui::Button("Move Camera Forward"))
		{
			ObjectManager.MoveCameraForward(ObjectManager.GetActiveCameraId());

			CallResetBuffer();
		}

		ImGui::SameLine();

		if (ImGui::Button("Move Camera Backward"))
		{
			ObjectManager.MoveCameraBackward(ObjectManager.GetActiveCameraId());

			CallResetBuffer();
		}

		if (ImGui::Button("Move Camera Left"))
		{
			ObjectManager.MoveCameraLeft(ObjectManager.GetActiveCameraId());

			CallResetBuffer();
		}

		ImGui::SameLine();

		if (ImGui::Button("Move Camera Right"))
		{
			ObjectManager.MoveCameraRight(ObjectManager.GetActiveCameraId());

			CallResetBuffer();
		}

		if (ImGui::Button("Move Camera Up"))
		{
			ObjectManager.MoveCameraUp(ObjectManager.GetActiveCameraId());

			CallResetBuffer();
		}

		ImGui::SameLine();

		if (ImGui::Button("Move Camera Down"))
		{
			ObjectManager.MoveCameraDown(ObjectManager.GetActiveCameraId());

			CallResetBuffer();
		}

		ImGui::Separator();

		if (ImGui::SliderFloat("Changer Pitch Camera", &m_Pitch_, -1.0f, 1.0f))
		{
			ObjectManager.SetPitch(ObjectManager.GetActiveCameraId(), m_Pitch_);

			CallResetBuffer();
		}

		if (ImGui::SliderFloat("Changer Heading Camera", &m_Heading_, -1.0f, 1.0f))
		{
			ObjectManager.SetHeading(ObjectManager.GetActiveCameraId(), m_Heading_);

			CallResetBuffer();
		}
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// End Debug tools

	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Button("Print camera info"))
		{
			ObjectManager.PrintCameraInfo(ObjectManager.GetActiveCameraId());
		}
		ImGui::SameLine(); ShowHelpMarker("Print the camera info in the console.");

		if (ImGui::Button("Reset camera"))
		{
			ObjectManager.ResetCamera(ObjectManager.GetActiveCameraId());

			CallResetBuffer();
		}
		ImGui::SameLine(); ShowHelpMarker("Reset the transform of the camera.");

		ImGui::Separator();

		if (ImGui::InputFloat3("Look At", &CamLookAt[0]))
		{
			SetCameraLookAt(CamLookAt);

			CallResetBuffer();
		}
		ImGui::SameLine(); ShowHelpMarker("Point the camera is looking at.");

		if (ImGui::InputFloat3("Position", &CamPosition[0]))
		{
			SetCameraPosition(CamPosition);

			CallResetBuffer();
		}

		ImGui::BeginDisabled(true);

		if (ImGui::InputFloat3("Rotation", &CamRotation[0]))
		{
			SetCameraRotation(CamRotation);

			CallResetBuffer();
		}

		if (ImGui::InputFloat3("Scale", &CamScale[0])) 
		{
			float avg_scale = (CamScale[0] + CamScale[1] + CamScale[2]) / 3.0f; // Average scale for link the 3 scale
			CamScale[0] = avg_scale;
			CamScale[1] = avg_scale;
			CamScale[2] = avg_scale;

			SetCameraScale(CamScale);
			CallResetBuffer();
		}

		ImGui::EndDisabled();

		
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// Advanced attributes
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f)); // Green
	ImGui::Text("Advanced attributes");
	ImGui::PopStyleColor();

	ImGui::Separator();

	if (ImGui::SliderFloat("FOV", &fov, 1.0f, 180.0f))
	{
		SetFov(fov);

		CallResetBuffer();
	}
	ImGui::SameLine(); ShowHelpMarker("Focal length of the camera lens.");

	static float nearClip = 0.1f;
	static float farClip = 1000.0f;
	ImGui::InputFloat("Near Clip", &nearClip);
	ImGui::SameLine(); ShowHelpMarker("Near clip plane of the camera.");
	ImGui::InputFloat("Far Clip", &farClip);
	ImGui::SameLine(); ShowHelpMarker("Far clip plane of the camera.");

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f)); // Green
	ImGui::Text("Depth of Field");
	ImGui::PopStyleColor();
	static bool enableDOF = false;
	ImGui::Checkbox("Enable DOF", &enableDOF);

	if (enableDOF) 
	{
		static float aperture = 2.8f;
		ImGui::SliderFloat("Aperture", &aperture, 1.0f, 22.0f);
		ImGui::SameLine(); ShowHelpMarker("Aperture setting for DOF effect.");

		static float focusDistance = 10.0f;
		ImGui::SliderFloat("Focus Distance", &focusDistance, 0.1f, 100.0f); 
		ImGui::SameLine(); ShowHelpMarker("Focus distance for DOF effect.");
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f)); // Green
	ImGui::Text("Camera Infos");
	ImGui::PopStyleColor();

	ImGui::Separator();

	ImGui::Text("Camera id: %d ", ObjectManager.GetActiveCameraId());
	ImGui::Text("Camera name: %s ", CameraName.c_str());
	ImGui::Text("Camera look at: %.2f %.2f %.2f ", CamLookAt.x, CamLookAt.y, CamLookAt.z);
	ImGui::Text("Camera position: %.2f %.2f %.2f ", CamPosition.x, CamPosition.y, CamPosition.z);
	ImGui::Text("Camera rotation: %.2f %.2f %.2f ", CamRotation.x, CamRotation.y, CamRotation.z);
	ImGui::Text("Camera scale: %.2f %.2f %.2f ", CamScale.x, CamScale.y, CamScale.z);
	ImGui::Text("Camera FOV: %.2f ", fov);
	ImGui::Text("Camera near clip: %.2f ", nearClip);
	ImGui::Text("Camera far clip: %.2f ", farClip);

	ImGui::Separator();
}

void HorusInspector::InspectorLight()
{
	ImGui::Text("Light Inspector");
	ImGui::Separator();

	// Type de Lumière
	static int lightType = 0; // 0 = Point, 1 = Spot, 2 = Directional
	ImGui::Combo("Light Type", &lightType, "Point\0Spot\0Directional\0\0");

	// Couleur
	static float color[3] = { 1.0f, 1.0f, 1.0f };
	ImGui::ColorEdit3("Color", color);

	// Intensité
	static float intensity = 1.0f;
	ImGui::SliderFloat("Intensity", &intensity, 0.0f, 10.0f);

	// Position et Direction (si nécessaire)
	static float position[3] = { 0.0f, 0.0f, 0.0f };
	static float direction[3] = { 0.0f, -1.0f, 0.0f };
	if (lightType == 1 || lightType == 2) { // Pour Spot et Directionnelle
		ImGui::InputFloat3("Position", position);
		ImGui::InputFloat3("Direction", direction);
	}

	// Paramètres Spécifiques
	if (lightType == 1) { // Pour Spot
		static float spotAngle = 45.0f;
		ImGui::SliderFloat("Spot Angle", &spotAngle, 0.0f, 180.0f);
	}

	// Activation/Désactivation
	static bool isEnabled = true;
	ImGui::Checkbox("Enabled", &isEnabled);

	// Autres paramètres spécifiques peuvent être ajoutés ici


}

void HorusInspector::InspectorMaterial()
{
	ImGui::Text("Material Inspector");
	ImGui::Separator();

	// Nom du Matériau
	static char materialName[128] = "New Material";
	ImGui::InputText("Name", materialName, IM_ARRAYSIZE(materialName));

	// Propriétés de Couleur
	static float diffuseColor[3] = { 1.0f, 1.0f, 1.0f };
	ImGui::ColorEdit3("Diffuse Color", diffuseColor);

	static float specularColor[3] = { 1.0f, 1.0f, 1.0f };
	ImGui::ColorEdit3("Specular Color", specularColor);

	// Textures (exemples)
	ImGui::Text("Textures");
	static char diffuseTexture[128] = "path/to/texture.png";
	ImGui::InputText("Diffuse Texture", diffuseTexture, IM_ARRAYSIZE(diffuseTexture));

	static char normalTexture[128] = "path/to/normal.png";
	ImGui::InputText("Normal Map", normalTexture, IM_ARRAYSIZE(normalTexture));

	// Propriétés Physiques
	static float opacity = 1.0f;
	ImGui::SliderFloat("Opacity", &opacity, 0.0f, 1.0f);

	static float roughness = 0.5f;
	ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f);

	// Autres Paramètres
	static bool isDoubleSided = false;
	ImGui::Checkbox("Double Sided", &isDoubleSided);

	// Ajoutez ici d'autres paramètres selon les besoins

	ImGui::Spacing();



	{

		//void HorusEngine::ui_material_editor(bool* p_open)
		//{
		//	HorusObjectManager& ObjectManager = HorusObjectManager::get_instance();
		//
		//	//----------------------------------------------------------------------------------------------------------------------
		//
		//
		//
		//	ObjectManager.show_material_editor(ObjectManager.get_material_editor_to_show());
		//
		//	ObjectManager.open_material_editor_browser();
		//
		//	//----------------------------------------------------------------------------------------------------------------------
		//
		//	//ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
		//
		//	//if (!ImGui::Begin("Material editor", p_open))
		//	//{
		//
		//	//	ImGui::End();
		//	//	return;
		//	//}
		//
		//	//if (ImGui::BeginMenuBar())
		//	//{
		//	//	if (ImGui::BeginMenu("File"))
		//	//	{
		//	//		if (ImGui::MenuItem("Ouvrir...")) { /* Fonctionnalit d'ouverture */ }
		//	//		if (ImGui::MenuItem("Save")) { /* Fonctionnalit d'enregistrement */ }
		//	//		if (ImGui::MenuItem("Quit")) { *p_open = false; }
		//	//		ImGui::EndMenu();
		//	//	}
		//
		//	//	ImGui::EndMenuBar();
		//	//}
		//
		//
		//	//ImGuiID dockspace_id = ImGui::GetID("MaterialEditorDockSpace");
		//	//ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
		//
		//
		//	//if (ImGui::Begin("Create", p_open, ImGuiWindowFlags_NoCollapse))
		//	//{
		//
		//	//	std::vector<std::string> m_node_names_ = { "Add", "Blend", "Multiply" };
		//	//	std::vector<const char*> m_node_names_cstr;
		//	//	for (const auto& str : m_node_names_) {
		//	//		m_node_names_cstr.push_back(str.c_str());
		//	//	}
		//
		//	//	int selectedItem = -1;
		//
		//
		//	//	if (ImGui::ListBox("Create node", &selectedItem, m_node_names_cstr.data(), m_node_names_cstr.size()))
		//	//	{
		//	//		if (selectedItem != -1)
		//	//		{
		//	//			std::cout << "Selected item: " << m_node_names_[selectedItem] << std::endl;
		//
		//	//			HorusNodeType type = determineNodeTypeBasedOnSelection(selectedItem);
		//	//			//g_material_editor.add_node(1, type);
		//	//		}
		//	//	}
		//
		//
		//
		//
		//
		//
		//
		//
		//	//	ImGui::End();
		//	//}
		//
		//	//if (ImGui::Begin("Browser", p_open, ImGuiWindowFlags_NoCollapse))
		//	//{
		//
		//	//	ImGui::Text("Navigateur de matriaux ici.");
		//	//	ImGui::End();
		//	//}
		//
		//
		//	//if (ImGui::Begin("Node Editor", p_open, ImGuiWindowFlags_NoCollapse))
		//	//{
		//	//	g_material_editor.update();
		//	//	ImGui::End();
		//	//}
		//
		//	//ImGui::End();
		//}


	}
}

void HorusInspector::InspectorMesh()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	//ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "Green");
	//ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.7f, 1.0f), "Pink");

	// Getters Mesh
	std::string& MeshName = ObjectManager.GetMeshName(ObjectManager.GetActiveMeshId());

	glm::vec3 MeshPosition = ObjectManager.GetMeshPosition(ObjectManager.GetActiveMeshId());
	glm::vec3 MeshRotation = ObjectManager.GetMeshRotation(ObjectManager.GetActiveMeshId());
	glm::vec3 MeshScale = ObjectManager.GetMeshScale(ObjectManager.GetActiveMeshId());


	// Mesh Inspector
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f)); // Green
	ImGui::Text("Mesh Inspector");
	ImGui::PopStyleColor();
	ImGui::Separator();

	// Informations Générales
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f)); // Green
	ImGui::Text("General Information");
	ImGui::PopStyleColor();

	int vertexCount = 0; 
	int faceCount = 0; 
	ImGui::Text("Vertices: %d", vertexCount);
	ImGui::Text("Faces: %d", faceCount);

	// Propriétés de Transformation
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Button("Reset Transform"))
		{
			//ObjectManager.ResetTransform(ObjectManager.GetActiveMeshId());

			CallResetBuffer();
		}
		ImGui::SameLine(); ShowHelpMarker("Reset the transform of the mesh.");

		ImGui::Separator();

		if (ImGui::InputFloat3("Position", &MeshPosition[0]))
		{
			ObjectManager.SetShapePosition(ObjectManager.GetActiveMeshId(), MeshPosition);

			CallResetBuffer();
		}

		if (ImGui::InputFloat3("Rotation", &MeshRotation[0]))
		{
			ObjectManager.SetShapeRotation(ObjectManager.GetActiveMeshId(), MeshRotation);

			CallResetBuffer();
		}

		if (ImGui::InputFloat3("Scale", &MeshScale[0]))
		{
			ObjectManager.SetShapeScale(ObjectManager.GetActiveMeshId(), MeshScale);

			CallResetBuffer();
		}
	}

	ImGui::Text("Materials");
	

	if (ImGui::Button("Recalculate Normals")) 
	{
		
	}
	ImGui::SameLine(); ShowHelpMarker("Recalculate the normals of the mesh.");

	// Visualisation
	static bool showNormals = false;
	ImGui::Checkbox("Show Normals", &showNormals);

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f)); // Green
	ImGui::Text("Mesh Infos");
	ImGui::PopStyleColor();

	ImGui::Separator();

	ImGui::Text("Mesh id: %d ", ObjectManager.GetActiveMeshId());
	ImGui::Text("Mesh name: %s ", MeshName.c_str());
	ImGui::Text("Mesh position: %.2f %.2f %.2f ", MeshPosition.x, MeshPosition.y, MeshPosition.z);
	ImGui::Text("Mesh rotation: %.2f %.2f %.2f ", MeshRotation.x, MeshRotation.y, MeshRotation.z);
	ImGui::Text("Mesh scale: %.2f %.2f %.2f ", MeshScale.x, MeshScale.y, MeshScale.z);
	ImGui::Text("Mesh vertex count: %d ", vertexCount);
	ImGui::Text("Mesh face count: %d ", faceCount);
	ImGui::Text("Mesh materials: %d ", 0);

	ImGui::Separator();
}

void HorusInspector::InspectorTexture() 
{
	ImGui::Text("Texture Inspector");
	ImGui::Separator();

	GLuint textureId = 0;
	ImGui::Image((void*)(intptr_t)textureId, ImVec2(100, 100));

	static char texturePath[128] = "path/to/texture.png";
	ImGui::InputText("Path", texturePath, IM_ARRAYSIZE(texturePath));

	int width = 0;
	int height = 0; 
	ImGui::Text("Resolution: %dx%d", width, height);

	static bool repeat = true;
	ImGui::Checkbox("Repeat", &repeat);

	static bool mirror = false;
	ImGui::Checkbox("Mirror", &mirror);

	static bool useMipmaps = false;
	ImGui::Checkbox("Use Mipmaps", &useMipmaps);

	if (ImGui::Button("Reload Texture")) {
		
	}
	if (ImGui::Button("Save Changes")) {
		
	}

	ImGui::Spacing();
}

void HorusInspector::InspectorProjectProperty()
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f)); // Green
	ImGui::Text("Project Inspector");
	ImGui::PopStyleColor();

	m_ResetBuffer_ = false;

	static char* FileDialogBuffer = nullptr;
	static char Path[500] = "";
	static std::string LastUsedPath;
	static bool WasBackdropImageEnabled = false;
	static bool JustDisabledBackdrop = false;

	m_Gpu00N_[0] = '\0';
	m_Gpu00N_[1] = '\0';

	rprContextGetInfo(Radeon.GetContext(), RPR_CONTEXT_GPU0_NAME, sizeof(m_Gpu00N_), m_Gpu00N_, nullptr);
	rprContextGetInfo(Radeon.GetContext(), RPR_CONTEXT_GPU1_NAME, sizeof(m_Gpu01N_), m_Gpu01N_, nullptr);
	rprContextGetInfo(Radeon.GetContext(), RPR_CONTEXT_RENDER_STATISTICS, sizeof(m_RenderStatistics_), &m_RenderStatistics_, 0);

	//ImGui::Begin("Property Panel");

	// gpu name
	rpr_longlong MemoryUsage = m_RenderStatistics_.gpumem_usage / 1024 / 1024;
	rpr_longlong SystemMemoryUsage = m_RenderStatistics_.sysmem_usage / 1024 / 1024;
	rpr_longlong GpuMaxAllocation = m_RenderStatistics_.gpumem_max_allocation / 1024 / 1024;
	rpr_longlong GpuTotal = m_RenderStatistics_.gpumem_total / 1024 / 1024;

	ImGui::Separator();

	if (ImGui::CollapsingHeader("Render Info"))
	{
		ImGui::Text("GPU name: %s", m_Gpu00N_); // %s is a placeholder for a string
		ImGui::Text("GPU name: %s", m_Gpu01N_);
		ImGui::Separator();
		ImGui::Text("Samples: %d", Radeon.GetSampleCount()); // %d is a placeholder for an integer
		ImGui::Separator();
		ImGui::Text("System memory usage : %d MB", SystemMemoryUsage);
		ImGui::Text("GPU Memory usage : %d MB", MemoryUsage);
		ImGui::Text("GPU max allocation : %d MB", GpuMaxAllocation);
		ImGui::Text("GPU total : %d MB", GpuTotal);
	}

	ImGui::Separator();

	const char* RenderModes[] = {
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
	static int SelectedRenderMode = 1;

	ImGui::Text("Select render mode :");
	if (ImGui::Combo("Render mode", &SelectedRenderMode, RenderModes, IM_ARRAYSIZE(RenderModes)))
	{
		switch (SelectedRenderMode)
		{
		case 0:
			rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_RENDER_MODE, RPR_RENDER_MODE_DIFFUSE);
			break;
		case 1:
			rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_RENDER_MODE, RPR_RENDER_MODE_GLOBAL_ILLUMINATION);
			break;
		case 2:
			rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_RENDER_MODE, RPR_RENDER_MODE_DIRECT_ILLUMINATION);
			break;
		case 3:
			rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_RENDER_MODE, RPR_RENDER_MODE_DIRECT_ILLUMINATION_NO_SHADOW);
			break;
		case 4:
			rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_RENDER_MODE, RPR_RENDER_MODE_WIREFRAME);
			break;
		case 5:
			rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_RENDER_MODE, RPR_RENDER_MODE_MATERIAL_INDEX);
			break;
		case 6:
			rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_RENDER_MODE, RPR_RENDER_MODE_POSITION);
			break;
		case 7:
			rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_RENDER_MODE, RPR_RENDER_MODE_NORMAL);
			break;
		case 8:
			rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_RENDER_MODE, RPR_RENDER_MODE_TEXCOORD);
			break;
		case 9:
			rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_RENDER_MODE, RPR_RENDER_MODE_AMBIENT_OCCLUSION);
			break;
		}
		CallResetBuffer();
	}

	ImGui::Separator();

	ImGui::TextUnformatted("Path: ");
	ImGui::InputText("##path", Path, sizeof(Path));
	ImGui::SameLine();
	if (ImGui::Button("Browse##path"))
	{
		FileDialogBuffer = Path;
		FileDialog::file_dialog_open = true;
		FileDialog::file_dialog_open_type = FileDialog::FileDialogType::OpenFile;
	}

	if (FileDialog::file_dialog_open)
	{
		FileDialog::ShowFileDialog(&FileDialog::file_dialog_open, FileDialogBuffer, sizeof(FileDialogBuffer), FileDialog::file_dialog_open_type);
	}

	// Backdrop image

	if (bool CheckboxbeckdropChanged = ImGui::Checkbox("Background image", &m_EnableBackdropImage_); CheckboxbeckdropChanged && (m_EnableBackdropImage_ != m_PreviousEnableBackdropImage_))
	{
		m_ResetBuffer_ = true;
		m_PreviousEnableBackdropImage_ = m_EnableBackdropImage_;
	}

	if (m_EnableBackdropImage_ == true)
	{
		if (Path != LastUsedPath || JustDisabledBackdrop)
		{
			ObjectManager.SetBackgroundImage(Path);
			LastUsedPath = Path;
			JustDisabledBackdrop = false;
			CallResetBuffer();
		}
		WasBackdropImageEnabled = true;
	}
	else if (WasBackdropImageEnabled)
	{
		ObjectManager.UnsetBackgroundImage();
		WasBackdropImageEnabled = false;
		JustDisabledBackdrop = true;
		CallResetBuffer();
	}

	if (WasBackdropImageEnabled)
	{
		ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "Backdrop image is enabled");
	}
	else
	{
		ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.7f, 1.0f), "Backdrop image is disabled");
	}

	static bool WasTransparentBackgroundEnabled = false;

	if (bool CheckboxTransparentBackgroundChanged = ImGui::Checkbox("Transparent background", &m_EnableTransparentBackground_))
	{
		if (m_EnableTransparentBackground_ && !WasTransparentBackgroundEnabled)
		{
			rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_TRANSPARENT_BACKGROUND, 1);
			spdlog::info("Transparent background enabled");
			Console.AddLog(" [info] Transparent background enabled");
			WasTransparentBackgroundEnabled = true;
			m_ResetBuffer_ = true;
		}
		else if (!m_EnableTransparentBackground_ && WasTransparentBackgroundEnabled)
		{
			rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_TRANSPARENT_BACKGROUND, 0);
			spdlog::info("Transparent background disabled");
			Console.AddLog(" [info] Transparent background disabled");
			WasTransparentBackgroundEnabled = false;
			m_ResetBuffer_ = true;
		}
	}

	if (WasTransparentBackgroundEnabled)
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
					ImGui::Text("Min samples: %d", m_MinSamples_);
					ImGui::SameLine();
					if (ImGui::SliderInt("Min samples", &m_MinSamples_, 1, 99))
					{

						if (m_MinSamples_ > m_MaxSamples_)
						{
							m_MinSamples_ = m_MaxSamples_;
						}

						Radeon.SetMinSamples(m_MinSamples_);
						CallResetBuffer();
					}

					ImGui::Text("Max samples: %d", m_MaxSamples_);
					ImGui::SameLine();
					if (ImGui::SliderInt("Max samples", &m_MaxSamples_, 1, 1024))
					{
						if (m_MaxSamples_ < m_MinSamples_)
						{
							m_MaxSamples_ = m_MinSamples_;
						}

						Radeon.SetMaxSamples(m_MaxSamples_);
						CallResetBuffer();
					}

					ImGui::Separator();

					// Russian roulette
					static bool RrParametersChanged = false;

					if (bool CheckboxRrChanged = ImGui::Checkbox("Russian Roulette", &m_EnableRussianRoulette_); CheckboxRrChanged && (m_EnableRussianRoulette_ != m_PreviousEnableRussianRoulette_))
					{
						m_ResetBuffer_ = true;
						m_PreviousEnableRussianRoulette_ = m_EnableRussianRoulette_;
						RrParametersChanged = true;
					}

					if (m_EnableRussianRoulette_)
					{
						ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "Russian roulette is enabled");

						static float RussianRouletteDepth = 5;

						if (ImGui::SliderFloat("Depth", &RussianRouletteDepth, 1.0f, 20.0f))  // Min 1.0, Max 20.0
						{
							rprContextSetParameterByKey1f(Radeon.GetContext(), RPR_CONTEXT_RUSSIAN_ROULETTE_DEPTH, RussianRouletteDepth);
							RrParametersChanged = true;
						}

					}
					else
					{
						ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.7f, 1.0f), "Russian roulette is disabled");
					}

					if (RrParametersChanged)
					{
						CallResetBuffer();
						RrParametersChanged = false;
					}

					ImGui::Separator();

					static bool ParametersChanged = false;

					if (bool CheckboxadsChanged = ImGui::Checkbox("Enable adaptive sampling", &m_EnableAdaptiveSampling_); CheckboxadsChanged && (m_EnableAdaptiveSampling_ != m_PreviousEnableAdaptiveSampling_))
					{
						m_ResetBuffer_ = true;
						m_PreviousEnableAdaptiveSampling_ = m_EnableAdaptiveSampling_;
						ParametersChanged = true;
					}

					if (m_EnableAdaptiveSampling_)
					{
						ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "Adaptive sampling is enabled");
					}
					else
					{
						ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.7f, 1.0f), "Adaptive sampling is disabled");
					}

					if (ParametersChanged)
					{
						if (m_EnableAdaptiveSampling_ == true)
						{
							rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_ADAPTIVE_SAMPLING_TILE_SIZE, 16);
							rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_ADAPTIVE_SAMPLING_MIN_SPP, m_MinSamples_);
							rprContextSetParameterByKey1f(Radeon.GetContext(), RPR_CONTEXT_ADAPTIVE_SAMPLING_THRESHOLD, m_AdaptiveThreshold_);
						}
						else
						{
							rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_ADAPTIVE_SAMPLING_TILE_SIZE, 0);
							rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_ADAPTIVE_SAMPLING_MIN_SPP, 0);
							rprContextSetParameterByKey1f(Radeon.GetContext(), RPR_CONTEXT_ADAPTIVE_SAMPLING_THRESHOLD, 0.0f);
						}

						ParametersChanged = false;
					}

					ImGui::Text("Threshold: %.2f", m_AdaptiveThreshold_);
					ImGui::SameLine();
					if (ImGui::SliderFloat("Threshold", &m_AdaptiveThreshold_, 0.0f, 1.0f))
					{
						CallResetBuffer();
						ParametersChanged = true;
					}
				}

				ImGui::Separator();

				// checkbox for enable preview mode
				static bool PreviewParametersChanged = false;

				if (bool CheckboxChanged = ImGui::Checkbox("Enable preview", &m_EnablePreviewMode_); CheckboxChanged && (m_EnablePreviewMode_ != m_PreviousResetMode_))
				{
					m_ResetBuffer_ = true;
					m_PreviousResetMode_ = m_EnablePreviewMode_;
					PreviewParametersChanged = true;
				}

				if (m_EnablePreviewMode_)
				{
					ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "Preview mode is enabled");
				}
				else
				{
					ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.7f, 1.0f), "Preview mode is disabled");
				}

				if (PreviewParametersChanged)
				{
					if (m_EnablePreviewMode_ == true)
					{
						rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_PREVIEW, 1);
					}
					else
					{
						rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_PREVIEW, 0);
					}

					PreviewParametersChanged = false;
				}

				ImGui::Separator();

				// Anti Aliasing
				static bool AaParametersChanged = false;

				bool CheckboxAaChanged = ImGui::Checkbox("Enable AA", &m_EnableAa_);

				if (CheckboxAaChanged)
				{
					m_ResetBuffer_ = true;
					m_PreviousEnableAa_ = m_EnableAa_;
					AaParametersChanged = true;
				}

				if (m_EnableAa_)
				{
					ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "Anti aliasing is enabled"); // green

				}
				else
				{
					ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.7f, 1.0f), "Anti aliasing is disabled"); // pink

				}

				if (AaParametersChanged)
				{
					if (m_EnableAa_)
					{
						rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_AA_ENABLED, 1);
					}
					else
					{
						rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_AA_ENABLED, 0);
					}

					AaParametersChanged = false;
				}

				const char* Filters[] = { "Box", "Triangle", "Gaussian", "Mitchell", "Lanczos", "Blackmanharris", "None" };
				static int SelectedFilter = 5; // Gaussian by default

				ImGui::Text("Select filter :");
				if (ImGui::Combo("Filter", &SelectedFilter, Filters, IM_ARRAYSIZE(Filters)))
				{
					switch (SelectedFilter)
					{
					case 0:
						rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_IMAGE_FILTER_TYPE, RPR_FILTER_BOX);
						break;
					case 1:
						rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_IMAGE_FILTER_TYPE, RPR_FILTER_TRIANGLE);
						break;
					case 2:
						rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_IMAGE_FILTER_TYPE, RPR_FILTER_GAUSSIAN);
						break;
					case 3:
						rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_IMAGE_FILTER_TYPE, RPR_FILTER_MITCHELL);
						break;
					case 4:
						rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_IMAGE_FILTER_TYPE, RPR_FILTER_LANCZOS);
						break;
					case 5:
						rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_IMAGE_FILTER_TYPE, RPR_FILTER_BLACKMANHARRIS);
						break;
					case 6:
						rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_IMAGE_FILTER_TYPE, RPR_FILTER_NONE);
						break;
					}
					CallResetBuffer();
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
						rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_MAX_DEPTH_DIFFUSE, maxDepthDiffuse);
						CallResetBuffer();
					}

					static int maxDepthGlossy = 1;
					if (ImGui::SliderInt("Reflection Ray Depth", &maxDepthGlossy, 1, 64))
					{
						rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_MAX_DEPTH_GLOSSY, maxDepthGlossy);
						CallResetBuffer();
					}

					static int maxDepthRefraction = 1;
					if (ImGui::SliderInt("Refraction Ray Depth", &maxDepthRefraction, 1, 64))
					{
						rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_MAX_DEPTH_REFRACTION, maxDepthRefraction);
						CallResetBuffer();
					}

					static int maxDepthGlossyRefraction = 1;
					if (ImGui::SliderInt("Glossy Refraction Ray Depth", &maxDepthGlossyRefraction, 1, 64))
					{
						rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_MAX_DEPTH_GLOSSY_REFRACTION, maxDepthGlossyRefraction);
						CallResetBuffer();
					}

					static int maxDepthShadow = 1;
					if (ImGui::SliderInt("Shadow Ray Depth", &maxDepthShadow, 1, 64))
					{
						rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_MAX_DEPTH_SHADOW, maxDepthShadow);
						CallResetBuffer();
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
					static int MaxRecursion = 4;

					if (ImGui::SliderInt("Trace Depth", &MaxRecursion, 1, 64))
					{
						rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_MAX_RECURSION, MaxRecursion);
						CallResetBuffer();
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

				// Tile rendering
				if (ImGui::CollapsingHeader("Tile Rendering", ImGuiTreeNodeFlags_DefaultOpen))
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
						CallResetBuffer();
					}*/

					static int CpuThreadLimit = 4;
					if (ImGui::SliderInt("CPU Thread limit", &CpuThreadLimit, 1, 16))
					{
						rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_CPU_THREAD_LIMIT, CpuThreadLimit);
						CallResetBuffer();
					}

					static int gpuMemoryLimitMB = 1024;
					if (ImGui::SliderInt("GPU Thread limit (MB)", &gpuMemoryLimitMB, 256, 16384))
					{
						rprContextSetParameterByKey1u(Radeon.GetContext(), RPR_CONTEXT_GPU_MEMORY_LIMIT, gpuMemoryLimitMB);
						CallResetBuffer();
					}
				}

				// Global preferences
				if (ImGui::CollapsingHeader("Global Preferences", ImGuiTreeNodeFlags_DefaultOpen))
				{
					

				}

				// Compute devices
				if (ImGui::CollapsingHeader("Compute Devices", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::Text("GPU name: %s", m_Gpu00N_); ImGui::SameLine(); ImGui::Text("GPU name: %s", m_Gpu01N_);

				}

				ImGui::EndTabItem();
			}

		}
		ImGui::EndTabBar();
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 1.0f, 0.6f, 1.0f)); // Green
	ImGui::Text("Export Settings");
	ImGui::PopStyleColor();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	if (ImGui::CollapsingHeader("Export render", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// Export render logic
		static int ExportFormat = 0;
		rpr_char const* Formats[] = { ".png", ".jpeg", ".tiff", ".bmp", ".exr", ".hdr", ".tga" };
		const int NumFormats = std::size(Formats);

		std::string BaseFilename = "HorusRender_ExportedFile";
		std::string Filename = BaseFilename + std::string(Formats[ExportFormat]);

		if (ImGui::BeginCombo("Export format", Formats[ExportFormat]))
		{
			for (int i = 0; i < NumFormats; i++)
			{
				bool IsSelected = (ExportFormat == i);
				if (ImGui::Selectable(Formats[i], IsSelected))
				{
					ExportFormat = i;
				}
				if (IsSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		ImGui::InputText("Filename", m_UserInput_, 256);
		if (ImGui::Button("Export"))
		{
			if (strlen(m_UserInput_) > 0)
			{
				if (std::filesystem::exists(m_UserInput_ + std::string(Formats[ExportFormat])))
				{
					m_Suffix_++;
					Filename = m_UserInput_ + ("_" + std::to_string(m_Suffix_) + "_" + std::to_string(Radeon.GetSampleCount()) + std::string(Formats[ExportFormat]));
					spdlog::warn("File already exists, renaming to: {}", Filename);
					Console.AddLog(" [warning] File already exists, renaming to : %s ", Filename.c_str());
				}
				else
				{
					m_Suffix_ = 0;
					Filename = m_UserInput_ + ("_" + std::to_string(Radeon.GetSampleCount()) + std::string(Formats[ExportFormat]));
					spdlog::info("Render exported with success: {}", Filename);
					Console.AddLog(" [success] Render exported with success: %s ", Filename.c_str());
				}
			}
			else
			{
				if (std::filesystem::exists(BaseFilename + std::string(Formats[ExportFormat])))
				{
					m_Suffix_++;
					Filename = BaseFilename + ("_" + std::to_string(m_Suffix_) + std::to_string(Radeon.GetSampleCount()) + std::string(Formats[ExportFormat]));
					spdlog::warn("File already exists, renaming to: {}", Filename);
					Console.AddLog(" [warning] File already exists, renaming to : %s ", Filename.c_str());
				}
				else
				{
					m_Suffix_ = 0;
					Filename = BaseFilename + ("_" + std::to_string(Radeon.GetSampleCount()) + std::string(Formats[ExportFormat]));
					spdlog::info("Render exported with success: {}", Filename);
					Console.AddLog(" [success] Render exported with success: %s ", Filename.c_str());
				}
			}

			if (rpr_int Status = rprFrameBufferSaveToFile(Radeon.GetFrameBufferResolved(), Filename.c_str()); Status != RPR_SUCCESS)
			{
				spdlog::error("Failed to save frame buffer to file: {}", Filename);
				Console.AddLog(" [error] Failed to save frame buffer to file: %s ", Filename.c_str());
			}
		}

		{
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

	}
	ImGui::Separator();

	//ImGui::End();
}

void HorusInspector::ShowHelpMarker(const char* desc)
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

// Camera Getters
glm::vec3 HorusInspector::GetCameraLookAt()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	int ActiveCamera = ObjectManager.GetActiveCameraId();

	if (ActiveCamera != -1)
	{
		return ObjectManager.GetCameraLookAt(ActiveCamera);
	}
	else
	{
		return {0.0f, 0.0f, 0.0f};
	}
}
glm::vec3 HorusInspector::GetCameraPosition()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	int ActiveCamera = ObjectManager.GetActiveCameraId();

	if (ActiveCamera != -1)
	{
		return ObjectManager.GetCameraPosition(ActiveCamera);
	}
	else
	{
		return {0.0f, 0.0f, 0.0f};
	}

}
glm::vec3 HorusInspector::GetCameraRotation()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	int ActiveCamera = ObjectManager.GetActiveCameraId();

	if (ActiveCamera != -1)
	{
		return ObjectManager.GetCameraRotation(ActiveCamera);
	}
	else
	{
		return {0.0f, 0.0f, 0.0f};
	}
}
glm::vec3 HorusInspector::GetCameraScale()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	int ActiveCamera = ObjectManager.GetActiveCameraId();

	if (ActiveCamera != -1)
	{
		return ObjectManager.GetCameraScale(ActiveCamera);
	}
	else
	{
		return {0.0f, 0.0f, 0.0f};
	}
}
float HorusInspector::GetFov()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	int ActiveCamera = ObjectManager.GetActiveCameraId();

	if (ActiveCamera != -1)
	{
		return ObjectManager.GetCameraFov(ActiveCamera);
	}
	else
	{
		return 0.0f;
	}
}

// Camera Setters
void HorusInspector::SetCameraLookAt(glm::vec3 look_at)
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	int ActiveCamera = ObjectManager.GetActiveCameraId();

	if (ActiveCamera != -1)
	{
		ObjectManager.SetCameraLookat(ActiveCamera, look_at);
	}
	else
	{
		spdlog::error("No camera selected");
		Console.AddLog(" [error] No camera selected");
	}
}
void HorusInspector::SetCameraPosition(glm::vec3 position)
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	int ActiveCamera = ObjectManager.GetActiveCameraId();

	if (ActiveCamera != -1)
	{
		ObjectManager.SetCameraPosition(ActiveCamera, position);
	}
	else
	{
		spdlog::error("No camera selected");
		Console.AddLog(" [error] No camera selected");
	}
}
void HorusInspector::SetCameraRotation(glm::vec3 rotation_axis)
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	int ActiveCamera = ObjectManager.GetActiveCameraId();

	if (ActiveCamera != -1)
	{
		ObjectManager.SetCameraRotation(ActiveCamera, rotation_axis);
	}
	else
	{
		spdlog::error("No camera selected");
		Console.AddLog(" [error] No camera selected");
	}
}
void HorusInspector::SetCameraScale(glm::vec3 scale)
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	int ActiveCamera = ObjectManager.GetActiveCameraId();

	if (ActiveCamera != -1)
	{
		ObjectManager.SetCameraScale(ActiveCamera, scale);
	}
	else
	{
		spdlog::error("No camera selected");
		Console.AddLog(" [error] No camera selected");
	}
}
void HorusInspector::SetFov(float fov)
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	int ActiveCamera = ObjectManager.GetActiveCameraId();

	if (ActiveCamera != -1)
	{
		ObjectManager.SetCameraFov(ActiveCamera, fov);
	}
	else
	{
		spdlog::error("No camera selected");
		Console.AddLog(" [error] No camera selected");
	}
}
