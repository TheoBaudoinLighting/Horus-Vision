#pragma once

class HorusOpenGlResource 
{
public:
	HorusOpenGlResource() = default;
	virtual ~HorusOpenGlResource() = 0;

	// Disable copy/move constructor and assignment operator
	HorusOpenGlResource(const HorusOpenGlResource&) = delete;
	HorusOpenGlResource& operator=(const HorusOpenGlResource&) = delete;

	HorusOpenGlResource(HorusOpenGlResource&&) noexcept = default;
	HorusOpenGlResource& operator=(HorusOpenGlResource&&) noexcept = default;
};

inline HorusOpenGlResource::~HorusOpenGlResource() {}