# Active Handoff — PC Port

_Last updated: 2026-06-30. Canonical roadmap lives in the `pc-port-phases` memory; this is the short cross-session frontier note._

## Where we are
- **Phase A** (boot → interactive KernelMenu): DONE.
- **Menu.2** (disc/archive overlay loading): DONE + verified. Disc image mounts, `ArchiveInit` loads the index, synchronous archive reads work; selecting Field loads + LZSS-decompresses the field overlay from disc.
- **Phase C (Field)**: STARTED — `FieldMain` decompiled and entering.

## What was just done (FieldMain gateway)
- `src/field/main/main.c`: `FieldMain` decompiled (port-first functional C, control flow 1:1 with asm, all calls preserved, logging under `#ifdef XENO_PC_PORT`). It now **enters and runs its full setup**.
- `pc_port/src/port_main.c`:
  - `*(int*)D_80010000 = -1` — restores the real static rodata value so `g_FieldSystemMode = SYSTEM_MODE_CD_ROM(1)` and the mode-0 `break 1` trap is skipped (the original control flow decides the mode; not a forced value).
  - `ArchiveInit(...,0)` kept (NOT D_80010000's -1): with -1 ArchiveInit skips the CD table/header read and expects an EXE-baked table the port doesn't migrate.

Reproduce: `cd pc_port/build_native && XENO_KERNEL_SEL=0 ./xeno-port`

## DONE: stub sizing + 64-bit pointer widening (commits 3c072b8, + this batch)
- `gen_port_stubs.py` sizes data stubs from `symbol_addrs` `size:` annotations, then
  **doubles** them: PSX sizes assume 4-byte pointers but the 64-bit port widens them
  to 8, so `void*[3]` (12 B on PSX) needs 24, and pointer-heavy structs up to 2x.
  Fixed `g_GameState`→0x2300 (was 16; `partyMembers`@0x1D34 was OOB) and the party
  pointer-arrays `g_PartyDataBuffers`/`g_PartyStreamDataPointers`/`g_PartyStreamDataQueue`.
- Field debug-entry harness (`XENO_FIELD_TEST`, port_main.c): sets `ArchiveSetIndex(4,0)`
  — the party-skin archive directory the new-game flow establishes — so the debug
  field entry's `GamePartyCharactersInitializeSkins` resolves sane sizes.

With those, **FieldMain now runs its entire setup and enters the main loop**
(`[FieldMain] entering main loop`), executing loop-body stubs.

## The frontier (start here next): field initial map load — func_80078D44 → FieldLoad
`FieldMain main loop → (main.c:395) FIELD_ACTOR_FLAGS(g_PlayerActorIndex) → SIGSEGV`
because **`g_FieldActors` is NULL** — the map was never loaded. Traced the exact path:

- **`func_80078D44`** (365 lines, `asm/field/.../misc4/func_80078D44.s`) is the field-init
  orchestrator. FieldMain calls it UNCONDITIONALLY in setup (main.c:223-ish). It calls
  **`FieldLoad`** (at 0x80078E80) plus the render/texture bring-up:
  `FieldInitializeRenderContexts`, `FieldDisplay`, `FieldImageConvert24BitTo15Bit`,
  `MoveImage`, `FieldLoadTIM`, `func_80070488`, `func_800A24C4`, etc.
- **`FieldLoad`** (904 lines, `asm/field/.../misc3/FieldLoad.s`) is the map parser: it
  HeapAllocs + `FieldLZSSDecompress`es the map's sections and **sets g_FieldActors**
  (misc3.s:1451). No archive streaming needed (good).
- The in-field scene-transition state machine `func_800A5924` (driven by `D_800ADB38`)
  is a DIFFERENT path (early-exits when D_800ADB38==0); not the initial load.

This is a large COHERENT slice (~1300 lines core + texture/render callees) that must be
ported as a unit before anything observable changes — single functions won't validate
(porting func_80078D44 with FieldLoad still stubbed leaves g_FieldActors NULL → same
crash). It's the heart of the field: map parse → actors/walkmesh + TIM textures (VRAM
via PsyCross) + render-context setup.

### Next steps
1. Port `func_80078D44` + `FieldLoad` together (+ the map-parse/texture/render callees
   they pull in). Expect to bring up TIM/texture loading to VRAM and the field render
   context. Multi-session; budget for it.
2. Run `XENO_KERNEL_SEL=0 XENO_FIELD_TEST=1` and oracle-iterate from there.
   (XENO_FIELD_TEST sets the party-skin archive dir; still a stand-in for new-game init.)

## Reproduce
`cd pc_port/build_native && XENO_KERNEL_SEL=0 XENO_FIELD_TEST=1 ./xeno-port`

## Rules in effect
One reversible change at a time; stop at each frontier; no fake field load; no permanent hardcoded mode without proof.
