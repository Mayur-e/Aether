#include "InputManager.h"

#include <cstring>

// -----------------------------------------------------------------------
// Static member definitions
// -----------------------------------------------------------------------
GLFWwindow* InputManager::window_     = nullptr;
double      InputManager::lastX_      = 0.0;
double      InputManager::lastY_      = 0.0;
double      InputManager::deltaX_     = 0.0;
double      InputManager::deltaY_     = 0.0;
bool        InputManager::firstMouse_ = true;
bool        InputManager::captured_   = false;
bool        InputManager::prevKeys_[GLFW_KEY_LAST + 1] = {};
bool        InputManager::currKeys_[GLFW_KEY_LAST + 1] = {};

// -----------------------------------------------------------------------
void InputManager::init(GLFWwindow* window)
{
    window_ = window;
    std::memset(prevKeys_, 0, sizeof(prevKeys_));
    std::memset(currKeys_, 0, sizeof(currKeys_));
    glfwSetCursorPosCallback(window_, cursorCallback);
}

// -----------------------------------------------------------------------
void InputManager::update()
{
    // Snapshot key states
    std::memcpy(prevKeys_, currKeys_, sizeof(currKeys_));
    for (int k = 0; k <= GLFW_KEY_LAST; ++k)
        currKeys_[k] = (glfwGetKey(window_, k) == GLFW_PRESS);

    // Reset per-frame mouse delta (callback will fill it)
    deltaX_ = 0.0;
    deltaY_ = 0.0;
    glfwPollEvents();
}

// -----------------------------------------------------------------------
bool InputManager::isKeyHeld(int key)
{
    if (key < 0 || key > GLFW_KEY_LAST) return false;
    return currKeys_[key];
}

bool InputManager::isKeyPressed(int key)
{
    if (key < 0 || key > GLFW_KEY_LAST) return false;
    return currKeys_[key] && !prevKeys_[key];
}

bool InputManager::isKeyReleased(int key)
{
    if (key < 0 || key > GLFW_KEY_LAST) return false;
    return !currKeys_[key] && prevKeys_[key];
}

// -----------------------------------------------------------------------
glm::vec2 InputManager::getMouseDelta()
{
    return { (float)deltaX_, (float)deltaY_ };
}

bool InputManager::isMouseButtonHeld(int button)
{
    return glfwGetMouseButton(window_, button) == GLFW_PRESS;
}

// -----------------------------------------------------------------------
void InputManager::setCaptureMouse(bool capture)
{
    captured_ = capture;
    firstMouse_ = true;
    glfwSetInputMode(window_, GLFW_CURSOR,
                     capture ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

bool InputManager::isMouseCaptured()
{
    return captured_;
}

// -----------------------------------------------------------------------
void InputManager::cursorCallback(GLFWwindow*, double xpos, double ypos)
{
    if (!captured_) return;

    if (firstMouse_)
    {
        lastX_      = xpos;
        lastY_      = ypos;
        firstMouse_ = false;
    }

    deltaX_ += xpos - lastX_;
    deltaY_ += lastY_ - ypos;   // reversed: y grows downward in screen space
    lastX_   = xpos;
    lastY_   = ypos;
}
