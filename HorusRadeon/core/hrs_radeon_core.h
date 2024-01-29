#pragma once



class HorusRadeonCore
{
public:

	static HorusRadeonCore& GetInstance()
	{
		static HorusRadeonCore Instance;
		return Instance;
	}

	HorusRadeonCore(HorusRadeonCore const&) = delete;
	void operator=(HorusRadeonCore const&) = delete;

	void PrintMessage();

private:

	HorusRadeonCore() {}

};