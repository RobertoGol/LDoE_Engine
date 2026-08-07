#pragma once
#include "../ECS.hpp"
#include <string>

struct InteractableComponent : public Component {
    std::string actionName = "Взаимодействовать";
    float interactRadius = 1.5f; // Радиус в клетках сетки
    bool isHovered = false;      // Находимся ли мы рядом
};