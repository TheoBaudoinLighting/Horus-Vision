#pragma once
#include <RadeonProRender_v2.h>

class HorusScene
{
public:

	void init();
	void destroy_scene();

	void CreateDefaultScene();
	void SetScene(rpr_scene scene);
	void load_scene(const char* path);
	void unload_scene();

	void export_scene(const char* path);
	void import_scene(const char* path);

	void show_scene();
	void clear_scene();
	void update_scene();

	// Debug
	void show_dummy_dragon();
	void show_dummy_plane();
	void show_LookdevScene();

	rpr_scene get_scene() { return m_scene_; }

private:

	rpr_scene m_scene_ = nullptr;
};



