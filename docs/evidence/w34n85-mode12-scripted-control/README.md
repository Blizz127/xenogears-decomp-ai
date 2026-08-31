# W34N85 — mode-12 scripted-control callback pair

## Scope and retail anchor

This code-producing rung transcribes retail `[0x8007C36C,0x8007C724)`, the
smaller of the final two mode-12-specific scheduler pairs registered by setup
`0x8007BF50`.

Authority:

- `disc/world_map.bin`, mapped at `0x8006FAF0`;
- `scratchpad/w34n85_mode12_pair_c36c.objdump`;
- the 65-entry retail jump table at `0x8006FC50`, read directly from
  `world_map.bin`;
- exact 952-byte slice SHA-256:
  `ee52abd571a48b474dbbe975d341cd1eaf874bd51b2074a4996b9246977e2c26`;
- initializer `[0x8007C36C,0x8007C3B8)` SHA-256:
  `9e40451816d9e7c4b25accbe0e7bcd0deae0cdae344f1aaa6304b1d09b57b246`;
- update `[0x8007C3B8,0x8007C724)` SHA-256:
  `e282474ca27a3686359ae0cdfb5f4e31725eeffbd0525e95680543eea34fd010`.

`0x8007C36C` seeds script index 0, loads the first command and timer from
guest tables `0x8009A4D8/0x8009A4E8`, advances the index, and returns state 1.

The update's only active command values are 1, 2, 16, 17, 18, 19, 20, 22,
and 64. Command 1 decrements a signed timer and advances the two parallel
script tables only after the timer becomes negative. The other active cases
perform exact slot claims, banked sound calls, marker-20 publication, and
transition-global writes. Command 64 clears `D554/D7CC`, providing mode 12's
scripted natural exit. All other command values—including 21—are retail
no-ops.

## Production change

- Added `world_map_callback_7c36c.c/.h`.
- Registered `0x8007C36C/0x8007C3B8` in the bounded scheduler resolver.
- Added the source to the normal port build manifest.

No mode-12 lifecycle code or final callback pair changed.

## Focused certificate

Command:

```text
pc_port/tests/run_w34n85_mode12_scripted_control.sh
```

Result:

- O0: PASS
- O2: PASS
- nonrecovering UBSan: PASS
- strict warnings: clean
- M1 timer seeded from the command table: detected by `init.script_seed`
- M2 timer expires at zero: detected by `timer.zero_is_live`
- M3 command-2 claim value changed: detected by `command2.claim`
- M4 command-16 marker ID changed: detected by `command16.marker`
- M5 command-17 timer changed: detected by `command17.globals`
- M6 command-18 slot-7 claim omitted: detected by `command18.claims`
- M7 command-22 transition state changed: detected by
  `command22.globals_state`
- M8 command-64 exit writes omitted: detected by `command64.exit`

The certificate additionally proves script index and parallel-table
advancement; the signed zero/negative countdown boundary; the exact 2/16/17
sound IDs with the live bank halfword; marker-20 position; every claim and
ordering for commands 17–20 and 22; all `CCA4/D3CC` values; command-64 state;
default no-op behavior; return states; and scheduler resolution for both
guest addresses.

Normal port build: `LINK OK`.

Adjacent regression:

- W34N84 four-link pair: PASS in O0/O2/UBSan, M1–M7 detected.

## Runtime boundary and next target

Mode 12 remains outside the accepted natural routes. Exactly one
mode-12-specific callback pair remains unresolved:
`0x8007C724/0x8007C7D8`. Restore and certify it next; only then may lifecycle
integration begin.
