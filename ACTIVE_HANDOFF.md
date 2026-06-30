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

## The frontier (start here next): field map-loading pipeline (g_FieldActors)
`FieldMain main loop → (main.c:395) FIELD_ACTOR_FLAGS(g_PlayerActorIndex) → SIGSEGV`
because **`g_FieldActors` is NULL**. It's allocated/set by **`FieldLoad`** (the field
map/scene loader, `asm/field/.../misc3/FieldLoad.s`, writes g_FieldActors @ misc3.s:1451),
which never runs because the loop's map-load drivers are stubs:
`func_80077DAC`, `func_8007554C`, `func_800A5924`, `func_80078BC8`, and the
render-context dispatch.

### Next steps
1. Decompile/port the field map-load path: the loop drivers above + `FieldLoad`
   (allocates g_FieldActors, loads the map/actors/walkmesh). This is the core field
   subsystem; expect it to pull in actor data, TIM/texture, and model loading.
2. Run `XENO_KERNEL_SEL=0 XENO_FIELD_TEST=1` and oracle-iterate from there.

## Reproduce
`cd pc_port/build_native && XENO_KERNEL_SEL=0 XENO_FIELD_TEST=1 ./xeno-port`

## Rules in effect
One reversible change at a time; stop at each frontier; no fake field load; no permanent hardcoded mode without proof.
