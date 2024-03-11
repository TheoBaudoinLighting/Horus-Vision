#include "hrs_mesh.h"


#include "hrs_reset_buffers.h"
#include "hrs_importer.h"
#include "hrs_radeon.h"
#include "hrs_object_manager.h"
#include "hrs_console.h"
#include "hrs_om_id_manager.h"

void ResetBuffer()
{
	HorusResetBuffers::GetInstance().CallResetBuffers();
}

// Group Shape Section
void HorusShape::Init(const char* Path)
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusMeshImporterUtility& MeshUtility = HorusMeshImporterUtility::GetInstance();
	HorusGarbageCollector& Gc = HorusGarbageCollector::GetInstance();
	HorusIdManager& HorusIdManager = HorusIdManager::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	//auto kModel = MeshUtility.GenerateModelData(Path); // Create Model Data from Path

	// Use the model data to create the shape for both OpenGL and Radeon
	///*MeshUtility.CreateOpenGlObject(kModel);
	//MeshUtility.CreateRadeonObject(kModel);*/





	/*for (auto& kMesh : mOpenGlShape_)
	{
		HorusOpenGlShape kShape = std::get<0>(kMesh);
		std::string kName = std::get<1>(kMesh);

		int kShapeID = HorusIdManager.GetNewId();
		HorusIdManager.SetObjectNameToID(kName, kShapeID);
		mOpenGlShapes_[kShapeID] = kShape;

	}

	rpr_scene GScene = ObjectManager.GetScene();
	for (auto& ShapeAndName : mRadeonShape_)
	{
		rpr_shape kShape = std::get<0>(ShapeAndName);
		std::string kName = std::get<1>(ShapeAndName);

		int kShapeID = HorusIdManager.GetNewId();
		HorusIdManager.SetObjectNameToID(kName, kShapeID);
		mRadeonShapes_[kShapeID] = kShape;

		CHECK(rprSceneAttachShape(GScene, kShape));
		CHECK(rprObjectSetName(kShape, kName.c_str()));
		Gc.Add(kShape);
	}*/


	spdlog::info("Mesh {} loaded", Path);
	Console.AddLog("Mesh %s loaded", Path);

	ResetBuffer();
}

void HorusShape::InitOpenGlShape(const char* Path)
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusGarbageCollector& Gc = HorusGarbageCollector::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	std::vector<VertexData> Vertices;
	std::vector<GLuint> Indices;


	spdlog::info("Mesh {} loaded", Path);
	Console.AddLog("Mesh %s loaded", Path);

	ResetBuffer();
}

void HorusShape::DestroyGroupShape()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	rpr_scene GScene = ObjectManager.GetScene();

	for (auto& ShapeAndName : mRadeonShape_)
	{
		rpr_shape shape = std::get<0>(ShapeAndName);
		std::string name = std::get<1>(ShapeAndName);

		CHECK(rprSceneDetachShape(GScene, shape));
	}

	ResetBuffer();
}

std::vector<std::pair<rpr_shape, std::string>> HorusShape::GetShapeAndName()
{
	std::vector<std::pair<rpr_shape, std::string>> ShapesAndNames;

	for (auto& ShapeAndName : mRadeonShape_)
	{
		rpr_shape shape = std::get<0>(ShapeAndName);
		std::string name = std::get<1>(ShapeAndName);

		ShapesAndNames.emplace_back(shape, name);
	}

	return ShapesAndNames;
}

std::vector<std::pair<HorusOpenGlShape, std::string>> HorusShape::GetOpenGlShapeAndName()
{
	std::vector<std::pair<HorusOpenGlShape, std::string>> ShapesAndNames;

	for (auto& ShapeAndName : mOpenGlShape_)
	{
		HorusOpenGlShape shape = std::get<0>(ShapeAndName);
		std::string name = std::get<1>(ShapeAndName);

		ShapesAndNames.emplace_back(shape, name);
	}

	return ShapesAndNames;
}

std::vector<rpr_shape> HorusShape::GetShape()
{
	std::vector<rpr_shape> Shapes;

	for (auto& ShapeAndName : mRadeonShape_)
	{
		rpr_shape shape = std::get<0>(ShapeAndName);

		Shapes.push_back(shape);
	}

	return Shapes;
}
std::vector<std::string> HorusShape::GetShapeName()
{
	std::vector<std::string> Names;

	for (auto& ShapeAndName : mRadeonShape_)
	{
		std::string name = std::get<1>(ShapeAndName);

		Names.push_back(name);
	}

	return Names;
}

void HorusShape::SetGroupShapePosition(glm::vec3 Pos)
{
	m_Position_ = Pos;

	UpdateGroupShapeTransform();
}
void HorusShape::SetGroupShapeRotation(glm::vec3 Rot)
{
	m_Rotation_ = Rot;

	UpdateGroupShapeTransform();
}
void HorusShape::SetGroupShapeScale(glm::vec3 Scale)
{
	m_Scale_ = Scale;

	UpdateGroupShapeTransform();
}
void HorusShape::SetGroupResetTransform()
{
	m_Position_ = glm::vec3(0.0f, 0.0f, 0.0f);
	m_Rotation_ = glm::vec3(0.0f, 0.0f, 0.0f);
	m_Scale_ = glm::vec3(1.0f, 1.0f, 1.0f);

	UpdateGroupShapeTransform();
}

void HorusShape::SetGroupShapeToScene()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	rpr_scene GScene = ObjectManager.GetScene();

	for (auto& ShapeAndName : mRadeonShape_)
	{
		rpr_shape shape = std::get<0>(ShapeAndName);
		std::string name = std::get<1>(ShapeAndName);

		rprSceneAttachShape(GScene, shape);
	}
}

void HorusShape::UpdateGroupShapeTransform()
{
	glm::mat4 Translate = translate(glm::mat4(1.0f), m_Position_);
	glm::mat4 RotationX = rotate(glm::mat4(1.0f), glm::radians(m_Rotation_.x), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 RotationY = rotate(glm::mat4(1.0f), glm::radians(m_Rotation_.y), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 RotationZ = rotate(glm::mat4(1.0f), glm::radians(m_Rotation_.z), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 Scale = scale(glm::mat4(1.0f), m_Scale_);

	glm::mat4 Rotation = RotationX * RotationY * RotationZ;

	m_Transform_ = Translate * Rotation * Scale;

	//rprShapeSetTransform(m_RadeonShape_, RPR_FALSE, &m_Transform_[0][0]); // RPR_TRUE = transpose matrix in row
}

// Shape Section
rpr_shape HorusShape::GetShape(const std::string& shapeName)
{
	for (const auto& ShapeTuple : mRadeonShape_)
	{
		if (std::get<1>(ShapeTuple) == shapeName)
		{
			return std::get<0>(ShapeTuple);
		}
	}
	return nullptr;
}

HorusOpenGlShape HorusShape::GetOpenGlShape(const std::string& shapeName)
{
	for (const auto& ShapeTuple : mOpenGlShape_)
	{
		if (std::get<1>(ShapeTuple) == shapeName)
		{
			return std::get<0>(ShapeTuple);
		}
	}
	return HorusOpenGlShape();
}


