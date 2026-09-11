#include "Game.h"
#include "Time.h"
#include "ResourceManager.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// -----------------------------------------------------------------------
// Level name table — authoritative per REBUILD.md §8
// -----------------------------------------------------------------------
static const char* LEVEL_NAMES[7] = {
    "I    THE ARRIVAL HALL",
    "II   THE VERTICAL MAZE",
    "III  THE STORAGE VAULTS",
    "IV   THE RELAY CHAMBERS",
    "V    THE ZERO FIELD",
    "VI   THE MACHINERY RING",
    "VII  THE AETHER CORE"
};

// -----------------------------------------------------------------------
Game::Game()  {}
Game::~Game() {
    delete renderer_;
    delete cubeMesh_;
    delete textRenderer_;
    delete lineRenderer_;
    delete uiRenderer_;
    ResourceManager::clear();
    if (window_) {
        glfwDestroyWindow(window_);
        glfwTerminate();
    }
// -----------------------------------------------------------------------
bool Game::init(int width, int height, const char* title) {
    width_  = width;
    height_ = height;

    if (!glfwInit()) { std::cerr << "Failed to init GLFW\n"; return false; }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window_ = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window_) { std::cerr << "Failed to create window\n"; glfwTerminate(); return false; }

    glfwMakeContextCurrent(window_);
    if (!gladLoadGL(glfwGetProcAddress)) { std::cerr << "Failed to init GLAD\n"; return false; }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    InputManager::init(window_);

    try {
        ResourceManager::getShader("lit", "shaders/lit.vert", "shaders/lit.frag");
    } catch (const std::exception& e) {
        std::cerr << "Failed to load lit shader: " << e.what() << "\n";
        return false;
    }

    try {
        renderer_ = new Renderer("shaders/lit.vert", "shaders/lit.frag");
    } catch (const std::exception& e) {
        std::cerr << "Failed to create Renderer: " << e.what() << "\n";
        return false;
    }

    try {
        textRenderer_ = new TextRenderer(width_, height_);
        textRenderer_->loadFont("assets/fonts/arial.ttf", 24);
    } catch (const std::exception& e) {
        std::cerr << "Failed to load TextRenderer or font: " << e.what() << "\n";
        return false;
    }

    lineRenderer_ = new LineRenderer(width_, height_);
    uiRenderer_   = new UIRenderer(width_, height_);

    cubeMesh_ = new Mesh(Mesh::makeCube());
    levelManager_.sharedCubeMesh = cubeMesh_;

    PointLight sun;
    sun.position = { 5.0f, 10.0f, 15.0f };
    sun.ambient  = { 0.2f, 0.2f, 0.2f };
    sun.diffuse  = { 0.8f, 0.8f, 0.8f };
    renderer_->lights.push_back(sun);

    camera_.isOrtho = false;
    camera_.fov     = 60.0f;
    camera_.position = { 0.0f, 0.0f, 25.0f };

    // Load Kael character model (Temporarily Disabled)
    // if (kaelModel_.load("assets/characters/kael/SK_Kael.glb")) {
    //     std::cout << "[Game] Kael model loaded. Height=" << kaelModel_.height() << std::endl;
    // } else {
    //     std::cerr << "[Game] WARNING: Kael model not found, using cube placeholder.\n";
    // }

    // Register levels
    levelOrder = { "level1","level2","level3","level4","level5","level6","level7" };
    for (const auto& ln : levelOrder)
        levelManager_.levelPaths[ln] = "levels/" + ln + ".txt";

    levelUnlocked.assign(7, false);
    levelUnlocked[0] = true; // Only Level 1 unlocked at start

    currentLevelIndex = 0;

    // Start at SPLASH
    state_      = GameState::SPLASH;
    stateTimer_ = 0.0f;
    menuSel_    = 0;

    std::cout << "Game init SUCCESS\n";
    return true;
}

// -----------------------------------------------------------------------
void Game::run() {
    double lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window_)) {
        double cur = glfwGetTime();
        Time::dt    = std::min((float)(cur - lastTime), 0.05f);
        lastTime    = cur;
        Time::total += Time::dt;

        InputManager::update();
        processInput();
        update(Time::dt);
        render();

        glfwSwapBuffers(window_);
        glfwPollEvents();
    }
}

// -----------------------------------------------------------------------
// STATE DISPATCH
// -----------------------------------------------------------------------
void Game::processInput() {
    switch (state_) {
        case GameState::SPLASH:       inputSplash();      break;
        case GameState::DISCLAIMER:   inputDisclaimer();  break;
        case GameState::MAIN_MENU:    inputMainMenu();    break;
        case GameState::PLAYING:      inputPlaying();     break;
        case GameState::PAUSED:       inputPaused();      break;
        case GameState::LEVEL_SELECT: inputLevelSelect(); break;
        case GameState::SETTINGS:     inputSettings();    break;
        case GameState::CREDITS:      inputCredits();     break;
    }
}

void Game::update(float dt) {
    switch (state_) {
        case GameState::SPLASH:  updateSplash(dt);  break;
        case GameState::PLAYING: updatePlaying(dt); break;
        default: break;
    }
}

void Game::render() {
    // Update screen size in case of resize
    glfwGetFramebufferSize(window_, &width_, &height_);
    if (textRenderer_) {
        // re-set projection for text (simple ortho)
    }

    switch (state_) {
        case GameState::SPLASH:       renderSplash();       break;
        case GameState::DISCLAIMER:   renderDisclaimer();   break;
        case GameState::MAIN_MENU:    renderMainMenu();     break;
        case GameState::PLAYING:      renderPlaying();      break;
        case GameState::PAUSED:       renderPaused();       break;
        case GameState::LEVEL_SELECT: renderLevelSelect();  break;
        case GameState::SETTINGS:     renderSettings();     break;
        case GameState::CREDITS:      renderCredits();      break;
    }
}

// -----------------------------------------------------------------------
// TRANSITION HELPER
// -----------------------------------------------------------------------
void Game::transitionTo(GameState next) {
    prevState_ = state_;
    state_     = next;
    stateTimer_ = 0.0f;
    menuSel_    = 0;
}

// -----------------------------------------------------------------------
// START LEVEL
// -----------------------------------------------------------------------
void Game::startLevel(int index) {
    currentLevelIndex = index;
    levelManager_.loadLevel(levelOrder[index]);
    transitionTo(GameState::PLAYING);
}

// -----------------------------------------------------------------------
// ---- SPLASH ----
// -----------------------------------------------------------------------
void Game::inputSplash() {
    // Any key skips splash
    if (InputManager::isKeyPressed(GLFW_KEY_SPACE) ||
        InputManager::isKeyPressed(GLFW_KEY_ENTER) ||
        InputManager::isKeyPressed(GLFW_KEY_ESCAPE))
        transitionTo(GameState::DISCLAIMER);
}

void Game::updateSplash(float dt) {
    stateTimer_ += dt;
    if (stateTimer_ >= 3.0f)
        transitionTo(GameState::DISCLAIMER);
}

void Game::renderSplash() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float cx = width_  * 0.5f;
    float cy = height_ * 0.5f;

    // Fade-in alpha
    float alpha = std::min(stateTimer_ / 1.0f, 1.0f);
    glm::vec3 col = glm::vec3(alpha);

    textRenderer_->renderText("A E T H E R",          cx - 170.f, cy - 50.f, 2.0f, col * glm::vec3(0.8f, 0.9f, 1.0f));
    textRenderer_->renderText("G R A V I T Y   R E I M A G I N E D", cx - 220.f, cy + 20.f, 0.9f, col * glm::vec3(0.5f, 0.65f, 0.8f));
    textRenderer_->renderText("Press SPACE to continue",              cx - 150.f, cy + 80.f,  0.6f, col * glm::vec3(0.4f, 0.5f, 0.55f));
}

// -----------------------------------------------------------------------
// ---- DISCLAIMER ----
// -----------------------------------------------------------------------
void Game::inputDisclaimer() {
    if (InputManager::isKeyPressed(GLFW_KEY_SPACE) ||
        InputManager::isKeyPressed(GLFW_KEY_ENTER) ||
        InputManager::isKeyPressed(GLFW_KEY_ESCAPE))
        transitionTo(GameState::MAIN_MENU);
}

void Game::renderDisclaimer() {
    glClearColor(0.03f, 0.03f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float cx = width_ * 0.5f;
    float y  = height_ * 0.18f;
    float lh = 36.0f; // line height

    auto line = [&](const char* text, float scale, glm::vec3 c) {
        float tw = (float)strlen(text) * 14.0f * scale;
        textRenderer_->renderText(text, cx - tw * 0.5f, y, scale, c);
        y += lh * scale + 8.0f;
    };

    glm::vec3 head  = {0.7f, 0.8f, 0.9f};
    glm::vec3 body  = {0.55f, 0.6f, 0.65f};
    glm::vec3 faint = {0.35f, 0.4f, 0.45f};

    line("DISCLAIMER", 1.3f, head);
    y += 16.0f;
    line("AETHER — GRAVITY REIMAGINED", 0.8f, body);
    y += 8.0f;
    line("This is an independent student project created for academic purposes.", 0.6f, body);
    line("Computer Graphics & Gaming — Semester 5 Course Project.", 0.6f, body);
    y += 8.0f;
    line("All characters, environments, and story elements are original works", 0.6f, faint);
    line("created for this project.", 0.6f, faint);
    y += 8.0f;
    line("Third-party assets, if any, are used under their respective licenses.", 0.6f, faint);
    line("See docs/ASSET_LICENSES.md for full attribution.", 0.6f, faint);
    y += 16.0f;
    line("Press SPACE or ENTER to continue", 0.65f, glm::vec3(0.4f, 0.5f, 0.55f));
}

// -----------------------------------------------------------------------
// ---- MAIN MENU ----
// -----------------------------------------------------------------------
static const std::vector<std::string> MAIN_MENU_ITEMS = {
    "NEW GAME",
    "CONTINUE",
    "LEVEL SELECT",
    "SETTINGS",
    "CREDITS",
    "EXIT"
};

void Game::inputMainMenu() {
    int n = (int)MAIN_MENU_ITEMS.size();
    if (InputManager::isKeyPressed(GLFW_KEY_UP))   menuSel_ = (menuSel_ - 1 + n) % n;
    if (InputManager::isKeyPressed(GLFW_KEY_DOWN))  menuSel_ = (menuSel_ + 1) % n;

    if (InputManager::isKeyPressed(GLFW_KEY_ENTER) ||
        InputManager::isKeyPressed(GLFW_KEY_SPACE)) {
        switch (menuSel_) {
            case 0: // NEW GAME
                levelUnlocked[0] = true;
                startLevel(0);
                break;
            case 1: // CONTINUE — resume at the highest unlocked level
                for (int i = 6; i >= 0; --i) {
                    if (levelUnlocked[i]) { startLevel(i); break; }
                }
                break;
            case 2: transitionTo(GameState::LEVEL_SELECT); break;
            case 3: transitionTo(GameState::SETTINGS);     break;
            case 4: transitionTo(GameState::CREDITS);      break;
            case 5: glfwSetWindowShouldClose(window_, true); break;
        }
    }
}

void Game::renderMainMenu() {
    // Dark atmospheric background
    glClearColor(0.04f, 0.04f, 0.07f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float cx = (float)width_  * 0.5f;
    float cy = (float)height_ * 0.5f;

    // Title
    textRenderer_->renderText("A E T H E R",
        cx - 165.f, cy - 210.f, 1.8f, {0.7f, 0.85f, 1.0f});
    textRenderer_->renderText("GRAVITY REIMAGINED",
        cx - 120.f, cy - 155.f, 0.8f, {0.4f, 0.55f, 0.7f});

    // Thin separator
    uiRenderer_->beginFrame(width_, height_);
    uiRenderer_->drawRect(cx - 150.f, cy - 128.f, 300.f, 2.f, {0.25f, 0.4f, 0.6f, 0.8f});
    uiRenderer_->endFrame();

    // Menu items
    float itemY   = cy - 100.f;
    float itemH   = 44.0f;

    for (int i = 0; i < (int)MAIN_MENU_ITEMS.size(); ++i) {
        bool sel = (i == menuSel_);

        // Highlight bar
        if (sel) {
            uiRenderer_->beginFrame(width_, height_);
            uiRenderer_->drawRect(cx - 160.f, itemY - 6.f, 320.f, 38.f,
                                  {0.15f, 0.3f, 0.55f, 0.75f});
            uiRenderer_->endFrame();
        }

        glm::vec3 col = sel ? glm::vec3(0.8f, 0.92f, 1.0f)
                             : glm::vec3(0.45f, 0.55f, 0.65f);
        float scale   = sel ? 0.85f : 0.75f;
        const std::string& label = MAIN_MENU_ITEMS[i];
        float tw = (float)label.size() * 13.0f * scale;
        textRenderer_->renderText(label, cx - tw * 0.5f, itemY, scale, col);
        itemY += itemH;
    }

    // Hint
    textRenderer_->renderText("UP / DOWN   NAVIGATE       ENTER   SELECT",
        cx - 210.f, (float)height_ - 40.f, 0.5f, {0.25f, 0.35f, 0.45f});
}

// -----------------------------------------------------------------------
// ---- PLAYING ----
// -----------------------------------------------------------------------
void Game::inputPlaying() {
    // ESC = Pause (not quit)
    if (InputManager::isKeyPressed(GLFW_KEY_ESCAPE))
        transitionTo(GameState::PAUSED);

    // Pass all other input to the level
    // (player update happens in updatePlaying via levelManager_.update)
}

void Game::updatePlaying(float dt) {
    levelManager_.update(dt);

    if (levelManager_.currentLevel) {
        // Level complete → advance
        if (levelManager_.currentLevel->isComplete) {
            levelUnlocked[std::min(currentLevelIndex + 1, 6)] = true;
            currentLevelIndex++;
            if (currentLevelIndex < (int)levelOrder.size()) {
                startLevel(currentLevelIndex);
            } else {
                // Game complete → credits
                transitionTo(GameState::CREDITS);
            }
        }

        // Camera
        if (levelManager_.currentLevel->player.bodyId != -1) {
            glm::vec2 pPos = levelManager_.currentLevel->physics
                                 .body(levelManager_.currentLevel->player.bodyId).pos;
            camera_.update(dt, pPos, levelManager_.currentLevel->gravity);
        }
    }
}

void Game::renderPlaying() {
    Renderer::clear(0.05f, 0.05f, 0.08f, 1.0f);

    float aspect = (float)width_ / (float)height_;
    renderer_->begin(camera_.getViewMatrix(),
                     camera_.getProjectionMatrix(aspect),
                     camera_.position);
    levelManager_.draw(*renderer_, camera_);
    renderer_->end();

    // Level name HUD (top-left)
    if (currentLevelIndex < 7) {
        std::string lname = std::string("LEVEL ") + LEVEL_NAMES[currentLevelIndex];
        textRenderer_->renderText(lname, 20.f, 20.f, 0.55f, {0.5f, 0.65f, 0.8f});
    }

    // Controls hint (first few seconds)
    if (Time::total < 8.0f) {
        textRenderer_->renderText("ARROW KEYS  Move    SPACE  Jump",
            20.f, (float)height_ - 60.f, 0.5f, {0.4f, 0.5f, 0.55f});
        textRenderer_->renderText("W/S/A/D  Set Gravity Direction   ESC  Pause",
            20.f, (float)height_ - 35.f, 0.5f, {0.4f, 0.5f, 0.55f});
    }

    // Text hints placed in the level
    if (textRenderer_ && levelManager_.currentLevel) {
        glm::mat4 view = camera_.getViewMatrix();
        glm::mat4 proj = camera_.getProjectionMatrix(aspect);
        for (const auto& th : levelManager_.currentLevel->texts)
            textRenderer_->renderTextWorld(th.text, th.pos.x, th.pos.y,
                                           0.5f, {1.f,1.f,1.f}, proj, view);

        // --- Draw Kael model at player's physics position ---
        if (kaelModel_.loaded() && levelManager_.currentLevel->player.bodyId != -1) {
            const Body2D& pb = levelManager_.currentLevel->physics
                                   .body(levelManager_.currentLevel->player.bodyId);

            // Scale model to match physics body (physics body half.y = 0.9 → height 1.8)
            float modelH = kaelModel_.height();
            float scale  = (modelH > 0.01f) ? (1.8f / modelH) : 1.0f;

            // Gravity rotation from GravitySystem::angle()
            float gravAngle = levelManager_.currentLevel->gravity.angle();

            glm::mat4 kaelM = glm::translate(glm::mat4(1.0f),
                                              glm::vec3(pb.pos.x, pb.pos.y, 0.0f));
            // Rotate Kael to stand upright relative to current gravity
            if (std::abs(gravAngle) > 0.001f)
                kaelM = glm::rotate(kaelM, gravAngle, glm::vec3(0.0f, 0.0f, 1.0f));
            // Offset pivot to feet (bMin.y is the negative half-height)
            float bottomOffset = kaelModel_.boundsMin().y * scale;
            kaelM = glm::translate(kaelM, glm::vec3(0.0f, -bottomOffset, 0.0f));
            kaelM = glm::scale(kaelM, glm::vec3(scale));

            // Draw using the lit shader (begin() already called by levelManager_.draw)
            kaelModel_.draw(renderer_->shaderProgramId(), kaelM);
        }

        // DDA — Sentinel patrol paths (orange)
        for (const auto& e : levelManager_.currentLevel->enemies) {
            if (e.type == EnemyType::SENTINEL) {
                glm::vec4 cA = proj * view * glm::vec4(e.patrolA.x, e.patrolA.y, 0.f, 1.f);
                glm::vec4 cB = proj * view * glm::vec4(e.patrolB.x, e.patrolB.y, 0.f, 1.f);
                int x1 = (int)((cA.x / cA.w * 0.5f + 0.5f) * width_);
                int y1 = (int)((cA.y / cA.w * 0.5f + 0.5f) * height_);
                int x2 = (int)((cB.x / cB.w * 0.5f + 0.5f) * width_);
                int y2 = (int)((cB.y / cB.w * 0.5f + 0.5f) * height_);
                lineRenderer_->drawDDALine(x1, y1, x2, y2, {0.9f, 0.35f, 0.1f});
            }
        }

        // Bresenham — Gravity field center lines (cyan)
        for (const auto& f : levelManager_.currentLevel->fields) {
            glm::vec4 cC = proj * view * glm::vec4(f.pos.x, f.pos.y, 0.f, 1.f);
            glm::vec4 cT = proj * view * glm::vec4(f.pos.x, f.pos.y + f.half.y, 0.f, 1.f);
            int x1 = (int)((cC.x / cC.w * 0.5f + 0.5f) * width_);
            int y1 = (int)((cC.y / cC.w * 0.5f + 0.5f) * height_);
            int x2 = (int)((cT.x / cT.w * 0.5f + 0.5f) * width_);
            int y2 = (int)((cT.y / cT.w * 0.5f + 0.5f) * height_);
            lineRenderer_->drawBresenhamLine(x1, y1, x2, y2, {0.3f, 0.85f, 0.95f});
        }

        lineRenderer_->renderAll();
    }
}

// -----------------------------------------------------------------------
// ---- PAUSED ----
// -----------------------------------------------------------------------
static const std::vector<std::string> PAUSE_ITEMS = {
    "RESUME",
    "RESTART LEVEL",
    "SETTINGS",
    "MAIN MENU"
};

void Game::inputPaused() {
    int n = (int)PAUSE_ITEMS.size();
    if (InputManager::isKeyPressed(GLFW_KEY_UP))   menuSel_ = (menuSel_ - 1 + n) % n;
    if (InputManager::isKeyPressed(GLFW_KEY_DOWN))  menuSel_ = (menuSel_ + 1) % n;
    if (InputManager::isKeyPressed(GLFW_KEY_ESCAPE)) transitionTo(GameState::PLAYING);

    if (InputManager::isKeyPressed(GLFW_KEY_ENTER) ||
        InputManager::isKeyPressed(GLFW_KEY_SPACE)) {
        switch (menuSel_) {
            case 0: transitionTo(GameState::PLAYING); break;
            case 1:
                levelManager_.restartLevel();
                transitionTo(GameState::PLAYING);
                break;
            case 2: transitionTo(GameState::SETTINGS); break;
            case 3: transitionTo(GameState::MAIN_MENU); break;
        }
    }
}

void Game::renderPaused() {
    // Draw the game in the background (no update)
    renderPlaying();
    drawMenuOverlay("PAUSED", PAUSE_ITEMS, menuSel_, 0.80f);
}

// -----------------------------------------------------------------------
// ---- LEVEL SELECT ----
// -----------------------------------------------------------------------
void Game::inputLevelSelect() {
    if (InputManager::isKeyPressed(GLFW_KEY_ESCAPE)) {
        transitionTo(GameState::MAIN_MENU);
        return;
    }
    if (InputManager::isKeyPressed(GLFW_KEY_UP))
        levelSelectSel_ = std::max(0, levelSelectSel_ - 1);
    if (InputManager::isKeyPressed(GLFW_KEY_DOWN))
        levelSelectSel_ = std::min(6, levelSelectSel_ + 1);

    if (InputManager::isKeyPressed(GLFW_KEY_ENTER) ||
        InputManager::isKeyPressed(GLFW_KEY_SPACE)) {
        if (levelUnlocked[levelSelectSel_])
            startLevel(levelSelectSel_);
    }
}

void Game::renderLevelSelect() {
    glClearColor(0.04f, 0.04f, 0.07f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float cx = (float)width_ * 0.5f;
    float y  = (float)height_ * 0.12f;

    textRenderer_->renderText("SELECT LEVEL", cx - 130.f, y, 1.2f, {0.7f, 0.85f, 1.0f});
    y += 80.f;

    for (int i = 0; i < 7; ++i) {
        bool sel     = (i == levelSelectSel_);
        bool unlocked = levelUnlocked[i];

        if (sel) {
            uiRenderer_->beginFrame(width_, height_);
            uiRenderer_->drawRect(cx - 270.f, y - 5.f, 540.f, 38.f,
                                  {0.12f, 0.25f, 0.5f, 0.75f});
            uiRenderer_->endFrame();
        }

        glm::vec3 col = !unlocked ? glm::vec3(0.25f, 0.3f, 0.35f)
                      : sel       ? glm::vec3(0.85f, 0.95f, 1.0f)
                                  : glm::vec3(0.5f, 0.62f, 0.72f);

        std::string label = std::string(LEVEL_NAMES[i]);
        if (!unlocked) label += "  [ LOCKED ]";
        float sc = sel ? 0.75f : 0.68f;
        textRenderer_->renderText(label, cx - 240.f, y, sc, col);
        y += 48.f;
    }

    textRenderer_->renderText("UP/DOWN  Navigate    ENTER  Play    ESC  Back",
        cx - 225.f, (float)height_ - 40.f, 0.5f, {0.25f, 0.35f, 0.45f});
}

// -----------------------------------------------------------------------
// ---- SETTINGS ----
// -----------------------------------------------------------------------
static const std::vector<std::string> SETTINGS_ITEMS = {
    "Resolution   :  1280 x 720",
    "Fullscreen   :  OFF",
    "VSync        :  ON",
    "Quality      :  HIGH",
    "Master Vol   :  100",
    "Music Vol    :  80",
    "SFX Vol      :  100",
    "",
    "-- Controls --",
    "Move         :  ARROW KEYS",
    "Jump         :  SPACE",
    "Gravity UP   :  W",
    "Gravity DOWN :  S",
    "Gravity LEFT :  A",
    "Gravity RIGHT:  D",
    "Interact     :  E",
    "Pause        :  ESC",
    "",
    "BACK"
};

void Game::inputSettings() {
    int n = (int)SETTINGS_ITEMS.size();
    if (InputManager::isKeyPressed(GLFW_KEY_UP))   settingsSel_ = (settingsSel_ - 1 + n) % n;
    if (InputManager::isKeyPressed(GLFW_KEY_DOWN))  settingsSel_ = (settingsSel_ + 1) % n;

    if (InputManager::isKeyPressed(GLFW_KEY_ESCAPE) ||
        (InputManager::isKeyPressed(GLFW_KEY_ENTER) &&
         SETTINGS_ITEMS[settingsSel_] == "BACK")) {
        transitionTo(prevState_);
    }
}

void Game::renderSettings() {
    glClearColor(0.04f, 0.04f, 0.07f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float cx = (float)width_ * 0.5f;
    float y  = (float)height_ * 0.06f;

    textRenderer_->renderText("SETTINGS", cx - 80.f, y, 1.1f, {0.7f, 0.85f, 1.0f});
    y += 70.f;

    for (int i = 0; i < (int)SETTINGS_ITEMS.size(); ++i) {
        const std::string& item = SETTINGS_ITEMS[i];
        if (item.empty()) { y += 14.f; continue; }

        bool sel = (i == settingsSel_);
        if (sel && !item.empty()) {
            uiRenderer_->beginFrame(width_, height_);
            uiRenderer_->drawRect(cx - 280.f, y - 4.f, 560.f, 30.f,
                                  {0.12f, 0.25f, 0.5f, 0.6f});
            uiRenderer_->endFrame();
        }

        bool isHeader = (item.size() > 2 && item[0] == '-' && item[1] == '-');
        glm::vec3 col = isHeader ? glm::vec3(0.45f, 0.6f, 0.75f)
                      : sel     ? glm::vec3(0.9f, 0.97f, 1.0f)
                                : glm::vec3(0.5f, 0.6f, 0.68f);
        float sc = isHeader ? 0.62f : (sel ? 0.65f : 0.6f);
        float tw = (float)item.size() * 13.f * sc;
        textRenderer_->renderText(item, cx - tw * 0.5f, y, sc, col);
        y += 32.f;
    }
}

// -----------------------------------------------------------------------
// ---- CREDITS ----
// -----------------------------------------------------------------------
void Game::inputCredits() {
    if (InputManager::isKeyPressed(GLFW_KEY_ESCAPE) ||
        InputManager::isKeyPressed(GLFW_KEY_ENTER) ||
        InputManager::isKeyPressed(GLFW_KEY_SPACE))
        transitionTo(GameState::MAIN_MENU);
}

void Game::renderCredits() {
    glClearColor(0.03f, 0.03f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float cx = (float)width_ * 0.5f;
    float y  = (float)height_ * 0.10f;
    float lh = 38.f;

    auto ln = [&](const char* text, float scale, glm::vec3 col) {
        float tw = (float)strlen(text) * 13.f * scale;
        textRenderer_->renderText(text, cx - tw * 0.5f, y, scale, col);
        y += lh * scale + 4.f;
    };

    ln("AETHER — GRAVITY REIMAGINED", 1.1f, {0.7f, 0.87f, 1.0f});
    y += 10.f;
    ln("Computer Graphics & Gaming — Semester 5", 0.68f, {0.45f, 0.57f, 0.68f});
    y += 20.f;
    ln("DEVELOPMENT", 0.78f, {0.55f, 0.72f, 0.85f});
    y += 6.f;
    ln("Mayuresh  —  Developer / Game Designer", 0.65f, {0.5f, 0.62f, 0.7f});
    y += 20.f;
    ln("ENGINE", 0.78f, {0.55f, 0.72f, 0.85f});
    y += 6.f;
    ln("Custom C++ / OpenGL rendering engine", 0.65f, {0.5f, 0.62f, 0.7f});
    ln("Physics: custom AABB + Euler integration", 0.65f, {0.5f, 0.62f, 0.7f});
    ln("CG Algorithms: DDA, Bresenham, Blinn-Phong", 0.65f, {0.5f, 0.62f, 0.7f});
    y += 20.f;
    ln("LIBRARIES", 0.78f, {0.55f, 0.72f, 0.85f});
    y += 6.f;
    ln("OpenGL  |  GLFW  |  GLAD  |  GLM  |  FreeType", 0.65f, {0.5f, 0.62f, 0.7f});
    y += 20.f;
    ln("Press ENTER or ESC to return", 0.6f, {0.3f, 0.4f, 0.48f});
}

// -----------------------------------------------------------------------
// ---- SHARED OVERLAY HELPER ----
// -----------------------------------------------------------------------
void Game::drawMenuOverlay(const char* title,
                           const std::vector<std::string>& items,
                           int selected,
                           float bgAlpha)
{
    float cx = (float)width_  * 0.5f;
    float cy = (float)height_ * 0.5f;

    float panelW = 380.f;
    float panelH = 80.f + items.size() * 50.f;
    float px     = cx - panelW * 0.5f;
    float py     = cy - panelH * 0.5f;

    // Semi-transparent backdrop
    uiRenderer_->beginFrame(width_, height_);
    uiRenderer_->drawRect(0.f, 0.f, (float)width_, (float)height_,
                          {0.0f, 0.0f, 0.05f, bgAlpha * 0.6f});
    uiRenderer_->drawRect(px, py, panelW, panelH, {0.05f, 0.08f, 0.14f, bgAlpha});
    uiRenderer_->drawRect(px, py, panelW, 3.f,    {0.3f, 0.55f, 0.85f, 1.0f});
    uiRenderer_->endFrame();

    // Title
    float tw = (float)strlen(title) * 16.f;
    textRenderer_->renderText(title, cx - tw * 0.5f, py + 18.f, 1.1f, {0.75f, 0.9f, 1.0f});

    // Items
    float itemY = py + 72.f;
    for (int i = 0; i < (int)items.size(); ++i) {
        bool sel = (i == selected);
        if (sel) {
            uiRenderer_->beginFrame(width_, height_);
            uiRenderer_->drawRect(px + 10.f, itemY - 5.f, panelW - 20.f, 36.f,
                                  {0.15f, 0.3f, 0.58f, 0.75f});
            uiRenderer_->endFrame();
        }
        glm::vec3 col = sel ? glm::vec3(0.85f, 0.95f, 1.0f) : glm::vec3(0.48f, 0.6f, 0.7f);
        float sc      = sel ? 0.8f : 0.72f;
        float iw      = (float)items[i].size() * 13.f * sc;
        textRenderer_->renderText(items[i], cx - iw * 0.5f, itemY, sc, col);
        itemY += 48.f;
    }
}
