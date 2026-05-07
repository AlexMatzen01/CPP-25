//! Copyright (c) Alexander Matzen. All rights reserved.
//! Author: Alexander Matzen
//! Licensed under the MIT license
//! Edit started 1/2/26

// Adventure Quest Engine - Main Entry Point
// Initializes console encoding and starts the game engine

#include "include/GameEngine.h"
#include <iostream>
#include <exception>
#include <clocale>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

namespace {

// Configure terminal encoding and enable ANSI color support
// Required for cross-platform Unicode and color output
void initializeConsoleEncoding() {
#ifdef _WIN32
    // Windows-specific configuration
    SetConsoleOutputCP(CP_UTF8);  // Output encoding to UTF-8
    SetConsoleCP(CP_UTF8);         // Input encoding to UTF-8

    // Enable virtual terminal processing for ANSI escape sequences
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD mode = 0;
        if (GetConsoleMode(hOut, &mode)) {
            // Add ENABLE_VIRTUAL_TERMINAL_PROCESSING flag
            SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        }
    }

    // Set locale to UTF-8 for proper character display
    std::setlocale(LC_ALL, ".UTF-8");
#else
    // Unix/Linux: use system default locale
    std::setlocale(LC_ALL, "");
#endif
}

    }

// Program entry point: Initialize console and start game
int main() {
    initializeConsoleEncoding();  // Setup terminal for proper display

    try {
        // Create and run game engine
        AQE::GameEngine game;
        return game.run();  // Returns exit code (0=success)
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred!" << std::endl;
        return 1;
    }
}
