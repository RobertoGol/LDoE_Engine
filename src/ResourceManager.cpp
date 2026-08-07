#include "ResourceManager.hpp"
#include "DevMenu.hpp"
#include <iostream>

#if ENABLE_DEV_MENU
#include "imgui.h"
#endif

void ResourceManager::LoadEditorResources() {
#if ENABLE_DEV_MENU
    ImGuiIO& io = ImGui::GetIO();
    static const ImWchar cyrillic_ranges[] = {
        0x0020, 0x00FF, // Basic Latin
        0x0400, 0x052F, // Cyrillic
        0,
    };

    ImFont* font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 18.0f, nullptr, cyrillic_ranges);
    if (!font) {
        std::cerr << "[ResourceManager] Ошибка загрузки шрифта! Будет использован стандартный.\n";
        io.Fonts->Build();
    } else {
        std::cout << "[ResourceManager] Кириллический шрифт успешно загружен.\n";
    }
#endif
}

SDL_Texture* ResourceManager::LoadTexture(const std::string& name, const std::string& path, SDL_Renderer* renderer) {
    if (m_Textures.find(name) != m_Textures.end()) {
        return m_Textures[name];
    }

    SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());
    if (!texture) {
        std::cerr << "[ResourceManager] Ошибка загрузки текстуры " << path << ": " << SDL_GetError() << "\n";
        return nullptr;
    }

    m_Textures[name] = texture;
    std::cout << "[ResourceManager] Текстура загружена: " << name << " (" << path << ")\n";
    return texture;
}

SDL_Texture* ResourceManager::GetTexture(const std::string& name) {
    if (m_Textures.find(name) != m_Textures.end()) {
        return m_Textures[name];
    }
    return nullptr;
}

void ResourceManager::Clear() {
    for (auto const& [name, texture] : m_Textures) {
        SDL_DestroyTexture(texture);
    }
    m_Textures.clear();
    std::cout << "[ResourceManager] Все ресурсы очищены.\n";
}
