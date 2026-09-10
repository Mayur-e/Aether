#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Movement directions (avoids relying on GLFW here)
enum class CameraMovement { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN };

// -----------------------------------------------------------------------
// Camera — Euler-angle free-fly camera.
//
// Usage:
//   Camera cam(glm::vec3(0,1,3));
//   cam.processKeyboard(CameraMovement::FORWARD, deltaTime);
//   cam.processMouseMovement(xOffset, yOffset);
//   shader.setMat4("view", cam.getViewMatrix());
// -----------------------------------------------------------------------
class Camera
{
public:
    // Camera state
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    // Euler angles (degrees)
    float yaw;
    float pitch;

    // Movement tuning
    float movementSpeed;
    float mouseSensitivity;
    float fov;           // field-of-view in degrees

    // Default constructor
    explicit Camera(glm::vec3 position  = glm::vec3(0.0f, 1.0f, 5.0f),
                    glm::vec3 worldUp   = glm::vec3(0.0f, 1.0f, 0.0f),
                    float     yaw       = -90.0f,
                    float     pitch     = 0.0f);

    // Build the view matrix
    glm::mat4 getViewMatrix() const;

    // Build projection matrix (pass window aspect ratio)
    glm::mat4 getProjectionMatrix(float aspect,
                                  float nearPlane = 0.1f,
                                  float farPlane  = 100.0f) const;

    // Keyboard: WASD + EQ movement
    void processKeyboard(CameraMovement direction, float deltaTime);

    // Mouse look (offset since last frame, pixels)
    void processMouseMovement(float xOffset, float yOffset,
                              bool constrainPitch = true);

    // Scroll zoom
    void processMouseScroll(float yOffset);

private:
    void updateVectors();
};
