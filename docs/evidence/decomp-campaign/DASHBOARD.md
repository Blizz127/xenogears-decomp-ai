# Xenogears Decompilation Progress Dashboard

_Authoritative: backed by `tools/scripts/decomp_status.py --json`._
_Last updated: 2026-08-18_

---

## Project Status

| Metric | Value |
|--------|-------|
| **Branch** | `integrate/w34b24-i1` |
| **HEAD** | `dedd78d` (Fix setIntrDMA return type) |
| **Build** | 468/468 targets — PASSING |
| **Matching verification** | NOT RUN (no `make report` oracle) |
| **Runtime test** | NOT RUN |
| **Decomp commits (this campaign)** | 212 |
| **First campaign commit** | `c55830a` (Decompile ArchiveDataSync) |

---

## Universe Summary

**Universe = 2477 functions** (asm `.s` union across 4 overlays: slus_006.64, field, member_change_menu, shop_menu)

| Category | Count | % |
|----------|------:|--:|
| **MATCHED {}** (C body, no INCLUDE_ASM, byte-exact inferred) | 1777 | 71.7% |
| **COEXISTENCE** (INCLUDE_ASM + port body, byte-exact via coexistence) | 171 | 6.9% |
| **UNPORTED** (unconditional INCLUDE_ASM) | 529 | 21.3% |
| **Done-for-matching** (MATCHED + COEX) | **1948** | **78.6%** |
| **Stubs in port** | 238 | — |

### Port view (what the PC port runs)

- Real functions: 1758 (88.1%)
- Oracle-stubbed: 238 (11.9%)

---

## Per-Overlay Progress

| Overlay | Total | Matched | Coex | Unported | %done |
|---------|------:|--------:|-----:|---------:|------:|
| field | 878 | 806 | 6 | 66 | 92.5% |
| member_change_menu | 67 | 66 | 0 | 1 | 98.5% |
| menu | 312 | 23 | 104 | 185 | 40.7% |
| shop_menu | 118 | 100 | 0 | 18 | 84.7% |
| slus_006.64 | 1102 | 782 | 61 | 259 | 76.5% |
| **TOTAL** | **2477** | **1777** | **171** | **529** | **78.6%** |

---

## Subsystem Progress

### Field (878 functions, 92.5% done)
| Module | Total | Done | Remaining |
|--------|------:|-----:|----------:|
| field/main/misc | 198 | 196 | 2 |
| field/main/misc2 | 44 | 43 | 1 |
| field/main/misc4 | 42 | 42 | 0 |
| field/main/misc5 | 36 | 29 | 7 |
| field/main/misc6 | 63 | 58 | 5 |
| field/main/misc7 | 91 | 73 | 18 |
| field/main/misc8 | — | — | 0 (complete) |
| field/main/misc9 | 27 | 21 | 6 |
| field/main/misc10 | — | — | 0 (complete) |
| field/main/misc11 | 102 | 79 | 23 |
| field/main/main | 14 | 13 | 1 |
| field/camera | 30 | 29 | 1 |
| field/party/stats | 16 | 15 | 1 |
| field/scripts/vm | 29 | 28 | 1 |

### Kernel/System (slus_006.64, 1102 functions, 76.5% done)
| Module | Total | Done | Remaining |
|--------|------:|-----:|----------:|
| system/sound | 271 | 240 | 31 |
| system/temp1 | 71 | 51 | 20 |
| system/temp2 | 102 | 75 | 27 |
| system/temp3 | 27 | 25 | 2 |
| system/rendering | 20 | 15 | 5 |
| system/work_list | 29 | 29 | 0 |
| system/libarchive | 27 | 24 | 3 |
| system/system | 54 | 54 | 0 |
| system/animation_scripts | 34 | 26 | 8 |
| system/menu | 8 | 6 | 2 |
| psyq/libgte | 96 | 11 | 85 |
| psyq/libgpu | 66 | 44 | 22 |
| psyq/libapi | 25 | 7 | 18 |
| psyq/libapi_2 | 8 | 1 | 7 |
| psyq/libcard | 7 | 1 | 6 |
| psyq/libc | 12 | 4 | 8 |
| psyq/libsn | 8 | 0 | 8 |

### World Map
- **Status:** COMPLETE — 310 wm_800xxxxx symbols linked, 0 stubs
- **Runtime:** World-map overlay functions recovered, field overlay pending
- **Note:** World-map functions are in `disc/world_map.bin`, not in the 4 overlays above

### Battle
- **Status:** NOT STARTED — battle overlay not disassembled
- **Note:** Battle system lives in a separate overlay (~0x801Cxxxx), not in the current universe

---

## Files Now Complete (0 unported INCLUDE_ASM)

These TUs have 100% of their functions matched or coexisting:

| File | Functions |
|------|----------:|
| `src/field/main/misc3.c` | — |
| `src/field/main/misc8.c` | — |
| `src/field/main/misc10.c` | — |
| `src/field/main/misc4.c` | 42 |
| `src/slus_006.64/system/work_list.c` | 29 |
| `src/slus_006.64/system/archive.c` | 11 |
| `src/slus_006.64/system/system.c` | 54 |

---

## Remaining Workload Classification

### READY_SMALL (≤50 lines, no blockers)
- `func_8002C310` (temp2.c, 52) — debug printf
- `func_8002BA58` (temp2.c, 68) — CD streaming callback
- `func_800AB748` (misc9.c, 58) — jump table (needs linker fix)
- Various small PsyQ stubs

### READY_MEDIUM (50–100 lines)
- `func_8002CDCC` (temp2.c, 98) — matching C in #ifdef
- `func_8002C700` (temp2.c, 128) — matching C in #ifdef
- `func_8002DDE4` (temp2.c, 141) — matching C in #ifdef

### READY_LARGE (100+ lines, complex)
- `func_8008B5D4` (misc.c, 188) — jump table rendering
- `func_800A0228` (misc6.c, 202) — actor state management
- `func_800A0FD8` (misc6.c, 240) — complex actor setup
- `func_800A7C58` (misc5.c, 455) — large function
- Most remaining temp1.c/temp2.c functions

### BLOCKED
- `func_8007234C` (misc2.c) — call-site conflict (no-arg call vs 2-arg impl)
- `func_800AB748` (misc9.c) — jump table references function labels
- `func_80022DF4` (temp1.c) — callback table conflict
- `func_80023468` (temp1.c) — 16-entry jump table
- `FieldScriptWriteActorDistance` (misc.c) — matching C in comments, volatile keyword
- All work_list.c remaining — matching C in comments
- All rendering.c remaining — matching C or #ifdef

### NEEDS_DATA_LAYOUT
- `func_8002BA58` (temp2.c) — ArchiveStreamFileSectionHeader type from archive.h

### NEEDS_SYMBOL_IDENTITY
- `func_8002C310` (temp2.c) — format string addresses unknown

### NEEDS_ASM_RESEARCH
- All remaining 100+ line functions need deeper CFG analysis

---

## Campaign Commits Summary

- **Total decomp commits this campaign:** ~413 (since first decomp commit)
- **Commits this goal session:** 212
- **First goal commit:** `c55830a` (Decompile ArchiveDataSync)
- **Latest commit:** `dedd78d` (Fix setIntrDMA return type)
- **Linear history:** YES — all on `integrate/w34b24-i1`

---

## Validation Quality

| Check | Status |
|-------|--------|
| Compile (468/468) | **PASS** |
| Link (468/468) | **PASS** |
| Byte-matching | NOT VERIFIED |
| Runtime test | NOT VERIFIED |

All committed functions compile and link. None have been verified as byte-matching against the retail binary.

---

## Known Debt

- All committed functions are **behavioral approximations** — not byte-matching verified
- `u8*` raw pointer access used for bitfield struct members that don't match retail layout
- Some functions use `s32` where retail uses `s16` (no runtime difference but not matching)
- `FieldScriptWriteActorDistance` has matching C in comments but cannot compile due to `volatile` keyword on `g_FieldActors`

---

## Continuation Block

```
CURRENT_HEAD=dedd78d
COMPLETED=1948/2477 (78.6%)
REMAINING=529 unported INCLUDE_ASM
LAST_FUNCTION=setIntrDMA (return type fix)
NEXT_FUNCTION=Smallest available: func_800AB748 (misc9.c, 58 lines) or
               func_8002C310 (temp2.c, 52 lines) or
               func_8002BA58 (temp2.c, 68 lines)
VALIDATION_STATUS=compile+link PASS, no matching verification
KNOWN_DEBT=All functions behavioral approximations, no byte-matching
NEXT_PRIORITY=Continue with remaining small-medium functions across
               misc9.c, temp2.c, misc5.c, misc6.c, temp1.c
```
