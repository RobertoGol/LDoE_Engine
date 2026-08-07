#pragma once
#include <vector>
#include <memory>
#include "../ECS.hpp"

class AnimationSystem {
public:
    void Update(float deltaTime, std::vector<std::unique_ptr<Entity>>& entities);
};
