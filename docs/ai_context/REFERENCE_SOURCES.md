# External Reference Sources

Reference-only material for Xenogears research. Nothing here is part of the
build; nothing here may be vendored into the source tree.

## yaz0r/Noah

- **Local clone (outside this repo):** `~/Projects/xenogears-reference/Noah`
- **Upstream:** https://github.com/yaz0r/Noah
- **Cloned revision:** `d725fe231e56a895c17df0554042b9e2bd1acb78` (2026-04-22,
  shallow clone; `git -C ~/Projects/xenogears-reference/Noah pull` to update)
- **What it is:** a non-matching C++ reimplementation/decompilation reference
  of Xenogears (SDL/bgfx host app). Layout highlights: `NoahLib/field/`
  (field engine + script VM), `NoahLib/kernel/`, `NoahLib/battle/`,
  `NoahLib/sprite/`, `NoahLib/psx/` (PSX HAL), `NoahLib/debug/` and
  `NoahLib/validation/` (debugger/validation tooling).

### Approved uses

- Behavior comparison (what a subsystem is supposed to do end to end).
- Naming hints for unlabeled functions/data (`func_8XXXXXXX`, `D_8XXXXXXX`).
- Data-format clues (archives, scenes, walkmesh, camera records, spawn
  tables, VRAM layouts).
- Field script VM opcode research (opcode semantics, argument shapes,
  handler tables).
- Rendering / camera / actor logic research.
- Debugger and tooling ideas (their validation/debug UI approaches).

### Rules

- **Not authoritative.** Noah is non-matching and may be wrong, simplified,
  or divergent. Any behavior taken from it must be confirmed against
  SLUS_006.64 asm, retail runtime behavior, gdb traces of our port, or our
  own matched decomp before it lands in this repo.
- **No direct code import/copying** into the decomp or `pc_port/` without a
  deliberate, explicit review (licensing and provenance included).
- Cite it in commit messages/handoff notes as "Noah reference" when it
  informed a decision, alongside the asm/runtime evidence that confirmed it.
