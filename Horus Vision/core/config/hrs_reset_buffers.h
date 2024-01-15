#pragma once

#include "hrs_ui.h" // nothing
#include "hrs_radeon.h" // glfw3.h

class HorusResetBuffers
{
public:
	static HorusResetBuffers& get_instance()
	{
		static HorusResetBuffers instance;
		return instance;
	}

	HorusResetBuffers(HorusResetBuffers const&) = delete;
	void operator=(HorusResetBuffers const&) = delete;

	void CallResetBuffers()
	{
		HorusRadeon& radeon = HorusRadeon::get_instance();
		HorusUI& ui = HorusUI::get_instance();

		ui.SetOptionsChanged(true);
		radeon.set_is_dirty(true);
		radeon.set_sample_count(1);
		CHECK(rprFrameBufferClear(radeon.get_frame_buffer()));
	}

private:
	HorusResetBuffers() {}

};