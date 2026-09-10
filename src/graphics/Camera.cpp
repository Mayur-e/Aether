#include "Camera.h"

#include <algorithm>

// -----------------------------------------------------------------------
Camera::Camera(glm::vec3 position, glm::vec3 worldUp, float yaw, float pitch)
    : position(position)
    , worldUp(worldUp)
    , yaw(yaw)
    , pitch(pitch)
    , front(glm::vec3(0.0f, 0.0f, -1.0f))
    , movementSpeed(5.0f)
    , mouseSensitivity(0.1f)
    , fov(45.0f)
{
    updateVectors();
}

// -----------------------------------------------------------------------
glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getProjectionMatrix(float aspect, float nearPlane, float farPlane) const
{
    return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
}

// -----------------------------------------------------------------------
void Camera::processKeyboard(CameraMovement direction, float deltaTime)
{
    float velocity = movementSpeed * deltaTime;

    if (direction == CameraMovement::FORWARD)  position += front * velocity;
    if (direction == CameraMovement::BACKWARD) position -= front * velocity;
    if (direction == CameraMovement::LEFT)     position -= right * velocity;
    if (direction == CameraMovement::RIGHT)    position += right * velocity;
    if (direction == CameraMovement::UP)       position += worldUp * velocity;
    if (direction == CameraMovement::DOWN)     position -= worldUp * velocity;
}

void Camera::processMouseMovement(float xOffset, float yOffset, bool constrainPitch)
{
    xOffset *= mouseSensitivity;
    yOffset *= mouseSensitivity;

    yaw   += xOffset;
    pitch += yOffset;

    if (constrainPitch)
        pitch = std::clamp(pitch, -89.0f, 89.0f);

    updateVectors();
}

void Camera::processMouseScroll(float yOffset)
{
    fov = std::clamp(fov - yOffset, 1.0f, 90.0f);
}

// -----------------------------------------------------------------------
void Camera::updateVectors()
{
    glm::vec3 f;
    f.x = std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    f.y = std::sin(glm::radians(pitch));
    f.z = std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    front = glm::normalize(f);
    right = glm::normalize(glm::cross(front, worldUp));
    up    = glm::normalize(glm::cross(right, front));
}
