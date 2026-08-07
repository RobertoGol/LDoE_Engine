#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include <memory>
#include <string>
#include "ECS.hpp"
#include "Systems/RenderSystem.hpp"
#include "Systems/PlayerMovementSystem.hpp"
#include "Systems/CameraFollowSystem.hpp"
#include "Systems/AnimationSystem.hpp"
#include "Systems/InteractionSystem.hpp"
#include "Math.hpp"

class Engine {
private:
    SDL_Window* window = nullptr;
    SDL_GLContext glContext = nullptr;
    SDL_Renderer* renderer = nullptr;
    bool isRunning = false;
    std::vector<std::unique_ptr<Entity>> entities;
    RenderSystem renderSystem;
    PlayerMovementSystem playerMovementSystem;
    CameraFollowSystem cameraFollowSystem;
    AnimationSystem animationSystem;
    CameraFollowSystem::CameraState camera;
    int windowWidth = 1280;
    int windowHeight = 720;
    static constexpr float TileWidth = 128.0f;
    static constexpr float TileHeight = 64.0f;
    static constexpr int MapGridSize = 50;

    bool isPlacementMode = false;
    SDL_Texture* previewTexture = nullptr;
    Vec2 previewGridPos { 0.0f, 0.0f };
    InteractionSystem interactionSystem;

public:
    Engine();
    ~Engine();

    bool Init(const char* title, int width, int height);
    void ProcessInput();
    void Update(float deltaTime);
    void Render();
    void Run();
    void Shutdown();

    void AddEntity(std::unique_ptr<Entity> entity) {
        entities.push_back(std::move(entity));
    }

    void RemoveEntity(Entity* entity) {
        std::erase_if(entities, [entity](const auto& e) {
            return e.get() == entity;
        });
    }

    std::vector<std::unique_ptr<Entity>>& GetEntities() {
        return entities;
    }

    void SaveScene(const std::string& path);
    void LoadScene(const std::string& path);

    bool IsPlacementMode() const { return isPlacementMode; }

private:
    void ResolveEntityTextures();
    void UpdatePreviewGridFromScreen(float screenX, float screenY);
    void PlacePreviewObject();
};
