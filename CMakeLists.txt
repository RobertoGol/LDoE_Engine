cmake_minimum_required(VERSION 3.16)
project(LDoE_Engine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Включаем модуль автоматического скачивания зависимостей
include(FetchContent)

# Отключаем сборку примеров и тестов SDL3 для ускорения компиляции
set(SDL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)

# Скачиваем SDL3 напрямую с GitHub (используем стабильный пререлиз)
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG preview-3.1.3
)
FetchContent_MakeAvailable(SDL3)

# Указываем исходники
set(SOURCES
    src/main.cpp
    src/Engine.cpp
    src/DevMenu.cpp
)

# Создаем исполняемый файл
add_executable(${PROJECT_NAME} ${SOURCES})

# Подключаем папку src для заголовочных файлов
target_include_directories(${PROJECT_NAME} PRIVATE src)

# Линкуем SDL3
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
