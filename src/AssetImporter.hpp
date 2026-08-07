#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include "ECS.hpp"
#include "ResourceManager.hpp"
#include "BoxColliderComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/SpriteRendererComponent.hpp"

class AssetImporter {
private:
    inline static SDL_Renderer* s_renderer = nullptr;

public:
    static AssetImporter& Get() {
        static AssetImporter instance;
        return instance;
    }

    static void Init(SDL_Renderer* renderer) {
        s_renderer = renderer;
    }

    std::unique_ptr<Entity> ImportEntity(const std::string& path, SDL_Renderer* renderer = nullptr) {
        SDL_Renderer* activeRenderer = renderer ? renderer : s_renderer;

        std::ifstream file(path);
        if (!file.is_open()) {
            std::cout << "[AssetImporter] Не удалось открыть файл: " << path << "\n";
            return nullptr;
        }

        try {
            nlohmann::json j;
            file >> j;
            std::cout << "[AssetImporter] Импорт ассета: " << path << "\n";

            auto entity = std::make_unique<Entity>();

            // Имя сущности по имени файла
            size_t lastSlash = path.find_last_of("/\\");
            std::string fileName = (lastSlash == std::string::npos) ? path : path.substr(lastSlash + 1);
            size_t lastDot = fileName.find_last_of('.');
            if (lastDot != std::string::npos) {
                fileName = fileName.substr(0, lastDot);
            }
            entity->name = fileName;

            entity->AddComponent<TransformComponent>();

            std::string texName = "";
            if (j.contains("texture") && j["texture"].is_string()) {
                texName = j["texture"];
            } else if (j.contains("icon") && j["icon"].is_string()) {
                texName = j["icon"];
            }

            auto* spriteRenderer = entity->AddComponent<SpriteRendererComponent>();
            if (!texName.empty()) {
                std::string texPath = "assets/textures/" + texName + ".png";
                if (activeRenderer) {
                    SDL_Texture* texture = ResourceManager::Get().LoadTexture(texName, texPath, activeRenderer);
                    spriteRenderer->AddLayer(texture, texName);
                }
            } else {
                SDL_Texture* texture = ResourceManager::Get().GetTexture("TestImage");
                spriteRenderer->AddLayer(texture, "TestImage");
            }

            if (j.contains("collider") && j["collider"].is_object()) {
                auto col = j["collider"];
                float cWidth = col.value("width", 32.0f);
                float cHeight = col.value("height", 32.0f);
                float cOffsetX = col.value("offsetX", 0.0f);
                float cOffsetY = col.value("offsetY", 0.0f);
                bool cIsTrigger = col.value("isTrigger", false);

                auto boxCol = entity->AddComponent<BoxColliderComponent>(cWidth, cHeight, cOffsetX, cOffsetY);
                boxCol->isTrigger = cIsTrigger;
            } else {
                entity->AddComponent<BoxColliderComponent>(32.0f, 32.0f);
            }

            return entity;
        } catch (const std::exception& e) {
            std::cerr << "[AssetImporter Error] Ошибка парсинга: " << e.what() << "\n";
            return nullptr;
        }
    }

    bool Import(const std::string& path, SDL_Renderer* renderer = nullptr) {
        auto ent = ImportEntity(path, renderer);
        return ent != nullptr;
    }

    bool Load(const std::string& path, SDL_Renderer* renderer) {
        return Import(path, renderer);
    }
};
