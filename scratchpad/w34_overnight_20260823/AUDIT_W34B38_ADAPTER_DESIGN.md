# W34B38 design — world-map-only OT adapter (retail 4-byte OT over guest links)

Date: 2026-08-23. Branch `integrate/w34b24-i1`, tip e82d8a27 + uncommitted
W34B37 writer fix. Ground truth re-established this session
(`b38_ground_truth.log` in the session scratchpad): natural run at frame 916
crashes in `ParsePrimitivesLinkedList` (PsyX_GPU.cpp:906 region, `getlen`
byte-3 read) with `p = g_PsxRam+0xa3224` = guest `0x800A2228 + 0xFFC`.

## Part 1 ground truth

- BE3C+0x70 naturally holds a GENUINE GUEST OT root (0x800A2228) with the
  W34B37 writer fix (`wm_8007369C` publishing `host_ptr_to_psx_u32`) in the
  tree. Case: writer-side fix was correct and stays; only the
  representation side crashed. The uncommitted frame-driver
  `ClearOTagR((u32*)PSX_ADDR(ot),…)` hunks are superseded by this design
  and will not be committed in that form.

## Re-derived retail OT contract

- OT: `HeapAlloc(0x1000,0)`; 0x400 entries x 4 bytes. Tail submits
  `root + 0xFFC` (entry 0x3FF). Reverse table: retail `ClearOTagR`
  (0x80044AD8, asm/slus_006.64/nonmatchings/psyq/libgpu/ClearOTagR.s)
  dispatches the driver clear via `D_800568C8` (entries 1..n-1: word =
  guest address of entry-1, low 24 bits, len byte 0) and then stores
  `ot[0] = D_8005698C & 0xFFFFFF` — a link to the retail terminator
  sentinel node at 0x8005698C = `{0x04FFFFFF,0,0,0,0}` (3F290.sdata.s:12074),
  i.e. a 4-word null payload whose own link 0xFFFFFF is the DMA end marker.
- Tag word layout (from the world writers themselves,
  world_map_callback_925a0.c `WM_925A0_TAG_LENGTH_MASK 0xFF000000` /
  `TAG_POINTER_MASK 0x00FFFFFF`, world_map_helper_73b04.c same): 32-bit word,
  link = low 24 bits (guest address), len = high 8 bits (payload words).
- addPrim-equivalent (925A0 lines 198-216): prim.tag =
  (prim.len<<24)|(bucket&0xFFFFFF); bucket = (bucket&0xFF000000)|(prim&0xFFFFFF).
- Packet payload: len words following the tag word; prim code at byte 7
  (word1 top byte). Terminator: link == 0xFFFFFF.

## Decisive host-ABI fact

`USE_EXTENDED_PRIM_POINTERS=0` selects libgpu.h's non-extended branch
(libgpu.h:354-365): `P_TAG = { unsigned addr:24; unsigned len:8; pad0..2, code }`
and prim structs use `u_int tag` — BYTE-FOR-BYTE the retail packet layout,
with `P_LEN=1`. `ParsePrimitive(P_TAG*)` therefore consumes retail-format
packet memory directly when handed a host pointer into g_PsxRam. The ONLY
mismatches are (a) `OT_TAG` carries `_xeno_ot_pad` so ClearOTag(R) strides
8 bytes and clears 0x2000 bytes, and (b) `nextPrim`/`isendprim` treat the
24-bit link as a host pointer.

## Host submission boundary (found — no stop condition)

`DrawPrim(void*)` (LIBGPU.C:462, declared C in libgpu.h:853) →
`ParsePrimitivesLinkedList(p, 1)` parses EXACTLY ONE primitive with no link
walking and no OT_TAG assumption. This is the narrowest existing entry.
`DrawAllSplits()` (C-visible; already used by psyq_compat.c:27) is called
once after the walk to mirror `DrawOTag`'s flush.

## Adapter (new files world_map_ot_adapter.c/.h; PsyCross untouched)

1. `wm_ot_clear_r_guest(u32 ot_guest, u32 count)` — replaces the world
   frame-driver's ClearOTagR call (both the production and M7-mutant call
   sites in world_map_frame_driver.c; call args unchanged: ot, 0x400).
   Writes retail 4-byte reverse table: `w[i] = (ot_guest+4*(i-1)) & 0xFFFFFF`
   for i=1..count-1, `w[0] = 0x00FFFFFF`.
   PORT SUBSTITUTION (documented): retail's `w[0]` links to the 0x8005698C
   sentinel (null 4-word payload, then DMA end). The port does not populate
   main-exe .sdata inside g_PsxRam, so entry 0 carries the direct end tag;
   semantics at the parser are identical (nothing drawn, walk ends).
2. `wm_ot_draw_otag_guest(u32 entry_guest)` — replaces the tail's
   `DrawOTag(PSX_ADDR(root+0xFFC))` (world_map_frame_tail_71984.c), taking
   the same `root+0xFFC` guest value. Walk:
   - read u32 tag via PSX_ADDR; `len = tag>>24`, `link = tag & 0xFFFFFF`
   - `len>0`: submit the packet once via `DrawPrim(PSX_ADDR(cur))`
   - stop on `link == 0xFFFFFF`
   - guards (log-and-count, abort walk, never deref raw): link >= 0x200000,
     link unaligned (link & 3), len > 32 (parser's own invalid threshold),
     step count > 0x1400 (0x400 buckets + generous prim headroom; cycles).
   - after the walk: `DrawAllSplits()`.
3. Counters/getters: packets submitted, buckets walked, aborts by kind,
   last tag/addr — for the natural capture and the focused test.

## Bounded scope

- Exact for single-primitive packets (all the world writers emit today:
  925A0 quads/DR_TPAGE, 73B04 0x28-stride FT4 packets). A packet whose
  first-prim length disagrees with its tag len is submitted once and
  counted (`multi_prim_packets`) — no crash, no guess. Generalizing to a
  reusable engine-wide guest-OT library is NOT attempted.
- No change to PsyCross OT_TAG/P_TAG/ClearOTagR/DrawOTag/addPrim/termPrim,
  no other DrawOTag callers touched (game_overrides.c host-OT paths keep
  calling PsyCross directly), no D554 backedge work, no tripwire changes.

## Test plan (production-linked, w34b28-style harness)

Fixture: synthetic guest OT + packets built in a test g_PsxRam; test
provides recording stubs for DrawPrim/DrawAllSplits (link-time). Cases:
empty OT; single bucket/single prim; multi-bucket chain order
(0x3FF-entry-first, back-to-front link order); malformed link out-of-range;
misaligned link; oversized len; cycle (step cap). Mutants (>=4, all must be
detected): wrong link mask (0xFFFF), wrong terminator check (0xFFFFFFFF),
wrong walk start/order (+0 instead of +0xFFC semantics / forward stride),
missing bounds check. O0/O2/UBSan-O2 byte-identical stdout.
