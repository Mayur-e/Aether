cmake_minimum_required(VERSION 3.25)
project(AETHER VERSION 1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# -----------------------------------------------------------------------
# MSYS2 UCRT64 search paths
# -----------------------------------------------------------------------
list(APPEND CMAKE_PREFIX_PATH "D:/msys64/ucrt64")

# -----------------------------------------------------------------------
# Find packages
# -----------------------------------------------------------------------
find_package(glfw3   CONFIG REQUIRED)
find_package(glm     CONFIG REQUIRED)
find_package(assimp  CONFIG REQUIRED)
find_package(Freetype       REQUIRED)
find_package(GLUT           REQUIRED)
find_package(OpenGL         REQUIRED)

# stb (header-only)
find_path(STB_INCLUDE_DIR stb_image.h
    HINTS "D:/msys64/ucrt64/include/stb" "D:/msys64/ucrt64/include")
if(STB_INCLUDE_DIR)
    message(STATUS "Found stb: ${STB_INCLUDE_DIR}")
else()
    message(WARNING "stb_image.h not found — Texture.cpp will not compile")
endif()

# -----------------------------------------------------------------------
# GLAD (header + single .c source bundled in the project)
# -----------------------------------------------------------------------
add_library(glad STATIC src/glad/src/gl.c)
target_include_directories(glad PUBLIC src/glad/include)

# -----------------------------------------------------------------------
# AETHER graphics engine (static lib used by main exe)
# -----------------------------------------------------------------------
add_library(aether_engine STATIC
    src/graphics/Shader.cpp
    src/graphics/Camera.cpp
    src/graphics/GameCamera.cpp
    src/graphics/Mesh.cpp
    src/graphics/Renderer.cpp
    src/graphics/LineRenderer.cpp
    src/graphics/UIRenderer.cpp
    src/graphics/Model.cpp
    src/graphics/Texture.cpp
    src/graphics/TextRenderer.cpp
    src/core/Game.cpp
    src/core/ResourceManager.cpp
    src/game/InputManager.cpp
    src/game/Scene.cpp
    src/game/Level.cpp
    src/game/LevelManager.cpp
    src/game/Player.cpp
    src/game/PuzzleObjects.cpp
    src/game/GravitySystem.cpp
    src/game/Physics2D.cpp
    src/game/Enemy.cpp
    src/physics/PhysicsWorld.cpp
)
target_include_directories(aether_engine PUBLIC src)
if(STB_INCLUDE_DIR)
    target_include_directories(aether_engine PRIVATE ${STB_INCLUDE_DIR})
endif()
target_link_libraries(aether_engine
    PUBLIC  glad glfw glm::glm OpenGL::GL
    PRIVATE assimp::assimp Freetype::Freetype
)

# -----------------------------------------------------------------------
# AETHER — main executable
# -----------------------------------------------------------------------
add_executable(AETHER src/main.cpp)
target_include_directories(AETHER PRIVATE src)
target_link_libraries(AETHER PRIVATE aether_engine)
if(STB_INCLUDE_DIR)
    target_include_directories(AETHER PRIVATE ${STB_INCLUDE_DIR})
endif()

# -----------------------------------------------------------------------
# Test libs executable (unchanged)
# -----------------------------------------------------------------------
add_executable(test_libs src/test_libs.cpp)
target_link_libraries(test_libs PRIVATE
    glad glfw GLUT::GLUT glm::glm assimp::assimp Freetype::Freetype OpenGL::GL)
if(STB_INCLUDE_DIR)
    target_include_directories(test_libs PRIVATE ${STB_INCLUDE_DIR})
endif()

# -----------------------------------------------------------------------
# Helper macro — creates a GLUT assignment executable
# -----------------------------------------------------------------------
macro(add_assignment NAME FILE)
    add_executable(${NAME} assignments/${FILE})
    target_include_directories(${NAME} PRIVATE src)
    target_link_libraries(${NAME} PRIVATE
        glad GLUT::GLUT glfw glm::glm OpenGL::GL OpenGL::GLU)
endmacro()

# -----------------------------------------------------------------------
# Original assignment executables
# -----------------------------------------------------------------------
add_assignment(assign_dda              dda.cpp)
add_assignment(assign_bresenham        bresenham.cpp)
add_assignment(assign_bresenham_circle bresenham_circle.cpp)
add_assignment(assign_midpoint_circle  midpoint_circle.cpp)

# -----------------------------------------------------------------------
# New assignment executables
# -----------------------------------------------------------------------
add_assignment(assign_cohen_sutherland  cohen_sutherland.cpp)
add_assignment(assign_liang_barsky      liang_barsky.cpp)
add_assignment(assign_scanline_fill     scanline_fill.cpp)
add_assignment(assign_flood_fill        flood_fill.cpp)
add_assignment(assign_bezier_curve      bezier_curve.cpp)
add_assignment(assign_transformations2d transformations_2d.cpp)
add_assignment(assign_transformations3d transformations_3d.cpp)
add_assignment(assign_transform         transform.cpp)
