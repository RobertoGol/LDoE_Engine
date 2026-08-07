#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <nlohmann/json.hpp>
#include <SDL3/SDL.h>

using json = nlohmann::json;

class Component {
public:
    virtual ~Component() = default;
    virtual void Update(float deltaTime) {}
    virtual json Serialize() = 0;
    virtual void Deserialize(const json& j) = 0;
};

class TilemapComponent : public Component {
public:
    int width = 10;
    int height = 10;
    std::vector<int> tiles;

    TilemapComponent(int w = 10, int h = 10) : width(w), height(h), tiles(w * h, 1) {}

    json Serialize() override {
        return {
            {"type", "TilemapComponent"},
            {"width", width},
            {"height", height},
            {"tiles", tiles}
        };
    }

    void Deserialize(const json& j) override {
        width = j.value("width", 10);
        height = j.value("height", 10);
        if (j.contains("tiles") && j["tiles"].is_array()) {
            tiles = j["tiles"].get<std::vector<int>>();
        }
    }
};

class Entity {
public:
    std::string name;
    std::unordered_map<std::type_index, std::unique_ptr<Component>> components;

    Entity(std::string n = "New Entity") : name(n) {}

    template <typename T, typename... Args>
    T* AddComponent(Args&&... args) {
        auto comp = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = comp.get();
        components[typeid(T)] = std::move(comp);
        return ptr;
    }

    template <typename T>
    bool HasComponent() const {
        return components.find(typeid(T)) != components.end();
    }

    template <typename T>
    T* GetComponent() {
        auto it = components.find(typeid(T));
        if (it != components.end()) {
            return static_cast<T*>(it->second.get());
        }
        return nullptr;
    }

    template <typename T>
    const T* GetComponent() const {
        auto it = components.find(typeid(T));
        if (it != components.end()) {
            return static_cast<const T*>(it->second.get());
        }
        return nullptr;
    }

    void Update(float deltaTime) {
        for (auto& [type, comp] : components) {
            comp->Update(deltaTime);
        }
    }

    json Serialize();
    void Deserialize(const json& j);
};
