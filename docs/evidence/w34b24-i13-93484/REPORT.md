# W34B24-I13 — wm_80093484 Implementation Report (914D0 wrap helper)

Ladder after G5 907F4. Base: canonical
`e73f9b798ae7bc3b4eb642fe270bd76983e0d993`. Live frontier is missing
slot-9 cb1 `0x800914D0`, whose only unresolved callee is this leaf.

## Identity

| Field | Value |
|---|---|
| Function | `wm_80093484` (fixed-threshold X/Z wrap) |
| Boundary | `[0x80093484, 0x80093534)` = 176 B / 44 insns |
| SHA-256 | `a2c73bba45bb0a315f117d89047e72de8fd3ee4283c48fdeb7f877422bc5300a` |
| File off | `0x23994` |
| Source | `pc_port/src/world_map_helper_93484.{c,h}` |
| Test | `pc_port/tests/w34b24_i13_93484_prod_test.c` + `run_w34b24_i13_93484.sh` |

Sibling of accepted `wm_80093354`. Same period words `0x8009D160` /
`0x8009D2B4` `<< 23`, but the compare window is the fixed signed pair
`0xFC000000` / `0x04000000` (not the period). Y is never accessed.
No callees. `914D0` JALs this twice.

## Certification

- O0 / O2 / UBSan-nonrecovering: PASS, stdout identical.
- Mutants: **12/12 KILLED**.
- Build: LINK OK ×2; exactly one strong `T wm_80093484` @
  `00000000004b0203`; no stub shadow; stubs **240/524 unchanged**.
- This helper is not a scheduler callback. The live frontier remains
  `0x800914D0` (now MISSING=0: 93354, 93484, 97770).

`0x80071A58` was not implemented (not on the live path).
