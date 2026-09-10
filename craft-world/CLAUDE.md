# AETHER — Gravity — Craft workspace

World bible for AETHER: a 2.5D gravity-manipulation puzzle-platformer. Documents player characters, level locations, game mechanics, enemy types, and narrative lore for the C++ OpenGL game.

This folder is a **checkout** of the Craft project **AETHER — Gravity** (`projectId: 01a074ae-3133-748c-a99f-bee9b011c57f`) from https://api.craftrpgs.com. Edit the files with any tools — nothing about editing requires the `craft` CLI. Two ways to get changes back into Craft (details under "Syncing changes back"):

- **The `craft` CLI (recommended)** — incremental `status → pull → edit → push` with validation, conflict detection, and extra authoring commands.
- **Platform import (no CLI)** — zip this folder (`.craft/` included) and import it in the Craft app under **Project settings → CDF Import / Export**. Same server-side validation as a push, with a full preview before anything applies.

## What you are building

Craft is a system-agnostic AI RPG platform: creators can build any kind of
TTRPG instead of relying on hard-coded support for a particular ruleset. A
finished project should give players a compelling experience that looks and
feels like a visual novel while preserving the freedom, rules, and
improvisation of a tabletop RPG.

This workspace is the game's shared model. Files hold the world's content and
changing state; file types are reusable blueprints; schemas define what data
means and what may be written; layouts define how that data appears to people.
During play, the Game Master is an AI agent that follows GM instructions, reads
the project context, and reads or updates these files as the fiction changes.
There is no universal built-in memory, quest, combat, or campaign-state system:
when a game needs one, define its behavior in GM instructions and its durable
state in suitable schemas and files.

## What is here

- **Working tree** — the files you edit. Named `Name.<file-type>.<md|json>` and laid out in real folders that mirror the project.
  - `.md` files are markdown with YAML frontmatter (`name`, optional `description`/`image`) followed by the body.
  - `.json` files are JSON objects that must satisfy the schema in `.craft/file-types/<type>.json`.
  - Each file carries a small `$craft` block (top-level JSON key / frontmatter key) holding its `referenceId` and per-file settings. Leave it alone; when you copy a file to make a new one, delete the copy's `$craft` block.
- **`.craft/`** — editable project configuration plus machine-owned sync state.
  - `workspace.json` — what this checkout is and where it came from.
  - `index.json` — THE LEDGER: sync bookkeeping — the base version/hash each file was last synced at (identity itself travels in each file's `$craft` block).
  - `project.json` — EDITABLE project settings (name, description, art direction via `settings.imageStyleInstructions`, GM persona, …). Edit and `craft push` like any file — see `.craft/instructions/configuring-your-project.md`.
  - `file-types/<slug>.json` — EDITABLE type definitions: each type's JSON Schema + layout, plus its own image generation settings (`imageStyleInstructions`, `showCoverImage`). Pushes with `--include-types`.
  - `base/<fileId>.json` — cache of each file's synced content, used for offline diffs and conflict merges.

## Editing rules

- A file's `name` (frontmatter or JSON `"name"`) is its identity in Craft. If you rename the file, keep the base filename in sync — or move it with `craft mv <from> <to>`, which records the move in the ledger (durable: you can keep editing the file before pushing). (After `craft mv`, the content `name` still holds the old value until `craft push` rewrites it — expected; don't hand-edit it in between.) The MCP `write_file` path basename is also authoritative when editing remotely. **When syncing with `craft push`, never shell-rename a file AND edit its content in the same motion** — rename recovery pairs by content hash, so a changed file becomes delete+create on push: the original is trashed, a new identity is minted, and inbound references silently dangle. `craft mv` first, then edit. (Via platform import this hazard does not exist: matched files keep their identity from the embedded `$craft` block, and imports never move files — rename a record by editing its content `name`.)
- Only use the file type extensions listed below — Craft rejects unknown types.
- Files with `"source": "package"` in `.craft/index.json` are managed by an installed package and are **read-only** through sync; edit them in the Craft app, not here.
- Under `.craft/`, edit only `project.json` and `file-types/*.json` as
  described by their instruction topics. Never hand-edit `workspace.json`,
  `index.json`, `base/`, the instruction files, or other machine-owned sync
  state unless a `craft` command explicitly tells you to.
- If two files would collide on disk, the export suffixes one with ` (fileId)`. Prefer the original name in the Craft app until sync is fully id-addressed.

## Syncing changes back

Use the CLI loop when the `craft` CLI is installed and authenticated;
otherwise finish the editing work and use platform import. A missing or
broken CLI never blocks the editing itself — everything editable here is
a plain file.

### The sync loop (CLI — recommended)

Authenticate once, then `status → pull → edit → push`. Exit codes:
`0` ok · `1` operational failure (conflicts, push rejected, network) ·
`2` usage/refusal (bad args, plan refused). Two push failure wordings, both
failures: "Push refused" = the CLI's local preflight stopped it (exit 2,
nothing sent); "Push rejected" = the server turned the batch down (exit 1).
Scripts should key off the exit code, not the wording.

1. `craft status --remote` — see what changed locally and remotely (add `--json` for machine-readable output). Open conflicts from a prior pull are listed here; resolve them before pushing.
2. `craft pull` — fast-forward files changed in the app and refresh schemas. If a file changed both here and in the app, pull writes a `Name.<type>.remote.<md|json>` sidecar next to it and records the conflict.
3. **Edit** the working files. Validate JSON against `.craft/file-types/<type>.json`.
4. Resolve any conflicts: make the working file what you want, then `craft resolve <path>` (keep local; deletes the sidecar and advances the base), or `craft checkout <path>` to take the remote version.
5. `craft push -m "what changed"` — validates every file against its schema and sends one atomic batch. A lost race (someone pushed the same file first) is normally caught by the CLI's own preflight as a `stale_files` refusal; a true server `409` can still happen in a tight race. Either way the remedy is the same: `craft pull`, re-merge, push again. Use `craft push --json` for scripting (refusals and failures include remediation `hint` fields).

Schema changes are never pushed by an ordinary `push`; that needs `craft push --include-types`.

### Platform import (no CLI)

Finish your edits, then have the user land them through the app:

1. Leave `.craft/` machine state exactly as it was; just make sure your
   edited files are saved.
2. Zip the whole workspace folder, `.craft/` included.
3. The user drags the zip into **Project settings → CDF Import / Export**
   in the Craft app. They get a dry-run preview of every file and setting
   change, with per-row veto, before anything applies. (To create a NEW
   project from the zip instead, use the app's /import page.)

Import validates exactly like a push, but its semantics differ:

- **Imports never delete.** Removing a file locally does not remove it in
  Craft — deletions need `craft push` or the app.
- **Imports never move matched files.** On-disk renames/moves of existing
  files do not propagate; content lands at each file's current in-app
  path. Rename a record by changing the `name` field inside its content.
- New files need no identity block: the import mints a stable id from the
  path. When you copy an existing file to start a new one, DELETE the
  copy's `$craft` block — two files claiming the same id are both
  rejected.
- Cross-references between new files: use slug forms
  (`<@new-guy.character>` in markdown, slug reference objects in JSON) —
  they resolve within the same import batch.
- Schema (`.craft/file-types/`) and `.craft/project.json` edits ride
  along: the app imports types and settings by default, each veto-able in
  the preview.

### If a `craft` command is failing

- `unrecognized_keys` / `invalid_shape` errors on `.craft/workspace.json`
  or other sync state mean the installed CLI is OLDER than this export.
  Fix: `npm install -g @craftrpgs/cli@latest`. Never hand-edit
  machine-owned `.craft/` state to satisfy an old CLI.
- `craft login` needs the user at a browser. If login can't happen in
  this session, keep working and sync via platform import.
- Either way, report the CLI problem to the user once, then continue the
  actual work — do not spend the session diagnosing the CLI.

### What still needs the CLI or the app

Plain file edits + platform import cover content files, folders, schemas,
layouts, and project settings. The rest needs the `craft` CLI — or the
user doing it in the Craft app:

- Deleting files or folders (imports never delete).
- Generating or uploading images (`craft image generate` /
  `craft image upload`).
- Maps (`craft map set`), playability metadata (`craft meta`), the
  voice catalog (`craft voices`).
- The local validation and completeness audits (`craft check` /
  `craft project-completeness`) and publishing releases (`craft publish`).
  Without them, the platform import's dry-run preview is the validation
  gate.

Without the CLI, finish the content work, then tell the user exactly
which of these steps remain and where to do them — never improvise
around them.

## Worldbuilding quality bar

When the task is CREATING content (not just editing/syncing), read
`.craft/instructions/building-a-world.md` first. The short version: real
descriptions (not one flavorless line), GM-facing secrets/hooks, files that
cross-reference each other, coherent numbers, a game start that opens
inside the real world, images on the key records (see
`generating-images.md` for the energy rules), and no scratch/test files
left behind. Store the world's art direction in
`settings.imageStyleInstructions` (`.craft/project.json`) BEFORE
generating images — it is composed into every generation automatically, so
prompts stay subject-only. Maps are authored with `craft map set` on
location files (`authoring-maps.md`) — a finished world has a root map
with a background image and linked pins. Playability metadata (game start
config, playable flags, GM triggers) is set with `craft meta`
(`making-it-playable.md`).

The bar above describes a COMPLETE playable world — the default target,
not a mandate. The user's stated intent scopes it: a text-only setting
bible or a lore wiki with no game layer is finished when it matches what
the user asked for. Never add images, maps, or playability metadata
nobody asked for just to satisfy the checklist.

Run `craft check` after schema or content edits — it validates the
workspace locally with the same checks the server runs at push; ✗
failures mean the push would be rejected: always fix them. Run
`craft project-completeness` when you built or finished a whole world —
it reports the ENTIRE project against this bar locally (no network);
after a small edit, skip it and verify just the files you touched. Its
warnings measure completeness against a fully playable world: fix them,
or consciously accept the ones that describe what this project
deliberately omits — and tell the user which you accepted and why.

## Releases

- `craft publish -m "notes"` — snapshot the project's **server** state as a numbered release (private checkpoint by default). Unpushed local edits are not included — `craft push` first if you meant to.
- `craft publish --public` also makes the project visible on Discover. **Ask the user before passing `--public`** — never make a project public without explicit consent.
- `craft releases` — list existing releases.

Without the CLI, the user can publish from the app once changes are imported.

## Detailed instructions (read before non-trivial work)

Deep, task-specific instructions live in `.craft/instructions/` (also
installed as skills under `.claude/skills/craft-*/` for Claude Code and
`.agents/skills/craft-*/` for Codex). Read the
relevant topic BEFORE creating file types, designing layouts, writing
reference or computed fields, or making non-trivial file edits:

- `.craft/instructions/bulk-work.md` — Read this BEFORE creating a new project, designing a game system, creating many files, or starting any other large batch — covers the main-agent plan, capable subagent selection, dependency waves, nested delegation, and centralized verification and sync ownership (CLI push or platform import).
- `.craft/instructions/building-a-world.md` — Read this when the task is to CREATE or EXPAND a world, a game system, or a complete playable game rather than just edit or sync files. Covers player experience, the separate completion bars for each outcome, content depth, cross-linking, images, maps, and the finish checklist.
- `.craft/instructions/configuring-your-project.md` — Read this before changing project-level settings (.craft/project.json — name, description, art direction / imageStyleInstructions, default image model, GM persona, headline, the Player's Handbook / Builder's Manual) or a file type's own image generation settings — all of it is editable from the workspace and syncs on push. It covers the settings file's own rules; the writing-handbooks and project-gallery topics cover what goes INSIDE the two heaviest settings.
- `.craft/instructions/writing-handbooks.md` — Read this before writing, rewriting, or adding a chapter to the Player's Handbook (settings.playerHandbook) or Builder's Manual (settings.builderManual) in .craft/project.json. Covers which material belongs in which book and which belongs in neither, chapter structure, file mentions, images, and voice.
- `.craft/instructions/project-gallery.md` — Read this before setting settings.gallery or settings.galleryLayout in .craft/project.json — the 'Glimpses' showcase strip on the project's cover page. Covers the item shape (kind, url, caption, focalPoint, animation), where media URLs may come from, and what each layout does with the order you write.
- `.craft/instructions/authoring-maps.md` — Read this before creating or editing a map — the interactive map on a location file (background image, regions, pins, tokens, grid, the project root map, map hierarchy). Covers the craft map get/set commands, the authoring JSON, and what a finished map contains.
- `.craft/instructions/making-it-playable.md` — Read this before finishing a world someone will actually PLAY — covers game start configuration (starting location, starting characters, opening), the character playable flag, GM instruction triggers and the craft meta commands that set them, and context visibility (pinned/partial/searchable/hidden — what the GM sees by default, and why pinning must be applied in the app).
- `.craft/instructions/authoring-flows.md` — Read this before writing a flow manifest — the project's cinematic prelude (settings.preludeFlow in .craft/project.json) or a file type's guided creation wizard (creationFlow in .craft/file-types/<slug>.json). Covers the manifest JSON, the screen catalog, branching, and how answers feed AI character generation.
- `.craft/instructions/file-types-and-schemas.md` — Read this BEFORE creating a new file type or editing any schema in .craft/file-types/ — covers craft type new, schema field types and constraints, server-injected base fields, and how craft push --include-types validates schema changes against every existing file.
- `.craft/instructions/refactoring-file-types.md` — Read this BEFORE auditing, reviewing, improving, or cleaning up schemas in .craft/file-types/ — "audit my file types", "improve my schemas", "review this file type", "clean up/modernize this schema" — or when a schema shows bad patterns: duplicated derived fields, numbered slots, per-level or per-class branches, resource counters, copied reference properties, presentation strings built by expressions, or placeholder computed fields.
- `.craft/instructions/designing-layouts.md` — Read this BEFORE creating, editing, or auditing the layout field of any .craft/file-types/<slug>.json — "audit my layout", "improve this sheet" — covers the layout spec format, every available component with its exact props and enum values, $state data binding, visibility conditions, repeat + editPath editable trackers, the layout audit workflow, and how to push layout changes safely.
- `.craft/instructions/references-and-computed-fields.md` — Read this when a schema has (or you are adding) reference fields that point at other files, or computed fields (declared with an expression key). Covers the hydrated {slug, referenceId, fileTypeSlug} form in cloned files, writing new references by slug, referencing not-yet-pushed files, multi-target qualified slugs, the expression language, and why computed values never appear in file content.
- `.craft/instructions/editing-files.md` — Read this before creating, editing, renaming, moving, or deleting content files (Name.<type>.json / .md) in the workspace — covers name-as-identity, markdown frontmatter, JSON schema validation on push, and package-managed read-only files.
- `.craft/instructions/generating-images.md` — Read this before generating an image for a workspace file (cover art, portrait, scene illustration) with `craft image generate`, or uploading a local image file with `craft image upload` — covers flags, the account opt-ins and daily caps, reference images, and how to apply the returned image object to a file.

These files are machine-managed and refreshed by `craft pull` — do not edit
them (edits get overwritten). Keep personal or project-specific agent notes
somewhere else, outside `.craft/`, `.claude/skills/craft-*/`, and
`.agents/skills/craft-*/`.

## Auth

Credentials are never stored in this folder.

- **`craft login`** — sign in with your browser; the CLI stores its own credentials (in `~/.config/craft/`) and refreshes them automatically.
- **MCP OAuth**: `.mcp.json` wires the `craft` MCP server; connect with `claude` / Codex and authenticate via `/mcp` when prompted.

## MCP (remote-only workflows)

`.mcp.json` points at https://api.craftrpgs.com/mcp. When you have this local checkout, prefer the `craft` CLI loop above. The per-file MCP tools (`read_file`, `write_file`, `move_path`, `delete_path`) still exist for editing the project **without** a local checkout — they act directly on the remote using the same VFS paths as these files. Pass `expectedVersionId` on writes when you care about concurrent app edits.

## File types

| Extension | Name | Content |
| --- | --- | --- |
| `.character.md` | Character | markdown |
| `.enemy.md` | Enemy | markdown |
| `.location.md` | Location | markdown |
| `.mechanic.md` | Mechanic | markdown |
