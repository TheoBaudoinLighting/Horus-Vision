#pragma once




class HorusImGuiCore
{
public:

	static HorusImGuiCore& GetInstance()
	{
		static HorusImGuiCore Instance;
		return Instance;
	}

	HorusImGuiCore(HorusImGuiCore const&) = delete;
	void operator=(HorusImGuiCore const&) = delete;

	void PrintMessage();

private:

	HorusImGuiCore() {}

};