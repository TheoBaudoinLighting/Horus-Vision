#pragma once

#include "hrs_config.h" // glfw3.h

#include <RadeonProRender.hpp>
#include <string>


class HorusContext
{
public:
	virtual ~HorusContext() = default;

	HorusContext(): m_Window_()
	{
	}

	virtual bool Init(int Width, int Height, HorusWindowConfig* WindowConfig)
	{
		m_Window_ = WindowConfig;
		m_WindowWidth_ = Width;
		m_WindowHeight_ = Height;

		return true;
	}

	virtual void InitRender() = 0;
	virtual void PostRender() = 0;
	virtual void QuitRender() = 0;

protected:
	HorusWindowConfig* m_Window_;

	int m_WindowWidth_ = 0;
	int m_WindowHeight_ = 0;
};
