#include "InteractionSystem.hpp"
#include "../Components/PlayerInputComponent.hpp"
#include "../Components/TransformComponent.hpp"
#include "../Components/InteractableComponent.hpp"
#include "../Math.hpp"
#include <iostream>
#include <cmath>

void InteractionSystem::Update(const bool* keyboardState, std::vector<std::unique_ptr<Entity>>& entities) {
    // Находим игрока
    Entity* player = nullptr;
    TransformComponent* playerTransform = nullptr;

    for (auto& entity : entities) {
        if (entity->HasComponent<PlayerInputComponent>()) {
            player = entity.get();
            playerTransform = entity->GetComponent<TransformComponent>();
            break;
        }
    }

    if (!player || !playerTransform) return;

    // Флаг, нажата ли кнопка E (чтобы не спамить, можно проверять события, но для теста сойдет стейт)
    static bool wasEPressed = false;
    bool isEPressed = keyboardState[SDL_SCANCODE_E];

    for (auto& entity : entities) {
        if (entity.get() == player) continue;

        auto* interactable = entity->GetComponent<InteractableComponent>();
        auto* targetTransform = entity->GetComponent<TransformComponent>();

        if (interactable && targetTransform) {
            // Вычисляем расстояние между игроком и объектом
            float dx = playerTransform->position.x - targetTransform->position.x;
            float dy = playerTransform->position.y - targetTransform->position.y;
            float distance = std::sqrt(dx * dx + dy * dy);

            // Если игрок в радиусе взаимодействия
            if (distance <= interactable->interactRadius) {
                if (!interactable->isHovered) {
                    interactable->isHovered = true;
                    std::cout << "[Interaction] Нажмите 'E' чтобы " << interactable->actionName << "\n";
                }

                if (isEPressed && !wasEPressed) {
                    std::cout << "[Interaction] Игрок взаимодействует с: " << interactable->actionName << "!\n";
                    // ЗДЕСЬ ПОЗЖЕ БУДЕТ ОТКРЫТИЕ ИНВЕНТАРЯ ИЛИ ВЕРСТАКА
                }
            } else {
                interactable->isHovered = false;
            }
        }
    }
    
    wasEPressed = isEPressed;
}