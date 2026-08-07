#pragma once
#include <vector>
#include <string>
#include <nlohmann/json.hpp>

struct TilemapLayer {
    std::string name = "Layer";
    int width = 10;
    int height = 10;
    std::vector<int> tiles; // ID тайлов, -1 — пусто
};

class TilemapComponent {
public:
    std::string name = "Tilemap";
    float tileWidth = 128.0f;
    float tileHeight = 64.0f;
    std::vector<TilemapLayer> layers;

    nlohmann::json Serialize() const {
        nlohmann::json j;
        j["name"] = name;
        j["tile_width"] = tileWidth;
        j["tile_height"] = tileHeight;
        nlohmann::json jLayers = nlohmann::json::array();
        for (const auto& layer : layers) {
            jLayers.push_back({
                {"name", layer.name},
                {"width", layer.width},
                {"height", layer.height},
                {"tiles", layer.tiles}
            });
        }
        j["layers"] = jLayers;
        return j;
    }

    void Deserialize(const nlohmann::json& j) {
        if (!j.is_object()) return;
        if (j.contains("name") && j["name"].is_string()) name = j["name"];
        if (j.contains("tile_width") && j["tile_width"].is_number()) tileWidth = j["tile_width"];
        if (j.contains("tile_height") && j["tile_height"].is_number()) tileHeight = j["tile_height"];
        
        layers.clear();
        if (j.contains("layers") && j["layers"].is_array()) {
            for (const auto& jl : j["layers"]) {
                if (!jl.is_object()) continue; // Защита от некорректных типов
                TilemapLayer layer;
                if (jl.contains("name") && jl["name"].is_string()) layer.name = jl["name"];
                if (jl.contains("width") && jl["width"].is_number_integer()) layer.width = jl["width"];
                if (jl.contains("height") && jl["height"].is_number_integer()) layer.height = jl["height"];
                if (jl.contains("tiles") && jl["tiles"].is_array()) {
                    layer.tiles = jl["tiles"].get<std::vector<int>>();
                }
                layers.push_back(layer);
            }
        }
    }
};
