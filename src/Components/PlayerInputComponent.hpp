#pragma once
#include "../ECS.hpp"

class PlayerInputComponent : public Component {
public:
    json Serialize() override {
        return { {"type", "PlayerInputComponent"} };
    }

    void Deserialize(const json& j) override {
        (void)j;
    }
};
