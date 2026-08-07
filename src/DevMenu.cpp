#include "DevMenu.hpp"
#include "Engine.hpp"
#include "ECS.hpp"
#include <iostream>

#if ENABLE_DEV_MENU
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

static SDL_Window* s_Window = nullptr;
static SDL_Renderer* s_Renderer = nullptr;
#endif

void DevMenu::Init(SDL_Window* window, SDL_Renderer* renderer) {
#if ENABLE_DEV_MENU
    s_Window = window;
    s_Renderer = renderer;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    if (!ImGui_ImplSDL3_InitForSDLRenderer(window, renderer)) {
        std::cerr << "[DevMenu] ImGui_ImplSDL3_InitForSDLRenderer failed\n";
        return;
    }

    if (!ImGui_ImplSDLRenderer3_Init(renderer)) {
        std::cerr << "[DevMenu] ImGui_ImplSDLRenderer3_Init failed\n";
        return;
    }

    initialized = true;
    std::cout << "[DevMenu] ImGui DevMenu initialized\n";
#else
    (void)window;
    (void)renderer;
#endif
}

void DevMenu::ProcessEvent(const SDL_Event* event) {
#if ENABLE_DEV_MENU
    if (initialized) {
        ImGui_ImplSDL3_ProcessEvent(event);
    }
#else
    (void)event;
#endif
}

void DevMenu::Render(Engine* engine) {
#if ENABLE_DEV_MENU
    if (!initialized || !s_Renderer || !engine) {
        return;
    }

    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Dev Menu");
    ImGui::Text("LDoE Engine");
    ImGui::Separator();
    ImGui::Text("Entities: %zu", engine->GetEntities().size());
    ImGui::Text("Placement mode: %s", engine->IsPlacementMode() ? "ON" : "OFF");
    ImGui::Text("Controls:");
    ImGui::BulletText("WASD - move player");
    ImGui::BulletText("B - toggle build mode");
    ImGui::BulletText("LMB - place object (build mode)");
    ImGui::End();

    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), s_Renderer);
#else
    (void)engine;
#endif
}

void DevMenu::Shutdown() {
#if ENABLE_DEV_MENU
    if (initialized) {
        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
        initialized = false;
        s_Window = nullptr;
        s_Renderer = nullptr;
        std::cout << "[DevMenu] Shutdown complete\n";
    }
#endif
}

bool DevMenu::initialized = false;
Entity* DevMenu::selectedEntity = nullptr;
