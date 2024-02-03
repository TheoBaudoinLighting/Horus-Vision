#pragma once

class HorusHelp
{
public:

	static HorusHelp& GetInstance()
	{
		static HorusHelp Instance;
		return Instance;
	}

	HorusHelp(HorusHelp const&) = delete;
	void operator=(HorusHelp const&) = delete;

	void RenderHelp(bool* p_open);

private:

	HorusHelp() = default;
};