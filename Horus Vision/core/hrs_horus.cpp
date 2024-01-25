
// Projects Include
#include "hrs_engine.h" 
#include "hrs_timer.h"

int main()
{
	HorusEngine& Engine = HorusEngine::GetInstance();
	HorusTimerManager::GetInstance().StartTimer("EngineInit");

	Engine.Init(1920, 1080, "Horus Vision", "DefaultUserSave.json");

	while (Engine.IsRunning())
	{
		Engine.PreRender();
		Engine.Render();
		Engine.PostRender();
	}

	spdlog::info("Exiting...");
	return 0;
}


