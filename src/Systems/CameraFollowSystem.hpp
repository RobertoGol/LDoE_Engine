#pragma once
#include <vector>
#include <memory>
#include "../ECS.hpp"

class CameraFollowSystem {
public:
    struct CameraState {
        float OffsetX = 0.0f;
        float OffsetY = 0.0f;
    };

    void Update(float deltaTime,
                const std::vector<std::unique_ptr<Entity>>& entities,
                float windowWidth,
                float windowHeight,
                float tileWidth,
                float tileHeight,
                CameraState& camera);
};
