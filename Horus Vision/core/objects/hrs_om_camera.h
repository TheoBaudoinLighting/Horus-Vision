#pragma once
#pragma warning(disable : 4996)
#pragma warning(disable : 4244)
#pragma warning(disable : 4267)
#pragma warning(disable : 4005)

// C++ includes
#include <string>

// Project includes
#include "hrs_om_id_manager.h"
#include "hrs_opengl_camera.h"
#include "hrs_radeon_camera.h"
#include "spdlog/spdlog.h"

class HorusOmCamera
{
public:
	static HorusOmCamera& GetInstance()
	{
		static HorusOmCamera Instance; // Instance unique
		return Instance;
	}

	HorusOmCamera(const HorusOmCamera&) = delete;
	void operator=(const HorusOmCamera&) = delete;

	// Reunified camera
	int CreateCamera(std::string Name, bool EditorMode = false);
	void DestroyCamera(int ID);
	void DestroyAllCameras();

	void ResetCamera(int ID);
	void PrintCameraInfo(int ID);
	void UpdateCamera(int ID);
	void UpdateCameraMatrix(int ID, int Width, int Height);

	// Setters
	void SetActiveCamera(int ID);
	void SetActiveOpenGLCamera(int id);
	void SetActiveRadeonCamera(int id);

	void ForceCopyCamera(int id);
	void ForceCopyCameraOpenGLToRadeon(int id);
	void ForceCopyCameraRadeonToOpenGL(int id);
	void ForceCopyAllCameras();

	void SetViewport(int id, int x, int y, int width, int height);

	void SetCameraPosition(int id, glm::vec3 Position);
	void SetCameraOpenGLPosition(int id, glm::vec3 Position);
	void SetCameraRadeonPosition(int id, glm::vec3 Position);

	void SetCameraRotation(int id, glm::vec3 Rotation);
	void SetCameraOpenGLRotation(int id, glm::vec3 Rotation);
	void SetCameraRadeonRotation(int id, glm::vec3 Rotation);

	void SetCameraScale(int id, glm::vec3 Scale);
	void SetCameraOpenGLScale(int id, glm::vec3 Scale);
	void SetCameraRadeonScale(int id, glm::vec3 Scale);

	void SetCameraLookAt(int id, glm::vec3 LookAt);
	void SetCameraOpenGLLookAt(int id, glm::vec3 LookAt);
	void SetCameraRadeonLookAt(int id, glm::vec3 LookAt);

	void SetCameraFov(int id, float Fov);
	void SetCameraOpenGLFov(int id, float Fov);
	void SetCameraRadeonFov(int id, float Fov);

	void SetCameraAspectRatio(int id, float AspectRatio);
	void SetCameraOpenGLAspectRatio(int id, float AspectRatio);
	void SetCameraRadeonAspectRatio(int id, float AspectRatio);
	
	void SetCameraNear(int id, float Near);
	void SetCameraFar(int id, float Far);
	void SetCameraFocusDistance(int id, float FocusDistance);
	void SetCameraFStop(int id, float FStop);
	void SetCameraApertureBlades(int id, int Blades);
	void SetSensorSize(int id, float Width, float Height);

	// Controls
	void SetMoveCameraForward(int id);
	void SetMoveCameraBackward(int id);
	void SetMoveCameraLeft(int id);
	void SetMoveCameraRight(int id);
	void SetMoveCameraUp(int id);
	void SetMoveCameraDown(int id);
	void SetScrollCamera(int id, float delta);
	void SetPitch(int id, float pitch);
	void SetHeading(int id, float heading);

	// advanced controls
	void SetOrbitalCamera(int id, float x, float y, float sensitivity);
	void SetPanCamera(int id, float x, float y, float sensitivity);
	void SetZoomCamera(int id, float distance);

	// Getters
	int GetActiveCameraID() { if (m_ActiveRadeonCameraId_ == m_ActiveOpenGLCameraId_) { return m_ActiveRadeonCameraId_; } else { spdlog::error("Active cameras are different, returning OpenGL camera id"); return m_ActiveOpenGLCameraId_; } }
	int GetActiveOpenGLCameraID() { return m_ActiveOpenGLCameraId_; }
	int GetActiveRadeonCameraID() { return m_ActiveRadeonCameraId_; }
	std::string GetActiveCameraName() { return m_RadeonCameraNames_[GetActiveCameraID()]; }

	void GetCameraInfo(int id);
	HorusOpenGLCamera& GetOpenGLCamera(int id) { return m_OpenGlCameras_[id]; }
	HorusRadeonCamera& GetRadeonCamera(int id) { return m_RadeonCameras_[id]; }
	void GetRadeonMatrice(int id, glm::mat4& Projection, glm::mat4& View, glm::mat4& Model);
	void GetOpenGLMatrice(int id, glm::mat4& Projection, glm::mat4& View, glm::mat4& Model);

	glm::vec3 GetCameraPosition(int id) { return m_OpenGlCameras_[id].GetPosition(); }
	glm::vec3 GetCameraOpenGLPosition(int id) { return m_OpenGlCameras_[id].GetPosition(); }
	glm::vec3 GetCameraRadeonPosition(int id) { return m_RadeonCameras_[id].GetPosition(); }

	glm::vec3 GetCameraRotation(int id) { return m_OpenGlCameras_[id].GetRotation(); }
	glm::vec3 GetCameraOpenGLRotation(int id) { return m_OpenGlCameras_[id].GetRotation(); }
	glm::vec3 GetCameraRadeonRotation(int id) { return m_RadeonCameras_[id].GetRotation(); }

	glm::vec3 GetCameraScale(int id) { return m_OpenGlCameras_[id].GetScale(); }
	glm::vec3 GetCameraOpenGLScale(int id) { return m_OpenGlCameras_[id].GetScale(); }
	glm::vec3 GetCameraRadeonScale(int id) { return m_RadeonCameras_[id].GetScale(); }

	glm::vec3 GetCameraLookAt(int id) { return m_OpenGlCameras_[id].GetLookAt(); }
	glm::vec3 GetCameraOpenGLLookAt(int id) { return m_OpenGlCameras_[id].GetLookAt(); }
	glm::vec3 GetCameraRadeonLookAt(int id) { return m_RadeonCameras_[id].GetLookAt(); }

	float GetCameraFov(int id) { return m_OpenGlCameras_[id].GetFov(); }
	float GetCameraOpenGLFov(int id) { return m_OpenGlCameras_[id].GetFov(); }
	float GetCameraRadeonFov(int id) { return m_RadeonCameras_[id].GetFov(); }

	float GetCameraAspectRatio(int id) { return m_OpenGlCameras_[id].GetAspectRatio(); }
	float GetCameraOpenGLAspectRatio(int id) { return m_OpenGlCameras_[id].GetAspectRatio(); }
	float GetCameraRadeonAspectRatio(int id) { return m_RadeonCameras_[id].GetAspectRatio(); }

	float GetCameraNear(int id) { return m_OpenGlCameras_[id].GetNear(); }
	float GetCameraOpenGLNear(int id) { return m_OpenGlCameras_[id].GetNear(); }
	float GetCameraRadeonNear(int id) { return m_RadeonCameras_[id].GetNear(); }

	float GetCameraFar(int id) { return m_OpenGlCameras_[id].GetFar(); }
	float GetCameraOpenGLFar(int id) { return m_OpenGlCameras_[id].GetFar(); }
	float GetCameraRadeonFar(int id) { return m_RadeonCameras_[id].GetFar(); }

	float GetCameraFocusDistance(int id) { return m_OpenGlCameras_[id].GetFocusPlane(); }
	float GetCameraOpenGLFocusDistance(int id) { return m_OpenGlCameras_[id].GetFocusPlane(); }
	float GetCameraRadeonFocusDistance(int id) { return m_RadeonCameras_[id].GetFocusPlane(); }

	float GetCameraFStop(int id) { return m_OpenGlCameras_[id].GetFStop(); }
	float GetCameraOpenGLFStop(int id) { return m_OpenGlCameras_[id].GetFStop(); }
	float GetCameraRadeonFStop(int id) { return m_RadeonCameras_[id].GetFStop(); }

	int GetCameraApertureBlades(int id) { return m_RadeonCameras_[id].GetApertureBlades(); } // TODO : Replace by OpenGL Later
	int GetCameraOpenGLApertureBlades(int id) { return m_OpenGlCameras_[id].GetApertureBlades(); }
	int GetCameraRadeonApertureBlades(int id) { return m_RadeonCameras_[id].GetApertureBlades(); }


	// Checkers 
	void CheckCamera(int ID);
	void CheckAllCameras();
	void CheckOpenGLCamera(int ID);
	void CheckRadeonCamera(int ID);

	// For outliner
	void GetOutlinerCameras(std::vector<std::string>& Cameras)
	{
		for (const auto& Key : m_RadeonCameras_ | std::views::keys)
		{
			int Id = Key;
			std::string Name = m_RadeonCameraNames_[Id];
			Cameras.push_back(Name);
		}
	}

private:

	HorusOmCamera() {}

	std::map<int, HorusOpenGLCamera> m_OpenGlCameras_;
	int m_ActiveOpenGLCameraId_ = 0;
	std::map<int, std::string> m_OpenGlCameraNames_;

	std::map<int, HorusRadeonCamera> m_RadeonCameras_;
	int m_ActiveRadeonCameraId_ = 0;
	std::map<int, std::string> m_RadeonCameraNames_;
};

// Reunified Camera
inline int HorusOmCamera::CreateCamera(std::string Name, bool EditorMode)
{
	// TODO : Implement the editor mode, create a camera without radeon, just for the editor and no link to the radeon context
	HorusIdManager& IdManager = HorusIdManager::GetInstance();

	std::string CameraName = Name;
	int Suffix = 001;

	while (IdManager.ContainsName(CameraName))
	{
		CameraName = Name + std::to_string(Suffix);
		Suffix++;

		spdlog::warn("Camera name already exists, trying {} instead", CameraName);
	}

	int ID = IdManager.GetNewId();
	IdManager.SetObjectNameToID(CameraName, ID);

	HorusOpenGLCamera& NewOpenGLCamera = m_OpenGlCameras_[ID];
	HorusRadeonCamera& NewRadeonCamera = m_RadeonCameras_[ID];

	NewOpenGLCamera.Init(800, 600);
	NewRadeonCamera.Init(CameraName);

	m_OpenGlCameraNames_[ID] = CameraName;
	m_RadeonCameraNames_[ID] = CameraName;

	m_ActiveOpenGLCameraId_ = ID;
	m_ActiveRadeonCameraId_ = ID;

	IdManager.SetObjectNameToID(CameraName, ID);

	spdlog::info("Camera {} created with id: {}", CameraName, ID);

	return ID;
}
inline void HorusOmCamera::DestroyCamera(int ID) {
	// TODO : Destroy the camera and release the id ( recreate a default camera if no camera is left)
}
inline void HorusOmCamera::DestroyAllCameras()
{
	m_OpenGlCameras_.clear();
	m_RadeonCameras_.clear();
	m_OpenGlCameraNames_.clear();
	m_RadeonCameraNames_.clear();
	HorusIdManager::GetInstance().DeleteClearAndRelease();
	m_ActiveOpenGLCameraId_ = 0;
	m_ActiveRadeonCameraId_ = 0;
}
inline void HorusOmCamera::ResetCamera(int ID)
{
	m_RadeonCameras_[ID].Reset();
	m_OpenGlCameras_[ID].Reset();

	spdlog::info("Camera {} reset", ID);
}

inline void HorusOmCamera::PrintCameraInfo(int ID)
{
	m_OpenGlCameras_[ID].PrintCameraInfo();
	m_RadeonCameras_[ID].PrintCameraInfo();
}

inline void HorusOmCamera::UpdateCamera(int ID)
{
	m_OpenGlCameras_[ID].UpdateCamera();
	m_RadeonCameras_[ID].UpdateCamera();
}
inline void HorusOmCamera::UpdateCameraMatrix(int ID, int Width, int Height)
{
	m_OpenGlCameras_[ID].UpdateMatrix(45.0f, 0.1f, 100.0f, Width, Height);
}

inline void HorusOmCamera::SetActiveCamera(int ID)
{
	m_ActiveOpenGLCameraId_ = ID;
	m_ActiveRadeonCameraId_ = ID;
}
inline void HorusOmCamera::SetActiveOpenGLCamera(int id)
{
	m_ActiveOpenGLCameraId_ = id;
}
inline void HorusOmCamera::SetActiveRadeonCamera(int id)
{
	m_ActiveRadeonCameraId_ = id;
}

inline void HorusOmCamera::ForceCopyCamera(int id)
{

}
inline void HorusOmCamera::ForceCopyCameraOpenGLToRadeon(int id)
{
}
inline void HorusOmCamera::ForceCopyCameraRadeonToOpenGL(int id)
{
}
inline void HorusOmCamera::ForceCopyAllCameras()
{
}

inline void HorusOmCamera::SetCameraPosition(int ID, glm::vec3 Position)
{
	m_OpenGlCameras_[ID].SetPosition(Position);
	m_RadeonCameras_[ID].SetPosition(Position);
}
inline void HorusOmCamera::SetCameraOpenGLPosition(int id, glm::vec3 Position)
{
	m_OpenGlCameras_[id].SetPosition(Position);
}
inline void HorusOmCamera::SetCameraRadeonPosition(int id, glm::vec3 Position)
{
	m_RadeonCameras_[id].SetPosition(Position);
}
inline void HorusOmCamera::SetCameraRotation(int ID, glm::vec3 Rotation)
{
	m_OpenGlCameras_[ID].SetCameraRotation(Rotation);
	m_RadeonCameras_[ID].SetCameraRotation(Rotation.x, Rotation.y, Rotation.z);
}
inline void HorusOmCamera::SetCameraOpenGLRotation(int ID, glm::vec3 Rotation)
{
	m_OpenGlCameras_[ID].SetCameraRotation(Rotation);
}
inline void HorusOmCamera::SetCameraRadeonRotation(int ID, glm::vec3 Rotation)
{
	m_RadeonCameras_[ID].SetCameraRotation(Rotation.x, Rotation.y, Rotation.z);
}
inline void HorusOmCamera::SetCameraScale(int ID, glm::vec3 Scale)
{
	m_OpenGlCameras_[ID].SetCameraScale(Scale);
	m_RadeonCameras_[ID].SetCameraScale(Scale);
}
inline void HorusOmCamera::SetCameraOpenGLScale(int ID, glm::vec3 Scale)
{
	m_OpenGlCameras_[ID].SetCameraScale(Scale);
}
inline void HorusOmCamera::SetCameraRadeonScale(int ID, glm::vec3 Scale)
{
	m_RadeonCameras_[ID].SetCameraScale(Scale);
}

inline void HorusOmCamera::SetCameraLookAt(int ID, glm::vec3 LookAt)
{
	m_OpenGlCameras_[ID].SetLookAt(LookAt);
	m_RadeonCameras_[ID].SetLookAt(LookAt);
}
inline void HorusOmCamera::SetCameraOpenGLLookAt(int ID, glm::vec3 LookAt)
{
	m_OpenGlCameras_[ID].SetLookAt(LookAt);
}
inline void HorusOmCamera::SetCameraRadeonLookAt(int ID, glm::vec3 LookAt)
{
	m_RadeonCameras_[ID].SetLookAt(LookAt);
}
inline void HorusOmCamera::SetCameraFov(int ID, float Fov)
{
	m_OpenGlCameras_[ID].SetFov(Fov);
	m_RadeonCameras_[ID].SetFov(Fov);
}
inline void HorusOmCamera::SetCameraOpenGLFov(int ID, float Fov)
{
	m_OpenGlCameras_[ID].SetFov(Fov);
}
inline void HorusOmCamera::SetCameraRadeonFov(int ID, float Fov)
{
	m_RadeonCameras_[ID].SetFov(Fov);
}
inline void HorusOmCamera::SetCameraAspectRatio(int ID, float AspectRatio)
{
	m_OpenGlCameras_[ID].SetAspectRatio(AspectRatio);
	m_RadeonCameras_[ID].SetAspectRatio(AspectRatio);
}
inline void HorusOmCamera::SetCameraOpenGLAspectRatio(int ID, float AspectRatio)
{
	m_OpenGlCameras_[ID].SetAspectRatio(AspectRatio);
}
inline void HorusOmCamera::SetCameraRadeonAspectRatio(int ID, float AspectRatio)
{
	m_RadeonCameras_[ID].SetAspectRatio(AspectRatio);
}
inline void HorusOmCamera::SetCameraNear(int ID, float Near)
{
	m_OpenGlCameras_[ID].SetNear(Near);
	m_RadeonCameras_[ID].SetNear(Near);
}
inline void HorusOmCamera::SetCameraFar(int ID, float Far)
{
	m_OpenGlCameras_[ID].SetFar(Far);
	m_RadeonCameras_[ID].SetFar(Far);
}
inline void HorusOmCamera::SetCameraFocusDistance(int ID, float FocusDistance)
{
	m_RadeonCameras_[ID].SetFocusPlane(FocusDistance);
}
inline void HorusOmCamera::SetCameraFStop(int ID, float FStop)
{
	m_RadeonCameras_[ID].SetFStop(FStop);
}
inline void HorusOmCamera::SetCameraApertureBlades(int ID, int Blades)
{
	m_RadeonCameras_[ID].SetApertureBlades(Blades);
}
inline void HorusOmCamera::SetSensorSize(int ID, float Width, float Height)
{
	m_RadeonCameras_[ID].SetSensorSize(Width, Height);
} 

// Controls
inline void HorusOmCamera::SetMoveCameraForward(int ID)
{
	//m_OpenGlCameras_[ID].MoveCamera(Forward);
	//m_RadeonCameras_[ID].MoveCamera(Forward);
}
inline void HorusOmCamera::SetMoveCameraBackward(int ID)
{
	//m_OpenGlCameras_[ID].MoveCamera(Backward);
	//m_RadeonCameras_[ID].MoveCamera(Backward);
}
inline void HorusOmCamera::SetMoveCameraLeft(int ID)
{
	//m_OpenGlCameras_[ID].MoveCamera(Left);
	//m_RadeonCameras_[ID].MoveCamera(Left);
}
inline void HorusOmCamera::SetMoveCameraRight(int ID)
{
	//m_OpenGlCameras_[ID].MoveCamera(Right);
	//m_RadeonCameras_[ID].MoveCamera(Right);
}
inline void HorusOmCamera::SetMoveCameraUp(int ID)
{
	//m_OpenGlCameras_[ID].MoveCamera(Up);
	//m_RadeonCameras_[ID].MoveCamera(Up);
}
inline void HorusOmCamera::SetMoveCameraDown(int ID)
{
	//m_OpenGlCameras_[ID].MoveCamera(Down);
	//m_RadeonCameras_[ID].MoveCamera(Down);
}
inline void HorusOmCamera::SetScrollCamera(int ID, float delta)
{
	//m_OpenGlCameras_[ID].Zoom(delta);
	//m_RadeonCameras_[ID].Zoom(delta);
}
inline void HorusOmCamera::SetPitch(int ID, float pitch)
{
	//m_OpenGlCameras_[ID].SetPitch(pitch);
	//m_RadeonCameras_[ID].ChangePitch(pitch);
}
inline void HorusOmCamera::SetHeading(int ID, float heading)
{
	//m_OpenGlCameras_[ID].SetHeading(heading);
	//m_RadeonCameras_[ID].ChangeHeading(heading);
}

inline void HorusOmCamera::SetOrbitalCamera(int ID, float x, float y, float sensitivity)
{
	m_OpenGlCameras_[ID].Tumble(x, y, sensitivity);
	m_RadeonCameras_[ID].Tumbling(x, y, sensitivity);
}
inline void HorusOmCamera::SetPanCamera(int ID, float x, float y, float sensitivity)
{
	m_OpenGlCameras_[ID].Pan(x, y, sensitivity);
	m_RadeonCameras_[ID].Pan(x, y, sensitivity);
}
inline void HorusOmCamera::SetZoomCamera(int ID, float distance)
{
	m_OpenGlCameras_[ID].Zoom(distance);
	m_RadeonCameras_[ID].Zoom(distance);
}

inline void HorusOmCamera::CheckCamera(int ID)
{
	// TODO : Check the value between the two cameras
}
inline void HorusOmCamera::CheckAllCameras()
{
}
inline void HorusOmCamera::CheckOpenGLCamera(int ID)
{
}
inline void HorusOmCamera::CheckRadeonCamera(int ID)
{
	m_RadeonCameras_[ID].VariableCheckers("Manual Check");
}

// Getters
inline void HorusOmCamera::GetCameraInfo(int ID)
{
	m_OpenGlCameras_[ID].PrintCameraInfo();
	m_RadeonCameras_[ID].GetCameraInfo();
}
inline void HorusOmCamera::GetRadeonMatrice(int ID, glm::mat4& Projection, glm::mat4& View, glm::mat4& Model)
{
	m_RadeonCameras_[ID].GetMatrices(Projection, View, Model);
}
inline void HorusOmCamera::GetOpenGLMatrice(int ID, glm::mat4& Projection, glm::mat4& View, glm::mat4& Model)
{
	m_OpenGlCameras_[ID].GetMatrices(View, Projection, Model);
}

inline void HorusOmCamera::SetViewport(int id, int x, int y, int width, int height)
{
	m_OpenGlCameras_[id].SetViewport(x, y, width, height);
	m_RadeonCameras_[id].SetViewport(x, y, width, height);
}



//void HorusOmCamera::DestroyeCamera(int id)
//{
//	if (id == m_ActiveRadeonCameraId_)
//	{
//		if (!m_RadeonCameras_.empty())
//		{
//			m_ActiveRadeonCameraId_ = m_RadeonCameras_.begin()->first;
//		}
//		else
//		{
//			CreateRadeonCamera(GetActiveSceneId(), "DefaultCamera");
//		}
//	}
//
//	if (auto It = m_RadeonCameras_.find(id); It != m_RadeonCameras_.end())
//	{
//		It->second.Destroy();
//		m_RadeonCameras_.erase(It);
//		m_RadeonCameraNames_.erase(id);
//	}
//	else
//	{
//		spdlog::error("no camera with this id exists. ");
//	}
//
//	HorusIdManager::GetInstance().ReleaseId(id);
//
//	if (m_RadeonCameras_.empty())
//	{
//		CreateRadeonCamera(GetActiveSceneId(), "DefaultCamera");
//	}
//}