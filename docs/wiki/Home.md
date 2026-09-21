# Xenogears Decomp / PC Port Wiki

This wiki is a **human-readable progress map** for the Xenogears matching decompilation and experimental native PC port (`pc_port/`). It summarizes verified project state without replacing the repo's canonical docs, handoffs, scripts, or evidence logs.

**Source of truth:** [`docs/ai_context/ACTIVE_HANDOFF.md`](https://github.com/Blizz127/xenogears-decomp-ai/blob/main/docs/ai_context/ACTIVE_HANDOFF.md) in the main repo.

## What this project is

- **Matching decompilation** of Xenogears (US release SLUS-006.64) into C, verified against retail assembly.
- **Experimental PC port** (Shipwright-style) that compiles decompiled game logic natively against PsyCross (PSX HAL → SDL2/OpenGL/OpenAL).
- Port progress is gated on functional decompilation coverage; byte-for-byte matching is not required for the port, but retail behavior is the target.

## Wiki pages

| Page | Purpose |
|------|---------|
| [Current Status](Current-Status) | What works, what is proven, what is broken right now |
| [Active Frontier](Active-Frontier) | The current real next investigation target |
| [Phase Log](Phase-Log) | Chronological committed milestones |
| [Build and Run](Build-and-Run) | How to build and run the PC port |
| [Field Rendering and Camera](Field-Rendering-and-Camera) | Camera, framing, and visibility findings |
| [Field Control and Movement](Field-Control-and-Movement) | Input, walkmesh, collision, player control |
| [Field Script VM and Opcodes](Field-Script-VM-and-Opcodes) | Decoded VM behavior and opcode discoveries |
| [Known Addresses and Globals](Known-Addresses-and-Globals) | Important addresses/globals with evidence |
| [Debugging and Tracing](Debugging-and-Tracing) | Practical gdb/logging repro commands |
| [Matching and Porting Rules](Matching-and-Porting-Rules) | How we decide what to implement |
| [Decomp and PC-Port Coexistence Architecture](Port-Coexistence-Architecture) | How matching sources, port replacements, overrides, and stubs coexist |
| [Useful Commands](Useful-Commands) | Copy-paste command reference |

## Status legend

Throughout these pages:

- **Verified working** — committed code + documented runtime proof
- **Proven (read-only)** — gdb/diagnostic proof without a code change, or docs-only checkpoint
- **Committed fix** — landed in git with evidence cited in handoff/commits
- **Temporary hack** — labeled workaround; not retail-final behavior
- **Decoded / trusted** — asm or runtime evidence is strong enough to guide implementation
- **Blocker** — current gate with no committed fix yet
- **Unresolved** — investigated but not fixed; may be cosmetic or lower priority
