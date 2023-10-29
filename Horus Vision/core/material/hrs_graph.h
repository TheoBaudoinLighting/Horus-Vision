#pragma once

#include <algorithm>
#include <cassert>
#include <iterator>
#include <stack>
#include <stddef.h>
#include <utility>
#include <vector>

#include <spdlog/spdlog.h>

#include "hrs_material_editor.h"
#include "hrs_material_editor.h"
#include "hrs_material_editor.h"
#include "hrs_material_editor.h"


template<typename HorusElementType>
struct HorusSpan
{
	using iterator = HorusElementType*;

	template<typename HorusContainer>
	HorusSpan(HorusContainer& c) : begin_(c.data()), end_(begin_ + c.size())
	{
	}

	iterator begin() const { return begin_; }
	iterator end() const { return end_; }

private:
	iterator begin_;
	iterator end_;
};

template<typename HorusElementType>
class HorusIdMap
{
public:
	using iterator = typename std::vector<HorusElementType>::iterator;
	using const_iterator = typename std::vector<HorusElementType>::const_iterator;

	// Iterators

	const_iterator begin() const { return m_elements_.begin(); }
	const_iterator end() const { return m_elements_.end(); }

	// Element access

	HorusSpan<const HorusElementType> elements() const { return m_elements_; }

	// Capacity

	bool   empty() const { return m_sorted_indices_.empty(); }
	size_t size() const { return m_sorted_indices_.size(); }

	// Modifiers

	std::pair<iterator, bool> insert(int id, const HorusElementType& element);
	std::pair<iterator, bool> insert(int id, HorusElementType&& element);
	size_t                    erase(int id);
	void                      clear();

	// Lookup

	iterator       find(int id);
	const_iterator find(int id) const;
	bool           contains(int id) const;

private:
	std::vector<HorusElementType> m_elements_;
	std::vector<int>         m_sorted_indices_;
};

template<typename HorusElementType>
std::pair<typename HorusIdMap<HorusElementType>::iterator, bool> HorusIdMap<HorusElementType>::insert(
	const int          id,
	const HorusElementType& element)
{
	auto lower_bound = std::lower_bound(m_sorted_indices_.begin(), m_sorted_indices_.end(), id);

	if (lower_bound != m_sorted_indices_.end() && id == *lower_bound)
	{
		return std::make_pair(
			std::next(m_elements_.begin(), std::distance(m_sorted_indices_.begin(), lower_bound)), false);
	}

	auto insert_element_at =
		std::next(m_elements_.begin(), std::distance(m_sorted_indices_.begin(), lower_bound));

	m_sorted_indices_.insert(lower_bound, id);
	return std::make_pair(m_elements_.insert(insert_element_at, element), true);
}

template<typename HorusElementType>
std::pair<typename HorusIdMap<HorusElementType>::iterator, bool> HorusIdMap<HorusElementType>::insert(
	const int     id,
	HorusElementType&& element)
{
	auto lower_bound = std::lower_bound(m_sorted_indices_.begin(), m_sorted_indices_.end(), id);

	if (lower_bound != m_sorted_indices_.end() && id == *lower_bound)
	{
		return std::make_pair(
			std::next(m_elements_.begin(), std::distance(m_sorted_indices_.begin(), lower_bound)), false);
	}

	auto insert_element_at =
		std::next(m_elements_.begin(), std::distance(m_sorted_indices_.begin(), lower_bound));

	m_sorted_indices_.insert(lower_bound, id);

	return std::make_pair(m_elements_.insert(insert_element_at, std::move(element)), true);
}

template<typename HorusElementType>
size_t HorusIdMap<HorusElementType>::erase(const int id)
{
	auto lower_bound = std::lower_bound(m_sorted_indices_.begin(), m_sorted_indices_.end(), id);

	if (lower_bound == m_sorted_indices_.end() || id != *lower_bound)
	{
		return 0ull;
	}

	auto erase_element_at =
		std::next(m_elements_.begin(), std::distance(m_sorted_indices_.begin(), lower_bound));

	m_sorted_indices_.erase(lower_bound);
	m_elements_.erase(erase_element_at);

	return 1ull;
}

template<typename HorusElementType>
void HorusIdMap<HorusElementType>::clear()
{
	m_elements_.clear();
	m_sorted_indices_.clear();
}

template<typename HorusElementType>
typename HorusIdMap<HorusElementType>::iterator HorusIdMap<HorusElementType>::find(const int id)
{
	const auto lower_bound = std::lower_bound(m_sorted_indices_.cbegin(), m_sorted_indices_.cend(), id);
	return (lower_bound == m_sorted_indices_.cend() || *lower_bound != id)
		? m_elements_.end()
		: std::next(m_elements_.begin(), std::distance(m_sorted_indices_.cbegin(), lower_bound));
}

template<typename HorusElementType>
typename HorusIdMap<HorusElementType>::const_iterator HorusIdMap<HorusElementType>::find(const int id) const
{
	const auto lower_bound = std::lower_bound(m_sorted_indices_.cbegin(), m_sorted_indices_.cend(), id);
	return (lower_bound == m_sorted_indices_.cend() || *lower_bound != id)
		? m_elements_.cend()
		: std::next(m_elements_.cbegin(), std::distance(m_sorted_indices_.cbegin(), lower_bound));
}

template<typename HorusElementType>
bool HorusIdMap<HorusElementType>::contains(const int id) const
{
	const auto lower_bound = std::lower_bound(m_sorted_indices_.cbegin(), m_sorted_indices_.cend(), id);

	if (lower_bound == m_sorted_indices_.cend())
	{
		return false;
	}

	return *lower_bound == id;
}

// a very simple directional graph
template<typename HorusNodeType>
class HorusGraph
{
public:
	HorusGraph() : current_id_(0), nodes_(), edges_from_node_(), node_neighbors_(), edges_() {}

	struct Edge
	{
		int id;
		int from, to;

		Edge() = default;
		Edge(const int id, const int f, const int t) : id(id), from(f), to(t) {}

		inline int  opposite(const int n) const { return n == from ? to : from; }
		inline bool contains(const int n) const { return n == from || n == to; }
	};

	// Element access

	HorusNodeType& node(int node_id);
	const HorusNodeType& node(int node_id) const;
	HorusSpan<const int>  neighbors(int node_id) const;
	HorusSpan<const Edge> edges() const;

	// Capacity

	size_t num_edges_from_node(int node_id) const;

	// Modifiers

	int  insert_node(const HorusNodeType& node);
	void erase_node(int node_id);

	int  insert_edge(int from, int to);
	void erase_edge(int edge_id);

private:
	int current_id_;
	// These contains map to the node id
	HorusIdMap<HorusNodeType>         nodes_;
	HorusIdMap<int>              edges_from_node_;
	HorusIdMap<std::vector<int>> node_neighbors_;

	// This container maps to the edge id
	HorusIdMap<Edge> edges_;
};

template<typename HorusNodeType>
HorusNodeType& HorusGraph<HorusNodeType>::node(const int id)
{
	return const_cast<HorusNodeType&>(static_cast<const HorusGraph*>(this)->node(id));
}

template<typename HorusNodeType>
const HorusNodeType& HorusGraph<HorusNodeType>::node(const int id) const
{
	const auto iter = nodes_.find(id);
	assert(iter != nodes_.end());

	return *iter;
}

template<typename HorusNodeType>
HorusSpan<const int> HorusGraph<HorusNodeType>::neighbors(int node_id) const
{
	const auto iter = node_neighbors_.find(node_id);
	assert(iter != node_neighbors_.end());
	return *iter;
}

template<typename HorusNodeType>
HorusSpan<const typename HorusGraph<HorusNodeType>::Edge> HorusGraph<HorusNodeType>::edges() const
{
	return edges_.elements();
}

template<typename HorusNodeType>
size_t HorusGraph<HorusNodeType>::num_edges_from_node(const int id) const
{
	auto iter = edges_from_node_.find(id);
	assert(iter != edges_from_node_.end());
	return *iter;
}

template<typename HorusNodeType>
int HorusGraph<HorusNodeType>::insert_node(const HorusNodeType& node)
{
	const int id = current_id_++;
	assert(!nodes_.contains(id));
	nodes_.insert(id, node);
	edges_from_node_.insert(id, 0);
	node_neighbors_.insert(id, std::vector<int>());
	
	return id;
}

template<typename HorusNodeType>
void HorusGraph<HorusNodeType>::erase_node(const int id)
{

	// first, remove any potential dangling edges
	{
		static std::vector<int> edges_to_erase;

		for (const Edge& edge : edges_.elements())
		{
			if (edge.contains(id))
			{
				edges_to_erase.push_back(edge.id);
			}
		}

		for (const int edge_id : edges_to_erase)
		{
			erase_edge(edge_id);
		}

		edges_to_erase.clear();
	}

	nodes_.erase(id);
	edges_from_node_.erase(id);
	node_neighbors_.erase(id);
}

template<typename HorusNodeType>
int HorusGraph<HorusNodeType>::insert_edge(const int from, const int to)
{
	const int id = current_id_++;
	assert(!edges_.contains(id));
	assert(nodes_.contains(from));
	assert(nodes_.contains(to));
	edges_.insert(id, Edge(id, from, to));

	// update neighbor count
	assert(edges_from_node_.contains(from));
	*edges_from_node_.find(from) += 1;
	// update neighbor list
	assert(node_neighbors_.contains(from));
	node_neighbors_.find(from)->push_back(to);


	return id;
}

template<typename HorusNodeType>
void HorusGraph<HorusNodeType>::erase_edge(const int edge_id)
{
	// This is a bit lazy, we find the pointer here, but we refind it when we erase the edge based
	// on id key.
	assert(edges_.contains(edge_id));
	const Edge& edge = *edges_.find(edge_id);

	// update neighbor count
	assert(edges_from_node_.contains(edge.from));
	int& edge_count = *edges_from_node_.find(edge.from);
	assert(edge_count > 0);
	edge_count -= 1;

	// update neighbor list
	{
		assert(node_neighbors_.contains(edge.from));
		auto neighbors = node_neighbors_.find(edge.from);
		auto iter = std::find(neighbors->begin(), neighbors->end(), edge.to);
		assert(iter != neighbors->end());
		neighbors->erase(iter);
	}

	edges_.erase(edge_id);
}

template<typename HorusNodeType, typename Visitor>
void dfs_traverse(const HorusGraph<HorusNodeType>& graph, const int start_node, Visitor visitor)
{
	std::stack<int> stack;

	stack.push(start_node);

	while (!stack.empty())
	{
		const int current_node = stack.top();
		stack.pop();

		visitor(current_node);

		for (const int neighbor : graph.neighbors(current_node))
		{
			stack.push(neighbor);
		}
	}
}
