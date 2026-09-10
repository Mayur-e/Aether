#pragma once
#include <glm/glm.hpp>
#include "../game/GravitySystem.h"

// -----------------------------------------------------------------------
// GameCamera
// A 2.5D side-scrolling camera that follows a target and adapts to gravity.
// -----------------------------------------------------------------------
class GameCamera {
public:
    glm::vec3 position    { 0.0f, 0.0f, 20.0f }; // World position
    glm::vec3 target      { 0.0f, 0.0f, 0.0f  }; // What we look at
    glm::vec3 up          { 0.0f, 1.0f, 0.0f  }; // Camera up vector
    
    float     fov         = 60.0f; // degrees
    float     orthoSize   = 15.0f; // For orthographic projection
    bool      isOrtho     = false;

    // Follow a 2D world position. The camera will smoothly interpolate its position
    // and up vector based on the provided gravity direction.
    void update(float dt, glm::vec2 playerPos, const GravitySystem& grav);

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspect) const;
};
