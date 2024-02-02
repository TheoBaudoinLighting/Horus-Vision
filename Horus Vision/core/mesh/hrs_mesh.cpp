#include "hrs_mesh.h"

#include "hrs_reset_buffers.h"
#include "hrs_importer.h"
#include "hrs_radeon.h"
#include "hrs_object_manager.h"
#include "hrs_console.h"

void ResetBuffer()
{
	HorusResetBuffers::GetInstance().CallResetBuffers();
}

// Group Shape Section
void HorusGroupShape::GetGroupShapeInfo()
{
	/*for (rpr_shape shape : m_Shape_)
	{
		rprShapeGetInfo(shape, RPR_SHAPE_TRANSFORM, sizeof(int), &m_Transform_, nullptr);
	}*/
}
void HorusGroupShape::Init(const char* Path)
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusMeshImporter& MeshImporter = HorusMeshImporter::GetInstance();
	HorusGarbageCollector& Gc = HorusGarbageCollector::GetInstance();
	HorusConsole& Console = HorusConsole::GetInstance();

	m_Shape_ = MeshImporter.LoadMesh(Path);

	GetGroupShapeInfo();

	rpr_scene GScene = ObjectManager.GetScene();

	for (auto& ShapeAndName : m_Shape_)
	{
		rpr_shape shape = std::get<0>(ShapeAndName);
		std::string name = std::get<1>(ShapeAndName);

		CHECK(rprSceneAttachShape(GScene, shape));
		CHECK(rprObjectSetName(shape, name.c_str())); // In testing for now (TODO : If it's working, apply it to all the shapes)
		Gc.Add(shape);
	}

	spdlog::info("Mesh {} loaded", Path);
	Console.AddLog("Mesh %s loaded", Path);

	ResetBuffer();
}
void HorusGroupShape::DestroyGroupShape()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	rpr_scene GScene = ObjectManager.GetScene();

	for (auto& ShapeAndName : m_Shape_)
	{
		rpr_shape shape = std::get<0>(ShapeAndName);
		std::string name = std::get<1>(ShapeAndName);

		CHECK(rprSceneDetachShape(GScene, shape));
		// Don't delete shape, it's done by the garbage collector
	}

	ResetBuffer();
}

std::vector<std::pair<rpr_shape, std::string>> HorusGroupShape::GetShapeAndName()
{
	std::vector<std::pair<rpr_shape, std::string>> ShapesAndNames;

	for (auto& ShapeAndName : m_Shape_)
	{
		rpr_shape shape = std::get<0>(ShapeAndName);
		std::string name = std::get<1>(ShapeAndName);

		ShapesAndNames.push_back(std::make_pair(shape, name));
	}

	return ShapesAndNames;
}

std::vector<rpr_shape> HorusGroupShape::GetShape()
{
	std::vector<rpr_shape> Shapes;

	for (auto& ShapeAndName : m_Shape_)
	{
		rpr_shape shape = std::get<0>(ShapeAndName);

		Shapes.push_back(shape);
	}

	return Shapes;
}
std::vector<std::string> HorusGroupShape::GetShapeName()
{
	std::vector<std::string> Names;

	for (auto& ShapeAndName : m_Shape_)
	{
		std::string name = std::get<1>(ShapeAndName);

		Names.push_back(name);
	}

	return Names;
}

void HorusGroupShape::SetGroupShapePosition(glm::vec3 Pos)
{
	GetGroupShapeInfo();

	m_Position_ = Pos;

	UpdateGroupShapeTransform();
}
void HorusGroupShape::SetGroupShapeRotation(glm::vec3 Rot)
{
	GetGroupShapeInfo();

	m_Rotation_ = Rot;

	UpdateGroupShapeTransform();
}
void HorusGroupShape::SetGroupShapeScale(glm::vec3 Scale)
{
	GetGroupShapeInfo();

	m_Scale_ = Scale;

	UpdateGroupShapeTransform();
}
void HorusGroupShape::SetGroupResetTransform()
{
	GetGroupShapeInfo();

	m_Position_ = glm::vec3(0.0f, 0.0f, 0.0f);
	m_Rotation_ = glm::vec3(0.0f, 0.0f, 0.0f);
	m_Scale_ = glm::vec3(1.0f, 1.0f, 1.0f);

	UpdateGroupShapeTransform();
}

void HorusGroupShape::SetGroupShapeToScene()
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();

	rpr_scene GScene = ObjectManager.GetScene();

	for (auto& ShapeAndName : m_Shape_)
	{
		rpr_shape shape = std::get<0>(ShapeAndName);
		std::string name = std::get<1>(ShapeAndName);

		rprSceneAttachShape(GScene, shape);
	}
}
void HorusGroupShape::UpdateGroupShapeTransform()
{
	glm::mat4 Translate = translate(glm::mat4(1.0f), m_Position_);
	glm::mat4 RotationX = rotate(glm::mat4(1.0f), glm::radians(m_Rotation_.x), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 RotationY = rotate(glm::mat4(1.0f), glm::radians(m_Rotation_.y), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 RotationZ = rotate(glm::mat4(1.0f), glm::radians(m_Rotation_.z), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 Scale = scale(glm::mat4(1.0f), m_Scale_);

	glm::mat4 Rotation = RotationX * RotationY * RotationZ;

	m_Transform_ = Translate * Rotation * Scale;

	

	//rprShapeSetTransform(m_Shape_, RPR_FALSE, &m_Transform_[0][0]); // RPR_TRUE = transpose matrix in row
}

// Shape Section
rpr_shape HorusGroupShape::GetShape(const std::string& shapeName)
{
	for (const auto& ShapeTuple : m_Shape_)
	{
		if (std::get<1>(ShapeTuple) == shapeName)
		{
			return std::get<0>(ShapeTuple);
		}
	}
	return nullptr;
}


