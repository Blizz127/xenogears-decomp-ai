# W34N72 — mode-14 path-camera callback pair

## Scope and retail anchor

This code-producing rung transcribes retail `[0x8007BB60,0x8007BF50)`, the
mode-14 scheduler pair registered by setup `0x8007A5DC`.  It does not yet
integrate the mode-14 lifecycle.

Authority:

- `disc/world_map.bin`, mapped at `0x8006FAF0`;
- `scratchpad/w34_render_scan_k/world_map_full.objdump.txt`;
- `config/symbol_addrs.slus_006.64.txt` for the PsyQ helper identities;
- exact 1008-byte slice SHA-256:
  `762eca28a6a254d628f0b0770d2ecdae640df56063d76d4bb032e8f5fc9fe1e6`.

`0x8007BB60` links context records 1, 2, and 3 to record 0, seeds the
scheduler slot's phase, roll, and speed as `0,256,256`, clears the three
context Euler halfwords, builds the initial rotation matrix, and returns
scheduler state 3.

`0x8007BBEC` implements the retail nine-segment path camera:

- segments 0–1 advance by the current speed and add 4 to roll;
- segments 2–4 reduce speed by 8 with a lower clamp of 128 and add 4 to
  roll;
- segments 5–7 add 8 to speed with an upper clamp of 256 and subtract 16
  from roll;
- segment 8 returns scheduler state 3 without advancing phase;
- the first `wm_80076858` sample observes the `-1` path sentinel, while the
  phase+128 sample is unconditional;
- the two samples produce the context position and a normalized path basis;
- the basis is assembled in scratch, transposed into `context+0x20`, and
  converted back to marker Euler angles by `wm_80097070`;
- marker 18 is published only when signed context Y is at least `-127`.

PsyCross does not export retail `TransposeMatrix` (`0x8004A8EC`).  The
production callback therefore contains a local exact nine-halfword
transpose matching that retail helper's bounded operation.  It writes no
translation words and does not create a new global compatibility seam.

## Production change

- Added `world_map_callback_7bb60.c/.h`.
- Registered `0x8007BB60/0x8007BBEC` in the bounded scheduler resolver.
- Added the source to the normal port build manifest.

Guest callback addresses remain data until the explicit resolver maps them
to linked native bodies; no raw guest function pointer is called.

## Focused certificate

Command:

```text
pc_port/tests/run_w34n72_mode14_path_camera.sh
```

Result:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- M1 missing third context link: detected by `init.links`
- M2 wrong lower speed clamp: detected by `motion.low_clamp`
- M3 wrong upper speed clamp: detected by `motion.high_clamp`
- M4 ignored path sentinel: detected by `sample.sentinel_skip`
- M5 omitted basis transpose: detected by `matrix.transpose`
- M6 wrong marker threshold: detected by `marker.visibility`

The certificate additionally checks the initializer return and destination,
all three motion families plus the segment-8 return, exact blend arguments,
sentinel preservation of the first sample, context position publication,
vector/matrix helper order, the transposed nine-halfword basis, Euler output
and final angle negation, hidden-marker behavior, and both scheduler resolver
addresses.

Regression certificates remain green for W34N70's timed marker, W34N71's
scripted sequence, and W34N60's mode-9 callbacks.  The normal port build
reports `LINK OK`.

## Runtime boundary and next target

Mode 14 remains outside the accepted natural base route.  This pair adds no
base-mode registration, so the detached W34N70 base-route smoke remains the
applicable live-route bound; natural execution belongs to the completed
mode-14 lifecycle.

Three mode-14-specific scheduler pairs remain unresolved:

- `0x8007AD34/0x8007ADD4`;
- `0x8007B200/0x8007B394`;
- `0x8007B604/0x8007B798`.

Restore the next bounded pair, then integrate and naturally accept
`0x8007A5DC/0x8007A8AC` only after the complete registered surface is owned.
