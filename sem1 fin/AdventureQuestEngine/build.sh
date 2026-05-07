#!/usr/bin/env bash

set -e

mkdir -p bin
mkdir -p build

CXX="g++"

CXXFLAGS=(
    -std=c++17
    -O2
    -march=native
    -pipe
    -flto
    -Wall
    -Wextra
    -Wpedantic
    -Iinclude
)

SRC=(
    src/Combat.cpp
    src/Events.cpp
    src/FileIO.cpp
    src/Inventory.cpp
    src/MapGenerator.cpp
    src/Utils.cpp
    src/GameEngine.cpp
    src/SkillTree.cpp
    src/Crafting.cpp
    src/Companion.cpp
    src/Achievement.cpp
    src/UIEnhanced.cpp
    src/Magic.cpp
    main.cpp
)

OBJ=()

# Compile all files in parallel
for file in "${SRC[@]}"; do
    obj="build/$(basename "${file%.cpp}.o")"
    OBJ+=("$obj")

    $CXX "${CXXFLAGS[@]}" -c "$file" -o "$obj" &
done

wait

# Link
$CXX "${CXXFLAGS[@]}" "${OBJ[@]}" -o bin/AdventureQuestEngine