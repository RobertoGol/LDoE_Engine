#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>
#include <set>
#include "Math.hpp"

namespace fs = std::filesystem;

class ResourceManager {
public:
    fs::path assetsRoot;

    ResourceManager(const std::string& basePath) {
        assetsRoot = fs::absolute(basePath);
        std::cout << "[ResourceManager] Корневая папка ресурсов: " << assetsRoot << std::endl;
    }

    Mesh LoadMesh(const std::string& filename) {
        Mesh mesh;
        fs::path targetPath = assetsRoot / filename;
        
        if (targetPath.extension() == ".obj") {
            targetPath.replace_extension(".mesh");
        }

        if (!fs::exists(targetPath)) {
            bool found = false;
            try {
                for (const auto& entry : fs::recursive_directory_iterator(assetsRoot)) {
                    if (entry.is_regular_file() && (entry.path().filename() == filename || entry.path().filename() == targetPath.filename())) {
                        targetPath = entry.path();
                        found = true;
                        break;
                    }
                }
            } catch (const fs::filesystem_error& e) {
                std::cerr << "[ERROR] Ошибка обхода директории: " << e.what() << std::endl;
            }

            if (!found) {
                std::cerr << "[ERROR] Файл не найден ни в одной подпапке активов: " << filename << std::endl;
                return mesh;
            }
        }

        std::ifstream file(targetPath, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "[ERROR] Не удалось открыть бинарный меш: " << targetPath << std::endl;
            return mesh;
        }

        char magic[4];
        file.read(magic, 4);
        if (std::string(magic, 4) != "LDMS") {
            std::cerr << "[ERROR] Неверный формат файла меша!" << std::endl;
            return mesh;
        }

        uint32_t vertexCount = 0;
        file.read(reinterpret_cast<char*>(&vertexCount), sizeof(vertexCount));
        mesh.vertices.resize(vertexCount);
        file.read(reinterpret_cast<char*>(mesh.vertices.data()), vertexCount * sizeof(Vector3));

        uint32_t indexCount = 0;
        file.read(reinterpret_cast<char*>(&indexCount), sizeof(indexCount));
        std::vector<uint32_t> indices(indexCount);
        if (indexCount > 0) {
            file.read(reinterpret_cast<char*>(indices.data()), indexCount * sizeof(uint32_t));
        }
        file.close();

        std::set<std::pair<int, int>> uniqueEdges;
        for (size_t i = 0; i + 2 < indices.size(); i += 3) {
            int i0 = static_cast<int>(indices[i]);
            int i1 = static_cast<int>(indices[i+1]);
            int i2 = static_cast<int>(indices[i+2]);

            auto addEdge = [&](int a, int b) {
                if (a != b) {
                    if (a > b) std::swap(a, b);
                    uniqueEdges.insert({a, b});
                }
            };
            addEdge(i0, i1);
            addEdge(i1, i2);
            addEdge(i2, i0);
        }

        if (uniqueEdges.empty() && mesh.vertices.size() >= 3) {
            for (size_t i = 0; i + 2 < mesh.vertices.size(); i += 3) {
                int v0 = static_cast<int>(i);
                int v1 = static_cast<int>(i + 1);
                int v2 = static_cast<int>(i + 2);
                uniqueEdges.insert({std::min(v0, v1), std::max(v0, v1)});
                uniqueEdges.insert({std::min(v1, v2), std::max(v1, v2)});
                uniqueEdges.insert({std::min(v2, v0), std::max(v2, v0)});
            }
        }

        mesh.edges.assign(uniqueEdges.begin(), uniqueEdges.end());

        std::cout << "[SUCCESS] Индексированный бинарный меш загружен: " << mesh.vertices.size() << " вершин, " 
                  << mesh.edges.size() << " уникальных ребер." << std::endl;
        return mesh;
    }
};
