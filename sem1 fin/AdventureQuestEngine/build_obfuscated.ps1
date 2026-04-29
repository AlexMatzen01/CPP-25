& g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic -finput-charset=UTF-8 -fexec-charset=UTF-8 `
-fvisibility=hidden -fvisibility-inlines-hidden `
-ffunction-sections -fdata-sections -fno-ident -s `
"-Wl,--gc-sections" `
-Iinclude `
src/Combat.cpp src/Events.cpp src/FileIO.cpp src/Inventory.cpp src/MapGenerator.cpp `
src/Utils.cpp src/GameEngine.cpp src/SkillTree.cpp src/Crafting.cpp `
src/Companion.cpp src/Achievement.cpp src/UIEnhanced.cpp src/Magic.cpp `
main.cpp -o bin/AdventureQuestEngine ; `
strip --strip-all bin/AdventureQuestEngine ; `
objcopy --remove-section .comment bin/AdventureQuestEngine