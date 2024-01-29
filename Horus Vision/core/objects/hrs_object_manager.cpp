
#include "hrs_opengl_camera.h"
#include "hrs_object_manager.h"

#include "ProRenderGLTF.h"
#include "spdlog/spdlog.h"

// Camera management ------------------------------------------------------------------------------

void HorusObjectManager::TransfertDataBetwweenCamera(int id, int id2)
{
	HorusRadeonCamera& Camera = GetRadeonCamera(id);
	HorusOpenGLCamera& Camera2 = GetOpenGlCamera(id2);

	Camera2.SetLookAt(Camera.GetLookAt());
	Camera2.SetPosition(Camera.GetPosition());
	Camera2.SetCameraRotation(Camera.GetRotation());
	Camera2.SetCameraScale(Camera.GetCameraScale());
	Camera2.SetFov(Camera.GetFov());
	Camera2.SetAspectRatio(Camera.GetAspect());
	Camera2.SetClipping(Camera.GetNear(), Camera.GetFar());
}
int HorusObjectManager::CreateOpenGlCamera(std::string Name)
{
	std::string CameraName = Name;

	int Suffix = 001;

	while (m_ObjectNameToIdMap_.contains(CameraName))
	{
		CameraName = Name + std::to_string(Suffix);
		Suffix++;

		spdlog::info("Camera name already exists, trying {} instead", CameraName);
	}

	int Id = IDManager::GetInstance().GetNewId();

	HorusOpenGLCamera& NewCamera = m_OpenGlCameras_[Id];
	NewCamera.Init();
	m_OpenGlCameraNames_[Id] = CameraName;
	m_ObjectNameToIdMap_[CameraName] = Id;
	m_ActiveRadeonCameraId_ = Id;

	spdlog::info("Camera {} created with id: {}", CameraName, Id);

	return Id;
}
int HorusObjectManager::CreateRadeonCamera(int SceneID, const std::string Name)
{
	std::string CameraName = Name;

	int Suffix = 001;

	while (m_ObjectNameToIdMap_.contains(CameraName))
	{
		CameraName = Name + std::to_string(Suffix);
		Suffix++;

		spdlog::info("Camera name already exists, trying {} instead", CameraName);
	}

	int Id = IDManager::GetInstance().GetNewId();

	HorusRadeonCamera& NewCamera = m_RadeonCameras_[Id];
	NewCamera.Init(CameraName);
	//m_cameras_[id] = new_camera;
	m_RadeonCameraNames_[Id] = CameraName;
	m_ObjectNameToIdMap_[CameraName] = Id;
	m_ActiveRadeonCameraId_ = Id;

	spdlog::info("Camera {} created with id: {}", CameraName, Id);

	return Id;
}
int HorusObjectManager::CreateRadeonCameraFromGLTF(int SceneID, std::string Name, rpr_camera Camera)
{
	std::string CameraName = Name;

	int Suffix = 001;

	while (m_ObjectNameToIdMap_.contains(CameraName))
	{
		CameraName = Name + std::to_string(Suffix);
		Suffix++;

		spdlog::info("Camera name already exists, trying {} instead", CameraName);
	}

	int Id = IDManager::GetInstance().GetNewId();

	HorusRadeonCamera& NewCamera = m_RadeonCameras_[Id];
	NewCamera.Init(CameraName);
	m_RadeonCameraNames_[Id] = CameraName;
	m_ObjectNameToIdMap_[CameraName] = Id;
	m_ActiveRadeonCameraId_ = Id;

	spdlog::info("Camera {} created with id: {}", CameraName, Id);

	return Id;
}
void HorusObjectManager::DestroyCamera(int id)
{
	if (id == m_ActiveRadeonCameraId_)
	{
		if (!m_RadeonCameras_.empty())
		{
			m_ActiveRadeonCameraId_ = m_RadeonCameras_.begin()->first;
		}
		else
		{
			CreateRadeonCamera(GetActiveSceneId(), "DefaultCamera");
		}
	}

	if (auto It = m_RadeonCameras_.find(id); It != m_RadeonCameras_.end())
	{
		It->second.Destroy();
		m_RadeonCameras_.erase(It);
		m_RadeonCameraNames_.erase(id);
	}
	else
	{
		spdlog::error("no camera with this id exists. ");
	}

	IDManager::GetInstance().ReleaseId(id);

	if (m_RadeonCameras_.empty())
	{
		CreateRadeonCamera(GetActiveSceneId(), "DefaultCamera");
	}
}

void HorusObjectManager::BindRadeonCamera(int id)
{
	m_RadeonCameras_[id].Bind();
}
void HorusObjectManager::UnbindRadeonCamera(int id)
{
	m_RadeonCameras_[id].Unbind();
}

void HorusObjectManager::UpdateRadeonCamera(int id)
{
	m_RadeonCameras_[id].UpdateCamera();
}
void HorusObjectManager::UpdateOpenGLCamera(int id)
{
	m_OpenGlCameras_[id].UpdateCamera();
}
void HorusObjectManager::SendToShaderOpenGLCamera(int id, const HorusShaderManager& Shader)
{
	m_OpenGlCameras_[id].SendToShader(Shader);
}

void HorusObjectManager::SetBackgroundImage(const std::string& path)
{
	m_BackgroundMaterial_.SetBackgroundImage(path);
}
void HorusObjectManager::UnsetBackgroundImage()
{
	m_BackgroundMaterial_.UnsetBackgroundImage();
}

void HorusObjectManager::CameraExtractor(rpr_camera& Camera)
{
	if (Camera == nullptr)
	{
		std::cerr << "Camera is null" << '\n';
		return;
	}

	glm::mat4 Transform = glm::mat4(0.0f);
	rprCameraGetInfo(Camera, RPR_CAMERA_TRANSFORM, sizeof(glm::mat4), &Transform, nullptr);
	std::cout << "Transform: (" << Transform[0][0] << ", " << Transform[0][1] << ", " << Transform[0][2] << ", " << Transform[0][3] << ")" << '\n';

	// Extract matricies from the camera
	// Position in matrix
	std::cout << "Position: (" << Transform[3][0] << ", " << Transform[3][1] << ", " << Transform[3][2] << ")" << '\n';
	// Lookat in matrix
	std::cout << "Lookat: (" << Transform[2][0] << ", " << Transform[2][1] << ", " << Transform[2][2] << ")" << '\n';
	// Up in matrix
	std::cout << "Up: (" << Transform[1][0] << ", " << Transform[1][1] << ", " << Transform[1][2] << ")" << '\n';






	rpr_float fov = 0.0f;
	rprCameraGetInfo(Camera, RPR_CAMERA_FSTOP, sizeof(fov), &fov, nullptr);
	std::cout << "FOV: " << fov << " degrees" << '\n';
}
void HorusObjectManager::PrintCameraInfo(int id)
{
	m_RadeonCameras_[id].VariableCheckers("Manual Check");
}
void HorusObjectManager::ResetCamera(int id)
{
	m_RadeonCameras_[id].Reset();
}

// Getters
void HorusObjectManager::GetMatrices(int id, glm::mat4& projection, glm::mat4& view, glm::mat4& model)
{
	m_RadeonCameras_[id].GetMatrices(projection, view, model);
}
void HorusObjectManager::GetOpenGLCameraMatrices(int id, glm::mat4& projection, glm::mat4& view, glm::mat4& model)
{
	m_OpenGlCameras_[id].GetMatricies(view, projection, model);
}
std::string& HorusObjectManager::GetCameraNameById(int id)
{
	std::string& Name = m_RadeonCameraNames_[id];

	return Name;
}
glm::vec3 HorusObjectManager::GetCameraLookAt(int id)
{
	HorusRadeonCamera& Camera = GetRadeonCamera(id);

	return Camera.GetLookAt();
}
glm::vec3 HorusObjectManager::GetCameraPosition(int id)
{
	HorusRadeonCamera& Camera = GetRadeonCamera(id);

	return Camera.GetPosition();
}
glm::vec3 HorusObjectManager::GetCameraTranslation(int id)
{
	HorusRadeonCamera& Camera = GetRadeonCamera(id);

	return Camera.GetTranslation();
}
glm::vec3 HorusObjectManager::GetCameraRotation(int id)
{
	HorusRadeonCamera& Camera = GetRadeonCamera(id);

	return Camera.GetRotation();
}
glm::vec3 HorusObjectManager::GetCameraScale(int id)
{
	HorusRadeonCamera& Camera = GetRadeonCamera(id);

	return Camera.GetCameraScale();
}
float HorusObjectManager::GetCameraFov(int id)
{
	HorusRadeonCamera& Camera = GetRadeonCamera(id);

	return Camera.GetFov();
}
float HorusObjectManager::GetCameraAspectRatio(int id)
{
	HorusRadeonCamera& Camera = GetRadeonCamera(id);

	return Camera.GetAspect();
}
float HorusObjectManager::GetCameraNearPlane(int id)
{
	HorusRadeonCamera& Camera = GetRadeonCamera(id);

	return Camera.GetNear();
}
float HorusObjectManager::GetCameraFarPlane(int id)
{
	HorusRadeonCamera& Camera = GetRadeonCamera(id);

	return Camera.GetFar();
}
float HorusObjectManager::GetCameraFocusDistance(int id)
{
	HorusRadeonCamera& Camera = GetRadeonCamera(id);

	return Camera.GetFocusPlane();
}
float HorusObjectManager::GetCameraFStop(int id)
{
	HorusRadeonCamera& Camera = GetRadeonCamera(id);

	return Camera.GetFStop();
}
int HorusObjectManager::GetActiveRadeonCameraId()
{
	return m_ActiveRadeonCameraId_;
}
int HorusObjectManager::GetActiveOpenGLCameraId()
{
	return m_ActiveOpenGLCameraId_;
}
int HorusObjectManager::GetCameraIdByName(const char* name)
{
	if (!m_ObjectNameToIdMap_.contains(name))
	{
		spdlog::error("no camera with this name exists. ");
	}

	return m_ObjectNameToIdMap_[name];
}
int HorusObjectManager::GetCameraApertureBlades(int id)
{
	HorusRadeonCamera& Camera = GetRadeonCamera(id);

	return Camera.GetApertureBlades();
}

// Setters
void HorusObjectManager::SetMoveCameraForward(int id)
{
	HorusRadeonCamera& Camera = GetRadeonCamera(id);

	Camera.MoveCamera(Forward);
}
void HorusObjectManager::SetMoveCameraBackward(int id)
{
	HorusRadeonCamera& Camera = GetRadeonCamera(id);

	Camera.MoveCamera(Backward);
}
void HorusObjectManager::SetMoveCameraLeft(int id)
{
	HorusRadeonCamera& Camera = GetRadeonCamera(id);

	Camera.MoveCamera(Left);
}
void HorusObjectManager::SetMoveCameraRight(int id)
{
	HorusRadeonCamera& Camera = GetRadeonCamera(id);

	Camera.MoveCamera(Right);
}
void HorusObjectManager::SetMoveCameraUp(int id)
{
	HorusRadeonCamera& Camera = GetRadeonCamera(id);

	Camera.MoveCamera(Up);
}
void HorusObjectManager::SetMoveCameraDown(int id)
{
	HorusRadeonCamera& Camera = GetRadeonCamera(id);

	Camera.MoveCamera(Down);
}
void HorusObjectManager::SetScrollCamera(int id, float delta)
{
	HorusRadeonCamera& Camera = GetRadeonCamera(id);

	Camera.Zoom(delta);
}
void HorusObjectManager::SetPitch(int id, float pitch)
{
	HorusRadeonCamera& Camera = GetRadeonCamera(id);

	spdlog::info("Camera ID: {} Pitch: {}", id, pitch);

	Camera.ChangePitch(pitch);
}
void HorusObjectManager::SetHeading(int id, float heading)
{
	HorusRadeonCamera& Camera = GetRadeonCamera(id);

	spdlog::info("Camera ID: {} Heading: {}", id, heading);

	Camera.ChangeHeading(heading);
}
void HorusObjectManager::SetCameraLookat(int id, glm::vec3& pivot)
{
	HorusRadeonCamera& Camera = GetRadeonCamera(id);

	Camera.SetLookAt(pivot);
}
void HorusObjectManager::SetTumbleCamera(int id, float x, float y, float sensitivity)
{
	HorusRadeonCamera& Camera = GetRadeonCamera(id);

	Camera.Tumbling(x, y, sensitivity);
}
void HorusObjectManager::SetPanCamera(int id, float x, float y, float sensitivity)
{
	HorusRadeonCamera& Camera = GetRadeonCamera(id);

	Camera.Pan(x, y, sensitivity);
}
void HorusObjectManager::SetZoomCamera(int id, float distance)
{
	HorusRadeonCamera& Camera = GetRadeonCamera(id);

	Camera.Zoom(distance);
}
void HorusObjectManager::SetViewport(int id, int x, int y, int width, int height)
{
	m_RadeonCameras_[id].SetViewport(x, y, width, height);
}
void HorusObjectManager::SetActiveRadeonCamera(int id)
{
	if (!m_RadeonCameras_.contains(id))
	{
		spdlog::error("no camera with this id exists. ");
	}

	m_ActiveRadeonCameraId_ = id;
}
void HorusObjectManager::SetActiveOpenGLCamera(int id)
{
	if (!m_OpenGlCameras_.contains(id))
	{
		spdlog::error("no camera with this id exists. ");
	}

	m_ActiveOpenGLCameraId_ = id;
}
void HorusObjectManager::SetCameraFov(int id, float fov)
{
	HorusRadeonCamera& Camera = GetRadeonCamera(id);

	Camera.SetFov(fov);
}
void HorusObjectManager::SetCameraAspectRatio(int id, float aspect_ratio)
{
	HorusRadeonCamera& Camera = GetRadeonCamera(id);

	Camera.SetAspect(aspect_ratio);
}
void HorusObjectManager::SetCameraNear(int id, float NearPlane)
{
	HorusRadeonCamera& Camera = GetRadeonCamera(id);

	Camera.SetClipping(NearPlane, NearPlane);
}
void HorusObjectManager::SetCameraFar(int id, float FarPlane)
{
	HorusRadeonCamera& Camera = GetRadeonCamera(id);

	Camera.SetClipping(FarPlane, FarPlane);
}
void HorusObjectManager::SetFocusDistance(int id, float focus_distance)
{
	HorusRadeonCamera& Camera = GetRadeonCamera(id);

	Camera.SetFocusPlane(focus_distance);
}
void HorusObjectManager::SetFStop(int id, float fstop)
{
	HorusRadeonCamera& Camera = GetRadeonCamera(id);

	Camera.SetFStop(fstop);
}
void HorusObjectManager::SetApertureBlades(int id, int blades)
{
	HorusRadeonCamera& Camera = GetRadeonCamera(id);

	Camera.SetApertureBlades(blades);
}
void HorusObjectManager::SetCameraLookAt(int id, glm::vec3 lookat)
{
	HorusRadeonCamera& camera = GetRadeonCamera(id);

	camera.SetLookAt(lookat);
}
void HorusObjectManager::SetCameraPosition(int id, glm::vec3 position)
{
	HorusRadeonCamera& camera = GetRadeonCamera(id);

	camera.SetPosition(position);
}
void HorusObjectManager::SetCameraRotation(int id, glm::vec3 rotation_axis)
{
	HorusRadeonCamera& camera = GetRadeonCamera(id);

	camera.SetCameraRotation(rotation_axis.x, rotation_axis.y, rotation_axis.z);
}
void HorusObjectManager::SetCameraScale(int id, glm::vec3 scale)
{
	HorusRadeonCamera& camera = GetRadeonCamera(id);

	camera.SetCameraScale(scale);
}

// Group Shape management -------------------------------------------------------------------------------

int HorusObjectManager::CreateGroupShape(const char* path, const std::string& name)
{
	std::string GroupShapeName = name;
	int Suffix = 001;

	// Check the uniqueness of the name
	while (m_ObjectNameToIdMap_.contains(GroupShapeName))
	{
		GroupShapeName = name + std::to_string(Suffix);
		Suffix++;

		spdlog::info("Mesh name already exists, trying {} instead", GroupShapeName);
	}

	// Create a new ID for The group shape
	int Id = IDManager::GetInstance().GetNewId();

	// Initializing the new group shape
	auto& NewGroupShape = m_GroupShape_[Id];
	NewGroupShape.Init(path);

	// Attribuate an ID to each shape of the group shape and store it in a map
	std::vector<std::pair<rpr_shape, std::string>> AssociatedShapes = NewGroupShape.GetShapeAndName();
	std::vector<rpr_shape> ShapeVector;

	for (const auto& [shape, shapeName] : AssociatedShapes)
	{
		int ShapeId = IDManager::GetInstance().GetNewId();
		m_ObjectNameToIdMap_[shapeName] = ShapeId;
		m_Shapes_[ShapeId] = shape;
		ShapeVector.push_back(shape);
	}

	m_MeshShapeMap_[GroupShapeName] = ShapeVector;
	m_GroupShapeNames_[Id] = GroupShapeName;
	m_ObjectNameToIdMap_[GroupShapeName] = Id;
	m_ActiveGroupShapeId_ = Id;

	spdlog::info("Mesh {} created with id: {}", GroupShapeName, Id);

	return Id;
}
void HorusObjectManager::DestroyGroupShape(int id)
{
	if (auto It = m_GroupShape_.find(id); It != m_GroupShape_.end())
	{
		m_GroupShape_[id].DestroyGroupShape();
		m_GroupShape_.erase(It);
		m_GroupShapeNames_.erase(id);
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
	}

	IDManager::GetInstance().ReleaseId(id);
}
void HorusObjectManager::DestroyGroupShapeByName(const char* name)
{
	if (!m_ObjectNameToIdMap_.contains(name))
	{
		spdlog::error("no mesh with this name exists. ");
	}

	int Id = m_ObjectNameToIdMap_[name];

	if (auto It = m_GroupShape_.find(Id); It != m_GroupShape_.end())
	{
		m_GroupShape_[Id].DestroyGroupShape();
		m_GroupShape_.erase(It);
		m_GroupShapeNames_.erase(Id);
	}
	else
	{
		spdlog::error("no mesh with this id exists. ");
	}

	for (auto It = m_ObjectNameToIdMap_.begin(); It != m_ObjectNameToIdMap_.end(); ++It)
	{
		if (It->second == Id)
		{
			m_ObjectNameToIdMap_.erase(It);
			break;
		}
	}

	IDManager::GetInstance().ReleaseId(Id);
}
void HorusObjectManager::DestroyAllGroupShape()
{
	for (auto& Val : m_GroupShape_ | std::views::values)
	{
		Val.DestroyGroupShape();
	}
}

// Getters
std::map<std::string, std::vector<std::string>>& HorusObjectManager::GetGroupShapeWithShapesAndNames()
{
	return m_GroupObjectOutlinerData_;
}
HorusGroupShape& HorusObjectManager::GetGroupShape(int id)
{
	return m_GroupShape_[id];
}
std::string& HorusObjectManager::GetGroupShapeName(int id)
{
	std::string& Name = m_GroupShapeNames_[id];

	return Name;
}
int HorusObjectManager::GetGroupShapeCount(int count)
{
	return m_GroupShape_.size();
}
int HorusObjectManager::GetGroupShapeId(const char* name)
{
	if (!m_ObjectNameToIdMap_.contains(name))
	{
		spdlog::error("no mesh with this name exists. ");
	}

	return m_ObjectNameToIdMap_[name];
}
int HorusObjectManager::GetActiveGroupShapeId()
{
	return m_ActiveGroupShapeId_;
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
	HorusGroupShape& Mesh = GetGroupShape(id);

	return Mesh.GetGroupShapePosition();
}
glm::vec3 HorusObjectManager::GetGroupShapeRotation(int id)
{
	HorusGroupShape& Mesh = GetGroupShape(id);

	return Mesh.GetGroupShapeRotation();
}
glm::vec3 HorusObjectManager::GetGroupShapeScale(int id)
{
	HorusGroupShape& Mesh = GetGroupShape(id);

	return Mesh.GetGroupShapeScale();
}

// Setters
void HorusObjectManager::SetActiveGroupShape(int id)
{
	m_ActiveGroupShapeId_ = id;
}
void HorusObjectManager::SetGroupShapeName(int id, const char* name)
{
	if (!m_GroupShape_.contains(id))
	{
		spdlog::error("no mesh with this id exists. ");
	}

	m_GroupShapeNames_[id] = name;
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

	for (auto& shapeId : m_MeshShapeMap_[m_GroupShapeNames_[id]])
	{
		/*SetShapePositionById(shapeId, pos);
		auto shape = GetShapeById(shapeId);
		UpdateShapeTransforms(shape);*/
	}
}

void HorusObjectManager::SetGroupShapeRotation(int id, glm::vec3 rot)
{
	if (!m_GroupShape_.contains(id))
	{
		spdlog::error("no mesh with this id exists. ");
	}

	m_GroupShape_[id].SetGroupShapeRotation(rot);

	for (auto& Shape : m_MeshShapeMap_[m_GroupShapeNames_[id]])
	{
		SetShapeRotationById(m_ObjectNameToIdMap_[m_GroupShapeNames_[id]], rot);
		UpdateShapeTransforms(Shape);
	}
}
void HorusObjectManager::SetGroupShapeScale(int id, glm::vec3 scale)
{
	if (!m_GroupShape_.contains(id))
	{
		spdlog::error("no mesh with this id exists. ");
	}

	m_GroupShape_[id].SetGroupShapeScale(scale);

	for (auto& Shape : m_MeshShapeMap_[m_GroupShapeNames_[id]])
	{
		SetShapeScaleById(m_ObjectNameToIdMap_[m_GroupShapeNames_[id]], scale);
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
	for (const auto& Key : m_MeshShapeMap_ | std::views::keys) 
	{
		HorusGroupShape& Mesh = m_GroupShape_[m_ObjectNameToIdMap_[Key]];
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
	// Structured binding for check in m_ObjectNameToIdMap_ if the id exists (if not, return empty string)
	for (const auto& [fst, snd] : m_ObjectNameToIdMap_)
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
	HorusGarbageCollector& GarbageCollector = HorusGarbageCollector::GetInstance();

	if (m_Shapes_.contains(id))
	{
		GarbageCollector.Remove(m_Shapes_[id]);
		CHECK(rprObjectDelete(m_Shapes_[id]));
		std::string ShapeName = GetShapeNameById(id);

		for (auto& [key, shapes] : m_MeshShapeMap_)
		{
			auto it = std::find(shapes.begin(), shapes.end(), m_Shapes_[id]);
			if (it != shapes.end())
			{
				shapes.erase(it);  
				if (shapes.empty()) {
					m_MeshShapeMap_.erase(key); 
				}
				break;
			}
		}


		m_Shapes_.erase(id);
		m_ObjectNameToIdMap_.erase(ShapeName);
		IDManager::GetInstance().ReleaseId(id);
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
	if (m_Shapes_.contains(id))
	{
		return m_Shapes_[id];
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
	if (!m_ObjectNameToIdMap_.contains(Name))
	{
		spdlog::error("no material with this name exists. ");
	}

	return m_ObjectNameToIdMap_[Name];
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
	return m_ObjectNameToIdMap_.contains(Name);
}
int HorusObjectManager::CreateMaterial(std::string Name)
{
	std::string material_name = Name;

	int Suffix = 0;

	while (m_ObjectNameToIdMap_.contains(material_name))
	{
		material_name = Name + "_" + std::to_string(Suffix);
		Suffix++;

		spdlog::info("Material with name {} already exists", Name);
	}

	int Id = IDManager::GetInstance().GetNewId();

	HorusMaterial NewMaterial;
	NewMaterial.Init();
	m_Materials_[Id] = NewMaterial;
	m_MaterialNames_[Id] = material_name;
	m_ObjectNameToIdMap_[material_name] = Id;
	m_ActiveMaterialId_ = Id;

	spdlog::info("Creating {} material with id {}", material_name, Id);

	return Id;
}
void HorusObjectManager::DestroyMaterial(int Id)
{
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

	for (auto It = m_ObjectNameToIdMap_.begin(); It != m_ObjectNameToIdMap_.end(); ++It)
	{
		if (It->second == Id)
		{
			m_ObjectNameToIdMap_.erase(It);
			break;
		}
	}

	IDManager::GetInstance().ReleaseId(Id);
}
void HorusObjectManager::DestroyAllMaterial()
{
	for (auto& material : m_Materials_)
	{
		material.second.DestroyMaterial();
	}
}

glm::vec4 HorusObjectManager::GetBaseColor(int id)
{
	HorusMaterial& Material = GetMaterial(id);

	return Material.GetBaseColor();
}

glm::vec4 HorusObjectManager::GetMetallic(int id)
{
	HorusMaterial& Material = GetMaterial(id);

	return Material.GetMetallic();
}

glm::vec4 HorusObjectManager::GetRoughness(int id)
{
	HorusMaterial& Material = GetMaterial(id);

	return Material.GetRoughness();
}

glm::vec4 HorusObjectManager::GetNormal(int id)
{
	HorusMaterial& Material = GetMaterial(id);

	return Material.GetNormal();
}

glm::vec4 HorusObjectManager::GetOpacity(int id)
{
	HorusMaterial& Material = GetMaterial(id);

	return Material.GetOpacity();
}

glm::vec4 HorusObjectManager::GetEmissive(int id)
{
	HorusMaterial& Material = GetMaterial(id);

	return Material.GetEmissive();
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


void HorusObjectManager::SetBaseColor(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
	}
	m_Materials_[id].SetBaseColor(texturePath);
}
void HorusObjectManager::SetBaseColor(int id, glm::vec4 color)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
		throw std::runtime_error("no material with this id exists. ");
	}
	m_Materials_[id].SetBaseColor(color);
}

void HorusObjectManager::SetMetallic(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
		throw std::runtime_error("no material with this id exists. ");
	}

	m_Materials_[id].SetMetallic(texturePath);
}
void HorusObjectManager::SetMetallic(int id, glm::vec4 color)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
		throw std::runtime_error("no material with this id exists. ");
	}

	m_Materials_[id].SetMetallic(color);
}

void HorusObjectManager::SetRoughness(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
		throw std::runtime_error("no material with this id exists. ");
	}

	m_Materials_[id].SetRoughness(texturePath);
}
void HorusObjectManager::SetRoughness(int id, glm::vec4 color)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
		throw std::runtime_error("no material with this id exists. ");
	}

	m_Materials_[id].SetRoughness(color);
}

void HorusObjectManager::SetNormal(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
		throw std::runtime_error("no material with this id exists. ");
	}

	m_Materials_[id].SetNormal(texturePath);
}
void HorusObjectManager::SetNormal(int id, glm::vec4 color)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
		throw std::runtime_error("no material with this id exists. ");
	}

	m_Materials_[id].SetNormal(color);
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

void HorusObjectManager::SetReflectionColor(int id, glm::vec4 color)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
		throw std::runtime_error("no material with this id exists. ");
	}

	m_Materials_[id].SetReflectionColor(color);
}
void HorusObjectManager::SetReflectionColor(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists. ");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetReflectionColor(texturePath);
}

void HorusObjectManager::SetReflectionWeight(int id, glm::vec4 color)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetReflectionWeight(color);
}
void HorusObjectManager::SetReflectionWeight(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetReflectionWeight(texturePath);
}

void HorusObjectManager::SetReflectionRoughness(int id, glm::vec4 color)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetReflectionRoughness(color);

}
void HorusObjectManager::SetReflectionRoughness(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetReflectionRoughness(texturePath);

}

void HorusObjectManager::SetRefractionColor(int id, glm::vec4 color)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetRefractionColor(color);
}
void HorusObjectManager::SetRefractionColor(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetRefractionColor(texturePath);

}

void HorusObjectManager::SetRefractionWeight(int id, glm::vec4 weight)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material whith this id exist.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetRefractionWeight(weight);
}
void HorusObjectManager::SetRefractionWeight(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material whith this id exist.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetRefractionWeight(texturePath);
}

void HorusObjectManager::SetRefractionRoughness(int id, glm::vec4 roughness)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material whith this id exist.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetRefractionRoughness(roughness);
}
void HorusObjectManager::SetRefractionRoughness(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material whith this id exist.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetRefractionRoughness(texturePath);
}

void HorusObjectManager::SetCoatingColor(int id, glm::vec4 color)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetCoatingColor(color);
}
void HorusObjectManager::SetCoatingColor(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetCoatingColor(texturePath);
}

void HorusObjectManager::SetSheen(int id, glm::vec4 color)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetSheen(color);
}
void HorusObjectManager::SetSheen(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetSheen(texturePath);
}

void HorusObjectManager::SetSssScatterColor(int id, glm::vec4 color)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exists.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetSssScatterColor(color);
}
void HorusObjectManager::SetSssScatterColor(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exist.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetSssScatterColor(texturePath);
}

void HorusObjectManager::SetBackscatterColor(int id, glm::vec4 color)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exist.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetBackscatterColor(color);
}
void HorusObjectManager::SetBackscatterColor(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material whith this id exist.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetBackscatterColor(texturePath);
}

void HorusObjectManager::SetIor(int id, float ior)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material whith this id exist.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetIor(ior);
}

void HorusObjectManager::SetCoatingWeight(int id, glm::vec4 weight)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material whith this id exist.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetCoatingWeight(weight);
}
void HorusObjectManager::SetCoatingWeight(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material whith this id exist.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetCoatingWeight(texturePath);
}

void HorusObjectManager::SetCoatingRoughness(int id, glm::vec4 roughness)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material whith this id exist.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetCoatingRoughness(roughness);
}
void HorusObjectManager::SetCoatingRoughness(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material whith this id exist.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetCoatingRoughness(texturePath);
}

void HorusObjectManager::SetSheenWeight(int id, glm::vec4 weight)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material whith this id exist.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetSheenWeight(weight);
}
void HorusObjectManager::SetSheenWeight(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material whith this id exist.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetSheenWeight(texturePath);
}

void HorusObjectManager::SetBackscatterWeight(int id, glm::vec4 weight)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material whith this id exist.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetBackscatterWeight(weight);
}
void HorusObjectManager::SetBackscatterWeight(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetBackscatterWeight(texturePath);
}

void HorusObjectManager::SetDiffuseWeight(int id, glm::vec4 color)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetDiffuseWeight(color);
}
void HorusObjectManager::SetDiffuseWeight(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetDiffuseWeight(texturePath);
}

void HorusObjectManager::SetEmissionWeight(int id, glm::vec4 color)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetEmissionWeight(color);
}
void HorusObjectManager::SetEmissionWeight(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetEmissionWeight(texturePath);
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

void HorusObjectManager::SetSssScatterDistance(int id, glm::vec4 color)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetSssScatterDistance(color);
}
void HorusObjectManager::SetSssScatterDistance(int id, const std::string& texturePath)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetSssScatterDistance(texturePath);
}

void HorusObjectManager::SetReflectionMode(int id, int mode)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exist.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetReflectionMode(mode);
}
void HorusObjectManager::SetCoatingMode(int id, int mode)
{
	if (!m_Materials_.contains(id))
	{
		spdlog::error("no material with this id exist.");
		throw std::runtime_error("no material with this id exists.");
	}

	m_Materials_[id].SetCoatingMode(mode);
}

// -----------------------------------------------------------------------------------------------
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

// -----------------------------------------------------------------------------------------------
// Light management ------------------------------------------------------------------------------

int HorusObjectManager::CreateLight(const std::string& Name, const std::string& LightType, const std::string& ImagePath)
{
	std::string LightName = Name;

	int Suffix = 001;

	while (m_ObjectNameToIdMap_.contains(LightName))
	{
		LightName = Name + std::to_string(Suffix);
		Suffix++;

		spdlog::info("Light name already exists, trying {} instead", LightName);
	}

	int Id = IDManager::GetInstance().GetNewId();

	HorusLight NewLight;
	NewLight.Init(LightType, ImagePath);
	m_Lights_[Id] = NewLight;
	m_LightNames_[Id] = LightName;
	m_ObjectNameToIdMap_[LightName] = Id;
	m_ActiveLightId_ = Id;

	spdlog::info("Light {} created with id: {}", LightName, Id);

	return Id;
}
void HorusObjectManager::DestroyLight(int Id)
{
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

	for (auto It = m_ObjectNameToIdMap_.begin(); It != m_ObjectNameToIdMap_.end(); ++It) {
		if (It->second == Id) {
			m_ObjectNameToIdMap_.erase(It);
			break;
		}
	}

	IDManager::GetInstance().ReleaseId(Id);
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
glm::vec3 HorusObjectManager::GetLightPosition(int id)
{
	HorusLight& Light = GetLight(id);

	return Light.GetPosition();
}
glm::vec3 HorusObjectManager::GetLightRotation(int id)
{
	HorusLight& Light = GetLight(id);

	return Light.GetRotation();
}
glm::vec3 HorusObjectManager::GetLightScale(int id)
{
	HorusLight& Light = GetLight(id);

	return Light.GetScale();
}
glm::vec3 HorusObjectManager::GetLightColor(int id)
{
	HorusLight& Light = GetLight(id);

	return Light.GetColor();
}
glm::vec3 HorusObjectManager::GetLightDirection(int id)
{
	HorusLight& Light = GetLight(id);

	return Light.GetDirection();
}
glm::vec3 HorusObjectManager::GetLightIntensity(int id)
{
	HorusLight& Light = GetLight(id);

	return Light.GetIntensity();
}
int HorusObjectManager::GetActiveLightId()
{
	return m_ActiveLightId_;
}

// General Setters
void HorusObjectManager::SetActiveLightId(int id)
{
	m_ActiveLightId_ = id;
}
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


// -----------------------------------------------------------------------------------------------

int HorusObjectManager::CreateScene(const std::string& name)
{
	std::string SceneName = name;

	int Suffix = 001;

	int Id = IDManager::GetInstance().GetNewId();

	HorusScene NewScene;
	NewScene.Init();
	m_Scenes_[Id] = NewScene;
	m_ActiveSceneId_ = Id;
	m_SceneNames_[Id] = SceneName;
	m_ObjectNameToIdMap_[SceneName] = Id;

	spdlog::info("Scene {} created with id: {}", SceneName, Id);

	return Id;
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
	while (!m_ObjectNameToIdMap_.contains(name))
	{
		spdlog::info("Scene name does not exist, trying again");
	}

	return m_ObjectNameToIdMap_[name];
}

int HorusObjectManager::GetActiveSceneId()
{
	return m_ActiveSceneId_;
}

std::string& HorusObjectManager::GetSceneNameById(int id)
{
	return m_SceneNames_[id];
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

	IDManager::GetInstance().ReleaseId(id);

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

int HorusObjectManager::CreateDefaultScene()
{
	int Id = CreateScene("DefaultScene");

	CreateRadeonCamera(Id, "DefaultCamera");

	//create_light("HDRI", "hdri", "core/scene/dependencies/light/horus_hdri_main.exr");
	//int HDRI = CreateLight("Lgt_Dome01", "hdri", "resources/Textures/resting_place_2_2k.exr");
	//int HDRI = CreateLight("Lgt_Dome01", "hdri", "resources/Lookdev/Light/niederwihl_forest_4k.exr");


	//SetLightRotation(HDRI, glm::vec3(0.0f, 1.0f, 0.0f), 90.0f);

	SetScene(Id);

	return Id;
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