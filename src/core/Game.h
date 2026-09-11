#pragma once
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "../graphics/Renderer.h"
#include "../graphics/GameCamera.h"
#include "../graphics/UIRenderer.h"
#include "../game/InputManager.h"
#include "../game/LevelManager.h"
#include "../graphics/Mesh.h"
#include "../graphics/TextRenderer.h"
#include "../graphics/LineRenderer.h"
#include "../graphics/Model.h"
#include <string>
#include <vector>

// -----------------------------------------------------------------------
// GameState — authoritative application states per REBUILD.md §16
// -----------------------------------------------------------------------
enum class GameState {
    SPLASH,
    DISCLAIMER,
    MAIN_MENU,
    PLAYING,
    PAUSED,
    LEVEL_SELECT,
    SETTINGS,
    CREDITS
};

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
    // Window
    GLFWwindow*  window_  = nullptr;
    int          width_   = 1280;
    int          height_  = 720;

    // Rendering
    Renderer*     renderer_     = nullptr;
    TextRenderer* textRenderer_ = nullptr;
    LineRenderer* lineRenderer_ = nullptr;
    UIRenderer*   uiRenderer_   = nullptr;
    GameCamera    camera_;
    Mesh*         cubeMesh_     = nullptr;
    Model         kaelModel_;            // Kael player character model

    // Game systems
    InputManager inputManager_;
    LevelManager levelManager_;

    // Level ordering
    int                      currentLevelIndex = 0;
    std::vector<std::string> levelOrder;
    std::vector<bool>        levelUnlocked;  // per-level unlock state

    // ---- State machine ----
    GameState state_       = GameState::SPLASH;
    GameState prevState_   = GameState::SPLASH; // used for Settings back-nav
    float     stateTimer_  = 0.0f;             // used for Splash auto-advance

    // Menu navigation
    int menuSel_       = 0;  // current highlighted menu item
    int settingsSel_   = 0;
    int levelSelectSel_= 0;

    // ---- State dispatch ----
    void processInput();
    void update(float dt);
    void render();

    // Per-state input
    void inputSplash();
    void inputDisclaimer();
    void inputMainMenu();
    void inputPlaying();
    void inputPaused();
    void inputLevelSelect();
    void inputSettings();
    void inputCredits();

    // Per-state update
    void updateSplash(float dt);
    void updatePlaying(float dt);

    // Per-state render
    void renderSplash();
    void renderDisclaimer();
    void renderMainMenu();
    void renderPlaying();
    void renderPaused();
    void renderLevelSelect();
    void renderSettings();
    void renderCredits();

    // Helpers
    void drawMenuOverlay(const char* title,
                         const std::vector<std::string>& items,
                         int selected,
                         float bgAlpha = 0.85f);
    void startLevel(int index);
    void transitionTo(GameState next);
};
