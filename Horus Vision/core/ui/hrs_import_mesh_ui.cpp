#include "hrs_import_mesh_ui.h"

// Assimp Includes
#include <regex>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/DefaultLogger.hpp>

//#include "commonMetaData.h"
#include "hrs_console.h"
#include "hrs_importer_utility.h"
#include "hrs_import_flags.h"
#include "hrs_om_group_shapes.h"
#include "hrs_platform.h"
#include "hrs_reset_buffers.h"
#include "hrs_utils.h"

void HorusImportMeshUI::ImportMeshUI(bool* p_open)
{
    if (!ImGui::IsPopupOpen("Import Mesh"))
        ImGui::OpenPopup("Import Mesh");
    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_Appearing);
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x / 2 - 300, ImGui::GetIO().DisplaySize.y / 2 - 200 * 1.5), ImGuiCond_Appearing);

    // Extraire le nom du fichier sans l'extension
    std::string windowTitle = "Import Mesh";
    if (!mPath_.empty())
    {
        size_t lastSlashPos = mPath_.find_last_of("/\\");
        if (lastSlashPos != std::string::npos)
        {
            size_t lastDotPos = mPath_.find_last_of(".");
            if (lastDotPos != std::string::npos && lastDotPos > lastSlashPos)
            {
                std::string fileName = mPath_.substr(lastSlashPos + 1, lastDotPos - lastSlashPos - 1);
                windowTitle = "Import Mesh - " + fileName;
            }
            else
            {
                std::string fileName = mPath_.substr(lastSlashPos + 1);
                windowTitle = "Import Mesh - " + fileName;
            }
        }
    }
    

	if (ImGui::BeginPopupModal("Import Mesh", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Import file");
		ImGui::Separator();

		ImGui::Text("Choose file to import");

		static bool kPreProcessAction = false;
		if (ImGui::Button("Browse.."))
		{
			mPath_ = Utils::HorusFileDialog::OpenFile("3D Files (*.fbx;*.obj;*.dae;*.3ds;*.blend;*.stl;*.gltf;*.ply;*.max;*.c4d)\0*.fbx;*.obj;*.dae;*.3ds;*.blend;*.stl;*.gltf;*.ply;*.max;*.c4d\0");
			kPreProcessAction = true;
		}ShowHandCursorOnHover();

		if (kPreProcessAction)
		{
			if (!PreProcessFileInformations(mPreProcessMeshData_, mPath_))
			{
				spdlog::warn("Failed to pre-process file informations");
				ImGui::CloseCurrentPopup();
			}

			spdlog::info("Pre-Process file informations, Path: {}", mPath_);
			kPreProcessAction = false;
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::TextWrapped("Path: %s", mPath_.c_str());

		// Import Options
		ImGui::Spacing();
		ImGui::Separator();
		if (ImGui::CollapsingHeader("Import options", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Leaf))
		{
			ImGui::BeginChild("Import options", ImVec2(0, 200), false);
			{
				ImGui::Columns(2, nullptr, false);
       			 ImGui::SetColumnWidth(0, 200);
				{
					ImGui::Separator();
					ImGui::Text("Import options");
					ImGui::Separator();

						ImGui::Checkbox("Calc Tangents", &mPreProcessMeshData_.bCalcTangentSpace); ShowHandCursorOnHover();
						ImGui::Checkbox("Make Left Handed", &mPreProcessMeshData_.bMakeLeftHanded); ShowHandCursorOnHover();
						ImGui::Checkbox("Remove Component", &mPreProcessMeshData_.bRemoveComponent); ShowHandCursorOnHover();
						ImGui::Checkbox("Gen Normals", &mPreProcessMeshData_.bGenNormals); ShowHandCursorOnHover();
						ImGui::Checkbox("Split Large Meshes", &mPreProcessMeshData_.bSplitLargeMeshes); ShowHandCursorOnHover();
						ImGui::Checkbox("Limit Bone Weights", &mPreProcessMeshData_.bLimitBoneWeights); ShowHandCursorOnHover();
						ImGui::Checkbox("Improve Cache Locality", &mPreProcessMeshData_.bImproveCacheLocality); ShowHandCursorOnHover();
						ImGui::Checkbox("Fix Infacing Normals", &mPreProcessMeshData_.bFixInfacingNormals); ShowHandCursorOnHover();
						ImGui::Checkbox("Sort By PType", &mPreProcessMeshData_.bSortByPType); ShowHandCursorOnHover();
						ImGui::Checkbox("Find Invalid Data", &mPreProcessMeshData_.bFindInvalidData); ShowHandCursorOnHover();
						ImGui::Checkbox("Transform UV Coords", &mPreProcessMeshData_.bTransformUVCoords); ShowHandCursorOnHover();
						ImGui::Checkbox("Optimize Meshes", &mPreProcessMeshData_.bOptimizeMeshes); ShowHandCursorOnHover();
						ImGui::Checkbox("Flip UVs", &mPreProcessMeshData_.bFlipUVs); ShowHandCursorOnHover();
						ImGui::Checkbox("Split By Bone Count", &mPreProcessMeshData_.bSplitByBoneCount); ShowHandCursorOnHover();
						ImGui::Checkbox("Force Gen Normals", &mPreProcessMeshData_.bForceGenNormals); ShowHandCursorOnHover();
						ImGui::Checkbox("Gen Bounding Boxes", &mPreProcessMeshData_.bGenBoundingBoxes); ShowHandCursorOnHover();
						ImGui::Checkbox("Convert Axis", &mPreProcessMeshData_.bConvertAxis); ShowHandCursorOnHover();
				

					ImGui::NextColumn();
						ImGui::Checkbox("Join Identical Vertices", &mPreProcessMeshData_.bJoinIdenticalVertices); ShowHandCursorOnHover();
						ImGui::Checkbox("Triangulate", &mPreProcessMeshData_.bTriangulate);
						ImGui::Checkbox("Gen Normals", &mPreProcessMeshData_.bGenNormals); ShowHandCursorOnHover();
						ImGui::Checkbox("Gen Smooth Normals", &mPreProcessMeshData_.bGenSmoothNormals); ShowHandCursorOnHover();
						ImGui::Checkbox("Pre Transform Vertices", &mPreProcessMeshData_.bPreTransformVertices); ShowHandCursorOnHover();
						ImGui::Checkbox("Validate Data Structure", &mPreProcessMeshData_.bValidateDataStructure); ShowHandCursorOnHover();
						ImGui::Checkbox("Remove Redundant Materials", &mPreProcessMeshData_.bRemoveRedundantMaterials); ShowHandCursorOnHover();
						ImGui::Checkbox("Populate Armature Data", &mPreProcessMeshData_.bPopulateArmatureData); ShowHandCursorOnHover();
						ImGui::Checkbox("Find Degenerates", &mPreProcessMeshData_.bFindDegenerates); ShowHandCursorOnHover();
						ImGui::Checkbox("Gen UV Coords", &mPreProcessMeshData_.bGenUVCoords); ShowHandCursorOnHover();
						ImGui::Checkbox("Find Instances", &mPreProcessMeshData_.bFindInstances); ShowHandCursorOnHover();
						ImGui::Checkbox("Optimize Graph", &mPreProcessMeshData_.bOptimizeGraph); ShowHandCursorOnHover();
						ImGui::Checkbox("Flip Winding Order", &mPreProcessMeshData_.bFlipWindingOrder); ShowHandCursorOnHover();
						ImGui::Checkbox("Debone", &mPreProcessMeshData_.bDebone); ShowHandCursorOnHover();
						ImGui::Checkbox("Global Scale", &mPreProcessMeshData_.bGlobalScale); ShowHandCursorOnHover();
						ImGui::Checkbox("Embed Textures", &mPreProcessMeshData_.bEmbedTextures); ShowHandCursorOnHover();
						ImGui::Checkbox("Drop Normals", &mPreProcessMeshData_.bDropNormals); ShowHandCursorOnHover();
					
				}
			} ImGui::EndChild();

		}

		ImGui::Spacing();
		ImGui::Separator();
		if (ImGui::CollapsingHeader("File content", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Meshes: %d", mPreProcessMeshData_.NumMeshes);

			if (mPreProcessMeshData_.NumMeshes <= 0)
			{
				ImGui::SameLine();
				//ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "< No mesh found >");
				ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.7f, 1.0f), "< No mesh found >");
			}

			ImGui::Text("Materials: %d", mPreProcessMeshData_.NumMaterials);
			if (mPreProcessMeshData_.NumMaterials <= 0)
			{
				ImGui::SameLine();
				//ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "< No material found >");
				ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.7f, 1.0f), "< No material found >");
			}

			ImGui::Text("Textures: %d", mPreProcessMeshData_.NumTextures);
			if (mPreProcessMeshData_.NumTextures <= 0)
			{
				ImGui::SameLine();
				//ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "< No texture found >");
				ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.7f, 1.0f), "< No texture found >");
			}

			ImGui::Text("Lights: %d", mPreProcessMeshData_.NumLights);
			if (mPreProcessMeshData_.NumLights <= 0)
			{
				ImGui::SameLine();
				//ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "< No light found >");
				ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.7f, 1.0f), "< No light found >");
			}

			ImGui::Text("Cameras: %d", mPreProcessMeshData_.NumCameras);
			if (mPreProcessMeshData_.NumCameras <= 0)
			{
				ImGui::SameLine();
				//ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "< No camera found >");
				ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.7f, 1.0f), "< No camera found >");
			}

			ImGui::Text("Animations: %d", mPreProcessMeshData_.NumAnimations);
			if (mPreProcessMeshData_.NumAnimations <= 0)
			{
				ImGui::SameLine();
				//ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "< No animation found >");
				ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.7f, 1.0f), "< No animation found >");
			}

			ImGui::Separator();
			ImGui::Text("Total Vertices: %d", mPreProcessMeshData_.NumTotalVertices);
			ImGui::Text("Total Faces: %d", mPreProcessMeshData_.NumTotalFaces);
			ImGui::Text("Total Indices: %d", mPreProcessMeshData_.NumTotalIndices);
		}

		ImGui::Spacing();
		ImGui::Separator();
		if (ImGui::CollapsingHeader("Import transformations"))
		{
			ImGui::Text("Translate");
			ImGui::SliderFloat3("##Translate", &mPreProcessMeshData_.ImportTranslate.x, -360.0f, 360.0f);

			ImGui::Text("Rotate");
			ImGui::SliderFloat3("##Rotate", &mPreProcessMeshData_.ImportRotate.x, -360.0f, 360.0f);

			ImGui::Text("Scale");
			ImGui::PushID("Scale");
			ImGui::SliderFloat3("##Scale", &mPreProcessMeshData_.ImportScale.x, 0.0f, 10.0f);
			ImGui::SameLine();
			static bool lockScale = false;
			ImGui::Checkbox("##LockScale", &lockScale);
			if (lockScale)
			{
				float scaleMean = (mPreProcessMeshData_.ImportScale.x + mPreProcessMeshData_.ImportScale.y + mPreProcessMeshData_.ImportScale.z) / 3.0f;
				mPreProcessMeshData_.ImportScale = glm::vec3(scaleMean);
			}
			ImGui::PopID();

			ImGui::Text("Uniform Scale");
			ImGui::SliderFloat("##Uniform Scale", &mPreProcessMeshData_.ImportUniformScale.x, 0.0f, 10.0f);
		}

		ImGui::Spacing();
		ImGui::Separator();
		if (ImGui::CollapsingHeader("File Informations"))
		{
			if (mPreProcessMeshData_.FileVersion.empty())
			{
				ImGui::Text("File Version:");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.7f, 1.0f), "Unknown");
			}
			else
			{
				ImGui::Text("File Version :");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "%s", mPreProcessMeshData_.FileVersion.c_str());
			}

			if (mPreProcessMeshData_.FileCreator.empty())
			{
				ImGui::Text("File Creator :");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.7f, 1.0f), "Unknown");
			}
			else
			{
				ImGui::Text("File Creator :");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "%s", mPreProcessMeshData_.FileCreator.c_str());
			}

			if (mPreProcessMeshData_.FileFormat.empty())
			{
				ImGui::Text("File Format :");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.7f, 1.0f), "Unknown");
			}
			else
			{
				ImGui::Text("File Format :");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), "%s", mPreProcessMeshData_.FileFormat.c_str());
			}
		}

		ImGui::Spacing();
		ImGui::Separator();
		if (ImGui::Button("Import"))
		{
			CreateModelDataWithPreProcessedFile(mPreProcessMeshData_);
			ImGui::CloseCurrentPopup();
			*p_open = false;
		} ShowHandCursorOnHover();
		ImGui::SameLine();

		if (ImGui::Button("Clear file"))
		{
			ClearProcessedData(mPreProcessMeshData_, true);
			kPreProcessAction = true;
		}ShowHandCursorOnHover();
		ImGui::SameLine();

		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
			*p_open = false;
		}ShowHandCursorOnHover();
		ImGui::Separator();
		ImGui::EndPopup();
	}
}

inline std::string CleanFileName(const std::string& FileName) {
	std::string kCleanedFileName = std::regex_replace(FileName, std::regex("[^a-zA-Z0-9.]"), "");
	return kCleanedFileName;
}
bool HorusImportMeshUI::PreProcessFileInformations(PreProcessMeshData& Data, std::string& kPath)
{
	HorusConsole& kConsole = HorusConsole::GetInstance();

	if (kPath.empty())
	{
		spdlog::error("Path is empty");
		return false;
	}

	Data.Path = kPath;

	Assimp::Importer kImporter;

	std::vector<ConfigFlagPair> kConfigFlags = {
		{&Data.bCalcTangentSpace, aiProcess_CalcTangentSpace},
		{&Data.bTriangulate, aiProcess_Triangulate},
		{&Data.bJoinIdenticalVertices, aiProcess_JoinIdenticalVertices},
		{&Data.bSortByPType, aiProcess_SortByPType},
		{&Data.bGenNormals, aiProcess_GenNormals},
		{&Data.bGenSmoothNormals, aiProcess_GenSmoothNormals},
		{&Data.bValidateDataStructure, aiProcess_ValidateDataStructure},
		{&Data.bMakeLeftHanded, aiProcess_MakeLeftHanded},
		{&Data.bRemoveComponent, aiProcess_RemoveComponent},
		{&Data.bSplitLargeMeshes, aiProcess_SplitLargeMeshes},
		{&Data.bPreTransformVertices, aiProcess_PreTransformVertices},
		{&Data.bLimitBoneWeights, aiProcess_LimitBoneWeights},
		{&Data.bImproveCacheLocality, aiProcess_ImproveCacheLocality},
		{&Data.bRemoveRedundantMaterials, aiProcess_RemoveRedundantMaterials},
		{&Data.bFixInfacingNormals, aiProcess_FixInfacingNormals},
		{&Data.bPopulateArmatureData, aiProcess_PopulateArmatureData},
		{&Data.bFindDegenerates, aiProcess_FindDegenerates},
		{&Data.bFindInvalidData, aiProcess_FindInvalidData},
		{&Data.bGenUVCoords, aiProcess_GenUVCoords},
		{&Data.bTransformUVCoords, aiProcess_TransformUVCoords},
		{&Data.bFindInstances, aiProcess_FindInstances},
		{&Data.bOptimizeMeshes, aiProcess_OptimizeMeshes},
		{&Data.bOptimizeGraph, aiProcess_OptimizeGraph},
		{&Data.bFlipUVs, aiProcess_FlipUVs},
		{&Data.bFlipWindingOrder, aiProcess_FlipWindingOrder},
		{&Data.bSplitByBoneCount, aiProcess_SplitByBoneCount},
		{&Data.bDebone, aiProcess_Debone},
		{&Data.bGlobalScale, aiProcess_GlobalScale},
		{&Data.bEmbedTextures, aiProcess_EmbedTextures},
		{&Data.bForceGenNormals, aiProcess_ForceGenNormals},
		{&Data.bDropNormals, aiProcess_DropNormals},
		{&Data.bGenBoundingBoxes,}
	};

	// Post Processing Flags
	int kPostProcessFlags = 0;
	//ApplyConfigFlags(kPostProcessFlags, kConfigFlags);

	const aiScene* kScene = kImporter.ReadFile(Data.Path, /*kPostProcessFlags*/ aiProcess_ValidateDataStructure);

	if (!kScene || kScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !kScene->mRootNode)
	{
		spdlog::error("Error with Assimp : {}", kImporter.GetErrorString());
		kConsole.AddLog(" [error] Error with Assimp : %s", kImporter.GetErrorString());
		return false;
	}

	const std::string kFileName = std::filesystem::path(kPath).stem().string();
	CleanFileName(kFileName);
	Data.FileName = kFileName;

	if (kScene->mMetaData == nullptr)
	{
		spdlog::error("No MetaData found");
		kConsole.AddLog(" [error] No MetaData found");
		return false;
	}

	if (kScene->HasMeshes())
		Data.NumMeshes = kScene->mNumMeshes;

	if (kScene->HasMaterials())
		Data.NumMaterials = kScene->mNumMaterials;

	if (kScene->HasTextures())
		Data.NumTextures = kScene->mNumTextures;

	if (kScene->HasLights())
		Data.NumLights = kScene->mNumLights;

	if (kScene->HasCameras())
		Data.NumCameras = kScene->mNumCameras;

	if (kScene->HasAnimations())
		Data.NumAnimations = kScene->mNumAnimations;

	if (kScene->mMetaData != nullptr)
	{
		for (unsigned int i = 0; i < kScene->mMetaData->mNumProperties; i++)
		{
			const aiString* kKey;
			const aiMetadataEntry* kEntry;
			auto kResult = kScene->mMetaData->Get(i, kKey, kEntry);

			if (!kResult)
			{
				spdlog::error("Failed to get property at index {}", i);
				kConsole.AddLog(" [error] Failed to get property at index %d", i);
				continue;
			}

			if (kEntry->mType == aiMetadataType::AI_AISTRING)
			{
				aiString kValue = *static_cast<aiString*>(kEntry->mData);
				std::string kKeyString = kKey->C_Str();
				std::string kValueString = kValue.C_Str();

				if (kKeyString == "SourceAsset_FormatVersion")
					Data.FileVersion = kValueString;
				else if (kKeyString == "SourceAsset_Generator")
					Data.FileCreator = kValueString;
				else if (kKeyString == "SourceAsset_Format")
					Data.FileFormat = kValueString;
			}
		}
	}

	// Vérifier si les informations du fichier sont manquantes et définir des valeurs par défaut
	if (Data.FileVersion.empty())
	{
		Data.FileVersion = "Unknown";
		spdlog::warn("File Version is unknown");
		kConsole.AddLog(" [warn] File Version is unknown");
	}

	if (Data.FileCreator.empty())
	{
		Data.FileCreator = "Unknown";
		spdlog::warn("File Creator is unknown");
		kConsole.AddLog(" [warn] File Creator is unknown");
	}

	if (Data.FileFormat.empty())
	{
		// Essayer de déduire le format du fichier à partir de l'extension
		std::string extension = std::filesystem::path(kPath).extension().string();
		if (!extension.empty()) {
			Data.FileFormat = extension.substr(1); // Enlever le point de l'extension
		} else {
			Data.FileFormat = "Unknown";
		}
		spdlog::warn("File Format is deduced from extension: {}", Data.FileFormat);
		kConsole.AddLog(" [warn] File Format is deduced from extension: %s", Data.FileFormat.c_str());
	}

	if (Data.NumMeshes == 0) {
		spdlog::warn("No Meshes found");
		kConsole.AddLog(" [warn] No Meshes found");
	}
	if (Data.NumMaterials == 0) {
		spdlog::warn("No Materials found");
		kConsole.AddLog(" [warn] No Materials found");
	}
	if (Data.NumTextures == 0) {
		spdlog::warn("No Textures found");
		kConsole.AddLog(" [warn] No Textures found");
	}
	if (Data.NumLights == 0) {
		spdlog::warn("No Lights found");
		kConsole.AddLog(" [warn] No Lights found");
	}
	if (Data.NumCameras == 0) {
		spdlog::warn("No Cameras found");
		kConsole.AddLog(" [warn] No Cameras found");
	}
	if (Data.NumAnimations == 0) {
		spdlog::warn("No Animations found");
		kConsole.AddLog(" [warn] No Animations found");
	}


	for (unsigned int i = 0; i < kScene->mNumMeshes; i++)
	{
		aiMesh* kMesh = kScene->mMeshes[i];
		if (!kMesh)
		{
			spdlog::error("Mesh is null");
			kConsole.AddLog(" [error] Mesh is null");
			continue;
		}
		
		std::vector<unsigned int> kIndices;
		size_t kIndicesIndex = 0;
		for (size_t j = 0; j < kMesh->mNumFaces; j++)
		{
			aiFace kFace = kMesh->mFaces[j];
			for (size_t k = 0; k < kFace.mNumIndices; k++) {

				kIndices.push_back(kFace.mIndices[k]);

				Data.NumTotalIndices++;
				kIndicesIndex++;
			}
		}

		Data.NumTotalVertices += kMesh->mNumVertices;
		Data.NumTotalFaces += kMesh->mNumFaces;

		spdlog::info("Mesh {} has {} vertices, {} faces and {} indices", i, kMesh->mNumVertices, kMesh->mNumFaces, Data.NumTotalIndices);
	}

	spdlog::info("Number of meshes: {}", Data.NumMeshes);
	spdlog::info("Number of vertices: {}", Data.NumTotalVertices);
	spdlog::info("Number of faces: {}", Data.NumTotalFaces);
	spdlog::info("Number of indices: {}", Data.NumTotalIndices);


	return true;
}
void HorusImportMeshUI::ClearProcessedData(PreProcessMeshData& Data, bool Clear)
{
	if (Clear)
	{
		mPath_.clear();
		Data.Path.clear();
	}

	Data.FileVersion.clear();
	Data.FileCreator.clear();
	Data.FileFormat.clear();

	Data.ImportTranslate = glm::vec3(0.0f);
	Data.ImportRotate = glm::vec3(0.0f);
	Data.ImportScale = glm::vec3(1.0f);
	Data.ImportUniformScale = glm::vec3(1.0f);

	Data.bCalcTangentSpace = true;
	Data.bJoinIdenticalVertices = true;
	Data.bMakeLeftHanded = false;
	Data.bTriangulate = true;
	Data.bRemoveComponent = false;
	Data.bGenNormals = true;
	Data.bGenSmoothNormals = true;
	Data.bSplitLargeMeshes = false;
	Data.bPreTransformVertices = false;
	Data.bLimitBoneWeights = false;
	Data.bValidateDataStructure = true;
	Data.bImproveCacheLocality = false;
	Data.bRemoveRedundantMaterials = false;
	Data.bFixInfacingNormals = false;
	Data.bPopulateArmatureData = false;
	Data.bSortByPType = true;
	Data.bFindDegenerates = false;
	Data.bFindInvalidData = false;
	Data.bGenUVCoords = false;
	Data.bTransformUVCoords = false;
	Data.bFindInstances = false;
	Data.bOptimizeMeshes = false;
	Data.bOptimizeGraph = false;
	Data.bFlipUVs = false;
	Data.bFlipWindingOrder = false;
	Data.bSplitByBoneCount = false;
	Data.bDebone = false;
	Data.bGlobalScale = false;
	Data.bEmbedTextures = false;
	Data.bForceGenNormals = false;
	Data.bDropNormals = false;
	Data.bGenBoundingBoxes = false;
	Data.bConvertAxis = false;

	Data.NumMeshes = 0;
	Data.NumMaterials = 0;
	Data.NumTextures = 0;
	Data.NumLights = 0;
	Data.NumCameras = 0;
	Data.NumAnimations = 0;

	Data.NumTotalVertices = 0;
	Data.NumTotalFaces = 0;
	Data.NumTotalIndices = 0;

	spdlog::info("Pre-Processed data cleared");
}

void HorusImportMeshUI::CreateModelDataWithPreProcessedFile(PreProcessMeshData& Data)
{
	HorusMeshImporterUtility& MeshUtility = HorusMeshImporterUtility::GetInstance();
	HorusGroupShapeManager& kGroupShapeManager = HorusGroupShapeManager::GetInstance();

	auto kModel = MeshUtility.GenerateModelData(Data);

	//kGroupShapeManager.CreateGroupShape(Data.FileName, Data.Path);

	ClearProcessedData(mPreProcessMeshData_, true);
	HorusResetBuffers::GetInstance().CallResetBuffers();
}