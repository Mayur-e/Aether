#pragma once
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "../graphics/Renderer.h"
#include "../graphics/GameCamera.h"
#include "../game/InputManager.h"
#include "../game/LevelManager.h"
#include "../graphics/Mesh.h"
#include "../graphics/TextRenderer.h"
#include "../graphics/LineRenderer.h"

// -----------------------------------------------------------------------
// Game class — manages the core loop, window, and high-level state.
// -----------------------------------------------------------------------
class Game {
public:
    Game();
    ~Game();

    bool init(int width, int height, const char* title);
    void run();

private:
    GLFWwindow*  window_ = nullptr;
    int          width_, height_;
    
    Renderer*    renderer_ = nullptr;
    TextRenderer* textRenderer_ = nullptr;
    LineRenderer* lineRenderer_ = nullptr;
    GameCamera   camera_;
    InputManager inputManager_;
    LevelManager levelManager_;
    
    int currentLevelIndex = 0;
    std::vector<std::string> levelOrder;

    Mesh*        cubeMesh_ = nullptr; // Shared primitive

    void processInput();
    void update(float dt);
    void render();
};
