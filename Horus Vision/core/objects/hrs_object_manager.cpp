
#include "hrs_opengl_camera.h"
#include "hrs_object_manager.h"

#include "hrs_om_camera.h"
#include "hrs_om_id_manager.h"
#include "ProRenderGLTF.h"
#include "spdlog/spdlog.h"

// Camera management ------------------------------------------------------------------------------


void HorusObjectManager::SetBackgroundImage(const std::string& path)
{
	m_BackgroundMaterial_.SetBackgroundImage(path);
}
void HorusObjectManager::UnsetBackgroundImage()
{
	m_BackgroundMaterial_.UnsetBackgroundImage();
}

// OpenGL Core Objects ----------------------------------------------------------------------------

int HorusObjectManager::CreateGrid(float Width, float Height, float Slices, const std::string& Name)
{
	HorusIdManager& IdManager = HorusIdManager::GetInstance();
	std::string GridName = Name;

	int Suffix = 001;

	while (IdManager.ContainsName(GridName))
	{
		GridName = Name + std::to_string(Suffix);
		Suffix++;

		spdlog::info("Grid name already exists, trying {} instead", GridName);
	}

	int Id = HorusIdManager::GetInstance().GetNewId();

	HorusGrid& NewGrid = m_Grids_[Id];
	NewGrid.Init(Width, Height, Slices);
	m_GridNames_[Id] = GridName;
	IdManager.SetObjectNameToID(GridName, Id);
	m_ActiveGridId_ = Id;


	spdlog::info("Grid {} created with id: {}", GridName, Id);

	return Id;
}

int HorusObjectManager::GetActiveGridId()
{
	return m_ActiveGridId_;
}

void HorusObjectManager::DrawGrid(int id, GLuint ProgramID, HorusOpenGLCamera& Camera)
{
	m_Grids_[id].Draw(ProgramID, Camera);
}

// Group Shape management -------------------------------------------------------------------------------
int HorusObjectManager::CreateGroupShape(const char* Path, const std::string& Name)
{
	HorusIdManager& kIdManager = HorusIdManager::GetInstance();
	std::string kGroupShapeName = Name;
	int kSuffix = 001;

	// Check the uniqueness of the name
	while (kIdManager.ContainsName(kGroupShapeName))
	{
		kGroupShapeName = Name + std::to_string(kSuffix);
		kSuffix++;

		spdlog::info("Mesh name already exists, trying {} instead", kGroupShapeName);
	}

	int Id = HorusIdManager::GetInstance().GetNewId();

	auto& kNewGroupShape = m_GroupShape_[Id];
	kNewGroupShape.Init(Path);






	spdlog::info("Mesh {} created with id: {}", kGroupShapeName, Id);
	return Id;
}

HorusOpenGlShape HorusObjectManager::CreateSingleOpenGlShape(const char* path, const std::string& name)
{
	HorusIdManager& IdManager = HorusIdManager::GetInstance();
	std::string ShapeName = name;

	int Suffix = 001;

	while (IdManager.ContainsName(ShapeName))
	{
		ShapeName = name + std::to_string(Suffix);
		Suffix++;

		spdlog::info("Shape name already exists, trying {} instead", ShapeName);
	}

	int Id = HorusIdManager::GetInstance().GetNewId();

	std::vector<VertexData> Vertices;
	std::vector<GLuint> Indices;

	std::vector<std::tuple<HorusOpenGlShape, std::string>> m_OpenGlShape_;


	HorusOpenGlShape NewShape;

	//NewShape = m_OpenGlShape_[0];*/

	NewShape.InitOpenGlMesh(Vertices, Indices);
	IdManager.SetObjectNameToID(ShapeName, Id);

	spdlog::info("OpenGL shape {} created with id: {}", ShapeName, Id);

	return NewShape;
}

void HorusObjectManager::DrawAllModel(GLuint ProgramID, HorusOpenGLCamera& Camera)
{
	for (auto& Mesh : m_OpenGlShapes_ | std::views::values)
	{
		Mesh.Draw(ProgramID, Camera);
	}
}

void HorusObjectManager::DestroyGroupShape(int id)
{
	/*if (auto It = m_GroupShape_.find(id); It != m_GroupShape_.end())
	{
		m_GroupShape_[id].DestroyGroupShape();
		m_GroupShape_.erase(It);
		m_RadeonGroupShapeNames_.erase(id);
	}
	else
	{
		spdlog::error("no mesh with this id exists. ");
	}

	for (auto It = m_ObjectNameToIdMap_.begin(); It != m_ObjectNameToIdMap_.end(); ++It) {
		if (It->second == id) {
			m_ObjectNameToIdMap_.erase(It);
			break;
		}
	}*/

	HorusIdManager::GetInstance().ReleaseId(id);
}
void HorusObjectManager::DestroyGroupShapeByName(const char* name)
{
	HorusIdManager& IdManager = HorusIdManager::GetInstance();
	if (!IdManager.ContainsName(name))
	{
		spdlog::error("no mesh with this name exists. ");
	}

	int Id = IdManager.GetIdFromObjectName(name);

	if (auto It = m_GroupShape_.find(Id); It != m_GroupShape_.end())
	{
		m_GroupShape_[Id].DestroyGroupShape();
		m_GroupShape_.erase(It);
		m_RadeonGroupShapeNames_.erase(Id);
	}
	else
	{
		spdlog::error("no mesh with this id exists. ");
	}

	for (auto It = IdManager.GetObjectNameToIdMap().begin(); It != IdManager.GetObjectNameToIdMap().end(); ++It)
	{
		if (It->second == Id)
		{
			IdManager.GetObjectNameToIdMap().erase(It);
			break;
		}
	}

	HorusIdManager::GetInstance().ReleaseId(Id);
}
void HorusObjectManager::DestroyAllGroupShape()
{
	
}

// Getters
std::map<std::string, std::vector<std::string>>& HorusObjectManager::GetGroupShapeWithShapesAndNames()
{
	return m_GroupObjectOutlinerData_;
}
HorusShape& HorusObjectManager::GetGroupShape(int id)
{
	return m_GroupShape_[id];
}
std::string& HorusObjectManager::GetGroupShapeName(int id)
{
	std::string& Name = m_RadeonGroupShapeNames_[id];

	return Name;
}
int HorusObjectManager::GetGroupShapeCount(int count)
{
	return m_GroupShape_.size();
}
int HorusObjectManager::GetGroupShapeId(const char* name)
{
	HorusIdManager& IdManager = HorusIdManager::GetInstance();
	if (!IdManager.GetObjectNameToIdMap().contains(name))
	{
		spdlog::error("no mesh with this name exists. ");
	}

	return IdManager.GetObjectNameToIdMap()[name];
}
int HorusObjectManager::GetActiveGroupShapeId()
{
	return m_ActiveRadeonGroupShapeId_;
}
glm::mat4 HorusObjectManager::GetGroupShapeTransform(int id)
{
	if (!m_GroupShape_.contains(id))
	{
		spdlog::error("no mesh with this id exists. ");
		throw std::runtime_error("no mesh with this id exists. ");
	}

	return m_GroupShape_[id].GetGroupTransform();
}
glm::vec3 HorusObjectManager::GetGroupShapePosition(int id)
{
	HorusShape& Mesh = GetGroupShape(id);

	return Mesh.GetGroupShapePosition();
}
glm::vec3 HorusObjectManager::GetGroupShapeRotation(int id)
{
	HorusShape& Mesh = GetGroupShape(id);

	return Mesh.GetGroupShapeRotation();
}
glm::vec3 HorusObjectManager::GetGroupShapeScale(int id)
{
	HorusShape& Mesh = GetGroupShape(id);

	return Mesh.GetGroupShapeScale();
}

// Setters
void HorusObjectManager::SetActiveGroupShape(int id)
{
	m_ActiveRadeonGroupShapeId_ = id;
}
void HorusObjectManager::SetGroupShapeName(int id, const char* name)
{
	if (!m_GroupShape_.contains(id))
	{
		spdlog::error("no mesh with this id exists. ");
	}

	m_RadeonGroupShapeNames_[id] = name;
}
void HorusObjectManager::SetGroupShapeId(int id, int new_id)
{
	if (!m_GroupShape_.contains(id))
	{
		spdlog::error("no mesh with this id exists. ");
	}

	m_GroupShape_[new_id] = m_GroupShape_[id];
	m_GroupShape_.erase(id);
}
void HorusObjectManager::SetGroupShapePosition(int id, glm::vec3 pos)
{
	if (!m_GroupShape_.contains(id))
	{
		spdlog::error("no mesh with this id exists.");
		return;
	}

	m_GroupShape_[id].SetGroupShapePosition(pos);

	for (auto& shapeId : m_RadeonMeshShapeMap_[m_RadeonGroupShapeNames_[id]])
	{
		/*SetShapePositionById(shapeId, pos);
		auto shape = GetShapeById(shapeId);
		UpdateShapeTransforms(shape);*/
	}
}
void HorusObjectManager::SetGroupShapeRotation(int id, glm::vec3 rot)
{
	HorusIdManager& IdManager = HorusIdManager::GetInstance();
	if (!m_GroupShape_.contains(id))
	{
		spdlog::error("no mesh with this id exists. ");
	}

	m_GroupShape_[id].SetGroupShapeRotation(rot);

	for (auto& Shape : m_RadeonMeshShapeMap_[m_RadeonGroupShapeNames_[id]])
	{
		SetShapeRotationById(IdManager.GetObjectNameToIdMap()[m_RadeonGroupShapeNames_[id]], rot);
		UpdateShapeTransforms(Shape);
	}
}
void HorusObjectManager::SetGroupShapeScale(int id, glm::vec3 scale)
{
	HorusIdManager& IdManager = HorusIdManager::GetInstance();
	if (!m_GroupShape_.contains(id))
	{
		spdlog::error("no mesh with this id exists. ");
	}

	m_GroupShape_[id].SetGroupShapeScale(scale);

	for (auto& Shape : m_RadeonMeshShapeMap_[m_RadeonGroupShapeNames_[id]])
	{
		SetShapeScaleById(IdManager.GetObjectNameToIdMap()[m_RadeonGroupShapeNames_[id]], scale);
		UpdateShapeTransforms(Shape);
	}
}
void HorusObjectManager::SetGroupShapeResetTransform(int id)
{
	if (!m_GroupShape_.contains(id))
	{
		spdlog::error("no mesh with this id exists. ");
	}

	m_GroupShape_[id].SetGroupResetTransform();
}

void HorusObjectManager::UpdateGroupShapeOutlinerData()
{
	HorusIdManager& IdManager = HorusIdManager::GetInstance();
	for (const auto& Key : m_RadeonMeshShapeMap_ | std::views::keys)
	{
		HorusShape& Mesh = m_GroupShape_[IdManager.GetObjectNameToIdMap()[Key]];
		std::vector<std::string> ShapeNames = Mesh.GetShapeName();
		m_GroupObjectOutlinerData_[Key] = ShapeNames;
	}
}

// Shape management -------------------------------------------------------------------------------

int HorusObjectManager::GetActiveShapeId()
{
	return m_ActiveShapeId_;
}
std::string HorusObjectManager::GetShapeNameById(int id)
{
	HorusIdManager& IdManager = HorusIdManager::GetInstance();
	// Structured binding for check in m_ObjectNameToIdMap_ if the id exists (if not, return empty string)
	for (const auto& [fst, snd] : IdManager.GetObjectNameToIdMap())
	{
		if (snd == id)
		{
			return fst;
		}
	}
	return "";
}

void HorusObjectManager::DeleteSelectedShape(int id)
{
	HorusIdManager& IdManager = HorusIdManager::GetInstance();
	HorusGarbageCollector& GarbageCollector = HorusGarbageCollector::GetInstance();

	if (m_RadeonShapes_.contains(id))
	{
		GarbageCollector.Remove(m_RadeonShapes_[id]);
		CHECK(rprObjectDelete(m_RadeonShapes_[id]));
		std::string ShapeName = GetShapeNameById(id);

		for (auto& [key, shapes] : m_RadeonMeshShapeMap_)
		{
			auto it = std::find(shapes.begin(), shapes.end(), m_RadeonShapes_[id]);
			if (it != shapes.end())
			{
				shapes.erase(it);
				if (shapes.empty()) {
					m_RadeonMeshShapeMap_.erase(key);
				}
				break;
			}
		}


		m_RadeonShapes_.erase(id);
		IdManager.GetObjectNameToIdMap().erase(ShapeName);
		HorusIdManager::GetInstance().ReleaseId(id);
		UpdateGroupShapeOutlinerData();
	}
	else
	{
		spdlog::error("no shape with this id exists.");
	}
}

// Getters
rpr_shape HorusObjectManager::GetShapeById(int id)
{
	if (m_RadeonShapes_.contains(id))
	{
		return m_RadeonShapes_[id];
	}
	else
	{
		spdlog::error("no shape with this id exists. ");
		return nullptr;
	}
}
rpr_shape HorusObjectManager::GetShapeFromGroup(int GroupId, const std::string& shapeName)
{
	auto groupShapeIter = m_GroupShape_.find(GroupId);

	if (groupShapeIter != m_GroupShape_.end())
	{
		return groupShapeIter->second.GetShape(shapeName);
	}
	return nullptr;
}

HorusOpenGlShape HorusObjectManager::GetOpenGlShapeById(int id)
{
	if (m_OpenGlShapes_.contains(id))
	{
		return m_OpenGlShapes_[id];
	}

	spdlog::error("no shape with this id exists. ");
	return m_OpenGlShapes_[0];
}

HorusOpenGlShape HorusObjectManager::GetOpenGlShapeFromGroup(int GroupId, const std::string& shapeName)
{
	auto groupShapeIter = m_GroupShape_.find(GroupId);

	if (groupShapeIter != m_GroupShape_.end())
	{
		return groupShapeIter->second.GetOpenGlShape(shapeName);
	}
	spdlog::error("no shape with this name exists. ");
	return m_OpenGlShapes_[0];
}

std::vector<HorusOpenGlShape> HorusObjectManager::GetOpenGlShapeToRender()
{
	std::vector<HorusOpenGlShape> Shapes;
	for (const auto& Value : m_OpenGlMeshShapeMap_ | std::views::values)
	{
		Shapes.insert(Shapes.end(), Value.begin(), Value.end());
	}
	return Shapes;
}

void HorusObjectManager::DrawOpenGlShape(GLuint& ProgramID, HorusOpenGLCamera& Camera)
{
	for (auto Value : m_OpenGlMeshShapeMap_ | std::views::values)
	{
		for (auto Shape : Value)
		{
			Shape.Draw(ProgramID, Camera, true);



			/*spdlog::info("Shape Model Matrix: {}, {}, {}, {}", Shape.GetModelMatrix()[0][0], Shape.GetModelMatrix()[0][1], Shape.GetModelMatrix()[0][2], Shape.GetModelMatrix()[0][3]);
			spdlog::info("Shape Model Matrix: {}, {}, {}, {}", Shape.GetModelMatrix()[1][0], Shape.GetModelMatrix()[1][1], Shape.GetModelMatrix()[1][2], Shape.GetModelMatrix()[1][3]);
			spdlog::info("Shape Model Matrix: {}, {}, {}, {}", Shape.GetModelMatrix()[2][0], Shape.GetModelMatrix()[2][1], Shape.GetModelMatrix()[2][2], Shape.GetModelMatrix()[2][3]);
			spdlog::info("Shape Model Matrix: {}, {}, {}, {}", Shape.GetModelMatrix()[3][0], Shape.GetModelMatrix()[3][1], Shape.GetModelMatrix()[3][2], Shape.GetModelMatrix()[3][3]);
			spdlog::info("Shape translation: ({}, {}, {})", Shape.GetTranslation().x, Shape.GetTranslation().y, Shape.GetTranslation().z);
			spdlog::info("Shape rotation: ({}, {}, {})", Shape.GetRotation().x, Shape.GetRotation().y, Shape.GetRotation().z);
			spdlog::info("Shape scale: ({}, {}, {})", Shape.GetScale().x, Shape.GetScale().y, Shape.GetScale().z);*/
		}
	}


	/*for (unsigned int i = 0; i < m_OpenGlMeshShapeMap_.size(); i++)
	{
		for (auto Shape : m_OpenGlMeshShapeMap_[m_OpenGlGroupShapeNames_[i]])
		{
			Shape.Draw(ProgramID, Camera);
			spdlog::info("Drawing shape {}", m_OpenGlGroupShapeNames_[i]);
			spdlog::info("Shape Model Matrix: {}, {}, {}, {}", Shape.GetModelMatrix()[0][0], Shape.GetModelMatrix()[0][1], Shape.GetModelMatrix()[0][2], Shape.GetModelMatrix()[0][3]);
			spdlog::info("Shape Model Matrix: {}, {}, {}, {}", Shape.GetModelMatrix()[1][0], Shape.GetModelMatrix()[1][1], Shape.GetModelMatrix()[1][2], Shape.GetModelMatrix()[1][3]);
			spdlog::info("Shape Model Matrix: {}, {}, {}, {}", Shape.GetModelMatrix()[2][0], Shape.GetModelMatrix()[2][1], Shape.GetModelMatrix()[2][2], Shape.GetModelMatrix()[2][3]);
			spdlog::info("Shape Model Matrix: {}, {}, {}, {}", Shape.GetModelMatrix()[3][0], Shape.GetModelMatrix()[3][1], Shape.GetModelMatrix()[3][2], Shape.GetModelMatrix()[3][3]);
			spdlog::info("Shape translation: ({}, {}, {})", Shape.GetTranslation().x, Shape.GetTranslation().y, Shape.GetTranslation().z);
			spdlog::info("Shape rotation: ({}, {}, {})", Shape.GetRotation().x, Shape.GetRotation().y, Shape.GetRotation().z);
			spdlog::info("Shape scale: ({}, {}, {})", Shape.GetScale().x, Shape.GetScale().y, Shape.GetScale().z);
		}
	}*/
}


glm::vec3 HorusObjectManager::GetShapePositionById(int id)
{
	auto Shape = GetShapeById(id);
	if (Shape)
	{
		glm::mat4 Transform = glm::mat4(0.0f);
		CHECK(rprShapeGetInfo(Shape, RPR_SHAPE_TRANSFORM, sizeof(glm::mat4), &Transform, nullptr));

		m_ActualShapePosition_.x = Transform[3][0];
		m_ActualShapePosition_.y = Transform[3][1];
		m_ActualShapePosition_.z = Transform[3][2];

		return m_ActualShapePosition_;
	}
	return glm::vec3(0.0f);
}
glm::vec3 HorusObjectManager::GetShapeRotationById(int id)
{
	auto Shape = GetShapeById(id);
	if (Shape)
	{
		glm::mat4 Transform = glm::mat4(0.0f);
		CHECK(rprShapeGetInfo(Shape, RPR_SHAPE_TRANSFORM, sizeof(glm::mat4), &Transform, nullptr));

		m_ActualShapeRotation_.x = Transform[2][0];
		m_ActualShapeRotation_.y = Transform[2][1];
		m_ActualShapeRotation_.z = Transform[2][2];

		return m_ActualShapeRotation_;
	}
	return glm::vec3(0.0f);
}
glm::vec3 HorusObjectManager::GetShapeScaleById(int id)
{
	auto Shape = GetShapeById(id);
	if (Shape)
	{
		glm::mat4 Transform = glm::mat4(0.0f);
		CHECK(rprShapeGetInfo(Shape, RPR_SHAPE_TRANSFORM, sizeof(glm::mat4), &Transform, nullptr));

		m_ActualShapeScale_.x = Transform[0][0];
		m_ActualShapeScale_.y = Transform[1][1];
		m_ActualShapeScale_.z = Transform[2][2];

		return m_ActualShapeScale_;
	}
	return glm::vec3(0.0f);
}

// Setters
void HorusObjectManager::SetShapePositionById(int shapeId, const glm::vec3& newPosition)
{
	if (const auto Shape = GetShapeById(shapeId); Shape != nullptr)
	{
		m_ActualShapePosition_ = newPosition;
		UpdateShapeTransforms(Shape);
	}
	else
	{
		spdlog::error("no shape with this id exists. ");
	}
}
void HorusObjectManager::SetShapeRotationById(int shapeId, const glm::vec3& newRotation)
{
	auto shape = GetShapeById(shapeId);
	if (shape) {
		m_ActualShapeRotation_ = newRotation;
		UpdateShapeTransforms(shape);
	}
	else
	{
		spdlog::error("no shape with this id exists. ");
	}
}
void HorusObjectManager::SetShapeScaleById(int shapeId, const glm::vec3& newScale)
{
	auto shape = GetShapeById(shapeId);
	if (shape) {
		m_ActualShapeScale_ = newScale;
		UpdateShapeTransforms(shape);
	}
	else
	{
		spdlog::error("no shape with this id exists. ");
	}
}
void HorusObjectManager::SetShapeResetTransformById(int shapeId)
{
	auto shape = GetShapeById(shapeId);
	if (shape) {
		m_ActualShapePosition_ = glm::vec3(0.0f);
		m_ActualShapeRotation_ = glm::vec3(0.0f);
		m_ActualShapeScale_ = glm::vec3(1.0f);
		UpdateShapeTransforms(shape);
	}
	else
	{
		spdlog::error("no shape with this id exists. ");
	}
}
void HorusObjectManager::UpdateShapeTransforms(rpr_shape shape)
{
	glm::mat4 Translate = translate(glm::mat4(1.0f), m_ActualShapePosition_);
	glm::mat4 RotationX = rotate(glm::mat4(1.0f), glm::radians(m_ActualShapeRotation_.x), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 RotationY = rotate(glm::mat4(1.0f), glm::radians(m_ActualShapeRotation_.y), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 RotationZ = rotate(glm::mat4(1.0f), glm::radians(m_ActualShapeRotation_.z), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 Scale = scale(glm::mat4(1.0f), m_ActualShapeScale_);

	glm::mat4 Rotation = RotationX * RotationY * RotationZ;

	m_ActualShapeTransform_ = Translate * Rotation * Scale;

	spdlog::debug("Shape transform: ({}, {}, {}, {})", m_ActualShapeTransform_[0][0], m_ActualShapeTransform_[0][1], m_ActualShapeTransform_[0][2], m_ActualShapeTransform_[0][3]);
	spdlog::debug("Shape Position : ({}, {}, {})", m_ActualShapePosition_.x, m_ActualShapePosition_.y, m_ActualShapePosition_.z);
	spdlog::debug("Shape Rotation : ({}, {}, {})", m_ActualShapeRotation_.x, m_ActualShapeRotation_.y, m_ActualShapeRotation_.z);
	spdlog::debug("Shape Scale    : ({}, {}, {})", m_ActualShapeScale_.x, m_ActualShapeScale_.y, m_ActualShapeScale_.z);

	CHECK(rprShapeSetTransform(shape, RPR_FALSE, &m_ActualShapeTransform_[0][0])); // RPR_TRUE = transpose matrix in row

}

// Material management ---------------------------------------------------------------------------

int HorusObjectManager::GetActiveMaterialId()
{
	return m_ActiveMaterialId_;
}
int HorusObjectManager::GetMaterialIdByName(std::string Name)
{
	HorusIdManager& IdManager = HorusIdManager::GetInstance();
	if (!IdManager.ContainsName(Name))
	{
		spdlog::error("no material with this name exists. ");
	}

	return IdManager.GetObjectNameToIdMap()[Name];
}
std::string& HorusObjectManager::GetMaterialName(int Id)
{
	std::string& Name = m_MaterialNames_[Id];

	return Name;
}
HorusMaterial& HorusObjectManager::GetMaterial(int Id)
{
	return m_Materials_[Id];
}
bool HorusObjectManager::MaterialExists(std::string Name)
{
	return HorusIdManager::GetInstance().ContainsName(Name);
}
int HorusObjectManager::CreateMaterial(std::string Name)
{
	HorusIdManager& IdManager = HorusIdManager::GetInstance();
	std::string material_name = Name;

	int Suffix = 0;

	while (IdManager.ContainsName(material_name))
	{
		material_name = Name + "_" + std::to_string(Suffix);
		Suffix++;

		spdlog::info("Material with name {} already exists", Name);
	}

	int Id = HorusIdManager::GetInstance().GetNewId();

	HorusMaterial NewMaterial;
	NewMaterial.Init();
	m_Materials_[Id] = NewMaterial;
	m_MaterialNames_[Id] = material_name;
	IdManager.GetObjectNameToIdMap()[material_name] = Id;
	m_ActiveMaterialId_ = Id;

	spdlog::info("Creating {} material with id {}", material_name, Id);

	return Id;
}
void HorusObjectManager::DestroyMaterial(int Id)
{
	HorusIdManager& IdManager = HorusIdManager::GetInstance();
	if (auto It = m_Materials_.find(Id); It != m_Materials_.end())
	{
		It->second.DestroyMaterial();
		m_Materials_.erase(It);
		m_MaterialNames_.erase(Id);
	}
	else
	{
		spdlog::error("Material with id {} does not exist", Id);
	}

	for (auto It = IdManager.GetObjectNameToIdMap().begin(); It != IdManager.GetObjectNameToIdMap().end(); ++It)
	{
		if (It->second == Id)
		{
			IdManager.GetObjectNameToIdMap().erase(It);
			break;
		}
	}

	HorusIdManager::GetInstance().ReleaseId(Id);
}
void HorusObjectManager::DestroyAllMaterial()
{
	for (auto& material : m_Materials_)
	{
		material.second.DestroyMaterial();
	}
}


void HorusObjectManager::SetActiveMaterialId(int id)
{
	m_ActiveMaterialId_ = id;
}
void HorusObjectManager::AssignMaterial(int mesh_id, int mat_id)
{
	if (!m_GroupShape_.contains(mesh_id))
	{
		spdlog::error("no mesh with this id exists. ");
	}

	if (!m_Materials_.contains(mat_id))
	{
		spdlog::error("no material with this id exists. ");
	}


	//m_GroupShape_[mesh_id].AssignMaterial(TODO);
}
void HorusObjectManager::AssignMaterialDirectly(rpr_shape shape, int materialId)
{
	rpr_material_node materialNode = GetMaterialNode(materialId);

	if (materialNode != nullptr)
	{
		rprShapeSetMaterial(shape, materialNode);
	}
	else
	{
		spdlog::error("Invalid material ID: {}", materialId);
	}
}

rpr_material_node HorusObjectManager::GetMaterialNode(int materialId)
{
	if (m_Materials_.find(materialId) != m_Materials_.end())
	{
		return m_Materials_[materialId].GetMaterialNode();
	}

	spdlog::error("Material ID {} not found", materialId);
	return nullptr;
}



// TODO: Add the rest of the setters
// Reflection
void HorusObjectManager::SetReflectionColor(int id, glm::vec4 color)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
	}

	m_Materials_[id].SetReflectionColor(color);
}
void HorusObjectManager::SetReflectionColor(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
	}

	m_Materials_[id].SetReflectionColor(texturePath);
}
void HorusObjectManager::SetReflectionRoughness(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
	}

	m_Materials_[id].SetReflectionRoughness(texturePath);
}
void HorusObjectManager::SetReflectionRoughness(int id, glm::vec4 color)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
	}

	m_Materials_[id].SetReflectionRoughness(color);
}
void HorusObjectManager::SetReflectionWeight(int id, glm::vec4 color)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
	}

	m_Materials_[id].SetReflectionWeight(color);
}
void HorusObjectManager::SetReflectionWeight(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
	}

	m_Materials_[id].SetReflectionWeight(texturePath);
}
void HorusObjectManager::SetReflectionAnisotropy(int id, glm::vec4 color)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
	}

	m_Materials_[id].SetReflectionAnisotropy(color);
}
void HorusObjectManager::SetReflectionAnisotropy(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
	}

	m_Materials_[id].SetReflectionAnisotropy(texturePath);
}
void HorusObjectManager::SetReflectionMode(int id, HorusMaterial::ReflectionType mode)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
	}

	m_Materials_[id].SetReflectionMode(mode);
}
void HorusObjectManager::SetReflectionMetalness(int id, glm::vec4 color)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
	}

	m_Materials_[id].SetReflectionMetalness(color);
}
void HorusObjectManager::SetReflectionMetalness(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
	}
	m_Materials_[id].SetReflectionMetalness(texturePath);
}

// Sheen
void HorusObjectManager::SetSheenColor(int id, glm::vec4 color)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
	}

	m_Materials_[id].SetSheenColor(color);
}
void HorusObjectManager::SetSheenColor(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
	}

	m_Materials_[id].SetSheenColor(texturePath);
}
void HorusObjectManager::SetSheenWeight(int id, glm::vec4 weight)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
	}

	m_Materials_[id].SetSheenWeight(weight);
}
void HorusObjectManager::SetSheenWeight(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
	}

	m_Materials_[id].SetSheenWeight(texturePath);
}
void HorusObjectManager::SetSheenTint(int id, glm::vec4 tint)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
	}

	m_Materials_[id].SetSheenTint(tint);
}
void HorusObjectManager::SetSheenTint(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
	}

	m_Materials_[id].SetSheenTint(texturePath);
}

// Refraction
void HorusObjectManager::SetRefractionColor(int id, glm::vec4 color)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
	}

	m_Materials_[id].SetRefractionColor(color);
}
void HorusObjectManager::SetRefractionColor(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
	}

	m_Materials_[id].SetRefractionColor(texturePath);

}
void HorusObjectManager::SetRefractionWeight(int id, glm::vec4 weight)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
	}

	m_Materials_[id].SetRefractionWeight(weight);
}
void HorusObjectManager::SetRefractionWeight(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
	}

	m_Materials_[id].SetRefractionWeight(texturePath);
}
void HorusObjectManager::SetRefractionRoughness(int id, glm::vec4 roughness)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
	}

	m_Materials_[id].SetRefractionRoughness(roughness);
}
void HorusObjectManager::SetRefractionRoughness(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
	}

	m_Materials_[id].SetRefractionRoughness(texturePath);
}
void HorusObjectManager::SetRefractionNormal(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
	}

	m_Materials_[id].SetRefractionNormal(texturePath);
}
void HorusObjectManager::SetRefractionNormal(int id, glm::vec4 normal)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
	}

	m_Materials_[id].SetRefractionNormal(normal);
}
void HorusObjectManager::SetRefractionAbsorptionColor(int id, glm::vec4 color)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
	}

	m_Materials_[id].SetRefractionAbsorptionColor(color);
}
void HorusObjectManager::SetRefractionAbsorptionColor(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
	}
	m_Materials_[id].SetRefractionAbsorptionColor(texturePath);
}
void HorusObjectManager::SetRefractionAbsorptionDistance(int id, glm::vec4 color)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
	}

	m_Materials_[id].SetRefractionAbsorptionDistance(color);
}
void HorusObjectManager::SetRefractionAbsorptionDistance(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
	}

	m_Materials_[id].SetRefractionAbsorptionDistance(texturePath);
}


// SSS
void HorusObjectManager::SetSssScatterColor(int id, glm::vec4 color)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
	}

	m_Materials_[id].SetSssScatterColor(color);
}
void HorusObjectManager::SetSssScatterColor(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
	}

	m_Materials_[id].SetSssScatterColor(texturePath);
}
void HorusObjectManager::SetSssScatterWeight(int id, glm::vec4 weight)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
	}

	m_Materials_[id].SetSssScatterWeight(weight);
}
void HorusObjectManager::SetSssScatterWeight(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
	}

	m_Materials_[id].SetSssScatterWeight(texturePath);

}
void HorusObjectManager::SetSssScatterDistance(int id, glm::vec4 color)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
	}

	m_Materials_[id].SetSssScatterDistance(color);
}
void HorusObjectManager::SetSssScatterDistance(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
	}

	m_Materials_[id].SetSssScatterDistance(texturePath);
}
void HorusObjectManager::SetSssScatterDirection(int id, glm::vec4 color)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
	}

	m_Materials_[id].SetSssScatterDirection(color);
}
void HorusObjectManager::SetSssScatterDirection(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material whith this id exist.");
	}

	m_Materials_[id].SetSssScatterDirection(texturePath);
}

// Coating
void HorusObjectManager::SetCoatingColor(int id, glm::vec4 color)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists.");
	}

	m_Materials_[id].SetCoatingColor(color);
}
void HorusObjectManager::SetCoatingColor(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists.");
	}

	m_Materials_[id].SetCoatingColor(texturePath);
}
void HorusObjectManager::SetCoatingWeight(int id, glm::vec4 weight)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists.");
	}

	m_Materials_[id].SetCoatingWeight(weight);
}
void HorusObjectManager::SetCoatingWeight(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists.");
	}

	m_Materials_[id].SetCoatingWeight(texturePath);
}
void HorusObjectManager::SetCoatingNormalMap(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists.");
	}

	m_Materials_[id].SetCoatingNormal(texturePath);
}
void HorusObjectManager::SetCoatingNormalMap(int id, glm::vec4 color)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists.");
	}

	m_Materials_[id].SetCoatingNormal(color);
}
void HorusObjectManager::SetCoatingRoughness(int id, glm::vec4 roughness)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists.");
	}

	m_Materials_[id].SetCoatingRoughness(roughness);
}
void HorusObjectManager::SetCoatingRoughness(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists.");
	}

	m_Materials_[id].SetCoatingRoughness(texturePath);
}
void HorusObjectManager::SetCoatingThickness(int id, glm::vec4 thickness)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists.");
	}

	m_Materials_[id].SetCoatingThickness(thickness);
}
void HorusObjectManager::SetCoatingThickness(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetCoatingThickness(texturePath);
}
void HorusObjectManager::SetCoatingTransmissionColor(int id, glm::vec4 color)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetCoatingTransmissionColor(color);
}
void HorusObjectManager::SetCoatingTransmissionColor(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetCoatingTransmissionColor(texturePath);
}
void HorusObjectManager::SetCoatingMetalness(int id, glm::vec4 metalness)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetCoatingMetalness(metalness);
}
void HorusObjectManager::SetCoatingMetalness(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetCoatingMetalness(texturePath);
}
void HorusObjectManager::SetCoatingMode(int id, HorusMaterial::CoatingType mode)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists.");
	}

	m_Materials_[id].SetCoatingMode(mode);
}

//  

void HorusObjectManager::SetNormalMap(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
	}

	m_Materials_[id].SetNormalMap(texturePath);
}
void HorusObjectManager::SetNormalMap(int id, glm::vec4 color)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
		throw std::runtime_error("no material with this id exists. ");
	}

	m_Materials_[id].SetNormalMap(color);
}
void HorusObjectManager::SetDisplacementMap(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
		throw std::runtime_error("no material with this id exists. ");
	}

	m_Materials_[id].SetDisplacementMap(texturePath);
}
void HorusObjectManager::SetDisplacementMap(int id, glm::vec4 color)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
		throw std::runtime_error("no material with this id exists. ");
	}

	m_Materials_[id].SetDisplacementMap(color);
}
void HorusObjectManager::SetOpacity(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
		throw std::runtime_error("no material with this id exists. ");
	}

	m_Materials_[id].SetOpacity(texturePath);
}
void HorusObjectManager::SetOpacity(int id, glm::vec4 color)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
		throw std::runtime_error("no material with this id exists. ");
	}

	m_Materials_[id].SetOpacity(color);
}
void HorusObjectManager::SetEmissive(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
		throw std::runtime_error("no material with this id exists. ");
	}

	m_Materials_[id].SetEmissive(texturePath);
}
void HorusObjectManager::SetEmissive(int id, glm::vec4 color)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
		throw std::runtime_error("no material with this id exists. ");
	}

	m_Materials_[id].SetEmissive(color);
}

void HorusObjectManager::SetEmissiveWeight(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetEmissionWeight(texturePath);

}

void HorusObjectManager::SetEmissiveWeight(int id, glm::vec4 color)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetEmissionWeight(color);
}


void HorusObjectManager::SetTransparency(int id, glm::vec4 color)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetTransparency(color);
}
void HorusObjectManager::SetTransparency(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetTransparency(texturePath);
}


// Material editor management --------------------------------------------------------------------

void HorusObjectManager::AssignMaterialEditorNode(int mesh_id, int mat_id)
{
	if (!m_GroupShape_.contains(mesh_id))
	{
		spdlog::error("no mesh with this id exists. ");
		throw std::runtime_error("no mesh with this id exists. ");
	}

	rpr_material_node newmat = nullptr;

	SetMaterialEditorMeshToSetMaterial(mesh_id);

	newmat = m_MaterialEditors_[mat_id].get_final_material_for_export();

	//m_GroupShape_[mesh_id].AssignMaterial(TODO);

	spdlog::info("material editor node assigned to mesh with id: {}", mesh_id);

}
void HorusObjectManager::SetMaterialFromEditorNode(int id, rpr_material_node mat)
{
	//m_GroupShape_[id].AssignMaterial(TODO);
}

void HorusObjectManager::ShowMaterialEditor(int id)
{
	if (!m_MaterialEditors_.contains(id))
	{
		spdlog::error("no material editor with this id exists. ");
		throw std::runtime_error("no material editor with this id exists. ");
	}

	m_MaterialEditors_[id].update();
}

void HorusObjectManager::OpenMaterialEditor(int id)
{
}

void HorusObjectManager::CloseMaterialEditor(int id)
{
}

void HorusObjectManager::OpenMaterialEditorBrowser()
{
	HorusMaterialEditorBrowser& MaterialEditor = HorusMaterialEditorBrowser::get_instance();

	MaterialEditor.update();
}

void HorusObjectManager::CloseMaterialEditorBrowser()
{
	HorusMaterialEditorBrowser& MaterialEditor = HorusMaterialEditorBrowser::get_instance();

	MaterialEditor.quit();
}

int HorusObjectManager::SetMaterialEditorToShow(int id)
{
	m_MaterialEditorToShow_ = id;

	return m_MaterialEditorToShow_;
}

int HorusObjectManager::GetMaterialEditorToShow()
{
	return m_MaterialEditorToShow_;
}

void HorusObjectManager::OpenMaterialEditorCreateMenu()
{
	//material_editor_create_menu_ImpM.update();
}

void HorusObjectManager::CloseMaterialEditorCreateMenu()
{
	//material_editor_create_menu_ImpM.quit();
}

void HorusObjectManager::QuitMaterialEditor(int id)
{
	m_MaterialEditors_[id].quit();
}

void HorusObjectManager::DestroyAllMaterialEditors()
{
	for (auto& material_editor : m_MaterialEditors_)
	{
		material_editor.second.quit();
	}
}

// Light management ------------------------------------------------------------------------------

int HorusObjectManager::CreateLight(const std::string& Name, const std::string& LightType, const std::string& ImagePath)
{
	HorusIdManager& IdManager = HorusIdManager::GetInstance();
	std::string LightName = Name;

	int Suffix = 001;

	while (IdManager.GetObjectNameToIdMap().contains(LightName))
	{
		LightName = Name + std::to_string(Suffix);
		Suffix++;

		spdlog::info("Light name already exists, trying {} instead", LightName);
	}

	int Id = HorusIdManager::GetInstance().GetNewId();

	HorusLight NewLight;
	NewLight.Init(LightType, ImagePath);
	m_Lights_[Id] = NewLight;
	m_LightNames_[Id] = LightName;
	IdManager.GetObjectNameToIdMap()[LightName] = Id;
	m_ActiveLightId_ = Id;

	// TODO : Check If light are nullptr if true then destroy the light and release the id

	spdlog::info("Light {} created with id: {}", LightName, Id);

	return Id;
}
void HorusObjectManager::DestroyLight(int Id)
{
	HorusIdManager& IdManager = HorusIdManager::GetInstance();
	if (auto It = m_Lights_.find(Id); It != m_Lights_.end())
	{
		m_Lights_[Id].DestroyLight();
		m_Lights_.erase(It);
		m_LightNames_.erase(Id);
	}
	else
	{
		spdlog::error("no light with this id exists. ");
	}

	for (auto It = IdManager.GetObjectNameToIdMap().begin(); It != IdManager.GetObjectNameToIdMap().end(); ++It) {
		if (It->second == Id) {
			IdManager.GetObjectNameToIdMap().erase(It);
			break;
		}
	}

	HorusIdManager::GetInstance().ReleaseId(Id);
}
void HorusObjectManager::DestroyAllLights()
{
	for (auto& Val : m_Lights_ | std::views::values)
	{
		Val.DestroyLight();
	}
}

// General Getters
int HorusObjectManager::GetLightType(int id)
{
	HorusLight& Light = GetLight(GetActiveLightId());

	rpr_light_type TempLightType = Light.GetLightType();

	switch (TempLightType)
	{
	case RPR_LIGHT_TYPE_POINT:
		return 0;
	case RPR_LIGHT_TYPE_DIRECTIONAL:
		return 1;
	case RPR_LIGHT_TYPE_SPOT:
		return 2;
	case RPR_LIGHT_TYPE_ENVIRONMENT:
		return 3;
	case RPR_LIGHT_TYPE_SKY:
		return 4;
	case RPR_LIGHT_TYPE_IES:
		return 5;
	case RPR_LIGHT_TYPE_SPHERE:
		return 6;
	case RPR_LIGHT_TYPE_DISK:
		return 7;
	}
	return -1;
}
std::string& HorusObjectManager::GetLightName(int id)
{
	std::string& Name = m_LightNames_[id];

	return Name;
}
HorusLight& HorusObjectManager::GetLight(int Id)
{
	return m_Lights_[Id];
}

// General Setters
void HorusObjectManager::SetLightType(int id, int type)
{
	if (!m_Lights_.contains(id))
	{
		spdlog::error("no light with this id : {} exists. ", id);
		spdlog::error("Unable to set light type");

		return;
	}

	switch (type)
	{
	case 0:
		m_Lights_[id].SetLightType("point");
		break;
	case 1:
		m_Lights_[id].SetLightType("directional");
		break;
	case 2:
		m_Lights_[id].SetLightType("spot");
		break;
	case 3:
		m_Lights_[id].SetLightType("environment");
		break;
	case 4:
		m_Lights_[id].SetLightType("sky");
		break;
	case 5:
		m_Lights_[id].SetLightType("ies");
		break;
	case 6:
		m_Lights_[id].SetLightType("sphere");
		break;
	case 7:
		m_Lights_[id].SetLightType("disk");
		break;
	}
}
void HorusObjectManager::SetLightName(int id, const std::string& name)
{
	if (!m_Lights_.contains(id))
	{
		spdlog::error("no light with this id exists. ");
	}

	m_LightNames_[id] = name;

}
void HorusObjectManager::SetLightTexture(int id, const std::string& ImagePath)
{
	if (!m_Lights_.contains(id))
	{
		spdlog::error("no light with this id exists. ");
	}

	m_Lights_[id].SetLightTexturePath(ImagePath);
}
void HorusObjectManager::SetLightVisibility(int id, bool visibility)
{
	if (!m_Lights_.contains(id))
	{
		spdlog::error("no light with this id : {} exists. ", id);
		spdlog::error("Unable to set light visibility");

		return;
	}

	m_Lights_[id].SetLightVisibility(visibility);
}
void HorusObjectManager::SetLightIntensity(int id, glm::vec3 Intensity)
{
	if (!m_Lights_.contains(id))
	{
		spdlog::error("no light with this id exists. : {}", id);
		spdlog::error("Unable to set light intensity");

		return;
	}

	m_Lights_[id].SetIntensity(Intensity);
}
void HorusObjectManager::SetLightPosition(int id, const glm::vec3& position)
{
	if (!m_Lights_.contains(id))
	{
		spdlog::error("no light with this id : {} exists. ", id);
		spdlog::error("Unable to set light position");

		return;
	}

	m_Lights_[id].SetPosition(position);
}
void HorusObjectManager::SetLightRotation(int id, const glm::vec3& rotation)
{
	if (!m_Lights_.contains(id))
	{
		spdlog::error("no light with this id : {} exists. ", id);
		spdlog::error("Unable to set light rotation");

		return;
	}

	m_Lights_[id].SetRotation(rotation);
}
void HorusObjectManager::SetLightScale(int id, const glm::vec3& scale)
{
	if (!m_Lights_.contains(id))
	{
		spdlog::error("no light with this id : {} exists. ", id);
		spdlog::error("Unable to set light scale");

		return;
	}

	m_Lights_[id].SetScale(scale);
}

// Directional light
void HorusObjectManager::SetDirectionalLightShadowSoftnessAngle(int id, float Coef)
{
	if (!m_Lights_.contains(id))
	{
		spdlog::error("no light with this id : {} exists. ", id);
		spdlog::error("Unable to set light shadow softness angle");

		return;
	}

	m_Lights_[id].SetDirectionalLightShadowSoftnessAngle(Coef);
}

// Spot light
void HorusObjectManager::SetConeShape(int id, float InAngle, float OutAngle)
{
	if (!m_Lights_.contains(id))
	{
		spdlog::error("no light with this id : {} exists. ", id);
		spdlog::error("Unable to set cone shape");

		return;
	}

	m_Lights_[id].SetConeShape(InAngle, OutAngle);
}
void HorusObjectManager::SetSpotLightImage(int id, const std::string& ImagePath)
{
	if (!m_Lights_.contains(id))
	{
		spdlog::error("no light with this id : {} exists. ", id);
		spdlog::error("Unable to set spot light image");

		return;
	}

	m_Lights_[id].SetSpotLightImage(ImagePath);
}

// Environment light
void HorusObjectManager::SetEnvironmentLightSetImage(int id, const std::string& ImagePath)
{
	if (!m_Lights_.contains(id))
	{
		spdlog::error("no light with this id : {} exists. ", id);
		spdlog::error("Unable to set environment light image");

		return;
	}

	m_Lights_[id].SetEnvironmentLightSetImage(ImagePath);
}
void HorusObjectManager::SetShapeEnvironmentLight(int id, rpr_shape Shape, bool IsEnvLight)
{
	if (!m_Lights_.contains(id))
	{
		spdlog::error("no light with this id : {} exists. ", id);
		spdlog::error("Unable to set environment light image");

		return;
	}

	m_Lights_[id].SetShapeEnvironmentLight(Shape, IsEnvLight);
}
void HorusObjectManager::SetEnvironmentLightAttachPortal(int id, rpr_scene Scene, rpr_light Light, rpr_shape Shape)
{
	if (!m_Lights_.contains(id))
	{
		spdlog::error("no light with this id : {} exists. ", id);
		spdlog::error("Unable to set environment light image");

		return;
	}

	m_Lights_[id].SetEnvironmentLightAttachPortal(Scene, Light, Shape);
}
void HorusObjectManager::SetEnvironmentLightDetachPortal(int id, rpr_scene Scene, rpr_light Light, rpr_shape Shape)
{
	if (!m_Lights_.contains(id))
	{
		spdlog::error("no light with this id : {} exists. ");
		spdlog::error("Unable to set environment light image");

		return;
	}

	m_Lights_[id].SetEnvironmentLightDetachPortal(Scene, Light, Shape);
}

// Sky light
void HorusObjectManager::SetSkyLightTurbidity(int id, float Turbidity)
{
	if (!m_Lights_.contains(id))
	{
		spdlog::error("no light with this id : {} exists. ");
		spdlog::error("Unable to set sky light turbidity");

		return;
	}

	m_Lights_[id].SetSkyLightTurbidity(Turbidity);
}
void HorusObjectManager::SetSkyLightAlbedo(int id, const glm::vec3& Albedo)
{
	if (!m_Lights_.contains(id))
	{
		spdlog::error("no light with this id : {} exists. ");
		spdlog::error("Unable to set sky light albedo");

		return;
	}

	m_Lights_[id].SetSkyLightAlbedo(Albedo);
}
void HorusObjectManager::SetSkyLightScale(int id, const glm::vec3& Scale)
{
	if (!m_Lights_.contains(id))
	{
		spdlog::error("no light with this id : {} exists. ");
		spdlog::error("Unable to set sky light scale");

		return;
	}

	m_Lights_[id].SetSkyLightScale(Scale);
}
void HorusObjectManager::SetSkyLightDirection(int id, const glm::vec3& Direction)
{
	if (!m_Lights_.contains(id))
	{
		spdlog::error("no light with this id : {} exists. ");
		spdlog::error("Unable to set sky light direction");

		return;
	}

	m_Lights_[id].SetSkyLightDirection(Direction);

}
void HorusObjectManager::SetSkyLightAttachPortal(int id, rpr_scene Scene, rpr_light Light, rpr_shape Shape)
{
	if (!m_Lights_.contains(id))
	{
		spdlog::error("no light with this id : {} exists. ");
		spdlog::error("Unable to set sky light attach portal");

		return;
	}

	m_Lights_[id].SetSkyLightAttachPortal(Scene, Light, Shape);
}
void HorusObjectManager::SetSkyLightDetachPortal(int id, rpr_scene Scene, rpr_light Light, rpr_shape Shape)
{
	if (!m_Lights_.contains(id))
	{
		spdlog::error("no light with this id : {} exists. ");
		spdlog::error("Unable to set sky light detach portal");

		return;
	}

	m_Lights_[id].SetSkyLightDetachPortal(Scene, Light, Shape);
}

// IES light
void HorusObjectManager::SetIesLightImage(int id, const std::string& ImagePath, int ImgSizeX, int ImgSizeY)
{
	if (!m_Lights_.contains(id))
	{
		spdlog::error("no light with this id : {} exists. ");
		spdlog::error("Unable to set ies light image");

		return;
	}

	m_Lights_[id].SetIesLightImage(ImagePath, ImgSizeX, ImgSizeY);
}
void HorusObjectManager::SetIesLightImage(int id, const char* ImagePath, int ImgSizeX, int ImgSizeY)
{
	if (!m_Lights_.contains(id))
	{
		spdlog::error("no light with this id : {} exists. ");
		spdlog::error("Unable to set ies light image");

		return;
	}

	m_Lights_[id].SetIesLightImage(ImagePath, ImgSizeX, ImgSizeY);
}

// Sphere light
void HorusObjectManager::SetSphereLightRadius(int id, float Radius)
{
	if (!m_Lights_.contains(id))
	{
		spdlog::error("no light with this id : {} exists. ");
		spdlog::error("Unable to set sphere light radius");

		return;
	}

	m_Lights_[id].SetSphereLightRadius(Radius);
}

// Disk light
void HorusObjectManager::SetDiskLightRadius(int id, const float& Radius)
{
	if (!m_Lights_.contains(id))
	{
		spdlog::error("no light with this id : {} exists. ");
		spdlog::error("Unable to set disk light radius");

		return;
	}

	m_Lights_[id].SetDiskLightRadius(Radius);
}
void HorusObjectManager::SetDiskLightAngle(int id, const float& Angle)
{
	if (!m_Lights_.contains(id))
	{
		spdlog::error("no light with this id : {} exists. ");
		spdlog::error("Unable to set disk light angle");

		return;
	}

	m_Lights_[id].SetDiskLightAngle(Angle);
}
void HorusObjectManager::SetDiskLightInnerAngle(int id, const float& InnerAngle)
{
	if (!m_Lights_.contains(id))
	{
		spdlog::error("no light with this id : {} exists.");
		spdlog::error("Unable to set disk light inner angle");

		return;
	}

	m_Lights_[id].SetDiskLightInnerAngle(InnerAngle);
}


// Scene management ------------------------------------------------------------------------------

int HorusObjectManager::CreateScene(const std::string& name)
{
	HorusIdManager& IdManager = HorusIdManager::GetInstance();
	std::string SceneName = name;

	int Suffix = 001;

	int Id = HorusIdManager::GetInstance().GetNewId();

	HorusScene NewScene;
	NewScene.Init();
	m_Scenes_[Id] = NewScene;
	m_ActiveSceneId_ = Id;
	m_SceneNames_[Id] = SceneName;
	IdManager.GetObjectNameToIdMap()[SceneName] = Id;

	spdlog::info("Scene {} created with id: {}", SceneName, Id);

	return Id;
}
int HorusObjectManager::CreateDefaultScene()
{
	HorusOmCamera& CameraManager = HorusOmCamera::GetInstance();
	int Id = CreateScene("DefaultScene");

	// Create camera
	CameraManager.CreateCamera("DefaultCamera");
	//CreateRadeonCamera(Id, "DefaultCamera");

	//create_light("HDRI", "hdri", "core/scene/dependencies/light/horus_hdri_main.exr");
	//int HDRI = CreateLight("Lgt_Dome01", "hdri", "resources/Textures/resting_place_2_2k.exr");
	//int HDRI = CreateLight("Lgt_Dome01", "hdri", "resources/Lookdev/Light/niederwihl_forest_4k.exr");


	//SetLightRotation(HDRI, glm::vec3(0.0f, 1.0f, 0.0f), 90.0f);

	SetScene(Id);

	return Id;
}
void HorusObjectManager::DestroyScene(int id)
{
	if (auto It = m_Scenes_.find(id); It != m_Scenes_.end())
	{
		It->second.DestroyScene();
		m_Scenes_.erase(It);
		m_SceneNames_.erase(id);
	}
	else
	{
		spdlog::error("no scene with this id exists. ");
	}

	/*if (id == m_active_scene_id_)
	{
		if (!m_scenes_.empty())
		{
			m_active_scene_id_ = m_scenes_.begin()->first;
		}
		else
		{
			create_scene("DefaultScene");
		}
	}*/

	HorusIdManager::GetInstance().ReleaseId(id);

	if (m_Scenes_.empty())
	{
		CreateScene("DefaultScene");
	}

	//HorusResetBuffers::get_instance().CallResetBuffers();
}
void HorusObjectManager::DestroyAllScenes()
{
	for (auto& scene : m_Scenes_)
	{
		scene.second.DestroyScene();
	}
}

void HorusObjectManager::ImportGltfScene()
{

}
void HorusObjectManager::SetScene(int id)
{
	if (!m_Scenes_.contains(id))
	{
		spdlog::error("no scene with this id exists. ");
	}

	m_ActiveSceneId_ = id;

	auto Scene = m_Scenes_[id].GetScene();

	m_Scenes_[id].SetScene(Scene);

	spdlog::info("Scene {} set as active scene", id);

}
rpr_scene& HorusObjectManager::GetScene()
{
	return  m_Scenes_[m_ActiveSceneId_].GetScene();
}
int HorusObjectManager::GetSceneIdByName(const char* name)
{
	HorusIdManager& IdManager = HorusIdManager::GetInstance();
	while (!IdManager.GetObjectNameToIdMap().contains(name))
	{
		spdlog::info("Scene name does not exist, trying again");
	}

	return IdManager.GetObjectNameToIdMap()[name];
}
int HorusObjectManager::GetActiveSceneId()
{
	return m_ActiveSceneId_;
}
std::string& HorusObjectManager::GetSceneNameById(int id)
{
	return m_SceneNames_[id];
}


void HorusObjectManager::ShowDummyDragon()
{
	m_Scenes_[m_ActiveSceneId_].ShowDummyDragon();
}
void HorusObjectManager::ShowDummyPlane()
{
	m_Scenes_[m_ActiveSceneId_].ShowDummyPlane();
}
void HorusObjectManager::ShowLookdevScene()
{
	m_Scenes_[m_ActiveSceneId_].ShowLookdevScene();
}
void HorusObjectManager::ShowJaguardXKSS()
{
	m_Scenes_[m_ActiveSceneId_].ShowJaguardXKSS();
}

// -----------------------------------------------------------------------------------------------