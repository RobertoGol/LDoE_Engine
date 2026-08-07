#pragma once

struct Vec2 {
    float x;
    float y;
};

class IsoMath {
public:
    // Преобразует декартовы координаты игрового мира в изометрические координаты экрана
    static Vec2 WorldToScreen(float worldX, float worldY, float tileW = 128.0f, float tileH = 64.0f) {
        return {
            (worldX - worldY) * (tileW / 2.0f),
            (worldX + worldY) * (tileH / 2.0f)
        };
    }

    // Преобразует экранные координаты (например, клик мыши) обратно в игровые координаты
    static Vec2 ScreenToWorld(float screenX, float screenY, float tileW = 128.0f, float tileH = 64.0f) {
        return {
            (screenX / (tileW / 2.0f) + screenY / (tileH / 2.0f)) / 2.0f,
            (screenY / (tileH / 2.0f) - screenX / (tileW / 2.0f)) / 2.0f
        };
    }
};
