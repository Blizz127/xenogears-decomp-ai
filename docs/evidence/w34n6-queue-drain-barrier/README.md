# W34N6 — retail world queue-drain barrier 0x80096694

Verdict: **TRANSCRIBED_AND_CERTIFIED**.

This is the first code-producing Lane B checkpoint after W34N4. It replaces
the active frame driver's local no-op for retail `0x80096694` with compiled,
called production C. It does not alter the broader session-loop structure.

## Anchor and retail decode

- Branch: `experiment/worldmap-open-gates-20260823`
- Starting HEAD: `66f100ee8acf33d152083ef67342a928c889c1f1`
- Local matched origin before the change.
- Retail authority: `disc/world_map.bin`, loaded at `0x8006FAF0`.
- Disassembly cross-check:
  `scratchpad/w34_render_scan_k/world_map_full.objdump.txt`.

Retail `[0x80096694,0x800966CC)` is 14 instructions:

```
80096694  addiu sp,sp,-24
80096698  save ra
8009669C  jal Vsync(0)
800966A4  jal 0x800967E4
800966AC  jal 0x80096668
800966B4  bnez v0,0x8009669C
800966BC  restore ra
800966C4  jr ra
```

Consequently it is a do/while barrier. Even an already-empty queue performs
one `Vsync(0)` and one `0x800967E4` dispatcher poll, then the helper repeats
until the circular head/tail distance returned by `0x80096668` is zero.

All dependencies were already present:

- `wm_800967E4` in `world_map_helper_96130.c`;
- exact `wm_80096668_circular_distance` in `world_map_init.c` (W25B);
- host `Vsync`.

The implementation was added beside `wm_800967E4`, exported through
`world_map_helper_96130.h`, and the private no-op was removed from
`world_map_frame_driver_712d0.c`. The driver already included that header and
its natural-session epilogue now resolves to the production function.

## Focused production certificate

Runner: `pc_port/tests/run_w34n6_queue_barrier.sh`.

The test links the real `world_map_helper_96130.c` and exercises:

- empty queue: exactly one VSync/dispatcher iteration;
- linear distance three: three items drained, VSync before each dispatch;
- wraparound tail 15 to head 1: two items drained and tail wraps to 1.

Results:

```
O0 PASS
O2 PASS
nonrecovering UBSan PASS
strict focused warnings PASS
M1 precheck empty queue: DETECTED by empty_do_while_vsync
M2 omit VSync: DETECTED by empty_do_while_vsync
M3 stop below distance two: DETECTED by linear_tail_drained
M4 dispatch before VSync: DETECTED by linear_vsync_before_dispatch
```

The exact circular-distance helper remains independently covered by W25B;
this certificate tests the retail composition and order around it.

## Port integration and runtime neutrality

Normal `./pc_port/build_port.sh` completed:

```
LINK OK -> pc_port/build_native/xeno-port (port-owned addresses verified)
```

The accepted detached 120-frame route completed on the final binary. Because
that bounded route never takes the natural-session epilogue, the repair must
be neutral there. It was byte-identical to the standing baseline:

- frame 60:
  `26e59e424829349c4d47de6523b3dbbf9e7d91f35629ec026c4709e703e576b3`
- frame 120:
  `ae06268a046507f3544a13e9c8708bbec57bcba16c526fd68eecf1e3f982a3dc`
- bounded exit: `frames=120 limit=120`

The detached child was terminated after the bounded world loop returned,
matching the established harness policy; the surrounding port caller remains
open after this test seam.

## Remaining boundary

This checkpoint proves `0x80096694` itself and wires the natural frame-driver
epilogue to it. The current accepted route does not naturally set `D554=0`, so
it does not yet prove the helper under live natural teardown. That acceptance
belongs with the later slot-2/session-exit integration run, using the retail
event oracle banked at W34N5.
