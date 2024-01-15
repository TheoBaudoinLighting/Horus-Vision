
// Projects Include
#include "hrs_engine.h" 

void main()
{
	HorusEngine& engine = HorusEngine::get_instance();

	engine.Init(800, 600, "Horus Vision", "DefaultUserSave.json");

	while (engine.IsRunning())
	{
		engine.PreRender();
		engine.Render();
		engine.PostRender();
	}

	spdlog::info("Exiting...");
	return exit(0);
}


