---
name: level7-aether-core
image:
  url: https://media.craftrpgs.com/containers/01a074ae-3136-72dc-a7f0-965bf56e0889/images/42e34aeb-d3d4-4781-a2c2-8bfc83395cb8
  generation:
    status: success
    startedAt: 2026-09-08T16:16:38.304Z
    finishedAt: 2026-09-08T16:16:43.500Z
    prompt: Wide establishing shot of a vast spherical chamber with a massive
      fractured white-violet crystal core at center radiating light, four
      platform arms radiating outward, orbital platforms, particle storm and
      glowing shadows, sense of awe and scale, empty of people, dark atmospheric
      sci-fi concept art, cinematic lighting, painterly, high detail
$craft:
  referenceId: 01a074db-72c8-723d-ba1a-a486f7906672
---

# The AETHER Core

> *Level 7: Final Challenge*
> *"AETHER — Gravity"*

## Overview

The geometric centre of the Institute. A vast spherical chamber containing the AETHER Core — a crystalline gravity generator the size of a building, now fractured and spinning. The Core continuously cycles through all four gravity directions in a rhythm that shifts every 30 seconds.

Everything Kael learned must be used here. Every mechanic appears: wall gravity, ceiling walking, crate puzzles, gravity fields, moving platforms, timed gates, sentinel enemies, fracture wraiths, and the shutdown terminal at the exact centre of the chamber.

The Core itself is a boss-level environmental entity.

## Visual Style

- **Palette:** Deep black (#050508) with the Core radiating blinding white-violet light
- **Mood:** Awe. Scale. This is bigger than anything in the game. The Core fills the background, rotating slowly, fractures pulsing.
- **Signature visual:** The Core crystal — a massive multi-faceted gemstone mesh (icosphere with high polygon count) rendered with emissive material and multiple layered point lights. Casts dynamic coloured shadows across all platform surfaces.
- **Particle density:** Maximum. The Core continuously emits gravity particles in all four directions.

## The Core Cycle

Every 30 seconds, global gravity cycles: DOWN → UP → LEFT → RIGHT → DOWN ...

The cycle is displayed on the HUD as a countdown ring (Bresenham circle arc).
Kael can temporarily override with his gravity switch — but the cycle keeps running.

```
Core Gravity Cycle:
  Phase DOWN:  0–30s
  Phase UP:    30–60s
  Phase LEFT:  60–90s
  Phase RIGHT: 90–120s
  (loops until shutdown)
```

## Geometry

```
World bounds: X [-30, 30], Y [-30, 30]  -- centred on Core
Core position: (0, 0)

Chamber structure (works under all 4 gravities):
  - 4 platform arms radiating from centre (horizontal + vertical)
  - Arm A (right):    pos(5,0),  size(20,2)
  - Arm B (left):     pos(-25,0), size(20,2)
  - Arm C (up):       pos(0,5),  size(2,20)
  - Arm D (down):     pos(0,-25), size(2,20)
  - Corner platforms: pos(±20, ±20), size(5,2) × 4

Moving Platforms:
  - Orbital RotPlat A: pivot(0,0), arm 12, speed 20 deg/sec
  - Orbital RotPlat B: pivot(0,0), arm 18, speed -15 deg/sec (opposite rotation)
  - Orbital RotPlat C: pivot(0,0), arm 8, speed 40 deg/sec

Gravity Fields (static zones):
  - Field [LEFT, 100%]:  pos(-20,-10), size(10,20)
  - Field [RIGHT, 100%]: pos(10,-10), size(10,20)
  - Field [UP, 100%]:    pos(-10,10), size(20,10)

Puzzle Sequence to reach shutdown terminal:
  1. Activate Valve A on right arm (requires RIGHT gravity)
  2. Move crate onto plate on ceiling (requires UP gravity + crate redirect)
  3. Disable Sentinel-Heavy guarding terminal (gravity-flip stun)
  4. Reach terminal at pos(0,0) — centre of chamber
  5. ARIA: "Activating shutdown sequence. Are you ready?"
  6. Player activates terminal — 10-second countdown
  7. Core begins to collapse — level exit materialises at pos(25,0)

Enemies:
  - Sentinel-Standard ×4: one per arm, patrol arm length
  - Sentinel-Heavy: pos(0,0) guarding terminal
  - Fracture Wraith ×3: orbit Core at varying radii
  
Final event:
  - All gravity vectors destabilise (rapid random switches every 2 seconds)
  - Player must reach exit before countdown ends
  - Screen edge vignette intensifies
  - Particle storm fills chamber
```

## Narrative — The Truth

ARIA reveals she is not a backup AI. She is the first AI the Institute built — and she was built *from* the Aether field itself. She is a gravity-sentient entity. She cannot leave. The shutdown will not destroy her — it will free her from the loop.

**Final dialogue:**

> **Kael:** "You knew this whole time."

> **ARIA:** "I knew you would get here. I didn't know how long it would take."

> **Kael:** "That doesn't answer—"

> **ARIA:** "Kael. Press the button."

**GAME COMPLETE screen:** The chamber goes dark. Then, one by one, gravity normalises throughout the Institute. Objects fall. Things land. And for the first time in the game — everything is still.

The final shot: the exit door at the end of a long corridor. Sunlight.

## Physics Demonstration (CGG Note)

> **All CGG Concepts Combined:**
> - Model/View/Projection matrices: full MVP pipeline for every draw call
> - Blinn-Phong lighting: Core radiates 4 point lights (one per gravity arm)
> - Particles: GPU particle system at maximum emitter count
> - AABB collision: all enemies, crates, and player simultaneously
> - Transformations: orbital platform hierarchy T*R*T_inverse
> - Gravity vector physics: all 4 directions simultaneously via GravityFields
> - Camera: follows player, adapts to gravity orientation, screen shake on Core pulse

---
