#include <glad/glad.h>
#include <SDL3/SDL_opengl.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Engine.hpp"
#include "DevMenu.hpp"
#include "ResourceManager.hpp"
#include "AssetImporter.hpp"
#include "Math.hpp"
#include "MathGenerator.hpp"
#include "Components/SpriteRendererComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "Components/PlayerInputComponent.hpp"
#include "Components/ColliderComponent.hpp"
#include "Components/InteractableComponent.hpp"
#include <iostream>
#include <fstream>
#include <cmath>

Engine::Engine() : isRunning(false), window(nullptr), renderer(nullptr) {}
Engine::~Engine() { Shutdown(); }

bool Engine::Init(const char* title, int width, int height) {
    windowWidth = width; windowHeight = height;

    // 1. Инициализация ядра SDL В САМОМ НАЧАЛЕ
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << "\n";
        return false;
    }

    // 2. Настраиваем атрибуты OpenGL ПЕРЕД созданием окна
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); // Z-буфер для 3D!

    // 3. Создаем ОДНО окно с поддержкой OpenGL
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_MAXIMIZED);
    window = SDL_CreateWindow(title, windowWidth, windowHeight, window_flags);
    if (!window) {
        std::cerr << "[Engine] Ошибка создания окна: " << SDL_GetError() << "\n";
        return false;
    }

    // 4. Создаем OpenGL контекст
    glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        std::cerr << "[Engine] Ошибка создания OpenGL контекста: " << SDL_GetError() << "\n";
        return false;
    }

    // 5. Активируем GLAD (ТОЛЬКО ПОСЛЕ СОЗДАНИЯ КОНТЕКСТА)
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "[Engine] Ошибка инициализации GLAD!\n";
        return false;
    }
    std::cout << "[Engine] GLAD инициализирован. Версия OpenGL: " << glGetString(GL_VERSION) << "\n";

    // 6. --- ЗАГРУЗКА 3D АССЕТОВ (Теперь всё безопасно) ---
    basicShader = new Shader("assets/Shaders/basic.vert", "assets/Shaders/basic.frag");
    // testWall = new Model("assets/Mesh/wall_straight_1m_concrete.obj");
    testWall = new Model("assets/models/TestBody3D_1p/Copilot3D-36d7902a-044d-4ab6-bf3f-ddd0895f6f1b.glb");
    // -----------------------------------------------------

    // ДОБАВЛЯЕМ ЭТО: 
    // Генерируем JSON с физикой (один раз)
    MathGenerator::GenerateFromDirectory("assets/Characters", "assets/GeneratedMath.json");
    // Инициализируем игрока
    mainPlayer.Init("TestBody", "assets/GeneratedMath.json");


    SDL_ShowWindow(window);

    // Инициализация ImGui 
    DevMenu::Init(window, glContext);
    
    std::cout << "[Engine] OpenGL 3.3 Инициализирован!\n";

    isRunning = true;
    return true;
}

void Engine::ResolveEntityTextures() {
    for (auto& entity : entities) {
        auto* sprite = entity->GetComponent<SpriteRendererComponent>();
        if (!sprite || sprite->LayerTextureIds.empty()) {
            continue;
        }

        sprite->Textures.clear();
        for (const auto& textureId : sprite->LayerTextureIds) {
            if (SDL_Texture* texture = ResourceManager::Get().GetTexture(textureId)) {
                sprite->Textures.push_back(texture);
            }
        }
    }
}

void Engine::SaveScene(const std::string& path) {
    json j = json::array();
    for (auto& ent : entities) {
        j.push_back(ent->Serialize());
    }
    std::ofstream file(path);
    if (file.is_open()) {
        file << j.dump(4);
        std::cout << "[Engine] Сцена успешно сохранена: " << path << "\n";
    }
}

void Engine::LoadScene(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cout << "[Engine] Не удалось открыть файл для загрузки: " << path << "\n";
        return;
    }
    try {
        json j;
        file >> j;
        
        if (j.is_array()) {
            entities.clear();
            for (const auto& ej : j) {
                if (ej.is_object()) {
                    auto ent = std::make_unique<Entity>();
                    ent->Deserialize(ej);
                    entities.push_back(std::move(ent));
                }
            }
            ResolveEntityTextures();
            std::cout << "[Engine] Сцена успешно загружена: " << path << "\n";
        } else if (j.is_object() && j.contains("entities") && j["entities"].is_array()) {
            entities.clear();
            for (const auto& ej : j["entities"]) {
                if (ej.is_object()) {
                    auto ent = std::make_unique<Entity>();
                    ent->Deserialize(ej);
                    entities.push_back(std::move(ent));
                }
            }
            ResolveEntityTextures();
            std::cout << "[Engine] Сцена успешно загружена: " << path << "\n";
        } else {
            std::cout << "[Engine] Пропуск файла: это файл ассета/материала, а не файл сцены.\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "[Engine Error] Ошибка при разборе JSON сцены: " << e.what() << "\n";
    }
}

void Engine::UpdatePreviewGridFromScreen(float screenX, float screenY) {
    const float localX = screenX - camera.OffsetX;
    const float localY = screenY - camera.OffsetY;

    const Vec2 worldPos = IsoMath::ScreenToWorld(localX, localY, TileWidth, TileHeight);
    previewGridPos.x = std::round(worldPos.x);
    previewGridPos.y = std::round(worldPos.y);
}

void Engine::PlacePreviewObject() {
    if (!previewTexture) {
        return;
    }

    // 1. Сначала СОЗДАЕМ сущность (строка, которая у тебя была ниже)
    auto placedObject = std::make_unique<Entity>("PlacedObject");
    
    // 2. Добавляем позицию
    auto* transform = placedObject->AddComponent<TransformComponent>();
    transform->position = previewGridPos;

    // 3. Добавляем спрайт (картинку)
    auto* sprite = placedObject->AddComponent<SpriteRendererComponent>();
    sprite->AddLayer(previewTexture, "workbench");

    // 4. Добавляем коллизию
    auto* collider = placedObject->AddComponent<ColliderComponent>();
    collider->width = 1.0f;
    collider->height = 1.0f;

    // 5. Добавляем интерактивность
    auto* interactable = placedObject->AddComponent<InteractableComponent>();
    interactable->actionName = "Открыть верстак";

    // 6. Сохраняем объект в мир
    entities.push_back(std::move(placedObject));
    std::cout << "[Engine] Объект размещен на (" << previewGridPos.x << ", " << previewGridPos.y << ")\n";
}

void Engine::ProcessInput() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            DevMenu::ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT || (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))) {
                isRunning = false;
            }
    
        
        if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window)) {
            std::cout << "[Engine] Запрошено закрытие окна.\n";
            isRunning = false;
        }

        if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat) {
            if (event.key.scancode == SDL_SCANCODE_B) {
                isPlacementMode = !isPlacementMode;

                if (isPlacementMode) {
                    previewTexture = ResourceManager::Get().LoadTexture(
                        "workbench",
                        "assets/workbench.png",
                        renderer
                    );
                    if (!previewTexture) {
                        previewTexture = ResourceManager::Get().GetTexture("TestImage");
                    }

                    float mouseX = 0.0f;
                    float mouseY = 0.0f;
                    SDL_GetMouseState(&mouseX, &mouseY);
                    UpdatePreviewGridFromScreen(mouseX, mouseY);
                    std::cout << "[Engine] Режим строительства: ВКЛ\n";
                } else {
                    std::cout << "[Engine] Режим строительства: ВЫКЛ\n";
                }
            }
        }

        if (!isPlacementMode) {
            continue;
        }

        if (event.type == SDL_EVENT_MOUSE_MOTION) {
            UpdatePreviewGridFromScreen(event.motion.x, event.motion.y);
        }

        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
            event.button.button == SDL_BUTTON_LEFT) {
            PlacePreviewObject();
        }
    }
}

void Engine::Update(float deltaTime) {
    for (auto& entity : entities) {
        entity->Update(deltaTime);
    }
}

void Engine::Render() {
    // 1. Очищаем экран и буфер глубины средствами OpenGL
    glClearColor(0.14f, 0.14f, 0.14f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 2. Включаем наш шейдер и рисуем 3D
    if (basicShader && testWall) {
        basicShader->Use();
    // Активируем шейдер
    // shader->Use();

        // 3. Создаем Идеальную Изометрическую Камеру (Orthographic)
        float orthoSize = 5.0f; // Охват камеры (зум)
        float aspect = (float)windowWidth / (float)windowHeight;
        glm::mat4 projection = glm::ortho(-orthoSize * aspect, orthoSize * aspect, -orthoSize, orthoSize, -50.0f, 50.0f);

        // Камера теперь следит за ИГРОКОМ
        glm::vec3 camOffset(10.0f, 10.0f, 10.0f); // Смещение изометрии
        glm::mat4 view = glm::lookAt(
            mainPlayer.position + camOffset, // Позиция камеры (игрок + смещение)
            mainPlayer.position,             // Куда смотрим (на игрока)
            glm::vec3(0.0f, 1.0f, 0.0f)     
        );

        // 4. Позиция персонажа в мире из класса Player
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        
        // ВАЖНО: Используем mainPlayer.position вместо старого playerPos!
        modelMatrix = glm::translate(modelMatrix, mainPlayer.position);
        
        // Поворачиваем модель в сторону движения
        modelMatrix = glm::rotate(modelMatrix, mainPlayer.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));

        // Отправляем матрицы в видеокарту
        basicShader->SetMat4("projection", projection);
        basicShader->SetMat4("view", view);
        basicShader->SetMat4("model", modelMatrix);

        // РИСУЕМ!
        testWall->Draw(*basicShader);
    }

    // 5. Рисуем меню разработчика поверх 3D
    DevMenu::Render(this);

    // 6. Выводим кадр на экран
    SDL_GL_SwapWindow(window);
}

void Engine::Run() {
    std::cout << "[Engine] Главный цикл запущен.\n";
    Uint64 lastTime = SDL_GetTicks();
    while (isRunning) {
        Uint64 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        
        ProcessInput();

        // ДОБАВЛЯЕМ ЭТО: Передаем кнопки и время игроку
        const bool* keyboardState = SDL_GetKeyboardState(nullptr);
        mainPlayer.Update(deltaTime, keyboardState);

        Update(deltaTime);
        Render();
    }
    std::cout << "[Engine] Главный цикл завершен.\n";
}

void Engine::Shutdown() {
    if (!isRunning) return;
    isRunning = false;
    entities.clear(); 
    
    if (basicShader) delete basicShader;
    if (testWall) delete testWall;
    
    DevMenu::Shutdown();
    
    // ВАЖНО: Удаляем GL контекст при выходе
    if (glContext) SDL_GL_DestroyContext(glContext);
    if (window) SDL_DestroyWindow(window);
    
    SDL_Quit();
}
