#pragma once
#include "../ECS.hpp"
#include "../Math.hpp"

class TransformComponent : public Component {
public:
    Vec2 position { 0.0f, 0.0f };
    Vec2 scale { 1.0f, 1.0f };

    json Serialize() override {
        return {
            {"type", "TransformComponent"},
            {"worldX", position.x},
            {"worldY", position.y},
            {"scaleX", scale.x},
            {"scaleY", scale.y}
        };
    }

    void Deserialize(const json& j) override {
        position.x = j.value("worldX", j.value("x", 0.0f));
        position.y = j.value("worldY", j.value("y", 0.0f));
        scale.x = j.value("scaleX", 1.0f);
        scale.y = j.value("scaleY", 1.0f);
    }
};
