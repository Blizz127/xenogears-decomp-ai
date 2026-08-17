# W34B24-I7 — wm_80095414 (Keystone Path-Node Movement Router)

## Identity

| Field | Value |
|---|---|
| Boundary | `[0x80095414, 0x80095CD4)` — 2240 B / 560 insns (re-verified this lane) |
| Slice SHA-256 | `78ebf3efaa0c21a3649acb3f07228d7984fd236c270fe2d9a5c3b140fe04b6ed` |
| START_CANONICAL | `ea03c4c9253b1142032d30da125a7450dd2e9e24` (verified, unchanged) |
| Source | `pc_port/src/world_map_func_95414.{c,h}` |
| Test | `pc_port/tests/w34b24_i7_95414_prod_test.c` + `run_w34b24_i7_95414.sh` |
| Audit base | `docs/evidence/w34b24-pre-95414/` (canonical) |

## Signature

```
s32 wm_80095414(u32 pos_vec, u32 dir_vec, u32 out_vec, s32 scale,
                s32 mode /* raw 32-bit passthrough to 951A8 */)
```

All nine callees canonical: 93354x2, 93978x2, 951A8 (2 live sites),
85760, 84D00, 85158x2, 85418, 952B0x3, 95324.

## Audit correction (instructions win)

The pair-resolver tie-break (mask==3) computes
`v = (typeA==0 ? 1:0) | (typeB==0 ? 2:0)` — bit 1 set when **typeB == 0**
(`bnez typeB` at 0x80095A58/0x80095C08 SKIPS the `ori v1,2`).  The PRE
JUMP_TABLE.csv prose said "typeB != 0"; the certificate exercises both
arms (I5/I6/J sections) and a dedicated mutant covers the polarity.

## Dead code (documented, not transcribed)

Mode select is strictly `(D_8009C840 == -1) ? 1 : 3`; the v1==0 arm at
0x80095598 (fourth 951A8 site), the uninit-`$s5` returns, and the case-1
`s5 = -1` preset at 0x80095610 are retail-unreachable/dead.  v1==2 shares
the v1==3 branch target.

## Host adaptations (both provably invisible to valid retail behavior)

1. **Frame slot**: retail's halfword attr slot at `0x18($sp)` (address
   passed to wm_80084D00) is materialized at guest `0x801FFE18` inside
   the dead retail-stack region of emulated RAM (above heap ceiling
   0x801FC000; the port never emulates the MIPS stack).  Every retail
   `0x18($sp)` access maps with its exact width (`sh`/`lh`/`lhu`).
2. **Dispatch bound**: 4096-iteration cap that aborts loudly (accepted
   scheduler convention); valid retail executions always terminate
   through a terminal arm and never observe it.

## Certificate

- Retail full-file + slice SHA gates (`stat -L` so provisioned copies
  and symlinks both pass).
- Focused oracle, O0/O2/UBSan-nonrecovering, normalized stdout
  byte-identical; scripted recording seams for all nine callees with
  hard-fail script-overrun detection (loop mutants die deterministically).
- Coverage: entry projections vs s64-truncation reference incl. a
  negative low-word term; 93354/93978 call orders + arguments; case-1
  all three exits; proximity boundary exactly diff=10 accept / 11
  reject, both signs; flagged and -1 candidates skipped; candidate
  marking; high-bit attr (lhu args vs lh sign-extended cache store);
  mode passthrough 0x7ABC1234 verbatim; cache set orders (case-1:
  out4->C840->C16C; slot0: C16C->out4->C840) and reset order
  (C16C->C840); jump-table slots 0/1/2/4 all arms; slot3 masks 0-3 with
  tie-break v=0 zero-out (+8,+4,+0), v=1 follow+6, v=2 continue-B;
  slot5 tie-break follows +8 (distinct targets); slot6 mask1->0x40,
  mask2->0x50; class >=8 guard; slot7 re-dispatch.
- Mutants: **17/17 KILLED** (MUTANTS.csv).  Honest note: a
  dispatch-guard mutant (sltiu->slti / off-by-one) is structurally
  non-expressible in C — the switch statement inherently bounds the
  dispatch — recorded instead of padding the count.

## Integration

- Registered in PORT_SOURCES; clean + incremental `LINK OK`.
- Exactly one strong `T wm_80095414`; no stub shadow (symbol previously
  absent; stubs 240/524 unchanged).
- Suites: **22/22 PASS** (all accepted + this rung).
- Hardened `w18b_natural.gdb`: `verdict=PASS exit=0 zero_verified=13`.
- Frontier (natural route, deep gates, DISPLAY=:10): **unchanged —
  `wm_8008A72C` slot-1 MISSING CALLBACK** (expected: 95414 is only
  reached from 8A72C, which still has no body).

## Milestone impact

G1 (95414 dependency DAG) closed; G2 implementation complete pending
independent acceptance.  Once landed, wm_8008A72C's missing direct
dependencies are ZERO — the G3 fresh closure audit can proceed.
