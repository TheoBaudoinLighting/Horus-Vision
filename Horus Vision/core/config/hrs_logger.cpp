#include "hrs_logger.h"

#include "spdlog/sinks/basic_file_sink.h"

void HorusLogger::init()
{
	auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/horus_vision_logs.txt", true);

	std::string pattern = "[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [Horus] %v";
	console_sink->set_pattern(pattern);
	file_sink->set_pattern(pattern);

	m_logger_ = std::make_shared<spdlog::logger>("Horus Vision Logs",
	                                             spdlog::sinks_init_list({console_sink, file_sink}));
	m_logger_->set_level(spdlog::level::info);

	set_default_logger(m_logger_);
}

void HorusLogger::quit()
{
}
