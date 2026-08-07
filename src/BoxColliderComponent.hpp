#pragma once
#include "ECS.hpp"
#include <nlohmann/json.hpp>

class BoxColliderComponent : public Component {
public:
    float width = 32.0f;
    float height = 32.0f;
    float offsetX = 0.0f;
    float offsetY = 0.0f;
    bool isTrigger = false;

    BoxColliderComponent() = default;
    BoxColliderComponent(float w, float h, float ox = 0.0f, float oy = 0.0f)
        : width(w), height(h), offsetX(ox), offsetY(oy) {}

    nlohmann::json Serialize() override {
        return {
            {"type", "BoxColliderComponent"},
            {"width", width},
            {"height", height},
            {"offsetX", offsetX},
            {"offsetY", offsetY},
            {"isTrigger", isTrigger}
        };
    }

    void Deserialize(const nlohmann::json& j) override {
        width = j.value("width", 32.0f);
        height = j.value("height", 32.0f);
        offsetX = j.value("offsetX", 0.0f);
        offsetY = j.value("offsetY", 0.0f);
        isTrigger = j.value("isTrigger", false);
    }
};
