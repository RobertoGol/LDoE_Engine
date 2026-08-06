#pragma once
#include <SDL3/SDL.h>

// Флаг сборки: 1 - Режим разработчика (включен интерфейс Unity-like), 0 - Релиз (заглушка)
#define ENABLE_DEV_MENU 1 

class DevMenu {
public:
    static void Init(SDL_Window* window, SDL_Renderer* renderer);
    static void ProcessEvent(const SDL_Event* event);
    static void Render();
    static void Shutdown();
};
