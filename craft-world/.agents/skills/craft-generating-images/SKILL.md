---
name: craft-generating-images
description: "Read this before generating an image for a workspace file (cover art, portrait, scene illustration) with `craft image generate`, or uploading a local image file with `craft image upload` — covers flags, the account opt-ins and daily caps, reference images, and how to apply the returned image object to a file."
---

# Generating Images

The `craft` CLI can generate an image for the current workspace's project:

```
craft image generate --prompt "A weathered ranger in a green cloak, storm-grey eyes, misty forest, oil-painting style" --json
```

Generation runs remotely: the server calls the image model, uploads the
result to Craft's media storage, and returns a finished image object with a
hosted URL. Nothing binary lands in the workspace and no media push is
needed — you only paste the returned object into a file and push as usual.

This is a CLI workflow (the user can also generate images in the app).
Working without the CLI: finish all non-visual work, NEVER fabricate image
URLs or invent image objects, and end by telling the user which records
still need art so they can generate it in the app.

## Before you generate

- **It spends the USER'S energy** (the same balance the app uses). Never
  fire generations the user didn't ask for, and prefer one good prompt over
  retry loops. If energy runs out, the user can say to use purchased
  credits instead — pass `--credits` ONLY when the user has explicitly
  approved spending credits. Credits also require their own account
  setting ("Allow credit spending" under the same Settings -> Account
  card); if it is off the server refuses `--credits` with instructions.
- **It is off by default.** If the command fails with "Image generation via
  the Craft CLI is disabled", tell the user to enable "Allow image
  generation" under "Image generation for the Craft CLI" in Craft under
  Settings -> Account. You cannot enable it yourself.
- **There is a daily cap** (50 generations per rolling 24 hours across all
  projects). A `rate_limited` error means the window is full — it frees up
  as older generations age out, not at a fixed reset time. Stop generating
  and say so.
- **Energy can simply run out** (`insufficient_balance`): the account's
  daily energy is spent and refills on the daily reset. When it happens
  mid-batch: STOP generating, keep building everything non-visual, and
  end your work by telling the user exactly which records still need art
  (so one later batch finishes the set). Never leave the gap unmentioned,
  and never switch to `--credits` without explicit approval.

## Style comes from settings, not from your prompt

The server automatically composes stored image style instructions into
every generation: with `--for <path>` the named file's TYPE style
applies, otherwise the PROJECT's `settings.imageStyleInstructions`
(`.craft/project.json`). The response's `styleSource` says which was
used (`"file_type"`, `"project"`, or null = none stored).

So before generating a batch: if the project has no stored style and you
are establishing one, WRITE IT INTO `.craft/project.json` and push —
see the configuring-your-project topic. Then keep every prompt SUBJECT
ONLY (who/what, composition, moment). Repeating style text in prompts on
top of stored style degrades results; `--no-style` exists for the rare
one-off that must ignore the stored style.

## Flags

- `--prompt "..."` (required) — the SUBJECT: who/what, composition, the
  moment. Under ~100 words. Use the file's content (description,
  appearance) as source material. Style belongs in settings (above).
- `--for <path>` — workspace path of the file this image is for. Applies
  that file type's `imageStyleInstructions` (falling back to the
  project's). The file does not need to be pushed yet — only its type
  matters. Use this on every per-record generation.
- `--ref <path-or-image>` (repeatable, max 3) — either a local image file,
  which the CLI uploads to the project's media first, or a workspace path
  whose CURRENT stored image becomes a source image (image-to-image), e.g.
  `--ref concept.png` or `--ref "Heroes/Aragorn.character.json"`. A
  referenced workspace file must already have an `image.url`; the error
  names any path that doesn't.
- `--no-style` — send the prompt verbatim, skipping stored style.
- `--model <id>` / `--size <id>` — optional overrides; the project's
  `defaultImageModel` setting is used when `--model` is omitted (an
  edit-capable model is chosen automatically when `--ref` is used).
- `--credits` — spend purchased credits instead of daily energy (same
  amount). Requires the user's explicit go-ahead.
- `--json` — machine-readable output (recommended for agents).

## Applying the result

The response's `image` value is the COMPLETE object a file's `image`
field stores — paste it verbatim, do not invent or restructure fields.

For a JSON file, set the top-level `image` field:

```json
{
  "name": "Aragorn",
  "image": {
    "url": "https://…",
    "generation": { "status": "success", "startedAt": "…", "finishedAt": "…", "prompt": "…" }
  }
}
```

For a markdown file, set `image` in the YAML frontmatter (the full object,
not just the URL):

```markdown
---
name: Fall of Gondor
image:
  url: https://…
  generation:
    status: success
    prompt: …
---
```

Then `craft push`. If the file already had an image object, replace the
`url` and `generation` keys but keep other keys (`focalPoint`,
`variations`, …) intact — unless the user wants them reset.

## Uploading a local image instead

`craft image upload <localFile>` sends an image file from disk (user-made
maps, commissioned art) to the project's media storage and returns the same
pasteable `image` object — apply it exactly as above. It costs no energy
but has its own rules:

- The server re-encodes every upload to webp — a file that does not decode
  as a real image is rejected, and only the pixels survive (metadata and
  anything else is stripped). Max 10MB; 50 uploads per day (1,000 on a
  paid subscription).
- Upload files the USER provided or asked for. Never mirror random web
  images into a project.

## When to use this

- The user asked you to build, expand, or illustrate world content: images
  for the KEY records are part of a finished world (see the
  building-a-world topic for prioritization). State how many generations
  you intend before running a batch, then one good prompt per record.
- The user pointed at a specific file and asked for art.

## When NOT to use this

- The file already has an image the user is happy with — regenerating
  overwrites it (the old URL is lost from the file).
- The task is editing/syncing/refactoring with no art component. Bulk-
  illustrating a project nobody asked you to illustrate burns real energy.
- To quiet a `craft project-completeness` warning. The report measures
  against a fully visual project; when the user wants a text-only or
  image-free project, accept the warning and say so — never spend energy
  to satisfy a checker.
