#include "Game.h"
#include "Time.h"
#include "ResourceManager.h"
#include <iostream>

Game::Game() {}

Game::~Game() {
    delete renderer_;
    delete cubeMesh_;
    delete textRenderer_;
    delete lineRenderer_;
    ResourceManager::clear();
    if (window_) {
        glfwDestroyWindow(window_);
        glfwTerminate();
    }
}

bool Game::init(int width, int height, const char* title) {
    width_ = width;
    height_ = height;
    
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window_ = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window_) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window_);
    
    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return false;
    }
    
    // Enable Depth Test
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    InputManager::init(window_);
    
    // Initialize graphics resources
    // Use the lit shaders from the existing demo
    ResourceManager::getShader("lit", "shaders/lit.vert", "shaders/lit.frag");
    renderer_ = new Renderer("shaders/lit.vert", "shaders/lit.frag");
    
    textRenderer_ = new TextRenderer(width_, height_);
    textRenderer_->loadFont("assets/fonts/arial.ttf", 24);
    
    lineRenderer_ = new LineRenderer(width_, height_);
    
    cubeMesh_ = new Mesh(Mesh::makeCube());
    levelManager_.sharedCubeMesh = cubeMesh_;
    
    // Setup light for renderer
    PointLight sun;
    sun.position = { 5.0f, 10.0f, 15.0f };
    sun.ambient = { 0.2f, 0.2f, 0.2f };
    sun.diffuse = { 0.8f, 0.8f, 0.8f };
    renderer_->lights.push_back(sun);
    
    camera_.isOrtho = false;
    camera_.fov = 60.0f;
    camera_.position = { 0.0f, 0.0f, 25.0f };

    // Register levels
    levelOrder = { "level1", "level2", "level3", "level4", "level5", "level6", "level7" };
    for (const auto& ln : levelOrder) {
        levelManager_.levelPaths[ln] = "levels/" + ln + ".txt";
    }

    currentLevelIndex = 0;
    // Load first level
    levelManager_.loadLevel(levelOrder[currentLevelIndex]);

    return true;
}

void Game::run() {
    double lastTime = glfwGetTime();
    
    while (!glfwWindowShouldClose(window_)) {
        double currentFrame = glfwGetTime();
        Time::dt = (float)(currentFrame - lastTime);
        lastTime = currentFrame;
        
        // Clamp dt to avoid huge steps on lag spikes
        if (Time::dt > 0.05f) Time::dt = 0.05f;
        Time::total += Time::dt;

        InputManager::update();
        processInput();
        
        update(Time::dt);
        render();

        glfwSwapBuffers(window_);
        glfwPollEvents();
    }
}

void Game::processInput() {
    if (InputManager::isKeyPressed(GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(window_, true);
    }
    if (InputManager::isKeyPressed(GLFW_KEY_R)) {
        levelManager_.restartLevel();
    }
}

void Game::update(float dt) {
    levelManager_.update(dt);
    
    if (levelManager_.currentLevel) {
        if (levelManager_.currentLevel->isComplete) {
            currentLevelIndex++;
            if (currentLevelIndex < levelOrder.size()) {
                levelManager_.loadLevel(levelOrder[currentLevelIndex]);
            } else {
                std::cout << "GAME COMPLETE!\n";
                // Go back to main menu or restart
                currentLevelIndex = 0;
                levelManager_.loadLevel(levelOrder[currentLevelIndex]);
            }
        }
    }
    
    if (levelManager_.currentLevel && levelManager_.currentLevel->player.bodyId != -1) {
        glm::vec2 pPos = levelManager_.currentLevel->physics.body(levelManager_.currentLevel->player.bodyId).pos;
        camera_.update(dt, pPos, levelManager_.currentLevel->gravity);
    }
}

void Game::render() {
    Renderer::clear(0.05f, 0.05f, 0.08f, 1.0f); // Dark background
    
    float aspect = (float)width_ / (float)height_;
    
    renderer_->begin(camera_.getViewMatrix(), camera_.getProjectionMatrix(aspect), camera_.position);
    
    levelManager_.draw(*renderer_, camera_);
    
    renderer_->end();

    if (textRenderer_ && levelManager_.currentLevel) {
        glm::mat4 view = camera_.getViewMatrix();
        glm::mat4 proj = camera_.getProjectionMatrix(aspect);
        
        // Render texts
        for (const auto& th : levelManager_.currentLevel->texts) {
            textRenderer_->renderTextWorld(th.text, th.pos.x, th.pos.y, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f), proj, view);
        }

        // Draw Sentinel Patrol Paths using DDA
        for (const auto& e : levelManager_.currentLevel->enemies) {
            if (e.type == EnemyType::SENTINEL) {
                glm::vec4 cA = proj * view * glm::vec4(e.patrolA.x, e.patrolA.y, 0.0f, 1.0f);
                glm::vec4 cB = proj * view * glm::vec4(e.patrolB.x, e.patrolB.y, 0.0f, 1.0f);
                
                // Screen coordinates
                int x1 = (int)((cA.x / cA.w * 0.5f + 0.5f) * width_);
                int y1 = (int)((cA.y / cA.w * 0.5f + 0.5f) * height_);
                int x2 = (int)((cB.x / cB.w * 0.5f + 0.5f) * width_);
                int y2 = (int)((cB.y / cB.w * 0.5f + 0.5f) * height_);
                
                lineRenderer_->drawDDALine(x1, y1, x2, y2, glm::vec3(0.9f, 0.3f, 0.1f)); // Orange
            }
        }

        // Draw Gravity Field connections/centers using Bresenham
        for (const auto& f : levelManager_.currentLevel->fields) {
            glm::vec4 cCenter = proj * view * glm::vec4(f.pos.x, f.pos.y, 0.0f, 1.0f);
            glm::vec4 cTop    = proj * view * glm::vec4(f.pos.x, f.pos.y + f.half.y, 0.0f, 1.0f);
            
            int x1 = (int)((cCenter.x / cCenter.w * 0.5f + 0.5f) * width_);
            int y1 = (int)((cCenter.y / cCenter.w * 0.5f + 0.5f) * height_);
            int x2 = (int)((cTop.x / cTop.w * 0.5f + 0.5f) * width_);
            int y2 = (int)((cTop.y / cTop.w * 0.5f + 0.5f) * height_);
            
            lineRenderer_->drawBresenhamLine(x1, y1, x2, y2, glm::vec3(0.3f, 0.8f, 0.9f)); // Cyan
        }

        lineRenderer_->renderAll();
    }
}
