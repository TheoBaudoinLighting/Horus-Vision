#pragma once

// Basic includes
#include <string>
#include <memory>

class HorusEngineCore
{
public:

	static HorusEngineCore& GetInstance()
	{
		static HorusEngineCore Instance;
		return Instance;
	}

	HorusEngineCore(HorusEngineCore const&) = delete;
	void operator=(HorusEngineCore const&) = delete;

	void PrintMessage(); // TODO : Remove this

	void Init(int Width, int Height, const std::string& Title, const std::string& SaveFilename);

private:

	HorusEngineCore() {}

};