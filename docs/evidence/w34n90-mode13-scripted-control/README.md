# W34N90 — mode-13 scripted control

## Scope and retail anchor

- Starting HEAD: `a8443ccac7157052ce1bbc459bf7c2f8a6d3f708`
- Branch: `experiment/worldmap-open-gates-20260823`
- Retail image: `disc/world_map.bin`, loaded at `0x8006FAF0`
- Retail slice: `[0x8008032C,0x80080578)`, 588 bytes
- SHA-256: `382507d7e94b9c25a128a1fadfefefadcd15ff2e2c8a88729fb272942a1157c7`
- Disassembly source: `scratchpad/w34_render_scan_k/world_map_full.objdump.txt`

This rung restores the next smallest unowned pair registered by mode-13 setup.
It does not integrate the lifecycle or alter the W34N89 marker pair.

## Production transcription

`wm_8008032C` seeds the mode-13 command interpreter from command table
`0x8009A698` and timer table `0x8009A6AC`. Retail deliberately leaves the
table index at zero after initialization, so the first timer expiry reloads
entry zero before advancing to entry one.

`wm_80080370` implements the exact signed command dispatch:

- command 1 decrements the signed 16-bit timer and advances only after it
  becomes negative;
- commands 2-7 publish the retail scheduler claims and mode-state values;
- command 8 plays SEDS-bank sound ids 22, 23, and 24;
- command 64 clears `D554/D7CC` and leaves command state zero;
- unsupported commands remain unchanged.

The scheduler resolves both guest addresses through linked native bodies; it
never calls a guest value as a host function pointer.

## Focused certificate

Command:

```text
pc_port/tests/run_w34n90_mode13_scripted_control.sh
```

Result:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- initialization, entry-zero replay, signed timer boundary: PASS
- command claims/state, SEDS ids, terminal exit semantics: PASS
- scheduler init/update resolution with zero missing/invalid hits: PASS
- M1 wrong initial timer table: `init.script_seed`
- M2 prematurely advanced initial index: `init.script_seed`
- M3 expired timer at zero: `timer.zero_is_live`
- M4 wrong command-2 claim: `command2.claim`
- M5 omitted command-5 second claim: `command5.claims_state`
- M6 wrong command-7 global state: `command7.state`
- M7 wrong sound range: `command8.sounds`
- M8 omitted command-64 exit: `command64.exit`

All M1-M8 were detected by their named assertions.

Adjacent W34N89 mode-13 marker certificate: PASS at
O0/O2/nonrecovering UBSan with M1-M6 detected.

Normal isolated port build: `LINK OK`. An earlier shared-tree build also
linked, but this result is explicitly based on the isolated rerun after the
other builder exited.

## Acceptance boundary

Two of four mode-specific callback pairs registered by `wm_8007FF70` are now
owned. Natural mode-13 acceptance remains correctly deferred until the pairs
`0x80080578/0x80080600` and `0x80080A28/0x80080AC4` are restored and the
lifecycle can dispatch without a missing callback boundary.
