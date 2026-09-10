# Authoring Flows

A flow is a slide deck the app PLAYS: full-screen slides, optional
branching, optional questions whose answers are collected. Two things in
a project are flows, and both are plain JSON you can author:

- **The prelude** — `settings.preludeFlow` in `.craft/project.json`.
  Plays in the game lobby before first play: the world's cold open. Push
  like any settings change.
- **A type's creation wizard** — `creationFlow` in
  `.craft/file-types/<slug>.json` (usually the character type). Plays
  when a player makes their OWN character: its answers + each screen's
  `ai_instruction` hints feed the AI generation of the new file. Push
  with `--include-types`.

The CLI validates both against the flow engine's schema at push time and
refuses broken manifests with field-level errors.

## Manifest shape

```json
{
  "id": "prelude",
  "version": 1,
  "title": "Cold Static",
  "start": "n1",
  "nodes": [
    { "id": "n1", "type": "cover",
      "data": { "title": "COLD STATIC", "subtitle": "Coriolis Station, day 4,211" },
      "advance": { "mode": "timer", "seconds": 5 } },
    { "id": "n2", "type": "statement",
      "data": { "title": "Nine days of silence.", "body": "Telemetry stopped without a distress call…" } },
    { "id": "n3", "type": "choice", "field": "approach",
      "data": { "prompt": "How do you dock?", "layout": "cards",
        "options": [
          { "id": "quiet", "label": "Run dark and listen first" },
          { "id": "loud", "label": "Announce yourself on all bands" }
        ] } }
  ],
  "edges": [
    { "id": "e1", "source": "n1", "target": "n2" },
    { "id": "e2", "source": "n2", "target": "n3" }
  ],
  "settings": { "generationInstructions": "Grounded hard sci-fi; competence over heroics." }
}
```

- `start` names the first node. A node with no outgoing edge is an ending.
- **Linear** = one plain edge per hop. **Branching** = one edge per choice
  option, carrying `"sourceHandle": "<option id>"`. Branches reconverge by
  sharing a `target`. Only single-select `choice` screens branch.
- `field` is the key an answer lands under. REQUIRED on screens that collect
  an answer, and must be absent on display-only screens.
- `advance` — `{"mode":"manual"}` (default), `{"mode":"timer","seconds":n}`,
  or `{"mode":"media"}` (when the background video ends; the default for
  `video` screens). Question screens always behave as manual.
- Optional per-node `transition` (`"slide"` | `"fade"` | `"zoom"` |
  `"rise"` | `"none"`), `audio` (a media ref that plays over the flow's
  music while that screen is up), and `typography`.
- Flow-wide `settings`: `music` (`{track, loop?, volume?}`),
  `typography`, `generationInstructions` (author guidance the host feeds
  to whatever the flow's answers drive — the flow engine never acts on it).
- **Media refs** are `{"url": …, "kind": "image"|"video"|"audio",
  "focalPoint"?: {x, y}, "overlayUrl"?, "animation"?}`. URLs must be
  Craft-hosted — never invent one.

## Screen catalog (`type` → `data`)

Every cinematic screen also accepts these presentation keys on `data`:
`eyebrow`, `background` (media ref), `backgroundFill` (a CSS
gradient/solid used when there's no media), `vignette` (number),
`anchor` (`"lower-left"` | `"centered"`), `accent` (color).

Display screens (no `field`):
- `cover` — `{title, subtitle?}`. The big title card. Text-only content;
  art comes from `background`.
- `statement` — `{title?, body?}`. One dramatic beat per screen.
- `reading` — `{title?, body?}`. Long prose in a scrollable column (a
  found letter, a log). Defaults to `anchor: "centered"`.
- `video` — `{background, anchor?}` only; no cinematic keys. Full-bleed
  clip, up to 30s.

Question screens (set `field`; `ai_instruction` on `data` tells a
downstream generator what the answer MEANS):
- `choice` — `{prompt, options, multiple?, optional?, maxSelections?,
  layout?, compact?}`. `layout` is `"list"` | `"cards"` | `"grid"` |
  `"select"`. Each option is `{id, label, image?, info?, ai_instruction?}`
  and may curate a real project file by carrying `referenceId`, `slug`,
  and `fileTypeSlug`. The only branchable screen.
- `shortText` / `longText` — `{prompt, placeholder?, optional?}`.
- `number` — `{prompt, min?, max?, optional?}`.
- `rating` — `{prompt, max?, optional?}`. A 1–N scale; `max` defaults
  to 5.

Stat-assignment screens are legal in a prelude but belong to character
creation wizards. All three share `{prompt, stats: [{id, label}],
ai_instruction?, optional?}` plus:
- `statFixedArray` — `{values: [15, 14, 13, 12, 10, 8]}`.
- `statPointBuy` — `{budget, minScore, maxScore, defaultScore, costs}`
  where `costs` is an ARRAY of `{"score": n, "cost": n}` objects giving
  each score's cumulative cost (NOT a score-keyed map). See below.
- `statRollDice` — `{assignmentMode: "assign"|"inOrder"|"perStat",
  dieCount, dieSides, keepCount, keepMode: "highest"|"lowest",
  rerollAtOrBelow, modifier}`.

## A creation wizard, end to end

Same format, different surface — and the one worth copying literally,
because it shows branching, reconvergence, and the exact `costs` array:

```json
{
  "id": "make-a-pilot",
  "version": 1,
  "title": "Make a Pilot",
  "start": "c1",
  "nodes": [
    { "id": "c1", "type": "choice", "field": "berth",
      "data": { "prompt": "Where did you serve?",
        "ai_instruction": "Sets the character's background, debts and contacts.",
        "options": [
          { "id": "salvage", "label": "Salvage crews", "ai_instruction": "Scavenger skills; owes someone." },
          { "id": "navy", "label": "System navy", "ai_instruction": "Drilled, ranked, and resented for it." }
        ] } },
    { "id": "c2", "type": "statPointBuy", "field": "stats",
      "data": { "prompt": "Spend 27 points.",
        "stats": [
          { "id": "grit", "label": "Grit" },
          { "id": "wits", "label": "Wits" },
          { "id": "nerve", "label": "Nerve" }
        ],
        "budget": 27, "minScore": 8, "maxScore": 15, "defaultScore": 8,
        "costs": [
          { "score": 8, "cost": 0 }, { "score": 9, "cost": 1 },
          { "score": 10, "cost": 2 }, { "score": 11, "cost": 3 },
          { "score": 12, "cost": 4 }, { "score": 13, "cost": 5 },
          { "score": 14, "cost": 7 }, { "score": 15, "cost": 9 }
        ] } }
  ],
  "edges": [
    { "id": "ce1", "source": "c1", "sourceHandle": "salvage", "target": "c2" },
    { "id": "ce2", "source": "c1", "sourceHandle": "navy", "target": "c2" }
  ],
  "settings": { "generationInstructions": "Competent, scarred, specific. No chosen ones." }
}
```

## Reading one accurately

The manifest is a graph, not a list: node order in `nodes` says nothing
about play order. To describe what a player actually sees, walk
`start` → `edges` → an ending, and follow each `sourceHandle` branch
separately.

## What a good one looks like

- **Prelude**: 4–8 screens. Cold open: a `cover` title card, then a
  `video` node carrying the establishing ART (its `background`
  mediaRef accepts `"kind": "image"` — the cover screen itself is
  text-only). Then 2–3 statement/reading beats that establish stakes IN
  THE WORLD'S VOICE, at most one choice for tone. It ends where the game
  start begins — do not spoil GM truth, and do NOT restate the game
  start's own opening message: the two play back-to-back, so read the
  game start first and write the prelude to hand off INTO it.
- **Creation wizard**: 3–6 screens. Ask what the GENERATOR needs (name
  is automatic): concept/background choice (link real faction/heritage
  files where they exist, via an option's `referenceId` from
  `.craft/index.json` plus `slug` and `fileTypeSlug`), one or two
  flavor questions with `ai_instruction` hints, and a stat screen
  matching the system's numbers — or, when the system's "stats" are
  narrative (no point-buy or dice), a `choice`/text screen whose
  `ai_instruction` states the value ranges instead. Set
  `settings.generationInstructions` to the same bar your pregens meet.
- **Keep stat numbers consistent** with the character schema they feed —
  AND with the project's pregen characters: if your shipped pregens carry
  a stat the wizard's `maxScore`/`costs` cannot produce, players will
  notice the wizard sells them short. Copy the point-buy example above
  only after checking it against your actual cast.
- Never author a flow you haven't linearly traced: follow start → edges
  → an ending for every branch.
