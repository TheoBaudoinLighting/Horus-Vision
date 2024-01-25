#pragma once


#include "RadeonProRender.h"
#include "Math/mathutils.h"
#include "RprLoadStore.h"
#include "ProRenderGLTF.h" 
#include "common.h"

#include <cassert>
#include <string>
#include <iostream>

class HorusImporterManager
{
public:

	static HorusImporterManager& GetInstance()
	{
		static HorusImporterManager Instance;
		return Instance;
	}

	HorusImporterManager(HorusImporterManager const&) = delete;
	void operator=(HorusImporterManager const&) = delete;

	bool ImportGltf(const std::string& Path, rpr_context Context, rpr_material_system Matsys, rpr_scene* Scene);
	bool ExportGltf(const std::string& Path, rpr_context Context, rpr_scene* Scene);

	// TODO : implement RPRS import/export
	/*bool ImportRPRS(const std::string& Path, rpr_context Context, rpr_material_system Matsys, rpr_scene* Scene);
	bool ExportRPRS(const std::string& Path, rpr_context Context, rpr_scene* Scene);*/

private:

	HorusImporterManager() {}

};
