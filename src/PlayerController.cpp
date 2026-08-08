#include "PlayerController.hpp"
#include <cmath>

void PlayerController::Update(float deltaTime) {
    const bool* keystate = SDL_GetKeyboardState(nullptr);
    float speed = 3.0f;
    bool isMoving = false;

    if (keystate[SDL_SCANCODE_W]) { position.y += speed * deltaTime; isMoving = true; }
    if (keystate[SDL_SCANCODE_S]) { position.y -= speed * deltaTime; isMoving = true; }
    if (keystate[SDL_SCANCODE_A]) { position.x -= speed * deltaTime; isMoving = true; }
    if (keystate[SDL_SCANCODE_D]) { position.x += speed * deltaTime; isMoving = true; }

    // Процедурная анимация покачивания (шага) при движении
    if (isMoving) {
        walkTimer += deltaTime * 15.0f;
        currentBobOffset = std::sin(walkTimer) * 0.08f; // Амплитуда шага
    } else {
        walkTimer = 0.0f;
        currentBobOffset = 0.0f;
    }
}

glm::mat4 PlayerController::GetModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    // Применяем позицию + вертикальное покачивание (эффект шагов)
    model = glm::translate(model, glm::vec3(position.x, position.y + currentBobOffset, 0.0f));
    return model;
}