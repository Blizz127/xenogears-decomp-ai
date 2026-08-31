# W34N75 — mode-14 camera-control callback pair

## Scope and retail anchor

This code-producing rung transcribes retail `[0x8007AD34,0x8007B200)`, the
last unresolved scheduler callback pair registered by mode-14 setup
`0x8007A5DC`.

Authority:

- `disc/world_map.bin`, mapped at `0x8006FAF0`;
- `scratchpad/w34n73_7ad34.objdump`;
- command jump table at `0x8006FBF4` and active-state jump table at
  `0x8006FC14`, read directly from the retail binary;
- exact 1228-byte slice SHA-256:
  `69e64e42ea9b659bb1e169164f790197a06aade4cd5678978a4bf8402bf704f7`.

`0x8007AD34` publishes the reset position to both the live and shadow world
position records, seeds geometry Y, view height, camera angles, and slot scale,
and returns scheduler state 1.

`0x8007ADD4` implements two independent retail dispatches.  Latch values 1–7
select scale-up, scale-down, high-camera reset, normal-camera reset,
minimum-scale ramp, tracking-camera flight, and yaw-ramp commands.  Active
states 0–5 then select the generic camera builder, the three exact scale ramps,
the tracking/look-vector camera, or the yaw ramp.  Every path finishes through
the retail two-sample random jitter tail, which adds the same X/Y perturbations
to both camera-input points.

## Production change

- Added `world_map_callback_7ad34.c/.h`.
- Registered `0x8007AD34/0x8007ADD4` in the bounded scheduler resolver.
- Added the source to the normal port build manifest.

No lifecycle code or other mode callback was changed.  Guest callback
addresses remain inert data until explicitly resolved to these linked native
functions.

## Focused certificate

Command:

```text
pc_port/tests/run_w34n75_mode14_camera_control.sh
```

Result:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- M1 wrong initializer view height: detected by `init.view_height`
- M2 wrong latch-2 scale seed: detected by `latch2.scale`
- M3 wrong scale-up clamp: detected by `state.scale_up_clamp`
- M4 wrong tracking-camera vertical offset: detected by
  `tracking.camera_input`
- M5 early yaw-ramp completion: detected by `state.angle_boundary`
- M6 omitted jitter mirror writes: detected by `jitter.mirrors`

The certificate additionally proves all seven command-latch effects; the
scale-up, scale-down, minimum-scale, tracking, and angle boundaries; reset and
shadow position publication; generic-camera arguments; tracking-camera input
construction and helper selection; both random samples and mirrored jitter
writes; return states; and scheduler resolution for both guest addresses.

Normal port build: `LINK OK`.

Adjacent regressions:

- W34N74 second scaled-object pair: PASS in O0/O2/UBSan, M1–M6 detected.
- W34N72 path-camera pair: PASS in O0/O2/UBSan, M1–M6 detected.

## Runtime boundary and next target

Mode 14 remains outside the accepted natural base route, so this rung makes no
visual claim.  All mode-14-specific callback pairs registered by setup are now
owned and scheduler-resolvable.  The next bounded target is lifecycle
integration of retail mode-14 setup/teardown `0x8007A5DC/0x8007A8AC`, followed
by natural mode-14 entry, displayed-frame, and teardown acceptance.
