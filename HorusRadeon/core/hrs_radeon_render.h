#pragma once

#include "RadeonProRender_V2.h"
#include "common.h"

class HorusRadeonRender
{
public:

	static HorusRadeonRender& GetInstance()
	{
		static HorusRadeonRender Instance;
		return Instance;
	}

	HorusRadeonRender(HorusRadeonRender const&) = delete;
	void operator=(HorusRadeonRender const&) = delete;

	// TODO : Add Move actual render code here

private:

	HorusRadeonRender() {}
	

};