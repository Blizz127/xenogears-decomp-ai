# W34N104 — mode-16 camera control

## Scope and retail anchor

- Starting HEAD: `647ccb0088ec34716e0a2592f485e6addf35633d`
- Branch: `experiment/worldmap-open-gates-20260823`
- Retail image: `disc/world_map.bin`, loaded at `0x8006FAF0`
- Disassembly: `scratchpad/w34_render_scan_k/world_map_full.objdump.txt`
- initializer `[0x800813E8,0x80081470)`, 136-byte SHA-256:
  `da6c4f94b4c53a470378bc52a4b2fb46886389cb58b48869b2ea311ee9663852`
- update `[0x80081470,0x800817A0)`, 816-byte SHA-256:
  `60760d92a5f790f24cce93b443dbecfb41fc1dfda25aed7d62d87d0cdf33515f`
- complete pair `[0x800813E8,0x800817A0)`, 952-byte SHA-256:
  `ad4aecd7dc6886e8f04464ebb5ca0c354b32f824382923a5b5d68631c6a69673`

## Production transcription

`wm_800813E8` restores the mode-16 camera initializer: camera-gate reset,
4096 jitter amplitude, 120-pixel view height, live and shadow position copies
from `C5AC/B0/B4`, latch 1, and idle state.

`wm_80081470` restores the retail update sequence:

1. latch 1 selects active state, angles `(-128,-512,0)`, height
   `0x00980000`, and matching fixed-point convergence targets;
2. latch 2 and all values outside the handled latch-1 case remain untouched;
3. when `D144 == 0`, `wm_80096F18(BD40,BE28,D3F0,BD38)` builds the
   camera through the already accepted producer;
4. active state eases height toward `0x00630000`, the first angle toward
   `0x00010000`, and the second toward `0x004B0000`, using retail shifts
   5, 4, and 5 respectively;
5. signed, amplitude-scaled random jitter is applied to both retail camera
   input halfwords through scratch `0x1F8000A2`.

All fixed-point shifts use explicit arithmetic semantics, avoiding host
signed-shift assumptions. The scheduler resolves both guest callbacks
symbolically.

## Focused certificate

Command:

```text
pc_port/tests/run_w34n104_mode16_camera_control.sh
```

Result:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- initializer/update/full retail slice hashes: PASS
- camera constants, live/shadow position copies, and slot seed: PASS
- latch-1 constants and first convergence step: PASS
- retail latch-2 no-op behavior: PASS
- camera-gate behavior and exact producer arguments: PASS
- primary, roll, and pitch clamps and easing shifts: PASS
- jitter range, bias, scratch value, and dual mirror: PASS
- symbolic scheduler initializer/update resolution: PASS
- M1 wrong view height: detected by `init.camera_constants`
- M2 wrong latch-1 pitch: detected by `latch1.state`
- M3 skipped generic camera: detected by `camera.generic_call`
- M4 wrong height clamp: detected by `state1.primary_clamp`
- M5 wrong roll easing shift: detected by `state1.roll_approach`
- M6 wrong pitch easing shift: detected by `state1.pitch_approach`
- M7 wrong jitter bias: detected by `state1.jitter_mirror`
- M8 skipped second jitter mirror: detected by `state1.jitter_mirror`

All M1-M8 were killed by named assertions. The normal product build completed
with `LINK OK`.

## Natural entrance-16 route

The detached entrance-16 route executed `0x800813E8` once and `0x80081470`
120 times. Neither address produced a stub hit. Both capture requests were
fulfilled in-frame, both upload pumps retained `unknowns=0`, and the bounded
loop returned normally.

- frame 60 SHA-256:
  `d4ef0ea7ea83456e2b9f4af5d85f6308eae215fd47f4a885f3c3dfa310990c9f`
- frame 120 SHA-256:
  `ef41f2ff09437be5ce1338d044d78da557d0db9b61d66190854709dd13dc0810`

Both hashes differ from W34N103 and from each other. Visual inspection shows
the world ocean, coastline, distant terrain, and sky, with a large tower-like
object centered inside a translucent enclosure. The framing changes between
frames 60 and 120 exactly as the restored camera converges. This is a material
rendering advance, not merely a callback-count change.

Four private mode-16 initializer addresses remain unresolved, each with 121
stub hits:

- `0x800817A0`
- `0x800819C8`
- `0x80081C3C`
- `0x80081FB4`

## Verdict and next target

`MODE16_CAMERA_CONTROL_RESTORED_NATURALLY_EXECUTED_VISUAL_ADVANCE`

The next target is `0x800817A0/0x80081868`, the third private pair in retail
registration order. It owns the first mode-16 model/primitive stream and is
now the earliest unresolved callback on the natural route.
