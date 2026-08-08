# W34B5-O validation results

## Focused helper

- `-O0`: 228/228
- `-O2`: 228/228
- UBSan (`-fsanitize=undefined -fno-sanitize-recover=undefined`): 228/228,
  zero diagnostics
- Strict production warning audit (`-Wall -Wextra -Wconversion
  -Wsign-conversion`): zero warnings attributable to the helper
- Defined swapped-winding mutant: detected; flat normal changes from
  `(0,-4096,0)` to `(0,+4096,0)`
- Returning-path memory parity: exact 12-byte output; scratch writes only
  `0x00..0x0B`, `0x10..0x1B`, and `0x20..0x2B`; canaries and inputs preserved

## Canonical build and symbols

- `bash pc_port/build_port.sh`: `LINK OK` twice
- Linked symbols: exactly one `T wm_80093740` and one distinct unchanged
  `T func_80093740`
- Generated stubs: zero references to `wm_80093740`
- Higher world helpers `wm_80093978` and `wm_8008A2C8`: still absent
- Field source SHA-256 remains
  `f71542b74c6935450cbe3cdfde96047bbeb9fed0776eeb688fbe0aebd7863b0c`

## Fresh regression matrix

- `wm_800935DC`: 116/116
- `wm_80093660`: 208/208
- `wm_800923A8`: 72/72
- scheduler: 230/230
- P5: 71/71 + 48/48
- P4: 89/89 + 48/48
- P3: 68/68 + 45/45
- P2: 55/55 + 40/40
- P1: 41/41
- P0: 22/22
- `wm_80089160`: 76/76
- unaligned: 43/43
- selector harness: 90/90, including exhaustive valid-seed matrix
  65,535/65,535

All listed commands exited successfully. The canonical builds emitted only
pre-existing external PsyCross warnings, not warnings from the new helper.
