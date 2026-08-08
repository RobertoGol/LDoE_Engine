#pragma once
#include "Math.hpp"
#include <SDL3/SDL.h>
#include <glm/glm.hpp>

class PlayerController {
public:
    Vec2 position { 0.0f, 0.0f };
    float walkTimer = 0.0f;
    float currentBobOffset = 0.0f; // Эффект покачивания при ходьбе

    void Update(float deltaTime);
    glm::mat4 GetModelMatrix() const;
};