#pragma once

#include <vector>
#include <string>
#include <json.hpp> // Utilisez une bibliothèque JSON comme nlohmann/json
#include <unordered_set> // Ajouté pour résoudre l'erreur de tags non définie

struct SceneNode {
    std::string name;
    unsigned int id;  // Nouvel identifiant unique
    bool isGroup;
    std::vector<SceneNode*> children;
    SceneNode* parent;
    std::unordered_set<std::string> tags;  // Ajoutez cette ligne

    static unsigned int nextId() {
        static unsigned int id = 0;
        return id++;
    }

    SceneNode(const std::string& nodeName, bool isGroupNode = false)
        : name(nodeName), isGroup(isGroupNode), parent(nullptr), id(nextId()) {}

    ~SceneNode() {
        for (auto child : children) {
            delete child;
        }
    }

    void AddChild(SceneNode* child) {
        children.push_back(child);
        child->parent = this;
    }

    void RemoveChild(SceneNode* child) {
        auto it = std::find(children.begin(), children.end(), child);
        if (it != children.end()) {
            children.erase(it);
            child->parent = nullptr;
        }
    }

    bool IsLeaf() const {
        return children.empty();
    }

    nlohmann::json Serialize() const {
        nlohmann::json j;
        j["name"] = name;
        j["id"] = id;
        j["isGroup"] = isGroup;
        j["tags"] = tags;
        j["children"] = nlohmann::json::array();
        for (const auto& child : children) {
            j["children"].push_back(child->Serialize());
        }
        return j;
    }

    static SceneNode* Deserialize(const nlohmann::json& j) {
        SceneNode* node = new SceneNode(j["name"], j["isGroup"]);
        node->id = j["id"];
        node->tags = j["tags"].get<std::unordered_set<std::string>>();
        for (const auto& childJson : j["children"]) {
            node->AddChild(Deserialize(childJson));
        }
        return node;
    }
};

class SceneHierarchy {
public:
    std::vector<SceneNode*> nodes;

    void AddNode(SceneNode* node) {
        nodes.push_back(node);
    }

    // Destructeur pour libérer la mémoire
    ~SceneHierarchy() {
        for (auto node : nodes) {
            DeleteNode(node);
        }
    }

private:
    // Fonction récursive pour supprimer un nœud et tous ses enfants
    void DeleteNode(SceneNode* node) {
        for (auto child : node->children) {
            DeleteNode(child);
        }
        delete node;
    }

    SceneNode* FindNodeRecursive(const std::vector<SceneNode*>& nodeList, const std::string& name) {
        for (auto node : nodeList) {
            if (node->name == name) return node;
            auto result = FindNodeRecursive(node->children, name);
            if (result) return result;
        }
        return nullptr;
    }

    SceneNode* FindNodeByIdRecursive(const std::vector<SceneNode*>& nodeList, unsigned int id) {
        for (auto node : nodeList) {
            if (node->id == id) return node;
            auto result = FindNodeByIdRecursive(node->children, id);
            if (result) return result;
        }
        return nullptr;
    }

public:
    // Fonction pour trouver un nœud par son nom
    SceneNode* FindNode(const std::string& name) {
        return FindNodeRecursive(nodes, name);
    }

    // Fonction pour supprimer un nœud spécifique
    void RemoveNode(SceneNode* node) {
        auto it = std::find(nodes.begin(), nodes.end(), node);
        if (it != nodes.end()) {
            nodes.erase(it);
            DeleteNode(node);
        }
    }

    SceneNode* FindNodeById(unsigned int id) {
        return FindNodeByIdRecursive(nodes, id);
    }

    void MoveNode(SceneNode* node, SceneNode* newParent) {
        if (node->parent) {
            node->parent->RemoveChild(node);
        } else {
            auto it = std::find(nodes.begin(), nodes.end(), node);
            if (it != nodes.end()) {
                nodes.erase(it);
            }
        }
        
        if (newParent) {
            newParent->AddChild(node);
        } else {
            nodes.push_back(node);
        }
    }

    nlohmann::json Serialize() const {
        nlohmann::json j;
        j["nodes"] = nlohmann::json::array();
        for (const auto& node : nodes) {
            j["nodes"].push_back(node->Serialize());
        }
        return j;
    }

    static SceneHierarchy Deserialize(const nlohmann::json& j) {
        SceneHierarchy hierarchy;
        for (const auto& nodeJson : j["nodes"]) {
            hierarchy.AddNode(SceneNode::Deserialize(nodeJson));
        }
        return hierarchy;
    }
};
