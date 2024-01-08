#pragma once

#include <iostream>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

class HorusLogger
{
public:
	static HorusLogger& get_instance()
	{
		static HorusLogger instance;
		return instance;
	}

	void init();

	std::shared_ptr<spdlog::logger> get_logger() { return m_logger_; }

	std::shared_ptr<spdlog::logger> m_logger_;
};
