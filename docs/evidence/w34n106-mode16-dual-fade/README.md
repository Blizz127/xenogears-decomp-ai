# W34N106 — mode-16 dual object/fade stream

## Scope and retail anchor

- Starting HEAD: `9a41a4f1dce24993d4db4c3a58db5c65cbb0229f`
- Branch: `experiment/worldmap-open-gates-20260823`
- Retail image: `disc/world_map.bin`, loaded at `0x8006FAF0`
- Disassembly: `scratchpad/w34_render_scan_k/world_map_full.objdump.txt`
- initializer `[0x800819C8,0x80081B24)`, 348-byte SHA-256:
  `d2bd501688fc74d3a383ac5784047a1b2333d1580a51524e3766bec9c64a7396`
- update `[0x80081B24,0x80081C3C)`, 280-byte SHA-256:
  `ce39af781dc1bdeaff8b117a059abc69cef2945e7c493402717cc74b106e635f`
- complete pair `[0x800819C8,0x80081C3C)`, 628-byte SHA-256:
  `c6ba5b2910adea6da0c75d2840980017e99139474691045bcfd1700d70078784`

The pair depends only on the existing PsyQ `ScaleMatrix`, the W34N105
primitive-stream initializer `wm_800816DC`, and the already certified
port-owned color helper `wm_800809EC`.

## Production transcription

`wm_800819C8` restores the fourth private mode-16 initializer:

1. seeds the scheduler slot position directly from `C5AC/B0/B4` and clears
   its state and RGB fade channels;
2. copies the retail base matrix at `0x8009A180` to context `+0x20`;
3. scales it by `(4096,28672,4096)` through guest scratch `0x1F800000`;
4. mirrors the scaled matrix to context `+0x74`; and
5. initializes both object owners (`context` and `context+0x54`) with ABR 3.

`wm_80081B24` restores the updater:

- latch 1 clears itself and activates the fade;
- active fades advance red/green/blue by 4/2/1 respectively;
- when signed red reaches 252, retail clamps only red to 252 and stops the
  fade; green and blue retain their independently advanced values; and
- both object owners' currently selected `D7F0` streams receive the same
  current RGB values through `wm_800809EC`.

The scheduler resolves both guest callbacks symbolically.

## Focused certificate

Command:

```text
pc_port/tests/run_w34n106_mode16_dual_fade.sh
```

Result:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- initializer/update/full retail slice hashes: PASS
- exact initial position and zero fade state: PASS
- base-matrix source, scale-vector values, call order, and matrix mirror: PASS
- both stream-owner/count/source/ABR calls: PASS
- latch-1 activation: PASS
- exact 4/2/1 fade steps: PASS
- `D7F0` buffer selection and both color calls: PASS
- red-only saturation at 252: PASS
- symbolic scheduler initializer/update resolution with one occupied slot:
  PASS
- M1 wrong initial Y: detected by `init.position`
- M2 wrong scale Y: detected by `init.scale`
- M3 skipped matrix mirror: detected by `init.matrix_mirror`
- M4 wrong first-stream ABR: detected by `init.streams`
- M5 skipped second stream: detected by `init.streams`
- M6 latch leaves fade idle: detected by `update.latch`
- M7 wrong red step: detected by `update.fade_steps`
- M8 incorrectly clamps all channels: detected by `update.fade_saturation`

All M1-M8 were killed by named assertions. The normal product build completed
with `LINK OK`.

## Natural entrance-16 route

The detached entrance-16 route executed `0x800819C8` once and `0x80081B24`
120 times. Neither produced a stub hit. Both capture requests were fulfilled
in-frame, both upload pumps retained `unknowns=0`, and the bounded loop
returned normally after exactly 120 frames.

- frame 60:
  `/home/blizz/dev/xenogears-decomp/scratchpad/w34n106_mode16_capture/world-frame-000060.bmp`
- frame 60 SHA-256:
  `eb6195d9423dfcbaa3434d60216fa8766c87fbc4c46e7577cfd98fc53bc13d93`
- frame 120:
  `/home/blizz/dev/xenogears-decomp/scratchpad/w34n106_mode16_capture/world-frame-000120.bmp`
- frame 120 SHA-256:
  `de1764e1541b990ea7a77fc607b5c0097ed691c8d4f6ff15c151519297af20f4`

Both hashes differ from W34N105 and from each other. Visual inspection shows
the coherent tower/landscape scene with a materially wider and brighter
translucent enclosure, consistent with the restored dual-stream fade path.

Two private mode-16 initializer addresses remain unresolved, each with 121
stub hits:

- `0x80081C3C`
- `0x80081FB4`

## Verdict and next target

`MODE16_DUAL_OBJECT_FADE_RESTORED_NATURALLY_EXECUTED_VISUAL_ADVANCE`

The next target is `0x80081C3C/0x80081D80`, the fifth private pair in retail
registration order and now the earliest unresolved callback on the natural
route.
