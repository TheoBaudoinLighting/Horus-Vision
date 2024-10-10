#include "hrs_ui_testing.h"
#include <vector>
#include <string>
#include "hrs_scene_hierarchy.h"
#include <algorithm>
#include <cctype>
#include <functional>

void HorusUITesting::RenderTestUI()
{
    static bool initialized = false;
    if (!initialized) {
        // Initialisation de la scène
        SceneNode* geometryGroup = new SceneNode("Geometry", true);
        SceneNode* cube = new SceneNode("Cube");
        SceneNode* sphere = new SceneNode("Sphere");
        geometryGroup->AddChild(cube);
        geometryGroup->AddChild(sphere);
        m_scene.AddNode(geometryGroup);

        SceneNode* lightsGroup = new SceneNode("Lights", true);
        SceneNode* pointLight = new SceneNode("Point Light");
        SceneNode* directionalLight = new SceneNode("Directional Light");
        lightsGroup->AddChild(pointLight);
        lightsGroup->AddChild(directionalLight);
        m_scene.AddNode(lightsGroup);

        SceneNode* camerasGroup = new SceneNode("Cameras", true);
        SceneNode* mainCamera = new SceneNode("Main Camera");
        camerasGroup->AddChild(mainCamera);
        m_scene.AddNode(camerasGroup);

        initialized = true;
    }

    ImGui::Begin("OutlinerTest", nullptr, ImGuiWindowFlags_MenuBar);

    // Menu bar
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Display"))
        {
            ImGui::MenuItem("Show hidden objects", nullptr, &m_ShowHiddenObjects);
            ImGui::MenuItem("Sort by name", nullptr, &m_SortByName);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    // Search bar
    static char searchBuffer[256] = "";
    if (ImGui::InputText("Search", searchBuffer, IM_ARRAYSIZE(searchBuffer)))
    {
        UpdateSearch(searchBuffer);
    }

    // Object tree list
    if (ImGui::BeginChild("ObjectTree", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())))
    {
        RenderHierarchy(m_scene.nodes);
    }
    ImGui::EndChild();

    // Bottom toolbar
    if (ImGui::Button("Add"))
    {
        AddNewObject();
    }
    ImGui::SameLine();
    if (ImGui::Button("Delete"))
    {
        DeleteSelectedObject();
    }

    RenderRenamePopup();

    ImGui::End();

    // Separate window to display selected object details
    ImGui::Begin("Object Details");
    if (!m_SelectedObjects.empty())
    {
        for (const auto& obj : m_SelectedObjects)
        {
            ImGui::Text("Selected object: %s", obj.c_str());
            // Add more details about the selected object here
        }
    }
    else
    {
        ImGui::Text("No object selected");
    }
    ImGui::End();

    // Vérifier l'état des touches Shift et Ctrl
    m_IsShiftPressed = ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift);
    m_IsCtrlPressed = ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl);
}

void HorusUITesting::RenderHierarchy(const std::vector<SceneNode*>& nodes)
{
    for (SceneNode* node : nodes)
    {
        if (!m_ShowHiddenObjects && node->name[0] == '.') continue;

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
        if (!node->isGroup)
        {
            flags |= ImGuiTreeNodeFlags_Leaf;
        }
        if (std::find(m_SelectedObjects.begin(), m_SelectedObjects.end(), node->name) != m_SelectedObjects.end())
        {
            flags |= ImGuiTreeNodeFlags_Selected;
        }

        bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)node, flags, "%s", node->name.c_str());

        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        {
            HandleSelection(node->name);
        }

        // Détection du double-clic
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
        {
            StartRenaming(node);
        }

        // Détection de la touche F2
        if (ImGui::IsItemFocused() && ImGui::IsKeyPressed(ImGuiKey_F2))
        {
            StartRenaming(node);
        }

        if (ImGui::BeginDragDropSource())
        {
            ImGui::SetDragDropPayload("SCENE_NODE", &node, sizeof(SceneNode*));
            ImGui::Text("Move %s", node->name.c_str());
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_NODE"))
            {
                SceneNode* droppedNode = *(SceneNode**)payload->Data;
                if (node->isGroup && droppedNode != node)
                {
                    // Remove the node from its current parent
                    if (droppedNode->parent)
                    {
                        auto& siblings = droppedNode->parent->children;
                        siblings.erase(std::remove(siblings.begin(), siblings.end(), droppedNode), siblings.end());
                    }
                    else
                    {
                        m_scene.nodes.erase(std::remove(m_scene.nodes.begin(), m_scene.nodes.end(), droppedNode), m_scene.nodes.end());
                    }

                    // Add the node to the new parent
                    node->children.push_back(droppedNode);
                    droppedNode->parent = node;
                }
            }
            ImGui::EndDragDropTarget();
        }

        HandleContextMenu(node->name.c_str());

        if (nodeOpen)
        {
            if (node->isGroup)
            {
                RenderHierarchy(node->children);
            }
            ImGui::TreePop();
        }
    }
}

void HorusUITesting::StartRenaming(SceneNode* node)
{
    m_NodeToRename = node;
    strncpy(m_RenameBuffer, node->name.c_str(), sizeof(m_RenameBuffer) - 1);
    m_RenameBuffer[sizeof(m_RenameBuffer) - 1] = '\0';
    ImGui::OpenPopup("Rename Node");
}

void HorusUITesting::HandleContextMenu(const char* object)
{
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Rename"))
        {
            RenameSelectedObject();
        }
        if (ImGui::MenuItem("Delete"))
        {
            DeleteSelectedObject();
        }
        ImGui::EndPopup();
    }
}

void HorusUITesting::AddNewObject()
{
    static int newObjectCounter = 0;
    std::string newObjectName = "NewObject" + std::to_string(++newObjectCounter);

    ImGui::OpenPopup("Add New Object");

    if (ImGui::BeginPopupModal("Add New Object", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        static char nameBuffer[256] = "";
        ImGui::InputText("Object Name", nameBuffer, IM_ARRAYSIZE(nameBuffer));

        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            if (strlen(nameBuffer) > 0)
            {
                newObjectName = nameBuffer;
            }

            SceneNode* newNode = new SceneNode(newObjectName);

            if (!m_SelectedObjects.empty())
            {
                SceneNode* parentNode = m_scene.FindNode(m_SelectedObjects.back().c_str());
                if (parentNode && parentNode->isGroup)
                {
                    parentNode->AddChild(newNode);
                }
                else
                {
                    m_scene.AddNode(newNode);
                }
            }
            else
            {
                m_scene.AddNode(newNode);
            }

            ImGui::CloseCurrentPopup();
            memset(nameBuffer, 0, sizeof(nameBuffer));
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
            memset(nameBuffer, 0, sizeof(nameBuffer));
        }

        ImGui::EndPopup();
    }
}

void HorusUITesting::DeleteSelectedObject()
{
    if (!m_SelectedObjects.empty())
    {
        for (const auto& obj : m_SelectedObjects)
        {
            SceneNode* nodeToDelete = m_scene.FindNode(obj);
            if (nodeToDelete)
            {
                if (nodeToDelete->parent)
                {
                    nodeToDelete->parent->RemoveChild(nodeToDelete);
                }
                else
                {
                    m_scene.RemoveNode(nodeToDelete);
                }
            }
        }
        m_SelectedObjects.clear();
        
        ImGui::OpenPopup("Confirmation de suppression");
    }
    
    if (ImGui::BeginPopupModal("Confirmation de suppression", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("L'objet a été supprimé avec succès.");
        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void HorusUITesting::RenameSelectedObject()
{
    if (!m_SelectedObjects.empty())
    {
        SceneNode* nodeToRename = m_scene.FindNode(m_SelectedObjects.back());
        if (nodeToRename)
        {
            static char buffer[256];
            strncpy(buffer, nodeToRename->name.c_str(), sizeof(buffer) - 1);
            buffer[sizeof(buffer) - 1] = '\0';
            ImGui::OpenPopup("Rename Object");
        }
    }

    if (ImGui::BeginPopupModal("Rename Object", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        static char buffer[256];
        ImGui::InputText("New Name", buffer, IM_ARRAYSIZE(buffer));
        
        ImGui::Separator();
        
        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            SceneNode* nodeToRename = m_scene.FindNode(m_SelectedObjects.back());
            if (nodeToRename)
            {
                nodeToRename->name = buffer;
                m_SelectedObjects.clear();
                m_SelectedObjects.push_back(nodeToRename->name);
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void HorusUITesting::UpdateSearch(const char* searchString)
{
    // Implement search functionality here
}

void HorusUITesting::SortHierarchy()
{
    if (m_SortByName)
    {
        std::sort(m_scene.nodes.begin(), m_scene.nodes.end(), 
            [](const SceneNode* a, const SceneNode* b) {
                return std::lexicographical_compare(
                    a->name.begin(), a->name.end(),
                    b->name.begin(), b->name.end(),
                    [](char a, char b) {
                        return std::tolower(a) < std::tolower(b);
                    }
                );
            }
        );
    }
}

void HorusUITesting::RenderRenamePopup()
{
    if (ImGui::BeginPopupModal("Rename Node", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::SetKeyboardFocusHere(0);
        if (ImGui::InputText("New Name", m_RenameBuffer, IM_ARRAYSIZE(m_RenameBuffer), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            ApplyRename();
        }
        
        if (ImGui::Button("OK", ImVec2(120, 0)) || ImGui::IsKeyPressed(ImGuiKey_Enter))
        {
            ApplyRename();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)) || ImGui::IsKeyPressed(ImGuiKey_Escape))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void HorusUITesting::ApplyRename()
{
    if (m_NodeToRename)
    {
        m_NodeToRename->name = m_RenameBuffer;
        m_SelectedObjects.clear();
        m_SelectedObjects.push_back(m_NodeToRename->name);
    }
    ImGui::CloseCurrentPopup();
}

void HorusUITesting::HandleSelection(const std::string& nodeName)
{
    if (m_IsShiftPressed)
    {
        if (m_SelectedObjects.empty())
        {
            m_SelectedObjects.push_back(nodeName);
            m_LastSelectedNode = nodeName;
        }
        else
        {
            std::vector<std::string> nodesBetween = GetNodesBetween(m_LastSelectedNode, nodeName);
            for (const auto& node : nodesBetween)
            {
                if (std::find(m_SelectedObjects.begin(), m_SelectedObjects.end(), node) == m_SelectedObjects.end())
                {
                    m_SelectedObjects.push_back(node);
                }
            }
            m_LastSelectedNode = nodeName;
        }
    }
    else if (m_IsCtrlPressed)
    {
        auto it = std::find(m_SelectedObjects.begin(), m_SelectedObjects.end(), nodeName);
        if (it == m_SelectedObjects.end())
        {
            m_SelectedObjects.push_back(nodeName);
        }
        else
        {
            m_SelectedObjects.erase(it);
        }
        m_LastSelectedNode = nodeName;
    }
    else
    {
        m_SelectedObjects.clear();
        m_SelectedObjects.push_back(nodeName);
        m_LastSelectedNode = nodeName;
    }
}

std::vector<std::string> HorusUITesting::GetNodesBetween(const std::string& start, const std::string& end)
{
    std::vector<std::string> result;
    bool startFound = false;
    bool endFound = false;

    std::function<void(const std::vector<SceneNode*>&)> traverseNodes = [&](const std::vector<SceneNode*>& nodes) {
        for (const auto& node : nodes)
        {
            if (node->name == start || node->name == end)
            {
                if (!startFound && !endFound)
                {
                    startFound = true;
                }
                else
                {
                    endFound = true;
                }
            }

            if (startFound)
            {
                result.push_back(node->name);
            }

            if (endFound)
            {
                return;
            }

            if (node->isGroup)
            {
                traverseNodes(node->children);
            }

            if (endFound)
            {
                return;
            }
        }
    };

    traverseNodes(m_scene.nodes);

    if (result.front() != start)
    {
        std::reverse(result.begin(), result.end());
    }

    return result;
}