#pragma once
#include "Physics2D.h"
#include "GravitySystem.h"
#include <vector>
#include <string>
#include <glm/glm.hpp>

// -----------------------------------------------------------------------
// PuzzleObjects.h
// All interactive objects in AETHER:
//   GravityCube, PressurePlate, Door, GravitySwitch, GravityField,
//   ConduitValve, Goal, Spike
// -----------------------------------------------------------------------

// A dynamic physics body the player can push (mass 2.0).
struct GravityCube {
    int   bodyId = -1;
    float mass   = 2.0f;
};

// Pressure plate: activates when a body with mass >= threshold rests on it.
struct PressurePlate {
    int         bodyId      = -1;
    std::string signalId;          // door/gate name to signal
    bool        isPressed   = false;
    float       massThresh  = 1.0f;
};

// A static barrier that slides open/closed on a signal.
struct Door {
    int         bodyId      = -1;
    std::string name;              // must match the signalId of its activator
    glm::vec2   closedPos   { 0.0f, 0.0f };
    glm::vec2   openPos     { 0.0f, 0.0f };
    bool        isOpen      = false;
    float       transition  = 0.0f;  // 0=closed, 1=open
    bool        isGoalDoor  = false; // reaching an open goal door completes the level
};

// Legacy alias (Button = PressurePlate) so Level.cpp doesn't break immediately
using Button = PressurePlate;

// Gravity switch pillar: walk into it to request a direction change.
struct GravitySwitch {
    int        bodyId    = -1;    // static trigger AABB
    GravityDir direction = GravityDir::DOWN;
    bool       wasOverlapping = false;
};

// Axis-aligned gravity field zone: bodies inside use a local gravity override.
struct GravityField {
    glm::vec2  pos  { 0.0f, 0.0f };
    glm::vec2  half { 1.0f, 1.0f };
    GravityDir direction = GravityDir::DOWN;
    float      strength  = 1.0f;  // multiplier on GravitySystem::STRENGTH
};

// Conduit valve: one-shot jump-activated switch. Sends a permanent signal.
struct ConduitValve {
    int         bodyId    = -1;
    std::string signalId;
    bool        activated = false;
    bool        wasOverlapping = false;
};

// Goal / exit portal: stepping into it completes the level.
struct Goal {
    int   bodyId    = -1;
    bool  triggered = false;
};

// Spike hazard: touching it resets the player to the last checkpoint.
struct Spike {
    int   bodyId = -1;
};

// -----------------------------------------------------------------------
// Forward declarations
class Player;

// Apply gravity field overrides to all bodies that are inside a field.
// Must be called BEFORE Physics2D::step() each frame.
void applyGravityFields(const std::vector<GravityField>& fields,
                        Physics2D& physics,
                        const GravitySystem& gravity);

// Main puzzle update: handles switch triggers, plate presses, door motion,
// valve activation, and goal detection.  Returns true if the Goal was reached.
bool updatePuzzleObjects(
    std::vector<PressurePlate>&  plates,
    std::vector<Door>&           doors,
    const std::vector<GravityCube>& cubes,
    std::vector<GravitySwitch>&  switches,
    std::vector<ConduitValve>&   valves,
    std::vector<Goal>&           goals,
    std::vector<Spike>&          spikes,
    Player&                      player,
    Physics2D&                   physics,
    GravitySystem&               gravity,
    bool&                        playerDied,   // out: true → reset to checkpoint
    float                        dt);
