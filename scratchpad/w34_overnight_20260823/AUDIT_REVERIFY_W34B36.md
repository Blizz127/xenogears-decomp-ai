# W34B36 fresh audit — frame tail 0x80071984..0x800719D0

Date: 2026-08-23

## Part 0 re-verification

- `sha256sum disc/world_map.bin` remains
  `4c15fd32b3a03d7cd5ea4403dcaabc70abaf99b6aaca65d63d6866803edaac70`.
- Fresh `disasm_world.py` decode reconfirmed `0x80074F2C..0x8007502C`
  as 64 instructions and `0x80075104..0x80075228` as 73 instructions.
- The tail decode reconfirmed:
  `0x80071994` loads `D_8009BE0C`, `0x8007199C` calls
  `SetGeomOffset(0xA0, value)`, `0x800719A8` loads `D_8009BE3C`,
  `0x800719B0` loads environment `+0x70`, `0x800719B4` calls
  `DrawOTag(ot + 0xFFC)`, and `0x800719C8` branches to `0x8007130C`
  when `D_8009D554 != 0`.
- `D_8009BE3C` is the live world-frame environment pointer. Its `+0x70`
  OT field is established by the prior OT setup. `D_8009D554` is a frame
  run flag: it is seeded at `0x80071308`, cleared by earlier frame/session
  paths, and is not a bounded one-shot timer.

## Classification and bounded implementation

This was a class-(c)/(d) host-boundary plus sync/render handoff slice. The
smallest safe implementation was the tail through the first backedge test:

- `SetGeomOffset` receives the retail immediate `0xA0` and signed
  `D_8009BE0C` value.
- Known PSX/KSEG1 environment and OT addresses are mapped with `PSX_ADDR`.
- Unknown environment/OT values are logged and counted; `DrawOTag` is not
  called with a raw guest or host value.
- The first natural `D554` backedge is LOG-AND-HOLD. No second scheduler
  pass or recursive jump to `0x8007130C` is introduced in this slice.

## Natural state

At frame 916, the production route entered the frame prologue once. The two
upload helpers executed with counts 2 and 3, respectively, and no unknown
upload pointers. The environment pointer was known, but its OT field was
`0x005f1068`, which failed the PSX/KSEG1 map predicate; the wrapper logged
one unknown OT and skipped `DrawOTag`. The retail backedge condition still
executed naturally: `D554=0x00000001`, hit count 1, held at `0x800719C8`.

The natural run returned `rc=0`; scheduler pass 2 remained 29 executed / 0
missing, with frontier `0x8007106C`, and the placeholder was entered cleanly.
Mode-loop `0x80072238`, renderer `0x8007299C`, and framebuffer output were
not reached.
