#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// -----------------------------------------------------------------------
// InputManager — thin static wrapper around GLFW input.
//
// Call InputManager::init(window) once after window creation.
// Each frame call InputManager::update() to snapshot the mouse delta.
//
// Usage:
//   InputManager::init(window);
//   ...
//   if (InputManager::isKeyHeld(GLFW_KEY_W)) ...
//   glm::vec2 d = InputManager::getMouseDelta();
// -----------------------------------------------------------------------
class InputManager
{
public:
    // Must be called once with the GLFW window
    static void init(GLFWwindow* window);

    // Call at the START of each frame (updates mouse delta + captures cursor)
    static void update();

    // Keyboard
    static bool isKeyHeld   (int glfwKey);
    static bool isKeyPressed (int glfwKey);   // true only on first frame held
    static bool isKeyReleased(int glfwKey);   // true only on release frame

    // Mouse
    static glm::vec2 getMouseDelta();         // pixels moved since last update()
    static bool      isMouseButtonHeld(int glfwButton);

    // Toggle cursor capture (true = FPS mode, cursor hidden + centred)
    static void setCaptureMouse(bool capture);
    static bool isMouseCaptured();

private:
    static GLFWwindow* window_;
    static double      lastX_, lastY_;
    static double      deltaX_, deltaY_;
    static bool        firstMouse_;
    static bool        captured_;
    static bool        prevKeys_[GLFW_KEY_LAST + 1];
    static bool        currKeys_[GLFW_KEY_LAST + 1];

    // GLFW callbacks
    static void cursorCallback(GLFWwindow*, double xpos, double ypos);
};
