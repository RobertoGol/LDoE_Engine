#include "AnimationSystem.hpp"
#include "../Components/SpriteRendererComponent.hpp"

void AnimationSystem::Update(float deltaTime, std::vector<std::unique_ptr<Entity>>& entities) {
    for (auto& entity : entities) {
        if (!entity->HasComponent<SpriteRendererComponent>()) {
            continue;
        }

        SpriteRendererComponent* sprite = entity->GetComponent<SpriteRendererComponent>();
        AnimationData& animation = sprite->Animation;

        if (!animation.IsActive() || !animation.IsPlaying) {
            continue;
        }

        animation.CurrentFrameTime += deltaTime;

        while (animation.CurrentFrameTime >= animation.FrameTime) {
            animation.CurrentFrameTime -= animation.FrameTime;
            animation.CurrentFrame = (animation.CurrentFrame + 1) % animation.TotalFrames;
        }
    }
}
