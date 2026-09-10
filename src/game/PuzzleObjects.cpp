#include "PuzzleObjects.h"
#include "Player.h"
#include <algorithm>
#include <cmath>

// -----------------------------------------------------------------------
// applyGravityFields
// Call BEFORE Physics2D::step() to override per-body gravity inside zones.
// -----------------------------------------------------------------------
void applyGravityFields(const std::vector<GravityField>& fields,
                        Physics2D& physics,
                        const GravitySystem& gravity)
{
    const int n = physics.numBodies();
    for (int i = 0; i < n; ++i) {
        Body2D& b = physics.body(i);
        if (!b.active || b.mass == 0.0f || b.noGravity) continue;

        // Reset override so bodies outside all fields use global gravity.
        b.gravityOverride = glm::vec2(0.0f);
        b.hasGravityOverride = false;

        // Check every field — last one wins (could be extended to blend).
        for (const auto& f : fields) {
            if (std::abs(b.pos.x - f.pos.x) < f.half.x + b.half.x &&
                std::abs(b.pos.y - f.pos.y) < f.half.y + b.half.y)
            {
                b.gravityOverride    = GravitySystem::toVec(f.direction)
                                       * (f.strength * GravitySystem::STRENGTH);
                b.hasGravityOverride = true;
            }
        }
    }
}

// -----------------------------------------------------------------------
// updatePuzzleObjects
// -----------------------------------------------------------------------
bool updatePuzzleObjects(
    std::vector<PressurePlate>&     plates,
    std::vector<Door>&              doors,
    const std::vector<GravityCube>& cubes,
    std::vector<GravitySwitch>&     switches,
    std::vector<ConduitValve>&      valves,
    std::vector<Goal>&              goals,
    std::vector<Spike>&             spikes,
    Player&                         player,
    Physics2D&                      physics,
    GravitySystem&                  gravity,
    bool&                           playerDied,
    float                           dt)
{
    playerDied = false;
    bool levelComplete = false;

    const Body2D* pb = (player.bodyId >= 0) ? &physics.body(player.bodyId) : nullptr;

    // -----------------------------------------------------------------
    // 1. Gravity Switches — edge-triggered on player entry
    // -----------------------------------------------------------------
    for (auto& sw : switches) {
        if (sw.bodyId < 0) continue;
        const Body2D& sb = physics.body(sw.bodyId);
        bool overlap = pb && aabbOverlap(sb, *pb);
        if (overlap && !sw.wasOverlapping) {
            gravity.request(sw.direction);
        }
        sw.wasOverlapping = overlap;
    }

    // -----------------------------------------------------------------
    // 2. Pressure Plates — level-triggered each frame
    // -----------------------------------------------------------------
    for (auto& plate : plates) {
        if (plate.bodyId < 0) continue;
        const Body2D& plateBB = physics.body(plate.bodyId);
        bool wasPressed = plate.isPressed;
        plate.isPressed = false;

        // Player weight
        if (pb && pb->mass >= plate.massThresh && aabbOverlap(plateBB, *pb))
            plate.isPressed = true;

        // Cube weight
        if (!plate.isPressed) {
            for (const auto& cube : cubes) {
                if (cube.bodyId < 0) continue;
                const Body2D& cb = physics.body(cube.bodyId);
                if (cb.mass >= plate.massThresh && aabbOverlap(plateBB, cb)) {
                    plate.isPressed = true;
                    break;
                }
            }
        }

        // Signal linked doors when state changes
        if (plate.isPressed != wasPressed) {
            for (auto& door : doors) {
                if (door.name == plate.signalId)
                    door.isOpen = plate.isPressed;
            }
        }
    }

    // -----------------------------------------------------------------
    // 3. Conduit Valves — one-shot, player must overlap while grounded
    // -----------------------------------------------------------------
    for (auto& valve : valves) {
        if (valve.activated || valve.bodyId < 0) continue;
        const Body2D& vb = physics.body(valve.bodyId);
        bool overlap = pb && aabbOverlap(vb, *pb);
        if (overlap && !valve.wasOverlapping) {
            valve.activated = true;
            // Permanently open the linked door
            for (auto& door : doors) {
                if (door.name == valve.signalId)
                    door.isOpen = true;
            }
        }
        valve.wasOverlapping = overlap;
    }

    // -----------------------------------------------------------------
    // 4. Doors — smooth slide animation
    // -----------------------------------------------------------------
    const float doorSpeed = 2.0f;   // seconds to fully open/close
    for (auto& door : doors) {
        if (door.bodyId < 0) continue;
        if (door.isOpen)
            door.transition = std::min(1.0f, door.transition + dt / doorSpeed);
        else
            door.transition = std::max(0.0f, door.transition - dt / doorSpeed);

        Body2D& db = physics.body(door.bodyId);
        db.pos = glm::mix(door.closedPos, door.openPos, door.transition);
    }

    // -----------------------------------------------------------------
    // 5. Goals — player enters the trigger → level complete
    // -----------------------------------------------------------------
    for (auto& goal : goals) {
        if (goal.triggered || goal.bodyId < 0) continue;
        const Body2D& gb = physics.body(goal.bodyId);
        if (pb && aabbOverlap(gb, *pb)) {
            goal.triggered = true;
            levelComplete  = true;
        }
    }

    // -----------------------------------------------------------------
    // 6. Spikes — touching any spike kills the player
    // -----------------------------------------------------------------
    for (const auto& spike : spikes) {
        if (spike.bodyId < 0) continue;
        const Body2D& sb = physics.body(spike.bodyId);
        if (pb && aabbOverlap(sb, *pb)) {
            playerDied = true;
            break;
        }
    }

    return levelComplete;
}
