#pragma once

// Basic includes
#include <cassert>
#include <iostream>
#include <map>

// Project includes
#include <RadeonProRender.hpp>
#include <Math/mathutils.h>

// External includes
#include "picojson.h"
#define TINYOBJLOADER_IMPLEMENTATION

#include "tiny_obj_loader.h"
#include "glm/glm.hpp"

// In developpment

template<typename T>
inline
void Fill(const picojson::object& Value, const char* Key, T& DST)
{
	auto C = Value.find(Key);
	if (C != Value.end())
	{
		const picojson::value& D = C->second;
		DST = static_cast<T>(D.get<T>());
	}
}

template<>
inline
void Fill(const picojson::object& Value, const char* Key, RadeonProRender::float3& Dst)
{
	auto C = Value.find(Key);
	if (C != Value.end())
	{
		int J = 0;
		const picojson::value& D = C->second;
		const picojson::array& A = D.get<picojson::array>();
		for (picojson::array::const_iterator I = A.begin(); I != A.end(); ++I)
		{
			Dst[J++] = std::stof(I->to_str());
		}
	}
}

template<>
inline
void Fill(const picojson::object& Value, const char* Key, RadeonProRender::float4& Dst)
{
	auto C = Value.find(Key);
	if (C != Value.end())
	{
		int J = 0;
		const picojson::value& D = C->second;
		const picojson::array& A = D.get<picojson::array>();
		for (picojson::array::const_iterator I = A.begin(); I != A.end(); ++I)
		{
			Dst[J++] = std::stof(I->to_str());
		}
	}
}

template<>
inline
void Fill(const picojson::object& Value, const char* Key, float& Dst)
{
	auto C = Value.find(Key);
	if (C != Value.end())
	{
		const picojson::value& D = C->second;
		Dst = static_cast<double>(D.get<double>());
	}
}

// All struct
struct Context
{
	rpr_context RprContext;
	rpr_scene RprScene;
};

struct ContextSettings
{
	rpr_render_mode RenderMode = RPR_RENDER_MODE_GLOBAL_ILLUMINATION;
	rpr_uint Recursion = 10;
	rpr_uint Width = 800;
	rpr_uint Height = 600;
	rpr_uint Iterations = 64;
	rpr_uint BatchSize = 32;
	std::string OutImageFile = "Output.png";
	std::string OutJsonFile = "Output.json";
	rpr_uint Gui = 0;
	rpr_creation_flags CreationFlags = RPR_CREATION_FLAGS_ENABLE_GPU0;
};

struct CameraSettings
{
	glm::vec3 Position = glm::vec3(0.0f, 9.0f, 40.0f);
	glm::vec3 LookAt = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
	rpr_float Fov = 45.0f;
	glm::vec3 Translation = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 Rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	rpr_camera_mode CameraMode = RPR_CAMERA_MODE_PERSPECTIVE;
};

struct LightSettings
{
	std::string Type = "point";
	glm::vec3 Translation = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 Rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 Power = glm::vec3(50.0f, 50.0f, 50.0f);
	rpr_float Intensity = 10.0f;
};

struct EnvLightSettings
{
	std::string ImagePath = "env.hdr";
	rpr_float Intensity = 1.0f;
};

struct ShapeSettings
{
	std::string Name = "mesh";
	std::string MeshPath = "/home/alex/Downloads/teapot.obj";
	glm::vec3 Translation = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 Rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 Scale = glm::vec3(0.f, 0.f, 0.f);
};

struct Configuration
{
	ContextSettings ContextSettings;
	CameraSettings CameraSettings;
	std::vector<LightSettings> LightSettings;
	std::vector<ShapeSettings> ShapeSettings;
	std::vector<EnvLightSettings> EnvLightSettings;
};

class HorusSceneManager
{
public:

	// https://github.com/GPUOpen-LibrariesAndSDKs/RadeonProRenderSDK/tree/master/tutorials/64_mesh_obj_demo

	static HorusSceneManager& GetInstance()
	{
		static HorusSceneManager Instance;
		return Instance;
	}

	HorusSceneManager(HorusSceneManager const&) = delete;
	void operator=(HorusSceneManager const&) = delete;

	void Init();

	const ContextSettings ParseContextSettings(const picojson::value& Value)
	{
		ContextSettings Settings;
		auto Context = Value.get<picojson::object>();

		std::string RenderMode;
		Fill(Context, "RenderMode", RenderMode);
		Settings.RenderMode = m_RenderModeMap_[RenderMode];

		std::string Recursion;
		Fill(Context, "Recursion", Recursion);
		Settings.Recursion = std::stoi(Recursion);

		std::string Width;
		Fill(Context, "Width", Width);
		Settings.Width = std::stoi(Width);

		std::string Height;
		Fill(Context, "Height", Height);
		Settings.Height = std::stoi(Height);

		std::string Iterations;
		Fill(Context, "Iterations", Iterations);
		Settings.Iterations = std::stoi(Iterations);

		std::string BatchSize;
		Fill(Context, "BatchSize", BatchSize);
		Settings.BatchSize = std::stoi(BatchSize);

		Fill(Context, "OutImageFile", Settings.OutImageFile);
		Fill(Context, "OutJsonFile", Settings.OutJsonFile);

		std::string Gui;
		Fill(Context, "Gui", Gui);
		Settings.Gui = std::stoi(Gui);

		std::string Cpu;
		std::string Gpu;

		auto C = Context.find("Device")->second.get<picojson::object>();

		Fill(C, "GPU0", Cpu);
		Fill(C, "CPU", Cpu);

		if (std::stoi(Gpu))
		{
			Settings.CreationFlags = m_CreationModesMap_[C.find("GPU0")->first];
		}

		if (std::stoi(Cpu))
		{
			Settings.CreationFlags != RPR_CREATION_FLAGS_ENABLE_CPU;
		}

#if defined(USING_NORTHSTAR)
		Settings.CreationFlags |= RPR_CREATION_FLAGS_ENABLE_METAL;
#endif

		return Settings;
	}
	const CameraSettings ParseCameraSettings(const picojson::value& Value)
	{
		CameraSettings Settings;
		auto Camera = Value.get<picojson::object>();

		Fill(Camera, "Position", Settings.Position);
		Fill(Camera, "LookAt", Settings.LookAt);
		Fill(Camera, "Up", Settings.Up);
		Fill(Camera, "Fov", Settings.Fov);
		Fill(Camera, "Translation", Settings.Translation);
		Fill(Camera, "Rotation", Settings.Rotation);

		std::string CameraMode;
		Fill(Camera, "Type", CameraMode);
		Settings.CameraMode = m_CameraModeMap_[CameraMode];

		return Settings;
	}
	const LightSettings ParseLightSettings(const picojson::value& Value)
	{
		LightSettings Settings;
		auto Light = Value.get<picojson::object>();

		Fill(Light, "Type", Settings.Type);
		Fill(Light, "Translation", Settings.Translation);
		Fill(Light, "Rotation", Settings.Rotation);
		Fill(Light, "Power", Settings.Power);
		Fill(Light, "Intensity", Settings.Intensity);

		return Settings;
	}
	const EnvLightSettings ParseEnvLightSettings(const picojson::value& Value)
	{
		EnvLightSettings Settings;
		std::string ImagePath;

		auto EnvLight = Value.get<picojson::object>();
		Fill(EnvLight, "ImagePath", ImagePath);
		Settings.ImagePath = ImagePath;

		return Settings;
	}
	const ShapeSettings ParseShapeSettings(const picojson::value& Value)
	{
		ShapeSettings Settings;
		auto Shape = Value.get<picojson::object>();

		Fill(Shape, "Name", Settings.Name);
		Fill(Shape, "Path", Settings.MeshPath);
		Fill(Shape, "Translation", Settings.Translation);
		Fill(Shape, "Rotation", Settings.Rotation);
		Fill(Shape, "Scale", Settings.Scale);

		return Settings;
	}









private:

	HorusSceneManager() = default;

	std::map<std::string, rpr_creation_flags> m_CreationModesMap_;
	std::map<std::string, rpr_render_mode> m_RenderModeMap_;
	std::map<std::string, rpr_camera_mode> m_CameraModeMap_;


};
