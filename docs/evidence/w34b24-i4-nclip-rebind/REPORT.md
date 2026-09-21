# W34B24-I4 — NormalClip Rebinding for wm_80094A5C's Probe (func_8004A70C)

## Problem

Canonical `world_map_func_94a5c.c` declared `extern func_8004A70C`, which the
port auto-stubbed to return 0 — so 94A5C's complex dispatch cases (5/6/9/10)
always took the `v == 0` probe route in native runs. Live fidelity gap found
by the W34B24-PRE2 84D00/84DB8 audit.

## Identity proof (retail)

`0x8004A70C` in SLUS_006.64 (load base 0x80010000, header 0x800) is PsyQ
**NormalClip**, named at that address in `config/symbol_addrs.slus_006.64.txt`
and disassembled in this rung:

```
8004a70c  mtc2 $a0, SXY0(r12)
8004a710  mtc2 $a2, SXY2(r14)
8004a714  mtc2 $a1, SXY1(r13)
8004a720  NCLIP (0x4b400006)
8004a724  mfc2 $v0, MAC0(r24)
8004a728  jr   $ra
```

Register mapping a0→SXY0, a1→SXY1, a2→SXY2 (write ORDER is 0/2/1 but the
assignment is standard). `$a3` is never read. The port's
`src/slus_006.64/psyq/libgte.c` `NormalClip` uses `gte_ldsxy3(sxy0,sxy1,sxy2)`
whose PsyCross macro maps r0→12, r1→13, r2→14 — the identical assignment, so
the NCLIP antisymmetry (sign) is preserved.

## Native semantics check

PsyCross build has `USE_PGXP` undefined (only `USE_EXTENDED_PRIM_POINTERS=0`),
so NCLIP compiles the integer path:
`C2_MAC0 = int(F((s64)(SX0*SY1 + SX1*SY2 + SX2*SY0 − SX0*SY2 − SX1*SY0 − SX2*SY1)))`
with `F()` returning the value unchanged (flags only) — the s64 cross-sum is
**truncated to the low 32 bits (wrap, not saturate)**, matching hardware MAC0.

## Change

`pc_port/src/world_map_func_94a5c.c`: replaced the `func_8004A70C` extern
with `extern long NormalClip(long,long,long)` plus a 4-arg adapter
`wm_94a5c_normal_clip_probe(a0,a1,a2,a3)` (`a3` explicitly dead) used as the
default probe in both the production and TEST_HOOK builds. The probe seam's
4-arg shape is unchanged, so the accepted certificate seam still works.

`pc_port/tests/w34b22_i5_94a5c_integration_test.c`: the focused build links
without PsyCross, so it now provides a reference `NormalClip` implementing
the retail s64-truncated cross-sum; every test case installs `probe_mock`,
so the body is also a canary that aborts if the default probe were ever
reached in the certificate.

## Certification

- `run_w34b22_i5_94a5c.sh`: PASS (O0/O2/UBSan identical, 5/5 mutants killed).
- Neighbor suites re-run on this tree: `run_w34b22_i4b_private_family.sh`,
  `run_w34b23_951a8_chain.sh`, `run_w34b24_i1_952b0_95324.sh` — all PASS.
- Build: clean + incremental `LINK OK`; **function stubs 241 → 240**
  (`func_8004A70C` stub eliminated; "not found in ELF" warnings 3 → 2);
  data stubs 524 unchanged.
- Hardened `w18b_natural.gdb`: verdict=PASS exit=0, 13× ZERO_VERIFIED.
- Frontier measurement: unchanged (`wm_8008A72C`, slot 1, frame 916) — the
  natural route does not reach 94A5C's probe yet; the rebinding matters when
  the 8A72C→95414 chain goes live.

## Base

Candidate branch `candidate/w34b24-i4-nclip-rebind` on canonical `6c62a74`.
