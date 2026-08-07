#pragma once
#include <vector>
#include <memory>
#include <SDL3/SDL.h>
#include "../ECS.hpp"
#include "../Components/TransformComponent.hpp"
#include "../Components/SpriteRendererComponent.hpp"

class RenderSystem {
public:
    struct RenderContext {
        float CameraOffsetX = 0.0f;
        float CameraOffsetY = 0.0f;
        float TileWidth = 128.0f;
        float TileHeight = 64.0f;
    };

    struct RenderableEntity {
        TransformComponent* Transform;
        SpriteRendererComponent* Sprite;
    };

    void Update(SDL_Renderer* renderer,
                const std::vector<std::unique_ptr<Entity>>& entities,
                const RenderContext& context);

private:
    static void SortRenderables(std::vector<RenderableEntity>& renderList);
    static void DrawRenderables(SDL_Renderer* renderer,
                                const std::vector<RenderableEntity>& renderList,
                                const RenderContext& context);
};
