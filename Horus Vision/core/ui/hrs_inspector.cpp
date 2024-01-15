
#include "hrs_inspector.h"
#include "hrs_radeon.h" // glfw3.h
#include "hrs_object_manager.h" // glfw3.h

// External includes
#include "L2DFileDialog.h"
#include <hrs_console.h>

void HorusInspector::Inspector(bool* p_open)
{
	ImGui::Begin("Inspector", p_open);

	switch (SelectionType)
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
	case InspectorType::SCENE:
		InspectorScene();
		break;
	case InspectorType::RENDER:
		InspectorRender();
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
	SelectionType = type;
}

void HorusInspector::InspectorCamera()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::get_instance();

	ImGui::Text("Camera Inspector");
	ImGui::Separator();

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.0f, 1.0f));

	// Basic attributes
	ImGui::Text("Basic attributes");
	ImGui::PopStyleColor();

	// Test functions

	if (ImGui::Button("Print camera info"))
	{
		ObjectManager.PrintCameraInfo(ObjectManager.get_active_camera_id());
	}

	if (ImGui::Button("Reset camera"))
	{
		ObjectManager.ResetCamera(ObjectManager.get_active_camera_id());

		CallResetBuffer();
	}

	ImGui::Separator();

	if (ImGui::Button("Move Camera Forward"))
	{
		ObjectManager.move_camera_forward(ObjectManager.get_active_camera_id());

		CallResetBuffer();
	}

	if (ImGui::Button("Move Camera Backward"))
	{
		ObjectManager.move_camera_backward(ObjectManager.get_active_camera_id());

		CallResetBuffer();
	}

	if (ImGui::Button("Move Camera Left"))
	{
		ObjectManager.move_camera_left(ObjectManager.get_active_camera_id());

		CallResetBuffer();
	}

	if (ImGui::Button("Move Camera Right"))
	{
		ObjectManager.move_camera_right(ObjectManager.get_active_camera_id());

		CallResetBuffer();
	}

	if (ImGui::Button("Move Camera Up"))
	{
		ObjectManager.move_camera_up(ObjectManager.get_active_camera_id());

		CallResetBuffer();
	}

	if (ImGui::Button("Move Camera Down"))
	{
		ObjectManager.move_camera_down(ObjectManager.get_active_camera_id());

		CallResetBuffer();
	}

	if (ImGui::SliderFloat("Changer Pitch Camera", &m_pitch_, -1.0f, 1.0f))
	{
		ObjectManager.SetPitch(ObjectManager.get_active_camera_id(), m_pitch_);

		CallResetBuffer();
	}

	if (ImGui::SliderFloat("Changer Heading Camera", &m_heading_, -1.0f, 1.0f))
	{
		ObjectManager.SetHeading(ObjectManager.get_active_camera_id(), m_heading_);

		CallResetBuffer();
	}





	// End test functions

	static float translation[3] = { 0.0f, 0.0f, 0.0f };
	ImGui::InputFloat3("Translation", translation);

	static float rotation[3] = { 0.0f, 0.0f, 0.0f };
	ImGui::InputFloat3("Rotation", rotation);

	static float scale[3] = { 1.0f, 1.0f, 1.0f };
	ImGui::InputFloat3("Scale", scale);

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// Advanced attributes
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.0f, 1.0f));
	ImGui::Text("Advanced Attributes");
	ImGui::PopStyleColor();
	static float fov = 45.0f;
	ImGui::SliderFloat("FOV", &fov, 1.0f, 180.0f);
	ImGui::SameLine(); ShowhelpMarker("Field of View of the camera.");

	static float focalLength = 50.0f;
	ImGui::InputFloat("Focal Length", &focalLength);
	ImGui::SameLine(); ShowhelpMarker("Focal length of the camera lens.");

	static float nearClip = 0.1f;
	static float farClip = 1000.0f;
	ImGui::InputFloat("Near Clip", &nearClip);
	ImGui::InputFloat("Far Clip", &farClip);

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// Options de DOF
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.0f, 1.0f));
	ImGui::Text("Depth of Field");
	ImGui::PopStyleColor();
	static bool enableDOF = false;
	ImGui::Checkbox("Enable DOF", &enableDOF);

	if (enableDOF) {
		static float aperture = 2.8f;
		ImGui::SliderFloat("Aperture", &aperture, 1.0f, 22.0f);
		ImGui::SameLine(); ShowhelpMarker("Aperture setting for DOF effect.");

		static float focusDistance = 10.0f;
		ImGui::SliderFloat("Focus Distance", &focusDistance, 0.1f, 100.0f);
	}
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
	ImGui::Text("Mesh Inspector");
	ImGui::Separator();

	// Informations Générales
	static char meshName[128] = "MeshName";
	ImGui::InputText("Name", meshName, IM_ARRAYSIZE(meshName));

	// Ces valeurs devraient être remplacées par les vraies données du maillage
	int vertexCount = 0; // Remplacez par la vraie valeur
	int faceCount = 0; // Remplacez par la vraie valeur
	ImGui::Text("Vertices: %d", vertexCount);
	ImGui::Text("Faces: %d", faceCount);

	// Propriétés de Transformation
	static float position[3] = { 0.0f, 0.0f, 0.0f };
	ImGui::InputFloat3("Position", position);

	static float rotation[3] = { 0.0f, 0.0f, 0.0f };
	ImGui::InputFloat3("Rotation", rotation);

	static float scale[3] = { 1.0f, 1.0f, 1.0f };
	ImGui::InputFloat3("Scale", scale);

	// Matériaux (si applicable)
	ImGui::Text("Materials");
	// Ici, ajoutez les contrôles pour afficher et modifier les matériaux associés

	// Opérations sur le Maillage
	if (ImGui::Button("Recalculate Normals")) {
		// Ajoutez l'action de recalcul des normales
	}

	// Ajoutez d'autres opérations ou contrôles si nécessaire

	// Visualisation
	static bool showNormals = false;
	ImGui::Checkbox("Show Normals", &showNormals);
	// Ajoutez d'autres options de visualisation si nécessaire

	ImGui::Spacing();
}

void HorusInspector::InspectorTexture() {
	ImGui::Text("Texture Inspector");
	ImGui::Separator();

	// Aperçu de la Texture
	// Remplacez `textureId` par l'identifiant OpenGL ou un autre identifiant de texture
	GLuint textureId = 0;
	ImGui::Image((void*)(intptr_t)textureId, ImVec2(100, 100));

	// Informations sur la Texture
	static char texturePath[128] = "path/to/texture.png";
	ImGui::InputText("Path", texturePath, IM_ARRAYSIZE(texturePath));

	// Ces valeurs devraient être remplacées par les vraies données de la texture
	int width = 0; // Remplacez par la vraie largeur
	int height = 0; // Remplacez par la vraie hauteur
	ImGui::Text("Resolution: %dx%d", width, height);

	// Paramètres de Mappage
	static bool repeat = true;
	ImGui::Checkbox("Repeat", &repeat);

	static bool mirror = false;
	ImGui::Checkbox("Mirror", &mirror);

	// Options de Mipmap
	static bool useMipmaps = false;
	ImGui::Checkbox("Use Mipmaps", &useMipmaps);

	// Modifications et Actions
	if (ImGui::Button("Reload Texture")) {
		// Ajoutez l'action pour recharger la texture
	}
	if (ImGui::Button("Save Changes")) {
		// Ajoutez l'action pour sauvegarder les modifications
	}

	ImGui::Spacing();
}

void HorusInspector::InspectorScene() {
	ImGui::Text("Scene Inspector");
	ImGui::Separator();

	// Hiérarchie de la Scène
	if (ImGui::TreeNode("Scene Hierarchy")) {
		// Ici, itérez sur les objets de la scène et affichez-les
		// Exemple: ImGui::Selectable("Object Name", isSelected);

		ImGui::TreePop();
	}

	ImGui::Spacing();

	// Propriétés de l'Objet Sélectionné
	if (ImGui::CollapsingHeader("Selected Object Properties", ImGuiTreeNodeFlags_DefaultOpen)) {
		// Afficher et modifier les propriétés de l'objet sélectionné
		static float position[3] = { 0.0f, 0.0f, 0.0f };
		ImGui::InputFloat3("Position", position);

		// ... autres propriétés comme la rotation, échelle, etc.
	}

	ImGui::Spacing();

	// Actions sur l'Objet
	if (ImGui::Button("Delete Object")) {
		// Action pour supprimer l'objet
	}
	if (ImGui::Button("Duplicate Object")) {
		// Action pour dupliquer l'objet
	}

	// Ajout d'Objets à la Scène
	if (ImGui::Button("Add New Object")) {
		// Action pour ajouter un nouvel objet à la scène
	}

	// ... autres actions ou fonctionnalités

	ImGui::Spacing();
}

void HorusInspector::InspectorRender()
{

	ImGui::Text("Render Inspector");

}

void HorusInspector::InspectorProjectProperty()

{
	HorusRadeon& Radeon = HorusRadeon::get_instance();
	HorusObjectManager& ObjectManager = HorusObjectManager::get_instance();
	HorusConsole& Console = HorusConsole::get_instance();

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

	//ImGui::Begin("Property Panel");

	// gpu name
	rpr_longlong memory_usage = render_statistics_.gpumem_usage / 1024 / 1024;
	rpr_longlong system_memory_usage = render_statistics_.sysmem_usage / 1024 / 1024;
	rpr_longlong gpu_max_allocation = render_statistics_.gpumem_max_allocation / 1024 / 1024;
	rpr_longlong gpu_total = render_statistics_.gpumem_total / 1024 / 1024;

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
		CallResetBuffer();
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
			CallResetBuffer();
		}
		was_backdrop_image_enabled = true;
	}
	else if (was_backdrop_image_enabled)
	{
		ObjectManager.unset_background_image();
		was_backdrop_image_enabled = false;
		just_disabled_backdrop = true;
		CallResetBuffer();
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
			Console.AddLog(" [info] Transparent background enabled");
			was_transparent_background_enabled = true;
			m_ResetBuffer_ = true;
		}
		else if (!enable_transparent_background && was_transparent_background_enabled)
		{
			rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_TRANSPARENT_BACKGROUND, 0);
			spdlog::info("Transparent background disabled");
			Console.AddLog(" [info] Transparent background disabled");
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
						CallResetBuffer();
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
						CallResetBuffer();
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
						CallResetBuffer();
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
						CallResetBuffer();
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
						rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_MAX_DEPTH_DIFFUSE, maxDepthDiffuse);
						CallResetBuffer();
					}

					static int maxDepthGlossy = 1;
					if (ImGui::SliderInt("Reflection Ray Depth", &maxDepthGlossy, 1, 64))
					{
						rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_MAX_DEPTH_GLOSSY, maxDepthGlossy);
						CallResetBuffer();
					}

					static int maxDepthRefraction = 1;
					if (ImGui::SliderInt("Refraction Ray Depth", &maxDepthRefraction, 1, 64))
					{
						rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_MAX_DEPTH_REFRACTION, maxDepthRefraction);
						CallResetBuffer();
					}

					static int maxDepthGlossyRefraction = 1;
					if (ImGui::SliderInt("Glossy Refraction Ray Depth", &maxDepthGlossyRefraction, 1, 64))
					{
						rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_MAX_DEPTH_GLOSSY_REFRACTION, maxDepthGlossyRefraction);
						CallResetBuffer();
					}

					static int maxDepthShadow = 1;
					if (ImGui::SliderInt("Shadow Ray Depth", &maxDepthShadow, 1, 64))
					{
						rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_MAX_DEPTH_SHADOW, maxDepthShadow);
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
					static int maxRecursion = 4;

					if (ImGui::SliderInt("Trace Depth", &maxRecursion, 1, 64))
					{
						rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_MAX_RECURSION, maxRecursion);
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
						CallResetBuffer();
					}*/

					static int cpuThreadLimit = 4;
					if (ImGui::SliderInt("CPU Thread limit", &cpuThreadLimit, 1, 16))
					{
						rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_CPU_THREAD_LIMIT, cpuThreadLimit);
						CallResetBuffer();
					}

					static int gpuMemoryLimitMB = 1024;
					if (ImGui::SliderInt("GPU Thread limit (MB)", &gpuMemoryLimitMB, 256, 16384))
					{
						rprContextSetParameterByKey1u(Radeon.get_context(), RPR_CONTEXT_GPU_MEMORY_LIMIT, gpuMemoryLimitMB * 1024 * 1024);
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
					Console.AddLog(" [warning] File already exists, renaming to : %s ", filename.c_str());
				}
				else
				{
					suffix = 0;
					filename = userInput + ("_" + std::to_string(Radeon.get_sample_count()) + std::string(formats[export_format]));
					spdlog::info("Render exported with success: {}", filename);
					Console.AddLog(" [success] Render exported with success: %s ", filename.c_str());
				}
			}
			else
			{
				if (std::filesystem::exists(baseFilename + std::string(formats[export_format])))
				{
					suffix++;
					filename = baseFilename + ("_" + std::to_string(suffix) + std::to_string(Radeon.get_sample_count()) + std::string(formats[export_format]));
					spdlog::warn("File already exists, renaming to: {}", filename);
					Console.AddLog(" [warning] File already exists, renaming to : %s ", filename.c_str());
				}
				else
				{
					suffix = 0;
					filename = baseFilename + ("_" + std::to_string(Radeon.get_sample_count()) + std::string(formats[export_format]));
					spdlog::info("Render exported with success: {}", filename);
					Console.AddLog(" [success] Render exported with success: %s ", filename.c_str());
				}
			}

			rpr_int status = rprFrameBufferSaveToFile(Radeon.get_frame_buffer_resolved(), filename.c_str());

			if (status != RPR_SUCCESS)
			{
				spdlog::error("Failed to save frame buffer to file: {}", filename);
				Console.AddLog(" [error] Failed to save frame buffer to file: %s ", filename.c_str());
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

void HorusInspector::ShowhelpMarker(const char* desc)
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
