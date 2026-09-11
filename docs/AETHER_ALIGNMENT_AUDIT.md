# AETHER — ALIGNMENT AUDIT
> Generated per AETHER_MASTER_ALIGNMENT_REBUILD.md §44 (First Action)  
> Status: PHASE 0 — AUDIT

---

## AUDIT TABLE

| Area | Current Implementation | Documented Design (REBUILD.md) | Status | Required Fix | Priority |
|---|---|---|---|---|---|
| **Control Scheme — Gravity** | Arrow keys (UP/DOWN/LEFT/RIGHT) switch gravity | W/S/A/D switch gravity | MISMATCH | Remap gravity switching to WASD | HIGH |
| **Control Scheme — Movement** | A/D keys move player left/right | Arrow keys move player | MISMATCH | Remap movement to Arrow keys | HIGH |
| **ESC key** | Closes game immediately | Opens Pause Menu | MISMATCH | ESC must open PAUSED screen | HIGH |
| **Game Flow — Startup** | Launches DIRECTLY into Level 1 | Splash > Disclaimer > Main Menu > New Game | MISSING | Implement full startup flow | HIGH |
| **Splash Screen** | None | AETHER / GRAVITY REIMAGINED splash | MISSING | Create splash screen | HIGH |
| **Disclaimer** | None | Academic/educational disclaimer | MISSING | Add disclaimer screen | HIGH |
| **Main Menu** | None | NEW GAME / CONTINUE / LEVEL SELECT / SETTINGS / CREDITS / EXIT | MISSING | Implement full main menu | HIGH |
| **Pause Menu** | None | PAUSED: Resume / Restart / Settings / Main Menu | MISSING | Implement pause menu (ESC) | HIGH |
| **Level Select** | None | All 7 levels, only L1 unlocked initially | MISSING | Implement level select with unlock system | MED |
| **Settings** | None | Resolution / Fullscreen / VSync / Audio / Controls | MISSING | Implement settings screen | MED |
| **Credits** | None | Credits screen required | MISSING | Implement credits screen | MED |
| **Ending / Game Complete** | Prints to console, restarts | Ending sequence -> Credits -> Main Menu | PARTIAL | Implement proper ending sequence | HIGH |
| **Kael Character** | Generic cube (0.8w x 1.8h box) | Real character: head, hair, torso, arms, coat, Aether burn mark | PLACEHOLDER | REQUEST BLOCKING ASSET | BLOCKING |
| **ARIA** | None | Holographic projector node, cyan translucent, 5 states | MISSING | REQUEST BLOCKING ASSET | HIGH |
| **Sentinel Enemies** | Generic cube with patrol logic | Proper low-poly model, 4 variants | PLACEHOLDER | Request/create Sentinel models | MED |
| **Wraith Enemies** | Generic cube with tracking logic | Translucent violet gravity-anomaly entity | PLACEHOLDER | Request/create Wraith model | MED |
| **Level Names (display)** | level1–level7 (internal only) | The Arrival Hall, Vertical Maze, etc. | MISMATCH | Rename in display/UI | HIGH |
| **Level 1 — The Arrival Hall** | Generic gray box level | Dark charcoal, amber emergency lighting, ARIA node | PLACEHOLDER | Full visual overhaul | HIGH |
| **Level 2 — The Vertical Maze** | Functional generic layout | Deep teal-green, vertical corridor, Sentinel-Standard | PLACEHOLDER | Lighting + environment pass | MED |
| **Level 3 — The Storage Vaults** | Has pressure plates and crates | Burnt orange/rust, 3 named puzzles, Sentinel-Standard | PARTIAL | Visual pass; verify crate push | MED |
| **Level 4 — The Relay Chambers** | Has conduit valves/doors | Midnight blue, conduit signal pulse VFX, AND-gate, Wraith | PARTIAL | Conduit pulse VFX + verify AND logic | MED |
| **Level 5 — The Zero Field** | Has gravity fields | Deep purple, gravity field borders/indicators/particles, 0.3g | PARTIAL | Indicators + particles missing | MED |
| **Level 6 — The Machinery Ring** | Static layout only | Rotating platforms (T*R*T matrix), oscillating platforms, timed gates | MISSING | Rotating/oscillating platforms NOT implemented | HIGH |
| **Level 7 — The AETHER Core** | Has valves, no final sequence | Auto-cycling gravity every 30s, HUD countdown ring, 14-step final sequence | MISSING | Cycling gravity, countdown, shutdown sequence | HIGH |
| **Objective System** | None | Per-level objective text updating on progress | MISSING | Implement objective UI | MED |
| **HUD — Gravity Cooldown** | None | Cooldown ring (Bresenham arc) around gravity indicator | MISSING | Bresenham circle arc on HUD | MED |
| **Lighting — Per Level** | Single static point light | Level-specific palettes (amber L1, teal L2, rust L3, etc.) | MISSING | Per-level light config | MED |
| **3D Models / Textures** | None (all procedural cubes) | Textured props, platforms, doors, switches, crates | MISSING | Asset pipeline not established | HIGH |
| **Particles / VFX** | None | Gravity shift pulse, field flow, Aether burn, conduit pulse | MISSING | Particle system needed | MED |
| **Audio System** | None | SFX for all interactions, music, ARIA dialogue events | MISSING | Audio system required | MED |
| **ARIA Dialogue System** | None | ARIA event system for tutorial, warnings, narrative | MISSING | ARIA dialogue event system | MED |
| **CG: DDA** | IMPLEMENTED in game (orange Sentinel paths) | DDA for path visualization | DONE | None |
| **CG: Bresenham** | IMPLEMENTED in game (cyan gravity field lines) | Bresenham for line visualization | DONE | None |
| **CG: Transformations** | IMPLEMENTED (translate/rotate/scale per object) | MVP matrices, T/R/S | DONE | None |
| **CG: MVP Pipeline** | IMPLEMENTED (lit.vert/lit.frag, Blinn-Phong) | OpenGL MVP rendering | DONE | None |
| **CG: AABB Collision** | IMPLEMENTED (Physics2D.cpp) | Custom physics | DONE | None |
| **CG: Euler Integration** | IMPLEMENTED (Physics2D.cpp) | Newtonian motion | DONE | None |
| **CG: Circle/Arc HUD** | NOT in game (assignments only) | HUD cooldown ring (Bresenham circle) | MISSING | Add to cooldown HUD | MED |
| **CG: Rotating Platform Matrix** | NOT implemented | T_pivot * R(time) * T_arm hierarchy for Level 6 | MISSING | Required for Level 6 | HIGH |
| **Asset Licenses doc** | None | docs/ASSET_LICENSES.md | MISSING | Create per §32 | MED |
| **Asset Manifest** | None | assets/ASSET_MANIFEST.md | MISSING | Create per §65 | MED |

---

## SUMMARY COUNTS

| Status | Count |
|---|---|
| DONE (fully working) | 6 |
| PARTIAL / PLACEHOLDER | 8 |
| MISSING / NOT IMPLEMENTED | 26 |

---

## BLOCKING ASSETS — REQUIRED BEFORE PHASE 2

Per REBUILD.md §58, these are BLOCKING assets. Phase 2 (Character Presentation) CANNOT proceed without them.

1. **Kael 3D Character** — No model exists. Player is a primitive box.
2. **ARIA Projector** — No visual entity exists anywhere in the engine.
3. **Sentinel Model** — All enemies are generic cubes.
4. **Fracture Wraith Model** — All enemies are generic cubes.

---

## NEXT STEP — PHASE 1: APPLICATION FLOW

Per REBUILD.md §44: begin with PHASE 1 — APPLICATION FLOW.

Phase 1 does NOT require character assets and implements:
- Splash screen (AETHER / GRAVITY REIMAGINED)
- Disclaimer screen
- Main Menu (NEW GAME / CONTINUE / LEVEL SELECT / SETTINGS / CREDITS / EXIT)
- Pause Menu on ESC (Resume / Restart Level / Settings / Main Menu)
- Control scheme fix: WASD = gravity switching, Arrow keys = left/right movement

AWAITING USER APPROVAL TO BEGIN PHASE 1.
