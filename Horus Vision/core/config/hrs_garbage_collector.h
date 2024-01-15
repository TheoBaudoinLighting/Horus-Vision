#pragma once

// External includes
#include "RadeonProRender_V2.h"
#include "common.h"

// Basic includes
#include <vector>

class HorusGarbageCollector
{
public:

	static HorusGarbageCollector& get_instance()
	{
		static HorusGarbageCollector instance; 
		return instance;
	}

	HorusGarbageCollector(const HorusGarbageCollector&) = delete;
	void operator=(const HorusGarbageCollector&) = delete;

	void add(rpr_material_node material) { m_NodesCollector_.push_back(material); }
	void add(rpr_image image) { m_NodesCollector_.push_back(image); }
	void add(rpr_shape shape) { m_NodesCollector_.push_back(shape); }
	void add(rpr_light light) { m_NodesCollector_.push_back(light); }
	void add(rpr_framebuffer framebuffer) { m_NodesCollector_.push_back(framebuffer); }
	void add(rpr_camera camera) { m_NodesCollector_.push_back(camera); }

	void clean()
	{
		for (const auto& i : m_NodesCollector_)
			if (i) { CHECK(rprObjectDelete(i)); }
		m_NodesCollector_.clear();
	}

private:

	HorusGarbageCollector() {}

	std::vector<void*> m_NodesCollector_;

};