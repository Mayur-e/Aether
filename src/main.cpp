#include "core/Game.h"
#include <iostream>

int main() {
    Game game;
    if (!game.init(1280, 720, "AETHER - Gravity")) {
        std::cerr << "Failed to initialize game.\n";
        return -1;
    }
    
    game.run();
    
    return 0;
}
