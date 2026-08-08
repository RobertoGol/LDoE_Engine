#include "DevMenu.hpp"
#include "Engine.hpp"
#include <iostream>

#if ENABLE_DEV_MENU
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

static SDL_Window* s_Window = nullptr;
static SDL_GLContext s_GLContext = nullptr;
#endif

void DevMenu::Init(SDL_Window* window, SDL_GLContext glContext) {
#if ENABLE_DEV_MENU
    s_Window = window;
    s_GLContext = glContext;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    ImGui::StyleColorsDark(); // Темная тема

    // Инициализация ImGui для SDL3 + OpenGL
    ImGui_ImplSDL3_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    
    std::cout << "[DevMenu] Инициализация ImGui (OpenGL 3) успешно завершена.\n";
#endif
}

void DevMenu::ProcessEvent(const SDL_Event* event) {
#if ENABLE_DEV_MENU
    ImGui_ImplSDL3_ProcessEvent(event);
#endif
}

void DevMenu::Render(Engine* engine) {
#if ENABLE_DEV_MENU
    // Начинаем новый кадр ImGui
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    // Рисуем базовое меню разработчика
    ImGui::Begin("LDoE Engine - Dev Menu");
    ImGui::Text("Рендер: OpenGL 3.3");
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    if (engine->IsPlacementMode()) {
        ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "Режим строительства: ВКЛ");
    }
    ImGui::End();

    // Рендерим окна ImGui в буфер OpenGL
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
}

void DevMenu::Shutdown() {
#if ENABLE_DEV_MENU
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
#endif
}