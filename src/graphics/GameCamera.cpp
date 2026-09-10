#include "GameCamera.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <algorithm>

void GameCamera::update(float dt, glm::vec2 playerPos, const GravitySystem& grav) {
    // 1. Determine desired up vector based on gravity
    glm::vec2 idealUp2D = grav.upVec();
    glm::vec3 idealUp   = glm::vec3(idealUp2D.x, idealUp2D.y, 0.0f);
    
    // Smoothly interpolate up vector
    const float upRotSpeed = 5.0f;
    up = glm::normalize(glm::mix(up, idealUp, dt * upRotSpeed));
    
    // 2. Follow player
    glm::vec3 idealTarget = glm::vec3(playerPos.x, playerPos.y, 0.0f);
    
    // The camera's "rest" position is set back along Z
    float distZ = isOrtho ? 0.0f : 20.0f;
    glm::vec3 idealPos = idealTarget + glm::vec3(0.0f, 0.0f, distZ);
    
    // Simple spring damping for smooth follow
    const float followSpeed = 10.0f;
    target   = glm::mix(target, idealTarget, dt * followSpeed);
    position = glm::mix(position, idealPos, dt * followSpeed);
}

glm::mat4 GameCamera::getViewMatrix() const {
    return glm::lookAt(position, target, up);
}

glm::mat4 GameCamera::getProjectionMatrix(float aspect) const {
    if (isOrtho) {
        float halfW = orthoSize * aspect;
        float halfH = orthoSize;
        return glm::ortho(-halfW, halfW, -halfH, halfH, -100.0f, 100.0f);
    } else {
        return glm::perspective(glm::radians(fov), aspect, 0.1f, 100.0f);
    }
}
