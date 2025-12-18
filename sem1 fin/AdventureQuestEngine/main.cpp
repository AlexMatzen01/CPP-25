//! Copyright (c) Alexander Matzen. All rights reserved.
//! Author: Alexander Matzen
//! Licensed under the MIT license

// Main entry point for Adventure Quest Engine

#include "include/GameEngine.h"
#include <iostream>
#include <exception>

int main() {
    try {
        // Create game engine and run
        AQE::GameEngine game;
        return game.run();
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred!" << std::endl;
        return 1;
    }
}
