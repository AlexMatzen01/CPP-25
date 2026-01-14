cmake_minimum_required(VERSION 3.10)
project(AdventureQuest3D VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Fetch GLFW if not found on the system
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
	glfw
	GIT_REPOSITORY https://github.com/glfw/glfw.git
	GIT_TAG        3.3.8
)
FetchContent_MakeAvailable(glfw)

# Fetch GLM (header-only)
FetchContent_Declare(
	glm
	GIT_REPOSITORY https://github.com/g-truc/glm.git
	GIT_TAG        0.9.9.8
)
FetchContent_MakeAvailable(glm)

# Fetch stb (for stb_image.h)
FetchContent_Declare(
	stb
	GIT_REPOSITORY https://github.com/nothings/stb.git
	GIT_TAG        master
)
FetchContent_MakeAvailable(stb)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Create executable
add_executable(AdventureQuest3D main.cpp ${SOURCES})

# Link libraries
find_package(OpenGL REQUIRED)
target_link_libraries(AdventureQuest3D PRIVATE OpenGL::GL glfw glm)

message(STATUS "Building Adventure Quest 3D Engine v${PROJECT_VERSION}")
