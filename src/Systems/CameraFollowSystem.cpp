#include "CameraFollowSystem.hpp"
#include "../Components/PlayerInputComponent.hpp"
#include "../Components/TransformComponent.hpp"
#include "../Math.hpp"

void CameraFollowSystem::Update(float deltaTime,
                                const std::vector<std::unique_ptr<Entity>>& entities,
                                float windowWidth,
                                float windowHeight,
                                float tileWidth,
                                float tileHeight,
                                CameraState& camera) {
    for (const auto& entity : entities) {
        if (!entity->HasComponent<PlayerInputComponent>() ||
            !entity->HasComponent<TransformComponent>()) {
            continue;
        }

        const TransformComponent* transform = entity->GetComponent<TransformComponent>();
        const Vec2 screenPos = IsoMath::WorldToScreen(
            transform->position.x,
            transform->position.y,
            tileWidth,
            tileHeight
        );

        const float targetOffsetX = (windowWidth * 0.5f) - screenPos.x;
        const float targetOffsetY = (windowHeight * 0.5f) - screenPos.y;

        constexpr float followSpeed = 5.0f;
        const float lerpFactor = followSpeed * deltaTime;

        camera.OffsetX += (targetOffsetX - camera.OffsetX) * lerpFactor;
        camera.OffsetY += (targetOffsetY - camera.OffsetY) * lerpFactor;

        return;
    }
}
