cmake_minimum_required(VERSION 3.20)
project(AdventureQuest3D VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ADVENTUREQUEST_USE_FETCHCONTENT "Download raylib if not found" ON)

find_package(raylib CONFIG QUIET)

if(NOT raylib_FOUND AND ADVENTUREQUEST_USE_FETCHCONTENT)
    include(FetchContent)
    set(FETCHCONTENT_QUIET OFF)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.5
    )
    FetchContent_MakeAvailable(raylib)
endif()

if(NOT raylib_FOUND AND NOT TARGET raylib)
    message(FATAL_ERROR "raylib not found. Install raylib or configure with ADVENTUREQUEST_USE_FETCHCONTENT=ON.")
endif()

set(SOURCES
    src/main.cpp
    src/engine/GameEngine.cpp
    src/world/Room.cpp
    src/world/WorldMap.cpp
    src/entities/Player.cpp
    src/entities/Enemy.cpp
    src/combat/CombatManager.cpp
    src/systems/AssetManager.cpp
    src/systems/SaveManager.cpp
    src/util/Logger.cpp
)

add_executable(adventure_quest_3d ${SOURCES})
target_include_directories(adventure_quest_3d PRIVATE src)
target_link_libraries(adventure_quest_3d PRIVATE raylib)

if(UNIX AND NOT APPLE)
    target_link_libraries(adventure_quest_3d PRIVATE m pthread dl rt X11)
endif()

if(UNIX)
    add_custom_target(sync_assets
        COMMAND ${CMAKE_COMMAND} -E env bash ${CMAKE_SOURCE_DIR}/scripts/sync_assets.sh ${CMAKE_SOURCE_DIR}/assets/manifest.txt ${CMAKE_SOURCE_DIR}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Syncing remote assets via curl/wget/powershell fallback"
    )
else()
    add_custom_target(sync_assets
        COMMAND powershell -ExecutionPolicy Bypass -File ${CMAKE_SOURCE_DIR}/scripts/sync_assets.ps1 -ManifestPath ${CMAKE_SOURCE_DIR}/assets/manifest.txt
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Syncing remote assets via PowerShell"
    )
endif()
