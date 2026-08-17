# W34B24-I29 — wm_800740B8 Implementation Report (71A58 horizon submitter)

Ladder after I28 `0x80089C78`. Next bounded missing 71A58 callee
with overlay `MISSING_CALLEES=0` (SLUS/PsyQ + COP2 only).

## Identity

| Field | Value |
|---|---|
| Function | `wm_800740B8` (RotMatrix + RTPT POLY_G3 + bit-sprite OT) |
| Boundary | `[0x800740B8, 0x80074594)` = 1244 B / 311 insns |
| SHA-256 | `a0b5afc3d5b4efc5970d5e730d4b466eaa6d5f94705830732bda346c2d3119ad` |
| File off | `0x45C8` |
| Source | `pc_port/src/world_map_helper_740b8.{c,h}` |
| Test | `pc_port/tests/w34b24_i29_740b8_prod_test.c` + `run_w34b24_i29_740b8.sh` |

PRE4's `[0x800740B8, 0x80074794)` (1756 B / 439) was wrong: it
swallowed two later prologues. Previous body ends `jr $ra; nop` at
`0x800740B0`. This body restores frame `0x38` and `jr $ra; nop` at
`0x8007458C`. `0x80074594` is the W17B HeapAlloc FT4-pool init
(jal from `0x80072480`). `0x8007474C` is the matching HeapFree.
Neither sibling is a 71A58 callee.

ABI: void. `71A58` jal delay is `nop`; `$a0` unused. Overlay JALs:
none. PsyQ: RotMatrix `0x8003F738`. COP2: CTC2 R+T from
`0x1F8000F0`, RTPT `0x4A280030`, swc2 SXY. GPU/OT: insert at
`*(db+0x70)` with masks `0xFF000000` / `0x00FFFFFF`.

## Certification

- Independent retail slice SHA checked by the runner.
- O0 / O2 / UBSan-nonrecovering: PASS, stdout identical (clang UBSan).
- Mutants: **19/19 KILLED**.
- Sibling I13 / I21 / I22 / I23 / I24 / I25 / I26 / I27 / I28 focused
  oracles: PASS.
- Suites: I9–I12, I14, I16–I18 PASS.
- Build: LINK OK ×2; `compiled=47 skipped=0`; exactly one strong
  `T wm_800740B8` @ `00000000004b6561`; no stub shadow; stubs
  **239/524 unchanged**. Remaining assumed: `wm_80097DC0`.
- Fresh clean-export at `/tmp/w34b24-i29-clean-export`: PASS (19/19),
  `world_map.bin` copied as a regular file (180422).
- This helper is not a scheduler callback. The live frontier remains
  `0x80071A58`. After this leaf, `71A58_MISSING_CALLEES=8`.

`0x80071A58` was not implemented (MISSING_CALLEES≠0).
`0x80086798` remains unimplemented; overlay store sites to
`0x8009CD40` were measured (see PRE4) but the jalr is still not
treated as a closed implementable set.
