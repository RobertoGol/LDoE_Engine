#include "Engine.hpp"
#include "DevMenu.hpp"
#include "ResourceManager.hpp"
#include "AssetImporter.hpp"
#include "Math.hpp"
#include "Components/SpriteRendererComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/PlayerInputComponent.hpp"
#include "Components/ColliderComponent.hpp"
#include <iostream>
#include <fstream>
#include <cmath>

Engine::Engine() : isRunning(false), window(nullptr), renderer(nullptr) {}
Engine::~Engine() { Shutdown(); }

bool Engine::Init(const char* title, int width, int height) {
    windowWidth = width; windowHeight = height;

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) return false;

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_MAXIMIZED);
    window = SDL_CreateWindow(title, width, height, window_flags);
    if (!window) return false;
    
    SDL_ShowWindow(window);
    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) return false;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    AssetImporter::Init(renderer);
    DevMenu::Init(window, renderer);
    
    ResourceManager::Get().LoadTexture("TestImage", "assets/test.png", renderer);

    std::ifstream file("assets/scene.json");
    if (file.good()) {
        LoadScene("assets/scene.json");
    } else {
        std::cout << "[Engine] Нет сохраненной сцены, создаем пустую.\n";
    }

    auto player = std::make_unique<Entity>("Player");
    auto* playerTransform = player->AddComponent<TransformComponent>();
    playerTransform->position = { 25.0f, 25.0f };

    player->AddComponent<PlayerInputComponent>();

    auto* playerSprite = player->AddComponent<SpriteRendererComponent>();
    playerSprite->ClearLayers();

    SDL_Texture* bodyTexture = ResourceManager::Get().LoadTexture("body", "assets/body.png", renderer);
    SDL_Texture* legsTexture = ResourceManager::Get().LoadTexture("legs", "assets/legs.png", renderer);
    SDL_Texture* torsoTexture = ResourceManager::Get().LoadTexture("torso", "assets/torso.png", renderer);

    if (bodyTexture) {
        playerSprite->AddLayer(bodyTexture, "body");
    }
    if (legsTexture) {
        playerSprite->AddLayer(legsTexture, "legs");
    }
    if (torsoTexture) {
        playerSprite->AddLayer(torsoTexture, "torso");
    }

    if (!playerSprite->HasLayers()) {
        playerSprite->UsePlaceholder = true;
        std::cout << "[Engine] Player textures missing, using placeholder shape.\n";
    }

    player->AddComponent<ColliderComponent>();

    constexpr int walkFrameCount = 8;
    float sheetWidth = 0.0f;
    float sheetHeight = 0.0f;
    if (bodyTexture) {
        SDL_GetTextureSize(bodyTexture, &sheetWidth, &sheetHeight);
    }
    if (sheetWidth > 0.0f && sheetHeight > 0.0f) {
        playerSprite->Animation.TotalFrames = walkFrameCount;
        playerSprite->Animation.FrameWidth = static_cast<int>(sheetWidth / walkFrameCount);
        playerSprite->Animation.FrameHeight = static_cast<int>(sheetHeight);
        playerSprite->Animation.FrameTime = 0.1f;
        playerSprite->Animation.CurrentFrame = 0;
        playerSprite->Animation.CurrentFrameTime = 0.0f;
    }

    entities.push_back(std::move(player));

    const Vec2 playerScreenPos = IsoMath::WorldToScreen(25.0f, 25.0f, TileWidth, TileHeight);
    camera.OffsetX = (static_cast<float>(windowWidth) * 0.5f) - playerScreenPos.x;
    camera.OffsetY = (static_cast<float>(windowHeight) * 0.5f) - playerScreenPos.y;

    std::cout << "[Engine] Тестовый игрок (Paper Doll) заспавнен.\n";

    isRunning = true;
    std::cout << "[Engine] Инициализация успешна. Запуск цикла...\n";
    return true;
}

void Engine::ResolveEntityTextures() {
    for (auto& entity : entities) {
        auto* sprite = entity->GetComponent<SpriteRendererComponent>();
        if (!sprite || sprite->LayerTextureIds.empty()) {
            continue;
        }

        sprite->Textures.clear();
        for (const auto& textureId : sprite->LayerTextureIds) {
            if (SDL_Texture* texture = ResourceManager::Get().GetTexture(textureId)) {
                sprite->Textures.push_back(texture);
            }
        }
    }
}

void Engine::SaveScene(const std::string& path) {
    json j = json::array();
    for (auto& ent : entities) {
        j.push_back(ent->Serialize());
    }
    std::ofstream file(path);
    if (file.is_open()) {
        file << j.dump(4);
        std::cout << "[Engine] Сцена успешно сохранена: " << path << "\n";
    }
}

void Engine::LoadScene(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cout << "[Engine] Не удалось открыть файл для загрузки: " << path << "\n";
        return;
    }
    try {
        json j;
        file >> j;
        
        if (j.is_array()) {
            entities.clear();
            for (const auto& ej : j) {
                if (ej.is_object()) {
                    auto ent = std::make_unique<Entity>();
                    ent->Deserialize(ej);
                    entities.push_back(std::move(ent));
                }
            }
            ResolveEntityTextures();
            std::cout << "[Engine] Сцена успешно загружена: " << path << "\n";
        } else if (j.is_object() && j.contains("entities") && j["entities"].is_array()) {
            entities.clear();
            for (const auto& ej : j["entities"]) {
                if (ej.is_object()) {
                    auto ent = std::make_unique<Entity>();
                    ent->Deserialize(ej);
                    entities.push_back(std::move(ent));
                }
            }
            ResolveEntityTextures();
            std::cout << "[Engine] Сцена успешно загружена: " << path << "\n";
        } else {
            std::cout << "[Engine] Пропуск файла: это файл ассета/материала, а не файл сцены.\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "[Engine Error] Ошибка при разборе JSON сцены: " << e.what() << "\n";
    }
}

void Engine::UpdatePreviewGridFromScreen(float screenX, float screenY) {
    const float localX = screenX - camera.OffsetX;
    const float localY = screenY - camera.OffsetY;

    const Vec2 worldPos = IsoMath::ScreenToWorld(localX, localY, TileWidth, TileHeight);
    previewGridPos.x = std::round(worldPos.x);
    previewGridPos.y = std::round(worldPos.y);
}

void Engine::PlacePreviewObject() {
    if (!previewTexture) {
        return;
    }

    auto placedObject = std::make_unique<Entity>("PlacedObject");
    auto* transform = placedObject->AddComponent<TransformComponent>();
    transform->position = previewGridPos;

    auto* sprite = placedObject->AddComponent<SpriteRendererComponent>();
    sprite->AddLayer(previewTexture, "workbench");

    placedObject->AddComponent<ColliderComponent>();

    entities.push_back(std::move(placedObject));
    std::cout << "[Engine] Объект размещен на (" << previewGridPos.x << ", " << previewGridPos.y << ")\n";
}

void Engine::ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        DevMenu::ProcessEvent(&event);
        if (event.type == SDL_EVENT_QUIT) {
            std::cout << "[Engine] Получено событие выхода (SDL_EVENT_QUIT).\n";
            isRunning = false;
        }
        if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window)) {
            std::cout << "[Engine] Запрошено закрытие окна.\n";
            isRunning = false;
        }

        if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat) {
            if (event.key.scancode == SDL_SCANCODE_B) {
                isPlacementMode = !isPlacementMode;

                if (isPlacementMode) {
                    previewTexture = ResourceManager::Get().LoadTexture(
                        "workbench",
                        "assets/workbench.png",
                        renderer
                    );
                    if (!previewTexture) {
                        previewTexture = ResourceManager::Get().GetTexture("TestImage");
                    }

                    float mouseX = 0.0f;
                    float mouseY = 0.0f;
                    SDL_GetMouseState(&mouseX, &mouseY);
                    UpdatePreviewGridFromScreen(mouseX, mouseY);
                    std::cout << "[Engine] Режим строительства: ВКЛ\n";
                } else {
                    std::cout << "[Engine] Режим строительства: ВЫКЛ\n";
                }
            }
        }

        if (!isPlacementMode) {
            continue;
        }

        if (event.type == SDL_EVENT_MOUSE_MOTION) {
            UpdatePreviewGridFromScreen(event.motion.x, event.motion.y);
        }

        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
            event.button.button == SDL_BUTTON_LEFT) {
            PlacePreviewObject();
        }
    }
}

void Engine::Update(float deltaTime) {
    for (auto& entity : entities) {
        entity->Update(deltaTime);
    }
}

void Engine::Render() {
    SDL_SetRenderDrawColor(renderer, 36, 36, 36, 255); 
    SDL_RenderClear(renderer);

    const float offsetX = camera.OffsetX;
    const float offsetY = camera.OffsetY;
    const float tileW = TileWidth;
    const float tileH = TileHeight;
    const int gridSize = MapGridSize;

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 150);
    for (int i = 0; i <= gridSize; ++i) {
        Vec2 p1_x = IsoMath::WorldToScreen(static_cast<float>(i), 0.0f, tileW, tileH);
        Vec2 p2_x = IsoMath::WorldToScreen(static_cast<float>(i), static_cast<float>(gridSize), tileW, tileH);
        SDL_RenderLine(renderer, offsetX + p1_x.x, offsetY + p1_x.y, offsetX + p2_x.x, offsetY + p2_x.y);

        Vec2 p1_y = IsoMath::WorldToScreen(0.0f, static_cast<float>(i), tileW, tileH);
        Vec2 p2_y = IsoMath::WorldToScreen(static_cast<float>(gridSize), static_cast<float>(i), tileW, tileH);
        SDL_RenderLine(renderer, offsetX + p1_y.x, offsetY + p1_y.y, offsetX + p2_y.x, offsetY + p2_y.y);
    }

    RenderSystem::RenderContext context {
        offsetX,
        offsetY,
        tileW,
        tileH
    };
    renderSystem.Update(renderer, entities, context);

    if (isPlacementMode && previewTexture) {
        const Vec2 previewScreenPos = IsoMath::WorldToScreen(
            previewGridPos.x,
            previewGridPos.y,
            tileW,
            tileH
        );

        float previewW = 0.0f;
        float previewH = 0.0f;
        SDL_GetTextureSize(previewTexture, &previewW, &previewH);

        const SDL_FRect previewDest {
            offsetX + previewScreenPos.x - (previewW * 0.5f),
            offsetY + previewScreenPos.y - previewH,
            previewW,
            previewH
        };

        SDL_SetTextureAlphaMod(previewTexture, 128);
        SDL_RenderTexture(renderer, previewTexture, nullptr, &previewDest);
        SDL_SetTextureAlphaMod(previewTexture, 255);
    }

    DevMenu::Render(this);
    SDL_RenderPresent(renderer);
}

void Engine::Run() {
    std::cout << "[Engine] Главный цикл запущен.\n";
    Uint64 lastTime = SDL_GetTicks();
    while (isRunning) {
        Uint64 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        ProcessInput();

        const bool* keyboardState = SDL_GetKeyboardState(nullptr);
        playerMovementSystem.Update(keyboardState, deltaTime, entities);

        cameraFollowSystem.Update(
            deltaTime,
            entities,
            static_cast<float>(windowWidth),
            static_cast<float>(windowHeight),
            TileWidth,
            TileHeight,
            camera
        );

        Update(deltaTime);
        animationSystem.Update(deltaTime, entities);
        Render();
    }
    std::cout << "[Engine] Главный цикл завершен.\n";
}

void Engine::Shutdown() {
    if (!isRunning) return;
    isRunning = false;
    entities.clear(); 
    DevMenu::Shutdown();
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}
