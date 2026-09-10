---
name: craft-making-it-playable
description: "Read this before finishing a world someone will actually PLAY — covers game start configuration (starting location, starting characters, opening), the character playable flag, GM instruction triggers and the craft meta commands that set them, and context visibility (pinned/partial/searchable/hidden — what the GM sees by default, and why pinning must be applied in the app)."
---

# Making a Project Playable

Content makes a project readable; instructions, state, and playability metadata
make it PLAYABLE.

## How the GM runs the game

The Game Master is an AI agent operating much like a human GM with a binder of
notes: it follows the project's GM instructions, receives relevant project
context, and reads or writes files as play changes the fiction. Craft does NOT
silently provide a universal memory system, quest tracker, combat engine,
relationship model, or campaign state. If the game needs one, the project must
define all four pieces:

1. **Rules** — GM instructions say when and how the mechanic operates.
2. **State model** — a schema defines the fields the mechanic can read/write.
3. **Current state** — files hold the actual quest, memory, encounter, clock,
   relationship, inventory, or other changing data.
4. **Context strategy** — visibility and instruction triggers put the right
   rules/state in front of the GM at the right moment.

For example, a quest system needs more than quest prose: a quest schema might
define status and objectives, quest files hold their current values, and GM
instructions say which events advance them and when to update those files.
Design every custom mechanic through this same rules + schema + files + context
loop. If one piece is missing, the mechanic is only documentation, not a
reliable part of play.

## Starting the experience

When the user presses Play, the game start decides where they are, who they can
be, and what the GM says first. Its app-managed metadata is set with
`craft meta` (files must be pushed first; the CLI advances the sync ledger
automatically):

```
craft meta get "Game Starts/Opening Night.game-start.md"
craft meta set-game-start "Game Starts/Opening Night.game-start.md" --file gs.json
craft meta set-playable "Heroes/Mira.character.json" true
craft meta set-gm-trigger "GM/Toll Rules.gm-instructions.md" --file trigger.json
```

These `craft meta` commands need the CLI. Working without it: complete
the content files (the game-start body, characters, GM instructions), then
list the remaining playability steps — game start config, playable flags,
triggers — for the user to finish in the app.

## Game start configuration

The game start FILE's markdown body is GM context; the metadata is the
machine config. Reference every file by workspace path (preferred; slugs
and plain file names also resolve) — never write ids; the CLI hydrates
links (and derives beat ids and the plain-text `message` projection) on
save:

```json
{
  "startingLocation": "Places/Last Chime.location.json",
  "startingCharacters": ["Heroes/Mira.character.json", "Heroes/Brann.character.json"],
  "opening": {
    "kind": "static",
    "beats": [
      { "speaker": "gm", "text": "The last bell cracked at dusk. Rain hammers the shutters." },
      { "speaker": "Heroes/Mira.character.json", "expression": "wary", "text": "Told you the bell would crack." }
    ]
  },
  "characterOpenings": [
    {
      "character": "Heroes/Brann.character.json",
      "opening": {
        "kind": "static",
        "beats": [
          { "speaker": "gm", "text": "You are last to the chapel, boots soaked through." }
        ]
      }
    }
  ],
  "openingInstructions": "Open quiet and personal; introduce the Hush by absence, not exposition.",
  "displayOrder": 0
}
```

- `startingLocation` — where play begins; should be a location that is
  ON the map.
- `startingCharacters` — the pregens offered to the player. Each must
  ALSO be flagged `craft meta set-playable <path> true` or it cannot be
  chosen in the app.
- `opening` — how the first turn opens. Prefer `"kind": "static"` with
  `beats`: each beat is one line of the scripted opening — `"speaker":
  "gm"` for narration, or a character link for a dialogue card with that
  character's portrait (in chat and the visual-novel story view). It plays
  the moment the player picks a character. The alternative,
  `{ "kind": "dynamic", "prompt": "…" }`, has the GM improvise from your
  prompt instead — fresh every game, but the player waits for the GM's
  first generation. Write openings in the world's voice; the template
  "Welcome to the game…" is the tell of an unfinished project.
- Each beat takes an optional `expression` — a one-word delivery cue from
  the speaker's expression states (the character file type's `states`, or
  the default set: neutral, happy, wary, angry, …). It swaps in the
  matching portrait variation in the story view; an unknown cue falls back
  to the base portrait.
- `characterOpenings` — optional per-character overrides of `opening`,
  played when the player picks that `character`. The top-level
  `opening` stays the required fallback for everyone else. Overrides
  should always be `static` beat openings — the app edits them as
  scripted only.
- `openingInstructions` — private direction for the GM's first turn.

A finished project has at least one COMPLETE game start: location +
characters + opening all set, referencing real, pushed files.

## GM instruction triggers

GM instruction files (the "gm_instructions" designation) come in two
kinds:

- **Always-on** (no trigger metadata): world rules the GM should always
  hold — system mechanics, tone, table rules. Body length is unlimited.
  Most projects want ONE of these carrying the core rules — and it is
  only actually standing context if it is PINNED (see context visibility
  below).
- **Triggered** (`set-gm-trigger`): fired at specific moments. LIMITS:
  name ≤ 120 chars, body ≤ 500 chars — write these as tight directives.
  - `{"triggerKind": "narrative", "triggerPhases": ["post_player_message"]}`
    — evaluated after each player message (or `post_gm_message`).
  - `{"triggerKind": "tool", "toolEvents": ["update"], "fileTypeSlugs": ["character"]}`
    — fired when the GM reads/creates/updates/deletes files of the named
    types (e.g. "when a character sheet changes, check the Hush Marks").
  - `--clear` removes the trigger, making the rule always-on again.

## Context visibility — what the GM sees by default

Every file resolves a context visibility — its own setting when the app
configured one, else its file type's default:

- `pinned` — full content always in GM context. A pinned
  `gm_instructions` file is the GM's STANDING instructions; a pinned
  file of any other type (a party tracker, a clock, a quest log) is kept
  fresh in context automatically — the right setting for mutable play
  state.
- `partial` — name + description sit in baseline context; the GM reads
  the body on demand. For things the GM should know exist.
- `searchable` — the default: not in context, found via file search
  when relevant. Right for bulk reference (rules chapters, catalogs).
- `hidden` — invisible to GM context and search.

Keep the pinned set SMALL (progressive disclosure): one short core-rules
instruction plus pointers to searchable reference files beats six pinned
chapters crowding the context window.

Visibility is app-managed and does NOT travel through the workspace:
working files and `craft meta` don't carry it, and pushed files resolve
to their type's default (`searchable` unless the app configured
otherwise). Files created from the app's GM instructions panel arrive
pinned; CLI-pushed ones do not — so a core-rules file authored here is
not standing context until it is pinned in the app. When the design
needs pinning or partial visibility, hand the user a short checklist
(file → visibility) to apply in the app's file settings.

## Letting players make their OWN character

Pregens cover the quick start; a finished campaign project also carries a
CREATION WIZARD on the character type — `creationFlow` in
`.craft/file-types/character.json`, a flow manifest whose answers feed
AI character generation. See the authoring-flows topic for the format and
what a good wizard asks. Push with `--include-types`.

## The GM persona

Set `settings.gm` in `.craft/project.json` (name, personality,
voiceId) — the personality should sound like the world, not like a
default assistant. See configuring-your-project.

## Playability checklist

1. Each important mechanic has GM rules, schema-backed state files, explicit
   update behavior, and an intentional context/trigger strategy.
2. One complete game start (location + playable characters + opening +
   openingInstructions), `displayOrder` set when there are several.
3. Every offered character flagged playable; their sheets are finished
   records, not stubs.
4. Core rules in an always-on GM instruction; 1–3 triggered rules for the
   moments that define the system (short and imperative).
5. GM persona set; opening references the map's starting location.
6. Visibility applied in the app: core rules pinned, trackers pinned or
   partial — hand the user the file → visibility checklist.
7. `craft meta get` on each configured file to verify what stuck.
