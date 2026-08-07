#pragma once

class Camera {
public:
    float x = 0.0f;
    float y = 0.0f;
    float speed = 10.0f; // Скорость движения камеры (тайлов в секунду)

    // Возвращает экранное смещение в зависимости от позиции камеры
    float GetOffsetX(float screenWidth) const {
        return (screenWidth / 2.0f) - x;
    }
    
    float GetOffsetY(float screenHeight) const {
        return (screenHeight / 2.0f) - y;
    }
};
