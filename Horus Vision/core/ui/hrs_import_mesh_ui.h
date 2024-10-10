#pragma once

// C++ Standard Library
#include <string>

// Spdlog
#include "hrs_preprocess_meshdata.h"
#include "imgui.h"
#include "glm/glm.hpp"
#include "spdlog/spdlog.h"

class HorusImportMeshUI
{
public:

	static HorusImportMeshUI& GetInstance()
	{
		static HorusImportMeshUI Instance;
		return Instance;
	}

	HorusImportMeshUI(HorusImportMeshUI const&) = delete;
	void operator=(HorusImportMeshUI const&) = delete;

	void ImportMeshUI(bool* p_open);
	bool PreProcessFileInformations(PreProcessMeshData& Data, std::string& kPath);
	void ClearProcessedData(PreProcessMeshData& Data, bool Clear);
	void CreateModelDataWithPreProcessedFile(PreProcessMeshData& Data);

private:

	HorusImportMeshUI() = default;
	std::string mPath_;
	PreProcessMeshData mPreProcessMeshData_;
};




