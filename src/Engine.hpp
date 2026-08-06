#pragma once
#include <SDL3/SDL.h>

class Engine {
public:
    Engine();
    ~Engine();
    
    bool Init(const char* title, int width, int height);
    void Run();
    void Shutdown();

private:
    void ProcessInput();
    void Update(float deltaTime);
    void Render();

    bool isRunning;
    SDL_Window* window;
    SDL_Renderer* renderer;
};
