#include "Engine.hpp"
#include <iostream>

int main(int /*argc*/, char* /*argv*/[]) {
    Engine engine;
    if (!engine.Init("LDoE Engine", 1280, 720)) {
        std::cerr << "[Main] Failed to initialize engine.\n";
        return 1;
    }

    engine.Run();
    return 0;
}
