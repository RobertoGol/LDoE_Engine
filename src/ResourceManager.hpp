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

    void LoadEditorResources();
    SDL_Texture* LoadTexture(const std::string& name, const std::string& path, SDL_Renderer* renderer);
    SDL_Texture* GetTexture(const std::string& name);
    void Clear();

private:
    ResourceManager() = default;
    ~ResourceManager() { Clear(); }

    std::unordered_map<std::string, SDL_Texture*> m_Textures;
};
