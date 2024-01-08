#pragma once

#include "glad/glad.h"

#include "hrs_context.h"
#include "hrs_logger.h"

class HorusOpenGL : public HorusContext
{
public:

	static HorusOpenGL& get_instance()
	{
		static HorusOpenGL instance; // Instance unique
		return instance;
	}

	HorusOpenGL(const HorusOpenGL&) = delete;
	void operator=(const HorusOpenGL&) = delete;

	bool init(int width, int height, HorusWindowConfig* window) override;

	void init_render() override;
	void post_render() override;
	void quit_render() override;

private:

	HorusOpenGL() {}

	HorusLogger m_logger_;

	const char* glsl_version_ = "#version 460";
};