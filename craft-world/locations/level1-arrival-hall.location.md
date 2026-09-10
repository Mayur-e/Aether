---
name: level1-arrival-hall
image:
  url: https://media.craftrpgs.com/containers/01a074ae-3136-72dc-a7f0-965bf56e0889/images/3d6472d9-ff31-4652-8b13-a909f7ccd73a
  generation:
    status: success
    startedAt: 2026-09-08T16:28:53.706Z
    finishedAt: 2026-09-08T16:29:02.195Z
    prompt: Wide establishing shot of ruined institutional arrival hall, dark
      charcoal stone architecture, warm amber emergency lighting, cracked floor
      and tilted pillars, dust hanging suspended, broken railings foreground,
      dark sci-fi concept art, moody cinematic lighting, deep shadows with
      glowing accents, highly detailed game key art, no text, no UI, no
      watermark
$craft:
  referenceId: 01a074db-72c7-73e8-bf42-09ce4be8e202
---

# Section A-1 — The Arrival Hall

> *Level 1: Introduction*

## Overview

The outermost section of the Aether Institute, once used as a receiving hall for new researchers. The catastrophe hit here mildest — the architecture is recognisable, the floor is still a floor. But something is wrong: objects float at wrong angles, dust hangs suspended, and the lights flicker to a slow broken rhythm.

This is where Kael regains consciousness.

## Visual Style

- **Palette:** Dark charcoal stone (#1A1A20) with warm amber emergency lighting
- **Mood:** Institutional ruin — once orderly, now tilted and cracked
- **Depth layers:**
  - Background: blurred institutional corridor geometry, receding pillars
  - Gameplay plane: platforms and objects
  - Foreground: broken railings, dust particles drifting
- **Atmosphere:** Sparse. Quiet. The first gravity flip moment should feel revelatory.

## Geometry

```
World bounds: X [0, 32], Y [0, 12]
Gravity start: DOWN

Platforms:
  - Floor: pos(0,0), size(32,1)              -- solid ground the full length
  - Raised ledge: pos(10,3), size(6,1)       -- first jump required
  - Higher ledge: pos(18,6), size(6,1)       -- requires gravity flip to DOWN after standing on wall
  - Ceiling section: pos(22,10), size(10,1)  -- player walks here under UP gravity

Objects:
  - ARIA node: pos(2,1)                      -- tutorial dialogue trigger
  - Gravity Switch [UP]: pos(14,4)           -- first gravity switch (teaches UP)
  - Gravity Switch [DOWN]: pos(24,11)        -- restores gravity (teaches DOWN)
  - Goal/Exit: pos(30,1)                     -- door, opens when player arrives
  
Enemies: None (tutorial section)
Hazards: One gap [16,0 to 18,0] -- player falls if they don't use gravity switch
Checkpoint: pos(16,1)
```

## Narrative

Kael wakes here. ARIA's first transmission guides him forward. He discovers his Aether burn mark glows near the first Gravity Switch — the switch doesn't just respond to buttons; it responds to *him*.

The final moment of this level: Kael steps through the exit door and looks down a vertical shaft. The game camera tilts. Below him — 30 metres of freefall. Then he flips gravity and *falls upward*, laughing in terrified disbelief.

## Physics Demonstration (CGG Note)

> **Coordinate Systems** — Level 1 exists purely in the XY plane. The camera uses `glm::ortho` with a fixed depth layer. Player transformation is `glm::translate(I, pos) * glm::rotate(I, gravAngle, {0,0,1}) * glm::scale(I, playerScale)`.

---
