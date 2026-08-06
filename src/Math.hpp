#pragma once
#include <vector>

struct Vector3 {
    float x, y, z;
    Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}
};

struct Vector2 {
    float x, y;
    Vector2(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
};

// Структура 3D-модели (хранит точки и линии)
struct Mesh {
    std::vector<Vector3> vertices;
    std::vector<std::pair<int, int>> edges;
};

class IsometricCamera {
public:
    float offsetX = 1280.0f / 2.0f;
    float offsetY = 720.0f / 4.0f;
    float tileWidth = 64.0f;
    float tileHeight = 32.0f;

    Vector2 WorldToScreen(const Vector3& worldPos) {
        Vector2 screenPos;
        screenPos.x = (worldPos.x - worldPos.y) * (tileWidth / 2.0f) + offsetX;
        screenPos.y = (worldPos.x + worldPos.y) * (tileHeight / 2.0f) - worldPos.z + offsetY;
        return screenPos;
    }
};
