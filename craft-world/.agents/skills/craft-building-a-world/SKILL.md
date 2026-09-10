---
name: craft-building-a-world
description: "Read this when the task is to CREATE or EXPAND a world, a game system, or a complete playable game rather than just edit or sync files. Covers player experience, the separate completion bars for each outcome, content depth, cross-linking, images, maps, and the finish checklist."
---

# Building Worlds, Systems, and Games

The other instruction topics cover the mechanics of editing and syncing.
This one covers the OUTCOME: what does "done" look like when a user asks for
a world, a region, a cast of characters, a game system, or a playable game?

For an entire project or any large multi-file build, read the bulk-work topic
before starting. The main agent should settle the project settings, overview,
system, file types, roster, and dependency order, then delegate disjoint file
batches to capable subagents and integrate their work in waves.

## Start with the intended player experience

Craft games look and feel like visual novels while playing with the freedom
of tabletop RPGs. Before deciding on folders or fields, identify what players
will repeatedly see, choose, roll, discover, and change. Build toward those
moments: evocative characters and scenes for presentation, clear rules for
adjudication, and durable state the GM can update as play changes the world.
Do not mistake a large lore archive for a good game experience.

## Choose the right completion bar

The user's intent decides which outcome you are building:

- **World or setting** — characters, locations, factions, items, and lore that
  interlock and create situations worth playing. It may be system-neutral.
- **Game system** — resolution rules, player-facing concepts, file types and
  schemas that model them, layouts that make them usable, GM instructions that
  apply them, and representative examples that prove the pieces work together.
- **Playable game** — a world and system composed into a specific player
  experience, with a game start, playable characters or character creation,
  GM persona/instructions, and persistent state for its important mechanics.

Everything below describes the DEFAULT target for substantial creation work.
The user's stated intent overrides any line of it. A text-only setting bible,
a lore wiki with no game layer, or a rules reference is FINISHED when it
matches what the user asked for. Omitting a feature on purpose is a design
choice, not a defect: never add images, maps, or playability metadata merely
to quiet a `craft project-completeness` warning (image generation spends
the user's real energy). Name deliberate omissions in your summary so the
user can opt in later.

## The completeness bar

A finished piece of Craft world content is more than valid JSON. Before
you call worldbuilding work done, each significant file should have:

- **A real description** — what a player SEES or experiences, in the
  world's voice. One flavorless sentence ("A reliable ranged weapon") is a
  placeholder, not a description. Two to five specific sentences beat one
  generic one.
- **GM truth where the schema offers it** (`notes`, `background`,
  secrets fields): the hidden fact, hook, or complication that makes the
  entry PLAYABLE, not just describable. The best test files in a project
  interlock: an NPC's secret references a location's rumor references an
  item's history.
- **Working cross-links.** The mechanism depends on the file's content
  type: JSON records link through REFERENCE FIELDS (characters carry
  gear, factions point at rivals — add a reference field to the schema if
  none fits); MARKDOWN bodies link through `<@slug.type>` mentions
  (GM instructions naming the cast, lore naming places). Mentions are a
  markdown-body feature — don't paste mention tokens into JSON string
  fields expecting chips. A world where nothing references anything is a
  list, not a world. See the references-and-computed-fields topic for syntax.
- **Coherent, executable mechanics.** Numbers must mean something relative to
  each other: if one sword is `value: 5` and another `value: 90`, the text
  should justify the gap. A mechanic is not complete until the project says
  how the GM applies it and where its changing state lives. Put durable state
  in schema-backed files and behavior in GM instructions; use layouts to make
  the same state legible to players. A bag of stat blocks with no rules that
  read or update them is not a system.
- **An updated game start.** The starter "Tavern Start" text is template
  filler. If the world has real content, rewrite the game start (and GM
  instructions) to open INSIDE that content, naming real people and
  places.

## Images

Craft's play experience has the visual presentation of a visual novel: art,
characters, dialogue, and scenes should help players inhabit the fiction while
their choices remain as open-ended as a TTRPG. In the app, every file's image
is its face on cards, in chat, and on the project page. A visual project whose
key records lack images looks unfinished even when the text is excellent.

- **Store the art direction in project settings FIRST.** Consistent art
  comes from `settings.imageStyleInstructions` in `.craft/project.json`
  (applied to every generation automatically — yours, the app's, and the
  GM's), not from repeating a style phrase in each prompt. Decide the
  world's visual style, write it into the settings, push, then generate.
  Types that need their own look (portraits vs item icons) get a per-type
  override in `.craft/file-types/<slug>.json` — see the
  configuring-your-project topic.
- When the user asked you to build or illustrate content, generating
  images for the IMPORTANT records (main characters, signature locations,
  the project's tone-setters) is part of the job — read the
  generating-images topic for the command, prompt shape, and the energy
  rules, and tell the user roughly how many generations you plan before
  running a batch.
- Derive each prompt from the file's own description/appearance text —
  subject only, since the stored style is composed in automatically. One
  good prompt per record; do not retry-loop.
- Not every file needs art. Prioritize: THE PROJECT COVER first
  (`settings.image` in `.craft/project.json` — it is the project's
  face in every list and share card; generate it like any other image
  and paste the object), then cover-worthy records (protagonists,
  villains, major locations), commodity records (ten variants of
  arrows) last or never. While in `.craft/project.json`, fill `tags`
  and the `headline`/`body` page copy — a finished world with a bare
  project page still reads as abandoned.

## Maps

When spatial exploration matters to the requested experience, the finished
world should have a map. Maps live on location files and are authored with
`craft map set` (see the authoring-maps topic): use a root/world map with a
background image, regions drawn as areas, and significant locations pinned and
LINKED to their files. Set the root map via
`settings.rootMapFileReferenceId` in `.craft/project.json`. Skip this when
the requested game or reference work is deliberately non-spatial.

## Playability

If the world is meant to be played, configure it: a complete game start
(starting location on the map, playable pregen characters, an opening in
the world's voice), the GM persona, and GM instruction rules. See the
making-it-playable topic — content without this metadata is a wiki, not
a game (which is fine when a wiki is what the user asked for).

## Scratch hygiene

Test stubs poison a world's credibility ("Test Ritual", "Repro Note",
"asdf"). Before finishing:

- Delete every experiment/scratch file you created and push the deletions
  (deletes are soft — recoverable from the project's trash in the Craft app;
  the CLI has no restore command, so recovery is an app-side step).
- Re-read starter files ("Example Character", "Example GM Instructions")
  — either rework them into real world content or delete them once real
  content replaces their role.

## Finish checklist

Run `craft check` FIRST — it VALIDATES the workspace locally with the
same checks the server runs at push (content vs type schemas, expressions,
references, settings). Fix every ✗ failure: those mean the server rejects
the work, never a design choice. Then run `craft project-completeness` —
it reports the machine-checkable half of this bar (art direction, cover,
root map, cross-links, page copy, game start, GM instructions, image
coverage, descriptions, scratch files). It reports on the WHOLE project,
so it belongs at the end of building or finishing a world; after a small
addition to an existing project, verify the files you touched instead and
leave pre-existing warnings alone. Every completeness finding is a
recommendation measured against the full playable-world profile — fix it
or consciously accept it, and a deliberately narrower project accepts the
ones describing its omissions and says so in the summary. A clean
`craft check` plus a read of every completeness warning is the floor,
not the finish — the items below are the judgment layer a checker cannot
see:

1. `craft status --remote` clean; everything pushed.
2. Every significant file: description with actual flavor, GM-facing
   truth, at least one cross-link where it makes sense.
3. Mechanics coherent; every important system has rules that explain when and
   how the GM reads or updates its schema-backed state.
4. Game start and GM instructions reference the real world, not template
   filler.
5. The COVER PAGE is dressed: cover image (`settings.image`), tags,
   headline + body, and a "Glimpses" gallery of your best art
   (`settings.gallery`); art direction in
   `settings.imageStyleInstructions`; GM persona matching the tone
   (`.craft/project.json` — see configuring-your-project).
6. Key records have images (if the user wanted art and energy allows).
7. If spatial exploration matters, a root map exists with a background image,
   linked pins/areas for significant locations, and
   `settings.rootMapFileReferenceId` set (authoring-maps topic).
8. Playability configured: game start (location + playable characters +
   opening), GM persona, GM instruction rules (making-it-playable topic);
   for worlds meant for campaigns, a character creation wizard and a short
   prelude (authoring-flows topic).
9. No scratch files left; starter placeholders reworked or removed.
10. Consider `craft publish -m "..."` as a checkpoint once the world is
    in a good state — ask the user first.
