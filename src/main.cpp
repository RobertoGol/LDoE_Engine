#include "Engine.hpp"

int main(int argc, char* argv[]) {
    Engine engine;
    
    // Запуск движка в 720p
    if (engine.Init("LDoE Engine - Dev Environment", 1280, 720)) {
        engine.Run();
    }
    
    engine.Shutdown();
    return 0;
}
