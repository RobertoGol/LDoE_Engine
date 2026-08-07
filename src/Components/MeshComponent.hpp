#pragma once
#include "../ECS.hpp"
#include <string>

struct MeshComponent : public Component {
    std::string modelPath; // Путь к .obj или .fbx
    unsigned int VAO = 0;  // Vertex Array Object
    unsigned int VBO = 0;  // Vertex Buffer Object
    unsigned int EBO = 0;  // Element Buffer Object
    int indexCount = 0;    // Количество индексов (вершин) для отрисовки
};