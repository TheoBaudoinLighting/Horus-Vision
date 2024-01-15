#pragma once

#include <string>
#include <vector>
#include <map>

class HorusOutlinerObject
{
public:

	std::string name;
	std::string type;
	std::vector<HorusOutlinerObject*> children;

	HorusOutlinerObject(std::string name, std::string type) : name(name), type(type) {}

	~HorusOutlinerObject()
	{
		for (auto& child : children)
		{
			delete child;
		}
	}

	void AddChild(HorusOutlinerObject* child)
	{
		children.push_back(child);
	}

	void RemoveChild(HorusOutlinerObject* child)
	{
		children.erase(std::remove(children.begin(), children.end(), child), children.end());
	}


private:



};

class HorusOutliner
{
public:

	static HorusOutliner& get_instance()
	{
		static HorusOutliner instance;
		return instance;
	}

	HorusOutliner(HorusOutliner const&) = delete;
	void operator=(HorusOutliner const&) = delete;

	void Outliner(bool* p_open);

	void AddRootObject(HorusOutlinerObject* object)
	{
		rootObjects.push_back(object);
	}

	void RemoveRootObject(HorusOutlinerObject* object)
	{
		rootObjects.erase(std::remove(rootObjects.begin(), rootObjects.end(), object), rootObjects.end());
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
		selectedObject = object;
	}

	std::string GetSelectedObject()
	{
		return selectedObject;
	}

	void MoveObject(HorusOutlinerObject* object, HorusOutlinerObject* newParent);



private:

	HorusOutliner() {}
	~HorusOutliner()
	{
		for (auto& object : rootObjects)
		{
			delete object;
		}
	}

	std::vector<HorusOutlinerObject*> rootObjects;

	std::string selectedObject;

	std::string selectedMesh;
	std::string selectedMaterial;
	std::string selectedCamera;
	std::string selectedLight;
	std::map<std::string, int> meshNameToID;


};
