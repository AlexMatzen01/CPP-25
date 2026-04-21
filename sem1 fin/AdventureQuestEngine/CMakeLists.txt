cmake_minimum_required(VERSION 3.10)
project(AdventureQuestEngine VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add compiler warnings
if(MSVC)
    add_compile_options(/W4 /utf-8)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/Combat.cpp
    src/Events.cpp
    src/FileIO.cpp
    src/Inventory.cpp
    src/MapGenerator.cpp
    src/Utils.cpp
    src/GameEngine.cpp
    src/Achievement.cpp
    src/Companion.cpp
    src/Crafting.cpp
    src/Magic.cpp
    src/SkillTree.cpp
    src/UIEnhanced.cpp
)

# Create executable
add_executable(AdventureQuestEngine main.cpp ${SOURCES})

# Optional: Set output directory
set_target_properties(AdventureQuestEngine PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Print build information
message(STATUS "Building Adventure Quest Engine v${PROJECT_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
