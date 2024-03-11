#pragma once

// Horus includes
#include "hrs_mesh_data.h"

// Consider that a group of meshes
struct ModelData // For multiple meshes
{
	int ModelID = 0;
	std::string ModelName = "";
	std::map<int, MeshData> Meshes;

	// Functions
	int SetModelDataID(int ID);
	void SetModelDataName(const std::string& Name);
};
