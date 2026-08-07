#include "RenderSystem.hpp"
#include "../Math.hpp"
#include <algorithm>

namespace {
    void DrawPlaceholder(SDL_Renderer* renderer,
                         const TransformComponent& transform,
                         const RenderSystem::RenderContext& context) {
        const Vec2 screenPos = IsoMath::WorldToScreen(
            transform.position.x,
            transform.position.y,
            context.TileWidth,
            context.TileHeight
        );

        constexpr float placeholderSize = 32.0f;
        const float centerX = context.CameraOffsetX + screenPos.x;
        const float centerY = context.CameraOffsetY + screenPos.y;

        SDL_SetRenderDrawColor(renderer, 255, 80, 80, 255);
        const SDL_FRect placeholderRect {
            centerX - (placeholderSize * 0.5f),
            centerY - placeholderSize,
            placeholderSize,
            placeholderSize
        };
        SDL_RenderFillRect(renderer, &placeholderRect);

        SDL_SetRenderDrawColor(renderer, 255, 200, 80, 255);
        const float radius = placeholderSize * 0.25f;
        const SDL_FRect headRect {
            centerX - radius,
            centerY - placeholderSize - radius,
            radius * 2.0f,
            radius * 2.0f
        };
        SDL_RenderFillRect(renderer, &headRect);
    }
}

void RenderSystem::Update(SDL_Renderer* renderer,
                          const std::vector<std::unique_ptr<Entity>>& entities,
                          const RenderContext& context) {
    std::vector<RenderableEntity> renderList;
    renderList.reserve(entities.size());

    for (const auto& entity : entities) {
        if (!entity->HasComponent<TransformComponent>() ||
            !entity->HasComponent<SpriteRendererComponent>()) {
            continue;
        }

        auto* transform = entity->GetComponent<TransformComponent>();
        auto* sprite = entity->GetComponent<SpriteRendererComponent>();

        if (sprite->UsePlaceholder) {
            DrawPlaceholder(renderer, *transform, context);
            continue;
        }

        if (sprite->HasLayers()) {
            renderList.push_back({ transform, sprite });
        }
    }

    SortRenderables(renderList);
    DrawRenderables(renderer, renderList, context);
}

void RenderSystem::SortRenderables(std::vector<RenderableEntity>& renderList) {
    std::sort(renderList.begin(), renderList.end(), [](const RenderableEntity& a, const RenderableEntity& b) {
        const float yA = a.Transform->position.y;
        const float yB = b.Transform->position.y;

        if (yA != yB) {
            return yA < yB;
        }

        return a.Transform->position.x < b.Transform->position.x;
    });
}

void RenderSystem::DrawRenderables(SDL_Renderer* renderer,
                                   const std::vector<RenderableEntity>& renderList,
                                   const RenderContext& context) {
    for (const auto& item : renderList) {
        const TransformComponent& transform = *item.Transform;
        const SpriteRendererComponent& sprite = *item.Sprite;

        const Vec2 screenPos = IsoMath::WorldToScreen(
            transform.position.x,
            transform.position.y,
            context.TileWidth,
            context.TileHeight
        );

        const float baseScreenX = context.CameraOffsetX + screenPos.x + sprite.OffsetX;
        const float baseScreenY = context.CameraOffsetY + screenPos.y + sprite.OffsetY;

        for (SDL_Texture* texture : sprite.Textures) {
            if (!texture) {
                continue;
            }

            float texW = 0.0f;
            float texH = 0.0f;
            SDL_GetTextureSize(texture, &texW, &texH);

            const SDL_FRect* srcRectPtr = nullptr;
            SDL_FRect animationSrcRect {};

            if (sprite.Animation.IsActive()) {
                animationSrcRect = sprite.GetAnimationSrcRect();
                texW = animationSrcRect.w;
                texH = animationSrcRect.h;
                srcRectPtr = &animationSrcRect;
            } else if (sprite.HasCustomSrcRect) {
                texW = sprite.SrcRect.w;
                texH = sprite.SrcRect.h;
                srcRectPtr = &sprite.SrcRect;
            }

            texW *= transform.scale.x;
            texH *= transform.scale.y;

            const SDL_FRect destRect = {
                baseScreenX - (texW * sprite.PivotX),
                baseScreenY - (texH * sprite.PivotY),
                texW,
                texH
            };

            const SDL_FlipMode flip = sprite.FlipX ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
            SDL_RenderTextureRotated(renderer, texture, srcRectPtr, &destRect, 0.0, nullptr, flip);
        }
    }
}
