---
name: level3-storage-vaults
image:
  url: https://media.craftrpgs.com/containers/01a074ae-3136-72dc-a7f0-965bf56e0889/images/3f60a202-c02d-49c5-ba6a-595a906ca46f
  generation:
    status: success
    startedAt: 2026-09-08T16:28:53.720Z
    finishedAt: 2026-09-08T16:29:05.741Z
    prompt: Wide establishing shot of industrial deep storage vault wing, burnt
      orange rust walls and corroded steel, rows of sealed vault doors, glowing
      deep violet cubes with rune lines as only saturated accent, hanging chains
      and cables, amber emergency strips, dark sci-fi concept art, moody
      cinematic lighting, highly detailed game key art, no text, no UI, no
      watermark
$craft:
  referenceId: 01a074db-72c7-73e8-bf42-12bae44d7b27
---

# Section C-3 — The Storage Vaults

> *Level 3: Weighted Objects*

## Overview

The Institute's deep storage wing — rows of hermetically sealed vaults now cracked open, their contents scattered across the floor. The key puzzle element here is the **Aether Crate**: shielded containers built to survive gravity anomalies. They feel gravity exactly like Kael does, but their mass makes them heavy, slow, and stubborn.

Kael must push and drop crates — under whatever direction gravity currently points — to weigh down pressure plates and open the vault doors in sequence. Three linked puzzles: a low gate, a high shelf, and a wall vault that demands sideways gravity.

## Visual Style

- **Palette:** Burnt orange / rust (`#2B1800`) walls with corroded steel (`#3A2A1A`), deep shadow, and the violet Aether crate glow (`#6633B3`) as the only saturated accent
- **Mood:** Industrial and heavy. Dense geometry, looming vault doors. Every landing should feel weighted — the "thunk" of a crate is the level's signature sound
- **Depth layers:**
  - Background: receding rows of sealed vault doors, stacked shelving silhouettes, slow drifting dust
  - Gameplay plane: platforms, crates, pressure plates, doors
  - Foreground: hanging chains, trailing cables, one collapsed shelf leaning toward the camera
- **Signature object — Aether Crate:** perfect 1×1 cube (mesh: cube, `halfExtents (0.5, 0.5)`, `mass 2.0`), colour `(0.4, 0.2, 0.7)` with animated violet rune lines (UV-scrolling emissive `(0.75, 0.35, 1.0)`, strength 2.0) and a dim violet `PointLight (0.6, 0.3, 1.0)` radius 2.5
- **Lighting:** amber emergency strip lights `PointLight (0.9, 0.6, 0.1)` radius 6 along the ceiling; pressure plates glow amber `(0.9, 0.7, 0.1)` when holding a load. Landing: dust-puff particle burst + "thunk" sound; dragging: thin scraper particles at the crate's base

## Geometry

```
World bounds: X [0, 40], Y [0, 14]
Gravity start: DOWN

Platforms:
  - Main floor:     pos(0,0),  size(40,1)
  - Raised shelf:   pos(8,5),  size(8,1)
  - High shelf:     pos(20,6), size(6,1)
  - Right column:   pos(36,0), size(1,10)   -- wall for RIGHT gravity puzzle

Objects:
  - ARIA node: pos(2,1)                     -- crate + pressure plate tutorial trigger

Puzzle 1 — "The Low Gate":
  - Crate A:             pos(4,1)            -- starts on floor
  - Pressure Plate 0:    pos(12,1)           -- Crate A must land here (mass 2.0 ≥ plate threshold 1.0)
  - Door 0:              pos(16,0), size(1,4) -- opens when plate activated
  
Puzzle 2 — "The High Shelf":
  - Crate B:             pos(10,6)           -- on raised shelf
  - Pressure Plate 1:    pos(22,9)           -- on high shelf (UP gravity needed to reach)
  - Gravity Switch [UP]: pos(6,6)
  - Door 1:              pos(28,0), size(1,5)

Puzzle 3 — "The Wall Vault":
  - Crate C:             pos(30,1)
  - Pressure Plate 2:    pos(37,5)           -- on right wall under RIGHT gravity
  - Gravity Switch [RIGHT]: pos(25,1)
  - Gravity Switch [DOWN]:  pos(39,8)
  - Door 2 (GOAL door):  pos(38,0), size(2,12) -- also serves as Goal/Exit trigger

Enemies:
  - Sentinel-Standard x1: patrols pos(18,1)..(22,1) on the main floor -- resets gravity to DOWN 1.5s after each switch, so time your crate pushes around it
    (counterplay: stand on top to ground it for 8s)

Hazards:
  - Floor gap: pos(20,0), size(1,1) -- missing plating under the high shelf; falling = death, reset to checkpoint

Checkpoint: pos(14,1) -- after first door
```

## Narrative

The storage logs reveal that the Aether crates were being used to *contain* gravity anomalies — like sealed boxes of broken physics. Some are already leaking. Kael notes the irony: he used to think the containment was the problem. Now he sees it was the only thing keeping things stable.

Kael approaches his first crate, and ARIA lights up the node beside it:

> **ARIA (crate tutorial):**
> "Dr. Kael. That is an Aether Crate. It obeys gravity exactly as you do — it is simply heavier, and slower to agree with you. Walk into it to push it. Move it onto the amber plate. Please avoid dropping it on yourself; the floor is not forgiving."

ARIA: *"Crate containment at 34%. I would suggest not dropping them from height. I would also suggest you are about to drop one from height."*

## Physics Demonstration (CGG Note)

> **AABB Collision + Dynamic Bodies** — Crates use the same `RigidBody` + `PhysicsWorld` as the player but with `mass: 2.0`. They interact with the player (pushing) and with static platforms. AABB overlap + minimum-penetration resolution stops a pushed crate flush against a wall, while a crate half-overhanging a platform edge loses its supporting contact and slides off — the min-penetration axis is explicitly visible every time it happens.
>
> Pushing is a contact impulse, not movement-copying:
> ```cpp
> if (playerAABB.intersects(crateAABB) && !playerStandingOnCrate) {
>     glm::vec2 pushDir = glm::normalize(crate.position - player.position);
>     crate.velocity += pushDir * (PUSH_FORCE / crate.mass); // mass 2.0 → crates feel heavy, slide on contact
> }
> ```
> Orientation never matters for AABBs — the same push code works along whatever axis gravity currently defines, whether that means shoving Crate C across the floor or up the right wall in Puzzle 3.
>
> Render transform follows the body (the cube needs no extra spin, but the gravity tilt applies):
> ```cpp
> glm::mat4 crateModel = glm::translate(I, glm::vec3(crate.pos, 0.0f))
>                      * glm::rotate(I, gravAngle, {0, 0, 1});
> ```
>
> **Polygon Fill (CGG concept link)** — The pressure plate visual uses a scanline fill algorithm (adapted from `scanline_fill.cpp`) to draw its filled indicator rectangle on the HUD.

---
