---
name: craft-writing-handbooks
description: "Read this before writing, rewriting, or adding a chapter to the Player's Handbook (settings.playerHandbook) or Builder's Manual (settings.builderManual) in .craft/project.json. Covers which material belongs in which book and which belongs in neither, chapter structure, file mentions, images, and voice."
---

# Writing Handbooks

A project's two books are settings values in `.craft/project.json`:
`settings.playerHandbook` and `settings.builderManual`. Edit the file in
place, then plain `craft push`.

Editing a book inside one JSON value is awkward, so be surgical: read the
file, rewrite ONLY the chapter object you are changing, and leave the rest
of the `chapters` array byte-identical. `project.json` is pushed whole —
regenerating it from a documented key list silently deletes both books. See
the configuring-your-project topic for the rest of the settings file's rules.

Chapter `content` is markdown carrying real newlines, so it has to be
JSON-escaped in the file. Write it with a tool that escapes for you rather
than hand-typing `\n` runs, and re-read the file afterwards to confirm the
JSON still parses.

## Two books, two readers

A project can carry two books. They are not long and short versions of each
other — they answer different questions for different people.

- **Player's Handbook** — how to PLAY within the system. How the rules work
  at the table, how to make a character, what a session feels like, how to
  have fun in this world. The reader is a player sizing the system up or
  learning it. Write TO them: "you", present tense, second person.
- **Builder's Manual** — how to BUILD on the system. How the file types are
  put together and how to extend them, what makes a new entry fit, what the
  design is actually trying to do, how to adapt the whole thing to another
  setting. The reader is a creator who cloned your project and is about to
  add to it. Write to a peer: state intent and constraints, not flavor.

The split test: "how do I use this at the table?" is player. "how do I add to
this without breaking it?" is builder. A rule the player needs AND the
builder extends belongs in the Player's Handbook, and the Builder's Manual
says how to extend it — don't paste the rule twice.

## What belongs in neither

- **Showcase copy.** The pitch, the tagline, the one-paragraph hook a
  stranger reads before clicking — those are the project page's
  `headline`, `headlineLabel`, `body` and `authorsNotes` settings.
  A handbook that opens by selling itself wastes its first chapter.
- **GM-only material.** Secrets, the twist, how the GM should adjudicate or
  escalate, the rulings the players are not supposed to read — those go in
  `gm_instructions` files, which the GM agent loads during play. A handbook
  is public: assume every player reads every chapter.
- **Raw schema dumps.** The file types are already readable. The Builder's
  Manual explains the INTENT behind a schema and the conventions a good entry
  follows; it does not restate the field list.

## The stored document

Both books share one shape — an ordered array of chapters. Whatever surface
you write a chapter on, this is what the chapters add up to:

```json
{
  "version": 1,
  "chapters": [
    { "id": "how-to-play",
      "title": "How to Play",
      "content": "Every roll in Ashfall is 2d6 plus one stat…" },
    { "id": "making-a-character",
      "title": "Making a Character",
      "content": "Pick a calling. <@0198f4b2-6c31-7a5e-9d40-1f2b3c4d5e6f> is the one most tables start with…" },
    { "id": "the-turn",
      "title": "The Turn",
      "content": "A scene runs until someone spends a token…" }
  ]
}
```

- Chapter order in the array IS reading order.
- `id` — stable, kebab-case, ≤64 characters. It addresses the chapter; keep
  it when you rewrite the body.
- `title` — ≤120 characters, shown in the chapter rail.
- `content` — markdown, ≤100,000 characters. Max 50 chapters per book.
- `updatedAt` is stamped server-side. Never write it yourself.

## Chapter conventions

- One job per chapter, titled as the reader would ask for it ("Making a
  Character", not "Character Creation Rules v2"). 5–12 chapters is a book;
  30 one-page chapters is a wiki nobody reads.
- Open with the chapter that answers "what is this and what do I do first" —
  the reader lands on chapter 1 with no other context.
- Inside a chapter: `##` for sections, `###` sparingly. The chapter title
  is already the `#`; don't repeat it in the body.
- Rules that are looked up (moves, tables, costs) want tables and lists.
  Rules that are absorbed (how a scene flows, what the world believes) want
  prose. Most chapters need both.
- 400–1,500 words per chapter. Long enough to be the real answer, short
  enough to read before a session.

## Linking compendium entries

Point at real files instead of naming them in plain text. A stored mention is
the file's `referenceId` wrapped in angle brackets:

```markdown
The exiles hold the north bank; <@0198f4b2-6c31-7a5e-9d40-1f2b3c4d5e6f>
speaks for them, and has since the vote.
```

It renders as a clickable `@Name` chip that opens the file. Rules:

- Use the file's `referenceId` (a UUID) — not its name, slug, or database
  id. **Never invent one**: an id that resolves to nothing renders as
  "@Missing file" in the middle of your sentence.
- The mention renders as the file's CURRENT name, so write the sentence so it
  still reads if the file is renamed.
- Mention a file the first time a chapter leans on it, then use plain prose.
  A paragraph of chips reads like a link farm.

## Images

Standard markdown: `![alt](url)`. Two layouts, chosen by how you write the
paragraph:

- A paragraph containing ONE image and nothing else becomes a full-measure
  figure with book margins.
- Several images in the SAME paragraph become a side-by-side row at equal
  height — the image row. Put them on one line, separated by spaces:

```markdown
![A courier's mask](https://media.craft.example/projects/abc/mask.webp) ![The ferry at dusk](https://media.craft.example/projects/abc/ferry.webp)
```

Mixing an image into a text paragraph inlines it small instead — that is
almost never what you want. As with everything else visual: the URL must be
one Craft already hosts, from a generation or upload result or a file's
stored `image.url`. Never write a URL you have not seen in data.

## Voice

Write like the world's own in-print rulebook, not like documentation about a
product. Concrete over generic — name a faction, quote a ruling, show the
roll. No meta-commentary about the project, the app, or the AI that helped
write it. If a sentence would survive being printed in a book someone paid
for, it belongs.

## One chapter, as written

The `content` value of a chapter titled "Making a Character", before
escaping (the title is its own key — it is not repeated as an `#` heading):

```markdown
You will need three things before the first scene: a calling, a debt, and a
name you would answer to on a bad night.

## Pick a calling

Your calling is what people call you when they need something. Most tables
start with <@0198f4b2-6c31-7a5e-9d40-1f2b3c4d5e6f> — it forgives a bad roll
and it always has somewhere to be.

| Calling | Opens with | Struggles with |
| --- | --- | --- |
| Courier | Doors, roads, rumors | Standing still |
| Assessor | Ledgers, lies | Being liked |

## Take a debt

Nobody in Ashfall starts even…
```

Reference ids come from `.craft/index.json`. Never invent one.
