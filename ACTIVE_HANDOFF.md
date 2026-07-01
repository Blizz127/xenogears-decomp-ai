# Active Handoff — PC Port

_Last updated: 2026-07-01. Commit e5982f8._

## Where we are
- **Phase A** (boot → interactive KernelMenu): DONE.
- **Menu.2** (disc/archive overlay loading): DONE.
- **Phase C (Field)** — render loop stable, crash-free. Black screen (CLUT data zero).

## Recent commits (this session)
```
e5982f8  field: migrate D_800ADC44 texture descriptor data (clean)
9d20b49  Revert "field: migrate D_800ADC44 texture descriptor ROM data"
e0222d8  field: implement FieldLoadUITextures — archive texture CLUT loader
f35238c  field: implement func_80074108 — background/camera draw dispatch
8f2b712  field: implement func_80080968 — actor state table lookup
18a3e97  field: implement func_80080A74 — per-actor ActorData field init
d0675c2  field: implement func_80080F44 — per-actor data initialization
29727c6  field: fix func_8007554C DrawOTag crash
```

## Actor init chain: COMPLETE
func_80080F44 → func_80080A74 → func_80080968 all implemented.

## Draw dispatch: COMPLETE
func_80074108 (background/camera) implemented. func_8007554C (per-frame render) implemented.

## Texture loading: COMPLETE (code + data)
FieldLoadUITextures implemented. RECT populated (x=0,y=0xFB,w=0x10,h=1).
LoadImage path in func_80074108 now **unblocked** (h=1, non-zero).

**D_800ADC44 data migration: DONE (clean).** Commit 1cf8a56 was BLOCKED by Kimi
because it replaced the `if (D_8004F344 == 0)` guard with an in-function memcpy,
breaking asm-faithful control flow. Reverted 1cf8a56 and reapplied the data
correctly as a dedicated PC port data file (`pc_port/src/data_field.c`), compiled
into the port-only sources list. `src/field/main/main.c` is byte-identical to the
Kimi-approved e0222d8. D_800ADC44 is now in `.data` with real values (verified
via objdump: entry 0 = {672, 448, 0, 251, 0, 0}). D_800AFC08 remains in `.bss`
(zeroed) — StoreImage does not read back VRAM in PsyCross yet.

## Next frontier
**StoreImage / GR_ReadVRAM** — PsyCross's StoreImage does not read back from VRAM
into host memory. D_800AFC08 (the CLUT readback buffer) stays zero after
StoreImage. This is the next frontier for a visible field: either implement
VRAM read-back in PsyCross's GR_ReadVRAM path, or find an alternative CLUT
source for func_80074108's rendering.

## Reproduce
```
cd pc_port && XENO_FIELD_TEST=1 XENO_KERNEL_SEL=0 timeout 10 build_native/xeno-port
```
Expected: exit 124 (timeout), black screen, ~53 stubs.
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

## DONE: FieldLoad decompiled — g_FieldActors now allocated
`FieldLoad` (904-line map parser, misc3) is a port-first functional C decompile
(src/field/main/misc3.c:93; replaces the INCLUDE_ASM). It parses the loaded map
`D_8005A4E0` (header sizes at 0x10C–0x12C, LZSS section offsets 0x130–0x154): TIM
textures, CLUTs, model data, walkmesh, scripts (`g_FieldCurScriptFile`), triggers
(`g_pFieldTriggerZones`), sprites (`g_FieldSpriteData`), then **allocates + zeroes
`g_FieldActors` = HeapAlloc(numActors*0x5C) and sets `g_FieldNumActors` =
*(u16*)(map+0x18C)**, then a per-actor (0x5C-byte) init loop. Verified at runtime:
`g_FieldActors`=valid, `g_FieldNumActors`=27, 17 actors processed. The old
main.c:395 (`g_FieldActors` NULL) crash is RESOLVED. Every 32-bit map/section
pointer is typed `u32*` (not `u_long*`) to avoid the widening-overflow class.
- Flagged-uncertain (port-first, watch at runtime): actor position reads
  (misc3.c:372-377 read `*(u16*)` entries — may need s16/32-bit), the model/fixup
  sections, `func_8002709C` 13-arg marshalling, and the unsized scratch stubs
  `D_800AFB14/18/20/…` (may default to 16 B; needed larger). See the subagent's
  section-by-section notes if these misbehave.

## DONE: per-actor model init + model relocation
Ported the model-init pipeline FieldLoad runs per model actor (temp2.c):
`func_8002CB54` (double-buffer alloc), `func_8002C8CC` (command-list build),
`func_8002C644` (buffer pin), `func_800303C8` (skeletal block), and
`func_8002C3E8` (offset→pointer relocation of the model data). Fixed two
integration bugs in the FieldLoad decompile: (1) the model control block `pModel`
uses PSX 4-byte pointer slots (+0x4/+0x8/+0xC) — store/load them as truncated u32,
not 8-byte `void*`, or an 8-byte write clobbers the neighbour; (2) `func_8002C8CC`'s
first arg is `pModel[0x4]` (modelData), not `pModel[0xC]` (the uninitialised out2).
The field now runs alloc → relocate → command-list-build per model actor.

## DONE: FieldLoad completes (model build guarded + actor array sized for host)
Toward a fast visible field (chosen direction), guarded FieldLoad's per-actor model
build under `#ifndef XENO_PC_PORT` (it needs the unported D_8004FE50 primitive
subsystem; running it partially corrupts the heap), and sized the actor-array alloc
by `sizeof(FieldActor)` instead of the hardcoded PSX 0x5C. **FieldLoad now runs to
completion** and the crash moved into FieldMain.

## The real blocker for a visible field: FieldActor struct widening (0x70 vs 0x5C)
`FieldMain main.c:395 → FIELD_ACTOR_FLAGS(g_PlayerActorIndex) → SIGSEGV`. The macro
is `*(s32*)(*(u32*)((u8*)g_FieldActors + idx*0x5C + 0x4C))` — reads the actor's
`pActorData` (PSX offset 0x4C) and derefs it. But on the host **`sizeof(FieldActor)
= 0x70`, not 0x5C**: its 4 pointer fields (`pModelData`/`pSpriteData`/`pShadow`/
`pActorData`, actor.h) widen 4→8 bytes, which (a) makes the array stride wrong and
(b) shifts every field past the first pointer, so PSX raw offsets like `0x4C`/`0x5C`
that the field code + macros use everywhere land on the wrong bytes (0x4C hits a
matrix element → garbage pointer → crash). The alloc-by-sizeof above stops the array
overrun, but the offset mismatch remains.
**Fix (the right one, unblocks the visible field AND correct actors): make FieldActor
PSX-faithful — change its 4 pointer fields to `u32` slots (== pointer size on the
MIPS target, so matching-safe; sizeof→0x5C) and reconstruct pointers at use sites.**
Scope: ~145 use sites across ~15 field files (camera/text_box/particles/misc*/main),
so it's a sizeable refactor (consider delegating). This same class recurs wherever a
game struct with pointer fields is accessed by raw PSX offset.

## Other frontier: model primitive-processor subsystem (D_8004FE50)
`func_8002C8CC → per-primitive callback (temp2.c:222) → calls a NULL fn ptr`.
The callback is `*(u32*)(D_8004FE50 + prim*0x28 + 0x18)`. **`D_8004FE50`** is the
primitive-descriptor table (0x28-byte entries: 6 fn ptrs at +0x0..+0x14, the
per-primitive callback at +0x18, advance counts at +0x1C/+0x20/+0x24) — real ROM
data (`.sdata` @0x8004FE50) that the port zeroes, so every callback is NULL. But
populating it isn't enough: the pointers target `0x8002Exxx` **model-primitive
processor** functions (`func_8002E038/ED20/E470/…`, ~dozens in the temp2 TU) that
are nearly all still `[stub]`s — a stubbed callback returning 0 makes the parse
loop misbehave. This is the deep core of model rendering: (a) define `D_8004FE50`
from ROM (game_overrides.c, same as the other zeroed data tables), (b) port the
`0x8002Exxx` primitive processors it dispatches to. Large multi-session slice.
- Alternative to see a VISIBLE field sooner: temporarily guard FieldLoad's
  per-actor model-init block so FieldLoad completes without models, and drive on
  to FieldMain's render loop to try to show the map background (no actor models).
  Stopgap, not faithful — but a fast way to a first field screenshot.

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
