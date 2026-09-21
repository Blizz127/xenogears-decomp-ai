# W34N125 — world-map sprites and objects visible (slot-9 camera tail)

- Branch: `experiment/worldmap-open-gates-20260823`
- Starting HEAD: `9d5ff780` (docs: record W34N124 lahan harness pass)
- Fix commit: `618726b6` (worldmap: restore slot-9 camera tail that publishes D_8009BE28)
- Date: 2026-09-01
- Verdict: **FEI_FOLLOWER_AND_WORLD_OBJECTS_RENDER_ON_FOOT_ROUTE**

## Symptom

On the accepted natural route (Lahan exit 1 -> world session, cold
new-game state) the terrain and minimap rendered, but neither Fei, the
follower, nor any scaled object (Lahan village model, trees, the Mountain
Path landmark) appeared in any frame (`scratchpad/w34n36_clean_capture`,
and the fresh captures from the W34N124 session).

## Root cause

The world position block `D_8009BE28..BE34` stayed zero for the whole
session.  Three consumers subtract or build from it:

- `wm_80085CDC` (sprite pass): `delta = record+0x28 - BE28` per object,
  wrapped by `wm_80093484`, then `<< 4` into the render transform.
- `wm_80089C78` (scaled-object renderer): camera-relative wrap of X/Z at
  retail `0x80089D50`.
- `wm_80096F18` (view-matrix builder), called from slot 10 `wm_80091C18`
  with `pos = BE28`, contributes `pos.Y >> 12` to the view matrix.

With `BE28 == 0` the "camera-relative" object positions were absolute
world positions (`~122,000,000` in 20.12), and after the wrap and `<< 4`
they landed behind the camera: the sprite render matrices carried
`m_t = (-9818, 4501, -2610)` for Fei and `(-9825, 4504, -2607)` for the
follower (`probe_cam_before.log`), so `func_8001E3D8` produced degenerate
packets at the projection clamp.

Retail writes `BE28` from the on-foot camera callback `0x800914D0`
(slot 9).  Every path of that function exits through the common tail at
`0x80091B04`:

```text
80091b04: jal 0x80093354        ; wrap slot+0x28 (a0 = s0+0x28)
80091b10: addiu a1,a1,-16856    ; a1 = &D_8009BE28
80091b14: lw v0,40(s0) ; lw v1,44(s0) ; lw a0,48(s0)
80091b20: sw v0,0(a1)  ; sw v1,4(a1)  ; sw a0,8(a1)
80091b2c: lw v0,52(s0) ; sw v0,12(a1)
80091b38: li v0,1 ; return
```

The port's `wm_800914D0` returned 1 directly from the post-dispatch block
and never executed this tail.  No other writer of `BE28` runs on the
on-foot route: the direct `sw ...,-16856(at)` stores in `world_map.bin`
are all in the vehicle/event mode callbacks (`0x8007AD98`, `0x8007C79C`,
`0x8007CAB8`, `0x8007E49C`, `0x800805C4`, `0x80081434`, `0x80082810`,
`0x80083930`, `0x80083BE4`, `0x80083C94`), and the session-restore copy
`wm_8007565C` only runs when `D_8009C894 != 0`.

## Fix

`pc_port/src/world_map_callback_914d0.c` is retranscribed from
`[0x800914D0, 0x80091B54)` with both jump tables read from
`world_map.bin`:

- `0x80070BE4` (pre-dispatch, index `(s16)(lhu(slot+4) - 9)`, 9 entries):
  `0 -> 0x80091528`, `1 -> 0x80091558`, `2..5 -> skip`, `6 -> 0x80091570`,
  `7 -> 0x80091580`, `8 -> 0x80091590`.
- `0x80070C0C` (main dispatch, index `lh(slot+0x20)`, 17 entries):
  `0 -> 0x800915D0`, `1 -> 0x800916CC`, `2 -> 0x800916F4`,
  `16 -> 0x800917BC`, `3..15 -> skip`.

Beyond the missing tail the old body diverged from retail in:

| region | retail | old port |
|---|---|---|
| post-dispatch gate | states 0/1/2/16 -> `0x800918A4`; state 3 -> `0x80091A2C` | only states 3 and 0x10, single body |
| `0x800918A4` far branch | `slot+0x28/+0x30 += delta>>3`, `BBB4/BBBC += delta>>3`, `+0x60 = 1` | `+0x60 = 1` only |
| `0x800918A4` Y | `slot+0x2C += (by - py) >> 3` | `slot+0x2C = by` |
| `0x80091A2C` (state 3) | snap X/Z, `slot+0x2C += (by - py) >> 4` | not distinguished |
| heading accumulator | `slot+0x58 = (acc + step) & 0x00FFFFFF` | no mask |
| states 2/16 clamp | `diff >= 0 ? 384 : -384` | inverted sign |
| state 1 | always falls into the heading step `0x8009164C` | skipped when target reached |
| pre-dispatch case 0 | `slot+0x50 = lh(D_8009D52C)` | `lh(D_8009BD3A)` |

## Runtime evidence

`scratchpad/w34n125_sprite_capture/probe_cam.gdb` breaks on
`wm_80085CDC` and `func_8001E298` for the first three world frames.

Before (`probe_cam_before.log`):

```text
CAM n=2 BE28=(0, 0, 0) C808_t=(0, 0, 1120) BD40_rot=(471, -900, -472, 0, 0, 0, ...)
  E298 obj=0x6aa624 m_t=(-9818, 4501, -2610)
  E298 obj=0x6aa858 m_t=(-9825, 4504, -2607)
```

After (`probe_cam_after.log`):

```text
CAM n=1 BE28=(122665808, -1278838, 45360304) C808_t=(0, 186, 1371) BD40_rot=(471, -1213, -472, 0, 0, -313, ...)
CAM n=2 BE28=(122671238, -1280372, 45354874)
  E298 obj=0x6aa624 m_t=(5, -3, 1117)
  E298 obj=0x6aa858 m_t=(-2, 0, 1120)
CAM n=3 BE28=(122678885, -1281352, 45347226)
```

`BE28` now follows slot 1 (`pos = 122686080 -> 122732416`) with the 1/8
step, and both sprites project about 1117 units in front of the camera.

Captures (`scratchpad/w34n125_sprite_capture/capture.gdb`, schedule
`0:0x2000,100:0x8000,240:0`, 250 world frames):

| frame | BMP sha256 | content |
|---:|---|---|
| 60 | `776af6ac5b99de9a43c75f23aa08ff05054091ae88b21dd93195f94ee0a209f5` | Fei + follower walking, Lahan model, trees, Mountain Path landmark |
| 120 | `f7e0f749b291fcfce130f41c0abde97f55520dfc459c5448a2b0846143556877` | same, camera rotated |
| 180 | `9869a70b41e3c91261a6511dcc63d29008088090648d938f3ec87d47240cfb63` | same |
| 240 | `e432eff9cf321b3ec3dedafe61add03c2f86e199fe92ab61f06934edec4499dd` | Fei + follower standing, camera turned |

PNG conversions sit next to the BMPs.

## Regression checks

- `./pc_port/build_port.sh` -> `LINK OK`.
- `run_w34n124_world_walk_entry.sh lahan` and `mountain`: 6/6 PASS each,
  identical positions and trigger ids to the W34N124 record.
- `make check`: 4/4 FAILED both at HEAD `9d5ff780` and with the fix;
  hashes identical (`slus 34e552c6…`, `field af1e5dc3…`,
  `member 242c5450…`, `shop 0f22fa9a…`).  `pc_port/` is not part of that
  build.  Note: member/shop were OK in the last grind-log baseline; they
  now fail at HEAD before this change (candidate: `2261d026` touched
  `src/menu/main/misc.c` and `src/slus_006.64/system/menu.c`).

## Open, separate

- Hard-edged white rectangles appear over the terrain in frames 120-240
  (cloud/shadow tile layer drawn untextured, or a paging tile fault).
  This is the previously recorded malformed-terrain item and is not
  affected by this rung.
- No standalone certificate exists for `wm_800914D0`; verification here
  is runtime-only (probe + captures + W34N124).
