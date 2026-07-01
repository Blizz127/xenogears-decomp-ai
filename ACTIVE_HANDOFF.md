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

## DONE: func_80078D44 ported + misc2.c unblocked (commit c54bd18)
- `func_80078D44` (misc4.c) decompiled — the field-init orchestrator now runs: loads
  the map file, sets up render contexts, `g_FieldCurRenderContextIndex=1`, calls
  FieldLoad, runs fade loops.
- Guarded `asm("break 0x400")` (MIPS trap, PC-HDD-dev-only) in misc2.c + shop_menu
  behind `#ifndef XENO_PC_PORT` — the host assembler can't emit it. This unblocked
  **misc2.c's whole TU**, so `FieldClearAndSwapOTag` is real C and sets
  `g_FieldCurRenderContext`.
- Field now runs the init through render-context setup (`ClearImage`/`PutDrawEnv`/
  `PutDispEnv` work) and reaches `FieldDisplay`.

## DONE: field render OT — FieldDisplay → DrawOTag (OT_TAG stride fix)
The early-render crash `FieldDisplay (misc5.c:139) → DrawOTag(ot3+7) →
ParsePrimitivesLinkedList SIGSEGV` was an **OT slot stride mismatch** between the
game and PsyCross, NOT a bad prim from FieldDisplay.
- PsyCross's `ClearOTag`/`ClearOTagR` (LIBGPU.C) build the OT linked list by casting
  the array to `OT_TAG*` and striding by `sizeof(OT_TAG)`. `OT_TAG` is `{addr:24,
  len:8}` = **4 bytes**. On PSX `u_long` is also 4 bytes so they coincide.
- In the 64-bit port `u_long` is **8 bytes**, so the game's OT arrays
  (`RenderContext.ot3[8]`, `ot1`/`ot2[0x1000]`, menu `ot[0x10]`) are 8-byte-strided
  and the game indexes/draws them that way (`DrawOTag(ot3 + 7)` = byte 56).
  `ClearOTagR` wrote a 4-byte-strided list (only bytes 0–31), leaving slots 4–7 (the
  upper half) **zeroed**. Confirmed by dumping ot3: slots 0–7 linked at +0/+4/+8…/+28,
  bytes 32–63 all `0x00000000`. `DrawOTag(ot3+7)` read slot 7 at byte 56 → `addr=0`
  (not the `0xffffff` terminator) → walked to null → SIGSEGV.
- **Fix (build_port.sh, idempotent grep-guarded sed):** pad PsyCross's `OT_TAG` to the
  host `u_long` size (8 B) so `ClearOTag(R)`'s stride matches the game's `u_long[]` OTs.
  General fix — also unblocks the in-game menu's `ot[0x10]` and battle OTs later.
- The KernelMenu was unaffected because it uses a **single-element** OT (`TermPrim` +
  `AddPrim`), never the array stride. Verified: field passes two full
  `FieldDisplay`/`DrawOTag` iterations; KernelMenu still reaches the sustained loop.

## DONE: field heap corruption — FieldImageConvert24BitTo15Bit u_long overflow
The `HeapConsolidate` SIGSEGV (during `func_800A5774`'s HeapAlloc, walking a free
list with a garbage `pNext`) was traced with a gdb hardware watchpoint to its
origin: `FieldImageConvert24BitTo15Bit` (misc5.c) overflowing its `pImage15Bit`
buffer into adjacent heap block headers.
- The convert loop runs `0x1C00` (7168) times doing `*g_Field15BitImageData = ...;
  g_Field15BitImageData += 1`. `g_Field15BitImageData` is `u_long*`; on PSX
  `u_long`=4 B so 7168×4 = `0x7000` exactly fills the `HeapAlloc(0x7000)` buffer.
  In the 64-bit port `u_long`=8 B → 7168×8 = `0xE000`, writing `0x7000` bytes PAST
  the buffer into the next blocks' headers (corrupting a `pNext` to e.g. `0x2a74`,
  which HeapConsolidate later followed off the heap). The 24-bit *read* side
  (`g_Field24BitImageData`) was broken the same way.
- **Fix (matching-safe):** typed these image buffers as `u32` instead of `u_long`
  (misc5.c) so the deref/stride stays 4 bytes. `u32`≡`u_long`≡4 B on the MIPS
  target → byte-identical codegen, matching preserved; correct in the port.
- Same class as the OT fix — a `u_long` 8-byte widening port-ism. Watch for more.
- Verified: field now clears the convert pass, runs the rest of `func_80078D44`,
  and **enters the FieldMain main loop** (`[FieldMain] entering main loop`).
  KernelMenu still reaches the sustained loop (exit 124).

## DONE: FieldLoad prerequisites — map load + archive overflow + overlay
Before FieldLoad can parse anything, its inputs must be real. Cleared a chain of
port-init gaps (all committed together):
- **Map file now loads.** `func_800777DC → func_8001B484` early-returned "already
  loaded" because the map-cache sentinels `D_8004F334`/`D_8004F330` were zeroed
  stubs (== fileIndex 0) — the real field-entry init `func_8001ACA4` sets them to
  -1 but the KernelMenu debug path skips it. Set both to -1 in the XENO_FIELD_TEST
  harness (same class as `D_80010000=-1`). Verified: `D_8005A4E0` now holds a real
  ~106KB map file with a structured header.
- **Archive read no longer overflows the heap.** The sync `ArchiveReadFile`
  (archive_port.c) `CdRead`s whole 2048-byte sectors, but buffers are sized to
  `ArchiveDecodeAlignedSize` (4-aligned, NOT sector-rounded), so a partial last
  sector scribbled up to 2047 bytes past the buffer into the next heap block's
  header (→ HeapConsolidate crash). Now bounces the tail through a scratch buffer
  and copies only the file's bytes.
- **Overlay symbols populated / overlay no longer crashes.**
  `g_GameStateOverlayArchiveOffsets` (the state→archive-index table, real ROM data
  {0,0xE,0x10,0xF,0xD,0x11,0x12}) and `g_MainGameStateOverlayBuffer` (→ 0x8006FAF0)
  were zeroed stubs, so per-state overlay decompress ran on garbage. Defined both
  (game_overrides.c). The overlay archive read still returns size 0 (see frontier
  below), so the port's `LZSSDecompress` now treats an implausible size (> emulated
  RAM) as a no-op — the overlay is redundant in the port (field code statically
  linked) so skipping it is safe. Field now runs its full setup with the map loaded.

## The frontier (start here next): decompile FieldLoad (g_FieldActors NULL)
`FieldMain main.c:395 → FIELD_ACTOR_FLAGS(g_PlayerActorIndex) → SIGSEGV` because
**`g_FieldActors` is NULL** — `FieldLoad` (904-line map parser at 0x80078E80,
misc3) is still a `[stub]`. Its input `D_8005A4E0` (the map file) NOW loads with
real data, so it can finally be ported. It sets `g_FieldActors`/`g_FieldNumActors`
in its own body (asm 0x80071354) then runs a per-actor init loop (0x5C-byte
actors). ~30 callees, ~50 data symbols (see the scoped slice below). Audit new
field code for more `u_long` byte-buffers (each is a latent 2× overflow — two
found+fixed this session in misc5.c).

### Separate frontier (later): overlay archive resolution
The per-state overlay never actually loads: `ArchiveDecodeSize(0xE)` returns 0 in
the overlay's archive context (`LoadGameStateOverlay` does `ArchiveSetIndex(0,1)`
then reads `g_GameStateOverlayArchiveOffsets[state]`). The overlay-directory /
archive-offset resolution is incomplete. Redundant for field CODE (statically
linked) but the field's initialised DATA (overlay `.data`, currently host-address
zeroed stubs) may need it eventually — a data-migration concern, not a FieldLoad
blocker.

## (later) field initial map load — func_80078D44 → FieldLoad
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
