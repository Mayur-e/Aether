---
name: puzzle-objects
image:
  url: https://media.craftrpgs.com/containers/01a074ae-3136-72dc-a7f0-965bf56e0889/images/4ca0c139-1c01-4b96-a63a-0ea38e7c9d12
  generation:
    status: success
    startedAt: 2026-09-08T16:28:53.759Z
    finishedAt: 2026-09-08T16:29:11.345Z
    prompt: "Dramatic showcase of sci-fi puzzle objects arranged in dark vault:
      violet glowing crate on amber pressure plate, tall blue sliding door,
      pillar with directional arrow switch, transparent gravity field with
      dashed border, glowing cyan valve sphere, tall pulsing exit arch, dark
      sci-fi concept art, moody cinematic lighting, highly detailed game key
      art, no text, no UI, no watermark"
$craft:
  referenceId: 01a074db-72c8-723d-ba1a-b32481374235
---

# Puzzle Objects

## Overview

AETHER's puzzle language uses a small set of reusable objects. Every level is built from combinations of these objects. New ones are introduced gradually.

**Seven objects form the whole language**: Pressure Plate, Door, Aether Crate, Gravity Switch, Gravity Field, Conduit Valve and Goal. Activators (plates, switches, valves) tag their output with a `signalId`; receivers (doors) subscribe to the same id. Wiring is declared in level data (TOML), never hardcoded — a level designer can rewire any plate to any door without touching code.

---

## Pressure Plate

A floor-level trigger that activates when a body with mass ≥ 1.0 rests on it.

```
Visual: Flat rectangle, sunken slightly into floor surface
Active colour:   (0.9, 0.7, 0.1) — amber glow  #E6B31A
Inactive colour: (0.3, 0.3, 0.3) — grey        #4D4D4D
Emits:  PointLight (amber) when active, radius 2
AABB:   (halfX=0.5, halfY=0.05) — very thin trigger
Activation: any dynamic body overlapping and grounded above it
```

Activates → sends a signal tagged `signalId` to the linked Door/Gate ID. A crate counts as a valid weight — weight puzzles work with crates, not just the player.

```cpp
// PressurePlate::update
bool pressed = false;
for (auto& b : world.bodies) {
    if (b.mass >= 1.0f && b.grounded && aabbOverlap(plateAABB, aabbOf(b)))
        pressed = true;
}
if (pressed != wasPressed) signalBus.emit(signalId, pressed ? ON : OFF);
wasPressed = pressed;
```

---

## Door

A static barrier that opens/closes in response to a pressure plate or switch signal on its `signalId`.

```
Visual: Vertical rectangle mesh (halfX=0.35, halfY=1.5 — blocks a 3 m corridor)
Colour:       (0.2, 0.5, 0.8) — Aether blue tint  #3380CC
Emits:        PointLight (dim blue) always, brightens when opening
AABB:         full rect while closed; retracts as it slides
Activation:   ON signal from linked plate/switch on its signalId → opens
Open state:   slides into floor (Y animates from 0 to -height over 0.5s)
Closed state: full height blocks passage
Animation:    smooth `glm::mix(closedPos, openPos, openT)` where openT lerps 0..1
```

```cpp
// Door::update — driven by the signal graph, not physics
openT = glm::clamp(openT + dt * (open ? 1.0f : -1.0f) / OPEN_TIME, 0.0f, 1.0f);
body.position.y = glm::mix(closedY, closedY - height, openT);   // slides into floor
```

---

## Movable Crate (Aether Crate)

A dynamic physics body the player can push.

```
Mass: 2.0
halfExtents: (0.5, 0.5)
Color: (0.4, 0.2, 0.7) — deep violet  #6633B3
Rune lines: animated emissive texture (UV-scrolling shader effect)
Responds to: gravity (same as player), player pushing (contact impulse), gravity fields
AABB:   (halfX=0.5, halfY=0.5)
Activation: pushed by walking into it — contact normal provides the impulse
Sounds: [stub] landing thud, dragging scrape
```

Player pushes crate by walking into it — contact normal provides push impulse:
```cpp
if (playerAABB.intersects(crateAABB)) {
    glm::vec2 pushDir = normalize(crate.position - player.position);
    crate.velocity += pushDir * PUSH_FORCE / crate.mass;
}
```

---

## Gravity Switch

An interactive object Kael activates by walking into it (or pressing interact near it).

```
Visual: Pillar with a glowing directional arrow on top
Active colour: direction-coded
  DOWN:  (0.2, 0.5, 1.0)  #3380FF — blue
  UP:    (0.8, 0.3, 0.9)  #CC4DE6 — purple
  LEFT:  (0.3, 0.9, 0.4)  #4DE666 — green
  RIGHT: (0.9, 0.6, 0.1)  #E6991A — amber
Arrow mesh: quad with directional texture (↑↓←→), emissive when active
AABB trigger: (halfX=0.6, halfY=1.2) — player walks into it to activate
Activation: player AABB overlaps trigger → activates once per entry (edge-triggered)
```

```cpp
// GravitySwitch::update
bool overlap = aabbOverlap(switchTrigger, playerAABB);
if (overlap && !wasOverlapping) world.gravity.requestSwitch(direction);
wasOverlapping = overlap;
```

Activation calls `GravitySystem::requestSwitch(direction)` — cooldown and blend are the gravity system's business.

---

## Gravity Field (Zone)

An invisible AABB volume with its own gravity vector.

```
Visual: Coloured transparent border only (no fill — player sees through it)
Border rendering: dashed lines along all 4 edges
  Colour = direction-coded (same as Gravity Switch)
  Arrow indicators at midpoint of each border edge
Interior: subtle directional particle flow (particles drift in field gravity direction)
AABB: configurable per level
Activation: immediate on entry, immediate exit on departure — no blend, snaps
```

```cpp
// PhysicsWorld::integrate — field applies a per-body gravity override
glm::vec2 g = gravity.gravityVector();
for (auto& field : gravityFields)                          // AABB volumes
    for (auto& b : bodies)
        if (aabbOverlap(field.aabb, aabbOf(b)))
            b.gravityOverride = field.direction * GRAVITY_STRENGTH;
```

---

## Conduit Valve

A one-time activation object — requires player proximity + jump to reach.

```
Visual: Sphere on a short pole, glowing ring orbiting it
Inactive: dim grey (0.3, 0.3, 0.35)  #4D4D59
Active:   bright cyan (0.0, 0.9, 1.0)  #00E6FF + particle burst
AABB trigger: (halfX=0.7, halfY=1.2) — pole + sphere volume
Activation: player overlaps trigger AND is grounded in the current gravity direction (jump-locked reach)
Once activated: stays on, sends permanent (locked) signal to linked object
```

```cpp
// ConduitValve::update
if (!activated && aabbOverlap(valveTrigger, playerAABB) && player.grounded) {
    activated = true;
    signalBus.emit(signalId, Signal::LOCKED_ON);           // permanent, never OFF
    spawnParticleBurst(cyan, valveCentre);
}
```

---

## Goal (Exit)

The level completion trigger.

```
Visual: Tall glowing arch
  Two vertical bar meshes + horizontal cap
  Pulsing emissive: colour cycles slowly through (0.5,0.8,1.0) → (0.8,0.5,1.0) → (0.5,0.8,1.0)
  Inner volume: bright particle column (upward regardless of gravity)
AABB trigger: (halfX=0.9, halfY=1.8) — player centre enters → level complete
Activation: player AABB overlaps → completes the level, once
On trigger: play completion animation (flash + expand), transition to next level
```

```cpp
// Goal::update
if (!completed && aabbOverlap(goalTrigger, playerAABB)) {
    completed = true;
    level.complete(LevelResult::SUCCESS);                  // flash + expand, next level
}
```

---
