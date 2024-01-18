#pragma once

// External includes
#include "RadeonProRender_V2.h"
#include "common.h"

// Basic includes
#include <vector>
#include <mutex>

inline std::mutex CleanMutex;

class HorusGarbageCollector
{
public:

	static HorusGarbageCollector& GetInstance()
	{
		static HorusGarbageCollector Instance; 
		return Instance;
	}

	HorusGarbageCollector(const HorusGarbageCollector&) = delete;
	void operator=(const HorusGarbageCollector&) = delete;

	void Add(rpr_material_node Material) { m_NodesCollector_.push_back(Material); }
	void Add(rpr_image Image) { m_NodesCollector_.push_back(Image); }
	void Add(rpr_shape Shape) { m_NodesCollector_.push_back(Shape); }
	void Add(rpr_light Light) { m_NodesCollector_.push_back(Light); }
	void Add(rpr_framebuffer Framebuffer) { m_NodesCollector_.push_back(Framebuffer); }
	void Add(rpr_camera Camera) { m_NodesCollector_.push_back(Camera); }

	void Clean() // TODO : add mutex
	{
		std::lock_guard Guard(CleanMutex);
		for (const auto& i : m_NodesCollector_)
			if (i) { rprObjectDelete(i); }
		m_NodesCollector_.clear();
	}

private:

	HorusGarbageCollector() {}

	std::vector<void*> m_NodesCollector_;

};