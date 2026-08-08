#pragma once
#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>

namespace fs = std::filesystem;

class MathGenerator {
public:
    static void GenerateFromDirectory(const std::string& assetsDir, const std::string& outputJsonPath) {
        std::cout << "[MathGenerator] Генерация кинематики и коллизий из .bda: " << assetsDir << "\n";
        
        std::ofstream outFile(outputJsonPath);
        outFile << "{\n  \"entities_math\": [\n";
        bool isFirst = true;

        if (!fs::exists(assetsDir)) {
            std::cerr << "[MathGenerator] Ошибка: папка не найдена - " << assetsDir << "\n";
            return;
        }

        for (const auto& entry : fs::recursive_directory_iterator(assetsDir)) {
            if (entry.is_directory() && entry.path().extension() == ".bda") {
                std::string bundleId = entry.path().stem().string(); 
                std::string modelPath = "";

                for (const auto& file : fs::directory_iterator(entry.path())) {
                    if (file.is_regular_file()) {
                        std::string ext = file.path().extension().string();
                        if (ext == ".glb" || ext == ".obj" || ext == ".fbx" || ext == ".dae") {
                            modelPath = file.path().string();
                            break; 
                        }
                    }
                }

                if (!modelPath.empty()) {
                    EntityMath math = CalculateMathForModel(modelPath);
                    
                    if (!isFirst) outFile << ",\n";
                    isFirst = false;

                    outFile << "    {\n";
                    outFile << "      \"id\": \"" << bundleId << "\",\n";
                    outFile << "      \"bundle_path\": \"" << entry.path().string() << "\",\n";
                    outFile << "      \"model_path\": \"" << modelPath << "\",\n";
                    outFile << "      \"collision\": {\n";
                    outFile << "        \"height\": " << math.height << ",\n";
                    outFile << "        \"radius\": " << math.radius << ",\n";
                    outFile << "        \"center_y\": " << math.centerY << "\n";
                    outFile << "      },\n";
                    outFile << "      \"movement\": {\n";
                    outFile << "        \"base_speed\": " << math.baseSpeed << ",\n";
                    outFile << "        \"step_size\": " << math.stepSize << ",\n";
                    outFile << "        \"bob_amplitude\": " << math.bobAmplitude << ",\n";
                    outFile << "        \"mass_kg\": " << math.mass << "\n";
                    outFile << "      }\n";
                    outFile << "    }";
                    
                    std::cout << "[MathGenerator] Расчеты завершены для: " << bundleId << "\n";
                }
            }
        }
        
        outFile << "\n  ]\n}\n";
        outFile.close();
        std::cout << "[MathGenerator] Вся математика сохранена в: " << outputJsonPath << "\n";
    }

private:
    struct EntityMath {
        // Коллизия
        float height;
        float radius;
        float centerY;
        // Кинематика (движение)
        float stepSize;
        float baseSpeed;
        float bobAmplitude;
        float mass;
    };

    static EntityMath CalculateMathForModel(const std::string& path) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_JoinIdenticalVertices);
        
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "Ошибка Assimp при чтении: " << importer.GetErrorString() << "\n";
            return {1.8f, 0.5f, 0.9f, 0.7f, 3.5f, 0.05f, 70.0f}; // Дефолтные параметры человека
        }

        glm::vec3 minAABB(99999.0f);
        glm::vec3 maxAABB(-99999.0f);

        // Ищем крайние точки модели
        for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[i];
            for (unsigned int j = 0; i < mesh->mNumVertices; j++) {
                aiVector3D v = mesh->mVertices[j];
                if (v.x < minAABB.x) minAABB.x = v.x;
                if (v.y < minAABB.y) minAABB.y = v.y;
                if (v.z < minAABB.z) minAABB.z = v.z;
                
                if (v.x > maxAABB.x) maxAABB.x = v.x;
                if (v.y > maxAABB.y) maxAABB.y = v.y;
                if (v.z > maxAABB.z) maxAABB.z = v.z;
            }
        }

        EntityMath math;
        
        // --- 1. СТАТИКА И КОЛЛИЗИИ ---
        math.height = std::abs(maxAABB.y - minAABB.y);
        
        // Фикс для сломанных моделей с нулевой высотой
        if (math.height < 0.01f) math.height = 1.0f; 

        float widthX = std::abs(maxAABB.x - minAABB.x);
        float widthZ = std::abs(maxAABB.z - minAABB.z);
        math.radius = std::max(widthX, widthZ) / 2.0f;
        math.centerY = minAABB.y + (math.height / 2.0f);

        // --- 2. КИНЕМАТИКА И ДВИЖЕНИЕ ---
        
        // Длина шага - ~42% от роста модели
        math.stepSize = math.height * 0.42f;
        
        // Базовая скорость (считаем, что юнит делает ~2.2 шага в секунду при нормальном беге)
        math.baseSpeed = math.stepSize * 2.2f;

        // Амплитуда раскачивания при ходьбе (2.5% от роста)
        math.bobAmplitude = math.height * 0.025f;

        // Масса. Считаем объем капсулы коллизии.
        // Формула цилиндра: V = PI * r^2 * h
        float volume = 3.14159f * (math.radius * math.radius) * math.height;
        // Магический коэффициент плотности (учитываем, что AABB шире реального тела). 
        // 50.0f дает примерно 70-80 кг для гуманоида высотой 1.8м и радиусом 0.4м
        math.mass = volume * 50.0f; 

        return math;
    }
};