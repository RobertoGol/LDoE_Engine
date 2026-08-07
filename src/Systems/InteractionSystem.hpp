#pragma once
#include <vector>
#include <memory>
#include <SDL3/SDL.h>
#include "../ECS.hpp"

class InteractionSystem {
public:
    void Update(const bool* keyboardState, std::vector<std::unique_ptr<Entity>>& entities);
};