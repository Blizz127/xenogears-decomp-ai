# W34N116 — retail mode-18 camera controller

## Anchors

- Starting HEAD: `52407ba93354127a40aa002ea0dea858bdabedac`.
- Canonical image: `disc/world_map.bin`, load base `0x8006FAF0`.
- `wm_80083A00 [0x80083A00,0x80083FE4)`, SHA-256
  `5da7664c8dd74594161dbf6da5da01e60d419d1a73956589238c8f57818830a8`.
- The two retail jump tables at `0x80070490` and `0x800704A8` were decoded
  from the canonical image rather than inferred from another port callback.

## Production result

`wm_80083A00` is now an exact bounded native transcription of the mode-18
slot-2 update controller and resolves symbolically through the scheduler. It
implements:

- all six retail latch presets and their exact camera, position, target, and
  world-coordinate side effects;
- the conditional `wm_80096F18` camera build;
- the state-1 and state-5 retail interpolation calls to `wm_800771D8`;
- the already-certified `wm_80076DA4`, `wm_80076F54`, and `wm_80076FA8`
  motion pipeline in retail order; and
- the centered retail random jitter publication to scratch and the two angle
  accumulators.

No new helper or subsystem was invented for this body.

## Certificate

`pc_port/tests/run_w34n116_mode18_camera.sh`:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- M1 wrong latch-1 state: DETECTED by `latch.mapping`
- M2 wrong latch-2 Y: DETECTED by `latch2.position`
- M3 missing latch-3 world-X adjustment: DETECTED by
  `latch3.world_x`
- M4 reversed latch-4 drift: DETECTED by `latch4.world_drift`
- M5 missing camera build: DETECTED by `camera.call_contract`
- M6 wrong state-1 step sign: DETECTED by `state1.interpolation`
- M7 wrong state-5 target: DETECTED by `state5.interpolation`
- M8 missing motion pipeline: DETECTED by `motion.pipeline`
- M9 uncentered jitter: DETECTED by `jitter.center`

The exact staged tree was materialized as isolated tree commit
`511a52541257e1f7264694578175d034547417ff`; its normal PC port build linked
successfully, and the resulting executable contained a strong
`wm_80083A00` symbol. This isolated build was necessary because unrelated
concurrent boot/menu work in the shared worktree temporarily made
`boot_str.c` fail compilation and also overwrote the shared `xeno-port`
artifact after an earlier successful link. That superseded shared-binary run
is not used as evidence here.

## Natural entrance-18 acceptance

The exact isolated executable ran the detached entrance-18 route with the
standard scripted input for 120 bounded frames.

- `wm_80083A00` executed successfully 120 times and had zero stub hits.
- `wm_80084068` remains the only unresolved mode-18 callback. Its failed
  update alternates with reinitialization, so it produced 60 stub hits.
- Both upload pumps retained `unknowns=0`.
- Frame-60 and frame-120 capture requests were fulfilled on their requested
  frames.
- The bounded loop returned after exactly 120 frames.

Capture digests:

- frame 60:
  `5c49c8cca07ebf07bed45c20ad5d6df3fd78b448dc0295d8ed71d13efa221612`
- frame 120:
  `c9fdec8599922be43e94e3d51a44561d509891b150c91847dbb51331726d9cad`

Frame 60 shows a coherent horizon, layered mountains, clouds, and textured
foreground terrain. Frame 120 shows a distinct elevated/descending terrain
view with clouds. The flat white foreground seen at W34N115 is absent. The
two different images prove that the camera sequence evolves naturally.

This is a major visual frontier movement, but mode 18 is not yet complete:
the slot-3 object update is still absent.

## Next exact frontier

Transcribe `wm_80084068 [0x80084068,0x8008440C)`, the sole remaining private
mode-18 callback. Its natural scheduler failure is already bounded to 60
attempts on this route.
