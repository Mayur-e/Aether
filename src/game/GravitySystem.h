#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// -----------------------------------------------------------------------
// GravityDir — the four cardinal gravity directions supported by AETHER
// -----------------------------------------------------------------------
enum class GravityDir { DOWN, UP, LEFT, RIGHT };

// -----------------------------------------------------------------------
// GravitySystem
//
// Manages the current gravity direction and smooth transitions between
// directions.  The player queries vector() each frame to get the effective
// gravity to apply to physics bodies.
//
// Usage:
//   grav.request(GravityDir::UP);    // request a switch
//   grav.update(dt);                 // advance blend each frame
//   phys.gravity = grav.vector();    // give Physics2D the new vector
// -----------------------------------------------------------------------
class GravitySystem {
public:
    // Tuning constants
    static constexpr float STRENGTH    = 20.0f;  // m/s²  (strong for snappy feel)
    static constexpr float BLEND_TIME  = 0.30f;  // seconds for full direction transition
    static constexpr float COOLDOWN    = 0.70f;  // minimum time between switches

    GravityDir current  = GravityDir::DOWN;
    GravityDir pending  = GravityDir::DOWN;
    float      blendT   = 1.0f;    // 0 = just started, 1 = fully transitioned
    float      cooldown = 0.0f;    // seconds until next switch allowed

    // ---------- API ----------

    // Request a gravity switch.  Returns true if accepted (not on cooldown).
    bool request(GravityDir dir);

    // Advance blend timer + cooldown — call once per frame.
    void update(float dt);

    // ---------- Queries ----------

    // Current effective gravity vector (magnitude = STRENGTH, blended).
    glm::vec2 vector() const;

    // Normalised "up" direction relative to current gravity.
    glm::vec2 upVec() const;

    // Rotation angle (radians, around Z) so the player body stands upright.
    float angle() const;

    bool isTransitioning()  const { return blendT < 1.0f; }
    bool canSwitch()        const { return cooldown <= 0.0f; }

    // 0 = just switched, 1 = ready again
    float cooldownFraction() const {
        return 1.0f - (cooldown / COOLDOWN);
    }

    // ---------- Helpers ----------
    static glm::vec2   toVec(GravityDir d);
    static float       toAngle(GravityDir d);   // radians
    static const char* toStr(GravityDir d);
};
