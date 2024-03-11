#pragma once

// C++ Standard Library
#include <map>
#include <string>
#include <vector>

// Radeon ProRender
#include "RadeonProRender_v2.h"

#include "hrs_console.h"
#include "hrs_object_manager.h"
#include "hrs_om_id_manager.h"
#include "hrs_opengl_shape.h"
#include "hrs_opengl_camera.h"

struct HorusGroupShapeData
{
	std::string GroupShapeName;
	std::vector<HorusShape> GroupShape;

	glm::mat4 Transform = glm::mat4(1.0f);
	glm::vec3 PositionGroup = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 RotationGroup = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 ScaleGroup = glm::vec3(1.0f, 1.0f, 1.0f);

	int GroupShapeTotalCount = 0;
	int OpenGlShapeCount = 0;
	int RadeonShapeCount = 0;
};

class HorusGroupShapeManager
{
public:

	static HorusGroupShapeManager& GetInstance()
	{
		static HorusGroupShapeManager Instance;
		return Instance;
	}

	HorusGroupShapeManager(HorusGroupShapeManager const&) = delete;
	void operator=(HorusGroupShapeManager const&) = delete;

	int CreateGroupShape(const std::string& GroupShapeName, const std::string& Path);
	void InitGroupShape(int GroupShapeID, const std::string& Path);
	HorusGroupShapeData GetGroupShape(int GroupShapeID);

	void DrawAllGroupShapeInOpenGl(GLuint ProgramID, HorusOpenGLCamera& Camera);

	int GetActiveGroupShapeID() const { return mActiveGroupShapeID_; }
	int GetGroupShapeCount() const { return mGroupShapeCount_; }

	// Getters
	std::string GetGroupShapeName(int GroupShapeID);
	std::vector<std::string> GetGroupShapeOpenGlNames(int GroupShapeID);
	std::vector<std::string> GetGroupShapeRadeonNames(int GroupShapeID);

	std::vector<HorusOpenGlShape> GetOpenGlShape(int GroupShapeID);
	std::vector<rpr_shape> GetRadeonShape(int GroupShapeID);

	glm::mat4 GetGroupShapeTransform(int GroupShapeID);
	glm::vec3 GetGroupShapePosition(int GroupShapeID);
	glm::vec3 GetGroupShapeRotation(int GroupShapeID);
	glm::vec3 GetGroupShapeScale(int GroupShapeID);

private:

	HorusGroupShapeManager() = default;

	std::map<int, HorusGroupShapeData> mGroupShapes_;
	int mGroupShapeCount_ = 0;
	int mActiveGroupShapeID_ = 0;

};


inline int HorusGroupShapeManager::CreateGroupShape(const std::string& GroupShapeName, const std::string& Path)
{
	HorusIdManager& kIdManager = HorusIdManager::GetInstance();
	std::string kGroupShapeName = GroupShapeName;
	int kSuffix = 001;

	spdlog::info("Creating group shape {}", kGroupShapeName);

	while (kIdManager.ContainsName(kGroupShapeName))
	{
		kGroupShapeName = kGroupShapeName + std::to_string(kSuffix);
		kSuffix++;

		spdlog::info("Mesh name already exists, trying {} instead", kGroupShapeName);
	}

	int kGroupShapeID = kIdManager.GetNewId();

	HorusGroupShapeData& kGroupShape = mGroupShapes_[kGroupShapeID];
	kGroupShape.GroupShapeName = kGroupShapeName;
	kGroupShape.GroupShape.emplace_back();

	InitGroupShape(kGroupShapeID, Path);

	mActiveGroupShapeID_ = kGroupShapeID;
	spdlog::info("Group shape {} created with ID {}", kGroupShapeName, kGroupShapeID);
	return kGroupShapeID;
}

inline void HorusGroupShapeManager::InitGroupShape(int GroupShapeID, const std::string& Path)
{
	if (mGroupShapes_.find(GroupShapeID) == mGroupShapes_.end())
	{
		spdlog::error("Group shape ID {} does not exist", GroupShapeID);
		return;
	}

	HorusGroupShapeData& kGroupShape = mGroupShapes_[GroupShapeID];
	for (auto& kShape : kGroupShape.GroupShape)
	{
		kShape.Init(Path.c_str());
	}

	spdlog::info("Group shape {} initialized", kGroupShape.GroupShapeName);
}

inline void HorusGroupShapeManager::DrawAllGroupShapeInOpenGl(GLuint ProgramID, HorusOpenGLCamera& Camera)
{
	for (auto& kGroupShapes : mGroupShapes_)
	{
		for (auto& kShape : kGroupShapes.second.GroupShape)
		{
			for (auto& kOpenGlShape : kShape.GetOpenGlShapeAndName())
			{
				HorusOpenGlShape& kShape = std::get<0>(kOpenGlShape);
				kShape.Draw(ProgramID, Camera);
			}
		}
	}
}



