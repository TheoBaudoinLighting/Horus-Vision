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

	void Add(rpr_material_node Material) { m_NodesCollector_.push_back(Material); }
	void Add(rpr_image Image) { m_NodesCollector_.push_back(Image); }
	void Add(rpr_shape Shape) { m_NodesCollector_.push_back(Shape); }
	void Add(rpr_light Light) { m_NodesCollector_.push_back(Light); }
	void Add(rpr_framebuffer Framebuffer) { m_NodesCollector_.push_back(Framebuffer); }
	void Add(rpr_camera Camera) { m_NodesCollector_.push_back(Camera); }

	void Clean()
	{
		for (const auto& i : m_NodesCollector_)
			if (i) { CHECK(rprObjectDelete(i)) }
		m_NodesCollector_.clear();
	}

private:

	HorusGarbageCollector() {}

	std::vector<void*> m_NodesCollector_;

};