---
name: kael
image:
  url: https://media.craftrpgs.com/containers/01a074ae-3136-72dc-a7f0-965bf56e0889/images/d8f44e11-e217-443b-a003-a09689188bbe
  generation:
    status: success
    startedAt: 2026-09-08T16:28:53.693Z
    finishedAt: 2026-09-08T16:28:57.802Z
    prompt: Lean wiry young male researcher standing sideways on a wall as if it
      were the floor, torn long research coat floating upward against gravity,
      short windswept dark hair, faint glowing blue circuit pattern on left arm,
      deep slate-blue outfit, shattered lab corridor behind him, dust hanging
      suspended, dark sci-fi concept art, moody cinematic lighting, deep shadows
      with glowing accents, highly detailed game key art, no text, no UI, no
      watermark
$craft:
  referenceId: 01a074db-72c7-73e8-bf41-ff1287c0e636
---

# Kael — The Drifter

> *"Gravity is just a suggestion. I stopped listening."*

## Overview

Kael is the player character of AETHER. Once a researcher at the Aether Institute, he was caught inside the AETHER Core when the facility's gravity experiments went catastrophically wrong. The Core fractured the laws of physics around the facility — and gave Kael an involuntary, permanent link to the Aether field.

He can feel gravity as a direction — not just force — and with effort, redirect it.

## Physical Description

- **Build:** Lean, wiry. Built for climbing and movement, not combat.
- **Height:** 5'11"
- **Appearance:** Short dark hair, perpetually windswept. Wears a torn researcher's coat over a graphene underlayer. Left arm has a faint luminous circuit pattern — the Aether burn mark.
- **Visual Orientation in-game:** Kael's body always rotates to stand perpendicular to the current gravity surface. When gravity flips, he tumbles then rights himself. His coat and hair react physically to gravity direction.

## Personality

- Dry, observational humour — copes with impossible situations by narrating them.
- Pragmatic. He didn't choose to be here. He just wants out.
- Hidden guilt: he was the one who activated the Core without authorization.

## Abilities (Gameplay)

| Ability | Description | Input |
|---|---|---|
| **Run** | Move horizontally relative to current gravity | A / D |
| **Jump** | Leap away from current ground surface | W / SPACE |
| **Gravity Shift — Down** | Reset gravity to standard downward | F |
| **Gravity Shift — Up** | Reverse gravity upward | R |
| **Gravity Shift — Left** | Rotate gravity to the left wall | Q |
| **Gravity Shift — Right** | Rotate gravity to the right wall | E |

## Physics Properties (C++ Implementation Reference)

```
halfExtents: (0.35, 0.5, 0.35)       -- AABB collision box (X narrower than platform unit)
mass: 1.0
jumpImpulse: 7.5                       -- Applied against current gravity direction
runSpeed: 5.0                          -- Along gravity's tangent axis
maxFallSpeed: 20.0                     -- Terminal velocity clamp
gravityBlend: 0.4 seconds              -- Smooth 0.4s blend of gravity direction per shift (linear, ease-in-out)
gravityFlipCooldown: 0.8 seconds       -- Prevents spam, tied to HUD arc indicator
groundFriction: 8.0                    -- Deceleration when grounded
airFriction: 1.0                       -- Reduced control mid-air (intentional)
```

## States

- `IDLE` — standing still
- `RUNNING` — moving along gravity surface
- `JUMPING` — ascending (positive component against gravity)
- `FALLING` — descending (positive component with gravity)
- `WALL_CLINGING` — brief pause when arriving at new surface post-flip
- `DEAD` — played death animation, respawning at checkpoint

## Render (C++ Notes)

- Rendered as a **capsule-shaped mesh** (cylinder + two hemisphere caps); capsule radius ~0.35, overall height ~1.0, matching `halfExtents`
- Colour: deep slate blue with emissive Aether-blue edge on the burn-mark arm
- Orientation matrix: `glm::rotate(model, gravityAngle, gravityAxis)` — smooth rotation; damp the angle toward the new gravity up-vector over the 0.4s blend so the capsule visibly tumbles then rights itself
- Particle emitter at feet on landing; emitter at torso on gravity shift

## Narrative Arc

Kael must traverse 7 sections of the shattered Aether Institute, each one a different expression of broken gravity. He is searching for the Core's emergency shutdown terminal. Along the way he pieces together what happened — and what *he* caused.

---
