#pragma once

#include "hrs_config.h"

#include <RadeonProRender_v2.h>
#include <string>


class HorusContext
{
public:
	HorusContext(): m_window_()
	{
	}


	virtual bool init(int width, int height, HorusWindowConfig* window_config)
	{
		m_window_ = window_config;
		m_window_width_ = width;
		m_window_height_ = height;

		return true;
	}

	virtual void init_render() = 0;
	virtual void post_render() = 0;
	virtual void quit_render() = 0;

protected:
	HorusWindowConfig* m_window_;

	int m_window_width_ = 0;
	int m_window_height_ = 0;
};
