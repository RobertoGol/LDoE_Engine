#include "DevMenu.hpp"
#include <iostream>

#if ENABLE_DEV_MENU
// Здесь позже подключим ImGui
void DevMenu::Init(SDL_Window* window, SDL_Renderer* renderer) {
    std::cout << "[DevMenu] Инициализация меню разработчика (ImGui placeholder)\n";
}

void DevMenu::ProcessEvent(const SDL_Event* event) {
    // Перехват событий для ImGui (клики, ввод текста)
}

void DevMenu::Render() {
    // Отрисовка докспейсов, инспекторов и окон ассетов
}

void DevMenu::Shutdown() {
    std::cout << "[DevMenu] Завершение работы меню разработчика\n";
}
#else
// --- ЗАГЛУШКИ ДЛЯ РЕЛИЗА (Игрок ничего не увидит, компилятор вырежет вызовы) ---
void DevMenu::Init(SDL_Window* window, SDL_Renderer* renderer) {}
void DevMenu::ProcessEvent(const SDL_Event* event) {}
void DevMenu::Render() {}
void DevMenu::Shutdown() {}
#endif
