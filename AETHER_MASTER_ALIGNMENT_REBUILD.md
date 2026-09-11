# AETHER — AUTHORITATIVE DESIGN LOCK & IMPLEMENTATION REBUILD

> **Purpose:** This document is the authoritative instruction set for bringing the current AETHER prototype into alignment with the already-defined AETHER project.
>
> **Critical rule:** Do **not** invent a new game plan. Do **not** redesign AETHER. Do **not** rename the game, characters, levels, story, mechanics, or scope. Read the existing AETHER `.md` design files and the supplied CGG presentation/PDF before making major changes.

---

## 1. PROJECT IDENTITY — LOCKED

**Game:** AETHER  
**Subtitle:** GRAVITY REIMAGINED  
**Genre:** 2.5D physics-based puzzle-platformer  
**Platform:** Windows PC  
**Language:** C++  
**Graphics:** OpenGL  
**Mode:** Single Player  
**Core Mechanic:** Gravity Manipulation

AETHER is a focused Computer Graphics & Gaming project.

### AETHER is NOT

- an FPS
- a generic platformer
- an RPG
- an open-world game
- a multiplayer game
- a shooter
- a combat-focused game
- a random sci-fi demo
- a collection of disconnected graphics demonstrations

The current prototype is the starting point. The goal is to turn it into the **fully presented and documented AETHER game**, not replace it with another concept.

---

# 2. SOURCE-OF-TRUTH RULE

Before modifying gameplay, inspect:

1. Every relevant AETHER `.md` file in the repository.
2. Character documentation.
3. Enemy documentation.
4. All Level 1–7 documentation.
5. Gravity System documentation.
6. Physics System documentation.
7. Puzzle Objects documentation.
8. Current implementation/status documentation.
9. The supplied AETHER/Wolfenstein Computer Graphics & Gaming presentation/PDF.

### Priority

When interpreting the existing project:

**Detailed AETHER design MDs**
→ gameplay mechanics, characters, levels, story, object behavior, physics values

**AETHER CGG presentation/PDF**
→ project scope, academic objectives, visual direction, CG algorithms, architecture, presentation expectations

**Current source code**
→ current implementation to be audited, repaired, and improved

If the current implementation conflicts with the detailed design, align the implementation with the documented design.

### DO NOT

- create a different game
- invent a different story
- invent different levels
- rename the seven levels
- replace Kael with another protagonist
- replace ARIA with another character
- add unrelated mechanics
- simplify the game into a generic platformer
- create an alternative project plan

---

# 3. CURRENT IMPLEMENTATION STATUS

The current project already contains substantial prototype systems.

Treat existing working systems as valuable.

Expected existing areas include:

- C++ OpenGL rendering
- player/controller
- gravity system
- physics
- AABB collision
- camera
- level manager
- puzzle objects
- enemies
- multiple levels
- transformations
- basic visual effects

However, the prototype still needs to become a **complete presentable game**.

Current quality gaps to address include:

- direct launch into gameplay
- missing professional startup flow
- missing proper main menu
- missing disclaimer
- missing settings
- missing pause menu
- missing polished level select
- weak objective presentation
- placeholder character presentation
- primitive-looking environments
- insufficient visual identity per level
- missing/unfinished audio
- insufficient asset pipeline
- weak story presentation
- weak level-to-level progression
- insufficient final-game polish

Do not remove the existing core mechanics just because the presentation layer is unfinished.

---

# 4. CHARACTER REQUIREMENT — REAL CHARACTERS, NOT CUBES

## 4.1 KAEL MUST BE A REAL CHARACTER

The final game must have an actual visible character representing **Kael**.

A cube/capsule/placeholder primitive is acceptable only during temporary development.

It is **NOT acceptable as the final player character**.

Kael should visually communicate:

- young male researcher
- lean/wiry body
- short dark windswept hair
- torn research coat
- graphene underlayer
- luminous Aether burn mark on left arm
- non-combat-oriented researcher
- gravity-linked protagonist

### Required character presentation

Create or integrate a proper stylized/low-poly 3D character asset.

At minimum the final Kael asset must have:

- head
- hair
- torso
- arms
- legs
- coat/research clothing silhouette
- recognizable proportions
- Aether burn mark/emissive detail
- materials/textures
- proper orientation for the gravity system

If a ready asset is not available, create a custom AETHER-styled character asset rather than leaving a primitive placeholder.

### Character pipeline

The project should support:

- character mesh
- materials
- textures
- animation if available
- idle
- running
- jumping/falling
- gravity transition
- landing
- death/reset
- interaction

The implementation may use a simpler custom low-poly character if a fully rigged model is not yet available, but it must still visually read as **Kael**, not as a cube.

---

# 5. OTHER REQUIRED CHARACTERS

AETHER should have proper visual character assets for the documented entities.

## 5.1 ARIA

ARIA is the Aether Residual Intelligence Array.

ARIA is **not the player**.

She appears through holographic projector nodes.

Visual identity:

- cyan hologram
- translucent/emissive presentation
- futuristic projection
- fragmented visual state later in the game

ARIA states:

- IDLE
- PULSING
- SPEAKING
- FRAGMENTED
- OFFLINE

ARIA provides:

- tutorial guidance
- environmental information
- warnings
- narrative progression
- final transmission

Do not turn ARIA into a playable character.

---

## 5.2 SENTINELS

Use the documented Sentinel family.

Required variants:

- Sentinel-Standard
- Sentinel-Heavy
- Sentinel-Swarm
- Sentinel-Phase

They are gravity/puzzle hazards, not conventional shooter enemies.

They should have proper visual models or coherent custom low-poly enemy assets.

Do not leave enemies as generic cubes in the final presentation.

---

## 5.3 FRACTURE WRAITHS

Use the documented Wraith family:

- Fracture Wraith
- Spiral Wraith
- Mirror Wraith

They should visually appear as:

- translucent
- violet
- gravity-anomaly entities
- non-conventional enemies

Do not replace them with random monsters.

---

# 6. CONTROL SCHEME — UPDATED AND LOCKED

The gravity controls are being changed from the previous F/R/Q/E design.

## GRAVITY

Use **WASD for the four gravity directions**:

| Key | Gravity |
|---|---|
| **W** | UP |
| **S** | DOWN |
| **A** | LEFT |
| **D** | RIGHT |

The four gravity directions remain:

- DOWN = `(0,-g)`
- UP = `(0,+g)`
- LEFT = `(-g,0)`
- RIGHT = `(+g,0)`

## PLAYER MOVEMENT

Use the **Left / Right Arrow keys** for horizontal movement:

| Key | Action |
|---|---|
| **←** | Move left |
| **→** | Move right |
| **SPACE** | Jump |

Optional interaction:

| Key | Action |
|---|---|
| **E** | Interact |

### IMPORTANT

Because A/D are now gravity controls:

**A/D must NOT be used for normal left/right movement.**

Arrow keys are the normal horizontal movement controls.

W/S are gravity controls, not normal vertical movement.

### Gravity switching behavior

Keep the documented gravity behavior:

- gravity blend = 0.4 seconds
- cooldown = 0.8 seconds
- velocity is preserved
- player orientation rotates toward the new gravity frame
- grounded state is evaluated relative to current gravity
- camera performs subtle gravity transition feedback

Holding a gravity key must not repeatedly trigger switches.

Use edge-triggered gravity input.

---

# 7. KAEL MOVEMENT MODEL

Movement remains gravity-relative.

The world does not rotate.

Only the direction of gravity changes.

Grounded surfaces can therefore be:

- floor under DOWN
- ceiling under UP
- left wall under LEFT
- right wall under RIGHT

The same physics/controller logic should handle all four states.

Do not implement four separate movement systems.

---

# 8. EXACT LEVEL STRUCTURE — LOCKED

The seven detailed AETHER levels are:

1. **The Arrival Hall**
2. **The Vertical Maze**
3. **The Storage Vaults**
4. **The Relay Chambers**
5. **The Zero Field**
6. **The Machinery Ring**
7. **The AETHER Core**

Do not rename them to alternative names.

---

# 9. LEVEL 1 — THE ARRIVAL HALL

## Purpose

Tutorial and introduction.

Kael regains consciousness here.

## Environment

Ruined institutional arrival hall.

Visual direction:

- dark charcoal architecture
- warm amber emergency lighting
- cracked floor
- tilted pillars
- broken railings
- suspended dust
- flickering lights

Depth:

- background: receding corridor geometry
- gameplay plane: platforms and objects
- foreground: broken railings/dust

Gravity:

**DOWN**

Required:

- Kael
- ARIA node
- Gravity Switch UP
- Gravity Switch DOWN
- one gap
- checkpoint
- Goal/Exit

No enemies.

Teach:

- movement
- jumping
- first gravity shift
- basic interaction

The first gravity change must feel like a major gameplay discovery.

---

# 10. LEVEL 2 — THE VERTICAL MAZE

## Theme

Gravity Walls.

## Environment

Tall, narrow, claustrophobic gravity-routing corridor.

Visual direction:

- deep teal-green
- cool blue-white lighting
- gears
- pipes
- chains
- wall machinery
- vertical architecture

Gravity directions required:

- DOWN
- UP
- LEFT
- RIGHT

Required:

- pressure plate
- door
- Sentinel-Standard
- spikes
- checkpoint
- goal

Teach:

- left-wall gravity
- right-wall gravity
- wall traversal
- momentum management

---

# 11. LEVEL 3 — THE STORAGE VAULTS

## Theme

Weighted Objects.

Environment:

Industrial deep storage vault.

Visual direction:

- burnt orange/rust
- corroded steel
- dark shadows
- violet Aether crates
- amber emergency strips
- hanging chains
- cables
- vault doors

Signature object:

**AETHER CRATE**

Required puzzles:

1. The Low Gate
2. The High Shelf
3. The Wall Vault

Required mechanics:

- crate
- pressure plate
- door
- gravity changes
- Sentinel-Standard
- checkpoint
- gap

The player must understand that gravity affects objects too.

Crates must be pushable and usable as puzzle weights.

---

# 12. LEVEL 4 — THE RELAY CHAMBERS

## Theme

Relay Signal & Door Chains.

Environment:

Cold midnight-blue relay network.

Structure:

Chamber A
→ Chamber B
→ Chamber C
→ Goal

Important visual mechanic:

**CONDUITS**

When a valve activates, an energy pulse travels visibly through the conduit.

ARIA becomes silent here.

Required:

- Conduit Valve
- Signal Pulse
- Door/Gate
- AND gate logic
- gravity changes
- crate
- pressure plate
- Sentinel-Heavy
- Fracture Wraith

Documented relay logic must remain.

Do not replace the relay puzzle with a simple button.

---

# 13. LEVEL 5 — THE ZERO FIELD

## Theme

Scaled Gravity & Drift.

Environment:

Vast spherical null-gravity chamber.

Visual direction:

- deep purple
- violet-white crystals
- floating islands
- cyan/amber/red gravity-field indicators
- slow particles
- large atmospheric space

Global gravity:

`0.3 × 9.81 = 2.943 m/s²`

Introduces:

**localized Gravity Fields**

Fields have:

- direction
- strength
- border
- directional indicators
- particle flow

Required:

- UP field
- RIGHT field
- strong DOWN field
- floating crate
- pressure plate
- Sentinel-Phase
- drifting crystal hazards
- ARIA fragmented transmission

This level must feel substantially different from previous levels.

---

# 14. LEVEL 6 — THE MACHINERY RING

## Theme

Rotating Platforms & T*R*T Hierarchy.

Environment:

Massive mechanical infrastructure.

Visual direction:

- iron-grey
- orange hazard strips
- steel girders
- flywheels
- sparks
- rotating machinery

Core mechanics:

- rotating platforms
- oscillating platforms
- timed gates
- gravity fields
- gravity switching
- velocity inheritance

Rotating platform transform:

`T_pivot * R(time * speed) * T_arm_offset`

Oscillating platform:

`position = basePos + direction * sin(time * frequency) * amplitude`

Required:

- Rotating Platforms A/B/C/D
- Oscillating Platforms
- Timed Gates
- UP Gravity Field
- Sentinel-Swarm
- checkpoint
- goal

This is a major transformations/matrix showcase.

---

# 15. LEVEL 7 — THE AETHER CORE

## Theme

Final Challenge.

Environment:

Massive spherical chamber.

Core:

Huge fractured white-violet crystal.

Visual direction:

- deep black
- white-violet Core
- enormous scale
- particle storm
- four gravity arms
- orbital platforms
- dynamic lighting

The Core continuously cycles gravity:

DOWN
→ UP
→ LEFT
→ RIGHT
→ repeat

Each phase:

**30 seconds**

HUD:

Show the cycle as a countdown/ring.

Required mechanics:

- wall gravity
- ceiling walking
- crate puzzle
- gravity fields
- moving platforms
- timed gates
- Sentinel-Standard
- Sentinel-Heavy
- Fracture Wraiths
- shutdown terminal

Final sequence:

1. Activate Valve A.
2. Move crate onto ceiling plate.
3. Disable Sentinel-Heavy using gravity manipulation.
4. Reach central terminal.
5. ARIA delivers final transmission.
6. Activate terminal.
7. 10-second shutdown countdown.
8. Core collapses.
9. Gravity destabilizes.
10. Exit appears.
11. Reach exit before countdown ends.
12. Game completes.
13. Gravity normalizes.
14. Final corridor and sunlight.

Do not replace this ending.

---

# 16. GAME FLOW

The game must NOT launch directly into gameplay.

Required:

```text
AETHER SPLASH
      ↓
TITLE / INTRO
      ↓
DISCLAIMER
      ↓
MAIN MENU
      ↓
NEW GAME
      ↓
INTRO SEQUENCE
      ↓
LEVEL 1
      ↓
LEVEL 2
      ↓
LEVEL 3
      ↓
LEVEL 4
      ↓
LEVEL 5
      ↓
LEVEL 6
      ↓
LEVEL 7
      ↓
ENDING
      ↓
CREDITS
      ↓
MAIN MENU
```

---

# 17. SPLASH SCREEN

Show:

**AETHER**

**GRAVITY REIMAGINED**

Use the established AETHER visual identity.

---

# 18. MAIN MENU

Required:

- NEW GAME
- CONTINUE
- LEVEL SELECT
- SETTINGS
- CREDITS
- EXIT

Background should show an atmospheric AETHER environment.

Use subtle animation.

Do not make it a plain debug menu.

---

# 19. DISCLAIMER

Include an educational-project disclaimer.

Mention:

- academic purpose
- third-party asset licensing
- independent student project status
- asset/license information in Credits/documentation

Do not invent legal claims.

---

# 20. LEVEL SELECT

Show:

- Level 1 — The Arrival Hall
- Level 2 — The Vertical Maze
- Level 3 — The Storage Vaults
- Level 4 — The Relay Chambers
- Level 5 — The Zero Field
- Level 6 — The Machinery Ring
- Level 7 — The AETHER Core

Initially only Level 1 is unlocked.

Completed levels become available.

Do not allow locked-level access.

---

# 21. PAUSE MENU

ESC opens:

```text
PAUSED

RESUME
RESTART LEVEL
SETTINGS
MAIN MENU
```

Do not immediately close the game when ESC is pressed.

---

# 22. SETTINGS

## Graphics

- Resolution
- Fullscreen
- VSync
- Quality

## Audio

- Master Volume
- Music Volume
- SFX Volume

## Controls

Show the updated control scheme.

---

# 23. OBJECTIVE SYSTEM

Every level must communicate:

- where the player is
- what the current objective is
- what needs to be activated
- where the next meaningful destination is

Do not overfill the screen with UI.

Example:

```text
OBJECTIVE

Restore the Arrival Hall gravity node.
Find the gravity switch and reach the upper platform.
```

Objectives should update when appropriate.

---

# 24. PUZZLE OBJECT VOCABULARY

The core puzzle language is:

1. Pressure Plate
2. Door
3. Aether Crate
4. Gravity Switch
5. Gravity Field
6. Conduit Valve
7. Goal

Use signal IDs for activators and receivers.

Level wiring should remain data-driven.

Do not hardcode every puzzle relationship.

---

# 25. GRAVITY SYSTEM

Keep the existing documented gravity model.

```text
DOWN  = (0,-g)
UP    = (0,+g)
LEFT  = (-g,0)
RIGHT = (+g,0)
```

Baseline:

`g = 9.81 m/s²`

Parameters:

- blend = 0.4 s
- cooldown = 0.8 s
- player rotation = 0.4 s
- camera roll = approximately 2°
- preserve velocity during switch

Gravity vector should blend between states.

Grounding, friction and jump direction must operate relative to the current gravity frame.

---

# 26. PHYSICS SYSTEM

Keep the custom lightweight physics system.

Do not replace it with a third-party physics engine.

Use:

- Newtonian motion
- Euler integration
- AABB collision
- minimum penetration resolution
- gravity-relative grounding
- friction
- gravity overrides for fields

The player and crates must react physically to gravity.

---

# 27. 2.5D GRAPHICS

AETHER remains a 2.5D game.

Gameplay is primarily XY.

Z is used for visual depth.

Use:

- background depth layer
- gameplay layer
- foreground layer

Background machinery and foreground elements should add depth without interfering with gameplay.

---

# 28. RENDERING

Use OpenGL with:

- VAO
- VBO
- EBO where useful
- GLSL
- MVP matrices
- depth testing
- blending
- textures
- lighting
- materials

Do not leave final scenes as colored cubes.

---

# 29. LIGHTING

Level-specific lighting:

### Level 1
Warm amber emergency lighting.

### Level 2
Cool blue-white lighting.

### Level 3
Amber emergency lighting + violet crates.

### Level 4
Midnight blue + cyan conduit pulses.

### Level 5
Purple/violet atmosphere.

### Level 6
Iron-grey + orange hazard lighting.

### Level 7
Black + white-violet Aether Core.

Use point/emissive lighting for important interactive objects.

---

# 30. VISUAL FEEDBACK

Gravity changes should visibly communicate:

- new gravity direction
- player rotation
- particle pulse
- subtle camera tilt
- cooldown state
- field state

Keep the gravity transition readable and satisfying.

---

# 31. AUDIO

Add an audio abstraction/system.

Required sound categories:

- gravity shift
- jump
- landing
- crate landing
- crate dragging
- pressure plate
- door
- valve
- Sentinel alert
- Wraith contact
- level completion
- Core
- shutdown

Support ARIA dialogue/events architecturally.

If final voice files are not yet available, implement the event/dialogue system without blocking the rest of development.

---

# 32. ASSET PIPELINE

Before importing external assets, create an inventory.

Required asset categories:

- Kael
- ARIA projector
- Sentinel
- Wraith
- Aether Crate
- Gravity Switch
- Gravity Field visual
- Pressure Plate
- Door
- Conduit Valve
- Goal
- Aether Core
- platforms
- walls
- rocks
- ruins
- machinery
- pipes
- chains
- crystals
- particles
- UI
- fonts

For external assets document:

- asset name
- creator
- source
- license
- URL
- where used

Create/update:

`docs/ASSET_LICENSES.md`

Do not use copyrighted commercial-game assets.

The games shown in the PDF are benchmarking/inspiration references, not assets to copy.

---

# 33. CHARACTER/ASSET STYLE CONSISTENCY

Maintain one coherent AETHER visual language:

- dark cinematic science-fantasy
- industrial ruins
- gravity technology
- cyan energy
- violet anomalies
- amber emergency lighting
- deep shadows
- strong silhouettes
- 2.5D depth
- stylized low-poly/cinematic 3D presentation

Do not mix unrelated asset styles.

Avoid a scene containing:

- realistic character + cartoon props
- pixel-art environment + 3D character
- medieval objects + sci-fi machinery
- unrelated asset packs with different art direction

---

# 34. COMPUTER GRAPHICS / ACADEMIC INTEGRATION

AETHER must demonstrate the CGG concepts already defined in the project.

Relevant concepts include:

- DDA
- Bresenham line drawing
- circle drawing algorithms
- polygon filling
- transformations
- translation
- rotation
- scaling
- composite transformations
- MVP matrices
- lighting
- depth
- particles
- camera
- OpenGL rendering

Integrate algorithms meaningfully rather than displaying them as random decorations.

Examples already aligned with the design:

- DDA/Bresenham for documented path/line visualization
- circle algorithms for HUD/cooldown arcs
- polygon filling for appropriate 2D object indicators
- transformations for player orientation
- transformations for moving/rotating machinery
- matrix hierarchy for rotating platforms
- MVP pipeline for rendering

---

# 35. ACADEMIC RESTRICTION

Some standalone course assignments explicitly state **DO NOT USE AI FOR THE IMPLEMENTATION**.

Respect that restriction.

Do not generate prohibited standalone assignment implementations intended to be submitted as the student's own work.

For those assignments, assistance should be limited to appropriate explanation, debugging, testing, integration guidance, and conceptual support.

Where the assignment/game documentation explicitly permits AI-assisted game integration, keep the required references/documentation.

---

# 36. ENVIRONMENTAL STORYTELLING

Do not add huge amounts of exposition.

Use:

- ARIA transmissions
- terminal logs
- damaged infrastructure
- lighting
- architecture
- environmental clues
- Core behavior

The story should gradually reveal:

1. The Aether experiment failed.
2. Gravity became unstable.
3. Kael is connected to the field.
4. Kael was involved in starting the sequence.
5. ARIA knows more than she initially reveals.
6. The Core must be shut down.
7. The final shutdown resolves the system and ARIA's story.

---

# 37. LEVEL READABILITY

Every level must make the intended puzzle understandable.

Use:

- lighting
- landmarks
- object color coding
- conduits
- gravity arrows
- environmental framing
- particle effects
- door states
- ARIA guidance
- clear silhouettes

Do not solve readability by covering the screen with arrows.

---

# 38. NO UNRELATED FEATURES

Do NOT add:

- inventory
- XP
- loot
- skill tree
- crafting
- weapons
- guns
- large combat systems
- multiplayer
- online services
- open-world systems
- NPC towns
- random RPG mechanics
- dozens of new enemy types

These are outside the established AETHER scope.

---

# 39. DEVELOPMENT ORDER — LOCKED

Do not attempt to redesign/rebuild everything at once.

Use this sequence.

## PHASE 0 — AUDIT

Inspect:

- source tree
- renderer
- player
- camera
- gravity
- physics
- collision
- puzzle system
- level manager
- levels
- assets
- UI
- audio

Create:

`docs/AETHER_ALIGNMENT_AUDIT.md`

Table:

| Area | Current Implementation | Documented Design | Mismatch | Required Fix | Priority |
|---|---|---|---|---|---|

Do not make major gameplay changes during the audit.

---

## PHASE 1 — APPLICATION FLOW

Implement/fix:

- splash
- disclaimer
- main menu
- New Game
- Continue
- Level Select
- Settings
- Credits
- Pause

---

## PHASE 2 — CHARACTER PRESENTATION

Implement/integrate:

- proper Kael character
- character materials/textures
- gravity orientation
- movement animation
- jump/fall presentation
- landing
- Aether burn mark

Also establish proper visual assets for:

- ARIA
- Sentinel
- Wraith

Do not leave the final player/enemies as primitive cubes.

---

## PHASE 3 — LEVEL 1

Make **The Arrival Hall** fully playable and visually coherent.

It must include:

- Kael
- ARIA
- movement
- jumping
- gravity switching
- objective
- puzzle interaction
- checkpoint
- goal
- level completion
- transition to Level 2

---

## PHASE 4 — LEVEL 2

Bring **The Vertical Maze** specification into the actual game.

---

## PHASE 5 — LEVEL 3

Bring **The Storage Vaults** specification into the actual game.

---

## PHASE 6 — LEVEL 4

Bring **The Relay Chambers** specification into the actual game.

---

## PHASE 7 — LEVEL 5

Bring **The Zero Field** specification into the actual game.

---

## PHASE 8 — LEVEL 6

Bring **The Machinery Ring** specification into the actual game.

---

## PHASE 9 — LEVEL 7

Bring **The AETHER Core** specification into the actual game.

---

## PHASE 10 — VISUAL POLISH

Improve:

- lighting
- materials
- textures
- particles
- atmospheric effects
- camera
- VFX
- depth
- environment detail

---

## PHASE 11 — AUDIO

Add:

- music
- SFX
- ARIA dialogue/event system
- ambience

---

## PHASE 12 — FINAL QA

Test:

```text
Launch
↓
Splash
↓
Disclaimer
↓
Main Menu
↓
New Game
↓
Level 1
↓
Level 2
↓
Level 3
↓
Level 4
↓
Level 5
↓
Level 6
↓
Level 7
↓
Ending
↓
Credits
↓
Main Menu
```

---

# 40. LEVEL 1 IS THE FIRST QUALITY GATE

Do not create random assets for all seven levels before proving Level 1 works.

Level 1 must be a polished vertical slice of the final game.

A new player must understand:

- who Kael is
- where he is
- what happened
- what gravity manipulation does
- how to move
- how to jump
- how to switch gravity
- what the objective is
- where to go
- how to finish the level

If Level 1 still looks like a debug/test environment, do not move on to polishing every other level.

---

# 41. BUILD / TEST LOOP

After each phase:

1. Build.
2. Run.
3. Play.
4. Test the new feature.
5. Fix regressions.
6. Verify controls.
7. Verify level transition.
8. Continue.

For each completed phase, report:

```text
PHASE:
FILES CHANGED:
FEATURES IMPLEMENTED:
BUILD RESULT:
RUNTIME RESULT:
TESTED:
REMAINING ISSUES:
NEXT LOCKED PHASE:
```

---

# 42. DO NOT CLAIM FEATURES ARE COMPLETE WITHOUT TESTING

Do not mark something as:

- complete
- finished
- fully implemented
- working

unless it has actually been built and tested.

Distinguish:

- implemented
- partially implemented
- placeholder
- visually unfinished
- runtime-tested
- not yet tested

---

# 43. FINAL QUALITY BAR

A new player launches AETHER.

They see:

**AETHER — GRAVITY REIMAGINED**

They enter a professional main menu.

They start a new game.

They meet Kael.

They encounter ARIA.

They learn gravity manipulation.

They explore the Arrival Hall.

They solve the first puzzle.

Then the game progressively introduces:

### Level 1
Movement + Gravity

### Level 2
Wall Gravity

### Level 3
Objects + Pressure Plates

### Level 4
Relay Logic

### Level 5
Localized/Scaled Gravity

### Level 6
Moving Machinery + Transformations + Timing

### Level 7
All Mechanics Combined

The final sequence resolves the story.

The game ends with:

**Gravity normalizing.**

**Everything becoming still.**

**Sunlight at the end of the corridor.**

---

# 44. FIRST ACTION — DO THIS NOW

Before writing a large amount of code:

1. Read every AETHER `.md`.
2. Read the AETHER CGG PDF/presentation.
3. Inspect the current source tree.
4. Inspect the current implementation.
5. Compare implementation against documentation.
6. Identify missing and conflicting features.
7. Create:

`docs/AETHER_ALIGNMENT_AUDIT.md`

The audit must explicitly identify:

- current working systems
- current placeholders
- current design mismatches
- character/asset gaps
- UI/game-flow gaps
- level gaps
- physics/gravity mismatches
- graphics gaps
- audio gaps
- academic CG integration gaps

Then begin only with:

**PHASE 1 — APPLICATION FLOW**

Before Phase 2, identify whether a real Kael character asset is required. If it is missing, create the exact **ASSET REQUEST** and STOP.

**PHASE 2 — REAL CHARACTER/CHARACTER ASSETS**

Do not continue until the required character assets have been supplied/approved or a clearly documented temporary placeholder is explicitly accepted for that phase.

Before Level 1, identify ALL blocking Level 1 assets and request them in the exact asset-request format defined in this document.

**PHASE 3 — LEVEL 1**

Stop after Level 1 is genuinely playable and visually coherent.

For every later level, repeat the same process:

READ LEVEL MD
→ identify required assets
→ request missing blocking assets
→ WAIT
→ implement
→ build
→ test
→ report
→ proceed only after the checkpoint.

---

# 45. FINAL NON-NEGOTIABLE INSTRUCTION

## IMPLEMENT THE AETHER THAT IS ALREADY DEFINED.

The documentation is the blueprint.

The current source code is the prototype.

The presentation is the project/academic/visual reference.

Your job is to:

**AUDIT → ALIGN → IMPLEMENT → TEST → POLISH**

Do not:

**REINVENT → REDESIGN → ADD RANDOM FEATURES**

### AETHER remains:

**A 2.5D OpenGL gravity-manipulation puzzle-platformer.**

### Controls remain:

**W = Gravity UP**  
**S = Gravity DOWN**  
**A = Gravity LEFT**  
**D = Gravity RIGHT**  
**← / → = Move Left / Right**  
**SPACE = Jump**  
**E = Interact**

### Final character requirement:

**KAEL MUST BE A REAL, PROPERLY PRESENTED CHARACTER.**

ARIA, Sentinels and Wraiths must also have coherent character/entity visuals.

**Do not leave the final game using generic cubes as the main character or enemies.**


# 46. ASSET REQUEST GATE — MANDATORY STEP-BY-STEP WORKFLOW

## THIS RULE IS NON-NEGOTIABLE

Antigravity must **NOT assume that all assets are available**.

Whenever the next implementation phase requires an asset that has not already been supplied and verified, **STOP BEFORE IMPLEMENTING THAT DEPENDENCY** and ask the user for the asset.

This applies especially to:

- Kael/player character
- ARIA hologram
- Sentinel models
- Wraith models
- level/environment models
- rocks
- ruins
- machinery
- platforms
- doors
- switches
- Aether Core
- Aether Crates
- gravity-field visuals
- crystals
- pipes
- chains
- props
- level maps/layout files
- textures
- materials
- particle textures
- UI artwork
- fonts
- animation files
- audio/music
- special VFX assets

## DO NOT DO THIS

If a level needs a character model and it does not exist:

DO NOT:
- silently download a random model
- silently generate a random model
- substitute an unrelated model
- continue the level implementation using a permanent placeholder
- invent a different character
- change the documented visual style

Instead:

**STOP AND ASK THE USER.**

---

# 47. ASSET REQUEST FORMAT

Whenever an asset is required, create/update:

`docs/ASSET_REQUESTS.md`

Then report the request to the user in a clear format.

Use this exact structure:

```text
ASSET REQUEST — [LEVEL / SYSTEM]

Asset:
[exact asset name]

Required for:
[level/system]

Purpose:
[what the asset does in gameplay]

Character / Environment / Prop / UI / Audio / VFX:
[type]

Required visual style:
[exact AETHER style]

Reference:
[relevant AETHER MD section / documented design]

Required file format:
[GLB / GLTF / FBX / OBJ / BLEND / PNG / JPG / etc.]

Required texture formats:
[PNG / JPG / etc.]

Required maps:
[Base Color / Normal / Roughness / Metallic / Emission / Alpha]

Required animations:
[Idle / Run / Jump / Fall / etc.]

Approximate scale:
[dimensions or relative scale]

Pivot/origin requirement:
[center / bottom-center / custom]

Material requirements:
[opaque / transparent / emissive / etc.]

Performance target:
[low-poly / medium-poly / maximum approximate triangle count]

Where it will be placed:
[level/location]

Do we already have it?
[YES / NO]

ACTION REQUIRED FROM USER:
[exact thing the user must provide]
```

Do not ask vague questions such as:

> "Can you provide assets?"

Tell the user **exactly what asset is needed and exactly how it should be delivered.**

---

# 48. CHARACTER ASSET REQUESTS

When requesting a character, specify ALL requirements.

Example:

```text
ASSET REQUEST — KAEL

Type:
3D player character

Required format:
Preferred: GLB/GLTF
Acceptable: FBX
Optional source/editing file: BLEND

Required:
- full body
- head
- hair
- torso
- arms
- legs
- research coat
- graphene underlayer
- Aether burn mark on left arm

Animation:
- Idle
- Run
- Jump
- Fall
- Land
- Death
- Interact

Preferred:
Rigged humanoid
Consistent skeleton
Game-ready topology

Textures:
- Base Color
- Normal
- Roughness
- Metallic where required
- Emission for Aether burn mark

Texture format:
PNG preferred

Visual style:
Stylized cinematic low-poly 3D
Dark science-fantasy
AETHER visual language

Do not provide:
- weapons
- guns
- unrelated armor
- cartoon style
- anime style
- photorealistic style
```

If only a static character is available, STOP and tell the user that animation will need to be handled separately.

---

# 49. ENVIRONMENT / LEVEL ASSET REQUESTS

A level must not be built from random props.

Before building a level that needs custom environment assets, request the required asset package.

For each environment asset specify:

- exact object
- purpose
- approximate dimensions
- visual style
- material
- texture requirements
- collision requirements
- whether it is decorative or gameplay-critical
- expected file format
- origin/pivot
- desired polygon complexity

Example:

```text
ASSET REQUEST — LEVEL 3 STORAGE VAULTS

Required environment assets:

1. Storage Vault Door
2. Industrial Pillar
3. Hanging Cable Bundle
4. Vault Shelf
5. Aether Crate
6. Emergency Light
7. Floor Panel
8. Wall Machinery
9. Pipe Set
10. Chain Set

Preferred 3D format:
GLB/GLTF

Optional source:
BLEND

Textures:
PNG

Required maps:
Base Color
Normal
Roughness
Metallic
Emission where applicable

Style:
Industrial ruined AETHER facility
Burnt orange/rust
Corroded steel
Dark shadows
Violet Aether technology

Collision:
Required for:
- doors
- pillars
- shelves
- floor panels

Decorative only:
- cables
- chains
- small machinery
```

Then **STOP** until the user supplies/approves the assets or explicitly authorizes a documented asset source.

---

# 50. LEVEL MAP / LEVEL LAYOUT REQUESTS

If a level requires a map/layout that has not been supplied, do not invent a completely different layout.

Use the documented AETHER level design as the basis.

Ask the user for the required format or create a clearly labeled request.

Preferred formats for level planning:

### Option A — Visual design map

`PNG`

Recommended for:
- level layout reference
- room arrangement
- puzzle placement
- player route
- camera composition

### Option B — Editable level source

`BLEND`

Recommended when the level is being authored in Blender.

### Option C — Structured level data

`JSON`

Recommended for:
- object positions
- object types
- gravity
- puzzle connections
- triggers
- spawn points
- checkpoints
- goals

### Option D — Tiled map

`TMX`

Only use this if the actual project architecture is configured for Tiled.

Do NOT introduce Tiled merely because it exists.

For each level map request specify:

- world dimensions
- gameplay bounds
- player spawn
- checkpoint positions
- gravity switches
- gravity fields
- crates
- pressure plates
- doors
- enemies
- goal
- camera bounds
- background layers
- foreground layers
- puzzle sequence

---

# 51. 3D MODEL FILE FORMAT RULES

Use these conventions unless the existing project requires otherwise.

## Preferred runtime model format

**GLB / GLTF**

Advantages:

- meshes
- materials
- textures
- animations
- scene hierarchy

can be packaged together.

## Acceptable alternatives

**FBX**

Use when the supplied character/animation asset requires FBX.

**OBJ**

Use primarily for simple static geometry.

OBJ should NOT be the preferred format for animated characters.

**BLEND**

Use as the editable source file when assets are authored in Blender.

Do not require Blender source files at runtime unless the pipeline specifically needs them.

---

# 52. TEXTURE FORMAT RULES

Preferred:

**PNG**

Use for:

- UI
- transparency
- emissive masks
- important textures
- normal maps
- decals

JPG may be used for:

- opaque photographic/environment textures

Avoid JPG for:

- normal maps
- alpha textures
- UI
- emissive masks

Keep texture dimensions reasonable for the target PC.

Prefer power-of-two dimensions where practical:

- 512 × 512
- 1024 × 1024
- 2048 × 2048

Do not automatically use 4K textures.

---

# 53. CHARACTER ANIMATION FORMAT REQUIREMENTS

For animated characters, request:

- rigged model
- consistent skeleton
- animation clips
- rest pose
- correct root orientation
- correct scale

Preferred:

**GLB/GLTF**

or:

**FBX**

Required animation naming should be documented.

Example:

```text
Idle
Run
Jump
Fall
Land
Interact
Death
GravityTransition
```

Do not import a character with unknown skeleton/scale and pretend it is production-ready.

---

# 54. MATERIAL REQUIREMENTS

For assets requiring PBR materials, request as applicable:

- Base Color
- Normal
- Roughness
- Metallic
- Ambient Occlusion
- Emission
- Alpha

Do not request maps that the object does not need.

Examples:

Aether Core:
- Base Color
- Normal
- Roughness
- Emission

Aether hologram:
- Base Color
- Emission
- Alpha

Rusty pillar:
- Base Color
- Normal
- Roughness
- Metallic

---

# 55. ASSET NAMING CONVENTION

Use predictable names.

Examples:

```text
SM_ = Static Mesh
SK_ = Skeletal Mesh
MAT_ = Material
T_ = Texture
TX_ = Texture Variant
AN_ = Animation
VFX_ = Visual Effect
SFX_ = Sound Effect
UI_ = User Interface
LVL_ = Level
```

Examples:

```text
SK_Kael
SK_Sentinel_Standard
SK_Sentinel_Heavy

SM_Aether_Crate
SM_Gravity_Switch
SM_Aether_Core
SM_Relay_Door

T_Kael_BaseColor
T_Kael_Normal
T_Kael_Roughness
T_Kael_Emission

AN_Kael_Idle
AN_Kael_Run
AN_Kael_Jump
AN_Kael_Fall

VFX_GravityShift
VFX_AetherPulse
```

Do not rename supplied source assets destructively.

Use project-side names where necessary.

---

# 56. ASSET IMPORT CHECKLIST

Before using a supplied asset, verify:

- file opens correctly
- mesh is valid
- scale is reasonable
- orientation is correct
- origin/pivot is correct
- materials load
- textures load
- normals are correct
- transparency works if required
- animation exists if required
- collision is appropriate
- triangle count is reasonable
- asset style matches AETHER

If anything fails, report it.

Do not silently replace the asset.

---

# 57. ASSET REQUEST TIMING

The asset request must happen **before the implementation phase that depends on it**.

Example:

If Level 3 requires:

- Aether Crate
- storage door
- shelves

do not start the final Level 3 environment implementation first.

Instead:

```text
READ LEVEL 3 DESIGN
        ↓
IDENTIFY REQUIRED ASSETS
        ↓
CHECK EXISTING ASSETS
        ↓
CREATE ASSET REQUEST
        ↓
ASK USER
        ↓
WAIT
        ↓
IMPORT / GENERATE / APPROVE ASSETS
        ↓
BUILD LEVEL 3
```

---

# 58. NEVER PROCEED PAST A BLOCKING ASSET

If an asset is **essential** to the current phase, stop.

Example:

If Kael's final character is required before completing the player presentation:

```text
STOP

Blocking Asset:
Kael Character

Reason:
Final player presentation cannot be completed correctly without the intended character asset.

Required:
Rigged 3D character in GLB/GLTF or FBX.

Waiting for:
User-provided/approved Kael asset.
```

Do not continue to unrelated implementation just to appear productive.

---

# 59. NON-BLOCKING ASSETS

Not every asset must stop the entire project.

Classify each asset:

### BLOCKING

The feature cannot be correctly completed without it.

Examples:

- Kael for final character presentation
- Aether Core for final level
- required level-specific gameplay object

→ **STOP AND ASK**

### NON-BLOCKING

A temporary development substitute is acceptable.

Examples:

- temporary dust texture
- temporary background rock
- temporary UI icon

→ Continue with a clearly marked placeholder, but record it.

Never allow a temporary placeholder to silently become the final asset.

---

# 60. AI ASSET GENERATION WORKFLOW

If a custom asset should be AI-generated, do not simply generate a random asset and insert it.

First provide the user with:

```text
AI ASSET REQUEST

Asset:
Aether Core

Purpose:
Final Level 7 centerpiece

Visual description:
[exact prompt/specification]

Style:
AETHER dark science-fantasy / cinematic low-poly

Required output:
[concept image / texture / model / reference]

Reference images:
[list]

Required dimensions/aspect ratio:
[exact requirement]

Next step:
User approval required.
```

If an AI image is only a concept/reference and not a runtime-ready 3D model, clearly distinguish the two.

Do not claim that a generated concept image is a game-ready 3D model.

---

# 61. WEB ASSET SOURCING WORKFLOW

If an asset is needed from the web:

1. Identify the exact asset category.
2. Prefer permissive/CC0 sources.
3. Check the actual license.
4. Record creator and source.
5. Record the URL.
6. Verify that commercial/academic use is allowed under that license.
7. Show the user the shortlisted asset(s) when approval is appropriate.
8. Do not silently import random assets.

Preferred sources may include appropriate permissive libraries such as Kenney or OpenGameArt, but the license of the **specific asset** must always be checked.

Do not copy assets from commercial games.

Do not use:

- Mario assets
- Zelda assets
- Doom/Wolfenstein assets
- Portal assets
- Hollow Knight assets
- other copyrighted commercial-game assets

The visual references in the AETHER presentation are references only.

---

# 62. ASSET APPROVAL RULE

For major visual assets, ask the user to approve before building the level around them.

Major assets include:

- Kael
- ARIA
- Sentinels
- Wraiths
- Aether Core
- major environment kit
- major level-specific machinery
- major UI style

Use:

```text
OPTION A
[asset description]

OPTION B
[asset description]

RECOMMENDED:
[choice + reason]

WAITING FOR USER APPROVAL.
```

Do not proceed with the major visual integration until the user chooses or explicitly authorizes the recommendation.

---

# 63. ONE PHASE AT A TIME

AETHER must be developed with a strict human-in-the-loop workflow.

For every major phase:

```text
PHASE START
    ↓
READ RELEVANT MD/PDF
    ↓
AUDIT CURRENT IMPLEMENTATION
    ↓
IDENTIFY REQUIRED ASSETS
    ↓
REQUEST MISSING ASSETS
    ↓
WAIT FOR USER
    ↓
IMPLEMENT
    ↓
BUILD
    ↓
RUN
    ↓
TEST
    ↓
REPORT
    ↓
REQUEST NEXT ASSETS IF NEEDED
    ↓
WAIT
    ↓
NEXT PHASE
```

Antigravity must NOT jump from Level 1 directly to implementing all remaining levels.

---

# 64. REQUIRED USER CHECKPOINTS

Before moving to each major stage, ask for confirmation when the stage depends on assets or visual decisions.

Required checkpoints:

### CHECKPOINT 1
Application flow approved.

### CHECKPOINT 2
Kael character approved.

### CHECKPOINT 3
ARIA/enemy visual direction approved.

### CHECKPOINT 4
Level 1 asset set approved.

### CHECKPOINT 5
Level 1 playable and visually approved.

### CHECKPOINT 6
Level 2 asset set approved.

Continue the same pattern for Levels 3–7.

Do not assume approval.

---

# 65. ASSET MANIFEST

Maintain:

`assets/ASSET_MANIFEST.md`

For each asset:

```text
Asset ID:
Name:
Type:
Level:
Source:
Creator:
License:
URL:
File:
Format:
Textures:
Animations:
Collision:
Runtime Usage:
Status:
```

Status values:

- REQUESTED
- WAITING
- RECEIVED
- VERIFIED
- IMPORTED
- INTEGRATED
- APPROVED
- REPLACED
- NOT NEEDED

This prevents assets from being lost or forgotten.

---

# 66. FINAL ASSET AUDIT

Before final release, verify:

- every required character exists
- every required environment asset exists
- every gameplay-critical object exists
- every level has its required visual assets
- all external assets have license records
- all runtime paths are relative
- no accidental copyrighted assets were imported
- no temporary placeholder remains where a final asset was required
- all character animations work
- all textures load
- all materials render correctly
- all assets match the AETHER visual language

