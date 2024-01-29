#pragma once

// External includes
#include "RadeonProRender_V2.h"

// Basic includes
#include <iostream>
#include <vector>
#include <mutex>

#include "common.h"
#include "spdlog/spdlog.h"

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

	template<typename T>
	void Remove(T item)
	{
		// TODO : correct the same instance problem, for now it's a workaround
		std::lock_guard<std::mutex> lock(CleanMutex);
		m_NodesCollector_.erase(
			std::remove(m_NodesCollector_.begin(), m_NodesCollector_.end(), static_cast<void*>(item)),
			m_NodesCollector_.end()
		);
	}

	void Add(rpr_material_node Material) { m_NodesCollector_.push_back(Material); }
	void Add(rpr_image Image) { m_NodesCollector_.push_back(Image); }
	void Add(rpr_shape Shape) { m_NodesCollector_.push_back(Shape); }
	void Add(rpr_light Light) { m_NodesCollector_.push_back(Light); }
	void Add(rpr_framebuffer Framebuffer) { m_NodesCollector_.push_back(Framebuffer); }
	void Add(rpr_camera Camera) { m_NodesCollector_.push_back(Camera); }
	void Add(rpr_scene Scene) { m_NodesCollector_.push_back(Scene); }
	void Add(rpr_composite Composite) { m_NodesCollector_.push_back(Composite); }
	void Add(rpr_post_effect PostEffect) { m_NodesCollector_.push_back(PostEffect); }

	void Clean()
	{
		for (const auto& i : m_NodesCollector_)
		{
			if (i != nullptr)
			{
				try
				{
					CHECK(rprObjectDelete(i));
				}
				catch (const std::exception& e)
				{
					spdlog::error("Exception caught while trying to delete an object: {}", e.what());
				}
			}
		}
		m_NodesCollector_.clear();
	}


private:

	HorusGarbageCollector() = default;

	std::vector<void*> m_NodesCollector_;

};