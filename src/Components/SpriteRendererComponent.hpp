#pragma once
#include <string>
#include <vector>
#include <SDL3/SDL.h>
#include "../ECS.hpp"

struct AnimationData {
    int TotalFrames = 1;
    int CurrentFrame = 0;
    float FrameTime = 0.1f;
    float CurrentFrameTime = 0.0f;
    int FrameWidth = 0;
    int FrameHeight = 0;
    bool IsPlaying = true;

    bool IsActive() const {
        return TotalFrames > 1 && FrameWidth > 0 && FrameHeight > 0;
    }
};

class SpriteRendererComponent : public Component {
public:
    std::vector<SDL_Texture*> Textures;
    std::vector<std::string> LayerTextureIds;

    AnimationData Animation;

    SDL_FRect SrcRect { 0.0f, 0.0f, 0.0f, 0.0f };
    bool HasCustomSrcRect = false;
    float PivotX = 0.5f;
    float PivotY = 1.0f;
    float OffsetX = 0.0f;
    float OffsetY = 0.0f;
    bool FlipX = false;
    bool UsePlaceholder = false;

    SDL_FRect GetAnimationSrcRect() const {
        return {
            static_cast<float>(Animation.CurrentFrame * Animation.FrameWidth),
            0.0f,
            static_cast<float>(Animation.FrameWidth),
            static_cast<float>(Animation.FrameHeight)
        };
    }

    void AddLayer(SDL_Texture* texture, const std::string& textureId = "") {
        if (!texture) {
            return;
        }
        Textures.push_back(texture);
        if (!textureId.empty()) {
            LayerTextureIds.push_back(textureId);
        }
    }

    void ClearLayers() {
        Textures.clear();
        LayerTextureIds.clear();
        UsePlaceholder = false;
    }

    bool HasLayers() const {
        return !Textures.empty();
    }

    json Serialize() override {
        json layers = json::array();
        for (const auto& id : LayerTextureIds) {
            layers.push_back(id);
        }

        return {
            {"type", "SpriteRendererComponent"},
            {"layers", layers},
            {"offsetX", OffsetX},
            {"offsetY", OffsetY},
            {"pivotX", PivotX},
            {"pivotY", PivotY}
        };
    }

    void Deserialize(const json& j) override {
        OffsetX = j.value("offsetX", 0.0f);
        OffsetY = j.value("offsetY", 0.0f);
        PivotX = j.value("pivotX", 0.5f);
        PivotY = j.value("pivotY", 1.0f);

        LayerTextureIds.clear();
        Textures.clear();

        if (j.contains("layers") && j["layers"].is_array()) {
            for (const auto& layer : j["layers"]) {
                if (layer.is_string()) {
                    LayerTextureIds.push_back(layer.get<std::string>());
                }
            }
        } else if (j.contains("textureId") && j["textureId"].is_string()) {
            LayerTextureIds.push_back(j["textureId"].get<std::string>());
        }
    }
};
