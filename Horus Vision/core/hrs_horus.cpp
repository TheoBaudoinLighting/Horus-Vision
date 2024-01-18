
// Projects Include
#include "hrs_engine.h" 

int main()
{
	HorusEngine& Engine = HorusEngine::GetInstance();

	Engine.Init(1200, 850, "Horus Vision", "DefaultUserSave.json");

	while (Engine.IsRunning())
	{
		Engine.PreRender();
		Engine.Render();
		Engine.PostRender();
	}

	spdlog::info("Exiting...");
	return 0;
}


