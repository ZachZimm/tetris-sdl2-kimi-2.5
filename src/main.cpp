#include "Game.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    
    Game game;
    
    if (!game.initialize()) {
        std::cerr << "Failed to initialize game!" << std::endl;
        return 1;
    }
    
    std::cout << "Tetris - SDL2 Edition" << std::endl;
    std::cout << "=====================" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  Left/Right Arrow - Move piece" << std::endl;
    std::cout << "  Down Arrow       - Soft drop" << std::endl;
    std::cout << "  Up/Z             - Rotate" << std::endl;
    std::cout << "  Space            - Hard drop" << std::endl;
    std::cout << "  P                - Pause/Resume" << std::endl;
    std::cout << "  Escape           - Quit" << std::endl;
    std::cout << std::endl;
    
    game.run();
    
    return 0;
}
