#pragma once
#include <string>
#include <iostream>
#include <glm/glm.hpp>
#include <SDL3/SDL.h>
// #include "Model.hpp" // Твой класс загрузки моделей через Assimp
// #include <nlohmann/json.hpp> // Если используешь эту библиотеку для JSON, или любую другую

class Player {
public:
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::vec3 rotation{0.0f, 0.0f, 0.0f};

    // --- Динамические параметры (загрузим из JSON) ---
    float speed = 2.0f;
    float stepSize = 0.5f;
    float collisionRadius = 0.3f;
    float height = 1.8f;
    
    // Model* model = nullptr;

    Player() = default;

    // Метод инициализации игрока по его ID (имени бандла)
    void Init(const std::string& characterId, const std::string& jsonPath) {
        std::cout << "[Player] Инициализация персонажа: " << characterId << "\n";
        
        // Здесь мы читаем GeneratedMath.json
        // Псевдокод парсинга (зависит от твоей библиотеки JSON, например nlohmann/json):
        /*
        std::ifstream file(jsonPath);
        json j;
        file >> j;
        
        for (const auto& character : j["characters_and_props"]) {
            if (character["id"] == characterId) {
                this->height = character["collision"]["height"];
                this->collisionRadius = character["collision"]["radius"];
                this->speed = character["movement"]["base_speed"];
                this->stepSize = character["movement"]["step_size"];
                
                std::string modelPath = character["model_path"];
                // this->model = new Model(modelPath); // Загружаем сам .glb
                
                std::cout << "[Player] Успешно загружен! Скорость: " << this->speed << "\n";
                break;
            }
        }
        */
    }

    // Метод обновления (вызывается каждый кадр в Engine::Run)
    void Update(float deltaTime, const bool* keyboardState) {
        glm::vec3 moveDir(0.0f);

        // Простое WASD управление
        if (keyboardState[SDL_SCANCODE_W]) moveDir.z -= 1.0f;
        if (keyboardState[SDL_SCANCODE_S]) moveDir.z += 1.0f;
        if (keyboardState[SDL_SCANCODE_A]) moveDir.x -= 1.0f;
        if (keyboardState[SDL_SCANCODE_D]) moveDir.x += 1.0f;

        // Нормализуем вектор (чтобы по диагонали не бегал быстрее)
        if (glm::length(moveDir) > 0.0f) {
            moveDir = glm::normalize(moveDir);
            
            // Двигаем игрока с учетом его ЧЕСТНОЙ скорости из JSON!
            position += moveDir * speed * deltaTime;

            // Поворачиваем персонажа в сторону движения
            rotation.y = atan2(moveDir.x, moveDir.z); 
        }
    }

    // Метод отрисовки
    void Render(/* Shader& shader */) {
        // if (model) {
        //     glm::mat4 modelMatrix = glm::mat4(1.0f);
        //     modelMatrix = glm::translate(modelMatrix, position);
        //     modelMatrix = glm::rotate(modelMatrix, rotation.y, glm::vec3(0, 1, 0));
        //     shader.SetMat4("model", modelMatrix);
        //     model->Draw(shader);
        // }
    }
};