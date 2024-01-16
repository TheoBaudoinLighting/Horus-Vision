#include "hrs_logger.h"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

void HorusLogger::Init()
{
	auto ConsoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	auto FileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/horus_vision_logs.txt", true);

	std::string Pattern = "[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [Horus] %v";
	ConsoleSink->set_pattern(Pattern);
	FileSink->set_pattern(Pattern);

	Logger = std::make_shared<spdlog::logger>("Horus Vision Logs",
	                                             spdlog::sinks_init_list({ConsoleSink, FileSink}));
	Logger->set_level(spdlog::level::info);

	set_default_logger(Logger);
}
