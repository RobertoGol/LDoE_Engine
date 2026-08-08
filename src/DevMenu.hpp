#pragma once
#include <SDL3/SDL.h>

class DevMenu {
public:
    // Теперь передаем OpenGL контекст вместо рендерера
    static void Init(SDL_Window* window, SDL_GLContext glContext);
    static void ProcessEvent(const SDL_Event* event);
    static void Render(class Engine* engine);
    static void Shutdown();
};