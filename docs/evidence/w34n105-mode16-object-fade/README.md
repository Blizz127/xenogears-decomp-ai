# W34N105 — mode-16 first object/fade stream

## Scope and retail anchor

- Starting HEAD: `fd2943cace1d32f1040b191e638cbe5f6a5fe29d`
- Branch: `experiment/worldmap-open-gates-20260823`
- Retail image: `disc/world_map.bin`, loaded at `0x8006FAF0`
- Disassembly: `scratchpad/w34_render_scan_k/world_map_full.objdump.txt`
- stream helper `[0x800816DC,0x800817A0)`, 196-byte SHA-256:
  `0f83d78059f92926b4e908f3ae687aaaa0da51239fd71e8fcaba07ea18f34b18`
- initializer `[0x800817A0,0x80081868)`, 200-byte SHA-256:
  `6f4bba9eb5e5535d43b2f0361319cac26f1035ecc8c760fdb73a9a05bdef0c7a`
- update `[0x80081868,0x800819C8)`, 352-byte SHA-256:
  `e2a6561950d1696cadd6e0ce5ba90866ccb8143913205be0bd6740d94e1ec540`
- complete unit `[0x800816DC,0x800819C8)`, 748-byte SHA-256:
  `c4547102a4cbed6aa366ecd5c1d496581b678af45def85de04b7cd3f72247b3e`

The only external retail calls are `GetTPage` (`0x80043A1C`), `memcpy`
(`0x8003F968`), and the already certified port-owned color helper
`wm_800809EC`. No new subsystem or guessed dependency was introduced.

## Production transcription

`wm_800816DC` initializes each 40-byte primitive record with retail length
9, opcode 44 plus semi-transparency bit 1, and
`GetTPage(0, abr, 384, 0)`, then mirrors the complete initialized stream from
the owner's `+0x48` buffer to its `+0x4C` buffer.

`wm_800817A0` restores the scheduler initializer:

1. seeds the slot position from `C5AC/B0/B4`, subtracting `0x100000` from Y;
2. clears the slot state and three fade channels;
3. initializes the context `+0xA8` primitive stream through the helper; and
4. publishes the signed fixed-point position (`>> 12`) to context
   `+0xB0/+0xB4/+0xB8`.

`wm_80081868` restores both retail latch paths and the per-frame fade:

- latch 1 clears itself and activates the fade;
- latch 2 clears itself, stops the fade, restores RGB 128 to every primitive
  in the currently selected `D7F0` buffer, and clears opcode bit 1;
- active fades increment all three channels and saturate all of them at 255
  when red reaches 255; and
- every update applies the current channels to the selected stream through
  `wm_800809EC`.

The scheduler resolves `0x800817A0` and `0x80081868` symbolically; neither
guest address is cast to a native function pointer.

## Focused certificate

Command:

```text
pc_port/tests/run_w34n105_mode16_object_fade.sh
```

Result:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- helper/initializer/update/full retail slice hashes: PASS
- exact `GetTPage` arguments and primitive layout: PASS
- source-to-mirror copy direction and bounds: PASS
- initializer position, signed publication, fade seed, and stream setup: PASS
- latch-1 activation and selected-stream color call: PASS
- latch-2 RGB/opcode restoration without fade advance: PASS
- fade saturation and stop boundary: PASS
- symbolic scheduler initializer/update resolution: PASS
- M1 wrong primitive opcode: detected by `helper.packet_layout`
- M2 wrong texture-page X: detected by `helper.tpage_args`
- M3 reversed stream copy: detected by `helper.copy_direction`
- M4 missing initial Y offset: detected by `init.position`
- M5 logical rather than arithmetic Y publication: detected by
  `init.position_publish`
- M6 latch 1 leaves fade idle: detected by `update.latch1`
- M7 latch 2 retains semi-transparency bit: detected by
  `update.latch2_restore`
- M8 wrong fade saturation boundary: detected by `update.fade_saturation`

All M1-M8 were killed by named assertions. The normal product build completed
with `LINK OK`.

## Natural entrance-16 route

The accepted detached entrance-16 route executed `0x800817A0` once and
`0x80081868` 120 times. Neither address produced a stub hit. Both capture
requests were fulfilled in-frame, both upload pumps retained `unknowns=0`,
and the bounded loop returned normally after exactly 120 frames.

- frame 60:
  `/home/blizz/dev/xenogears-decomp/scratchpad/w34n105_mode16_capture/world-frame-000060.bmp`
- frame 60 SHA-256:
  `a3f49cd80577f99a6a8494705e4c8720da6e666390857fa233b68d964bb02559`
- frame 120:
  `/home/blizz/dev/xenogears-decomp/scratchpad/w34n105_mode16_capture/world-frame-000120.bmp`
- frame 120 SHA-256:
  `45ed05aeb486205478d296563b2110598d1a0dfdd2a6c6b9295551079673828d`

Both hashes differ from W34N104 and from each other. Visual inspection shows
the ocean/coastline/terrain/sky scene and the central tower-like object inside
its translucent enclosure. The object stream is now initialized through the
retail helper and its appearance changes with the camera/fade evolution.

Three private mode-16 initializer addresses remain unresolved, each with 121
stub hits:

- `0x800819C8`
- `0x80081C3C`
- `0x80081FB4`

## Verdict and next target

`MODE16_FIRST_OBJECT_FADE_RESTORED_NATURALLY_EXECUTED_VISUAL_ADVANCE`

The next target is `0x800819C8/0x80081B24`, the fourth private pair in retail
registration order and now the earliest unresolved callback on the natural
route.
