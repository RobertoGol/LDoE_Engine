#pragma once
#include <string>
#include <unordered_map>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

class ResourceManager {
public:
    static ResourceManager& Get() {
        static ResourceManager instance;
        return instance;
    }

    SDL_Texture* LoadTexture(SDL_Renderer* renderer, const std::string& filepath) {
        auto it = m_Textures.find(filepath);
        if (it != m_Textures.end()) return it->second;

        SDL_Texture* texture = IMG_LoadTexture(renderer, filepath.c_str());
        if (!texture) {
            SDL_Log("ResourceManager Error: Failed to load texture '%s': %s", filepath.c_str(), SDL_GetError());
            return nullptr;
        }
        m_Textures[filepath] = texture;
        return texture;
    }

    void Clear() {
        for (auto& [path, texture] : m_Textures) {
            if (texture) SDL_DestroyTexture(texture);
        }
        m_Textures.clear();
    }

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
private:
    ResourceManager() = default;
    ~ResourceManager() { Clear(); }
    std::unordered_map<std::string, SDL_Texture*> m_Textures;
};
