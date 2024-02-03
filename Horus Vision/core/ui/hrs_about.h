#pragma once


class HorusAbout
{
public:
	static HorusAbout& GetInstance()
	{
		static HorusAbout Instance;
		return Instance;
	}

	HorusAbout(HorusAbout const&) = delete;
	void operator=(HorusAbout const&) = delete;

	void About(bool* p_open);

private:

	HorusAbout() = default;
};
