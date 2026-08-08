#pragma once
#include "../ECS.hpp"
#include <string>

struct MeshComponent : public Component {
    std::string modelPath; // Путь к файлу .obj или .fbx
    
    // Идентификаторы OpenGL для хранения 3D-геометрии
    unsigned int VAO = 0; 
    unsigned int VBO = 0; 
    unsigned int EBO = 0; 
    
    int indexCount = 0;    // Количество точек для отрисовки

    // Добавляем обязательные методы для сохранения/загрузки:
    json Serialize() override {
        return {
            {"type", "MeshComponent"},
            {"modelPath", modelPath}
        };
    }

    void Deserialize(const json& j) override {
        modelPath = j.value("modelPath", "");
    }
};