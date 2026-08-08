#pragma once
#include "../ECS.hpp"
#include <string>

struct InteractableComponent : public Component {
    std::string actionName = "Взаимодействовать";
    float interactRadius = 1.5f; // Радиус в клетках сетки
    bool isHovered = false;      // Находимся ли мы рядом

    // Добавляем обязательные методы для сохранения/загрузки:
    json Serialize() override {
        return {
            {"type", "InteractableComponent"},
            {"actionName", actionName},
            {"interactRadius", interactRadius}
        };
    }

    void Deserialize(const json& j) override {
        actionName = j.value("actionName", "Взаимодействовать");
        interactRadius = j.value("interactRadius", 1.5f);
    }
};