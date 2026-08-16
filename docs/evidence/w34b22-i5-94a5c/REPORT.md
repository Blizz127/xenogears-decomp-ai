# W34B22-I5 — wm_80094A5C Implementation Report

## Identity

| Field | Value |
|---|---|
| **Function** | `wm_80094A5C` |
| **VA range** | `[0x80094A5C, 0x800951A8)` |
| **Size** | `0x800951A8 - 0x80094A5C = 0x74C = 1868 bytes = 467 instructions` |
| **SHA-256** | `de0dd1579711cc565909438971d6d286a77b171287c62555106cf42f23fffc37` |
| **File offset** | `0x24f6c` in `disc/world_map.bin` |
| **Overlay load base** | `0x8006FAF0` |
| **Source** | `pc_port/src/world_map_func_94a5c.c` / `.h` |
| **Test** | `pc_port/tests/w34b22_i5_94a5c_integration_test.c` (+ `run_w34b22_i5_94a5c.sh`) |

## Boundary Proof

Function boundary verified from `scratchpad/world-callgraph-v2/FUNCTIONS.csv`:
- Start: `0x80094A5C`
- End: `0x800951A8` (next function `wm_800948D8`? no — next symbol `wm_800951A8` boundary)
- Size: `0x800951A8 - 0x80094A5C = 0x74C = 1868 bytes = 467 instructions`

SHA-256 verified against the raw bytes at file offset `0x24f6c` in `disc/world_map.bin`
(`0x80094A5C - 0x8006FAF0 = 0x24f6c`), confirming the exact range.

## ABI

```
Signature: s32 wm_80094A5C(u32 base_vec, u32 direction_vec, s32 scale, s32 mode)
Arg $a0:  base_vec     (u32*)  3 x s32 [baseX, ?, baseZ]
Arg $a1:  direction_vec (u32*) 3 x s32 [dirX, ?, dirZ]
Arg $a2:  scale         (s32)   fixed-point scale factor
Arg $a3:  mode          (s32)   only low 16 bits (sign16) used
Return $v0: s32  (helper result, 0, or 1 on success; 0 also = tail blocked)
Saved regs: $s0,$s1,$s2,$s3,$s4,$ra  (0x28 stack frame)
Workspace: scratchpad $s1 = 0x1F800000
Clobbers: $v0,$v1,$a0,$a1,$a2,$a3,$t0,$t1,$at
```

## Prologue (0x80094A5C – 0x80094B70)

- `newX = baseX + ((dirX * scale) >> 12)` (arithmetic) ; stored to `s1[0x10]`
- `newZ = baseZ + ((dirZ * scale) >> 12)` (arithmetic) ; stored to `s1[0x18]`
- `s1[0xa0] = (s16)(baseX >> 19)` (sign-extended half)
- `s1[0xa4] = (s16)(baseZ >> 19)`
- `s1[0xa8] = (s16)(newX >> 19)`
- `s1[0xac] = (s16)(newZ >> 19)`
- `t0 = 0`
  - if `newXhi < baseXhi`: `t0 |= 2`; else if `baseXhi < newXhi`: `t0 |= 1`
  - if `newZhi < baseZhi`: `t0 |= 8`; else if `baseZhi < newZhi`: `t0 |= 4`
- if `t0 >= 11`: go to tail with `a2 = 0` (no helper run)

## Dispatcher (0x80094B74 – 0x80094B88)

Computed jump through table at `0x80070C50` indexed by `t0` (the X/Z cell move code):

| t0 | Handler | Behaviour |
|----|---------|-----------|
| 0,3,7 | `0x80095124` (tail) | no helper; tail only |
| 1 | `0x80094B90` | single helper `wm_8009443C` |
| 2 | `0x80095104` | single helper `wm_800945C8` |
| 4 | `0x80094BB0` | single helper `wm_80094750` |
| 8 | `0x80094BD0` | single helper `wm_800948D8` |
| 5 | `0x80094BF0` | complex; A=`9443C`, B=`94750` |
| 6 | `0x80094D4C` | complex; A=`945C8`, B=`94750` |
| 9 | `0x80094E84` | complex; A=`9443C`, B=`948D8` |
| 10 | `0x80094FD8` | complex; A=`945C8`, B=`948D8` |

Single-helper: `r = helper(base,dir,WS,sign16(mode)); if (r != 0) return r; return tail(0,mode);`
If `t0 >= 11`: `return tail(0, mode)` directly (no helper).

## Complex cases (5/6/9/10)

1. Set `s1[0]`/`s1[8]` (the mask) per case:
   - Case5: `(b0 & 0xFFF80000)|0x80000`, `(b2 & 0xFFF80000)|0x80000`
   - Case6: `b0 & 0xFFF80000`, `(b2 & 0xFFF80000)|0x80000`
   - Case9: `(b0 & 0xFFF80000)|0x80000`, `b2 & 0xFFF80000`
   - Case10: `b0 & 0xFFF80000`, `b2 & 0xFFF80000`
2. Run identical delta block (writes `s1[0x20,0x28,0x30,0x38,0x40,0x48]`).
3. `v = func_8004A70C(s1[0x40], 0, s1[0x48], mode)`  (EV collision probe; auto-stubbed in port, returns 0)
4. Dispatch two helpers by sign of `v` (signed):

| case | v<0 | v==0 | v>0 | pattern |
|------|-----|------|-----|---------|
| 5 | A then B | A | B then A | P1 |
| 6 | B then A | A | A then B | P2 |
| 9 | B then A | A | A then B | P2 |
| 10 | A then B | A | B then A | P1 |

First helper's non-zero result is returned immediately; otherwise the second
helper's result is used; if both zero, `tail(0, mode)` runs.

## Tail (0x80095124)

```
if (a2 != 0) return a2;
wm_80093354(s1 + 0x10);
c  = wm_80093f18(s1 + 0x10);
r  = wm_80094060(sign16(mode), sign16(c));
if (r != 0) return 0;
copy s1[0x10..0x1c] -> s1[0..0xc];
return 1;
```

## Certification

- `./pc_port/build_port.sh` builds clean; `wm_80094A5C` present; `func_8004A70C` auto-stubbed.
- Integration test `pc_port/tests/w34b22_i5_94a5c_integration_test.c` drives all 11 `t0`
  codes × sub-cases (no-probe / probe 0 / probe + / probe −) and asserts helper
  selection, call order, early-return, mask values, and tail copy.
- Verified identical behaviour at `-O0`, `-O2`, and `-O2 -fsanitize=undefined`.
- 5 logic mutants are all KILLED (see `MUTANTS.csv`).
- The v-sign routing patterns P1/P2 were independently re-verified against the
  raw disassembly (case5/10 = P1, case6/9 = P2).
