#include "Engine.hpp"
#include "DevMenu.hpp"
#include <iostream>

Engine::Engine() : isRunning(false), window(nullptr), renderer(nullptr) {}
Engine::~Engine() { Shutdown(); }

bool Engine::Init(const char* title, int width, int height) {
    // Инициализация видео и аудио подсистем SDL3
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        std::cerr << "[Engine] Ошибка SDL_Init: " << SDL_GetError() << "\n";
        return false;
    }

    // Создаем окно
    window = SDL_CreateWindow(title, width, height, SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "[Engine] Ошибка создания окна: " << SDL_GetError() << "\n";
        return false;
    }

    // Создаем рендерер (пока стандартный SDL3, позже заменим на кастомный OpenGL пайплайн если потребуется)
    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "[Engine] Ошибка создания рендерера: " << SDL_GetError() << "\n";
        return false;
    }

    // Инициализация интерфейса редактора (отработает только если ENABLE_DEV_MENU == 1)
    DevMenu::Init(window, renderer);
    
    isRunning = true;
    std::cout << "[Engine] Движок успешно инициализирован.\n";
    return true;
}

void Engine::ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        // Сначала отдаем события редактору (ImGui)
        DevMenu::ProcessEvent(&event);

        if (event.type == SDL_EVENT_QUIT) {
            isRunning = false;
        }
    }
}

void Engine::Update(float deltaTime) {
    // Здесь будет вызов обновления логики систем (физика, анимации)
}

void Engine::Render() {
    // Темно-серый фон (в стиле редакторов)
    SDL_SetRenderDrawColor(renderer, 36, 36, 36, 255); 
    SDL_RenderClear(renderer);

    // ==============================================
    // РЕНДЕР ИГРОВОГО МИРА БУДЕТ ЗДЕСЬ
    // ==============================================

    // Отрисовка UI разработчика поверх всего
    DevMenu::Render();

    SDL_RenderPresent(renderer);
}

void Engine::Run() {
    Uint64 lastTime = SDL_GetTicks();
    while (isRunning) {
        Uint64 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        ProcessInput();
        Update(deltaTime);
        Render();
    }
}

void Engine::Shutdown() {
    DevMenu::Shutdown();
    if (renderer) { SDL_DestroyRenderer(renderer); renderer = nullptr; }
    if (window) { SDL_DestroyWindow(window); window = nullptr; }
    SDL_Quit();
    std::cout << "[Engine] Работа завершена.\n";
}
