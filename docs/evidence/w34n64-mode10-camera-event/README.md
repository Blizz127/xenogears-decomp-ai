# W34N64 — retail mode-10 camera/event callback pair

## Scope and anchor

- Starting HEAD: `0f7c8d55e327f6a1ef49c4ed7d54bb19cae0432e`
- Branch: `experiment/worldmap-open-gates-20260823`
- Retail source: `disc/world_map.bin`, loaded at `0x8006FAF0`
- Retail callback pair: `0x80078E2C/0x80078EA4`, ending at `0x800794D8`
- Slice SHA-256: `9ee1f145bcf085ddff75248b9d2dbea1bb7d2261dbbdf12906725b3dab08e09c`

This is a bounded code-producing mode-10 slice. It does not integrate the
mode-10 lifecycle or alter an accepted natural route.

## Production behavior restored

`wm_80078E2C` restores the retail initializer: geometry Y 120, camera height
`0x00200000`, mode state zero, callback state 16, cleared phase words, camera
angles `(-192, 1664, 0)`, timer 64, and cleared slot latch.

`wm_80078EA4` restores the complete retail 19-state dispatch domain. States
0–7 and 16–18 have behavior; states 8–15 are deliberate no-ops. The active
states preserve retail's:

- timer transitions and slot claims;
- packed sound-bank IDs and start/stop/fade calls;
- paired camera-input jitter with exact random moduli and biases;
- camera height and angle transition at state 17;
- mode/control publications at states 6, 7, 16, and 18; and
- state-18 loop reset back to state zero.

The view producer `wm_80096F18` runs from the common tail unless the
post-switch state is 4 or 5. Those two jitter states build the view at entry;
when state 5 expires into state 6, retail therefore performs the second
common-tail view build. The implementation and certificate preserve that
post-switch ordering rather than flattening it to one call per callback.

The scheduler now resolves guest addresses `0x80078E2C` and `0x80078EA4`
directly to their port-owned bodies without a guest-function-pointer cast.

## Focused certificate

`pc_port/tests/run_w34n64_mode10_camera_event.sh`:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- initializer state, all live dispatch states, timer/claim/audio ordering,
  jitter arithmetic, global publications, loop reset, view-call placement,
  and scheduler resolution: PASS
- M1 wrong initial state: detected by `init.state16`
- M2 missing state-0 claim: detected by `state0.claim`
- M3 missing final state-2 claim: detected by `state2.claims`
- M4 wrong state-4 random modulus: detected by `jitter.state4`
- M5 missing mirrored jitter angle: detected by `jitter.state4`
- M6 wrong state-6 globals: detected by `state6.globals`
- M7 inverted state-16 flag gate: detected by `entry.flag_gate`
- M8 wrong state-18 loop reset: detected by `entry.loop_reset`
- M9 duplicate common-tail view call in jitter states: detected by
  `jitter.view_once`

## Regression and build gates

- W34N63 scaled-stream callback certificate, M1–M7: PASS
- W34N62 small mode-10 callback certificate, M1–M6: PASS
- W34C1 scheduler cadence certificate, M1–M21: PASS
- canonical `./pc_port/build_port.sh`: LINK OK; port-owned addresses verified
- `git diff --check`: clean

## Remaining mode-10 frontier

Only the mode-10 callback pair `0x800795E4/0x80079778` remains unresolved.
After it lands, the already-decoded `0x80078A60/0x80078D24` setup/teardown
lifecycle can be integrated and taken through natural visual and teardown
acceptance.
