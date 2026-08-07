#pragma once
#include <SDL3/SDL.h>

class Engine;
class Entity;

class DevMenu {
private:
    static bool initialized;
    static Entity* selectedEntity;

public:
    static void Init(SDL_Window* window, SDL_Renderer* renderer);
    static void ProcessEvent(const SDL_Event* event);
    static void Render(Engine* engine);
    static void Shutdown();
};
