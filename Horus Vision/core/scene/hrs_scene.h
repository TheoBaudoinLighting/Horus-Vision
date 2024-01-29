#pragma once
#include <RadeonProRender.hpp>

#include <mutex>

class HorusScene
{
public:

	void Init();
	void DestroyScene();

	void CreateDefaultScene();
	void SetScene(rpr_scene Scene);
	void LoadScene(const char* Path);
	void UnloadScene();

	void ExportScene(const char* path);
	void ImportScene(const char* path);

	void ShowScene();
	void ClearScene();
	void UpdateScene();

	// Debug
	void ShowDummyDragon();
	void ShowDummyPlane();
	void ShowLookdevScene();
	void ShowJaguardXKSS();

	rpr_scene& GetScene() { return m_Scene_; }

private:

	rpr_scene m_Scene_ = nullptr;

	// Mutex for scene
	//std::mutex m_SceneMutex_;



};



