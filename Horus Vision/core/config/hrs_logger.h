#pragma once

#include "spdlog/spdlog.h"

class HorusLogger
{
public:
	static HorusLogger& GetInstance()
	{
		static HorusLogger Instance;
		return Instance;
	}

	void Init();

	std::shared_ptr<spdlog::logger> GetLogger() { return Logger; }

	std::shared_ptr<spdlog::logger> Logger;
};
