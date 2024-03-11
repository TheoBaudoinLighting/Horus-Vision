#pragma once

// C++ Standard Library
#include <iostream>
#include <fstream>
#include <string>

// Spdlog
#include <spdlog/spdlog.h>

// Json 
#include <nlohmann/json.hpp>

inline std::string HORUS_APP_VERSION = "Version 1.4.0";
inline std::string HORUS_APP_VERSION_VIEWER = "Viewer 5.1.2";
inline std::string HORUS_APP_ORGANIZATION = "TBM";
inline int HORUS_APP_NUMBER_OF_GPUS = 1;
inline int HORUS_APP_NUMBER_OF_CPU = 1;
inline std::string HORUS_APP_HIPBIN_PATH = "hipbin";
inline std::string HORUS_APP_CACHE_PATH = "cache";
inline std::string HORUS_APP_TEXTURE_CACHE_PATH = "cache";
inline std::string HORUS_APP_OOC_CACHE_PATH = "cache";
inline std::string HORUS_UI_THEME = "GreenHorus";
inline std::string HORUS_UI_FONT = "Montserra";
inline int HORUS_UI_FONT_SIZE = 14;

inline std::string HORUS_UI_NAME_MAINWINDOW = "Horus Vision";
inline std::string HORUS_UI_NAME_OPENGLVIEWPORT = "Viewport";
inline std::string HORUS_UI_NAME_RADEONVIEWPORT = "Render";
inline std::string HORUS_UI_NAME_OUTLINER = "Outliner";
inline std::string HORUS_UI_NAME_CONSOLE = "Console";
inline std::string HORUS_UI_NAME_SCENE = "Scene";
inline std::string HORUS_UI_NAME_INSPECTOR = "Inspector";
inline std::string HORUS_UI_NAME_STATISTICS = "Statistics";
inline std::string HORUS_UI_NAME_SETTINGS = "Settings";
inline std::string HORUS_UI_NAME_ABOUT = "About";
inline std::string HORUS_UI_NAME_HELP = "Help";

inline std::string HORUS_UI_APP_MAIN_LOGO_PATH = "resources/Icons/Horus_Logo_100x20.png";

// Config file
inline nlohmann::json SaveDefaultConfig(std::string& FilePath)
{
	nlohmann::json Config;

	// General parameters
	Config["HORUS_APP_VERSION"] = HORUS_APP_VERSION;
	Config["HORUS_APP_VERSION_VIEWER"] = HORUS_APP_VERSION_VIEWER; //Config["HORUS_APP_VERSION_VIEWER"]
	Config["HORUS_APP_ORGANIZATION"] = HORUS_APP_ORGANIZATION;
	Config["HORUS_APP_NUMBER_OF_GPUS"] = HORUS_APP_NUMBER_OF_GPUS;
	Config["HORUS_APP_NUMBER_OF_CPU"] = HORUS_APP_NUMBER_OF_CPU;
	Config["HORUS_APP_HIPBIN_PATH"] = HORUS_APP_HIPBIN_PATH;
	Config["HORUS_APP_CACHE_PATH"] = HORUS_APP_CACHE_PATH;
	Config["HORUS_APP_TEXTURE_CACHE_PATH"] = HORUS_APP_TEXTURE_CACHE_PATH;
	Config["HORUS_APP_OOC_CACHE_PATH"] = HORUS_APP_OOC_CACHE_PATH;

	// UI settings
	Config["HORUS_UI_THEME"] = HORUS_UI_THEME;
	Config["HORUS_UI_FONT"] = HORUS_UI_FONT;
	Config["HORUS_UI_FONT_SIZE"] = HORUS_UI_FONT_SIZE;

	// UI Interface names
	Config["HORUS_UI_NAME_MAINWINDOW"] = HORUS_UI_NAME_MAINWINDOW;
	Config["HORUS_UI_NAME_OPENGLVIEWPORT"] = HORUS_UI_NAME_OPENGLVIEWPORT;
	Config["HORUS_UI_NAME_RADEONVIEWPORT"] = HORUS_UI_NAME_RADEONVIEWPORT;
	Config["HORUS_UI_NAME_OUTLINER"] = HORUS_UI_NAME_OUTLINER;
	Config["HORUS_UI_NAME_CONSOLE"] = HORUS_UI_NAME_CONSOLE;
	Config["HORUS_UI_NAME_SCENE"] = HORUS_UI_NAME_SCENE;
	Config["HORUS_UI_NAME_INSPECTOR"] = HORUS_UI_NAME_INSPECTOR;
	Config["HORUS_UI_NAME_STATISTICS"] = HORUS_UI_NAME_STATISTICS;
	Config["HORUS_UI_NAME_SETTINGS"] = HORUS_UI_NAME_SETTINGS;
	Config["HORUS_UI_NAME_ABOUT"] = HORUS_UI_NAME_ABOUT;
	Config["HORUS_UI_NAME_HELP"] = HORUS_UI_NAME_HELP;

	// UI paths
	Config["HORUS_UI_APP_MAIN_LOGO_PATH"] = HORUS_UI_APP_MAIN_LOGO_PATH;

	if (std::ofstream ConfigFile(FilePath); ConfigFile.is_open())
	{
		ConfigFile << Config.dump(4);
		ConfigFile.close();
		spdlog::info("Configuration saved to {}", FilePath);
	}
	else
	{
		spdlog::error("Failed to open {} for writing.", FilePath);
	}

	return Config;
}
inline nlohmann::json SaveConfig(std::string& filePath)
{
	nlohmann::json config;

	// General parameters
	config["HORUS_APP_VERSION"] = "Version 1.4.0";
	config["HORUS_APP_VERSION_VIEWER"] = "Viewer 5.1.2";
	config["HORUS_APP_ORGANIZATION"] = "TBM";
	config["HORUS_APP_NUMBER_OF_GPUS"] = 1;
	config["HORUS_APP_NUMBER_OF_CPU"] = 1;
	config["HORUS_APP_HIPBIN_PATH"] = "hipbin";
	config["HORUS_APP_CACHE_PATH"] = "cache";
	config["HORUS_APP_TEXTURE_CACHE_PATH"] = "cache";
	config["HORUS_APP_OOC_CACHE_PATH"] = "cache";

	// UI settings
	config["HORUS_UI_THEME"] = "GreenHorus";
	config["HORUS_UI_FONT"] = "Montserra";
	config["HORUS_UI_FONT_SIZE"] = 14;

	// UI Interface names
	config["HORUS_UI_NAME_MAINWINDOW"] = "Horus Vision";
	config["HORUS_UI_NAME_OPENGLVIEWPORT"] = "Viewport";
	config["HORUS_UI_NAME_RADEONVIEWPORT"] = "Render";
	config["HORUS_UI_NAME_OUTLINER"] = "Outliner";
	config["HORUS_UI_NAME_CONSOLE"] = "Console";
	config["HORUS_UI_NAME_SCENE"] = "Scene";
	config["HORUS_UI_NAME_INSPECTOR"] = "Inspector";
	config["HORUS_UI_NAME_STATISTICS"] = "Statistics";
	config["HORUS_UI_NAME_SETTINGS"] = "Settings";
	config["HORUS_UI_NAME_ABOUT"] = "About";
	config["HORUS_UI_NAME_HELP"] = "Help";

	// UI paths
	config["HORUS_UI_APP_MAIN_LOGO_PATH"] = "resources/Icons/Horus_Logo_100x20.png";

	std::ofstream configFile(filePath);
	if (configFile.is_open()) {
		configFile << config.dump(4);
		configFile.close();
		spdlog::info("Configuration saved to {}", filePath);
	}
	else {
		spdlog::error("Failed to open {} for writing.", filePath);
	}

	return config;
}

inline void PrintConfig(nlohmann::json& config)
{
	std::cout << config.dump(4) << '\n';
}
inline nlohmann::json LoadConfig(std::string& path)
{
	std::ifstream ConfigFile(path);

	if (!ConfigFile.is_open())
	{
		spdlog::warn("Failed to open config file: {}", path);
		return SaveDefaultConfig(path);
	}

	nlohmann::json config;
	ConfigFile >> config;
	return config;
}
inline nlohmann::json CheckConfigFile(std::string& FilePath)
{
	std::ifstream ConfigFile(FilePath);
	if (!ConfigFile.is_open()) {
		spdlog::warn("Configuration file not found. Creating default configuration file.");
		return SaveDefaultConfig(FilePath);
	}

	spdlog::info("Configuration file found.");
	return LoadConfig(FilePath);
}

inline void ExtractConfig(nlohmann::json& Config)
{
	// General parameters
	HORUS_APP_VERSION = Config["HORUS_APP_VERSION"].get<std::string>();
	HORUS_APP_ORGANIZATION = Config["HORUS_APP_ORGANIZATION"].get<std::string>();
	HORUS_APP_NUMBER_OF_GPUS = Config["HORUS_APP_NUMBER_OF_GPUS"].get<int>();
	HORUS_APP_NUMBER_OF_CPU = Config["HORUS_APP_NUMBER_OF_CPU"].get<int>();
	HORUS_APP_HIPBIN_PATH = Config["HORUS_APP_HIPBIN_PATH"].get<std::string>();
	HORUS_APP_CACHE_PATH = Config["HORUS_APP_CACHE_PATH"].get<std::string>();
	HORUS_APP_TEXTURE_CACHE_PATH = Config["HORUS_APP_TEXTURE_CACHE_PATH"].get<std::string>();
	HORUS_APP_OOC_CACHE_PATH = Config["HORUS_APP_OOC_CACHE_PATH"].get<std::string>();

	// UI settings
	HORUS_UI_THEME = Config["HORUS_UI_THEME"].get<std::string>();
	HORUS_UI_FONT = Config["HORUS_UI_FONT"].get<std::string>();
	HORUS_UI_FONT_SIZE = Config["HORUS_UI_FONT_SIZE"].get<int>();

	// UI Interface names
	HORUS_UI_NAME_MAINWINDOW = Config["HORUS_UI_NAME_MAINWINDOW"].get<std::string>();
	HORUS_UI_NAME_OPENGLVIEWPORT = Config["HORUS_UI_NAME_OPENGLVIEWPORT"].get<std::string>();
	HORUS_UI_NAME_RADEONVIEWPORT = Config["HORUS_UI_NAME_RADEONVIEWPORT"].get<std::string>();
	HORUS_UI_NAME_OUTLINER = Config["HORUS_UI_NAME_OUTLINER"].get<std::string>();
	HORUS_UI_NAME_CONSOLE = Config["HORUS_UI_NAME_CONSOLE"].get<std::string>();
	HORUS_UI_NAME_SCENE = Config["HORUS_UI_NAME_SCENE"].get<std::string>();
	HORUS_UI_NAME_INSPECTOR = Config["HORUS_UI_NAME_INSPECTOR"].get<std::string>();
	HORUS_UI_NAME_STATISTICS = Config["HORUS_UI_NAME_STATISTICS"].get<std::string>();
	HORUS_UI_NAME_SETTINGS = Config["HORUS_UI_NAME_SETTINGS"].get<std::string>();
	HORUS_UI_NAME_ABOUT = Config["HORUS_UI_NAME_ABOUT"].get<std::string>();
	HORUS_UI_NAME_HELP = Config["HORUS_UI_NAME_HELP"].get<std::string>();

	// UI paths
	HORUS_UI_APP_MAIN_LOGO_PATH = Config["HORUS_UI_APP_MAIN_LOGO_PATH"].get<std::string>();
}
