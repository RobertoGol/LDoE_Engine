#pragma once
#include <vector>
#include <memory>
#include <SDL3/SDL.h>
#include "../ECS.hpp"

class PlayerMovementSystem {
public:
    void Update(const bool* keyboardState,
                float deltaTime,
                std::vector<std::unique_ptr<Entity>>& entities);
};
