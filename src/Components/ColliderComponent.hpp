#pragma once
#include "../ECS.hpp"

class ColliderComponent : public Component {
public:
    float width = 0.5f;
    float height = 0.5f;
    bool isSolid = true;

    json Serialize() override {
        return {
            {"type", "ColliderComponent"},
            {"width", width},
            {"height", height},
            {"isSolid", isSolid}
        };
    }

    void Deserialize(const json& j) override {
        width = j.value("width", 0.5f);
        height = j.value("height", 0.5f);
        isSolid = j.value("isSolid", true);
    }
};
