g++ -std=c++17 -O0 -g3 -Wall -Wextra -Wpedantic \
-fno-omit-frame-pointer \
-Iinclude \
src/Combat.cpp src/Events.cpp src/FileIO.cpp src/Inventory.cpp \
src/MapGenerator.cpp src/Utils.cpp src/GameEngine.cpp \
src/SkillTree.cpp src/Crafting.cpp src/Companion.cpp \
src/Achievement.cpp src/UIEnhanced.cpp src/Magic.cpp \
main.cpp \
-o bin/AdventureQuestEngine