---
name: craft-editing-files
description: "Read this before creating, editing, renaming, moving, or deleting content files (Name.<type>.json / .md) in the workspace — covers name-as-identity, markdown frontmatter, JSON schema validation on push, and package-managed read-only files."
---

# Editing Workspace Files

AGENTS.md covers how edits get back to Craft (the CLI sync loop, or
zipping the workspace for platform import); this covers what makes an
individual edit valid — the rules are the same whichever path lands them.

## File anatomy

Every content file is `<Name>.<file-type-slug>.<md|json>` —
`Aragorn.character.json`, `Fall of Gondor.lore.md`. The extension pair
identifies the file type; only extensions listed in `.craft/file-types/`
are valid. Folders are real directories and mirror the project; new
directories push automatically as folders.

**The name inside the content IS the file's identity.** The JSON `"name"`
field (or markdown frontmatter `name`) must match the base filename. To
rename a file use `craft mv <from> <to>` — it records the move in the
ledger (durable: editing the file's content afterwards is fine) and the
push updates filename and content name together. If you rename out-of-band,
change the filename AND the name field consistently; `craft status` recovers
simple renames by content hash but ambiguous cases require `craft mv`.

**WARNING (CLI push path) — never combine an out-of-band rename with a
content edit.** Hash
recovery only pairs a delete+untracked when the content is identical (or
identical except `name`). Shell-`mv` a file AND change its content before
the next `craft` command, and status sees an unrelated delete + create: the
push then trashes the original file, mints a new identity, and every inbound
reference and mention silently keeps pointing at the trashed file. `craft
push` prints a warning when a batch deletes and creates files of the same
type — if you see it and a rename was intended, stop: restore the old path,
run `craft mv`, then re-apply the edit.

Syncing via platform import instead? Matched files keep their identity from
the embedded `$craft` block and imports never move files: on-disk renames
of existing files do not propagate. Rename a record by changing the `name`
field inside its content.

Editing only the `name` field (without `craft mv` or a filename change)
also works as a rename: the server renames the file's canonical path from
the content name on push. Your local filename then lags until the next
`craft pull` — bare `craft status` will not flag it; `status --remote`
will. Prefer `craft mv` so filename and name move together.

## JSON files

Content must satisfy the type's schema in `.craft/file-types/<type>.json`
plus the injected base fields (`name` required; `description` and
`image` optional). Push and platform-import validation (same pipeline) is
strict and field-level:

```
validation_failed: Content failed schema validation for file type "quest"
  reward: must be number
```

Fix the listed fields and push again — nothing was written. Scalars are
coerced where unambiguous ("5" → 5 for a number field), so quoting a number
is survivable; wrong shapes are not.

Rules:

- Never put ids (`fileId`, `referenceId`, `versionId`) in content
  fields — identity lives in the file's `$craft` block and the sync
  ledger, not in your data. (The one exception: hydrated reference objects
  contain a `referenceId`; see the references-and-computed-fields
  instruction.)
- Image fields hold ImageObjects (`{ "url": …, "focalPoint": … }`). For
  "no image", OMIT the field or set `{}` (empty, ready for upload) —
  `null` fails validation (`image: must be object`). Never fabricate a
  URL, and keep existing image objects intact when editing other fields.
- Never write expression (computed) field values; they are stripped on push.
- Enum fields must use one of the listed values exactly.
- Character-designated JSON types have an optional built-in `voice` field.
  Run `craft voices` (or `craft voices --json`) and store one returned
  `voiceId`; omit the field to use the Game Master voice.

## Markdown files

YAML frontmatter + body:

```markdown
---
name: Fall of Gondor
description: How the white city fell.
---

# Fall of Gondor

The body is plain markdown…
```

- `name` is required in spirit but forgiving in practice: when it is
  missing entirely, the server fills it in from the filename stem (the same
  fallback the app's write tools use). Rely on that for scratch files only —
  an explicit `name` is what renames survive on. `description` and
  `image` (the full image object) are optional.
- Updates are field-scoped merges server-side: keys the app stores on the
  file beyond these (voice settings, GM-instruction triggers, …) are
  preserved even though the workspace never shows them. Do not try to manage
  them from here.
- Mentions of other files are stored as `<@referenceId>` tokens in the
  body. You may write `<@file-slug.file-type>` (e.g.
  `<@aragorn.character>`) and the push normalizes it; for files that are
  new in this same push, use the pending referenceId from
  `.craft/index.json` directly. (Platform import works the same way —
  slug mentions resolve against files created in the same import batch.) The referenceId form is also the safe
  choice for a file being RENAMED in the same push — a slug token
  resolves against post-push names, so the old name's slug will fail.
  Mentions inside backtick code spans or code fences are NOT resolved —
  they stay inert text, silently. Keep mentions in plain prose.
- Unlike JSON reference fields (where a not-yet-existing slug is stored as
  an incomplete draft), a mention that cannot resolve REJECTS the push with
  `validation_failed`. The mention target must exist after the push —
  already pushed, or created anywhere in the same push (order within the
  batch does not matter).
- **Only `name`, `description`, and `image` frontmatter keys are
  stored.** Any other key you invent survives only in your local file — the
  server drops it, and because hashing covers just the stored record, no
  status/diff will ever flag the divergence. `craft push` warns when a
  pushed file carries such keys. Put structured data in a JSON type instead.

## Creating and deleting

- **Create**: write the file with a valid name/extension. `craft status`
  mints its identity (a pending `referenceId`); `craft push` creates it
  remotely. New empty folders are fine. Via platform import, no id is
  needed — the import mints a stable one from the path. Either way, when
  you copy an existing file to start a new one, DELETE the copy's
  `$craft` block: two files claiming the same id are both rejected.
- **Delete** (CLI only): delete the file and push — the app moves it to
  trash (recoverable, never a hard delete). Deleting a directory deletes
  the folder remotely once it is empty. Platform imports never delete:
  without the CLI, tell the user which files to remove in the app.

## Package-managed files are read-only

Files with `"source": "package"` in `.craft/index.json` belong to an
installed package. They are in the working tree so you can READ them for
context, but pushes that touch them are rejected (`package_managed`).
Manage them through the package flow in the Craft app.

## Expect server normalization

After a push, some working files may be rewritten and reported as
`normalized by server`: reference slugs hydrate into
`{slug, referenceId, fileTypeSlug}` objects, mention tokens normalize, and
the file on disk is updated to match what the server stored. This is routine
bookkeeping, not data loss — never revert it, and never hand-edit anything
under `.craft/`.
