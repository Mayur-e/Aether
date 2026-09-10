#include "Player.h"
#include "InputManager.h"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>

void Player::init(Physics2D& phys, glm::vec2 startPos) {
    Body2D b;
    b.pos = startPos;
    b.half = { 0.4f, 0.9f }; // Player size (approx 0.8w x 1.8h meters)
    b.mass = 1.0f;
    b.noGravity = false;
    
    if (bodyId != -1) {
        phys.removeBody(bodyId);
    }
    bodyId = phys.addBody(b);
}

void Player::update(float dt, Physics2D& phys, GravitySystem& grav) {
    if (bodyId == -1) return;
    
    Body2D& body = phys.body(bodyId);
    
    // 1. Gravity Switching Logic
    if (InputManager::isKeyPressed(GLFW_KEY_DOWN))  grav.request(GravityDir::DOWN);
    if (InputManager::isKeyPressed(GLFW_KEY_UP))    grav.request(GravityDir::UP);
    if (InputManager::isKeyPressed(GLFW_KEY_LEFT))  grav.request(GravityDir::LEFT);
    if (InputManager::isKeyPressed(GLFW_KEY_RIGHT)) grav.request(GravityDir::RIGHT);
    
    // 2. Movement Logic (relative to current gravity)
    glm::vec2 upVec = grav.upVec();
    glm::vec2 rightVec = { upVec.y, -upVec.x }; // Perpendicular right
    
    // Input axis (-1 to 1)
    float moveDir = 0.0f;
    if (InputManager::isKeyHeld(GLFW_KEY_A)) moveDir -= 1.0f;
    if (InputManager::isKeyHeld(GLFW_KEY_D)) moveDir += 1.0f;
    
    // Project current velocity onto horizontal (rightVec) and vertical (upVec) axes
    float vHorizontal = glm::dot(body.vel, rightVec);
    float vVertical   = glm::dot(body.vel, upVec);
    
    // Apply horizontal acceleration
    if (moveDir != 0.0f) {
        vHorizontal += moveDir * moveAccel * dt;
        
        // Clamp to max speed
        if (std::abs(vHorizontal) > maxRunSpeed) {
            vHorizontal = (vHorizontal > 0.0f ? 1.0f : -1.0f) * maxRunSpeed;
        }
    } else {
        // Friction / Deceleration
        if (vHorizontal > 0.0f) {
            vHorizontal = std::max(0.0f, vHorizontal - moveDeccel * dt);
        } else if (vHorizontal < 0.0f) {
            vHorizontal = std::min(0.0f, vHorizontal + moveDeccel * dt);
        }
    }
    
    // 3. Jumping
    if (body.onGround && InputManager::isKeyPressed(GLFW_KEY_SPACE)) {
        vVertical = jumpForce;
        body.onGround = false;
    }
    
    // Reconstruct velocity vector
    body.vel = rightVec * vHorizontal + upVec * vVertical;
}
