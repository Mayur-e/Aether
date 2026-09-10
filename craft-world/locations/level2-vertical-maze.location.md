---
name: level2-vertical-maze
image:
  url: https://media.craftrpgs.com/containers/01a074ae-3136-72dc-a7f0-965bf56e0889/images/1b88c72c-38d8-4932-9bd0-34783e0f6278
  generation:
    status: success
    startedAt: 2026-09-08T16:28:53.729Z
    finishedAt: 2026-09-08T16:29:07.776Z
    prompt: Wide establishing shot of tall narrow claustrophobic vertical maze
      corridor, deep teal-green walls, cool blue-white strip lighting, wall
      machinery gears turning, hanging chains and vertical pipes, platforms on
      walls and ceiling, dark sci-fi concept art, moody cinematic lighting,
      highly detailed game key art, no text, no UI, no watermark
$craft:
  referenceId: 01a074db-72c7-73e8-bf42-0c4ae241fcda
---

# Section B-2 — The Vertical Maze

> *Level 2: Gravity Walls*

## Overview

A tall narrow corridor section where the walls *are* the floors. The Aether catastrophe shredded the gravity anchors in this section — left and right walls now attract with equal force to down. Kael must traverse what feels like a constantly rotating box, walking walls as floors and navigating passages that only make sense when gravity is sideways.

## Visual Style

- **Palette:** Deep teal-green (#0D2B2B) with cool blue-white overhead strip lighting
- **Mood:** Claustrophobic. The geometry is dense and confusing by design. Good silhouette contrast between Kael and the walls.
- **Depth layers:**
  - Background: wall-mounted machinery gears slowly turning
  - Gameplay plane: narrow platforms, vertical passages
  - Foreground: hanging chains, vertical pipes

## Geometry

```
World bounds: X [0, 20], Y [0, 28]    -- tall and narrow
Gravity start: DOWN

Platforms:
  - Floor:               pos(0,0),   size(20,1)
  - Left wall section:   pos(0,8),   size(1,6)     -- player walks this under LEFT gravity
  - Left ledge:          pos(0,14),  size(4,1)
  - Right wall section:  pos(19,12), size(1,8)     -- player walks under RIGHT gravity
  - Right ledge:         pos(15,20), size(5,1)
  - Ceiling section:     pos(0,27),  size(20,1)
  - Mid platform:        pos(8,14),  size(4,1)

Objects:
  - ARIA node: pos(1,1)                             -- warns about wall gravity
  - Gravity Switch [LEFT]:  pos(2,1)                -- first LEFT switch
  - Gravity Switch [RIGHT]: pos(19,14)              -- RIGHT switch mid-level
  - Gravity Switch [UP]:    pos(1,20)               -- reaches ceiling
  - Gravity Switch [DOWN]:  pos(10,27)              -- reset from ceiling
  - Pressure Plate: pos(10,20) -- opens door below
  - Door: pos(18,0), size(1,3)
  - Goal: pos(19,26)

Enemies:
  - Sentinel-Standard: patrol between pos(6,1) and pos(14,1)
  
Hazards:
  - Spikes on right wall Y[4..8]: touching with wrong gravity = death reset to checkpoint
  
Checkpoint: pos(4,14)
```

## Narrative

ARIA's signal is clearer here. She explains that the B-sections were experimental gravity re-routing zones — the walls were *designed* to attract at variable vectors. The Aether explosion just... made all of them active simultaneously.

Kael finds a data terminal with a partial log from the night of the catastrophe. His own voice, recorded: *"Initiating Core sequence. This will work."*

## Physics Demonstration (CGG Note)

> **Transformations** — Left/Right gravity uses the same AABB physics but with `gravityVector = {-g,0}` or `{+g,0}`. Player model matrix includes a composite rotation: `R_gravity * R_animation * T_position`. Walking on walls is a consequence of correct physics, not special-cased code.

---
