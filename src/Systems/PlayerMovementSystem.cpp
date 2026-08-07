#include "PlayerMovementSystem.hpp"
#include "../Components/PlayerInputComponent.hpp"
#include "../Components/TransformComponent.hpp"
#include "../Components/SpriteRendererComponent.hpp"
#include "../Components/ColliderComponent.hpp"
#include <algorithm>

namespace {
    constexpr float MIN_BOUND = -0.5f;
    constexpr float MAX_BOUND = 50.5f;

    struct AABB {
        float MinX = 0.0f;
        float MinY = 0.0f;
        float MaxX = 0.0f;
        float MaxY = 0.0f;
    };

    AABB BuildAABB(float centerX, float centerY, const ColliderComponent& collider) {
        const float halfWidth = collider.width * 0.5f;
        const float halfHeight = collider.height * 0.5f;
        return {
            centerX - halfWidth,
            centerY - halfHeight,
            centerX + halfWidth,
            centerY + halfHeight
        };
    }

    bool Intersects(const AABB& a, const AABB& b) {
        return a.MinX < b.MaxX &&
               a.MaxX > b.MinX &&
               a.MinY < b.MaxY &&
               a.MaxY > b.MinY;
    }

    bool WouldCollide(Entity* self,
                    const std::vector<std::unique_ptr<Entity>>& entities,
                    float centerX,
                    float centerY) {
        const ColliderComponent* selfCollider = self->GetComponent<ColliderComponent>();
        if (!selfCollider) {
            return false;
        }

        const AABB selfBox = BuildAABB(centerX, centerY, *selfCollider);

        for (const auto& other : entities) {
            if (other.get() == self) {
                continue;
            }

            const ColliderComponent* otherCollider = other->GetComponent<ColliderComponent>();
            const TransformComponent* otherTransform = other->GetComponent<TransformComponent>();
            if (!otherCollider || !otherCollider->isSolid || !otherTransform) {
                continue;
            }

            const AABB otherBox = BuildAABB(
                otherTransform->position.x,
                otherTransform->position.y,
                *otherCollider
            );

            if (Intersects(selfBox, otherBox)) {
                return true;
            }
        }

        return false;
    }
}

void PlayerMovementSystem::Update(const bool* keyboardState,
                                  float deltaTime,
                                  std::vector<std::unique_ptr<Entity>>& entities) {
    if (!keyboardState) {
        return;
    }

    constexpr float moveSpeed = 200.0f;

    for (auto& entity : entities) {
        if (!entity->HasComponent<PlayerInputComponent>() ||
            !entity->HasComponent<TransformComponent>()) {
            continue;
        }

        TransformComponent* transform = entity->GetComponent<TransformComponent>();
        SpriteRendererComponent* sprite = entity->GetComponent<SpriteRendererComponent>();

        const float previousX = transform->position.x;
        const float previousY = transform->position.y;

        float moveX = 0.0f;
        float moveY = 0.0f;

        if (keyboardState[SDL_SCANCODE_W]) {
            moveY -= moveSpeed * deltaTime;
        }
        if (keyboardState[SDL_SCANCODE_S]) {
            moveY += moveSpeed * deltaTime;
        }
        if (keyboardState[SDL_SCANCODE_A]) {
            moveX -= moveSpeed * deltaTime;
        }
        if (keyboardState[SDL_SCANCODE_D]) {
            moveX += moveSpeed * deltaTime;
        }

        const float targetX = previousX + moveX;
        const float targetY = previousY + moveY;

        if (moveX != 0.0f && !WouldCollide(entity.get(), entities, targetX, previousY)) {
            transform->position.x = targetX;
        }

        if (moveY != 0.0f && !WouldCollide(entity.get(), entities, transform->position.x, targetY)) {
            transform->position.y = targetY;
        }

        transform->position.x = std::clamp(transform->position.x, MIN_BOUND, MAX_BOUND);
        transform->position.y = std::clamp(transform->position.y, MIN_BOUND, MAX_BOUND);

        if (!sprite) {
            continue;
        }

        const float deltaX = transform->position.x - previousX;
        const float deltaY = transform->position.y - previousY;
        const bool isMoving = (deltaX != 0.0f) || (deltaY != 0.0f);

        if (isMoving) {
            sprite->Animation.IsPlaying = true;

            if (deltaX < 0.0f) {
                sprite->FlipX = true;
            } else if (deltaX > 0.0f) {
                sprite->FlipX = false;
            }
        } else {
            sprite->Animation.IsPlaying = false;
            sprite->Animation.CurrentFrame = 0;
            sprite->Animation.CurrentFrameTime = 0.0f;
        }
    }
}
