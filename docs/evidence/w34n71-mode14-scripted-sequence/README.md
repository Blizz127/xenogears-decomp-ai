# W34N71 — mode-14 scripted-sequence callback pair

## Scope and authority

This code-producing rung transcribes retail `[0x8007A9B4,0x8007AD34)`, the
mode-14 scheduler pair registered at `0x8007A7D0..0x8007A7E0`.  It does not
yet integrate the mode-14 lifecycle.

Sources:

- `disc/world_map.bin`, mapped at `0x8006FAF0`;
- `scratchpad/w34_render_scan_k/world_map_full.objdump.txt`;
- exact 896-byte slice SHA-256:
  `14ceb8b2058febe002a7b574fe3558cd2570be7b750d6e893b3247cde2551ee1`.

The retail jump table at `0x8006FBC4` maps states 0–11 to the twelve decoded
arms.  Static sequence tables are:

```text
state @ 0x8009A450 = 1,2,3,8,4,5,6,8,9,7,10,11
timer @ 0x8009A46C = 30,160,8,30,20,60,8,30,120,76,72,1
```

The initializer zeros `slot+0x50` and seeds state/timer from table element 0.
State 1 decrements the unsigned timer and advances only after the result is
negative as signed 16-bit; zero remains live for one iteration.  The other
arms publish markers 15–17, claim scheduler slots through `wm_80097770`,
issue packed sound IDs 1 and 4–12 through the retail-selected sound entry,
set transition word `D3CC=4`, and finally clear `D554` and `D7CC`.

## Production change

- Added `world_map_callback_7a9b4.c/.h`.
- Registered `0x8007A9B4/0x8007A9F8` in the bounded scheduler resolver.
- Added the source to the normal port build manifest.

No raw guest function pointer is called.  Table reads use the executable
static-data copy in guest RAM established by W34C2.

## Certificate

Command:

```text
pc_port/tests/run_w34n71_mode14_scripted_sequence.sh
```

Result:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- M1 wrong initial timer table: detected by `init.tables`
- M2 advance at timer zero: detected by `timer.zero_is_live`
- M3 pre-increment sequence index: detected by `timer.table_transition`
- M4 wrong state-3 marker: detected by `state3.marker15`
- M5 wrong state-6 sound function: detected by `state6.sound11_function`
- M6 missing terminal `D554` clear: detected by `state11.terminal_clear`

The certificate additionally checks both scheduler addresses, every claim
family, ordered marker/sound calls, the `D3CC` transition, and the paired
terminal clears.  W34N70's mode-14 timed-marker certificate remains green.

Normal port build: `LINK OK`.

## Runtime boundary

Mode 14 remains outside all accepted natural routes.  W34N70 established the
post-resolver base-route smoke result immediately before this rung; W34N71
adds no base-mode registration.  Natural runtime acceptance belongs to the
complete mode-14 lifecycle, after all seven mode-specific scheduler pairs are
linked.

## Next exact target

Restore the next smallest unresolved mode-14 registered callback pair, then
integrate `0x8007A5DC/0x8007A8AC` only after its complete scheduler surface is
owned.
