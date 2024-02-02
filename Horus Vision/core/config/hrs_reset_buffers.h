#pragma once

#include "common.h"
#include "hrs_ui.h" // nothing
#include "hrs_radeon.h" // glfw3.h
#include "hrs_timer.h"

class HorusResetBuffers
{
public:
	static HorusResetBuffers& GetInstance()
	{
		static HorusResetBuffers Instance;
		return Instance;
	}

	HorusResetBuffers(HorusResetBuffers const&) = delete;
	void operator=(HorusResetBuffers const&) = delete;

	// Mutex
	std::mutex Mutex;

	void CallResetBuffers()
	{
		HorusRadeon& Radeon = HorusRadeon::GetInstance();
		HorusUI& Ui = HorusUI::GetInstance();
		HorusTimerManager::GetInstance().ResetTimer("RenderTimer");

		if (Radeon.GetIsAdaptiveRender())
		{
			Ui.SetOptionsChanged(true);
			Radeon.SetIsDirty(true);
			Radeon.SetActivePixelRemains(Radeon.GetWindowSize().x / Radeon.GetWindowSize().y);
			CHECK(rprFrameBufferClear(Radeon.GetAdaptiveRenderFrameBuffer()));
		}
		else
		{
			Ui.SetOptionsChanged(true);
			Radeon.SetIsDirty(true);
			Radeon.SetSampleCount(1);
			CHECK(rprFrameBufferClear(Radeon.GetClassicRenderFrameBuffer()));
		}
	}

private:
	HorusResetBuffers() {}

};
