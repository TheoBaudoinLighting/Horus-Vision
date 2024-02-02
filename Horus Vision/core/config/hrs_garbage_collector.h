#pragma once

// External includes
#include "RadeonProRender_V2.h"

// Basic includes
#include <iostream>
#include <vector>
#include <mutex>
#include <ranges>

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
	void Remove(T item) {
		std::lock_guard Lock(CleanMutex);
		m_NodesCollector_.erase(
			std::remove_if(
				m_NodesCollector_.begin(),
				m_NodesCollector_.end(),
				[item](const std::pair<void*, ObjectType>& pair) { return pair.first == static_cast<void*>(item); }
			),
			m_NodesCollector_.end()
		);
	}


	enum ObjectType {
		Material,
		Image,
		Shape,
		Light,
		Framebuffer,
		Camera,
		Scene,
		Composite,
		PostEffect,
	};

	void Adding(void* Obj, ObjectType Type)
	{
		m_NodesCollector_.emplace_back(Obj, Type);
		//std::cout << "Adding " << Type << " to garbage collector." << '\n';
	}

	void Add(rpr_material_node material) { Adding((void*)material, Material); }
	void Add(rpr_image image) { Adding((void*)image, Image); }
	void Add(rpr_shape shape) { Adding((void*)shape, Shape); }
	void Add(rpr_light light) { Adding((void*)light, Light); }
	void Add(rpr_framebuffer framebuffer) { Adding((void*)framebuffer, Framebuffer); }
	void Add(rpr_camera camera) { Adding((void*)camera, Camera); }
	void Add(rpr_scene scene) { Adding((void*)scene, Scene); }
	void Add(rpr_composite composite) { Adding((void*)composite, Composite); }
	void Add(rpr_post_effect postEffect) { Adding((void*)postEffect, PostEffect); }

	void Clean()
	{
		for (const auto& Fst : m_NodesCollector_ | std::views::keys)
		{
			//std::cout << "Cleaning " << Fst << '\n';
			CHECK(rprObjectDelete(Fst));
		}
		m_NodesCollector_.clear();
	}


private:

	HorusGarbageCollector() = default;

	std::vector<std::pair<void*, ObjectType>> m_NodesCollector_;

};