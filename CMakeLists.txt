cmake_minimum_required(VERSION 3.16)
project(LDoE_Engine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

include(FetchContent)

# SDL3
FetchContent_Declare(
    SDL3
    URL https://github.com/libsdl-org/SDL/releases/download/release-3.2.8/SDL3-3.2.8.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(SDL3)

# Отключаем лишние форматы SDL3_image
set(SDLIMAGE_AVIF OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_WEBP OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_TIF OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_JXL OFF CACHE BOOL "" FORCE)

# SDL3_image
FetchContent_Declare(
    SDL3_image
    URL https://github.com/libsdl-org/SDL_image/releases/download/release-3.2.4/SDL3_image-3.2.4.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(SDL3_image)

# --- GLM (3D Математика) ---
include(FetchContent)
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(glm)

# --- ASSIMP (Загрузка 3D моделей - FBX, OBJ, GLTF) ---
FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG        v5.3.1
)
# Отключаем сборку тестов и лишнего мусора для Assimp, чтобы быстрее компилилось
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(assimp)

# nlohmann/json
FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(nlohmann_json)

# Dear ImGui
FetchContent_Declare(
    imgui
    URL https://github.com/ocornut/imgui/archive/refs/tags/v1.91.6.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(imgui)

# =====================================================================
# ИСПРАВЛЕННАЯ ЧАСТЬ СБОРКИ ПРОЕКТА
# =====================================================================

# Ищем все исходники, включая новый glad.c
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS "src/*.cpp" "src/*.hpp" "src/glad/glad.c")

add_executable(LDoE_Engine 
    ${ENGINE_SOURCES}
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp # <-- ВАЖНО: заменили sdlrenderer3 на opengl3!
)

# СЮДА кладем ПУТИ к папкам с заголовочными файлами (.hpp / .h)
target_include_directories(LDoE_Engine PRIVATE 
    src 
    ${imgui_SOURCE_DIR} 
    ${imgui_SOURCE_DIR}/backends
    ${glm_SOURCE_DIR}       # <-- Математика здесь
    src/glad/include        # <-- GLAD здесь
)

# СЮДА кладем скомпилированные БИБЛИОТЕКИ (И НИКАКИХ ПУТЕЙ К ПАПКАМ!)
target_link_libraries(LDoE_Engine PRIVATE 
    SDL3::SDL3 
    SDL3_image::SDL3_image 
    nlohmann_json::nlohmann_json
    assimp     # <-- Подключаем загрузчик моделей к проекту
    opengl32   # <-- Системный OpenGL
)

target_compile_definitions(LDoE_Engine PRIVATE ENABLE_DEV_MENU=1)