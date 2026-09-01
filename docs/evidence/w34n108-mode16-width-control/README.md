# W34N108 — final mode-16 width controller

## Scope and retail anchor

- Starting HEAD: `d5f2357357e5ac38dbf06c1f0aa60a50625dec2d`
- Branch: `experiment/worldmap-open-gates-20260823`
- Retail image: `disc/world_map.bin`, loaded at `0x8006FAF0`
- Disassembly: `scratchpad/w34_render_scan_k/world_map_full.objdump.txt`
- initializer `[0x80081FB4,0x80081FD8)`, 36-byte SHA-256:
  `0e39044d68e3c1baa81845a86797cac09e7e749e38436b362dc2c417acc5e9ca`
- update `[0x80081FD8,0x80082324)`, 844-byte SHA-256:
  `489182ddd290227f2577f67778abfd1bd6d079068007f2d1e79482bb25da11c4`
- complete pair `[0x80081FB4,0x80082324)`, 880-byte SHA-256:
  `7151eab77895432a0219d46eca8d551471bcd35435cba019b897be40c3b2b9b6`
- Retail jump tables at `0x800703EC` and `0x80070404` were read directly
  from the image. They map commands 1–5 to controller states 1–5 and
  states 0–5 to the six decoded update cases.

## Production transcription

`wm_80081FB4` restores the sixth and final private mode-16 initializer. It
sets the slot controller state at `+0x20` to zero and its 32-bit current
width at `+0x50` to one.

`wm_80081FD8` restores the retail width-table controller consumed by the
W34N107 distortion strip:

- a nonzero command latch at slot `+0x04` maps commands 1–5 to states 1–5
  and is consumed;
- state 0 fills all 192 widths from slot `+0x50`, then applies the exact
  three randomized 64-entry-band walks;
- state 1 increments the width, clamps it at 64, and fills the table;
- state 2 decrements the width, clamps it at 2, and fills the table;
- state 3 sparsely randomizes individual entries with the retail
  one-in-four gate and range 1–64;
- state 4 fills every entry with 2; and
- state 5 and out-of-range states are no-ops.

The scheduler resolves `0x80081FB4` and `0x80081FD8` symbolically. No guest
callback address is cast to a host function pointer.

## Focused certificate

Command:

```text
pc_port/tests/run_w34n108_mode16_width_control.sh
```

Result:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- initializer/update/full retail slice hashes: PASS
- exact initializer state and width: PASS
- command consume and 1–5 mapping: PASS
- state-0 full fill and all three randomized bands: PASS
- state-1 grow/clamp/fill: PASS
- state-2 shrink/clamp/fill: PASS
- state-3 sparse randomization: PASS
- state-4 constant fill and state-5 no-op: PASS
- symbolic scheduler initializer/update resolution: PASS
- M1 wrong initial state: detected by `init.state`
- M2 wrong initial width: detected by `init.width`
- M3 retained command latch: detected by `command.consume_and_map`
- M4 wrong command mapping: detected by `command.consume_and_map`
- M5 skipped state-0 fill: detected by `state0.fill_and_bands`
- M6 wrong grow clamp: detected by `state1.grow_clamp_fill`
- M7 wrong shrink clamp: detected by `state2.shrink_clamp_fill`
- M8 wrong sparse-random gate: detected by
  `state3.sparse_randomization`
- M9 wrong state-4 fill: detected by `state4.constant_fill`

All M1–M9 were killed by named assertions. The W34N102 lifecycle and W34N107
distortion-strip certificates were rerun and passed. The normal product build
completed with `LINK OK`.

## Natural entrance-16 route

The detached entrance-16 route executed every private mode-16 initializer
once and every private updater 120 times. Specifically for this pair:

- `0x80081FB4`: 1 successful initializer execution
- `0x80081FD8`: 120 successful update executions
- pair stub hits: 0
- all private mode-16 callback stub hits: 0
- upload-pump unknowns: 0 on both pumps
- capture requests 60 and 120: fulfilled on the requested frame
- bounded exit: exactly 120 frames, returned normally

Captures:

- frame 60:
  `/home/blizz/dev/xenogears-decomp/scratchpad/w34n108_mode16_capture/world-frame-000060.bmp`
- frame 60 SHA-256:
  `42e4bff9b5ff9b0a8b25f13b19a70c1c15b999abf9fe8b5c8fc350cfee5d897c`
- frame 120:
  `/home/blizz/dev/xenogears-decomp/scratchpad/w34n108_mode16_capture/world-frame-000120.bmp`
- frame 120 SHA-256:
  `9f8bab9364c7ff28fb03e47e8715310c4bd213ca18b44768ad3e46bc67107b5f`

Both hashes differ from the committed W34N107 baselines
(`2a5aaa0a...` / `1664fa97...`) and from each other. Visual inspection shows
the coherent terrain/tower presentation with the restored central
distortion/copy region evolving between the two captures.

Two unrelated generic host stubs remain visible on this route:
`func_80028B14` and `SpuSetNoiseClock`. Neither is a private mode-16
scheduler callback.

## Verdict

`MODE16_PRIVATE_CALLBACK_FRONTIER_COMPLETE_NATURALLY_EXECUTED`

All six private mode-16 initializer/update pairs are now port-owned,
symbolically dispatched, certified, and naturally exercised. The next exact
world-map target must come from the remaining route-wide stub/invariant
census rather than another mode-16 private callback.
