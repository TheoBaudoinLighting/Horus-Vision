#pragma once

#include "hrs_ui.h" // nothing
#include "hrs_radeon.h" // glfw3.h

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

	void CallResetBuffers()
	{
		HorusRadeon& Radeon = HorusRadeon::GetInstance();
		HorusUI& Ui = HorusUI::GetInstance();

		Ui.SetOptionsChanged(true);
		Radeon.SetIsDirty(true);
		Radeon.SetSampleCount(1);
		CHECK(rprFrameBufferClear(Radeon.GetFrameBuffer()));
	}

private:
	HorusResetBuffers() {}

};