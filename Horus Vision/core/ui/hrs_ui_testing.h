#pragma once

#include "hrs_scene_hierarchy.h"
#include "imgui.h"
#include <vector>
#include <string>

class HorusUITesting
{
public:
    static HorusUITesting& GetInstance()
    {
        static HorusUITesting instance;
        return instance;
    }

    void RenderTestUI();

private:
    SceneHierarchy m_scene;
    HorusUITesting() = default;
    ~HorusUITesting() = default;

    HorusUITesting(const HorusUITesting&) = delete;
    HorusUITesting& operator=(const HorusUITesting&) = delete;

    bool m_ShowHiddenObjects = false;
    bool m_SortByName = false;
    std::vector<std::string> m_SelectedObjects;
    bool m_IsShiftPressed = false;
    bool m_IsCtrlPressed = false;
    std::string m_LastSelectedNode;  // Ajouté pour garder une trace du dernier nœud sélectionné

    void HandleContextMenu(const char* object);
    void RenderHierarchy(const std::vector<SceneNode*>& nodes);
    
    void AddNewObject();
    void DeleteSelectedObject();
    void RenameSelectedObject();
    
    void UpdateSearch(const char* searchString);
    void SortHierarchy();
    
    SceneNode* m_NodeToRename = nullptr;
    char m_RenameBuffer[256];

    void StartRenaming(SceneNode* node);
    void RenderRenamePopup();
    
    void ApplyRename();

    void HandleSelection(const std::string& nodeName);
    std::vector<std::string> GetNodesBetween(const std::string& start, const std::string& end);  // Nouvelle méthode
};
