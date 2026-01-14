# AdventureQuest 3D Engine

This is the 3D engine for AdventureQuest, built from scratch using OpenGL and GLFW.

## Dependencies

To build this project, you need:
- **CMake** (3.10 or higher)
- **GLFW 3**
- **GLM** (OpenGL Mathematics)
- **OpenGL**
- **stb_image.h** (Place in `include/` or system path)
- A C++17 compatible compiler

## Building

1. Create a build directory: `mkdir build && cd build`
2. Run CMake: `cmake ..`
3. Build: `cmake --build .`

## Structure

- `include/`: Header files for the engine.
- `src/`: Source files for the engine.
- `shaders/`: GLSL shader files.
- `main.cpp`: Entry point for testing the engine.
