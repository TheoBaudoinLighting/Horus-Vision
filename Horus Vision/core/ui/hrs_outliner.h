#pragma once

#include <string>
#include <vector>
#include <map>

class HorusOutlinerObject
{
public:

	std::string Name;
	std::string Type;
	std::vector<HorusOutlinerObject*> Children;

	HorusOutlinerObject(std::string name, std::string type) : Name(name), Type(type) {}

	~HorusOutlinerObject()
	{
		for (auto& Child : Children)
		{
			delete Child;
		}
	}

	void AddChild(HorusOutlinerObject* child)
	{
		Children.push_back(child);
	}

	void RemoveChild(HorusOutlinerObject* child)
	{
		std::erase(Children, child);
	}
};

class HorusOutliner
{
public:

	static HorusOutliner& GetInstance()
	{
		static HorusOutliner Instance;
		return Instance;
	}

	HorusOutliner(HorusOutliner const&) = delete;
	void operator=(HorusOutliner const&) = delete;

	void Outliner(bool* p_open);

	void AddRootObject(HorusOutlinerObject* object)
	{
		m_RootObjects_.push_back(object);
	}

	void RemoveRootObject(HorusOutlinerObject* object)
	{
		std::erase(m_RootObjects_, object);
	}

	void AddChildToObject(HorusOutlinerObject* parent, HorusOutlinerObject* child)
	{
		parent->AddChild(child);
	}

	void RemoveChildFromObject(HorusOutlinerObject* parent, HorusOutlinerObject* child)
	{
		parent->RemoveChild(child);
	}

	void SetSelectedObject(std::string object)
	{
		m_SelectedObject_ = object;
	}

	std::string GetSelectedObject()
	{
		return m_SelectedObject_;
	}

	void MoveObject(HorusOutlinerObject* object, HorusOutlinerObject* newParent);

private:

	HorusOutliner() {}
	~HorusOutliner()
	{
		for (auto& Object : m_RootObjects_)
		{
			delete Object;
		}
	}

	std::vector<HorusOutlinerObject*> m_RootObjects_;

	std::string m_SelectedObject_;

	std::string m_SelectedMesh_;
	std::string m_SelectedMaterial_;
	std::string m_SelectedCamera_;
	std::string m_SelectedLight_;
	std::map<std::string, int> m_MeshNameToId_;
};
