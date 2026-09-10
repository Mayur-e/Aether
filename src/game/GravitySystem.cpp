#include "GravitySystem.h"
#include <cmath>
#include <algorithm>

// -----------------------------------------------------------------------
glm::vec2 GravitySystem::toVec(GravityDir d) {
    switch (d) {
        case GravityDir::DOWN:  return {  0.0f, -1.0f };
        case GravityDir::UP:    return {  0.0f,  1.0f };
        case GravityDir::LEFT:  return { -1.0f,  0.0f };
        case GravityDir::RIGHT: return {  1.0f,  0.0f };
    }
    return { 0.0f, -1.0f };
}

float GravitySystem::toAngle(GravityDir d) {
    // Angle (around Z) such that rotating by this value stands the player
    // upright in the given gravity field.
    //   DOWN  → 0 (default upright)
    //   UP    → π (upside-down)
    //   LEFT  → +π/2 (standing on left wall)
    //   RIGHT → -π/2 (standing on right wall)
    switch (d) {
        case GravityDir::DOWN:  return 0.0f;
        case GravityDir::UP:    return  glm::pi<float>();
        case GravityDir::LEFT:  return  glm::half_pi<float>();
        case GravityDir::RIGHT: return -glm::half_pi<float>();
    }
    return 0.0f;
}

const char* GravitySystem::toStr(GravityDir d) {
    switch (d) {
        case GravityDir::DOWN:  return "DOWN  [F]";
        case GravityDir::UP:    return "UP    [R]";
        case GravityDir::LEFT:  return "LEFT  [Q]";
        case GravityDir::RIGHT: return "RIGHT [E]";
    }
    return "?";
}

// -----------------------------------------------------------------------
bool GravitySystem::request(GravityDir dir) {
    if (cooldown > 0.0f) return false;
    if (dir == pending && blendT >= 1.0f) return false;

    // If mid-transition, snap current to wherever we are now
    if (blendT < 1.0f) {
        current = pending;
        blendT  = 1.0f;
    }

    pending  = dir;
    blendT   = 0.0f;
    cooldown = COOLDOWN;
    return true;
}

void GravitySystem::update(float dt) {
    if (cooldown > 0.0f) cooldown = std::max(0.0f, cooldown - dt);

    if (blendT < 1.0f) {
        blendT += dt / BLEND_TIME;
        if (blendT >= 1.0f) {
            blendT  = 1.0f;
            current = pending;
        }
    }
}

glm::vec2 GravitySystem::vector() const {
    if (blendT >= 1.0f) return toVec(current) * STRENGTH;

    glm::vec2 from = toVec(current);
    glm::vec2 to   = toVec(pending);
    float t = blendT * blendT * (3.0f - 2.0f * blendT);   // smoothstep
    return glm::mix(from, to, t) * STRENGTH;
}

glm::vec2 GravitySystem::upVec() const {
    return -glm::normalize(vector());
}

float GravitySystem::angle() const {
    float a = toAngle(current);
    if (blendT >= 1.0f) return a;

    float b    = toAngle(pending);
    float diff = b - a;
    // Shortest-path interpolation
    while (diff >  glm::pi<float>()) diff -= glm::two_pi<float>();
    while (diff < -glm::pi<float>()) diff += glm::two_pi<float>();

    float t = blendT * blendT * (3.0f - 2.0f * blendT);
    return a + diff * t;
}
