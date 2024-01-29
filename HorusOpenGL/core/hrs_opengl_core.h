#pragma once





class HorusOpenGLCore
{
public:

	static HorusOpenGLCore& GetInstance()
	{
		static HorusOpenGLCore Instance;
		return Instance;
	}

	HorusOpenGLCore(HorusOpenGLCore const&) = delete;
	void operator=(HorusOpenGLCore const&) = delete;

	void PrintMessage();

private:

	HorusOpenGLCore() {}

};