#pragma once


class HorusRenderCore
{
public:

	static HorusRenderCore& GetInstance()
	{
		static HorusRenderCore Instance;
		return Instance;
	}

	HorusRenderCore(HorusRenderCore const&) = delete;
	void operator=(HorusRenderCore const&) = delete;

	void PrintMessage();

private:

	HorusRenderCore() {}

};
