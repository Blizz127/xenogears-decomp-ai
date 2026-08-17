# W34B24-I15 — wm_80096F18 Implementation Report (91C18 transform helper)

Ladder after G5 914D0. Base: canonical
`1bba6e32727e44ae51b1122110021215a96f538c`. Live frontier is missing
slot-10 cb1 `0x80091C18`, whose unresolved overlay callees are
`0x80091FF8` and this leaf.

## Identity

| Field | Value |
|---|---|
| Function | `wm_80096F18` (heading/pose transform) |
| Boundary | `[0x80096F18, 0x80097070)` = 344 B / 86 insns |
| SHA-256 | `16dc5d45e6c50c5e301b1a0462e4cf875b23c2490e85fea3a5bd7bb4f7380aa0` |
| File off | `0x27428` |
| Source | `pc_port/src/world_map_helper_96f18.{c,h}` |
| Test | `pc_port/tests/w34b24_i15_96f18_prod_test.c` + `run_w34b24_i15_96f18.sh` |

ABI: `$a0` dest, `$a1` pose (only `+4` is `lw`/`sra 12`), `$a2` signed
scale (`sra 12` then `negu` → ApplyMatrixLV Z), `$a3` angles SVECTOR.
Void. Scratch `0x1F8000A0` / `0x1F8000F0` / `0x1F800000` / `0x1F800010`.

PsyQ only: `RotMatrixYXZ` ×2, `ApplyMatrixLV`, `ApplyMatrix`
(`0x8004A92C` / `0x8004947C` / `0x80049CEC`). MISSING_CALLEES=0.
No JALR / COP2 / GPU / OT. dest+2 uses `lhu dest+0xA` + out.Y then `sh`.

## Certification

- O0 / O2 / UBSan-nonrecovering: PASS, stdout identical (clang UBSan
  fallback; host gcc lacks `libubsan.so.1.0.0`).
- Mutants: **15/15 KILLED**.
- Sibling I9–I14 focused oracles: PASS.
- Suites: 28 PASS / 2 HOST / 0 FAIL. HOST =
  `run_w34b_r4world_73b04.sh`, `run_w34cb1_90a84.sh` (`-fpermissive`
  is C++-only under `-Werror`) — not a 96F18 regression.
- Build: LINK OK ×2; exactly one strong `T wm_80096F18` @
  `00000000004b1256`; no stub shadow; stubs **240/524 unchanged**.
- Fresh clean-export at `/tmp/w34b24-i15-clean-export`: PASS (15/15),
  `world_map.bin` copied as a regular file (180422).
- This helper is not a scheduler callback. The live frontier remains
  `0x80091C18`. After this leaf, `0x80091FF8` is MISSING=0
  (`96F18`, `93354`, `93660`).

`0x80071A58` was not implemented (not on the live path).
