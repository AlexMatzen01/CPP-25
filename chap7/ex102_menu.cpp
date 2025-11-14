/*
# RevisedMenu accepts (no) arguments
# it demonstrates a simple looping menu (calls are illustrative — run exercises directly)
# it outputs the user's choice and instructs how to run exercise programs
*/

#include <iostream>
#include <string>
#include <cstdlib>
#include <filesystem>
#include "DisplayMenu.h"

int main(){
    while (true) {
        int c = displayMenu();
        if (c == 0) break;
        std::string relPath; // executable name expected in the current working directory (menu is intended to be run from chap7)
        switch(c){
            case 1: relPath = "ex12"; break;
            case 2: relPath = "ex16"; break;
            case 3: relPath = "ex18"; break;
            case 4: relPath = "ex19"; break;
            case 5: relPath = "ex20"; break;
            case 6: relPath = "ex22"; break;
            default:
                std::cout << "Not implemented in menu.\n";
                continue;
        }

        namespace fs = std::filesystem;
        if (!fs::exists(relPath)) {
            std::cout << "Executable '" << relPath << "' not found. The menu and exercises are meant to be built and run from the 'chap7' directory.\n";
            std::cout << "Build instructions (run inside 'chap7'):\n";
            std::cout << "  g++ -std=c++17 ex12_gradebook.cpp -o ex12\n";
            std::cout << "  g++ -std=c++17 ex16_worldseries.cpp -o ex16\n";
            std::cout << "  g++ -std=c++17 ex18_tictactoe.cpp -o ex18\n";
            std::cout << "  g++ -std=c++17 ex19_magic8ball.cpp -o ex19\n";
            std::cout << "  g++ -std=c++17 ex20_gas1994.cpp -o ex20\n";
            std::cout << "  g++ -std=c++17 ex22_theater.cpp -o ex22\n";
            std::cout << "  g++ -std=c++17 ex102_menu.cpp DisplayMenu.cpp -o menu\n";
            continue;
        }
        std::string cmd = "./" + relPath;
        int rc = std::system(cmd.c_str());
        if (rc == -1) {
            std::cout << "Failed to run '" << cmd << "'.\n";
        }
        // after program exits, loop back to menu
    }
    return 0;
}
