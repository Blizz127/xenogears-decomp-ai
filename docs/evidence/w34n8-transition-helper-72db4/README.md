# W34N8 — retail transition helper 0x80072DB4

Verdict: **TRANSCRIBED_AND_CERTIFIED**.

The mandatory loading-transition helper called by the base-world slot-1 setup
callback now exists as compiled production C.  This rung deliberately does not
integrate the larger `0x80072238` owner: it closes the only mandatory absent
body discovered while anchoring that callback, so the eventual owner does not
route through a generated no-op.

## Anchor and retail authority

- Branch: `experiment/worldmap-open-gates-20260823`
- Starting HEAD: `47d85cd49d777ff52b9ef4a7962ed5f734e1e681`
- Local matched origin before the change.
- Retail authority: `disc/world_map.bin`, loaded at `0x8006FAF0`, decoded with
  `mips-linux-gnu-objdump -D -b binary -m mips:3000 -EL
  --adjust-vma=0x8006faf0`.
- Focused listing: `scratchpad/w34n8_pre_slot_helpers.objdump`.

The exact retail boundary is `[0x80072DB4,0x80073300)`.  Slot 1 calls it as
`wm_80072DB4(64, 0, 4, 2)`.

## Implemented behavior

`pc_port/src/world_map_helper_72db4.c` preserves the bounded retail sequence:

1. Allocate 120, 72, and 8 bytes with allocation flag 1.
2. Build three 40-byte textured FT4 strips spanning the 320x239 source image,
   two alternating 36-byte semi-transparent G4 full-screen quads, and one
   DR_TPAGE packet.
3. Select the FT4 texture pages at x=704, 832, and 960, and the draw tpage from
   the caller's ABR argument.
4. Perform the initial DrawSync/Vsync and environment publication.
5. For exactly 64 iterations, alternate draw environments `0x8009BC40` and
   `0x8009BBC8`, clear the corresponding 0x400-word guest OT, link all five
   packets in retail bucket/order, publish one presentation, and advance the
   G4 shade by 4 (0 through 252).
6. Perform the final sync/display publication and free all three allocations.

Heap allocations are native host views into `g_PsxRam`, while retail OT links
are 24-bit guest addresses.  The implementation therefore uses
`PsxMemory_GuestAddr` for packet links and the established
`wm_ot_clear_r_guest` / `wm_ot_draw_otag_guest` adapter for the four-byte retail
OT representation.  It never truncates a native heap pointer into an OT tag.

The only deliberate safety extension is allocation-failure cleanup and a `-1`
return.  The successful path used by retail remains exact.

## Focused certificate

Runner: `pc_port/tests/run_w34n8_72db4.sh`.

The test links the production helper with deterministic guest heap and GPU
seams.  It proves allocation sizes/flags, all four GetTPage calls, exact packet
coordinates/UV/tpage fields, five-packet OT traversal order, guest packet
addresses, draw-environment alternation, shade progression, 64 clears and
draws, 66 syncs/Vsyncs, initial/final environment calls, and identity/order of
all three frees.

```text
O0 PASS
O2 PASS
nonrecovering UBSan PASS
strict warnings PASS
M1 short loop: DETECTED by ASSERT_DRAW_COUNT
M2 wrong intensity step: DETECTED by ASSERT_G4_INTENSITY
M3 wrong G4 bucket: DETECTED by ASSERT_PACKET_ORDER
M4 missing final frees: DETECTED by ASSERT_FREE_COUNT
M5 missing environment alternation: DETECTED by ASSERT_DRAW_ENV_ALTERNATION
M6 missing OT draw: DETECTED by ASSERT_DRAW_COUNT
```

## Full build and remaining boundary

Normal `./pc_port/build_port.sh` completed with:

```text
LINK OK -> pc_port/build_native/xeno-port (port-owned addresses verified)
```

This helper is not called by the current forced route, so no visual or natural
route claim is made here.  The next code-producing rung is the integrated
base-world slot-1 owner `[0x80072238,0x8007299C)`, which must invoke this helper
in retail order over the already transcribed setup stages.  Natural acceptance
still depends on replacing the external gate ladder with that owner.
