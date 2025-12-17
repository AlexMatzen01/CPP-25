/*******************************************************************************
 * C++ Adventure Quest Engine
 * main.cpp - Program Entry Point
 * 
 * This is the main entry point for the Adventure Quest Engine game.
 * It creates the game engine instance and starts the game loop.
 * 
 * Author: Adventure Quest Engine Team
 * Version: 1.0
 ******************************************************************************/

#include "include/GameEngine.h"
#include <iostream>
#include <exception>

int main() {
    try {
        // Create game engine instance
        AQE::GameEngine game;
        
        // Run the game
        return game.run();
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred!" << std::endl;
        return 1;
    }
}
